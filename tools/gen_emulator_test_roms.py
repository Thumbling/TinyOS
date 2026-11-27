#!/usr/bin/env python3
"""Generate deterministic, legally self-built emulator test ROMs.

The files are written into the build directory only. They are not part of the
repository and contain no commercial game code or BIOS. The GB logo bytes are
the fixed cartridge-header fingerprint required by mGBA's ROM detector.
"""
from __future__ import annotations

import sys
from pathlib import Path

GB_LOGO = bytes.fromhex(
    "ceed6666cc0d000b03730083000c000d0008111f8889000e"
    "dccc6ee6ddddd999bbbb67636e0eecccdddc999fbbb9333e"
)
RAM_PATTERN = bytes([0x5A, 0xA5, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66])


def gb_header(rom: bytearray, title: str, cgb: bool) -> None:
    rom[0x100:0x104] = bytes([0x00, 0xC3, 0x50, 0x01])  # nop; jp 0x0150
    rom[0x104:0x134] = GB_LOGO
    rom[0x134:0x143] = title.encode("ascii").ljust(15, b"\0")[:15]
    rom[0x143] = 0x80 if cgb else 0x00
    rom[0x147] = 0x03  # MBC1 + RAM + battery
    rom[0x148] = 0x00  # 32 KiB
    rom[0x149] = 0x02  # 8 KiB RAM
    checksum = 0
    for byte in rom[0x134:0x14D]:
        checksum = (checksum - byte - 1) & 0xFF
    rom[0x14D] = checksum
    rom[0x14E:0x150] = b"\0\0"


class SM83Writer:
    def __init__(self) -> None:
        self.code = bytearray()
        self.labels: dict[str, int] = {}

    def raw(self, *values: int) -> None:
        self.code.extend(values)

    def label(self, name: str) -> None:
        self.labels[name] = len(self.code)

    def jr(self, label: str) -> None:
        target = self.labels[label]
        offset = target - (len(self.code) + 2)
        if not -128 <= offset <= 127:
            raise ValueError(f"JR to {label} out of range: {offset}")
        self.raw(0x20, offset & 0xFF)

    def ld_a(self, value: int) -> None:
        self.raw(0x3E, value & 0xFF)

    def ldh(self, register: int) -> None:
        self.raw(0xE0, register & 0xFF)

    def ld_hl(self, address: int) -> None:
        self.raw(0x21, address & 0xFF, (address >> 8) & 0xFF)

    def ld_hl_a(self) -> None:
        self.raw(0x22)


def gb_program(cgb: bool) -> bytes:
    w = SM83Writer()
    w.raw(0xF3)  # di
    w.raw(0x31, 0xFE, 0xFF)  # ld sp,$fffe
    w.ld_a(0)
    w.ldh(0x40)  # LCD off

    if cgb:
        # Palette 0: red, green, blue, white. The values are the same 15-bit
        # colours as the GBA test so the RGB565 assertion can be shared.
        w.ld_a(0x80)
        w.ldh(0x68)  # BCPS: palette 0, auto increment
        for value in (0x1F, 0x00, 0xE0, 0x03, 0x00, 0x7C, 0xFF, 0x7F):
            w.ld_a(value)
            w.ldh(0x69)
    else:
        w.ld_a(0xE4)  # identity mapping: BGP 00 01 10 11
        w.ldh(0x47)

    # Enable external RAM and write a deterministic battery-save pattern.
    w.ld_a(0x0A)
    w.raw(0xEA, 0x00, 0x00)  # ld ($0000),a
    w.ld_hl(0xA000)
    for value in RAM_PATTERN:
        w.ld_a(value)
        w.ld_hl_a()

    # Clear the entire 32x32 background map. This also removes the HLE boot
    # logo so a final frame is deterministic on DMG and CGB.
    w.ld_hl(0x9800)
    w.raw(0x06, 0x08)  # ld b,8: both 0x9800 and 0x9C00 maps
    w.ld_a(0x00)
    w.label("outer")
    w.raw(0x0E, 0x00)  # ld c,0 = 256 iterations
    w.label("inner")
    w.ld_hl_a()
    w.raw(0x0D)  # dec c
    w.jr("inner")
    w.raw(0x05)  # dec b
    w.jr("outer")  # JR NZ: use the same relative helper

    # Tile 0 has four two-pixel bands: index 0, 1, 2, 3, then repeat.
    w.ld_hl(0x8000)
    for value in (0x00, 0x00, 0x00, 0x00,
                  0xFF, 0x00, 0xFF, 0x00,
                  0x00, 0xFF, 0x00, 0xFF,
                  0xFF, 0xFF, 0xFF, 0xFF):
        w.ld_a(value)
        w.ld_hl_a()

    # Clear OAM as well; the HLE boot animation can leave sprite entries.
    w.ld_hl(0xFE00)
    w.raw(0x06, 0xA0)
    w.ld_a(0x00)
    w.label("oam")
    w.ld_hl_a()
    w.raw(0x05)
    w.jr("oam")

    w.ld_a(0x91)  # LCD on, BG on, tile data at $8000, map at $9800
    w.ldh(0x40)
    w.raw(0x18, 0xFE)  # jr -2
    return bytes(w.code)


def make_gb(cgb: bool) -> bytes:
    rom = bytearray(32768)
    gb_header(rom, "TINYOS SELFTEST", cgb)
    code = gb_program(cgb)
    rom[0x150:0x150 + len(code)] = code
    return bytes(rom)


def make_gba() -> bytes:
    # Assembled from the tiny source shown in tests/emulator_selftest.c:
    # mode 3, four known pixels, one SRAM byte to trigger type detection,
    # then a 32-bit SRAM pattern and a self-loop.
    code = bytes.fromhex(
        "0103a0e3"  # mov r0,#0x04000000
        "40109fe5"  # ldr r1,[pc,#0x40]
        "b010c0e1"  # strh r1,[r0]
        "0624a0e3"  # mov r2,#0x06000000
        "38309fe5"  # ldr r3,[pc,#0x38]
        "003082e5"  # str r3,[r2]
        "34309fe5"  # ldr r3,[pc,#0x34]
        "043082e5"  # str r3,[r2,#4]
        "30309fe5"  # ldr r3,[pc,#0x30]
        "083082e5"  # str r3,[r2,#8]
        "2c309fe5"  # ldr r3,[pc,#0x2c]
        "0c3082e5"  # str r3,[r2,#12]
        "0e44a0e3"  # mov r4,#0x0e000000
        "24509fe5"  # ldr r5,[pc,#0x24]
        "0050c4e5"  # strb r5,[r4]
        "005084e5"  # str r5,[r4]
        "1c509fe5"  # ldr r5,[pc,#0x1c]
        "045084e5"  # str r5,[r4,#4]
        "feffffea"  # b .
        "03040000"  # literal 0x00000403
        "1f001f00"  # red/red
        "e003e003"  # green/green
        "007c007c"  # blue/blue
        "ff7fff7f"  # white/white
        "44332211"  # first SRAM word
        "88776655"  # second SRAM word
    )
    rom = bytearray(32768)
    rom[0x00:0x04] = (0xEA00002E).to_bytes(4, "little")  # b 0x080000c0
    rom[0xA0:0xB0] = b"MINIMAL" + b"\0" * 5 + b"TEST"
    rom[0xC0:0xC0 + len(code)] = code
    return bytes(rom)


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: gen_emulator_test_roms.py OUTPUT_DIR", file=sys.stderr)
        return 2
    root = Path(sys.argv[1])
    for system, suffix, cgb in (("gb", ".gb", False), ("gbc", ".gbc", True)):
        directory = root / system
        directory.mkdir(parents=True, exist_ok=True)
        (directory / ("tinyos-selftest" + suffix)).write_bytes(make_gb(cgb))
    directory = root / "gba"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "tinyos-selftest.gba").write_bytes(make_gba())
    print(f"emulator test ROMs written to {root}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

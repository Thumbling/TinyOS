#!/usr/bin/env python3
"""Convert a panel-mipi-dbi text sequence to the driver's binary format.

Usage: mipi_dbi_cmd.py input.txt output.bin

The format is the one documented by the panel-mipi-dbi project:
15-byte magic "MIPI DBI" followed by seven NUL bytes, one version byte,
then repeated (command, parameter count, parameters...) records. Command
0x00 with one parameter is a millisecond delay.
"""

import sys


def parse_line(line: str) -> bytes:
    line = line.split("#", 1)[0].strip()
    if not line:
        return b""

    parts = line.split()
    if parts[0] == "delay":
        if len(parts) != 2:
            raise ValueError(f"bad delay line: {line!r}")
        delay = int(parts[1], 0)
        if not 0 <= delay <= 255:
            raise ValueError("delay must fit in one byte")
        return bytes((0x00, 0x01, delay))

    if parts[0] != "command" or len(parts) < 2:
        raise ValueError(f"bad command line: {line!r}")

    command = int(parts[1], 0)
    parameters = [int(value, 0) for value in parts[2:]]
    if not 0 <= command <= 255:
        raise ValueError("command must fit in one byte")
    if len(parameters) > 255:
        raise ValueError("too many parameters")
    if any(not 0 <= value <= 255 for value in parameters):
        raise ValueError("parameter out of range")

    return bytes((command, len(parameters), *parameters))


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__, file=sys.stderr)
        return 2

    source, destination = sys.argv[1], sys.argv[2]
    records = bytearray(b"MIPI DBI" + b"\x00" * 7 + b"\x01")
    with open(source, "r", encoding="utf-8") as text:
        for line_number, line in enumerate(text, 1):
            try:
                records.extend(parse_line(line))
            except ValueError as error:
                print(f"{source}:{line_number}: {error}", file=sys.stderr)
                return 1

    with open(destination, "wb") as binary:
        binary.write(records)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

# TinyOS host tools.
#
#   make sim HOSTCC="/path/to/zig cc" AR="/path/to/zig ar"
#   make shots HOSTCC="/path/to/zig cc" AR="/path/to/zig ar"
#
# The firmware itself is built by Buildroot in CI (see .github/workflows).

HOSTCC ?= cc
AR ?= ar
ZIG ?= zig
HOST_CFLAGS ?= -std=gnu11 -O2 -g -Wall -Wextra -Wno-unused-parameter
LVGL_DIR := third_party/lvgl
LVGL_HOST_DIR := build/lvgl-host
LVGL_HOST_LIB := $(LVGL_HOST_DIR)/liblvgl.a

MGBA_DIR := third_party/mgba
MGBA_BUILD := build/mgba-host
MGBA_LIB := $(MGBA_BUILD)/libmgba.a
include $(MGBA_DIR)/sources.mk

ifeq ($(OS),Windows_NT)
MGBA_OS_SOURCES := $(MGBA_WINDOWS_SOURCES)
MGBA_OS_CFLAGS := -D_WIN32_WINNT=0x0600
SIM_PLATFORM_LIBS := -lws2_32 -lshlwapi -lole32
else
MGBA_OS_SOURCES := $(MGBA_POSIX_SOURCES)
MGBA_OS_CFLAGS := -D_GNU_SOURCE -DHAVE_LOCALE -DHAVE_USELOCALE
SIM_PLATFORM_LIBS := -lpthread
endif

MGBA_SOURCES := $(MGBA_CORE_SOURCES) $(MGBA_OS_SOURCES)
MGBA_OBJECTS := $(patsubst %.c,$(MGBA_BUILD)/%.o,$(MGBA_SOURCES))
MGBA_CFLAGS := -std=gnu11 -O2 -w -DBUILD_STATIC -DM_CORE_GBA -DM_CORE_GB \
	-DHAVE_STRDUP -DHAVE_VASPRINTF -DHAVE_SETLOCALE \
	-I$(MGBA_DIR)/include -I$(MGBA_DIR)/src $(MGBA_OS_CFLAGS)

SIM_SOURCES := sim/main.c sim/png.c sim/layout_audit.c src/app.c \
	$(wildcard src/core/*.c) $(wildcard src/emulator/*.c) $(wildcard src/ui/*.c) \
	$(wildcard src/ui/fonts/*.c) $(wildcard src/modules/*.c) \
	$(wildcard src/platform/host/*.c)
SIM_OBJECTS := $(patsubst %.c,build/%.o,$(SIM_SOURCES))
SIM_DEPS := $(SIM_OBJECTS:.o=.d)
SIM_HEADERS := $(shell find src sim config -name '*.h' 2>/dev/null)

# Some host compilers do not emit usable -MMD output on Windows. Treat the
# project headers as inputs explicitly so an edit to layout.h can never leave
# stale objects behind.
$(SIM_OBJECTS): $(SIM_HEADERS)

SIM_CFLAGS := $(HOST_CFLAGS) -DTINYOS_SIM \
	-DTINYOS_SETTINGS_PATH='"build/tinyos-settings.ini"' \
	-DTINYOS_ROMS_ROOT='"build/roms"' \
	-DLV_CONF_INCLUDE_SIMPLE -Iconfig -Isrc -Isim -Ithird_party \
	-I$(MGBA_DIR)/include -I$(MGBA_DIR)/src -I$(LVGL_DIR)/include

# Headless emulator proof. This deliberately links only the session, ROM,
# input, logging and raw-panel code plus mGBA; it does not need LVGL, ALSA or a
# display device.
EMU_SELFTEST_BUILD := build/emulator-selftest
EMU_SELFTEST_BIN := $(EMU_SELFTEST_BUILD)/emulator-selftest
EMU_SELFTEST_ROM_DIR := $(EMU_SELFTEST_BUILD)/roms
EMU_SELFTEST_SOURCES := tests/emulator_selftest.c src/emulator/session.c \
	src/emulator/rom.c src/emulator/input_map.c src/core/log.c \
	src/platform/linux/fbdev_blit.c
EMU_SELFTEST_OBJECTS := $(patsubst %.c,$(EMU_SELFTEST_BUILD)/obj/%.o,$(EMU_SELFTEST_SOURCES))
EMU_SELFTEST_CFLAGS := $(HOST_CFLAGS) -Isrc -Ithird_party \
	-I$(MGBA_DIR)/include -I$(MGBA_DIR)/src

.PHONY: all sim shots layout-audit emulator-selftest arm-link-audit clean conf lvgl-host mgba-host

all: sim

conf: config/lv_conf.h

config/lv_conf.h: tools/gen_lv_conf.py $(LVGL_DIR)/lv_conf_template.h
	python3 tools/gen_lv_conf.py

$(LVGL_HOST_LIB): $(shell find $(LVGL_DIR)/src -name '*.c' 2>/dev/null)
	$(MAKE) -f tools/lvgl-build.mk \
		LVGL_DIR=$(LVGL_DIR) LVGL_BUILD_DIR=$(LVGL_HOST_DIR) \
		CC='$(HOSTCC)' AR='$(AR)' \
		CFLAGS='$(HOST_CFLAGS) -DLV_CONF_INCLUDE_SIMPLE -Iconfig'

$(MGBA_LIB): $(MGBA_OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(MGBA_OBJECTS)

$(MGBA_BUILD)/%.o: $(MGBA_DIR)/%.c $(shell find $(MGBA_DIR)/include -name '*.h' 2>/dev/null)
	@mkdir -p $(dir $@)
	$(HOSTCC) $(MGBA_CFLAGS) -c $< -o $@

mgba-host: $(MGBA_LIB)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(HOSTCC) $(SIM_CFLAGS) -MMD -MP -c $< -o $@

sim: config/lv_conf.h $(LVGL_HOST_LIB) $(MGBA_LIB) $(SIM_OBJECTS)
	@mkdir -p build
	$(HOSTCC) $(HOST_CFLAGS) -o build/tinyos-sim $(SIM_OBJECTS) $(LVGL_HOST_LIB) $(MGBA_LIB) -lm $(SIM_PLATFORM_LIBS)
	./build/tinyos-sim --screen home --out build/home.png
	@echo "simulator: build/tinyos-sim, screenshot: build/home.png"

layout-audit: sim
	bash tests/layout-audit.sh

$(EMU_SELFTEST_BUILD)/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(HOSTCC) $(EMU_SELFTEST_CFLAGS) -MMD -MP -c $< -o $@

$(EMU_SELFTEST_BIN): $(EMU_SELFTEST_OBJECTS) $(MGBA_LIB)
	@mkdir -p $(dir $@)
	$(HOSTCC) $(HOST_CFLAGS) -o $@ $(EMU_SELFTEST_OBJECTS) $(MGBA_LIB) \
		-lm $(SIM_PLATFORM_LIBS)

emulator-selftest: $(EMU_SELFTEST_BIN)
	@python3 tools/gen_emulator_test_roms.py "$(EMU_SELFTEST_ROM_DIR)"
	@rm -f "$(EMU_SELFTEST_ROM_DIR)"/gb/*.sav \
		"$(EMU_SELFTEST_ROM_DIR)"/gbc/*.sav \
		"$(EMU_SELFTEST_ROM_DIR)"/gba/*.sav
	@$(EMU_SELFTEST_BIN) "$(EMU_SELFTEST_ROM_DIR)"

arm-link-audit:
	@ZIG="$(ZIG)" AR="$(AR)" MAKE="$(MAKE)" bash tools/arm-link-audit.sh

shots: sim
	@mkdir -p docs
	@rm -f build/tinyos-settings.ini
	@for screen in home games settings music system about appearance themes wallpapers typeface; do \
		./build/tinyos-sim --screen $$screen --out docs/ui-$$screen.png || exit 1; \
	done
	@echo "screenshots written to docs/"

clean:
	rm -rf build

-include $(SIM_DEPS)

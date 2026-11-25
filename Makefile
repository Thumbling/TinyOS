# TinyOS host tools.
#
#   make sim HOSTCC="/path/to/zig cc" AR="/path/to/zig ar"
#   make shots HOSTCC="/path/to/zig cc" AR="/path/to/zig ar"
#
# The firmware itself is built by Buildroot in CI (see .github/workflows).

HOSTCC ?= cc
AR ?= ar
HOST_CFLAGS ?= -std=gnu11 -O2 -g -Wall -Wextra -Wno-unused-parameter
LVGL_DIR := third_party/lvgl
LVGL_HOST_DIR := build/lvgl-host
LVGL_HOST_LIB := $(LVGL_HOST_DIR)/liblvgl.a

SIM_SOURCES := sim/main.c sim/png.c src/app.c \
	$(wildcard src/core/*.c) $(wildcard src/ui/*.c) \
	$(wildcard src/modules/*.c) $(wildcard src/platform/host/*.c)
SIM_OBJECTS := $(patsubst %.c,build/%.o,$(SIM_SOURCES))

SIM_CFLAGS := $(HOST_CFLAGS) -DTINYOS_SIM \
	-DTINYOS_SETTINGS_PATH='"build/tinyos-settings.ini"' \
	-DLV_CONF_INCLUDE_SIMPLE -Iconfig -Isrc -Isim -Ithird_party \
	-I$(LVGL_DIR)/include

.PHONY: all sim shots clean conf lvgl-host

all: sim

conf: config/lv_conf.h

config/lv_conf.h: tools/gen_lv_conf.py $(LVGL_DIR)/lv_conf_template.h
	python3 tools/gen_lv_conf.py

$(LVGL_HOST_LIB): $(shell find $(LVGL_DIR)/src -name '*.c' 2>/dev/null)
	$(MAKE) -f tools/lvgl-build.mk \
		LVGL_DIR=$(LVGL_DIR) LVGL_BUILD_DIR=$(LVGL_HOST_DIR) \
		CC='$(HOSTCC)' AR='$(AR)' \
		CFLAGS='$(HOST_CFLAGS) -DLV_CONF_INCLUDE_SIMPLE -Iconfig'

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(HOSTCC) $(SIM_CFLAGS) -c $< -o $@

sim: config/lv_conf.h $(LVGL_HOST_LIB) $(SIM_OBJECTS)
	@mkdir -p build
	$(HOSTCC) $(HOST_CFLAGS) -o build/tinyos-sim $(SIM_OBJECTS) $(LVGL_HOST_LIB) -lm
	./build/tinyos-sim --screen home --out build/home.png
	@echo "simulator: build/tinyos-sim, screenshot: build/home.png"

shots: sim
	@mkdir -p docs
	@for screen in home apps settings music system about; do \
		./build/tinyos-sim --screen $$screen --out docs/ui-$$screen.png || exit 1; \
	done
	@echo "screenshots written to docs/"

clean:
	rm -rf build

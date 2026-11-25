# Build LVGL 9.6 as a static library.
#
# Used by the host simulator, the ARM smoke build, and the Buildroot package so
# all three compile exactly the same sources with the same configuration.

LVGL_DIR ?= third_party/lvgl
LVGL_BUILD_DIR ?= build/lvgl
LVGL_LIB ?= $(LVGL_BUILD_DIR)/liblvgl.a
CC ?= cc
AR ?= ar
CFLAGS ?= -O2
LVGL_CFLAGS ?=

LVGL_SOURCES := $(shell find $(LVGL_DIR)/src -name '*.c' 2>/dev/null)
LVGL_OBJECTS := $(patsubst $(LVGL_DIR)/%.c,$(LVGL_BUILD_DIR)/%.o,$(LVGL_SOURCES))

.PHONY: all clean
all: $(LVGL_LIB)

$(LVGL_LIB): $(LVGL_OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(LVGL_BUILD_DIR)/%.o: $(LVGL_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LVGL_CFLAGS) -I$(LVGL_DIR)/include -I$(LVGL_DIR) -c $< -o $@

clean:
	rm -rf $(LVGL_BUILD_DIR)

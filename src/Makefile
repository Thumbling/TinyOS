# TinyOS firmware build.
#
# This Makefile is used by the Buildroot package (cross compiler + sysroot) and
# can also be driven manually for an ARM smoke build. The host simulator has its
# own Makefile at the repository root.

CC ?= cc
AR ?= ar
CFLAGS ?= -O2 -g
LDFLAGS ?=
LVGL_LIBS ?= -llvgl
CONFIG_DIR ?= ../config

override CFLAGS += -std=c11 -Wall -Wextra -Wno-unused-parameter \
	-D_GNU_SOURCE -D_DEFAULT_SOURCE -DLV_CONF_INCLUDE_SIMPLE -I. -I$(CONFIG_DIR)
override LDLIBS += $(LVGL_LIBS) -lasound -lpthread -lm

MGBA_DIR ?= ../third_party/mgba
MGBA_BUILD := build/mgba
MGBA_LIB := $(MGBA_BUILD)/libmgba.a
include $(MGBA_DIR)/sources.mk

ifeq ($(OS),Windows_NT)
MGBA_OS_SOURCES := $(MGBA_WINDOWS_SOURCES)
MGBA_OS_CFLAGS := -D_WIN32_WINNT=0x0600
else
MGBA_OS_SOURCES := $(MGBA_POSIX_SOURCES)
MGBA_OS_CFLAGS := -D_GNU_SOURCE -DHAVE_LOCALE -DHAVE_USELOCALE
endif

MGBA_SOURCES := $(MGBA_CORE_SOURCES) $(MGBA_OS_SOURCES)
MGBA_OBJECTS := $(patsubst %.c,$(MGBA_BUILD)/%.o,$(MGBA_SOURCES))
MGBA_DEPS := $(MGBA_OBJECTS:.o=.d)
MGBA_CFLAGS := $(CPPFLAGS) $(CFLAGS) -std=gnu11 -O2 -w -MMD -MP \
	-DBUILD_STATIC -DM_CORE_GBA -DM_CORE_GB \
	-DHAVE_STRDUP -DHAVE_VASPRINTF -DHAVE_SETLOCALE \
	-I$(MGBA_DIR)/include -I$(MGBA_DIR)/src $(MGBA_OS_CFLAGS)

SOURCES := \
	main.c \
	app.c \
	core/key_repeat.c \
	core/log.c \
	core/settings.c \
	core/util.c \
	emulator/emulator.c \
	emulator/session.c \
	emulator/input_map.c \
	emulator/rom.c \
	ui/scale.c \
	ui/theme.c \
	ui/fonts.c \
	ui/widgets.c \
	ui/wallpaper.c \
	ui/shell.c \
	$(wildcard ui/fonts/*.c) \
	modules/modules.c \
	modules/home.c \
	modules/games.c \
	modules/settings.c \
	modules/music.c \
	modules/system.c \
	modules/about.c \
	modules/appearance.c \
	modules/themes.c \
	modules/wallpapers.c \
	modules/typeface.c \
	platform/linux/display_fbdev.c \
	platform/linux/fbdev_blit.c \
	platform/linux/input_evdev.c \
	platform/linux/power_linux.c \
	platform/linux/audio_alsa.c

OBJECTS := $(SOURCES:.c=.o)
DEPS := $(OBJECTS:.o=.d)
TARGET := tinyos

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MGBA_LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(MGBA_LIB) $(LDLIBS)

$(MGBA_LIB): $(MGBA_OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(MGBA_OBJECTS)

$(MGBA_BUILD)/%.o: $(MGBA_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(MGBA_CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)
	rm -rf build

-include $(DEPS) $(MGBA_DEPS)

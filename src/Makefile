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

SOURCES := \
	main.c \
	app.c \
	core/log.c \
	core/settings.c \
	core/util.c \
	ui/theme.c \
	ui/fonts.c \
	ui/widgets.c \
	ui/wallpaper.c \
	ui/shell.c \
	$(wildcard ui/fonts/*.c) \
	modules/modules.c \
	modules/home.c \
	modules/apps.c \
	modules/settings.c \
	modules/music.c \
	modules/system.c \
	modules/about.c \
	modules/appearance.c \
	modules/themes.c \
	modules/wallpapers.c \
	modules/typeface.c \
	platform/linux/display_fbdev.c \
	platform/linux/input_evdev.c \
	platform/linux/power_linux.c \
	platform/linux/audio_alsa.c

OBJECTS := $(SOURCES:.c=.o)
DEPS := $(OBJECTS:.o=.d)
TARGET := tinyos

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

-include $(DEPS)

# Build the vendored mGBA 0.10.5 core as a static library.
#
# The core is small enough to compile from source in both the host simulator
# and the Buildroot package. No CMake, OpenGL, debugger, scripting or external
# decoder is involved; the frontend links this archive directly.

MGBA_DIR ?= .
MGBA_BUILD ?= build
MGBA_CC ?= cc
MGBA_AR ?= ar
MGBA_LIB ?= $(MGBA_BUILD)/libmgba.a

include $(MGBA_DIR)/sources.mk

ifeq ($(OS),Windows_NT)
MGBA_PLATFORM_SOURCES := $(MGBA_WINDOWS_SOURCES)
MGBA_PLATFORM_CFLAGS := -D_WIN32_WINNT=0x0600
else
MGBA_PLATFORM_SOURCES := $(MGBA_POSIX_SOURCES)
MGBA_PLATFORM_CFLAGS := -D_GNU_SOURCE -DHAVE_LOCALE
endif

MGBA_SOURCES := $(MGBA_CORE_SOURCES) $(MGBA_PLATFORM_SOURCES)
MGBA_OBJECTS := $(patsubst %.c,$(MGBA_BUILD)/%.o,$(MGBA_SOURCES))

# mGBA is third-party code; its own warning set is intentionally not mixed
# with TinyOS's -Wall -Wextra gate. The flags below are the portable subset
# used by mGBA's CMake LIBMGBA_ONLY build.
MGBA_CFLAGS ?= -std=gnu11 -O2 -w -DBUILD_STATIC -DM_CORE_GBA -DM_CORE_GB \
	-DHAVE_STRDUP -DHAVE_VASPRINTF -DHAVE_SETLOCALE \
	-I$(MGBA_DIR)/include -I$(MGBA_DIR)/src $(MGBA_PLATFORM_CFLAGS)

.PHONY: all clean

all: $(MGBA_LIB)

$(MGBA_LIB): $(MGBA_OBJECTS)
	@mkdir -p $(dir $@)
	$(MGBA_AR) rcs $@ $(MGBA_OBJECTS)

$(MGBA_BUILD)/%.o: $(MGBA_DIR)/%.c
	@mkdir -p $(dir $@)
	$(MGBA_CC) $(MGBA_CFLAGS) -c $< -o $@

clean:
	rm -rf $(MGBA_BUILD)

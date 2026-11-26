################################################################################
#
# tinyos
#
################################################################################

TINYOS_VERSION = 0.1.0
TINYOS_SITE = $(BR2_EXTERNAL_TINYOS_PATH)/../src
TINYOS_SITE_METHOD = local
TINYOS_DEPENDENCIES = lvgl alsa-lib
TINYOS_LICENSE = MIT

define TINYOS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_DIR="$(BR2_EXTERNAL_TINYOS_PATH)/../config" \
		LDFLAGS="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib" \
		LVGL_LIBS="-llvgl"
endef

define TINYOS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/tinyos $(TARGET_DIR)/usr/bin/tinyos
endef

# The panel driver is probed before the rootfs is available. Embed the init
# sequence in the kernel image so the display comes up during the same probe.
define TINYOS_LINUX_FIRMWARE
	$(INSTALL) -D -m 0644 \
		"$(BR2_EXTERNAL_TINYOS_PATH)/board/rg-nano/firmware/saef,sftc154b.bin" \
		"$(@D)/firmware/saef,sftc154b.bin"
endef
LINUX_POST_PATCH_HOOKS += TINYOS_LINUX_FIRMWARE

$(eval $(generic-package))

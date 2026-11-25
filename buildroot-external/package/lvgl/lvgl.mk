################################################################################
#
# lvgl
#
################################################################################

LVGL_VERSION = 9.6.0
LVGL_SITE = $(call github,lvgl,lvgl,v$(LVGL_VERSION))
LVGL_LICENSE = MIT
LVGL_LICENSE_FILES = LICENCE.txt
LVGL_INSTALL_STAGING = YES
LVGL_INSTALL_TARGET = NO

define LVGL_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -f $(BR2_EXTERNAL_TINYOS_PATH)/tools/lvgl-build.mk \
		LVGL_DIR=$(@D) \
		LVGL_BUILD_DIR=$(@D)/build \
		CC="$(TARGET_CC)" \
		AR="$(TARGET_AR)" \
		CFLAGS="$(TARGET_CFLAGS) -DLV_CONF_INCLUDE_SIMPLE -I$(BR2_EXTERNAL_TINYOS_PATH)/config"
endef

define LVGL_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/build/liblvgl.a $(STAGING_DIR)/usr/lib/liblvgl.a
	$(INSTALL) -d $(STAGING_DIR)/usr/include/lvgl
	cp -r $(@D)/include/lvgl/. $(STAGING_DIR)/usr/include/lvgl/
endef

$(eval $(generic-package))

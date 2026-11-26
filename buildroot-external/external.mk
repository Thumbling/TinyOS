include $(sort $(wildcard $(BR2_EXTERNAL_TINYOS_PATH)/package/*/*.mk))

# U-Boot 2025.10 (commit a239b0b0b6db) replaced the COPYING file with a symlink
# to Licenses/gpl-2.0.txt. Buildroot 2025.02.3 still ships the pre-2025.10 hook,
# whose "install COPYING Licenses/gpl-2.0.txt" then aborts the extract step with
# "are the same file". Upstream fixed the hook in commit 128c26f2875d and
# backported the one-line condition to 2025.02.x. This tree is parsed after
# boot/uboot/uboot.mk, so the redefinition below takes effect.
define UBOOT_COPY_OLD_LICENSE_FILE
	if [ -f $(@D)/COPYING ] && [ ! -f $(@D)/Licenses/gpl-2.0.txt ]; then \
		$(INSTALL) -m 0644 -D $(@D)/COPYING $(@D)/Licenses/gpl-2.0.txt; \
	fi
endef

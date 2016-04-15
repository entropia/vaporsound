# source included

RAM_OVERLAY_SOURCE =

RAM_OVERLAY_VERSION = 1.0.0
RAM_OVERLAY_LICENSE = GPLv2

define RAM_OVERLAY_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		package/ram-overlay/mount_overlay.c -o $(@D)/mount_ram_overlay
endef

define RAM_OVERLAY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/mount)_ram_overlay $(TARGET_DIR)/usr/sbin/mount_ram_overlay
endef

$(eval $(generic-package))

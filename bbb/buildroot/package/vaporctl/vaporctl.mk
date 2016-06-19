# source included

VAPORCTL_SOURCE =

VAPORCTL_VERSION = 1.0.0
VAPORCTL_LICENSE = GPLv2

define VAPORCTL_BUILD_CMDS
	cp -r $(BR2_EXTERNAL)/package/vaporctl/* $(@D)
	make -C $(@D)/src CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" all
endef

define VAPORCTL_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/src/vaporctl $(TARGET_DIR)/usr/sbin/vaporctl
	$(INSTALL) -D -m 755 $(@D)/vaporctl.service $(TARGET_DIR)/etc/service/vaporctl/run
endef

$(eval $(generic-package))

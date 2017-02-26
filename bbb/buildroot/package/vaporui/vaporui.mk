# source included

VAPORUI_SOURCE =

VAPORUI_VERSION = 1.0.0
VAPORUI_LICENSE = GPLv2

define VAPORUI_BUILD_CMDS
	cp -r $(BR2_EXTERNAL_Vaporsound_PATH)/package/vaporui/* $(@D)
	make -C $(@D)/src CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" all
endef

define VAPORUI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/src/vaporui $(TARGET_DIR)/usr/sbin/vaporui
	$(INSTALL) -D -m 755 $(@D)/vaporui.service $(TARGET_DIR)/etc/service/vaporui/run
endef

$(eval $(generic-package))

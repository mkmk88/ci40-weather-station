include $(TOPDIR)/rules.mk

PKG_NAME:=ci40-weather-station
PKG_VERSION:=HEAD
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/CreatorDev/weather-station-gateway

#usefull for development
#PKG_SOURCE_URL:=$(TOPDIR)/../../packages/weather-station-gateway

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_VERSION:=$(PKG_VERSION)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

CMAKE_INSTALL:=1

CMAKE_OPTIONS += -DSTAGING_DIR=$(STAGING_DIR)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ci40-weather-station
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:= +awalwm2m +letmecreate
  TITLE:=Weather Station gateway app
endef

define Package/ci40-weather-station/description
  Imagination Technologies Weather Station gateway app
endef

define Build/Compile
	$(call Build/Compile/Default,all)
endef

define Build/Clean
	$(call Build/Compile/Default,clean)
endef

define Package/ci40-weather-station/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/clientObjectsDefine.sh $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/weather_station_initd $(1)/etc/init.d/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

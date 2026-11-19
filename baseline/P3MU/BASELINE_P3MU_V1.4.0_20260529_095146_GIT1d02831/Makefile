include Makefile.param

define make_app
	$(call make_subdir, app, $(1), $(2), $(3))
endef

define make_firmware
	$(call make_subdir, $(FIRMWARE_ROOT_DIR), $(1), $(2), $(3))
endef

define make_modules
	$(call make_subdir, source/modules, $(1), $(2), $(3))
endef

define make_driver
	$(call make_subdir, $(DRIVER_DIR), $(1), $(2), $(3))
endef

define make_tools
	$(call make_subdir, source/tools, $(1), $(2), $(3))
endef

define make_partition
	$(call make_subdir, partition, $(1), $(2), $(3))
endef

define make_3rd
	$(call make_subdir, 3rd, $(1), $(2), $(3))
endef

define make_samples
	$(call make_subdir, samples, $(1), $(2), $(3))
endef

all: FORCE
	$(MAKE) distclean
	$(MAKE) firmware
	$(MAKE) 3rd
	$(MAKE) usr
	$(MAKE) partition

# Compile APPS
app: FORCE
	$(call make_app, all)
	$(call make_app, install)

clean-app: FORCE
	$(call make_app, clean)

#
# Common Compile Modules
# $(1): Module Name
# 
#define compile_module
#MOD = $(strip $(1))
#
#$(MOD)_MOD_NAME	:= $(MOD)
#$(MOD)_LIB_NAME	:= lib$$($(MOD)_MOD_NAME).a
#$(MOD)_SRC_DIR		:= $$(MODULES_DIR)/$(MOD)
#$(MOD)_NEWEST_DEP	:= $$(call find_newest_file, $$($(MOD)_SRC_DIR))
#$(MOD)_INSTALL_LIB	:= $$(LIB_DIR)
#$(MOD)_INSTALL_INC	:= $$(INC_DIR)
#$(MOD)_TARGET		:= $$($(MOD)_INSTALL_LIB)/$$($(MOD)_LIB_NAME)
#
#$(MOD): $$($(MOD)_TARGET)
#$$($(MOD)_TARGET): $$($(MOD)_NEWEST_DEP)
#	$$(Q)$$(ECHO) Compiling [$$@], Changed Dep: $$?
#	$$(call make_modules, $$($(MOD)_MOD_NAME))
#endef

ifeq ($(PRODUCT_MODE), dev)
# Compile 3rd
3rd: FORCE
	$(call make_3rd, install)

clean-3rd: FORCE
	$(call make_3rd, clean)

# Module List
MODULES	:= mauth

$(foreach MOD, $(MODULES), $(eval $(call compile_module, $(MOD))))

# Firmware
firmware: skeleton		\
		  uboot			\
		  kernel		\
		  busybox		\
		  fw-drivers	\
		  fw-lib		\
		  fw-tools

## UBOOT
UBOOT_TARGET	:= $(PARTITION_DIR)/u-boot.bin
uboot: FORCE
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(call make_firmware, uboot-install, INSTALL=$(UBOOT_TARGET))

## KERNEL 编译内核完成后就编译驱动，保持驱动跟内核版本一致
KERNEL_TARGET	:= $(PARTITION_DIR)/uImage
kernel: FORCE
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(call make_firmware, kernel-install, INSTALL=$(KERNEL_TARGET))

## ROOTFS
BUSYBOX_TARGET	:= $(PARTITION_DIR)/root
busybox: FORCE
	$(call make_firmware, busybox-install, INSTALL=$(BUSYBOX_TARGET))

## Driver
DRIVER_TARGET	:= $(PARTITION_DIR)/usr/ko
fw-drivers: FORCE
	$(call make_firmware, driver-install, INSTALL=$(DRIVER_TARGET))

## Firmware Tools
fw-tools: FORCE
	$(call make_firmware, pkgtools)

## Firmware Lib
fw-lib: FORCE
	$(call make_firmware, lib-install, INSTALL=$(PARTITION_DIR)/usr/lib)

skeleton: FORCE
	$(call make_firmware, skeleton)

clean-firmware: FORCE
	$(call make_firmware, clean)

### XRS Source
# Modules
modules: FORCE
	$(call make_modules, all)

clean-modules: FORCE
	$(call make_modules, clean)

# Drivers
usr-drivers: FORCE
	$(call make_driver, drivers)
	$(call make_driver, install, INSTALL=$(DRIVER_TARGET))

clean-drivers: FORCE
	$(call make_driver, clean)

# Tools
tools: FORCE
	$(call make_tools, all)

clean-tools: FORCE
	$(call make_tools, clean)

usr: modules usr-drivers tools
clean-usr: clean-modules clean-drivers clean-tools

endif

#########################################################
# RELEASE                                               #
#########################################################
ifeq ($(PRODUCT_MODE), dev)
RELEASE_BASE_LIST	:= app								\
					   $(CONFIG_DIR)/BoardFunc.mk		\
					   $(CONFIG_DIR)/$(BOARD_TYPE)		\
					   $(PACKAGE_LIB_DIR)				\
					   $(PACKAGE_LIB_DEBUG_DIR)			\
					   $(PACKAGE_INC_DIR)  		    	\
					   $(INC_DIR)						\
					   $(LIB_DIR)						\
					   $(SAMPLES_DIR)					\
					   $(SCRIPTS_DIR)					\
					   $(TOOLS_DIR)						\
					   $(PARTITION_DIR)					\
					   .gitignore						\
					   Makefile							\
					   Makefile.param					\
					   ReleaseNote-BSP.md				\
					   ReleaseNote-DSP.md

REVISION_STR := $(shell $(GET_REVISION_SH))
ifeq ($(DEBUG), y)
RELEASE_TARGET_NAME	:= DEBUG_$(BOARD_TYPE)_$(BASELINE_VER)_$(shell date +%Y%m%d_%H%M%S)_$(REVISION_STR)
else
RELEASE_TARGET_NAME	:= BASELINE_$(BOARD_TYPE)_$(BASELINE_VER)_$(shell date +%Y%m%d_%H%M%S)_$(REVISION_STR)
endif

prepare-release: FORCE
	$(MAKE) clean
	$(MAKE) firmware
	$(MAKE) 3rd
	$(MAKE) usr
	$(MAKE) clean

release: prepare-release
	@$(ECHO) "############# MAKE RELEASE #############"
	@$(ECHO) PLATFORM: $(PLATFORM) BOARD: $(BOARD_TYPE) SENSORS: $(SENSORS)
	@$(RM) $(RELEASE_ROOT_DIR)/$(RELEASE_TARGET_NAME)
	@$(MKDIR) -p $(RELEASE_ROOT_DIR)/$(RELEASE_TARGET_NAME)
	@for file in $(RELEASE_BASE_LIST); do 						\
		if [ ! -e $$(dirname $(RELEASE_ROOT_DIR)/$(RELEASE_TARGET_NAME)/$$file) ]; then		\
			mkdir -p $$(dirname $(RELEASE_ROOT_DIR)/$(RELEASE_TARGET_NAME)/$$file);			\
		fi;														\
		cp -rvaf $$file $(RELEASE_ROOT_DIR)/$(RELEASE_TARGET_NAME)/$$file;					\
	done
	$(RM) $(RELEASE_ROOT_DIR)/$(RELEASE_TARGET_NAME)/$(PRODUCT_MODE_MK)
	$(PUSHD) $(RELEASE_ROOT_DIR);										\
	find . -name ".gitkeep" | xargs rm -rf;								\
	find . -name ".svn" | xargs rm -rf;									\
	$(TAR) czvf $(RELEASE_TARGET_NAME).tar.gz $(RELEASE_TARGET_NAME);	\
	$(RM) -rf $(RELEASE_TARGET_NAME)									\
	$(POPD);
endif

#########################################################
# PARTITION                                             #
#########################################################
PARTITION_DEP_TOOLS := $(UPGRADE_MAKER) $(IMAGESIGN) $(MKENVIMAGE)
factory: $(PARTITION_DIR) $(PARTITION_DEP_TOOLS)
	$(call make_partition, factory)

upgrade: $(PARTITION_DIR) $(PARTITION_DEP_TOOLS)
	$(call make_partition, upgrade)

resume: $(PARTITION_DIR) $(PARTITION_DEP_TOOLS)
	$(call make_partition, resume)

restore: $(PARTITION_DIR) $(PARTITION_DEP_TOOLS)
	$(call make_partition, restore)

partition: factory upgrade resume restore

clean-partition: FORCE
	@$(ECHO) CLEAN    partition
	$(call make_partition, clean)

###########################################
# samples
###########################################
samples: FORCE
	$(call make_samples, all)

clean-samples: FORCE
	@$(ECHO) CLEAN    samples
	$(call make_samples, clean)

ifeq ($(PRODUCT_MODE), dev)
deepclean: distclean
	$(call make_3rd, deepclean)

distclean: clean
	@$(RM) $(LIB_DIR)/*
	@$(RM) $(LIB_X86_DIR)/*
	@$(RM) $(INC_DIR)/3rd/*
	$(call make_partition, distclean)
endif

clean: FORCE
ifeq ($(PRODUCT_MODE), dev)
	@$(RM) $(RELEASE_ROOT_DIR)
	@$(MAKE) clean-3rd 
	@$(MAKE) clean-firmware 
	@$(MAKE) clean-usr
endif
	@$(MAKE) clean-partition
	@$(MAKE) clean-samples

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)

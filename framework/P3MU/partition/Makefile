include ../Makefile.param

MK_JFFS2	= mkfs.jffs2
MK_CRAMFS	= mkfs.cramfs
MK_SQUASHFS	= mksquashfs
MK_EXT4FS	= make_ext4fs
MK_JFFS2	= $(TOOLS_DIR)/pc/mkfs.jffs2
MK_YAFFS2	= $(TOOLS_DIR)/pc/mkyaffs2image
MAKE_FACTORY_SH	= $(SCRIPTS_DIR)/MKFactoryFS.sh

ifneq ($(findstring clean, $(MAKECMDGOALS)), clean)

# Upgrade Define
UPGRADE_ERASURE	:= erasure
UPGRADE_OVERLAY	:= overlay
UPGRADE_TAR		:=     tar
UPGRADE_USR_PART	:= USR
UPGRADE_IMG_LIST_FILE	:= upgrade_list.txt
UPGRADE_DEVLIST_FILE	:= $(PARTITION_DIR)/custom_made/$(custom)/DeviceType.txt
UPGRADE_DEVTYPE			:= $(shell cat $(UPGRADE_DEVLIST_FILE) | head -n 1 | cut -b 6-)

UPGRADE_PACKAGE_TYPE	:= 1
RESTORE_PACKAGE_TYPE	:= 2
RESUME_PACKAGE_TYPE		:= 3

UPGRADE_ENCRYPT_TYPE_NONE	:= 0
UPGRADE_ENCRYPT_TYPE_AES	:= 1
UPGRADE_ENCRYPT_TYPE		:= 0
ifeq ($(SUPPORT_ENCRYPT_UPGRADE), true)
UPGRADE_ENCRYPT_TYPE = $(UPGRADE_ENCRYPT_TYPE_AES)
endif

# Hardware id mask
HWID_MASK		?= 0xffffffff

ifeq ($(D), ls)
BUILD_TIME  = $(shell date '+%Y%m%d%H%M')
else
BUILD_TIME  = $(shell date '+%Y%m%d')
endif

SWVER_FILE		:= $(PARTITION_DIR)/custom_made/$(custom)/$(customer)/DeviceVersion.ini
SWVER_CUR_MAJOR	:= $(shell $(READINI) $(SWVER_FILE) Software Major_version)
SWVER_CUR_MINOR := $(shell $(READINI) $(SWVER_FILE) Software Minor_version)
SWVER_CUR_REVER	:= $(shell $(READINI) $(SWVER_FILE) Software Reversion)
SWVER_CUR_DATE	:= $(shell $(READINI) $(SWVER_FILE) Software BuildDate)
SWVER_CUR		:= V$(SWVER_CUR_MAJOR).$(SWVER_CUR_MINOR).$(SWVER_CUR_REVER)_$(SWVER_CUR_DATE)

CUSTOM_INFO_FILE := $(PARTITION_DIR)/app/custom/info/custominfo.ini
CUSTOM_INFO_CUSTOMNO := $(shell $(READINI) $(CUSTOM_INFO_FILE) CloudUpgrade CustomNo)

CUSTOM_MODE_FILE := $(PARTITION_DIR)/custom_made/$(custom)/$(customer)/custommode.ini
CUSTOM_MODE_LANGUAGE := $(shell $(READINI) $(CUSTOM_MODE_FILE) Speech SpeechLanguage)
#custommode.ini 1--中文 2--英文
ifeq ($(CUSTOM_MODE_LANGUAGE), 2)
	SPEECH_LANGUAGE := EN
else
	SPEECH_LANGUAGE := CN
endif

CUSTOM_MODE_FUNCTION_TYPE := $(shell $(READINI) $(CUSTOM_MODE_FILE) SystemFunction FunctionType)
ifeq ($(CUSTOM_MODE_FUNCTION_TYPE), P3MU-52PGR-S/W/BT)
CUSTOM_MODE_FUNCTION_TYPE := P3MU-52PGR-S_W_BT
endif
UPGRADE_BIN_NAME=upgrade.bin
RESTORE_BIN_NAME=restore.bin
#UPGRADE_BIN_NAME := UPDATE_"$(CUSTOM_MODE_FUNCTION_TYPE)"_$(SWVER_CUR)_$(customer)_$(SPEECH_LANGUAGE)_$(CUSTOM_INFO_CUSTOMNO).bin
#RESTORE_BIN_NAME := ipcUpgrade_$(CUSTOM_MODE_FUNCTION_TYPE)_$(SWVER_CUR)_$(customer)_$(SPEECH_LANGUAGE)_$(CUSTOM_INFO_CUSTOMNO).bin


SWVER_BASE_MAJOR := $(shell $(READINI) $(SWVER_FILE) BaseSoftware Major_version)
SWVER_BASE_MINOR := $(shell $(READINI) $(SWVER_FILE) BaseSoftware Minor_version)
SWVER_BASE_REVER := $(shell $(READINI) $(SWVER_FILE) BaseSoftware Reversion)
SWVER_BASE_DATE	 := $(shell $(READINI) $(SWVER_FILE) BaseSoftware BuildDate)
SWVER_BASE		 := V$(SWVER_BASE_MAJOR).$(SWVER_BASE_MINOR).$(SWVER_BASE_REVER)_$(SWVER_BASE_DATE)

all: FORCE
	@$(ECHO) -e "\033[1;31m BUILD_VERSION: [${BUILD_TIME}] \033[0m"
	$(shell sed -i "/\[Software\]/,/\[/ {/BuildDate/s/=.*/=${BUILD_TIME}/}" ${SWVER_FILE})
	$(MAKE) factory
	$(MAKE) upgrade
	$(MAKE) resume
	$(MAKE) restore
	@if [ -x $(FINISH_MAKE_SH) ];then							\
		$(FINISH_MAKE_SH) all $(UPGRADE_DEVTYPE) $(SWVER_CUR);	\
	fi
endif

ifeq ($(FLASH_TYPE), SPI_NAND)
include $(NAND_FLASH_FS_MK)
endif

ifeq ($(FLASH_TYPE), SPI_NOR)
include $(NOR_FLASH_FS_MK)
endif

ifneq ($(findstring clean, $(MAKECMDGOALS)), clean)
# Custom Script
CUSTOM_MADE_SH	:= $(PARTITION_DIR)/CustomMade.sh
custom-made: FORCE
	@$(ECHO) Run Custom-Made Script... $(BOARD_TYPE) $(SENSORS) $(debug) $(custom) $(customer) $(speechlanguage) $(customno)
	@$(CUSTOM_MADE_SH) -b $(BOARD_TYPE) -s $(SENSORS) -d $(debug) -p $(custom) -c $(customer) -l $(speechlanguage) -v $(customno)

# Finish Script
FINISH_MAKE_SH	:= $(PARTITION_DIR)/FinishMake.sh

# FACTORY
FACTORY_BIN	= factory.bin

# Must List By Order
FACTORY_DEP_NAME	= $(UBOOT_BIN)
FACTORY_DEP_SIZE	= $(PART_UBOOT_SIZE)
FACTORY_DEP_OFFS	= $(PART_UBOOT_OFFSET)

FACTORY_DEP_NAME	+= $(ENV_BIN)
FACTORY_DEP_SIZE	+= $(PART_ENV_SIZE)
FACTORY_DEP_OFFS	+= $(PART_ENV_OFFSET)

FACTORY_DEP_NAME	+= $(ROOTFS_BIN)
FACTORY_DEP_SIZE	+= $(PART_ROOTFS_SIZE)
FACTORY_DEP_OFFS	+= $(PART_ROOTFS_OFFSET)

ifeq ($(SUPPORT_DUAL_BACKUP), y)
FACTORY_DEP_NAME	+= $(ROOTFS_BIN)
FACTORY_DEP_SIZE	+= $(PART_ROOTFSB_SIZE)
FACTORY_DEP_OFFS	+= $(PART_ROOTFSB_OFFSET)
endif

FACTORY_DEP_NAME	+= $(DATA_BIN)
FACTORY_DEP_SIZE	+= $(PART_DATA_SIZE)
FACTORY_DEP_OFFS	+= $(PART_DATA_OFFSET)

ifeq ($(SUPPORT_DUAL_BACKUP), y)
FACTORY_DEP_NAME	+= $(DATB_BIN)
FACTORY_DEP_SIZE	+= $(PART_DATB_SIZE)
FACTORY_DEP_OFFS	+= $(PART_DATB_OFFSET)
endif

ifeq ($(SUPPORT_PART_BACK), y)
FACTORY_DEP_NAME	+= $(BACK_BIN)
FACTORY_DEP_SIZE	+= $(PART_BACK_SIZE)
FACTORY_DEP_OFFS	+= $(PART_BACK_OFFSET)
endif

ifeq ($(SUPPORT_PART_STORAGE), y)
FACTORY_DEP_NAME	+= $(STORAGE_BIN)
FACTORY_DEP_SIZE	+= $(PART_STORAGE_SIZE)
FACTORY_DEP_OFFS	+= $(PART_STORAGE_OFFSET)
endif

FACTORY_DEP_NAME	+= $(DEVINFO_BIN)
FACTORY_DEP_SIZE	+= $(PART_DEVINFO_SIZE)
FACTORY_DEP_OFFS	+= $(PART_DEVINFO_OFFSET)

factory: custom-made $(FACTORY_BIN)
$(FACTORY_BIN): $(FACTORY_DEP_NAME)
	$(MAKE_FACTORY_SH) -p "$(PLATFORM)"			\
					   -v "$(SWVER_CUR)" 		\
					   -o "$(PARTITION_DIR)"	\
					   -n "$(FACTORY_DEP_NAME)"	\
					   -s "$(FACTORY_DEP_SIZE)"	\
					   -f "$(FACTORY_DEP_OFFS)" \
					   -c "$(CUSTOM_INFO_CUSTOMNO)" \
					   -l "$(SPEECH_LANGUAGE)" \
					   -d "$(CUSTOM_MODE_FUNCTION_TYPE)"
	@if [ -x $(FINISH_MAKE_SH) ];then 								\
		$(FINISH_MAKE_SH) factory $(UPGRADE_DEVTYPE) $(SWVER_CUR);	\
	fi

# UPGRADE
# TAR
UPGRADE_CFG_TAR_DIR		:= tar
UPGRADE_CFG_TAR_FILE	:= $(OUTPUT_TMP)/cfg.tar
UPGRADE_CFG_TAR_DST		:= /mnt/mtd
upgrade-tar: $(UPGRADE_CFG_TAR_FILE)
$(UPGRADE_CFG_TAR_FILE): $(UPGRADE_CFG_TAR_DIR) $(call find_newest_file_all, $(UPGRADE_CFG_TAR_DIR))
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(PUSHD) $(UPGRADE_CFG_TAR_DIR);	\
	$(TAR) cvf ../$(UPGRADE_CFG_TAR_FILE) *;	\
	$(POPD);

list-prepare: FORCE
	@$(RM) $(UPGRADE_IMG_LIST_FILE)
	$(ECHO) "# PARTITION	UPGRADE-TYPE	FILE	DST" >> $(UPGRADE_IMG_LIST_FILE)

list-append-tar: $(UPGRADE_IMG_LIST_FILE)
ifneq ($(WITHOUT-TAR), y)
	$(ECHO) "$(UPGRADE_USR_PART)	$(UPGRADE_TAR)	$(UPGRADE_CFG_TAR_FILE)	$(UPGRADE_CFG_TAR_DST)" >> $<
endif

list-append-base: FORCE
ifneq ($(WITHOUT-UBOOT), y)
	$(ECHO) "$(PART_UBOOT_NAME)		$(UPGRADE_ERASURE)	$(UBOOT_BIN)	$(PART_UBOOT_NAME)"	>> $(UPGRADE_IMG_LIST_FILE)
endif
ifneq ($(WITHOUT-ENV), y)
	$(ECHO) "$(PART_ENV_NAME)		$(UPGRADE_ERASURE)	$(ENV_BIN)		$(PART_ENV_NAME)"	>> $(UPGRADE_IMG_LIST_FILE)
endif
ifneq ($(WITHOUT-ROOTFS), y)
ifeq ($(SUPPORT_DUAL_BACKUP), y)
	$(ECHO) "$(PART_ROOTFSB_NAME)	$(UPGRADE_ERASURE)	$(ROOTFS_BIN)	$(PART_ROOTFSB_NAME)"	>> $(UPGRADE_IMG_LIST_FILE)
else
	$(ECHO) "$(PART_ROOTFS_NAME)	$(UPGRADE_ERASURE)	$(ROOTFS_BIN)	$(PART_ROOTFS_NAME)"	>> $(UPGRADE_IMG_LIST_FILE)
endif
endif
ifneq ($(WITHOUT-DATB), y)
ifeq ($(SUPPORT_DUAL_BACKUP), y)
	$(ECHO) "$(PART_DATB_NAME)		$(UPGRADE_ERASURE)	$(DATB_BIN)		$(PART_DATB_NAME)"	>> $(UPGRADE_IMG_LIST_FILE)
endif
endif

rlist-append-base: list-append-base
ifneq ($(WITHOUT-DATA), y)
	$(ECHO) "$(PART_DATA_NAME)		$(UPGRADE_ERASURE)	$(DATA_BIN)	$(PART_DATA_NAME)"	>> $(UPGRADE_IMG_LIST_FILE)
endif

# Append mpart file
define list-append-mpart

MPART := $(strip $(1))

MPART_$(MPART)_BIN := $$(MPART_$(MPART)_NAME).single.$$(MPART_$(MPART)_FORMAT)
MPART_$(MPART)_CHK := check_$$(MPART_$(MPART)_NAME).md5
$(MPART)_INSTALL_DIR := $(SINGLE_MPART_INSTALL_DIR)

list-append-mpart-$(MPART): $(UPGRADE_IMG_LIST_FILE)
ifneq ($$(WITHOUT-$(MPART)), y)
	$(ECHO) "$(UPGRADE_USR_PART) $(UPGRADE_OVERLAY)	$$($(MPART)_INSTALL_DIR)/$$(MPART_$(MPART)_BIN)	/data/$$(MPART_$(MPART)_BIN)" >> $$<
	$(ECHO) "$(UPGRADE_USR_PART) $(UPGRADE_OVERLAY)	$$($(MPART)_INSTALL_DIR)/$$(MPART_$(MPART)_CHK)	/data/$$(MPART_$(MPART)_CHK)" >> $$<
endif

endef

$(eval $(foreach MPART, $(SINGLE_MPART_LIST),$(call list-append-mpart, $(MPART))))

list-append-mpart: $(addprefix list-append-mpart-, $(SINGLE_MPART_LIST))

upgrade-list: list-prepare list-append-base list-append-mpart list-append-tar
upgrade: custom-made $(FACTORY_DEP_NAME) upgrade-tar upgrade-list
	$(UPGRADE_MAKER) -o $(UPGRADE_BIN_NAME)			\
					 -l $(UPGRADE_IMG_LIST_FILE)	\
					 -d $(UPGRADE_DEVLIST_FILE)		\
					 -t $(UPGRADE_PACKAGE_TYPE)		\
					 -v $(SWVER_CUR)				\
					 -b $(SWVER_BASE)				\
					 -e $(UPGRADE_ENCRYPT_TYPE)		\
					 -H $(HWID_MASK)
	@if [ -x $(FINISH_MAKE_SH) ];then								\
		$(FINISH_MAKE_SH) upgrade $(UPGRADE_DEVTYPE) $(SWVER_CUR);	\
	fi
	#cp $(UPGRADE_BIN_NAME) $(PWD)/upgrade.bin

upgrade-debug: FORCE
	$(UPGRADE_MAKER) -o $(UPGRADE_BIN_NAME) 		\
					 -l $(UPGRADE_IMG_LIST_FILE)	\
					 -d $(UPGRADE_DEVLIST_FILE)		\
					 -t $(UPGRADE_PACKAGE_TYPE)		\
					 -v $(SWVER_CUR)				\
					 -b $(SWVER_BASE)				\
					 -e $(UPGRADE_ENCRYPT_TYPE)		\
					 -H $(HWID_MASK)
	@if [ -x $(FINISH_MAKE_SH) ];then								\
		$(FINISH_MAKE_SH) upgrade $(UPGRADE_DEVTYPE) $(SWVER_CUR);	\
	fi

restore-list: list-prepare rlist-append-base
restore: custom-made $(FACTORY_DEP_NAME) restore-list
	$(UPGRADE_MAKER) -o $(RESTORE_BIN_NAME) 		\
					 -l $(UPGRADE_IMG_LIST_FILE)	\
					 -d $(UPGRADE_DEVLIST_FILE)		\
					 -t $(RESTORE_PACKAGE_TYPE)		\
					 -v $(SWVER_CUR) 				\
					 -b $(SWVER_BASE)				\
					 -e $(UPGRADE_ENCRYPT_TYPE)		\
					 -H $(HWID_MASK)
	@if [ -x $(FINISH_MAKE_SH) ];then								\
		$(FINISH_MAKE_SH) restore $(UPGRADE_DEVTYPE) $(SWVER_CUR);	\
	fi
	#cp $(RESTORE_BIN_NAME) $(PWD)/restore.bin

resume-list: list-prepare list-append-base
resume: custom-made $(FACTORY_DEP_NAME) resume-list
	$(UPGRADE_MAKER) -o $(RESUME_BIN_NAME)			\
					 -l $(UPGRADE_IMG_LIST_FILE)	\
					 -d $(UPGRADE_DEVLIST_FILE)		\
					 -t $(RESUME_PACKAGE_TYPE)		\
					 -v $(SWVER_CUR)				\
					 -b $(SWVER_BASE)				\
					 -e $(UPGRADE_ENCRYPT_TYPE)		\
					 -H $(HWID_MASK)
	@if [ -x $(FINISH_MAKE_SH) ];then								\
		$(FINISH_MAKE_SH) resume $(UPGRADE_DEVTYPE) $(SWVER_CUR);	\
	fi
endif # NOT CLEAN

ifeq ($(PRODUCT_MODE), dev)
distclean: clean
	$(RM) u-boot.bin bl31.bin uImage
	$(RM) root usr data datb model web app tar tmp storage
	$(RM) env.txt upgrade_list.txt
endif

clean_factory: FORCE
	@$(ECHO) CLEAN    factory
	@$(RM) $(FACTORY_BIN)

clean: clean_factory clean_images
	@$(RM) $(UPGRADE_CFG_TAR_FILE)
	@$(RM) $(UPGRADE_IMG_LIST_FILE)
	$(shell ls -al -Iu-boot.bin -Ibl31.bin | grep ".bin" | awk '{print $$9}' | xargs rm -rf)

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)

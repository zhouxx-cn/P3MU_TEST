
OUTPUT_TMP		:= $(PARTITION_DIR)/tmp

# Flash Config
NAND_BLOCK_SIZE	:= $(shell expr 64 \* 1024)
NAND_PAGE_SIZE	:= $(shell expr 4 \* 1024)

YAFFS2_PAGE_SIZE	=	"-p 2048"
YAFFS2_ECC_TYPE		=	"--inband-tags"

SQUASHFS_PARAM_GZ	= -b 64K -noappend -all-root
SQUASHFS_PARAM_XZ	= $(SQUASHFS_PARAM_GZ) -comp xz
SQUASHFS_PARAM		= $(SQUASHFS_PARAM_XZ)

# NOT CLEAN
ifneq ($(findstring clean, $(MAKECMDGOALS)), clean)

ifeq ($(wildcard $(OUTPUT_TMP)),)
$(shell mkdir -p $(OUTPUT_TMP))
endif

# UBOOT
UBOOT_BIN	:= $(PARTITION_DIR)/$(PART_UBOOT_FILE) 
uboot: $(UBOOT_BIN)
	$(info $(UBOOT_BIN))
	$(call check_filesize, $(UBOOT_BIN), $(PART_UBOOT_SIZE))

# ENV
ENV_BIN	:= $(OUTPUT_TMP)/$(PART_ENV_FILE)
ENV_TMP := $(OUTPUT_TMP)/env.tmp
ENV_SINGLE_BIN	:= $(OUTPUT_TMP)/128K-env.bin
ENV_TXT	:= $(PARTITION_DIR)/env.txt

env-prepare: $(ENV_TXT)
ifeq ($(UPGRADE_DEVTYPE),)
	$(error Device Type in $(UPGRADE_DEVLIST_FILE) Can not be Empty!)
else
	@if [ "`cat $(ENV_TXT) | grep boardtype`"x = ""x ]; then				\
		echo "boardtype=$(UPGRADE_DEVTYPE)" >> $(ENV_TXT);					\
	else																	\
		sed -i 's#^boardtype.*#boardtype=$(UPGRADE_DEVTYPE)#' $(ENV_TXT);	\
	fi
endif
ifeq ($(SWVER_CUR),)
	$(error Device Current Version in $(SWVER_FILE) Can not be Empty!)
else
	@if [ "`cat $(ENV_TXT) | grep boardver`"x = ""x ]; then				\
		echo "boardver=$(SWVER_CUR)" >> $(ENV_TXT);						\
	else																\
		sed -i 's/^boardver.*/boardver=$(SWVER_CUR)/' $(ENV_TXT);		\
	fi
endif

env: $(ENV_BIN)
$(ENV_BIN): env-prepare $(MKENVIMAGE)
	$(MKENVIMAGE) -r -s 0x20000 -p 0 -o $(ENV_TMP) $(ENV_TXT)
	dd if=/dev/zero of=$(ENV_SINGLE_BIN) bs=1K count=128
	dd if=$(ENV_TMP) of=$(ENV_SINGLE_BIN) conv=notrunc
	cat $(ENV_SINGLE_BIN) $(ENV_SINGLE_BIN) > $(ENV_BIN)

#
# 文件系统起始地址必须对齐
#
KERNEL_BIN		= $(PARTITION_DIR)/uImage
#KERNEL_SIZE		= $(shell expr 3 \* 1024 \* 1024)
#KERNEL_SZ_BYTE		= $(shell ls -l $(KERNEL_BIN) | awk '{print $$5}')

ifeq ($(HAVE_ATF), true)
ATF_BIN		:= $(PARTITION_DIR)/bl31.bin
ATF_PARAM	:= --atf $(ATF_BIN)
endif

ROOT_DIR		= $(PARTITION_DIR)/root
ROOT_BIN		= $(OUTPUT_TMP)/root.rsca.squashfs
# rootfs size without reserved space
ifndef PART_ROOTFS_RSVD_SIZE
ROOTFS_REAL_SIZE	= $(PART_ROOTFS_SIZE)
else
ROOTFS_REAL_SIZE	= $(shell expr $(PART_ROOTFS_SIZE) - $(PART_ROOTFS_RSVD_SIZE))
endif

ROOTFS_DEP		= $(KERNEL_BIN) $(ROOT_BIN)
ROOTFS_BIN		= $(OUTPUT_TMP)/rootfs.rsca.squashfs
rootfs: $(ROOTFS_BIN)
$(ROOTFS_BIN): $(ROOTFS_DEP)
	@$(ECHO) [$@] Changed Dep: $?
	$(call check_filesize, $(ROOTFS_DEP), $(ROOTFS_REAL_SIZE))
	$(RM) $@
	$(IMAGESIGN) --output=$(@) 						\
				 --imgsize=$(ROOTFS_REAL_SIZE) 		\
				 --uImage=$(KERNEL_BIN) 			\
				 --root=$(ROOT_BIN) $(ATF_PARAM)	\
				 --encrypt=none 					\
				 --check=crc32

$(ROOT_BIN): $(ROOT_DIR)
	$(RM) $@
	$(MK_SQUASHFS) $(ROOT_DIR) $@ $(SQUASHFS_PARAM)

ifeq ($(SUPPORT_PART_BACK), y)
BACK_BIN		= $(OUTPUT_TMP)/back.rsca.yaffs2
back: $(BACK_BIN)
$(BACK_BIN): $(PART_BACK_DIR)
	@$(ECHO) [$@] Changed Dep: $?
	$(RM) $@
#	$(MK_YAFFS2) $(PART_BACK_DIR) $@ $(YAFFS2_PAGE_SIZE) $(YAFFS2_ECC_TYPE)
	tr '\000' '\377' < /dev/zero | $(DD) of=$(BACK_BIN) bs=1K count=$(shell expr $(PART_BACK_SIZE) / 1024)
	$(CHMOD) 666 $(BACK_BIN)
endif

ifeq ($(SUPPORT_PART_STORAGE), y)
STORAGE_BIN		= $(OUTPUT_TMP)/storage.rsca.yaffs2
storage: $(STORAGE_BIN)
$(STORAGE_BIN): $(PART_STORAGE_DIR)
	@$(ECHO) [$@] Changed Dep: $?
	$(RM) $@
ifeq ($(wildcard $(PART_STORAGE_DIR)/*), )
	tr '\000' '\377' < /dev/zero | $(DD) of=$(STORAGE_BIN) bs=1K count=$(shell expr $(PART_STORAGE_SIZE) / 1024)
else
	$(MK_YAFFS2) $(PART_STORAGE_DIR) $@ $(YAFFS2_PAGE_SIZE) $(YAFFS2_ECC_TYPE)
endif
	$(CHMOD) 666 $(STORAGE_BIN)
endif

DEVINFO_BIN	:= $(OUTPUT_TMP)/devinfo.rsca.bin
devinfo: $(DEVINFO_BIN)
$(DEVINFO_BIN):
	$(DD) if=/dev/zero of=$@ bs=1K count=$(shell expr $(PART_DEVINFO_SIZE) / 1024)

# Common Compile MParts
define mk_mpart
MPART := $(strip $(1))

ifeq ($$(MPART_$(MPART)_BACKUP), y)
$(MPART)_INSTALL_DIR := $(DUAL_MPART_INSTALL_DIR)
MPART_$(MPART)_BIN := $$(MPART_$(MPART)_NAME).dual.$$(MPART_$(MPART)_FORMAT)
DUAL_TARGET_LIST += $$($(MPART)_INSTALL_DIR)/$$(MPART_$(MPART)_BIN)
DUAL_MPART_LIST += $(MPART)
else
$(MPART)_INSTALL_DIR := $(SINGLE_MPART_INSTALL_DIR)
MPART_$(MPART)_BIN := $$(MPART_$(MPART)_NAME).single.$$(MPART_$(MPART)_FORMAT)
SINGLE_TARGET_LIST += $$($(MPART)_INSTALL_DIR)/$$(MPART_$(MPART)_BIN)
SINGLE_MPART_LIST += $(MPART)
endif

$(MPART): $$($(MPART)_INSTALL_DIR)/$$(MPART_$(MPART)_BIN)
$$($(MPART)_INSTALL_DIR)/$$(MPART_$(MPART)_BIN): $(OUTPUT_TMP)/$$(MPART_$(MPART)_BIN)
	$(CP) $(OUTPUT_TMP)/$$(MPART_$(MPART)_BIN) $$($(MPART)_INSTALL_DIR)
	$(PUSHD) $$(PARTITION_DIR)/$$($(MPART)_INSTALL_DIR);	\
	$$(MD5SUM) $$(MPART_$(MPART)_BIN) > check_$$(MPART_$(MPART)_NAME).md5;	\
	$(POPD);

$(OUTPUT_TMP)/$$(MPART_$(MPART)_BIN):
	$$(MK_SQUASHFS) $$(MPART_$(MPART)_DIR) $$@ $$(SQUASHFS_PARAM)
endef

DATA_BIN	:= $(OUTPUT_TMP)/data.rsca.yaffs2
DATB_BIN	:= $(OUTPUT_TMP)/datb.rsca.yaffs2
SINGLE_MPART_INSTALL_DIR	:= $(PART_DATA_DIR)
DUAL_MPART_INSTALL_DIR		:= $(PART_DATB_DIR)
SINGLE_TARGET_LIST	:=
SINGLE_MPART_LIST	:=
DUAL_TARGET_LIST	:=
DUAL_MPART_LIST		:=

ifeq ($(SUPPORT_DUAL_BACKUP), y)
$(foreach MPART, $(MPART_LIST), $(eval $(call mk_mpart, $(MPART))))

data: $(DATA_BIN)
$(DATA_BIN): $(SINGLE_TARGET_LIST) $(PART_DATA_DIR)
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(MK_YAFFS2) $(PART_DATA_DIR) $@ $(YAFFS2_PAGE_SIZE) $(YAFFS2_ECC_TYPE)
	$(CHMOD) 666 $(DATA_BIN)
	$(call check_filesize, $(DATA_BIN), $(PART_DATA_SIZE))

datb: $(DATB_BIN)
$(DATB_BIN): $(DUAL_TARGET_LIST) $(PART_DATB_DIR)
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(MK_YAFFS2) $(PART_DATB_DIR) $@ $(YAFFS2_PAGE_SIZE) $(YAFFS2_ECC_TYPE)
	$(CHMOD) 666 $(DATB_BIN)
	$(call check_filesize, $(DATB_BIN), $(PART_DATB_SIZE))
else
$(foreach MPART, $(MPART_LIST), $(eval $(call mk_mpart, $(MPART))))

data: $(DATA_BIN)
$(DATA_BIN): $(SINGLE_TARGET_LIST) $(PART_DATA_DIR)
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(MK_YAFFS2) $(PART_DATA_DIR) $@ $(YAFFS2_PAGE_SIZE) $(YAFFS2_ECC_TYPE)
	$(CHMOD) 0666 $(DATA_BIN)
	$(call check_filesize, $(DATA_BIN), $(PART_DATA_SIZE))
endif
endif # NOT CLEAN

clean_datb: FORCE
	@$(ECHO) CLEAN    datb
	@$(RM) $(PARTITION_DIR)/datb/*

clean_data: FORCE
	@$(ECHO) CLEAN    data
	@$(RM) $(PARTITION_DIR)/data/*.md5
	@$(RM) $(PARTITION_DIR)/data/*.squashfs

clean_kernel: FORCE
	@$(ECHO) CLEAN    kernel
#	$(RM) $(PARTITION_DIR)/$(KERNEL_BIN)

clean_uboot: FORCE
	@$(ECHO) CLEAN    uboot
#	$(RM) $(PARTITION_DIR)/$(UBOOT_BIN)

clean_images: FORCE
	@$(ECHO) CLEAN    images
	@$(MAKE) clean_uboot
	@$(MAKE) clean_kernel
	@$(MAKE) clean_data
	@$(MAKE) clean_datb
	@$(RM) $(OUTPUT_TMP)/*


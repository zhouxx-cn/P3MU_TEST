
OUTPUT_TMP		:= $(PARTITION_DIR)/tmp

# Flash Config
JFFS2_ERASE_SIZE	= 0x8000
JFFS2_PARAM			= -l -e $(JFFS2_ERASE_SIZE)

ifeq ($(SUPPORT_CUSTOM_SQUASHFS_BS), true)
SQUASHFS_PARAM_GZ	= -b $(CUSTOM_SQUASHFS_BS) -noappend -all-root
else
SQUASHFS_PARAM_GZ	= -b 64K -noappend -all-root
endif
SQUASHFS_PARAM_XZ	= $(SQUASHFS_PARAM_GZ) -comp xz
SQUASHFS_PARAM		= $(SQUASHFS_PARAM_XZ) $(CUSTOM_SQUASHFS_PARAM)

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
ENV_TXT	:= $(PARTITION_DIR)/env.txt
env: $(ENV_BIN)
$(ENV_BIN): $(ENV_TXT) $(MKENVIMAGE)
	$(MKENVIMAGE) -s 0x10000 -p 0 -o $(ENV_BIN) $(ENV_TXT)

#
# 文件系统起始地址必须对齐
#
KERNEL_BIN		= $(PARTITION_DIR)/uImage
#KERNEL_SIZE		= $(shell expr 3 \* 1024 \* 1024)
#KERNEL_SZ_BYTE		= $(shell ls -l $(KERNEL_BIN) | awk '{print $$5}')

ROOT_DIR		= $(PARTITION_DIR)/root
ROOT_TMP_DIR	= $(PARTITION_DIR)/tmp/root
ROOT_BIN		= $(OUTPUT_TMP)/root.rsca.squashfs
#ROOT_SIZE		= $(shell expr 4 \* 1024 \* 1024)

ROOTFS_DEP		= $(KERNEL_BIN) $(ROOT_BIN)
ROOTFS_BIN		= $(OUTPUT_TMP)/rootfs.rsca.squashfs
ifeq ($(FLASH_TYPE), SPI_NOR)
ROOTFS_ALIGN	= $(shell expr 4 \* 1024)
endif
ifeq ($(FLASH_TYPE), SPI_NAND)
ROOTFS_ALIGN	= $(shell expr 128 \* 1024)
endif
rootfs: $(ROOTFS_BIN)
$(ROOTFS_BIN): $(ROOTFS_DEP)
	@$(ECHO) [$@] Changed Dep: $?
	$(call check_filesize, $(ROOTFS_DEP), $(PART_ROOTFS_SIZE))
	$(RM) $@
	$(IMAGESIGN) --output=$(@) \
				 --imgsize=$(PART_ROOTFS_SIZE) \
	             --uImage=$(KERNEL_BIN) \
				 --root=$(ROOT_BIN) \
				 --encrypt=none \
				 --check=crc32 \
				 --align=$(ROOTFS_ALIGN)

$(ROOT_TMP_DIR): $(ROOT_DIR)
	$(RM) $(ROOT_TMP_DIR)
	$(CP) $(ROOT_DIR) $(ROOT_TMP_DIR)
	@for mpart in $(MPART_IN_ROOTFS_LIST); do	\
		make $$mpart;							\
	done
	find $(ROOT_TMP_DIR) -name ".gitkeep" | xargs rm -rf;

$(ROOT_BIN): $(ROOT_TMP_DIR)
	$(RM) $@
	$(MK_SQUASHFS) $(ROOT_TMP_DIR) $@ $(SQUASHFS_PARAM)

ifeq ($(SUPPORT_PART_BACK), y)
BACK_BIN		= $(OUTPUT_TMP)/back.rsca.jffs2
back: $(BACK_BIN)
$(BACK_BIN): $(PART_BACK_DIR)
	@$(ECHO) [$@] Changed Dep: $?
	$(RM) $@
	tr '\000' '\377' < /dev/zero | $(DD) of=$(BACK_BIN) bs=1K count=$(shell expr $(PART_BACK_SIZE) / 1024)
	$(CHMOD) 666 $(BACK_BIN)
endif

ifeq ($(SUPPORT_PART_STORAGE), y)
STORAGE_BIN		= $(OUTPUT_TMP)/storage.rsca.jffs2
storage: $(STORAGE_BIN)
$(STORAGE_BIN): $(PART_STORAGE_DIR)
	@$(ECHO) [$@] Changed Dep: $?
	$(RM) $@
	tr '\000' '\377' < /dev/zero | $(DD) of=$(STORAGE_BIN) bs=1K count=$(shell expr $(PART_STORAGE_SIZE) / 1024)
	$(CHMOD) 666 $(STORAGE_BIN)
endif

DEVINFO_BIN	:= $(OUTPUT_TMP)/devinfo.rsca.bin
devinfo: $(DEVINFO_BIN)
$(DEVINFO_BIN):
	$(DD) if=/dev/zero of=$@ bs=1K count=$(shell expr $(PART_DEVINFO_SIZE) / 1024)

#####################################################################################
# Common Compile MParts
#####################################################################################
MPART_IN_ROOTFS_LIST	:=
MPART_MK_IMAGE_LIST		:=

# 将MPART进行分类
define classify_mpart
CMPART := $(strip $(1))

ifeq ($$(MPART_$(CMPART)_IN_ROOTFS), y)
MPART_IN_ROOTFS_LIST += $(CMPART)
else
MPART_MK_IMAGE_LIST += $(CMPART)
endif
endef

$(foreach CMPART, $(MPART_LIST), $(eval $(call classify_mpart, $(CMPART))))

# 将合入rootfs的分区拷贝到rootfs目录中
define mpart_to_rootfs
CPMPART := $(strip $(1))

$(CPMPART): $(ROOT_TMP_DIR) $$(MPART_$(CPMPART)_DIR) FORCE
	$(ECHO) Copy $(CPMPART) to rootfs
	$(CP) $$(MPART_$(CPMPART)_DIR) $(ROOT_TMP_DIR)/mpart/
endef

$(foreach CPMPART, $(MPART_IN_ROOTFS_LIST), $(eval $(call mpart_to_rootfs, $(CPMPART))))

# 通用的挂载分区制作脚本
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

DATA_BIN	:= $(OUTPUT_TMP)/data.rsca.jffs2
DATB_BIN	:= $(OUTPUT_TMP)/datb.rsca.jffs2
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
	$(MK_JFFS2) -d $(PART_DATA_DIR) $(JFFS2_PARAM) -o $@
	$(CHMOD) 666 $(DATA_BIN)
	$(call check_filesize, $(DATA_BIN), $(PART_DATA_SIZE))

datb: $(DATB_BIN)
$(DATB_BIN): $(DUAL_TARGET_LIST) $(PART_DATB_DIR)
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(MK_JFFS2) -d $(PART_DATB_DIR) $(JFFS2_PARAM) -o $@
	$(CHMOD) 666 $(DATB_BIN)
	$(call check_filesize, $(DATB_BIN), $(PART_DATB_SIZE))
else
$(foreach MPART, $(MPART_MK_IMAGE_LIST), $(eval $(call mk_mpart, $(MPART))))

data: $(DATA_BIN)
$(DATA_BIN): $(SINGLE_TARGET_LIST) $(PART_DATA_DIR)
	@$(ECHO) Compiling [$@], Changed Dep: $?
	$(MK_JFFS2) -d $(PART_DATA_DIR) $(JFFS2_PARAM) -o $@
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

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
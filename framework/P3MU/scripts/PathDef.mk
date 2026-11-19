#
# Common Path Define
#

export INC_DIR			:= $(ROOT_DIR)/include
export LIB_DIR			:= $(ROOT_DIR)/lib
export LIB_X86_DIR		:= $(ROOT_DIR)/lib-x86
export CONFIG_DIR		:= $(ROOT_DIR)/config
export APP_SRC_DIR		:= $(ROOT_DIR)/app
export SAMPLES_DIR		:= $(ROOT_DIR)/samples
export PARTITION_DIR	:= $(ROOT_DIR)/partition
export SOURCE_DIR		:= $(ROOT_DIR)/source
export TOOLS_DIR		:= $(ROOT_DIR)/tools
export FIRMWARE_ROOT_DIR	:= $(ROOT_DIR)/firmware
export RELEASE_ROOT_DIR		:= $(ROOT_DIR)/release
export 3RD_DIR				:= $(ROOT_DIR)/3rd

# Firmware
export FW_SKELETON_DIR	:= $(FIRMWARE_ROOT_DIR)/skeleton
export FW_PACKAGE_DIR	:= $(FIRMWARE_ROOT_DIR)/package
export FW_TOOLS_DIR		:= $(FIRMWARE_ROOT_DIR)/tools

# Source
export DRIVER_DIR		:= $(SOURCE_DIR)/drivers
export MODULES_DIR		:= $(SOURCE_DIR)/modules
export TOOLS_SRC_DIR	:= $(SOURCE_DIR)/tools

# Partition
export BOARD_TOOLS_DIR		:= $(PARTITION_DIR)/usr/bin
export PARTITION_ROOT_DIR	:= $(PARTITION_DIR)/root
export PARTITION_USR_DIR	:= $(PARTITION_DIR)/usr
export PARTITION_MODEL_DIR	:= $(PARTITION_DIR)/model
export PARTITION_WEB_DIR	:= $(PARTITION_DIR)/web
export PARTITION_DATA_DIR	:= $(PARTITION_DIR)/data
export PARTITION_DATB_DIR	:= $(PARTITION_DIR)/datb
export PARTITION_APP_DIR	:= $(PARTITION_DIR)/app
export PARTITION_TAR_DIR	:= $(PARTITION_DIR)/tar
export PARTITION_BIN_DIR	:= $(PARTITION_USR_DIR)/bin
export PARTITION_LIB_DIR	:= $(PARTITION_USR_DIR)/lib

# SKELETON BASE DIRS
export SKELETON_ROOT_DIR	:= $(FW_SKELETON_DIR)/root
export SKELETON_USR_DIR		:= $(FW_SKELETON_DIR)/usr
export SKELETON_MODEL_DIR	:= $(FW_SKELETON_DIR)/model
export SKELETON_WEB_DIR		:= $(FW_SKELETON_DIR)/web
export SKELETON_DATA_DIR	:= $(FW_SKELETON_DIR)/data
export SKELETON_DATB_DIR	:= $(FW_SKELETON_DIR)/datb
export SKELETON_STORAGE_DIR	:= $(FW_SKELETON_DIR)/storage
export SKELETON_APP_DIR		:= $(FW_SKELETON_DIR)/app
export SKELETON_TAR_DIR		:= $(FW_SKELETON_DIR)/tar
export SKELETON_STDLIB_DIR	:= $(FW_SKELETON_DIR)/stdlib
export SKELETON_CONFIG_DIR	:= $(FW_SKELETON_DIR)/configs/$(BOARD_TYPE)

# Tools
export PC_TOOLS_DIR		:= $(TOOLS_DIR)/pc
export DEBUG_TOOLS_DIR	:= $(TOOLS_DIR)/board

# Files
export CMD_DEF_MK		:= $(SCRIPTS_DIR)/CmdDef.mk
export BOARD_CFG_MK		:= $(SCRIPTS_DIR)/BoardCfg.mk
export PLATFORM_CFG_MK	:= $(SCRIPTS_DIR)/PlatformCfg.mk
export SRC_SPLINT_MK	:= $(SCRIPTS_DIR)/SrcSplint.mk
export AUTO_DEP_BIN_MK	:= $(SCRIPTS_DIR)/AutoDepBin.mk
export AUTO_DEP_LIB_MK	:= $(SCRIPTS_DIR)/AutoDepLib.mk
export BOARD_TYPE_CFG_MK	:= $(CONFIG_DIR)/$(BOARD_TYPE)/config.mk
export BOARD_PARTATION_MK	:= $(CONFIG_DIR)/$(BOARD_TYPE)/partition.mk
export BOARD_FUNC_DEF_MK	:= $(CONFIG_DIR)/BoardFunc.mk
export NAND_FLASH_FS_MK		:= $(SCRIPTS_DIR)/NandFlashFS.mk
export NOR_FLASH_FS_MK		:= $(SCRIPTS_DIR)/NorFlashFS.mk
export PRODUCT_MODE_MK		:= $(SCRIPTS_DIR)/ProductMode.mk
export GET_REVISION_SH		:= $(SCRIPTS_DIR)/GetRevision.sh
export VERSION_MK			:= $(SCRIPTS_DIR)/Version.mk

# Name
export DEPEND_FILE_NAME := dep.mk.rej

# Output File Name
export UPGRADE_BIN_NAME	:= upgrade.bin
export RESUME_BIN_NAME	:= resume.bin
export RESTORE_BIN_NAME	:= restore.bin
export APP_NAME			:= xapp

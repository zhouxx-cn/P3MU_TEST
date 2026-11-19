#
# BOARD-T5AI Partation Info
#

# Partition Support
export SUPPORT_DUAL_BACKUP	:= n
export SUPPORT_PART_STORAGE	:= n
export SUPPORT_PART_BACK	:= n
# 支持挂载分区合并入根文件系统，节省挂载分区的内存
export SUPPORT_MPART_IN_ROOTFS	:= y

# UBOOT
export PART_UBOOT_NAME	:= UBOOT
export PART_UBOOT_SIZE	:= $(shell expr 128 \* 1024)
export PART_UBOOT_FILE	:= u-boot.bin
export PART_UBOOT_FORMAT	:= binary
export PART_UBOOT_OFFSET	:= 0

# ENV
export PART_ENV_NAME	:= ENV
export PART_ENV_SIZE	:= $(shell expr 64 \* 1024)
export PART_ENV_FILE	:= env.bin
export PART_ENV_FORMAT	:= binary
export PART_ENV_OFFSET	:= $(shell expr $(PART_UBOOT_OFFSET) + $(PART_UBOOT_SIZE))

# ROOTFSA
export PART_ROOTFS_NAME	:= ROOTFS
export PART_ROOTFS_SIZE	:= $(shell expr 7424 \* 1024)
export PART_ROOTFS_FILE	:= rootfs.rsca.squashfs
export PART_ROOTFS_FORMAT	:= binary
export PART_ROOTFS_OFFSET	:= $(shell expr $(PART_ENV_OFFSET) + $(PART_ENV_SIZE))

# ROOTFSB
export PART_ROOTFSB_NAME	:= ROOTFSB
# Set 0 if Not Support
ifneq ($(SUPPORT_DUAL_BACKUP), y)
export PART_ROOTFSB_SIZE	:= 0
else
export PART_ROOTFSB_SIZE	:= $(PART_ROOTFS_SIZE)
endif
export PART_ROOTFSB_FILE	:= rootfs.rsca.squashfs
export PART_ROOTFSB_FORMAT	:= binary
export PART_ROOTFSB_OFFSET	:= $(shell expr $(PART_ROOTFS_OFFSET) + $(PART_ROOTFS_SIZE))

# DATA
export PART_DATA_NAME	:= DATA
export PART_DATA_SIZE	:= $(shell expr 512 \* 1024)
export PART_DATA_FILE	:= data.rsca.jffs2
export PART_DATA_DIR	:= data
export PART_DATA_FORMAT	:= jffs2
export PART_DATA_OFFSET	:= $(shell expr $(PART_ROOTFSB_OFFSET) + $(PART_ROOTFSB_SIZE))

# DATB
export PART_DATB_NAME	:= DATB
ifneq ($(SUPPORT_DUAL_BACKUP), y)
export PART_DATB_SIZE	:= 0
else
export PART_DATB_SIZE	:= $(shell expr 26 \* 1024 \* 1024)
endif
export PART_DATB_FILE	:= datb.rsca.jffs2
export PART_DATB_DIR	:= datb
export PART_DATB_FORMAT	:= jffs2
export PART_DATB_OFFSET	:= $(shell expr $(PART_DATA_OFFSET) + $(PART_DATA_SIZE))

# BACK
export PART_BACK_NAME	:= BACK
ifneq ($(SUPPORT_PART_BACK), y)
export PART_BACK_SIZE	:= 0
else
export PART_BACK_SIZE	:= $(shell expr 4 \* 1024 \* 1024)
endif
export PART_BACK_FILE	:= back.rsca.jffs2
export PART_BACK_DIR	:= back
export PART_BACK_FORMAT	:= jffs2
export PART_BACK_OFFSET	:= $(shell expr $(PART_DATB_OFFSET) + $(PART_DATB_SIZE))

# STORAGE
export PART_STORAGE_NAME	:= STORAGE
ifneq ($(SUPPORT_PART_STORAGE), y)
export PART_STORAGE_SIZE	:= 0
else
export PART_STORAGE_SIZE	:= $(shell expr 15 \* 1024 \* 1024)
endif
export PART_STORAGE_FILE	:= storage.rsca.jffs2
export PART_STORAGE_FORMAT	:= jffs2
export PART_STORAGE_OFFSET	:= $(shell expr $(PART_BACK_OFFSET) + $(PART_BACK_SIZE))

# DEVINFO
export PART_DEVINFO_NAME	:= DEVINFO
export PART_DEVINFO_SIZE	:= $(shell expr 64 \* 1024)
export PART_DEVINFO_FILE	:= devinfo.zero.bin
export PART_DEVINFO_FORMAT	:= binary
export PART_DEVINFO_OFFSET	:= $(shell expr $(PART_STORAGE_OFFSET) + $(PART_STORAGE_SIZE))

#
# DESC:			Mount-Partition info
# NAME:			partition file prefix, such as app.rsca.squashfs
# MAX-SIZE(KB):	partition max size, 0 for not-limited
# SOURCE:		source-dir relative to partition
# FORMAT:		partition format, just-support squashfs for now
# BACKUP:		support dual backup or not
#

export MPART_LIST	:= APP MODEL WEB USR

# APP
export MPART_APP_NAME		:= app
export MPART_APP_MAX_SIZE	:= 0
export MPART_APP_DIR		:= app
export MPART_APP_FORMAT		:= squashfs
ifeq ($(SUPPORT_DUAL_BACKUP), y)
export MPART_APP_BACKUP		:= n
else
export MPART_APP_BACKUP		:= n
endif
ifeq ($(SUPPORT_MPART_IN_ROOTFS), y)
export MPART_APP_IN_ROOTFS	:= y
else
export MPART_APP_IN_ROOTFS	:= n
endif

# MODEL
export MPART_MODEL_NAME			:= model
export MPART_MODEL_MAX_SIZE		:= 0
export MPART_MODEL_DIR			:= model
export MPART_MODEL_FORMAT		:= squashfs
ifeq ($(SUPPORT_DUAL_BACKUP), y)
export MPART_MODEL_BACKUP		:= n
else
export MPART_MODEL_BACKUP		:= n
endif
ifeq ($(SUPPORT_MPART_IN_ROOTFS), y)
export MPART_MODEL_IN_ROOTFS	:= y
else
export MPART_MODEL_IN_ROOTFS	:= n
endif

# WEB
export MPART_WEB_NAME		:= web
export MPART_WEB_MAX_SIZE	:= 0
export MPART_WEB_DIR		:= web
export MPART_WEB_FORMAT		:= squashfs
ifeq ($(SUPPORT_DUAL_BACKUP), y)
export MPART_WEB_BACKUP		:= y
else
export MPART_WEB_BACKUP		:= n
endif
ifeq ($(SUPPORT_MPART_IN_ROOTFS), y)
export MPART_WEB_IN_ROOTFS	:= y
else
export MPART_WEB_IN_ROOTFS	:= n
endif

# USR
export MPART_USR_NAME		:= usr
export MPART_USR_MAX_SIZE	:= 0
export MPART_USR_DIR		:= usr
export MPART_USR_FORMAT		:= squashfs
ifeq ($(SUPPORT_DUAL_BACKUP), y)
export MPART_USR_BACKUP		:= y
else
export MPART_USR_BACKUP		:= n
endif
ifeq ($(SUPPORT_MPART_IN_ROOTFS), y)
export MPART_USR_IN_ROOTFS	:= y
else
export MPART_USR_IN_ROOTFS	:= n
endif

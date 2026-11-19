#
# Platform Config
#

ifeq ($(PLATFORM),)
$(error "PLATFORM Not Define!")
endif 

# Avoid Conflict
PLAT_CFLAGS		= -Wall -ffunction-sections -fdata-sections -Werror=implicit-int  -Werror=return-type
PLAT_LDFLAGS	= -Wl,-gc-sections -std=c++11
PLAT_ARFLAGS	= -rc
LIB_CFLAGS		= 

ifeq ($(SUPPORT_FUNC_INSTRUMENT), true)
LIB_CFLAGS		+= -finstrument-functions
endif

ifeq ($(PLATFORM), HI3516CV610)
ARCH			:= arm
CROSS_COMPILE	?= aarch64-v01c01-linux-musl-
KERNEL_VERSION	?= 5.10
PLAT_CFLAGS		+= -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fstack-protector-strong -fPIE
PLAT_LDFLAGS	+= -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -pie
PLAT_FAMILY		:= HI3516CV610
ARCH_VERSION	:= armv7
HAVE_ATF		:= false
endif

ifeq ($(PLATFORM), HI3516CV608)
ARCH			:= arm
CROSS_COMPILE	?= aarch64-v01c01-linux-musl-
KERNEL_VERSION	?= 5.10
PLAT_CFLAGS		+= -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fstack-protector-strong -fPIE
PLAT_LDFLAGS	+= -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -pie
PLAT_FAMILY		:= HI3516CV610
ARCH_VERSION	:= armv7
HAVE_ATF		:= false
endif

ifeq ($(PLATFORM), X86)
PLAT_CFLAGS		+= -m32 -march=i386 -mtune=i386
PLAT_CXXFlAGS	+= -m32 -march=i386 -mtune=i386
CROSS_COMPILE	:= 
HOST			:= X86
export LIB_DIR = $(LIB_X86_DIR)
endif

ifneq ($(CROSS_COMPILE),)
HOST	?= $(patsubst %-,%,$(CROSS_COMPILE))
endif

ifeq ($(HOST),)
$(error ################# HOST Not Set! #####################)
endif

ifeq ($(STDLIB), uClibc)
PLAT_CFLAGS		+= -muclibc
PLAT_LDFLAGS	+= -muclibc
endif

ifeq ($(DEBUG), y)
PLAT_CFLAGS += -g3 -O0
else
ifneq ($(BOARD_TYPE), X86)
PLAT_CFLAGS += -O2
else
PLAT_CFLAGS += -Os
endif
endif

export ARCH ARCH_VERSION CROSS_COMPILE KERNEL_VERSION
export PLAT_CFLAGS PLAT_LDFLAGS PLAT_ARFLAGS LIB_CFLAGS

# Firmware Package
ifneq ($(BOARD_TYPE), X86)
export PACKAGE_DIR	= $(FW_PACKAGE_DIR)/$(SDK)

ifeq ($(MPP_VENC), true)
export PACKAGE_MPP_DIR		:= $(PACKAGE_DIR)/mpp_venc
else
export PACKAGE_MPP_DIR		:= $(PACKAGE_DIR)/mpp
endif

ifeq ($(PATCH), base)
export PACKAGE_MPP_OUT_DIR	:= $(PACKAGE_MPP_DIR)/out
else
ifeq ($(PATCH), )
export PACKAGE_MPP_OUT_DIR	:= $(PACKAGE_MPP_DIR)/out
else
export PACKAGE_MPP_OUT_DIR	:= $(PACKAGE_MPP_DIR)/out-$(PATCH)
endif
endif

export PACKAGE_LIB_DIR		:= $(PACKAGE_MPP_OUT_DIR)/lib
export PACKAGE_INC_DIR		:= $(PACKAGE_MPP_OUT_DIR)/include
export PACKAGE_TOOLS_DIR	:= $(PACKAGE_DIR)/tools

export UBOOT_ROOT_DIR		:= $(PACKAGE_DIR)/uboot
export KERNEL_ROOT_DIR		:= $(PACKAGE_DIR)/kernel
export BUSYBOX_ROOT_DIR		:= $(PACKAGE_DIR)/busybox
export DRIVERS_ROOT_DIR		:= $(PACKAGE_DIR)/drivers

# Source
export UBOOT_SOURCE_DIR		:= $(UBOOT_ROOT_DIR)/u-boot-$(UBOOT_VERSION)
export UBOOT_CONFIG_DIR		:= $(UBOOT_ROOT_DIR)/configs/$(BOARD_TYPE)
export KERNEL_SOURCE_DIR	:= $(KERNEL_ROOT_DIR)/linux-$(KERNEL_VERSION)
export KERNEL_CONFIG_DIR	:= $(KERNEL_ROOT_DIR)/configs/$(BOARD_TYPE)
export BUSYBOX_SOURCE_DIR	:= $(BUSYBOX_ROOT_DIR)/busybox-$(BUSYBOX_VERSION)
export BUSYBOX_CONFIG_DIR	:= $(BUSYBOX_ROOT_DIR)/configs/$(BOARD_TYPE)

# Third Part
export 3RD_BUILD_DIR	:= $(abspath $(3RD_DIR)/build/$(HOST))
export 3RD_LIB_DIR		:= $(3RD_BUILD_DIR)/lib
export 3RD_INC_DIR		:= $(3RD_BUILD_DIR)/include
export 3RD_BIN_DIR		:= $(3RD_BUILD_DIR)/bin
export 3RD_SBIN_DIR		:= $(3RD_BUILD_DIR)/sbin
export 3RD_MODULES_DIR	:= $(abspath $(3RD_DIR)/modules)
export 3RD_TOOLS_DIR	:= $(abspath $(3RD_DIR)/tools)

# Tag
export KERNEL_BUILD_TAG		:= $(KERNEL_SOURCE_DIR)/vmlinux
endif

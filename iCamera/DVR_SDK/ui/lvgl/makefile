#!/usr/bin/make -f
# =============================================================================
#  lvgl/makefile  —  把 LVGL 8.3 官方源码编译成 liblvgl.a 并安装到 SDK 静态库目录
#
#  用法：
#    1) 将 LVGL 8.3 源码放到本目录：lvgl/lvgl.h 与 lvgl/src/**（官方仓库结构）
#    2) make PRODUCT_TYPE=P3MU
#
#  说明：
#    - lv_conf.h 不放这里，统一在 ../ui_common/lv_conf.h，用 -DLV_CONF_INCLUDE_SIMPLE
#      + -I../ui_common 让 LVGL 找到它。
#    - 本目录位于 DVR_SDK/ui/lvgl，故 profile 与安装目录为上两级（../../）。
#    - 生成 liblvgl.a 安装到 lib_$(TYPE)-static/$(PRODUCT_TYPE)/，
#      my_gui/Makefile 自动链接。
# =============================================================================

include ../../profile

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
CROSS_COMPILE_PREFIX ?= arm-v01c02s020-linux-musleabi-
CPU_FLAGS := -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
endif

CC := $(CROSS_COMPILE_PREFIX)gcc
AR := $(CROSS_COMPILE_PREFIX)ar
RM := rm -rf

INSTALL_DIR := ../../lib_$(TYPE)-static/$(PRODUCT_TYPE)

INCLUDES := -I. -I../ui_common

CFLAGS := $(CPU_FLAGS) -Os -std=gnu11 -fno-strict-aliasing \
          -ffunction-sections -fdata-sections -w \
          -DLV_CONF_INCLUDE_SIMPLE

# 递归收集 LVGL 源码（官方 src 树）
SRCS := $(shell find ./src -name '*.c' 2>/dev/null)
OBJS := $(SRCS:.c=.o)

.PHONY: release clean install
release: liblvgl.a install

liblvgl.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	@echo "  [AR]   liblvgl.a ($(words $(OBJS)) objs)"

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

install: liblvgl.a
	@mkdir -p $(INSTALL_DIR)
	@cp -f liblvgl.a $(INSTALL_DIR)/
	@echo "  [CP]   liblvgl.a -> $(INSTALL_DIR)/"

clean:
	$(RM) $(OBJS) liblvgl.a

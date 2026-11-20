#!/usr/bin/make -f
# =============================================================================
#  ui/makefile  —  拇指相机 UI 聚合构建（单一产物 libui.a）
#
#  源码仍按 8 个子模块目录隔离（ui_common / ui_main / ui_video / ui_input /
#  ui_statusbar / ui_menu / ui_setting / ui_popup），本 makefile 收集所有模块的
#  .o 聚合成单个 libui.a，安装到 SDK 静态库目录，my_gui 自动链接。
#
#  第三方 LVGL 独立编译为 liblvgl.a（不并入 libui.a）：
#    - 第三方代码与自研库分离，便于单独升级 LVGL；
#    - 二者编译告警级别不同（LVGL 用 -w，自研用 -Wall -Wextra）。
#
#  用法（在 DVR_SDK/ui 下）：
#     make PRODUCT_TYPE=P3MU          # 编 liblvgl.a + libui.a 并安装
#     make libui PRODUCT_TYPE=P3MU    # 仅编自研 UI（需 lvgl 源码头文件在位）
#     make lvgl  PRODUCT_TYPE=P3MU    # 仅编 LVGL
#     make clean PRODUCT_TYPE=P3MU
# =============================================================================

include ../profile

# ---- 交叉工具链（P3MU = Hi3516CV608，与 my_gui/Makefile 一致）----
ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
CROSS_COMPILE_PREFIX ?= arm-v01c02s020-linux-musleabi-
CPU_FLAGS := -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
endif

CC := $(CROSS_COMPILE_PREFIX)gcc
AR := $(CROSS_COMPILE_PREFIX)ar
RM := rm -rf

LIBUI       := libui.a
INSTALL_DIR := ../lib_$(TYPE)-static/$(PRODUCT_TYPE)

# ---- 9 个自研模块目录（源码隔离不变；不含 lvgl、不含示例）----
UI_DIRS := ui_common ui_font ui_main ui_video ui_input ui_statusbar ui_menu ui_setting ui_popup

# ---- 头文件搜索路径：各模块 + LVGL 源码 + SDK(cJSON) ----
INCLUDES := $(addprefix -I,$(UI_DIRS)) -Ilvgl -I../NET -I../../modules/common/

# ---- 编译标志（LV_CONF_INCLUDE_SIMPLE 让 LVGL 用 "lv_conf.h"，经 -Iui_common 命中）----
CFLAGS := $(CPU_FLAGS) -Os -std=gnu11 -fno-strict-aliasing \
          -ffunction-sections -fdata-sections \
          -Wall -Wextra -Wno-unused-parameter \
          -DLV_CONF_INCLUDE_SIMPLE

# ---- 收集全部模块源码，排除参考示例 ui_integration_example.c ----
SRCS := $(wildcard $(addsuffix /*.c,$(UI_DIRS)))
SRCS := $(filter-out ui_main/ui_integration_example.c,$(SRCS))
OBJS := $(SRCS:.c=.o)

.PHONY: all libui lvgl install clean echo
all: lvgl libui install

# ---- 第三方 LVGL：独立静态库，自带 install ----
lvgl:
	$(MAKE) -C lvgl PRODUCT_TYPE=$(PRODUCT_TYPE)

# ---- 自研 UI：聚合单一静态库 ----
libui: $(LIBUI)

$(LIBUI): $(OBJS)
	$(AR) rcs $@ $(OBJS)
	@echo "  [AR]   $@  <= $(words $(OBJS)) objs / $(words $(UI_DIRS)) modules"

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

install: $(LIBUI)
	@mkdir -p $(INSTALL_DIR)
	@cp -f $(LIBUI) $(INSTALL_DIR)/
	@echo "  [CP]   $(LIBUI) -> $(INSTALL_DIR)/"

clean:
	$(RM) $(OBJS) $(LIBUI)
	-$(MAKE) -C lvgl clean PRODUCT_TYPE=$(PRODUCT_TYPE)

# ---- 调试：打印收集到的源码/目标，便于核对 ----
echo:
	@echo "UI_DIRS = $(UI_DIRS)"
	@echo "SRCS    = $(SRCS)"
	@echo "OBJS    = $(words $(OBJS)) files"
	@echo "INSTALL = $(INSTALL_DIR)"

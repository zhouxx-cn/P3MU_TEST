#!/usr/bin/make -f
# =============================================================================
#  ThumbCamDemo/makefile  —  拇指相机 UI 接入 demo, 聚合为 libThumbCamDemo.a
#
#  复用 DVR_SDK/common.mak(交叉工具链/产品宏/静态库安装)。额外补充:
#    - UI 框架头(ui_main/ui_video/ui_input/ui_common) 与 LVGL 头;
#    - openh264 头(baseline/include/3rd);
#    - 海思底层 LCD 头(baseline/include/mavi);
#    - 取流接口头(../MPU 的 MPU_CIU_Preview.h; MemRecord.h 由 common.mak 的 -I../include 命中)。
#
#  产物 libThumbCamDemo.a 自动安装到 ../lib_$(TYPE)-static/$(PRODUCT_TYPE)/,
#  my_gui/Makefile 会 glob 该目录下 *.a 并链接, 无需额外改动链接脚本。
#
#  用法(在 DVR_SDK/ThumbCamDemo 下, 或由 DVR_SDK/makefile 统一驱动):
#     make release PRODUCT_TYPE=P3MU
#     make clean   PRODUCT_TYPE=P3MU
# =============================================================================

include ../common.mak

# ---- UI 框架 + LVGL 头文件 ----
INCPATH += -I../ui/ui_common -I../ui/ui_main -I../ui/ui_video -I../ui/ui_input -I../ui/lvgl
# ---- 取流接口头(子码流循环缓冲区) ----
INCPATH += -I../MPU
# ---- openh264 + 海思底层 LCD 头 ----
INCPATH += -I../../../baseline/$(BASELINE_INC)/3rd -I../../../baseline/$(BASELINE_INC)/mavi

# ---- 让 LVGL 用 "lv_conf.h"(经 -I../ui/ui_common 命中) ----
CFLAGS  += -DLV_CONF_INCLUDE_SIMPLE
# ---- 引用的 LVGL/openh264 第三方头在严格告警(-Werror)下可能告警, 放宽为非致命 ----
CFLAGS  += -Wno-error

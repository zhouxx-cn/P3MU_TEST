#!/usr/bin/make -f
# =============================================================================
#  statemachine/makefile  —  拇指相机整机层次化状态机(HFSM)模块
#
#  聚合为 libstatemachine.a（目录名即库名，common.mak 自动推导）。
#  纯 C、零第三方依赖（仅 pthread），复用 DVR_SDK/common.mak
#  （交叉工具链/产品宏/静态库安装到 ../lib_$(TYPE)-static/$(PRODUCT_TYPE)/）。
#
#  文件说明：
#    hsm.h / hsm.c          开源 HSM 引擎（MIT，doc/hsm-master 拷贝适配）
#    thumcam_hsm.h/.c       框架封装：消息队列 + 单线程派发 + 系统定时器
#    thumcam_states.c       全部状态实现（硬件动作仅占位注释）
#
#  用法(在 DVR_SDK/statemachine 下, 或由 DVR_SDK/makefile 统一驱动):
#     make release PRODUCT_TYPE=P3MU
#     make clean   PRODUCT_TYPE=P3MU
# =============================================================================

include ../common.mak

# 状态机为纯逻辑模块，不依赖任何 SDK 头文件；common.mak 的 INCPATH 已够用
# CFLAGS += -Wno-error   # 如需放宽告警再打开

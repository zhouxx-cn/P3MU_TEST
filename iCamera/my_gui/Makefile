#!/usr/bin/make -f

include ../DVR_SDK/profile
include ../../baseline/baseline_version.mk

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
CFLAGS += -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS += -DACTIVE_CODE_SUPPORT -DDL_ACTIVE_CODE_SUPPORT
endif

CC := $(CROSS_COMPILE_PREFIX)gcc
CXX := $(CROSS_COMPILE_PREFIX)g++
STRIP := $(CROSS_COMPILE_PREFIX)strip
OBJCOPY := $(CROSS_COMPILE_PREFIX)objcopy

ifeq ($(WITH_AUTH),yes)
CFLAGS +=  -DAF_DEV_SUPPORT -DSOFT_ENCRYPTION
endif

ifeq ($(WITH_RTMP),yes)
CFLAGS += -D __USE_RTMP__
endif

ifeq ($(IR_CUT_CONTROL_IMPROVE),yes)
CFLAGS += -D__IR_CUT_CONTROL_IMPROVE__
endif

ifeq ($(DOUBLE_LIGHT_NO_LDR_CUSTOM),yes)
CFLAGS += -D__DOUBLE_LIGHT_NO_LDR_CUSTOM__
endif

ifeq ($(USE_P2P_PUSH),yes)
CFLAGS += -D__USE_P2P_PUSH__
endif

ifeq ($(USE_FTP),yes)
CFLAGS += -D__USE_FTP__
endif

ifeq ($(USE_EMAIL),yes)
CFLAGS += -D__USE_EMAIL__
endif

ifeq ($(NET_PROTOCOL_CONTROL),yes)
CFLAGS += -D__NET_PROTOCOL_CONTROL__
endif

ifeq ($(USE_CIRCLEQUEUE),yes)
CFLAGS += -D__USE_CIRCLEQUEUE__
endif

ifeq ($(IS_SUPPORT_WLAN),yes)
CFLAGS += -D__IS_SUPPORT_WLAN__
endif

ifeq ($(IS_SUPPORT_ZOOM),yes)
CFLAGS += -D__DIGITAL_ZOOM_DRAG_SUPPORT__
endif

ifeq ($(USE_MJPEG_CIRCLE_BUFFER),yes)
CFLAGS += -D__USE_MJPEG_CIRCLE_BUFFER__
endif

ifeq ($(IS_SUPPORT_APP_SHARE),yes)
CFLAGS += -D__IS_SUPPORT_APP_SHARE__
endif

ifeq ($(IS_SUPPORT_MULIT_CHANNEL),yes)
CFLAGS += -DSYSTEM_SUPPORT_CHANNEL=$(DEVICE_SUPPORT_CHANNEL_NUM)
endif

ifeq ($(IS_SUPPORT_HIS_BLUETOOTH), yes)
CFLAGS += -D__IS_SUPPORT_HIS_BLUETOOTH__
endif

ifeq ($(IS_SUPPORT_PTZ_BUILTIN),yes)
CFLAGS += -D__IS_SUPPORT_PTZ_BUILTIN__
endif

ifeq ($(IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK),yes)
CFLAGS += -D__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__
endif

ifeq ($(IS_SUPPORT_4G_MODULE),yes)
CFLAGS += -D__MODULE_4G_SUPPORT__
endif

ifeq ($(IS_SUPPORT_SAVE_TF_LOG),yes)
CFLAGS += -D__SUPPORT_SAVE_TF_LOG__
endif

ifeq ($(IS_SUPPORT_COMMON_DID_OPREATE),yes)
CFLAGS += -D__SUPPORT_COMMON_DID_OPREATE__
endif

ifeq ($(IS_SUPPORT_AGINGTEST),yes)
CFLAGS += -D__AGINGTEST__
endif

ifeq ($(IS_SUPPORT_P2P_MAXCLIENT),yes)
CFLAGS += -D__SUPPORT_P2P_MAXCLIENT__
endif

ifeq ($(SUPPORT_SD_CARD),yes)
CFLAGS += -D__SUPPORT_SD_CARD__
endif

ifeq ($(IS_SUPPORT_CLOUD_STORAGE),yes)
CFLAGS += -D__SUPPORT_CLOUD_STORAGE__
endif

ifeq ($(IS_SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES),yes)
CFLAGS += -D__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
endif

ifeq ($(STATIC_LINK_STDC),yes)
LDFLAGS += -static-libstdc++
CFLAGS += -D__STATIC_LINK_STDC__
endif

ifeq ($(IS_SUPPORT_4G_CLOUD_DOMAIN),yes)
CFLAGS += -D__IS_SUPPORT_4G_CLOUD_DOMAIN__
endif

ifeq ($(WITH_RTMP),yes)
CFLAGS += -D __USE_RTMP__
endif

ifeq ($(IS_SUPPORT_AIC_BLUETOOTH),yes)
CFLAGS += -D__IS_SUPPORT_AIC_BLUETOOTH__
endif

ifeq ($(IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM),yes)
CFLAGS += -D__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
endif

CFLAGS += -D__PRODUCT_${PRODUCT_TYPE}__

ifeq ($(DEBUG), true)
CFLAGS += -g -DNDEBUG
else 
CFLAGS += -Os -DNDEBUG -w
endif

CFLAGS+= -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR

# C 编译器特有的编译标志针对 C++ 无效,会有警告
CCFLAGS += -Werror=implicit-int -Werror=implicit-function-declaration -Wbad-function-cast

#添加如下编译参数
CFLAGS += -Werror -Wall -Werror=return-type -Wpointer-arith -Wundef -Wcast-qual\
          -ffunction-sections -fdata-sections -fno-exceptions

LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--start-group
LDFLAGS += -lrt -lpthread -lm -ldl
LDFLAGS += -Xlinker -no-enum-size-warning

# 库搜索路径
#本地库搜索路径
LIBDIR += -L../DVR_SDK/lib_${TYPE}-static/${PRODUCT_TYPE}
#协议库搜索路径
#LIBDIR += -L../DVR_SDK/lib_${TYPE}-static/${PRODUCT_TYPE}/a
LIBDIR += $(shell find ../DVR_SDK/lib_${TYPE}-static/${PRODUCT_TYPE}/a -type d 2>/dev/null | sed 's/^/-L/')
#算法库搜索路径
LIBDIR += -L../../aisdk/$(AISDK_LIB_PATH)
#平台库（海思）搜索路径
LIBDIR += -L../../baseline/${BASELINE_SDK_LIB}
#基线库搜索路径
LIBDIR += -L../../baseline/${BASELINE_LIB}

#静态库集合
#本地库
LIBS-static = $(shell find ../DVR_SDK/lib_${TYPE}-static/${PRODUCT_TYPE} -maxdepth 1 -name "*.a" | sort | sed -e s/.*lib/-l/g -e s/.a$$//)
#协议库
LIBS-static += $(shell find ../DVR_SDK/lib_${TYPE}-static/${PRODUCT_TYPE}/a -name "*.a" | sort | sed -e s/.*lib/-l/g -e s/.a$$//)
#平台库（海思）
LIBS-static += $(shell find ../../baseline/${BASELINE_SDK_LIB} -maxdepth 1 -name "*.a" | sort | sed -e s/.*lib/-l/g -e s/.a$$//)
#基线库
LIBS-static += $(shell find ../../baseline/${BASELINE_LIB} -maxdepth 1 -name "*.a" | sort | sed -e s/.*\\/lib/-l/g -e s/.a$$//)
#算法库
LIBS-static += $(shell find ../../aisdk/$(AISDK_LIB_PATH) -maxdepth 1 -name "*.a" | sort | sed -e s/.*\\/lib/-l/g -e s/.a$$//)

# P4/P4U/P3MU 产品不需要图像调试动态库
ifneq ($(filter W63F P4 P4U P3MU,$(PRODUCT_TYPE)),)
    LIBS-dynamic :=
else
    LIBS-dynamic := -lhi_mpi_ae -lhi_mpi_awb
endif

LDFLAGS += -Wl,-Bstatic $(LIBS-static)
LDFLAGS += -Wl,-Bdynamic $(LIBS-dynamic)
LIBPAH += ${LIBDIR}

TARGET=xrscam
SYMBOL=symbol.xrscam
TEST_LIB_PATH += ../test_dir/lib/lib_${TYPE}/${PRODUCT_TYPE}/
TEST_OBJ_PATH += ../test_dir/obj/obj_${TYPE}
INSTALL=install -cv -m 777

INCLUDES=   -I../DVR_SDK \
            -I../DVR_SDK/include \
            -I../DVR_SDK/include/wolfssl \
            -I../DVR_SDK/PUB \
            -I../modules/include \
            -I../DVR_SDK/PAR \
            -I../DVR_SDK/NET \
            -I../DVR_SDK/PUB_BORAD \
			-I../DVR_SDK/ui/lvgl \
            -I../DVR_SDK/ui/ui_main/ \
            -I../DVR_SDK/ui/ui_video/ \
            -I../DVR_SDK/ui/ui_menu/ \
            -I../DVR_SDK/ui/ui_common/ \
            -I../DVR_SDK/ui/ui_input/ \
            -I../DVR_SDK/ThumbCamDemo/ \
            -I../modules/common 

INCLUDES +=  -I../../baseline/$(BASELINE_INC)/mauth \
        -I../../baseline/$(BASELINE_INC)/mavi \
        -I../../baseline/$(BASELINE_INC)/mbase \
        -I../../baseline/$(BASELINE_INC)/misc \
        -I../../baseline/$(BASELINE_INC)/misp \
        -I../../baseline/$(BASELINE_INC)/tools \
        -I../../baseline/$(BASELINE_INC)/utils \
        -I../../baseline/$(BASELINE_INC)/humanTrack \
        -I../../baseline/$(BASELINE_INC)/3rd

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

LDFLAGS += -Wl,--end-group

all:$(TARGET)
$(TARGET):$(OBJS)
	echo LIBPAH=${LIBPAH}
	#$(CXX) -g -o $@ $^ mem_wrap.o $(INCLUDES) $(LIBPAH) $(LDFLAGS) -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,calloc -Wl,-wrap,realloc -Wl,-wrap,valloc 
ifeq ($(DEBUG), true)
	#@#$(STRIP) $(TARGET)
	$(CXX) -g -o $@ $^ -Wl,-Map=edvr.map  -Wl,--dynamic-list=dl.txt $(INCLUDES) $(LIBPAH) $(LDFLAGS)
	$(OBJCOPY) --only-keep-debug $(TARGET) $(SYMBOL)
	$(OBJCOPY) --strip-debug --strip-unneeded $(TARGET)
	$(OBJCOPY) --add-gnu-debuglink=$(SYMBOL) $(TARGET)
else 
	$(CXX) -Os -o $@ $^ -Wl,-Map=edvr.map  -Wl,--dynamic-list=dl.txt $(INCLUDES) $(LIBPAH) $(LDFLAGS) 
	$(STRIP) $(TARGET)
endif
	@ls -lh $(TARGET)
	@md5sum $(TARGET)
	@echo -e "\e[1;35mCompile over! start to copy files to test_dir ...\e[0m"
	@rm -rf ${TEST_LIB_PATH} ${TEST_OBJ_PATH}
	@mkdir -p ${TEST_LIB_PATH} ${TEST_OBJ_PATH}
	@cp -rfd ../DVR_SDK/lib_${TYPE}-static/${PRODUCT_TYPE}/* ${TEST_LIB_PATH}
	@cp -rfd ${OBJS}  ${TEST_OBJ_PATH}
	@echo -e "\e[1;35mCopy files to test_dir over!\e[0m"

%.o:%.c
ifeq ($(DEBUG), true)
	$(CXX) -c -g -o $@ $^ $(INCLUDES) $(CFLAGS)
else
	$(CXX) -c -o $@ $^ $(INCLUDES) $(CFLAGS)
endif
echo:
	software===============${LIBPAH}
	
.PHONY :clean
clean:
	-rm -rf $(TARGET) ${OBJS} edvr.map

install:
	install $(TARGET) ../../framework/$(PRODUCT_TYPE)/partition/app/bin/

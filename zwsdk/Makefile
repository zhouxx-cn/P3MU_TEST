
include ./profile
include ../aisdk/aisdk_version.mk
include ../baseline/baseline_version.mk

CROSS_COMPILE = arm-v01c02s020-linux-musleabi-

CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip

XRS_PATH := -I./include/xrs/mauth \
	-I../baseline/$(BASELINE_VERSION)/include/mavi \
	-I../baseline/$(BASELINE_VERSION)/include/mbase \
	-I../baseline/$(BASELINE_VERSION)/include/misc \
	-I../baseline/$(BASELINE_VERSION)/include/misp \
	-I../baseline/$(BASELINE_VERSION)/include/tools \
	-I../baseline/$(BASELINE_VERSION)/include/utils \
	-I../baseline/$(BASELINE_VERSION)/include/mauth \
	-I../baseline/$(BASELINE_VERSION)/include/3rd \
	-I../baseline/$(BASELINE_VERSION)/include/driver \

ALGO_PATH := -I../aisdk/$(AISDK_INC_PATH)

INC_PATH += $(XRS_PATH) \
            $(ALGO_PATH) \
            -I./include \
            -I./include/common \
            -I../iCamera/modules/common \
            -I../iCamera/DVR_SDK/include \
            -I../iCamera/DVR_SDK/PUB

CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 \
        -fno-aggressive-loop-optimizations $(INC_PATH)

CFLAGS += -Werror -Wall -Werror=return-type -Wpointer-arith \
          -Werror=implicit-int -Werror=implicit-function-declaration \
          -ffunction-sections -fdata-sections

ifeq ($(AudioEncodeType),aac)
CFLAGS += -DUSE_AAC_ENCODE
endif

CFLAGS += -D__PRODUCT_$(PRODUCT_TYPE)__

# -laacenc
ifeq ($(DEBUG),true)
	CFLAGS += -g
else
 	CFLAGS += -Os
endif
RFLAGS += $(CFLAGS)

LFLAGS += -laudio_apps -laudio_core -lvideo_apps -lvideo_core -lpthread -liconv_pic

ifeq ($(SUPPORT_WIFI_FUNC),yes)
CFLAGS += -D__SUPPORT_WIFI_FUNC__
endif

ifeq ($(PIC_CUSTOM_COLOR_TONE),yes)
CFLAGS += -DPIC_CUSTOM_COLOR_TONE
endif

ifeq ($(USE_CIRCLEQUEUE),yes)
CFLAGS += -D__USE_CIRCLEQUEUE__
endif

ifeq ($(IS_SUPPORT_AGINGTEST),yes)
CFLAGS += -D__AGINGTEST__
endif

ifeq ($(IS_SUPPORT_4G_MODULE),yes)
CFLAGS += -D__MODULE_4G_SUPPORT__
endif

ifeq ($(ENABLE_ISP_IMAGE_DEBUG),yes)
CFLAGS += -D__ISP_IMAGE_DEBUG__
endif

ifeq ($(IS_SUPPORT_ZOOM),yes)
CFLAGS += -D__DIGITAL_ZOOM_DRAG_SUPPORT__
endif

ifeq ($(IS_SUPPORT_SMARTP),yes)
CFLAGS += -D__SMARTP_SUPPORT__
endif

ifeq ($(IS_SUPPORT_MULIT_CHANNEL),yes)
CFLAGS += -DSYSTEM_SUPPORT_CHANNEL=$(DEVICE_SUPPORT_CHANNEL_NUM)
endif

ifeq ($(IS_SUPPORT_ADUJUST_SENSOR_FRAME),yes)
CFLAGS += -D__IS_SUPPORT_ADUJUST_SENSOR_FRAME__
endif

ifeq ($(IS_SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES),yes)
CFLAGS += -D__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
endif

ifeq ($(IS_SUPPORT_COMMON_DID_OPREATE),yes)
CFLAGS += -D__SUPPORT_COMMON_DID_OPREATE__
endif

ifeq ($(IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM),yes)
CFLAGS += -D__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
endif

ifeq ($(IS_SUPPORT_12FPS),yes)
CFLAGS += -D__IS_SUPPORT_12FPS__
endif

ifeq ($(IS_SUPPORT_ONE_CLICK_CALL),yes)
CFLAGS += -D__ONE_CLICK_CALL__
endif

ifeq ($(IS_SUPPORT_HS_CRY_DETECT),yes)
CFLAGS += -D__IS_SUPPORT_HS_CRY_DETECT__
endif

ifeq ($(IS_SUPPORT_JZ_CRY_DETECT),yes)
CFLAGS += -D__IS_SUPPORT_JZ_CRY_DETECT__
endif

ifeq ($(IS_SUPPORT_LCD_MANAGER),yes)
CFLAGS += -D__IS_SUPPORT_LCD_MANAGER__
endif

ACTUAL_HD_ALGO_CHANNEL_NUM := $(if $(filter yes,$(IS_SUPPORT_MULIT_CHANNEL_HD_ALGO)),$(DEVICE_SUPPORT_HD_ALGO_CHANNEL_NUM),1)
CFLAGS += -DSYSTEM_SUPPORT_HD_ALGO_CHANNEL=$(ACTUAL_HD_ALGO_CHANNEL_NUM)

SRC_DIRS := $(shell find . -maxdepth 4 -type d)
CFILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,%.o, $(CFILES))
CPPFILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
CPPOBJS = $(patsubst %.cpp,%.o, $(CPPFILES))

# 确定实际值
ACTUAL_CHANNEL_NUM := $(if $(filter yes,$(IS_SUPPORT_MULIT_CHANNEL)),$(DEVICE_SUPPORT_CHANNEL_NUM),1)

RELEASE_DIR := RELEASE
TARGET := libzwsdk.so
TARGET_STATIC := libzwsdk.a
RELEASE_TARGET := $(RELEASE_DIR)/$(TARGET)
RELEASE_TARGET_STATIC := $(RELEASE_DIR)/$(TARGET_STATIC)
.PHONY : clean

%/.:
	mkdir -p $*

release:$(TARGET_STATIC)

$(TARGET):$(OBJS) $(CPPOBJS)
	$(CC) -g -o $@  $^ -shared -fPIC -static $(LIBS_PATH)  $(LFLAGS)
	$(STRIP) $@
	md5sum $@
	echo "share lib release"

$(TARGET_STATIC): $(OBJS) $(CPPOBJS)
	${AR} crv $@ $^
	md5sum $@
	echo "static lib release"

$(RELEASE_DIR)/%.o:%.c
	$(CC) $(RFLAGS) -c $^ -o $@

$(RELEASE_DIR)/%.o:%.cpp
	$(CPP) $(RFLAGS) -c $^ -o $@

clean:
	rm -rf $(TARGET) $(TARGET_STATIC) $(RELEASE_DIR) $(OBJS) $(CPPOBJS)

install:
	install libzwsdk.a ../iCamera/DVR_SDK/lib_$(TYPE)-static/$(PRODUCT_TYPE)/a

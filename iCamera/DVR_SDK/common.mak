#!/usr/bin/make -f

#enable this macro only when you want to build it singly

include ../profile
include ../printLevel
include ../../../baseline/baseline_version.mk

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations
CFLAGS+= -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR -DNO_USE_SQLITE3 -DDAOEX -DKDB_DEFAULT_RESOLVER=wresolver
CFLAGS += -DACTIVE_CODE_SUPPORT -DDL_ACTIVE_CODE_SUPPORT -DDEBUG_NOT_USE_OPL_LIB
CFLAGS += -Werror=return-type
LFLAGS += -fno-exceptions
endif

ifeq ($(WITH_RTMP),yes)
CFLAGS += -D __USE_RTMP__ -D ACTIVE_CODE_SUPPORT
endif
ifeq ($(WITH_AUTH),yes)
CFLAGS += -DAF_DEV_SUPPORT  -DSOFT_ENCRYPTION
endif
ifeq ($(USE_HOSTS_PARSER),yes)
CFLAGS += -DUSE_HOSTS_PARSER
endif
ifeq ($(IR_CUT_AUTO_MANUAL),yes)
CFLAGS += -D__IR_CUT_AUTO_MANUAL__
endif
ifeq ($(IS_SOPPORT_ZOOM_OSD),yes)
CFLAGS += -D__IS_SOPPORT_ZOOM_OSD__
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
ifeq ($(IS_SUPPORT_AUTH_V2),yes)
CFLAGS += -D__AUTH_V2__
endif
ifeq ($(USE_CIRCLEQUEUE),yes)
CFLAGS += -D__USE_CIRCLEQUEUE__
endif
ifeq ($(IS_SUPPORT_WLAN),yes)
CFLAGS += -D__IS_SUPPORT_WLAN__
endif
ifeq ($(CLOSE_SWAP),yes)
CFLAGS += -D__CLOSE_SWAP__
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
ifeq ($(IS_SUPPORT_SMARTP),yes)
CFLAGS += -D__DIGITAL_SUPPORT_SMARTP__
endif
ifeq ($(IS_SUPPORT_ZOOM),yes)
CFLAGS += -D__DIGITAL_ZOOM_DRAG_SUPPORT__
endif
ifeq ($(SUPPORT_SD_CARD),yes)
CFLAGS += -D__SUPPORT_SD_CARD__
else
CFLAGS += -DDEBUG_NOT_USE_STR_LIB
endif
ifeq ($(SUPPORT_128G_DISK),yes)
CFLAGS += -D__SUPPORT_128G_DISK__
endif
ifeq ($(SUPPORT_UPGRADE_V2),yes)
CFLAGS += -D__UPGRADE_V2__
endif
ifeq ($(IS_SUPPORT_UBOOT_RECOVER),yes)
CFLAGS += -D__UBOOT_RECOVER__
endif
ifeq ($(USE_APPLY_DID_MANUAL),yes)
CFLAGS += -D__USE_APPLY_DID_MANUAL__
endif
ifeq ($(USE_STRONG_PW),yes)
CFLAGS += -D__USE_STRONG_PW__
endif
ifeq ($(IS_SUPPORT_P6SMESSAGE),yes)
CFLAGS += -D__IS_SUPPORT_P6SMESSAGE__
endif
ifeq ($(IS_SUPPORT_AGINGTEST),yes)
CFLAGS += -D__AGINGTEST__
endif
ifeq ($(IS_SUPPORT_DYNAMIC_GRAPHICS),yes)
CFLAGS += -D__IS_SUPPORT_DYNAMIC_GRAPHICS__
endif
ifeq ($(IS_SUPPORT_P2P_MAXCLIENT),yes)
CFLAGS += -D__SUPPORT_P2P_MAXCLIENT__
endif
ifeq ($(IS_SUPPORT_CLOUDWORLD),yes)
CFLAGS += -D__IS_SUPPORT_CLOUDWORLD__
endif
ifeq ($(IS_SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES),yes)
CFLAGS += -D__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
endif
ifeq ($(IS_SUPPORT_COMMON_DID_OPREATE),yes)
CFLAGS += -D__SUPPORT_COMMON_DID_OPREATE__
endif
ifeq ($(IS_SUPPORT_4G_CLOUD_DOMAIN),yes)
CFLAGS += -D__IS_SUPPORT_4G_CLOUD_DOMAIN__
endif
ifeq ($(IS_SUPPORT_12FPS),yes)
CFLAGS += -D__IS_SUPPORT_12FPS__
endif
ifeq ($(IS_SUPPORT_ONE_CLICK_CALL),yes)
CFLAGS += -D__ONE_CLICK_CALL__
endif
ifeq ($(IS_SUPPORT_MAS_CALIBRATION),yes)
CFLAGS += -D__IS_SUPPORT_MAS_CALIBRATION__
endif
ifeq ($(IS_SUPPORT_REMINDER_SCHEDULE),yes)
CFLAGS += -D__IS_SUPPORT_REMINDER_SCHEDULE__
endif
ifeq ($(IS_SUPPORT_LONGAWAYHOME),yes)
CFLAGS += -D__IS_SUPPORT_LONGAWAYHOME__
endif
ifeq ($(IS_SUPPORT_AIC_BLUETOOTH),yes)
CFLAGS += -D__IS_SUPPORT_AIC_BLUETOOTH__
endif
ifeq ($(IS_SUPPORT_LCD_MANAGER),yes)
CFLAGS += -D__IS_SUPPORT_LCD_MANAGER__
endif
ifeq ($(IS_SUPPORT_TRACK_HUMAN_FACE),yes)
CFLAGS += -D__IS_SUPPORT_TRACK_HUMAN_FACE__
endif
ifeq ($(IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM),yes)
CFLAGS += -D__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
endif

CFLAGS += -DDEVICE_SUPPORT_PTZ_NUM=$(DEVICE_SUPPORT_PTZ_NUM)

CFLAGS += -D__PRODUCT_$(PRODUCT_TYPE)__

ACTUAL_HD_ALGO_CHANNEL_NUM := $(if $(filter yes,$(IS_SUPPORT_MULIT_CHANNEL_HD_ALGO)),$(DEVICE_SUPPORT_HD_ALGO_CHANNEL_NUM),1)
CFLAGS += -DSYSTEM_SUPPORT_HD_ALGO_CHANNEL=$(ACTUAL_HD_ALGO_CHANNEL_NUM)

LIBDIR=../lib_${TYPE}

CC=$(CROSS_COMPILE_PREFIX)gcc
CXX=$(CROSS_COMPILE_PREFIX)g++
AR=$(CROSS_COMPILE_PREFIX)ar
STRIP=$(CROSS_COMPILE_PREFIX)strip
RM=rm -rf
INSTALL=install -cv -m 777

# C 编译器特有的编译标志针对 C++ 无效,会有警告
CCFLAGS += -Werror=implicit-int -Werror=implicit-function-declaration -Wbad-function-cast
          
#添加如下编译参数
CFLAGS += -Werror -Wall -Werror=return-type -Wpointer-arith -Wundef -Wcast-qual\
          -ffunction-sections -fdata-sections -fno-exceptions

ifeq ($(STATIC), true)
ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
LIBDIR=../lib_${TYPE}-static/$(PRODUCT_TYPE)/
else
LIBDIR=../lib_${TYPE}-static
endif
BIN=lib$(LIBNAME).a
RELEASE_DIR=RELEASE-$(ARCH)-static
else
LIBDIR=../lib_${TYPE}
BIN=lib$(LIBNAME).so
RELEASE_DIR=RELEASE-$(ARCH)
CFLAGS+= -fPIC
endif

ifeq ($(DEBUG), true)
RFLAGS+= $(CFLAGS) -g -DNDEBUG
else
RFLAGS+= $(CFLAGS) -Os -DNDEBUG
endif

LFLAGS+= -lpthread

INCPATH+= -I.. -I../include -I../AVL -I../PUB -I../PUB_BORAD -I../PAR -I../RNL -I../../modules/include -I../NET/wolfssl -I../NET -I../../modules/CircleQueue/include -I../../modules/common
INCPATH+= -I../../../baseline/$(BASELINE_INC)/misc -I../../../baseline/$(BASELINE_INC)/mbase -I../../../baseline/$(BASELINE_INC)/mauth

MPATH=$(shell pwd)
LIBNAME=$(notdir $(MPATH))

DEBUG_DIR=DEBUG-$(ARCH)
TEST_DIR=test

DEBUG_TARGET=$(DEBUG_DIR)/$(BIN)
RELEASE_TARGET=$(RELEASE_DIR)/$(BIN)
TARGETS=$(DEBUG_TARGET) $(RELEASE_TARGET)

SRC_DIRS := $(shell find . -maxdepth 3 -type d )
CPPFILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS+= $(patsubst %.cpp,%.o, $(CPPFILES))


.PHONY:debug relase clean build test install

release:$(RELEASE_DIR)/. ${LIBDIR}/. $(PROTO_TARGET) $(RELEASE_TARGET)
debug:$(DEBUG_DIR)/. ${LIBDIR}/. $(PROTO_TARGET) $(DEBUG_TARGET)

clean:
	$(RM) ${OBJECTS} ${TARGETS}
	$(RM) RELEASE-* DEBUG-* Release

# create default build dirs
build:
%/.:
	mkdir -m 777 -p $*
	cd $(MPATH)/$* && mkdir -m 777 -p $(SRC_DIRS)

test:
	$(CXX) $(TEST_DIR)/*.cpp -o $(TEST_DIR)/$(LIBNAME)_test $(TESTFLAGS) $(INCPATH) $(LIBPATH) $(LFLAGS) -l$(LIBNAME)
	$(INSTALL) $(TEST_DIR)/$(LIBNAME)_test

$(DEBUG_TARGET):$(addprefix $(DEBUG_DIR)/,$(OBJECTS))
	$(CXX) -o $@ $^ $(LFLAGS) $(LIBPATH) $(DFLAGS)
	$(INSTALL) $@ ${LIBDIR}

ifeq ($(STATIC), true)
$(RELEASE_TARGET):$(addprefix $(RELEASE_DIR)/,$(OBJECTS))
	@$(AR) crD $@ $^
	$(INSTALL) $@ ${LIBDIR}
#	$(INSTALL) $@ ${FS_LIBDIR}
else
$(RELEASE_TARGET):$(addprefix $(RELEASE_DIR)/,$(OBJECTS))
	$(CXX) -shared -o $@ $^ $(LFLAGS) $(LIBPATH) $(RFLAGS)
	$(STRIP) $@
	$(INSTALL) $@ ${LIBDIR}
#	$(INSTALL) $@ ${FS_LIBDIR}
endif

$(DEBUG_DIR)/%.o:%.c
	$(CC) -c $^ -o $@ $(DFLAGS) $(CCFLAGS) $(INCPATH)

$(RELEASE_DIR)/%.o:%.c
	$(CC) -c $^ -o $@ $(RFLAGS) $(CCFLAGS) $(INCPATH)

$(DEBUG_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(DFLAGS) $(INCPATH)

$(RELEASE_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(RFLAGS) $(INCPATH)

include ../../DVR_SDK/profile
include ../../../baseline/baseline_version.mk

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
endif

CFLAGS+= -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS+= -D__FAT32__

ifeq ($(WITH_RTMP),yes)
CFLAGS += -D __USE_RTMP__ -D ACTIVE_CODE_SUPPORT
endif
ifeq ($(WITH_AUTH),yes)
CFLAGS += -D ACTIVE_CODE_SUPPORT -DAF_DEV_SUPPORT -DDL_ACTIVE_CODE_SUPPORT -DSOFT_ENCRYPTION
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
ifeq ($(SUPPORT_SDCARDHOTSWAP),yes)
CFLAGS += -D__SUPPORT_SDCARDHOTSWAP__
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
ifeq ($(USE_CIRCLEQUEUE),yes)
CFLAGS += -D__USE_CIRCLEQUEUE__
endif
ifeq ($(CLOSE_SWAP),yes)
CFLAGS += -D__CLOSE_SWAP__
endif

ifeq ($(USE_MJPEG_CIRCLE_BUFFER),yes)
CFLAGS += -D__USE_MJPEG_CIRCLE_BUFFER__
endif

ifeq ($(IS_SUPPORT_MULIT_CHANNEL),yes)
CFLAGS += -DSYSTEM_SUPPORT_CHANNEL=$(DEVICE_SUPPORT_CHANNEL_NUM)
endif
ifeq ($(IS_SUPPORT_PTZ_BUILTIN),yes)
CFLAGS += -D__IS_SUPPORT_PTZ_BUILTIN__
ifeq ($(IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK),yes)
CFLAGS += -D__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__
endif
endif

ifeq ($(IS_SUPPORT_4G_MODULE),yes)
CFLAGS += -D__MODULE_4G_SUPPORT__
endif

ifeq ($(IS_SUPPORT_SAVE_TF_LOG),yes)
CFLAGS += -D__SUPPORT_SAVE_TF_LOG__
endif

ifeq ($(SUPPORT_SD_CARD),yes)
CFLAGS += -D__SUPPORT_SD_CARD__
endif

ifeq ($(SUPPORT_128G_DISK),yes)
CFLAGS += -D__SUPPORT_128G_DISK__
endif

ifeq ($(IS_SUPPORT_AGINGTEST),yes)
CFLAGS += -D__AGINGTEST__
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

CFLAGS += -D__PRODUCT_$(PRODUCT_TYPE)__
CFLAGS += -DDEVICE_SUPPORT_PTZ_NUM=$(DEVICE_SUPPORT_PTZ_NUM)

CFLAGS+= -D_POSIX_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE

CC=$(CROSS_COMPILE_PREFIX)gcc
CXX=$(CROSS_COMPILE_PREFIX)g++
STRIP=$(CROSS_COMPILE_PREFIX)strip
AR=$(CROSS_COMPILE_PREFIX)ar
RM=rm -rfv
INSTALL=install -c -v -m 777

ifeq ($(DEBUG), true)
CFLAGS+= -g
else
CFLAGS+= -Os
endif

# C 编译器特有的编译标志针对 C++ 无效,会有警告
CCFLAGS += -Werror=implicit-int -Werror=implicit-function-declaration -Wbad-function-cast
          
#添加如下编译参数
CFLAGS += -Werror -Wall -Werror=return-type -Wpointer-arith -Wundef -Wcast-qual\
          -ffunction-sections -fdata-sections -fno-exceptions

LDFLAGS+= -lpthread
LIBPATH+= -Llib -L${LIBDIR}
INCLUDES+= -Iinclude -I../include -I../../include -I../common -I../../DVR_SDK -I../../DVR_SDK/include -I../../DVR_SDK/PUB -I../../DVR_SDK/PUB_BORAD -I../hybackup
INCLUDES+= -I../../../baseline/$(BASELINE_INC)/mbase -I../../../baseline/$(BASELINE_INC)/driver
DFLAGS+= -g -DDEBUG

MPATH=$(shell pwd)
LIBNAME=$(notdir $(MPATH))

ifeq ($(STATIC), true)
LIBDIR=../../DVR_SDK/lib_${TYPE}-static/$(PRODUCT_TYPE)/
BIN=lib$(LIBNAME).a
RELEASE_DIR=RELEASE-$(ARCH)-static
else
LIBDIR=../../DVR_SDK/lib_${TYPE}
BIN=lib$(LIBNAME).so
RELEASE_DIR=RELEASE-$(ARCH)
CFLAGS+= -fPIC
endif

DEBUG_DIR=DEBUG-$(ARCH)
TARGET_DIRS=$(DEBUG_DIR) $(RELEASE_DIR)
TARGET_DIR_CHECK=$(patsubst %,%/.,$(TARGET_DIRS))

DEBUG_TARGET=$(DEBUG_DIR)/$(BIN)
RELEASE_TARGET=$(RELEASE_DIR)/$(BIN)
TARGETS=$(DEBUG_TARGET) $(RELEASE_TARGET)
LIB_TARGETS=$(addprefix ${LIBDIR}/, $(DEBUG_TARGET), $(RELEASE_TARGET))

OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS+=$(patsubst %.cpp, %.o, $(wildcard *.cpp))

.PHONY:debug relase clean build test

release:$(RELEASE_DIR)/. $(PROTO_TARGET) $(RELEASE_TARGET)
debug:$(DEBUG_DIR)/. $(PROTO_TARGET) $(DEBUG_TARGET)

clean:
	$(RM) ${OBJECTS} ${TARGETS} $(TARGET_DIRS)
	$(RM) RELEASE-* DEBUG-*

# create default build dirs
build:
%/.:
	mkdir -m 777 -p $*

$(DEBUG_TARGET):$(addprefix $(DEBUG_DIR)/,$(OBJECTS))
	$(CXX) -o $@ $^ $(LDFLAGS) $(GTK_LIB) $(LIBPATH) $(CFLAGS) $(DFLAGS)
#	$(INSTALL) $(LIBNAME)_api.h ../include/
	$(INSTALL) $@ ../../DVR_SDK/lib_${TYPE}/

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
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDES) $(GTK_INC) $(DFLAGS)

$(RELEASE_DIR)/%.o:%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDES) $(GTK_INC)

$(DEBUG_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS) $(INCLUDES) $(GTK_INC) $(DFLAGS)

$(RELEASE_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS) $(INCLUDES) $(GTK_INC)


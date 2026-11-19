#!/usr/bin/make -f

include ../profile
include ../../../aisdk/aisdk_version.mk
include ../../../baseline/baseline_version.mk
include ../../../baseline/${BASELINE_VERSION}/Makefile.param

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
CFLAGS += -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS += -D__Hi3516CV610__ -DACTIVE_CODE_SUPPORT -DDL_ACTIVE_CODE_SUPPORT
CFLAGS += -fno-exceptions -Werror=implicit-int -Werror=return-type
LFLAGS += -fno-exceptions
endif

ifeq ($(ENCRYPT_MODE),AUTHORIZE)
CFLAGS += -D__AUTHORIZE__
endif

ifeq ($(IS_SUPPORT_4G_CLOUD_DOMAIN),yes)
CFLAGS += -D__IS_SUPPORT_4G_CLOUD_DOMAIN__
endif

ifeq ($(STATIC), true)
LIBDIR += -L../../DVR_SDK/lib_${TYPE}-static/${BOARD_TYPE}
LIBDIR += -L../../DVR_SDK/lib_${TYPE}-static/${BOARD_TYPE}/a
LIBDIR += -L../../../aisdk/${AISDK_VERSION}/lib
LIBDIR += -L../../../baseline/${BASELINE_VERSION}/lib
LIBDIR += -L../../../baseline/${BASELINE_SDK_LIB}
else 
LIBDIR += -L../../DVR_SDK/lib_${TYPE}/${BOARD_TYPE}
LIBDIR += -L../../DVR_SDK/lib_${TYPE}/${BOARD_TYPE}/a
LIBDIR += -L../../DVR_SDK/lib_${TYPE}/${BOARD_TYPE}/sdk
LIBDIR += -L../../DVR_SDK/lib_${TYPE}/${BOARD_TYPE}/sdk/algo
LIBDIR += -L../../DVR_SDK/lib_${TYPE}/${BOARD_TYPE}/sdk/hs
LIBDIR += -L../../DVR_SDK/lib_${TYPE}/${BOARD_TYPE}/sdk/xrs
endif


CC=$(CROSS_COMPILE_PREFIX)gcc
CXX=$(CROSS_COMPILE_PREFIX)g++
STRIP=$(CROSS_COMPILE_PREFIX)strip
RM=rm -rf

CFLAGS += -Wall

CFLAGS += -ffunction-sections -fdata-sections

RFLAGS += $(CFLAGS) -Os

LFLAGS += -lpthread

LFLAGS += -Wl,--gc-sections
LFLAGS += -static-libstdc++

LIBPATH +=${LIBDIR}
INCPATH += -I./ -I../include -I../../DVR_SDK/include -I../../DVR_SDK/PUB -I../../DVR_SDK/PAR -I../../modules/include -I../../modules/common -I../../DVR_SDK/Authorize/include/
INCPATH += -I../../../baseline/$(BASELINE_INC)/mauth  -I../../../baseline/$(BASELINE_INC)/mbase

MPATH=$(shell pwd)
BINNAME=$(notdir $(MPATH))

BIN=$(BINNAME)

BIN_DIR=../bin_${TYPE}

RELEASE_DIR=RELEASE-$(ARCH)

RELEASE_TARGET=$(BIN_DIR)/$(BIN)
TARGETS=$(RELEASE_TARGET)

OBJECTS=$(patsubst %.c, %.o, $(sort $(wildcard *.c)))
OBJECTS += $(patsubst %.cpp, %.o, $(sort $(wildcard *.cpp)))

.PHONY:relase clean

release:$(RELEASE_DIR)/. ${BIN_DIR}/. $(RELEASE_TARGET)

clean:
	$(RM) ${OBJECTS} ${TARGETS}
	$(RM) RELEASE-* ../bin_*

# create default build dirs
build:
%/.:
	mkdir -m 777 -p $*

$(RELEASE_TARGET):$(addprefix $(RELEASE_DIR)/,$(OBJECTS))
	$(CXX) -o $@ $^ $(LFLAGS) $(LIBPATH) $(RFLAGS)
	$(STRIP) $@

$(RELEASE_DIR)/%.o:%.c
	$(CC) -c $^ -o $@ $(RFLAGS) $(INCPATH)

$(RELEASE_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(RFLAGS) $(INCPATH)

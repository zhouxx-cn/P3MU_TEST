#!/usr/bin/make -f

include ./profile

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02-linux-musleabi-
CROSS_COMPILE_PATH=/opt/linux/x86-arm/arm-v01c02-linux-musleabi-gcc/bin/
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
CFLAGS += -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS += -DACTIVE_CODE_SUPPORT -DDL_ACTIVE_CODE_SUPPORT
CFLAGS += -fno-exceptions -Werror=return-type
LFLAGS += -fno-exceptions

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
endif

#动态定义宏，放于公共部分，避免每个平台或版型下都要加一遍

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
CFLAGS += -D__STATIC_LINK_STDC__
endif

ifeq ($(IS_SUPPORT_4G_CLOUD_DOMAIN),yes)
CFLAGS += -D__IS_SUPPORT_4G_CLOUD_DOMAIN__
endif

INSTALL=install -cv -m 777

CC := $(CROSS_COMPILE_PREFIX)gcc
CXX := $(CROSS_COMPILE_PREFIX)g++
STRIP := $(CROSS_COMPILE_PREFIX)strip

CFLAGS+= -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR 
CFLAGS+= -Wall
CFLAGS += -Os -ffunction-sections -fdata-sections

#LDFLAGS += -z muldefs

LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--start-group
LDFLAGS += -lrt -lpthread -lm -ldl
LDFLAGS += -Xlinker -no-enum-size-warning

ifeq ($(STATIC), true)

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)
LIBS-static = $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/ -maxdepth 1 -name "*.a" | sort | sed -e s/.*\\/lib/-l/g -e s/.a$$//)

LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)/a
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/a -maxdepth 1 -name "*.a"| sort | sed -e s/.*\\/lib/-l/g -e s/\\.a//)

LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)/a/HYOnvif
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/a/HYOnvif -maxdepth 1 -name "*.a"| sort | sed -e s/.*\\/lib/-l/g -e s/\\.a//)

LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)/a/4G
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/a/4G -maxdepth 1 -name "*.a"| sort | sed -e s/.*\\/lib/-l/g -e s/\\.a//)

LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/hs
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/hs -maxdepth 1 -name "*.a"| sort | sed -e s/.*\\/lib/-l/g -e s/\\.a//)
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/hs -maxdepth 1 -name "*.so" | sort | sed -e s/.*lib/-l/g -e s/.so$$//)

LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/xrs
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/xrs -maxdepth 1 -name "*.a" | sort | sed -e s/.*\\/lib/-l/g -e s/.a$$//)
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/xrs -maxdepth 1 -name "*.so" | sort | sed -e s/.*lib/-l/g -e s/.so$$//)

LIB_DIR += -L./lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/algo
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/algo -maxdepth 1 -name "*.a" | sort | sed -e s/.*\\/lib/-l/g -e s/.a$$//)
LIBS-static += $(shell find lib/lib_${TYPE}/$(PRODUCT_TYPE)/sdk/algo -maxdepth 1 -name "*.so" | sort | sed -e s/.*lib/-l/g -e s/.so$$//)

LDFLAGS += -lzwhosts
endif

LDFLAGS	+=  $(LIBS-static)

else
LDFLAGS += -lrt -lpthread -lm -ldl -Wl,-gc-sections
# 私有公共库
LDFLAGS += -lmysystem -lmxml -lzwsdk
# 中间层库
LDFLAGS	+= -lmemrecord -lftp_new -lsmtp -lwolfssl
LDFLAGS	+= -lMPU -lPUB -lPAR -lAVL -lDIP -lDOP -lNET -lPTZ -lOPL -lSYS -lUSR -lSnapShot
LDFLAGS	+= -lwatch -lip -ladsl -lCOM -lRNL -lddt 
LDFLAGS +=  -lmd5 -ldes -ldes_std -lbase64 -lz -lssl -lcrypto -lssh2 -lcurl
LDFLAGS	+= -lhie_dnsupdate -lhybackup
LDFLAGS	+= -laaa 
LDFLAGS += -lCGIServer
# 网络库
ifeq ($(USE_HOSTS_PARSER),yes)
LDFLAGS += -lzwhosts
endif
# LDFLAGS += -lhieserver -lhyprivate -lonvif -lzwmobile -lzwp2ppushclient -ljstp
# LDFLAGS += -lhyonvif -lrtsp -lNDT_API_PPCS -lzwp2p -lPPCS_API -lSPS_API 
LDFLAGS += -lhieserver -lhyprivate -lonvif -lhyonvif -lrtsp

ifeq ($(IS_CUSTOM_LS),yes)
LDFLAGS += -lrt -ljstp -ldl -lm -lpthread -llongsee -llscpdevice -lssl -lcrypto -lcJSON
endif
ifeq ($(WITH_AUTH),yes)
LDFLAGS	+= -lAuthority
endif
ifeq ($(WITH_RTMP),yes)
CFLAGS += -D __USE_RTMP__
LDFLAGS	+= -lzwrtmp -lrtmp
endif
#STATIC宏条件结束
endif

LIBPAH += ${LIBDIR}
TARGET=xrscam
LDFLAGS += -Wl,--end-group

ifeq ($(STATIC_LINK_STDC),yes)
LDFLAGS += -static-libstdc++
endif

RELEASE_DIR := Release
OBJ_DIR := ./obj/obj_${TYPE}
RELEASE_TARGET := $(RELEASE_DIR)/$(TARGET)

.PHONY : clean

%/.:
	mkdir -p $*

release:$(RELEASE_DIR)/. $(RELEASE_TARGET)

$(RELEASE_TARGET): $(OBJ_DIR)/*.o
	$(CXX) -Os -o $@ $^ -Wl,-Map=edvr.map  -Wl,--dynamic-list=dl.txt $(LIB_DIR) $(LDFLAGS) 
	$(STRIP) $(RELEASE_TARGET)
	@ls -lh $(RELEASE_TARGET)
	@md5sum $(RELEASE_TARGET)
	@echo -e "\e[1;35mCompile successful!\e[0m"

clean:
	rm -vrf $(RELEASE_DIR)

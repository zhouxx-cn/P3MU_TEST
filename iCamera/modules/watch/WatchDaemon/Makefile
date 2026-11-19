#!/usr/bin/make -f

include ../../../DVR_SDK/profile
include ../../../../baseline/baseline_version.mk

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CROSS_COMPILE_PATH=/opt/linux/x86-arm/arm-v01c02s020-linux-musleabi-gcc/bin/
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
CFLAGS += -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR

ifeq ($(WITH_RTMP),yes)
CFLAGS += -D __USE_RTMP__ -D ACTIVE_CODE_SUPPORT
endif
LIBS-static += $(shell find ../../../../baseline/$(BASELINE_VERSION)/lib -maxdepth 1 -name "*.a" | sort | sed -e s/.*\\/lib/-l/g -e s/.a$$//)
LIBPAH += -L../../../../baseline/$(BASELINE_VERSION)/lib
CFLAGS += -lupgrade -laes -lcrc32 -lsha256 -lmysystem
endif

FS_PATH=../../../../framework/$(PRODUCT_TYPE)/partition/custom_made/$(PRODUCT_TYPE)/

ifeq ($(SUPPORT_UPGRADE_V2),yes)
CFLAGS += -D__UPGRADE_V2__
LDFLAGS += -lmd5
endif

ifeq ($(IS_SUPPORT_UBOOT_RECOVER),yes)
CFLAGS += -D__UBOOT_RECOVER__
endif

ifeq ($(STATIC_LINK_STDC),yes)
CFLAGS += -D__STATIC_LINK_STDC__
endif

ifeq ($(IS_SUPPORT_4G_MODULE),yes)
CFLAGS += -D__MODULE_4G_SUPPORT__
endif

INSTALL=install -cv -m 777

CFLAGS+= -DEGUI_FOR_ICAMERA_${TYPE}_BASED_DVR

CC      =  $(CROSS_COMPILE_PREFIX)gcc
STRIP   =  $(CROSS_COMPILE_PREFIX)strip
CFLAGS  += -Os -D_POSIX_SOURCE -D_BSD_SOURCE
LDFLAGS+=-lpthread 
LIBPAH +=  -L./

# C 编译器特有的编译标志针对 C++ 无效,会有警告
CCFLAGS += -Werror=implicit-int -Werror=implicit-function-declaration

#添加如下编译参数
CFLAGS += -Werror -Wall -Werror=return-type -Wpointer-arith -Wcast-qual \
          -ffunction-sections -fdata-sections -fno-exceptions

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
LIBPAH +=  -L../../../DVR_SDK/lib_${TYPE}-static/$(PRODUCT_TYPE)/
LIBPAH +=  -L../../../DVR_SDK/lib_${TYPE}-static/$(PRODUCT_TYPE)/sdk
else
LIBPAH +=  -L../../../DVR_SDK/lib_${TYPE}-static
LIBPAH +=  -L../../../DVR_SDK/lib_${TYPE}-static/sdk
endif
INCLUDES=  -I./inc -I../../include -I../../../DVR_SDK/PUB/ -I../../../DVR_SDK/PUB_BORAD/ -I../../../../baseline/$(BASELINE_INC)/misc -I../../../../baseline/$(BASELINE_INC)/mbase
DFLAGS  =  -g -DDEBUG  
RM      =  rm -rf

BIN = myWatchDogProcess
PRE_ALL     =  build
TARGET_DIRS = DEBUG RELEASE playdist
TARGET_DIR_CHECK = $(patsubst %,%/.,$(TARGET_DIRS))
TARGETS  = DEBUG/$(BIN)  ../$(BIN)
DEBUG_TARGET    = DEBUG/$(BIN)
RELEASE_TARGET  = ../$(BIN)
PROTO_TARGET    =
VPATH += ../../../DVR_SDK/PUB/ 
VPATH += ../../../DVR_SDK/PUB_BORAD/ 
SOURCES  = $(wildcard *.c) PUB_BoradInfo.c

OBJECTS  = $(patsubst %.c, %.o, $(SOURCES))
POST_ALL = dist

.PHONY: dummy debug relase all clean build

release: RELEASE/. $(PROTO_TARGET) $(RELEASE_TARGET)
debug:   DEBUG/. $(PROTO_TARGET) $(DEBUG_TARGET)
all: $(PRE_ALL) $(TARGET_DIR_CHECK) $(PROTO_TARGET) ${TARGETS} $(POST_ALL)

dist:  dummy
	cp -rvf  $(DEBUG_TARGET)    dist/
	cp -rvf  $(RELEASE_TARGET)  dist/

clean: dummy
	$(RM) ${OBJECTS} ${TARGETS} $(TARGET_DIRS) func_proto.h

build: 

# create missing build dirs
%/.:
	mkdir -p $*

# set a "do nothing" target
dummy:

doc:
	doxygen

func_proto.h:
	$(RM) func_proto.h
	touch func_proto.h
	#ctags --c-types=+x -R *
	cproto -I. `pkg-config --cflags gtk+-2.0` $(BIN).c -s -o func_proto.h

DEBUG/$(BIN): $(addprefix DEBUG/,$(OBJECTS))
	$(CC) -o $@  $^ $(CFLAGS) $(CCFLAGS) $(INCLUDES) $(DFLAGS) $(LDFLAGS) $(GTK_LIB) $(LIBPAH)
	echo "debug"

RELEASE/$(BIN): $(addprefix RELEASE/,$(OBJECTS))
	$(CC) -o $@  $^ $(CFLAGS) $(CCFLAGS) $(INCLUDES) $(LDFLAGS) $(GTK_LIB) $(LIBPAH)
	echo "release"

../$(BIN): $(addprefix RELEASE/,$(OBJECTS))
	$(CC) -o $@  $^ $(CFLAGS) $(CCFLAGS) $(INCLUDES)  $(LDFLAGS) $(GTK_LIB) $(LIBPAH) 
	$(STRIP) $@
	@cp -v $@ $(FS_PATH)
	echo "release"
	
DEBUG/%.o:%.c
	$(CC) $(CFLAGS) $(CCFLAGS) $(INCLUDES) $(DFLAGS) $(GTK_INC) $(LIBPAH)  -c $^ -o $@

RELEASE/%.o:%.c
	$(CC) $(CFLAGS) $(CCFLAGS) $(INCLUDES) $(GTK_INC) $(LIBPAH)  -c $^ -o $@


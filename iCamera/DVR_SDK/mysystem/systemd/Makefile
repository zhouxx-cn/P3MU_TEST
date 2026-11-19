#!/usr/bin/make -f
#ble this macro only when you want to build it singly

include ../../profile
include ../../printLevel

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations
CFLAGS+= -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS += -Werror=implicit-int  -Werror=return-type
endif

CC=$(CROSS_COMPILE_PREFIX)gcc
CXX=$(CROSS_COMPILE_PREFIX)g++
STRIP=$(CROSS_COMPILE_PREFIX)strip
RM=rm -rf
INSTALL=install -cv -m 777

CFLAGS+= -Wall
RFLAGS+= $(CFLAGS) -O3 -DNDEBUG
DFLAGS+= $(CFLAGS) -W -g
TESTFLAGS+= -O3 -DNDEBUG -Wall
LFLAGS+= -lpthread
LIBPATH+= -L${LIBDIR} -L.
INCPATH+= -I.. -I../.. 

MPATH=$(shell pwd)
LIBNAME=$(notdir $(MPATH))
BIN=systemd

DEBUG_DIR=DEBUG-$(ARCH)
RELEASE_DIR=RELEASE-$(ARCH)
TEST_DIR=test

DEBUG_TARGET=$(DEBUG_DIR)/$(BIN)
RELEASE_TARGET=$(RELEASE_DIR)/$(BIN)
TARGETS=$(DEBUG_TARGET) $(RELEASE_TARGET)

all:
	${CC} systemd.c ../disk_shm.c -o systemd -I../ $(RFLAGS)

clean:
	rm -rvf systemd *.o

install:
	./install.sh


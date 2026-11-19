#makefile for the protocol dll
include ../../../DVR_SDK/profile


#CROSS_COMPILE_PREFIX=arm-hismall-linux-
#CROSS_COMPILE_PATH=/
#CROSS_COMPILE_PREFIX=powerpc-linux-
#CROSS_COMPILE_PATH=/opt/mtwk/rootfs/
#CROSS_COMPILE_PREFIX = mips_fp_le-
#CROSS_COMPILE_PATH   = /opt/montavista/pro/devkit/mips/fp_le/target/
#CROSS_COMPILE_PREFIX=arm-linux-
#CROSS_COMPILE_PATH=/opt/hisi-linux/x86-arm/gcc-3.4.3-uClibc-0.9.28/
#CROSS_COMPILE_PREFIX=arm-linux-uclibcgnueabi-
#CROSS_COMPILE_PATH=/opt/Mozart_Toolchain/arm-eabi-uclibc/

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CROSS_COMPILE_PATH=/opt/linux/x86-arm/arm-v01c02s020-linux-musleabi-gcc/bin/
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations
CFLAGS+= -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS += -fno-exceptions
CFLAGS += -Werror=implicit-int  -Werror=return-type
endif

CC      =  $(CROSS_COMPILE_PREFIX)gcc
CXX=$(CROSS_COMPILE_PREFIX)g++
STRIP=$(CROSS_COMPILE_PREFIX)strip
#CFLAGS  = -D_POSIX_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -DGNU_SOURCE 
LDFLAGS = -lpthread -ldl
LIB =  -L. -L./ -L$(CROSS_COMPILE_PATH)usr/X11R6/lib -L$(CROSS_COMPILE_PATH)usr/lib  -L$(CROSS_COMPILE_PATH)usr/local/lib
INCLUDES=  -I. -I../../../DVR_SDK/PUB -I$(CROSS_COMPILE_PATH)usr/include -I$(CROSS_COMPILE_PATH)usr/local/include
DFLAGS  =  -g -DDEBUG

CLINK		= -c -o

RM		= rm -f


OBJ		= DevControl.o
SVNVERSION="`LANGUAGE=en_US:en svn info . | sed -n "/Last Changed Rev:/p" | sed "s/.*: \([0-9]\+\)/\1/g"`"
ifneq ("$(SVNVERSION)", "")
DATE="`LANGUAGE=en_US:en svn info . | sed -n "/Last Changed Date:/p" | sed "s/.*\([0-9]\{4\}-[0-9]\{2\}-[0-9]\{2\}\).*\([0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\}\).*/\1/g" | sed "s/-//g"`"
DATETIME="`LANGUAGE=en_US:en svn info . | sed -n "/Last Changed Date:/p" | sed "s/.*\([0-9]\{4\}-[0-9]\{2\}-[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\}\).*/\1/g"`"
else
DATE="`svn info . | sed -n "/最后修改的时间:/p" | sed "s/.*\([0-9]\{4\}-[0-9]\{2\}-[0-9]\{2\}\).*\([0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\}\).*/\1/g" | sed "s/-//g"`"
DATETIME="`svn info . | sed -n "/最后修改的时间:/p" | sed "s/.*\([0-9]\{4\}-[0-9]\{2\}-[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\}\).*/\1/g"`"
SVNVERSION := "`svn info . | sed -n "/最后修改的版本:/p" | sed "s/.*: \([0-9]\+\)/\1/g"`"
endif

CFLAGS+=-DBASEVERSION=\"$(SVNVERSION)\" -DDATETIME=\"${DATETIME}\"
CFLAGS+=-DP_NAME=\"`expr $(EXECUTABLE) : 'lib\(.*\).so'`\"

all:		$(EXECUTABLE)

clean:	
		$(RM) *.o *.so *~ $(EXECUTABLE)

$(EXECUTABLE):	$(OBJ)
		$(CC) -fPIC -shared -o  $@ $(OBJ) $(LIB)
		#$(STRIP) $@
		@echo $(SVNVERSION)
		#cp $@ ../../../fs_${TYPE}/root/edvr/IntDll

%.o:		%.cpp
		$(CC) -fPIC $(CFLAGS) $(CLINK) $@ $(INCLUDES) $<


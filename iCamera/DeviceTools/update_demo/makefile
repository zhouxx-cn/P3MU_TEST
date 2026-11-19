#!/usr/bin/make -f

include ../../DVR_SDK/profile

ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-v01c02s020-linux-musleabi-
CROSS_COMPILE_PATH=/opt/linux/x86-arm/arm-v01c02s020-linux-musleabi-gcc/bin/
CFLAGS += -DEGUI_FOR_ICAMERA_NXP8850_BASED_DVR
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4
CFLAGS += -DACTIVE_CODE_SUPPORT -DDL_ACTIVE_CODE_SUPPORT
CFLAGS += -Werror=implicit-int  -Werror=return-type
endif

CXX=$(CROSS_COMPILE_PREFIX)g++
STRIP=$(CROSS_COMPILE_PREFIX)strip
RM=rm -rf
INSTALL=install -cv -m 777

ifeq ($(STATIC), true)
ifneq ($(findstring $(TYPE), Hi3516CV608 Hi3516CV610),)
LIBDIR=../../DVR_SDK/lib_${TYPE}-static/$(PRODUCT_TYPE)/
else
LIBDIR=../../DVR_SDK/lib_${TYPE}-static 
endif
endif 

CFLAGS	+= -Wall

CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS += -Wl,--gc-sections

LDFLAGS += -static-libstdc++

LDFLAGS	+= -lpthread -ldl
LDFLAGS	+= -lPAR -lRNL
LIBPAH	+= -L${LIBDIR}
INCLUDES=  -I../../DVR_SDK -I../../DVR_SDK/include -I../../DVR_SDK/PUB -I../../modules/include
DFLAGS	=  -g -DDEBUG $(CFLAGS)
RFLAGS	=  -O3 $(CFLAGS)

BIN		= update
PRE_ALL		= build

DEBUG_DIR=DEBUG
RELEASE_DIR=RELEASE
TARGET_DIRS	= $(DEBUG_DIR) $(RELEASE_DIR)

TARGET_DIR_CHECK= $(patsubst %,%/.,$(TARGET_DIRS))

DEBUG_TARGET	=$(DEBUG_DIR)/$(BIN)
RELEASE_TARGET	=./$(BIN)
TARGETS		= $(DEBUG_TARGET) $(RELEASE_TARGET)
PROTO_TARGET	= 
SOURCES		= $(wildcard *.c)
OBJECTS		= $(patsubst %.c, %.o, $(SOURCES))
POST_ALL	= 

.PHONY: dummy debug relase all clean build

release: $(RELEASE_DIR)/. $(PROTO_TARGET) $(RELEASE_TARGET)
debug:   $(DEBUG_DIR)/. $(PROTO_TARGET) $(DEBUG_TARGET)
all: $(PRE_ALL) $(TARGET_DIR_CHECK) $(PROTO_TARGET) ${TARGETS} $(POST_ALL)

clean: dummy
	$(RM) ${OBJECTS} ${TARGETS} $(TARGET_DIRS)

build: 

# create missing build dirs
%/.:
	mkdir -p $*

# set a "do nothing" target
dummy:

$(DEBUG_TARGET): $(addprefix $(DEBUG_DIR)/,$(OBJECTS))
	$(CXX) -o $@  $^  $(DFLAGS) $(LDFLAGS) $(LIBPAH)
	echo "debug $@"
	$(INSTALL) $@ $(INSTALL_DIR)

$(RELEASE_TARGET): $(addprefix $(RELEASE_DIR)/,$(OBJECTS))
	$(CXX) -o $@  $^  $(RFLAGS) $(LDFLAGS) $(LIBPAH) 
	$(STRIP) $@
	echo "release $@"
	#$(INSTALL) $@ $(INSTALL_DIR)

$(DEBUG_DIR)/%.o:%.c
	$(CXX) $(DFLAGS) $(INCLUDES) -c $^ -o $@

$(RELEASE_DIR)/%.o:%.c
	$(CXX) $(RFLAGS) $(INCLUDES) -c $^ -o $@


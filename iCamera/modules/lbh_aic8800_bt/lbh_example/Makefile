include ../../../DVR_SDK/profile

APP_NAME = lbh_client
BIN=lib$(APP_NAME).a
LDFLAGS := -lpthread

//====================CONFIG SETTING=========================
CONFIG_LBH_CODEC = y
//===========================================================

//==================PLATFORM SELECTION========================

CONFIG_PLATFORM_MIPS = n
CONFIG_PLATFORM_MIPS_UCLIBC = y
CONFIG_PLATFORM_MIPS_XX = n
CONFIG_PLATFORM_X86 = n

ifeq ($(CONFIG_PLATFORM_MIPS_UCLIBC), y)
OTHER_PARAM = -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4  -Werror=implicit-int  -Werror=return-type
CROSS_COMPILE = arm-$(COMPILER_VERSION)-linux-musleabi-
AR=$(CROSS_COMPILE)ar
ARCH = arm
#because not support yet
CONFIG_LBH_CODEC = n
else ifeq ($(CONFIG_PLATFORM_MIPS), y)
CROSS_COMPILE = /home/zhenyadeng/D/AIC-LBH/toolchain/arm/cvte/openwrt-linux-gnueabi/bin/arm-openwrt-linux-gnueabi-
ARCH = arm
CPFLAGS += -std=gnu99
else ifeq ($(CONFIG_PLATFORM_X86), y)
CROSS_COMPILE = 
ARCH = x86_64
else ifeq ($(CONFIG_PLATFORM_MIPS_XX), y)
CROSS_COMPILE = /home/zhenyadeng/D/AIC-LBH/toolchain/mips/xx/mips-linux-gnu-ingenic-gcc7.2.0-glibc2.29-fp64/bin/mips-linux-gnu-
ARCH = mips
CONFIG_BT := y
CONFIG_BLE := y
CONFIG_LBH_CODEC = n
endif

//===========================================================

CONFIG_BT ?= y
CONFIG_BLE ?= y

STATICLAB = lbh
OBJPATH = ./app/src

INCS += -I app/include \
	-I common/include \
	-I ./atbm_nimble_api.h \

SRCS := $(wildcard *.c ./app/src/*.c)
OBJS := $(patsubst %c,$(OBJSPATH)/%o,$(notdir $(SRCS)))

CFILES += lbh_client.c
ifeq ($(CONFIG_BT), y)
	CFILES += app/src/lbh_bt.c
	CFLAGS += -DLBH_BT_ENABLE
endif
ifeq ($(CONFIG_BLE), y)
	CFILES += app/src/lbh_ble.c
	CFLAGS += -DLBH_BLE_ENABLE
endif

ifeq ($(CONFIG_LBH_CODEC), y)
	CFLAGS += -DCONFIG_LBH_CODEC \
		  -DA2DP_AAC_ENABLE

        CFILES += app/src/lbh_codec.c \
		common/src/lbh_wav.c

	CFILES += $(foreach dir, utils/aaccodec, $(wildcard $(dir)/*.c))
	CFILES += $(foreach dir, utils/sbccodec/src, $(wildcard $(dir)/*.c))
	CFILES += $(foreach dir, utils/sbccodec/sbc, $(wildcard $(dir)/*.c))

	INCS += -I utils/sbccodec/sbc \
	       -I utils/aaccodec 
endif

ifeq ($(ARCH), x86_64)
	CFLAGS += -D__i386__ 
endif

ifeq ($(PRODUCT_TYPE), H4S-W)
	CFLAGS += -D__H4S_W__
else ifeq ($(PRODUCT_TYPE),H8SX)
	CFLAGS += -D __H8SX__
else ifeq ($(PRODUCT_TYPE),H8SVX)
	CFLAGS += -D __H8SVX__
else ifeq ($(PRODUCT_TYPE),H5SVX)
	CFLAGS += -D __H5SVX__
else ifeq ($(PRODUCT_TYPE), H4-W)
	CFLAGS += -D __H4_W__
endif
 
all: $(APP_NAME)

lbh_client:
	$(CROSS_COMPILE)gcc $(OTHER_PARAM) $(INCS) -I ../lbh_lib -c $(CFILES) $(CFLAGS) -lm  $(LDFLAGS) -L../lbh_lib -l $(STATICLAB)
	$(CROSS_COMPILE)ar -x ../lbh_lib/liblbh.a
	$(CROSS_COMPILE)ar -rcs $(BIN) ./lbh_ble.o ./lbh_bt.o ./lbh_client.o queue_client.c.o queue_client_plf.c.o 
	cp -v $(BIN) ../../../DVR_SDK/lib_Hi3516CV610-static/$(PRODUCT_TYPE)/
print:
	@echo $(CFILES)
clean:
	rm -rf *.o $(APP_NAME) $(BIN)
debug:
	$(CROSS_COMPILE)gcc $(OTHER_PARAM) $(INCS) -I ../lbh_lib -o $(APP_NAME) $(CFILES)  $(CFLAGS) -lm  $(LDFLAGS) -L../lbh_lib -l $(STATICLAB)
test:
	cp lbh_client /home/public/share/nfs/t4/lbh_server_f
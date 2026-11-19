#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_adec

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -ladec -limp -lalog -lsysutils -lhalmod -lai
ifeq ($(SUPPORT_AUDIO_AEC_FUNC), true)
LIBS_XRS	+= -laudioProcess
endif
# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) -L$(FIRMWARE_LIB_DIR) $(LIBS_XRS) $(LINUX_LIB)

CFLAGS		+= -I$(FIRMWARE_INC_DIR) -I$(INC_DIR) -I$(INC_DIR)/mavi -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/misp  -I$(INC_DIR)/mauth -I$(FIRMWARE_DIR)/include/imp

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

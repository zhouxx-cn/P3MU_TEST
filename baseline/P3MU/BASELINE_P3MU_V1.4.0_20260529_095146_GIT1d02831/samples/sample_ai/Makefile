#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_ai

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lai -limp -lalog -lsysutils -laenc 
ifeq ($(SUPPORT_AUDIO_AEC_FUNC), true)
LIBS_XRS	+= -laudioProcess
endif
ifeq ($(SUPPORT_FAAC), true)
LIBS_XRS        += -lfaac
endif
ifeq ($(SUPPORT_XTRACE_MUTEX), true)
LIBS_XRS        += -lxtrace
endif
ifeq ($(SUPPORT_JZ_MERT_AUDIO_ALGO), true)
LIBS_XRS	+= -lmert -ldrivers
endif
# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) -L$(FIRMWARE_LIB_DIR) $(LIBS_XRS) $(LINUX_LIB)

CFLAGS		+= -I$(FIRMWARE_INC_DIR) -I$(INC_DIR) -I$(INC_DIR)/mavi -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/misp  -I$(INC_DIR)/mauth

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

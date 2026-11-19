#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_vosd

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lvisp -lvpss -lvenc -lvi -lvosd -lvda -lai -lao -laenc -ladec -lhalmod -limp -lalog -lsysutils -lmysystem # -lturbojpeg -ljpeg
ifeq ($(SUPPORT_AUDIO_AEC_FUNC), true)
LIBS_XRS	+= -laudioProcess
endif
ifeq ($(SUPPORT_FAAC), true)
LIBS_XRS	+= -lfaac
endif
ifeq ($(SUPPORT_QRCODE), true)
LIBS_XRS	+= -lzbar
endif
ifeq ($(SUPPORT_HIGH_MD_ALGO), true)
#LIBS_XRS	+= -lmove_inf -lmxu_merge -lmxu_contrib -lmxu_video -lmxu_imgproc -lmxu_core
LIBS_XRS	+= -lmove_inf -lmxu_merge -lmxu_imgproc -lmxu_core -lcjson
endif

#ifeq ($(SUPPORT_JZ_VIDEO_MERGE_FUNC),true)
#LIBS_XRS	+= -lvideoMerge_inf
#endif

ifeq ($(SUPPORT_XTRACE_MUTEX), true)
LIBS_XRS        += -lxtrace
endif

ifeq ($(SUPPORT_JZ_MERT_AUDIO_ALGO), true)
LIBS_XRS        += -lmert -ldrivers
endif

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl -lstdc++

LDFLAGS		+= -L$(LIB_DIR) -L$(LIB_DIR)/alog -L$(FIRMWARE_LIB_DIR) $(LIBS_XRS) $(LINUX_LIB) 

CFLAGS		+= -I$(FIRMWARE_INC_DIR)/imp -I$(INC_DIR) -I$(INC_DIR)/mavi -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/misp -I$(INC_DIR)/mauth

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

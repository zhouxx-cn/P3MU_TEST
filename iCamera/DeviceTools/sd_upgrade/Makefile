#
# GPL source Makefile
#

include ../profile
include ../common.mak
MPATH=$(shell pwd)
BINNAME=$(notdir $(MPATH))
BIN=$(BINNAME)
BIN_DIR=../bin_${TYPE}
RELEASE_TARGET=$(BIN_DIR)/$(BIN)
TARGET_BIN=$(RELEASE_TARGET)

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lupgrade -lcrc32 -lmysystem -laes

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) $(LIBS_XRS) $(LINUX_LIB)

CFLAGS		+= -I$(INC_DIR) -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/utils 

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

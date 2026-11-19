#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_xtrace

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lxtrace

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) $(LIBS_XRS) $(LINUX_LIB)

CFLAGS		+= -I$(INC_DIR) -I$(INC_DIR)/mauth -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/utils 

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

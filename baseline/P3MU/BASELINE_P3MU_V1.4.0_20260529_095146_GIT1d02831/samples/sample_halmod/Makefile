#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_halmod

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lmauth -lcrc32 -lupgrade -ldes -lhalmod -lmysystem

LIBS_HI		= -lhi_mpi

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -static -L$(LIB_DIR) -L$(PACKAGE_LIB_DIR) -Wl,--start-group $(LIBS_XRS) $(LINUX_LIB) $(LIBS_HI) -Wl,--end-group

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

#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= gpio_test

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

# XRS libs
LIBS_XRS	= 

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) -L$(FIRMWARE_LIB_DIR) $(LIBS_XRS) $(LINUX_LIB)

CFLAGS		+= -I$(FIRMWARE_INC_DIR) -I$(INC_DIR) -I$(INC_DIR)/mbase $(BOARD_CFLAGS)

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

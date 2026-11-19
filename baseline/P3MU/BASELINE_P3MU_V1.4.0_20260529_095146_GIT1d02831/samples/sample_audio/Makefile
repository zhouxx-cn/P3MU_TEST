#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_audio

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lao -lhalmod -lai -ladec -laenc -lcjson -lmysystem

ifeq ($(SUPPORT_FAAC), true)
LIBS_XRS        += -lfaac
endif
ifeq ($(SUPPORT_XTRACE_MUTEX), true)
LIBS_XRS        += -lxtrace
endif

LIBS_HI		= -lhi_mpi	\
		-lhi_mpi_sysbind	\
		-lhi_mpi_sysmem	\
		-lhi_mpi_ae		\
		-lhi_mpi_isp		\
		-lot_mpi_isp		\
		-lhi_mpi_awb		\
		-lvqe_aec	\
		-lvqe_hpf	\
		-lvqe_anr	\
		-lvqe_agc	\
		-ldehaze			\
		-lextend_stats	\
		-ldrc				\
		-lldci			\
		-lbnr				\
		-lcalcflicker		\
		-lir_auto			\
		-lacs				\
		-lhi_mpi_audio		\
		-lvoice_engine	\
		-lupvqe		\
		-ldnvqe		\
		-lopus			\
		-lsecurec

ifeq ($(SUPPORT_AUDIO_VQE_HIGH_VERSION), true)
LIBS_XRS        += -lvqe_talkv2 -lvqe_res
endif


ifeq ($(SUPPORT_AO_HS_ALGO), true)
LIBS_XRS	+= -lvqe_hs
endif

ifeq ($(SUPPORT_AMR), true)
LIBS_XRS	+= -lopencore-amrnb
endif

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) -L$(PACKAGE_LIB_DIR) -Wl,-Bstatic -Wl,--start-group $(LIBS_XRS) $(LIBS_HI) -Wl,--end-group -Wl,-Bdynamic $(LINUX_LIB)

CFLAGS		+= -I$(PACKAGE_INC_DIR) -I$(INC_DIR) -I$(INC_DIR)/mavi -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/misp -I$(INC_DIR)/utils -I$(INC_DIR)/mauth

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_visp

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lvi -lvpss -lvenc -lvisp -lrtsp -lvosd -lvda -lxtrace -lcjson -lmysystem
ifeq ($(SUPPORT_XTRACE_MUTEX), true)
LIBS_XRS        += -lxtrace
endif

LIBS_HI		= -lhi_mpi	\
			  -lhi_mpi_sysbind	\
			  -lhi_mpi_sysmem	\
			  -lhi_mpi_isp		\
			  -lot_mpi_isp		\
			  -lhi_ivs_md		\
			  -lhi_mpi_ive		\
			  -ldehaze			\
			  -lextend_stats	\
			  -ldrc				\
			  -lldci			\
			  -lbnr				\
			  -lcalcflicker		\
			  -lir_auto			\
			  -lacs				\
			  -lsecurec

ifeq ($(SUPPORT_PQTOOLS), true)
LIBS_HI	+= -l_pqcontrol
endif

ifeq ($(SUPPORT_AIISP), true)
LIBS_HI	+=	-laiisp				\
			-lhi_mpi_aibnr		\
			-lsvp_acl			\
			-lprotobuf-c
else
ifeq ($(SUPPORT_PQTOOLS), true)
			LIBS_HI	+=	-lsvp_acl			\
			-lprotobuf-c
ifeq ($(SDK), Hi3516CV610_SDK_V1.0.1.0_HiYueying)
			LIBS_HI	+= -laiisp				\
			-lhi_mpi_aibnr
endif

ifeq ($(SDK), Hi3516CV610_SDK_V1.0.1.0_HiYueying_4KCROP)
			LIBS_HI	+= -laiisp				\
			-lhi_mpi_aibnr
endif
ifeq ($(SDK), Hi3516CV610_SDK_V1.0.2.0)
			LIBS_HI	+= -laiisp				\
			-lhi_mpi_aibnr
endif
endif
endif

LIBS_HI_DYNAMIC	= -lhi_mpi_ae		\
			  	  -lhi_mpi_awb

ifeq ($(findstring OS04D10, $(SENSORS)), OS04D10)
LIBS_HI		+= -lsns_os04d10
endif
ifeq ($(findstring OS04D10_MINOR, $(SENSORS)), OS04D10_MINOR)
LIBS_HI		+= -lsns_os04d10_minor
endif
ifeq ($(SENSORS), JXK362P)
LIBS_HI		+= -lsns_jxk362p
endif
ifeq ($(SENSORS), JXK308P)
LIBS_HI		+= -lsns_jxk308p
endif
ifeq ($(SENSORS), IMX415)
LIBS_HI		+= -lsns_imx415
endif
ifeq ($(SENSORS), IMX335)
LIBS_HI		+= -lsns_imx335
endif
ifneq ($(filter F38P, $(SENSORS)),)
LIBS_HI		+= -lsns_jxf38p
endif
ifneq ($(filter F38PA, $(SENSORS)),)
LIBS_HI         += -lsns_jxf38pa
endif
ifeq ($(findstring OS03A10, $(SENSORS)), OS03A10)
LIBS_HI		+= -lsns_os03a10
endif
ifeq ($(findstring GC2083, $(SENSORS)), GC2083)
LIBS_HI		+= -lsns_gc2083
endif
ifeq ($(SENSORS), Q03)
LIBS_HI		+= -lsns_jxq03
endif
ifeq ($(findstring OS04L10, $(SENSORS)), OS04L10)
LIBS_HI		+= -lsns_os04l10
endif
ifeq ($(SENSORS), OS08E10)
LIBS_HI		+= -lsns_os08e10
endif
ifeq ($(SENSORS), JXK06A)
LIBS_HI		+= -lsns_jxk06a
endif
ifeq ($(SENSORS), GC20C3)
LIBS_HI		+= -lsns_gc20c3
endif
ifeq ($(SENSORS), CS6286)
LIBS_HI		+= -lsns_cs6286
endif

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl

LDFLAGS		+= -L$(LIB_DIR) -L$(PACKAGE_LIB_DIR) -Wl,-Bstatic -Wl,--start-group $(LIBS_XRS) $(LINUX_LIB) $(LIBS_HI) -Wl,--end-group -Wl,-Bdynamic $(LIBS_HI_DYNAMIC)

CFLAGS		+= -I$(PACKAGE_INC_DIR) -I$(INC_DIR) -I$(INC_DIR)/mavi -I$(INC_DIR)/mbase -I$(INC_DIR)/misc -I$(INC_DIR)/misp -I$(INC_DIR)/utils

ifdef INSTALL
# Auto Compile
_INSTALL = $(ROOT_DIR)/$(INSTALL)
else
# Manual Compile
_INSTALL =
endif

include $(AUTO_DEP_BIN_MK)

# vim:noet:sw=4:ts=4

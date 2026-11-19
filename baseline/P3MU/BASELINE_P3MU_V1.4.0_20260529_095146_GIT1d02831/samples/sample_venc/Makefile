#
# GPL source Makefile
#

include ../../Makefile.param

TARGET_BIN	:= sample_venc

ifeq ($(PLATFORM), T41N)
LIB_PLAT	= $(LIB_SENSOR)
endif

CFLAGS := $(PLAT_CFLAGS) $(BOARD_CFLAGS)
LDFLAGS := $(PLAT_LDFLAGS)

# XRS libs
LIBS_XRS	= -lvi -lvpss -lvenc -lvisp -lrtsp -lvosd -lvda -lai -laenc -ladec -lao -lhalmod -lzbar -lcjson -lmysystem
ifeq ($(SUPPORT_FAAC), true)
LIBS_XRS		+= -lfaac
endif
ifeq ($(SUPPORT_XTRACE_MUTEX), true)
LIBS_XRS		+= -lxtrace
endif
ifeq ($(SUPPORT_FFMPEG), true)
LIBS_XRS	+= -lvdec		\
			-llcd			\
			-lavcodec		\
			-lavformat		\
			-lavutil
endif
ifeq ($(SUPPORT_OPENH264), true)
LIBS_XRS	+= -lvdec -llcd -lopenh264
endif
LIBS_HI		= -lhi_mpi	\
			  -lhi_mpi_sysbind	\
			  -lhi_mpi_sysmem	\
			  -lhi_mpi_ae		\
			  -lhi_mpi_isp		\
			  -lot_mpi_isp		\
			  -lhi_mpi_awb		\
			  -lhi_ivs_md		\
			  -lhi_mpi_ive		\
			  -lhi_mpi_audio	\
			  -lvqe_hpf	\
			  -lvqe_aec	\
			  -lvqe_anr	\
			  -lvqe_agc	\
			  -lvqe_res	\
			  -lvqe_hs	\
			  -lvqe_talkv2 \
			  -ldehaze			\
			  -lextend_stats	\
			  -ldrc				\
			  -lldci			\
			  -lbnr				\
			  -lcalcflicker		\
			  -lir_auto			\
			  -lacs				\
			  -lsecurec

ifeq ($(findstring OS04D10, $(SENSORS)), OS04D10)
LIBS_HI		+= -lsns_os04d10
endif
ifeq ($(findstring OS04D10_MINOR, $(SENSORS)), OS04D10_MINOR)
LIBS_HI		+= -lsns_os04d10_minor
endif
ifeq ($(findstring OS03A10, $(SENSORS)), OS03A10)
LIBS_HI		+= -lsns_os03a10
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
ifeq ($(findstring GC2083, $(SENSORS)), GC2083)
LIBS_HI		+= -lsns_gc2083
endif
ifeq ($(findstring GC20B3, $(SENSORS)), GC20B3)
LIBS_HI		+= -lsns_gc20b3
endif
ifeq ($(findstring OS04L10, $(SENSORS)), OS04L10)
LIBS_HI		+= -lsns_os04l10
endif
ifneq ($(filter F38P, $(SENSORS)),)
LIBS_HI		+= -lsns_jxf38p
endif

ifneq ($(filter F38PA, $(SENSORS)),)
LIBS_HI         += -lsns_jxf38pa
endif

ifeq ($(SENSORS), Q03)
LIBS_HI		+= -lsns_jxq03
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

LIBS_HI		+= -lhi_mpi_audio	\
			-lupvqe		\
			-ldnvqe		\
			-lvoice_engine

ifeq ($(SUPPORT_AUDIO_VQE_HIGH_VERSION), true)
LIBS_XRS        += -lvqe_talkv2 -lvqe_res
endif

ifeq ($(SUPPORT_BCD), true)
LIBS_XRS	+= -lhi_bcd
endif

ifeq ($(SUPPORT_PQTOOLS), true)
LIBS_HI		+= -l_pqcontrol
endif

ifeq ($(SUPPORT_AIISP), true)
LIBS_HI		+= -laiisp			\
			-lhi_mpi_aibnr		\
			-lsvp_acl		\
			-lprotobuf-c
else
ifeq ($(SUPPORT_PQTOOLS), true)
#LIBS_HI		+= -laiisp			\
#			-lhi_mpi_aibnr		\
#			-lsvp_acl		\
#			-lprotobuf-c
LIBS_HI		+= -lprotobuf-c
endif
endif

# LINUX libs
LINUX_LIB	= -lpthread -lm -lrt -ldl -liconv_pic
ifeq ($(SUPPORT_OPENH264), true)
LINUX_LIB	+= -lstdc++
endif

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

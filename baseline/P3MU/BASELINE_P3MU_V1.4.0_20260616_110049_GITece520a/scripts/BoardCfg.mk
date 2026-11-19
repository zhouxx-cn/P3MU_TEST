#
# Board Config
#

ifeq ($(BOARD_TYPE),)
$(error "BOARD_TYPE not set!")
endif

export BOARD_CFLAGS	:=

ifneq ($(BOARD_TYPE_CFG_MK),$(wildcard $(BOARD_TYPE_CFG_MK)))
$(error "$(BOARD_TYPE_CFG_MK) Not Exist!")
endif 

ifneq ($(BOARD_FUNC_DEF_MK),$(wildcard $(BOARD_FUNC_DEF_MK)))
$(error "$(BOARD_FUNC_DEF_MK) Not Exist!")
endif 

ifneq ($(BOARD_TYPE), X86)
ifneq ($(BOARD_PARTATION_MK),$(wildcard $(BOARD_PARTATION_MK)))
$(error "$(BOARD_PARTATION_MK) Not Exist!")
endif
endif

include $(BOARD_TYPE_CFG_MK)
ifneq ($(BOARD_TYPE), X86)
include $(BOARD_PARTATION_MK)
include $(BOARD_FUNC_DEF_MK)
endif

BOARD_CFLAGS	+= -DPLAT_$(PLATFORM)
BOARD_CFLAGS	+= -DBOARD_$(BOARD_TYPE)
ifdef SENSORS
BOARD_CFLAGS	+= $(foreach SENSOR,$(SENSORS),-DSNR_$(SENSOR))
endif
ifdef FLASH_TYPE
BOARD_CFLAGS	+= -D$(FLASH_TYPE)
endif
ifdef FLASH_SIZE
BOARD_CFLAGS	+= -D$(FLASH_SIZE)
endif


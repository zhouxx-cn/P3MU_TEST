include ../Makefile.param

define make_subsample
	$(call make_subdir, $(SAMPLES_DIR)/$(strip $(1)), $(2), $(3), $(4), $(5))
endef
all: samples

#
# Common Compile Modules
# $(1): Module Name
# 
define compile_driver
SAMPLE = $(strip $(1))

$(SAMPLE):
	$$(call make_subsample, $(SAMPLE), BOARD_TYPE=$$(BOARD_TYPE))

clean-$(SAMPLE):
	$$(call make_subsample, $(SAMPLE), BOARD_TYPE=$$(BOARD_TYPE), clean)
endef

# Driver List
SUB_SAMPLES := sample_he2866 \
			   sample_mauth \
			   sample_upgrade \
			   sample_halmod \
			   sample_expwm \
			   sample_light \
			   sample_hwconf \
			   sample_venc \
			   sample_visp \
			   sample_vosd \
			   sample_vpss \
			   sample_adec \
			   sample_aenc \
			   sample_ai \
			   sample_ao	\
			   sample_wtdg	\
			   sample_xtrace
			   
ifeq ($(SUPPORT_MOTOR), true)
SUB_SAMPLES += sample_motor
endif

CLEAN_SAMPLES := $(addprefix clean-, $(SUB_SAMPLES))

$(foreach SAMPLE, $(SUB_SAMPLES), $(eval $(call compile_driver, $(SAMPLE))))

samples: $(SUB_SAMPLES)

clean: $(CLEAN_SAMPLES)

.PHONY: all install samples clean $(SUB_SAMPLES) $(CLEAN_SAMPLES)

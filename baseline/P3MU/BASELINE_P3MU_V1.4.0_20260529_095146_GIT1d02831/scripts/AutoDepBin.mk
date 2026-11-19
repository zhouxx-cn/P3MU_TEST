#
# Auto Dependency For Binary
#

CFLAGS	+= -fstack-protector-all
ifndef SRCS
SRCS	:= $(wildcard *.c)
endif
OBJS	:= $(SRCS:%.c=%.o)

.PHONY:	clean strip all

ifdef _INSTALL
all: $(_INSTALL)/$(TARGET_BIN)

$(_INSTALL)/$(TARGET_BIN): $(TARGET_BIN)
	$(Q)$(MKDIR) -p $(_INSTALL)
	$(Q)$(CP) $< $(_INSTALL)
endif

$(TARGET_BIN): $(OBJS) $(LIB_DEPEND) $(COPY_TARGET_INCS)
	$(Q)$(ECHO) Compiling [$@], Changed Dep: $?
	$(Q)$(RM) $(TARGET_BIN)*.map
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)
ifneq ($(DEBUG), y)
	$(STRIP) $(TARGET_BIN)
endif
	$(Q)$(ECHO) "==========Excutable Binary" $@ "Build OK =========="

%.o: %.c
	$(Q)$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	$(Q)$(RM) $(OBJS) *.o
	$(Q)$(RM) $(TARGET_BIN)
	$(Q)$(RM) $(MAKE_LOG_FILE)
	$(Q)$(RM) $(TARGET_BIN)*.map
	$(Q)$(RM) $(DEPEND_FILE_NAME)
#ifdef _INSTALL
#	$(Q)$(RM) $(_INSTALL)/$(TARGET_BIN)
#endif
ifdef COPY_TARGET_INCS
	@for x in $(COPY_TARGET_INCS);	do				\
		$(RM) $(COPY_INCS_PATH)/`basename $$x`;		\
	done
endif

strip:
	$(STRIP) -S $(TARGET_BIN)

$(DEPEND_FILE_NAME): $(SRCS) Makefile
	$(Q)$(RM) $@
	@for x in $(SRCS); do									\
		$(CC) -MM -MT"$${x%.c}.o" $(CFLAGS) $$x >> $@; \
		if [ $$? -ne 0 ]; then $(RM) $@; break; fi			\
	done

ifeq (, $(filter $(MAKECMDGOALS), clean m menu))
include $(SRC_SPLINT_MK)
sinclude $(DEPEND_FILE_NAME)
endif

# vim:noet:sw=4:ts=4

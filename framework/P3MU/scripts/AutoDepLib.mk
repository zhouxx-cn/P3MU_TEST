#
# Auto Dependency For Static Library
#

CFLAGS	+= -fstack-protector-all

ifndef SRCS
SRCS	:= $(wildcard *.c)
endif
OBJS	:= $(SRCS:%.c=%.o)

.PHONY: clean

install: $(TARGET_LIB)
ifneq ($(TARGET_LIB_DIR),)
	$(Q)if [ ! -e $(TARGET_LIB_DIR) ]; then $(MKDIR) -p $(TARGET_LIB_DIR); fi
	$(Q)$(CP) -uv $(TARGET_LIB) $(TARGET_LIB_DIR)/$(TARGET_LIB)
endif
ifneq ($(TARGET_HEADER_DIR),)
	$(Q)$(MKDIR) -p $(TARGET_HEADER_DIR)
	@for x in $(TARGET_HEADERS);	do					\
		$(CP) -rf $$x $(TARGET_HEADER_DIR)/`basename $$x`;	\
	done
endif

$(TARGET_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(Q)$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	$(Q)$(RM) $(TARGET_LIB)
	$(Q)$(RM) $(MAKE_LOG_FILE)
	$(Q)$(RM) $(OBJS) *.o *.so
	$(Q)$(RM) $(DEPEND_FILE_NAME)
# 清除时暂时不清除输出
#	$(Q)$(RM) $(TARGET_LIB_DIR)/$(TARGET_LIB)
#ifeq ($(COPY_HEADER_EN), y)
#	@for x in $(TARGET_HEADERS);	do				\
#		$(RM) $(TARGET_HEADER_DIR)/`basename $$x`;		\
#	done
#endif

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

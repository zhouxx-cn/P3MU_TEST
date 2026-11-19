#
# Splint rules
#

ifeq ($(SRCS), )
$(error Error SRC is empty!)
endif

$(SPLINT_TARGET): $(SRCS)
	for x in $(SRCS); do																	\
		$(ECHO) "=========== Splint $$x ==========";										\
		splint +single-include +trytorecover +skipsysheaders +unixlib +bounds +charindex	\
			-bufferoverflowhigh -incondefs -type -nullassign -globstate -initallelements	\
			-preproc -nullret -nullpass -shiftimplementation -formattype -predboolothers	\
			-retvalint-boundswrite -usedef -compdef -retvalother -paramuse -shiftnegative	\
			-predboolint -boolops -fcnuse -exportlocal -compdestroy-mustfreeonly -kepttrans	\
			-unrecog -nullstate -temptrans -immediatetrans -branchstate -uniondef  -badflag	\
			-compmempass -nullderef $(CFLAGS) $$x;											\
	done

# vim:noet:sw=4:ts=4

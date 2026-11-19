#
# Setup Common Used Command
#
export SHELL		= bash
export ECHO 		= echo
export MV			= mv
ifeq ($(RM_DISABLE), y)
unexport RM
else
export RM			= rm -rf
endif
export CP			= cp -rafvp
export DD			= dd
export LN			= ln
export LS			= ls
export CAT			= cat
export TAR			= tar
export MKDIR		= mkdir
export MD5SUM		= md5sum
export SVN			= svn --non-interactive
export PYTHON		= python
export PYTHON2		= python2
export PYTHON3		= python3
export OPENSSL		= openssl
export PRINTF		= printf
export PATCH		= patch
export ICONV		= iconv
export EXIT			= exit
export PUSHD		= pushd > /dev/null
export POPD			= popd > /dev/null
export SOURCE 		= source
export CHMOD		= chmod
export PROTOC_C		= protoc-c

export DONE			= "\033[0m"
export TWINKLE		= "\033[5m"
export BLACK		= "\033[0;30m"
export LIGHT_GRAY	= "\033[0;37m"
export DARK_GRAY	= "\033[1;30m"
export BLUE			= "\033[0;34m"
export LIGHT_BLUE	= "\033[1;34m"
export GREEN		= "\033[0;32m"
export LIGHT_GREEN	= "\033[1;32m"
export CYAN			= "\033[0;36m"
export LIGHT_CYAN	= "\033[1;36m"
export RED			= "\033[0;31m"
export LIGHT_RED	= "\033[1;31m"
export PURPLE		= "\033[0;35m"
export LIGHT_PURPLE	= "\033[1;35m"
export BROWN		= "\033[0;33m"
export YELLOW		= "\033[1;33m"
export WHITE		= "\033[1;37m"

ifeq ($(XQUIET), y)
export MAKE	= make --no-print-directory
else
export MAKE	= make
endif

export IMAGESIGN		= $(PC_TOOLS_DIR)/imagesign
export UPGRADE_MAKER	= $(PC_TOOLS_DIR)/upgrade_maker
export MKENVIMAGE		= $(PC_TOOLS_DIR)/mkenvimage
export READINI			= $(PC_TOOLS_DIR)/readini

ifneq ($(BOARD_TYPE), HOST)
# Cross Compile Define
export CC		= $(CROSS_COMPILE)gcc
export CXX		= $(CROSS_COMPILE)g++
export AR		= $(CROSS_COMPILE)ar
export AS		= $(CROSS_COMPILE)as
export RANLIB	= $(CROSS_COMPILE)ranlib
export STRIP	= $(CROSS_COMPILE)strip
export LD   	= $(CROSS_COMPILE)ld
export OBJCOPY  = $(CROSS_COMPILE)objcopy
else
# Host Compile Define
export CC		= gcc
export CXX		= g++
export AR		= ar
export AS		= as
export RANLIB	= ranlib
export STRIP	= strip
export LD   	= ld
export OBJCOPY  = objcopy
endif

# Compile Commond Define
export make_subdir = 														\
	@if [[ -e $(1) && -d $(1) ]]; then										\
		$(PUSHD) $(1); $(MAKE) $(2) $(3) $(4) || $(EXIT) "$$?"; $(POPD);	\
	else																	\
		$(ECHO) "***Error: Subdir $(1) not exist";							\
		$(EXIT) -1;															\
	fi

#export find_newest_file =								\
#	$(shell find $(1) -type f | grep -vE "dep.mk.rej|*\.o|*\.depend|*\.su|*\.dep" | xargs ls -ltr | tail -n 1 | awk '{print $$9}')

export find_newest_file = 								\
	$(shell 											\
		find $(1) -type f -printf "%T@ %p\n" | 			\
		grep -E "Makefile|*\.c|*\.h|*\.cpp|*\.hpp|*\.mk" |	\
		awk '											\
		BEGIN { recent = 0; file = "" }					\
		{if ($$1 > recent){recent = $$1;file = $$0;}}	\
		END { print file; }' | 							\
		sed 's/^[0-9]*\.[0-9]* //'						\
	)

export find_newest_file_all = 							\
	$(shell 											\
		find $(1) -type f -printf "%T@ %p\n" | 			\
		awk '											\
		BEGIN { recent = 0; file = "" }					\
		{if ($$1 > recent){recent = $$1;file = $$0;}}	\
		END { print file; }' | 							\
		sed 's/^[0-9]*\.[0-9]* //'						\
	)

export uppercase = 										\
	$(shell echo $(1) | tr "[:lower:]" "[:upper:]")

export lowercase = 										\
	$(shell echo $(1) | tr "[:upper:]" "[:lower:]")

export check_filesize = \
	@total=`wc -c $(1)  | tail -n 1 | awk '{print $$1}'`;		\
	if [ $$total -gt $(2) ]; then								\
		echo "****[ERROR] file($(1)) size $$total bigger then limit $(2).";	\
		exit -1;												\
	fi														

# vim:noet:sw=4:ts=4

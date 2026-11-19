TESTFLAGS+= -lpthread -lSTR -lPAR -lpkgstream -ldisk -lusb -lhybackup -lsd
LFLAGS+= -lpthread 
#CFLAGS+= -D__PKGSTREAM__
CFLAGS+= -D__JFS__ -D__USER_OPEN__ -D__CI785__
#CFLAGS+= -D__JFS__ 
INCPATH+= -I../ -I../PAR -I./ -I../SnapShot -I../AVL
LIBPATH+= -L../lib -L../../ -L../../../modules/lib -L$(CROSS_COMPILE_PATH)/lib -L$(CROSS_COMPILE_PATH)/usr/lib
#CFLAGS += -D TEST_MEM_INFO -D USE_MEMORY_POOL
include ../common.mak
test:
	$(MAKE) -C test clean
	$(MAKE) -C test

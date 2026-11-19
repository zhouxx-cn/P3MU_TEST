#TESTFLAGS+= -lpthread -lSTR -lPAR -lpkgstream -ldisk -lusb -SnapShot
LFLAGS+= -lpthread 
INCLUDES+= -I../ -I../PAR -I./ 
CFLAGS+= -D__CI785__
LIBPATH+= -L../lib -L../../ -L../../modules/lib -L$(CROSS_COMPILE_PATH)/lib -L$(CROSS_COMPILE_PATH)/usr/lib
#CFLAGS += -D TEST_MEM_INFO  -D USE_MEMORY_POOL
include ../common.mak

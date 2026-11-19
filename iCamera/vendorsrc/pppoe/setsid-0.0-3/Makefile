DESTDIR=

prefix=${DESTDIR}/usr
bindir=${prefix}/bin
datadir=${prefix}/share
mandir=${datadir}/man/man8
cygdocdir=${datadir}/doc/Cygwin

#EXEEXT=.exe

TARGET=	setsid$(EXEEXT)

CC=arm-none-linux-gnueabi-gcc
CFLAGS=-g -O2
LDFLAGS=

OBJ=    setsid.o

all: $(TARGET)

setsid$(EXEEXT): $(OBJ)
	$(CC) -s -o $@ $(LDFLAGS) $(OBJ)

install: setsid$(EXEEXT) setsid.8
	install -d $(bindir)
	install -d $(mandir)
	install -d $(cygdocdir)
	install -s setsid$(EXEEXT) $(bindir)/setsid$(EXEEXT)
	install setsid.8 $(mandir)/setsid.8
	install setsid.README $(cygdocdir)/setsid.README

clean:
	rm $(TARGET) $(OBJ)


#!make -f
#SUBDIRS=`ls -lh |awk 'BEGIN {CURDIR=""}; /^d/ {CURDIR= CURDIR=="" ? $$9 : sprintf(" %s", $$9); print CURDIR}'`
SUBDIRS=`./protocol.sh`
SUBDIR=
all:
	rm -rf ../lib/lib*.so
	for SUBDIR in $(SUBDIRS);do cd $$SUBDIR && make; cd ..; done
clean:
	for SUBDIR in $(SUBDIRS); do cd $$SUBDIR && make clean; cd ..; done	

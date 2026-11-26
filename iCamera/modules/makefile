#!/usr/bin/make -f

include ../DVR_SDK/profile

MODS:= md5 moto base64 adsl avi ddt disk ip watch memrecord sd_ops SD wifi hybackup iniparser CircleQueue common XRSBleCommuProtocol mp4

MODS += watch/WatchDaemon

.PHONY: relase clean 

RELEASE = cd $(mod) && $(MAKE); [ $$? -ne 0 ] && echo make lib$(mod).a error!!! && exit 1; cd ..;
CLEAN = cd $(mod) && $(MAKE) -w clean; cd ..;

release:
	$(foreach mod, $(MODS), $(RELEASE))

clean:
	$(foreach mod, $(MODS), $(CLEAN))

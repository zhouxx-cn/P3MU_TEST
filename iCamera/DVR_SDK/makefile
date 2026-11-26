#!/usr/bin/make -f

MODS:=mysystem Authorize NET PAR PUB SYS SnapShot PRO PUB_BORAD ui ThumbCamDemo

.PHONY:release clean

RELEASE = cd $(mod) && $(MAKE); [ $$? -ne 0 ] && echo make lib$(mod).a error!!! && exit 1; cd ..;
CLEAN = cd $(mod) && $(MAKE) -w clean; cd ..;

release:
	$(foreach mod, $(MODS), $(RELEASE))

clean:
	$(foreach mod, $(MODS), $(CLEAN))


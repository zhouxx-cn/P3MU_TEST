#!/usr/bin/make -f

MODS:=mysystem Authorize AVL COM DOP NET OPL PAR PTZ PUB RNL SYS USR MPU STR SnapShot PRO LCD PUB_BORAD

.PHONY:release clean

RELEASE = cd $(mod) && $(MAKE); [ $$? -ne 0 ] && echo make lib$(mod).a error!!! && exit 1; cd ..;
CLEAN = cd $(mod) && $(MAKE) -w clean; cd ..;

release:
	$(foreach mod, $(MODS), $(RELEASE))

clean:
	$(foreach mod, $(MODS), $(CLEAN))


#!/usr/bin/make -f

MODS:= BoardAuthInit MonitorProcess calc_ip checkSDCard com_over_telnet outputFlow savetime wifi_detect sd_upgrade RShell

.PHONY:release clean

RELEASE = cd $(mod) && $(MAKE); [ $$? -ne 0 ] && echo make $(mod) error!!! && exit 1; cd ..;
CLEAN = cd $(mod) && $(MAKE) -w clean; cd ..;

release:
	$(foreach mod, $(MODS), $(RELEASE))

clean:
	$(foreach mod, $(MODS), $(CLEAN))


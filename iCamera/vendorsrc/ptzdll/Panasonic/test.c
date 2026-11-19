#include <stdio.h>
#include <dlfcn.h>
#include "Ttydevcommon.h"

//获取该协议支持的特性
void (*GetTTYDriverInfo) (TTYINFO * tty);

int main()
{
	void *libid;

	char *error;

	TTYINFO ttyinfo;

	libid = dlopen("./libPelcop.so", RTLD_LAZY);
	error = dlerror();
	if (error)
	{
		printf("open dll error\n");
		return 1;
	}
	GetTTYDriverInfo = dlsym(libid, "GetTTYDriverInfo");
	error = dlerror();
	if (error)
	{
		dlclose(libid);
		printf("Can't load dll\n");
		return 1;
	}
	GetTTYDriverInfo(&ttyinfo);
	printf("Ttyinfo:%d,%d,%d\n", ttyinfo.dwPresetCount, ttyinfo.dwCruiseCount, ttyinfo.dwTrackCount);
	dlclose(libid);
}

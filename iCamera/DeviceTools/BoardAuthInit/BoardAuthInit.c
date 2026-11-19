
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/time.h>
#include "sensor_user.h"

//#include "mysystem.h"

#include "Authority.h"

#define INIT_ERR(format, args...)  do{printf("\033[1;31m[BOARD_INIT][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define INIT_DBG(format, args...)  do{printf("\033[1;32m[BOARD_INIT][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define INIT_INF(format, args...)  do{printf("\033[1;35m[BOARD_INIT][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define SYS_ERR(format, args...)   do{printf("\033[1;32m[BOARD_INIT][%s-%d](%s) \033[m" format, __FILE__, __LINE__, strerror(errno), ##args);}while(0)

#define SENSOR_NAME_PATH		"/tmp/.SensorName"

static int ShellCmdOut(const char *cmd, char *result, int len) 
{  
	struct timeval now;
	char fileName[64];
	char command2[256];
	int ret = 0;
	FILE *fp = NULL;

	if(!cmd || !strlen(cmd) || len<=0)	
		return -1;

	gettimeofday(&now, NULL);

	memset(fileName, 0, sizeof(fileName));
	snprintf(fileName, sizeof(fileName), "/tmp/.ShellCmdOut%d", now.tv_usec);

	memset(result, 0, len); 
	memset(command2, 0, sizeof(command2));
	snprintf(command2, sizeof(fileName), "%s > %s", cmd, fileName);

	mysystem(command2);
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		perror("fopen");
		ret = -1;
	}
	else
	{
		fread(result, sizeof(char), len, fp);
		printf("result: %s", result);

		if(fclose(fp))
		{
			perror("fclose");
			ret = -1;
		}
	}
	
	memset(command2, 0, sizeof(command2));
	snprintf(command2, sizeof(command2), "rm -rf %s", fileName);
	mysystem(command2);
	
	return ret; 							   
}

int AdjustSensorType(void)
{
	int iRet = 0;
	unsigned char acSensorName[32] = {0};
	FILE *fp = NULL;

	snprintf(acSensorName, sizeof(acSensorName), "%s", "JXF38P");

	if (0 == iRet)
	{
		fp = fopen(SENSOR_NAME_PATH, "w+");
		if (NULL == fp)
		{
			INIT_ERR("Fail to fopen %s\n", SENSOR_NAME_PATH);
			return -1;
		}
		
		fwrite(acSensorName, strlen(acSensorName), 1, fp);//写入需要写的那一行
		INIT_INF("SensorName=[%s]\n", acSensorName);
		fclose(fp);

		return 0;
	}
	else
	{
		INIT_ERR("Fail to called sensor_user_i2c_get_id\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
    /* Step 1: 从I2C和SPI里面适配sensor，设置到u-boot环境变量里面 */
    INIT_DBG("Start to adjust sensor type ...\n");
#if 1
    if (AdjustSensorType())
    {
        INIT_ERR("Fail to called AdjustSensorType\n");
    }
#endif
    /* Step 2: 授权验证 */
    INIT_DBG("Start to init license authorize ...\n");
    if (InitLicenseAuthorize())
    {
        INIT_ERR("Fail to called InitLicenseAuthorize\n");
    }

	return 0;
}

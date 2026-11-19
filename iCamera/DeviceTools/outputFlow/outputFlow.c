#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "CPassengerFlow.h"

#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

#define	ZW_PRINT(format, args...)	 do {	 \
        printf(format, ##args);  \
      } while (0)
      

#define ZW_RED(format, args...)	 ZW_PRINT(LIGHT_RED format NONE, ##args)
#define ZW_GREEN(format, args...)	 ZW_PRINT(LIGHT_GREEN format NONE, ##args)
#define ZW_BLUE(format, args...)	 ZW_PRINT(LIGHT_BLUE format NONE, ##args)
#define ZW_PURPLE(format, args...)	 ZW_PRINT(LIGHT_PURPLE format NONE, ##args)
#define ZW_NONE(format, args...)	 ZW_PRINT(NONE format NONE, ##args)

static PassengerFlowDataInfo_S g_flowData = {0};

int loadPassengerFlowFile()
{
	int fd = 0;
	int ret = 0;
	char *pPoint = NULL;
	struct stat buf;
	PassengerFlowFileHead_S headInfo = {0};

	pPoint = (char *)(&g_flowData);

    if (access(PASSENGER_FLOW_FILE_PATH, F_OK))//如果不存在客流文件,
	{
		printf("%s not exist!!!\n");
		return -1;
	}
	else//如果文件存在，则先校验，然后读取到缓存
	{
		//打开文件
		fd = open(PASSENGER_FLOW_FILE_PATH, O_RDONLY);
		if(fd <= 0)
		{
			ZW_RED("Open file %s fail!\n", PASSENGER_FLOW_FILE_PATH);
			return -1;
		}
		
		fstat(fd, &buf);

		if(PASSENGER_FLOW_FILE_SIZE == buf.st_size)
		{
			ret = read(fd, pPoint, PASSENGER_FLOW_FILE_SIZE);
			if(ret != PASSENGER_FLOW_FILE_SIZE)//读取失败，没有继续尝试，那么意味着原始数据将会被清空
			{
				ZW_RED("Read %s length %d fail, read length %d!\n", PASSENGER_FLOW_FILE_PATH, PASSENGER_FLOW_FILE_SIZE, ret);
				if(ret > PASSENGER_FLOW_FILE_HEAD_LENTH)
				{
					if(g_flowData.headInfo.version != PASSENGER_FLOW_FILE_FORMAT_VERSION)
					{
						ZW_RED("file version is %d, soft versiong is %d\n", g_flowData.headInfo.version, PASSENGER_FLOW_FILE_FORMAT_VERSION);
					}
				}
				
				close(fd);
				return -1;
			}
			
			//校验文件头异常则丢弃
			if((PASSENGER_FLOW_FILE_START_CODE != g_flowData.headInfo.startCode)
				|| (g_flowData.headInfo.index >= MAX_PASSENGER_FLOW_NUMBER_OF_DAYS)
				|| (g_flowData.headInfo.version != PASSENGER_FLOW_FILE_FORMAT_VERSION))
			{
				ZW_RED("Check %s startCode 0x%x or index %d error or file version is %d != soft versiong is %d!\n", g_flowData.headInfo.startCode, g_flowData.headInfo.index,
					g_flowData.headInfo.version, PASSENGER_FLOW_FILE_FORMAT_VERSION);
				close(fd);
				return -1;
			}

			//
			close(fd);
			ret = 0;
		}
		else//长度不等，认为文件破坏，可以有两种选择，恢复部分数据或者清空，这里不处理即是清空，下次写入时会将缓存的值刷入文件
		{
			printf("file %s length %d error, not %d!\n", PASSENGER_FLOW_FILE_PATH, buf.st_size, PASSENGER_FLOW_FILE_SIZE);
			ret = read(fd, &headInfo, PASSENGER_FLOW_FILE_HEAD_LENTH);
			if(ret != PASSENGER_FLOW_FILE_HEAD_LENTH)//读取失败，没有继续尝试，那么意味着原始数据将会被清空
			{
				ZW_RED("Read %s file head fail!\n", PASSENGER_FLOW_FILE_PATH, PASSENGER_FLOW_FILE_HEAD_LENTH);
				close(fd);
				return -1;
			}

			if(headInfo.version != PASSENGER_FLOW_FILE_FORMAT_VERSION)
			{
				ZW_RED("file version is %d, soft versiong is %d\n", headInfo.version, PASSENGER_FLOW_FILE_FORMAT_VERSION);
			}
			
			close(fd);
			return -1;
		}
				
		return ret;
	}
}

void dumpPassengerFlow()
{
	int i,j;

	ZW_NONE("==========================================Start=======================================\n");
	for(i=0; i<MAX_PASSENGER_FLOW_NUMBER_OF_DAYS; i++)
	{
		if(g_flowData.headInfo.index == i)//
		{
			ZW_RED("***********************************************Date:%s***********************************************\n", g_flowData.oneDayFlowData[i].strDateTime);
		}
		else
		{
			ZW_NONE("***********************************************Date:%s***********************************************\n", g_flowData.oneDayFlowData[i].strDateTime);
		}

		ZW_GREEN("Hours:|");
		for(j=0; j<ONE_DAY_HOURS; j++)
		{
			ZW_GREEN("%4d|", j);
		}
		ZW_GREEN("\n");

		ZW_NONE("In   :|");
		for(j=0; j<ONE_DAY_HOURS; j++)
		{
			if(g_flowData.oneDayFlowData[i].oneHoursFlowData[j].in)
			{
				ZW_PURPLE("%4d", g_flowData.oneDayFlowData[i].oneHoursFlowData[j].in);
				ZW_NONE("|");
			}
			else
			{
				ZW_NONE("%4d", g_flowData.oneDayFlowData[i].oneHoursFlowData[j].in);
				ZW_NONE("|");
			}
		}
		ZW_NONE("\n");

		ZW_NONE("Out  :|");
		for(j=0; j<ONE_DAY_HOURS; j++)
		{
			if(g_flowData.oneDayFlowData[i].oneHoursFlowData[j].out)
			{
				ZW_PURPLE("%4d", g_flowData.oneDayFlowData[i].oneHoursFlowData[j].out);
				ZW_NONE("|");
			}
			else
			{
				ZW_NONE("%4d", g_flowData.oneDayFlowData[i].oneHoursFlowData[j].out);
				ZW_NONE("|");
			}
		}
		ZW_NONE("\n");
	}

	ZW_NONE("========================================== end =======================================\n");
	
}

int main(int argc, char **argv)
{
	int ret = 0;
	
	ret = loadPassengerFlowFile();

	if(!ret)
	{
		dumpPassengerFlow();
	}

}

// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef __TRAFFICSTATISTICS_H__
#define __TRAFFICSTATISTICS_H__

#define  PASSENGER_FLOW_FILE_FORMAT_VERSION (2)
#define  MAX_AREADEFENSE_NUM 1
#define	 MAX_PASSENGER_FLOW_NUMBER_OF_DAYS (7)	//客流统计保存的最大天数
#define	 ONE_DAY_HOURS (24)
#define  WRITE_FILE_INTERVAL_SECOND (30)
#define  PASSENGER_FLOW_FILE_PATH "/mnt/mtd/config/passengerFlow.db"
#define  PASSENGER_FLOW_FILE_BAK_PATH "/mnt/mtd/config/passengerFlow.db-bak"
#define  PASSENGER_FLOW_FILE_SIZE (sizeof(PassengerFlowDataInfo_S))  //客流统计的数据文件总大小，长度是固定的，且每个节点的位置也是固定的
#define  PASSENGER_FLOW_ONE_DAY_DATA_LENTH (sizeof(PassengerFlowOneDayDataInfo_S))  //客流统计一天的数据量占的字节长度
#define	 PASSENGER_FLOW_ONE_HOURS_DATA_LENTH (sizeof(PassengerFlowUnitDataInfo_S))  //客流统计一个小时的数据量占的字节长度
#define  PASSENGER_FLOW_FILE_HEAD_LENTH (sizeof(PassengerFlowFileHead_S))  //客流文件头结构体长度

#define	 PASSENGER_FLOW_DATE_DATA_LENTH (8)  //客流统计日期占的字节长度
#define  PASSENGER_FLOW_FILE_START_CODE (0x5852533A)   //"XRS:"
#define	 PASSENGER_FLOW_REGION_POINT_NUM (4)
#define  DEBUG_DUMP_PASSENGER_FLOW_CACHE_FILE_FLAG "/tmp/outputFlow"

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

//!一个小时的客流数据状态
typedef struct 
{
	unsigned char valid;//该节点是否有效，即有数据
	unsigned char hoursDataUpdate;//该小时的数据是否有更新，作为写文件时刷数据的判断依据，写入文件后要清空
} PassengerFlowUnitStatusInfo_S;

//!一天的客流数据状态
typedef struct 
{
	int valid;//该天是否有客流数据

	int Year;
	int Month;
	int Day;
	
	int clearDayFlag;//清空该天的数据标记,如果置位了表示需要覆盖该天的数据，那么需要立马将该天同步到文件，将整天的都写入文件后要清空
	int	dayDataUpdate;//该天的数据是否有更新，作为写文件时遍历哪天数据有变化的判断依据，该整天的都写入文件后要清空

	PassengerFlowUnitStatusInfo_S oneHoursFlowStatus[ONE_DAY_HOURS];//该天24小时的客流状态
} PassengerFlowOneDayStatusInfo_S;

//!一个小时的客流数据
typedef struct 
{
	char valid;//该小时是否有客流数据
	unsigned int in;//进入的统计值
	unsigned int out;//离开的统计值
}__attribute__((packed)) PassengerFlowUnitDataInfo_S;

//!一天的客流数据
typedef struct 
{
	char valid;//该天是否有客流数据
	char strDateTime[PASSENGER_FLOW_DATE_DATA_LENTH+1];//日期字符串，精确到天，如“20211021”

	PassengerFlowUnitDataInfo_S oneHoursFlowData[ONE_DAY_HOURS];//该天24小时的客流
}__attribute__((packed)) PassengerFlowOneDayDataInfo_S;

//!一天的客流数据
typedef struct 
{
	unsigned int startCode;//日期字符串，精确到天，如“20211021”
	unsigned char index;//状态,0~(MAX_PASSENGER_FLOW_NUMBER_OF_DAYS-1),标识当前写到哪个节点
	unsigned char version;//版本号，从2开始，如果版本不匹配则删除整个客流统计数据文件
	unsigned char szReserved[2];
}__attribute__((packed)) PassengerFlowFileHead_S;

//!当天的累计客流数据
typedef struct 
{
	int Year;
	int Month;
	int Day;

	unsigned int in;//进入的统计值
	unsigned int out;//离开的统计值
} PassengerFlowAllDataInDay_S;

//!设备的客流数据
typedef struct 
{
	PassengerFlowFileHead_S headInfo;

	PassengerFlowOneDayDataInfo_S oneDayFlowData[MAX_PASSENGER_FLOW_NUMBER_OF_DAYS];//客流的数据，对应到文件
}__attribute__((packed)) PassengerFlowDataInfo_S;

#endif							/* _INC_CHUMANSHAPE_49B0E4200206_INCLUDED */


#ifndef __PUB_BOARD_INFO_H__
#define __PUB_BOARD_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif



/****************根据版型添加宏定义****************************/
#define HS610_G85T_BOARD_ID    0x216D
#define HS610_W85T_BOARD_ID    0x216B
#define HS608_P4_BOARD_ID      0x2171
#define HS608_P3MU_BOARD_ID    0x2174
#define HS608_G43TA_BOARD_ID    0x2175
#define HS608_P4U_BOARD_ID     0x2176
#define HS610_G84F_BOARD_ID    0x2179
#define HS610_W84F_BOARD_ID    0x217D
#define HS610_G63F_BOARD_ID    0x217E
#define HS610_W63F_BOARD_ID    0x217F

/**************************************************************/


/* 注意：每次添加一个新的版型，都要在下面添加一个新的类型 */
/****************根据版型添加宏定义****************************/
#define HS610_G85T_DEVICE_ID   0x5184
#define HS610_W85T_DEVICE_ID   0x5182
#define HS608_P4_DEVICE_ID     0x5189
#define HS608_P3MU_DEVICE_ID   0x518E
#define HS608_G43TA_DEVICE_ID   0x518F
#define HS608_P4U_DEVICE_ID    0x5190
#define HS610_G84F_DEVICE_ID   0x5193
#define HS610_W84F_DEVICE_ID   0x5197
#define HS610_G63F_DEVICE_ID   0x5198
#define HS610_W63F_DEVICE_ID   0x5199

/**************************************************************/

/**************************************
注意该枚举和gstuBoardInfo数组中严格对应
下标从0开始，不允许随意更改顺序  
**************************************/
typedef enum
{
	eBType_G85T = 0,
	eBType_W85T = 1,
	eBType_P4	= 2,
	eBType_P3MU	= 3,
	eBType_G43TA= 4,
	eBType_P4U	= 5,
	eBType_G84F	= 6,
	eBType_W84F	= 7,
	eBType_G63F	= 8,
	eBType_W63F	= 9,
}eBoardTypeSet;


typedef struct tagBoardInfo{
    eBoardTypeSet eBtypeNum;
    unsigned long dwBoardId;
    unsigned long nDeviceType;
    unsigned char szBoardType[32];
    unsigned char szSensorName[32];
}stuBoardInfo, *pBoardInfo;

// 声明为外部常量
extern const stuBoardInfo gstuBoardInfo[];
extern const unsigned int gstuBoardInfoSize;

//根据BoardId返回BoardType
const char* PUB_FindBoardTypeInfoById(unsigned long nBoardId);


//根据BoardId返回DeviceType
unsigned long PUB_FindDeviceTypeInfoById(unsigned long nBoardId);





#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#include <stdio.h>
#include "PUB_BoradInfo.h"


// 定义版型数组常量
const stuBoardInfo gstuBoardInfo[] = {
    {eBType_G85T,    HS610_G85T_BOARD_ID,      HS610_G85T_DEVICE_ID,      "G85T-50PHM-S/G",   "F38PA"},
    {eBType_W85T,    HS610_W85T_BOARD_ID,      HS610_W85T_DEVICE_ID,      "W85T-50PHM-S/W",   "F38PA"},
    {eBType_P4,    	 HS608_P4_BOARD_ID,        HS608_P4_DEVICE_ID,        "P4-52PHR-S/W/BT",  "JXK06A"},
    {eBType_P3MU,    HS608_P3MU_BOARD_ID,      HS608_P3MU_DEVICE_ID,      "P3MU-52PGR-S/W/BT", "GC20C3"},
    {eBType_G43TA,    HS608_G43TA_BOARD_ID,      HS608_G43TA_DEVICE_ID,   "G43TA-52PGM-S/G",  "GC20C3"},
    {eBType_P4U,    HS608_P4U_BOARD_ID,        HS608_P4U_DEVICE_ID,       "P4U-52PHR-S/W/BT",  "JXK06A"},
    {eBType_G84F,    HS610_G84F_BOARD_ID,      HS610_G84F_DEVICE_ID,      "G84F-50PHM-S/G",   "GC20C3"},
    {eBType_W84F,    HS610_W84F_BOARD_ID,      HS610_W84F_DEVICE_ID,      "W84F-50PGM-S/W/BT",   "GC20C3"},
    {eBType_G63F,    HS610_G63F_BOARD_ID,      HS610_G63F_DEVICE_ID,      "G63F-50PGM-S/G",   "GC20C3"},
    {eBType_W63F,    HS610_W63F_BOARD_ID,      HS610_W63F_DEVICE_ID,      "W63F-50PGM-S/W/BT","GC20C3"},
};

// 定义数组大小常量
const unsigned int gstuBoardInfoSize = sizeof(gstuBoardInfo) / sizeof(gstuBoardInfo[0]);

//根据BoardId返回BoardType
const char* PUB_FindBoardTypeInfoById(unsigned long nBoardId)
{
    for (unsigned int i = 0; i < gstuBoardInfoSize; i++)
    {
        if (gstuBoardInfo[i].dwBoardId == nBoardId)
        {
            return (const char *)gstuBoardInfo[i].szBoardType;
        }
    }

    return NULL;
}

//根据BoardId返回DeviceType
unsigned long PUB_FindDeviceTypeInfoById(unsigned long nBoardId)
{
    for (unsigned int i = 0; i < gstuBoardInfoSize; i++)
    {
        if (gstuBoardInfo[i].dwBoardId == nBoardId)
        {
            return gstuBoardInfo[i].nDeviceType;
        }
    }

    return 0;
}

const stuBoardInfo* PUB_GetBoardInfoArray(void)
{
    return gstuBoardInfo;
}

unsigned int PUB_GetBoardInfoCount(void)
{
    return gstuBoardInfoSize;
}


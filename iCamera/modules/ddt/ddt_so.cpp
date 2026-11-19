
/*********************************************************************
* 版权所有 (C)2008, 武汉恒亿电子科技发展有限公司。
* 
* 文件名称： ddt_so.cpp
* 文件标识： 暂无
* 内容摘要： 实现嵌入式DVR/S的功能描述的管理， 供嵌入式DVR/S动态获取设备的功能表。
* 其它说明： 当增加新的设备型号时， 需手动把设备的信息录入到libddt.so文件中。
* 当前版本： 1.0.0
* 作    者： 张林 (zl@higheasyrd.com)
* 完成日期： 2008.6.2
* 
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
**********************************************************************/

#include <stdio.h>
#include "ddt_so.h"
#include "BuildDD.h"
#include "sys_banned.h"
#include "PUB_BoradInfo.h"

/**************************************************************************
*                          常  量                                        *
**************************************************************************/


/**************************************************************************
*                            宏                                          *
**************************************************************************/


/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

struct ddt_map_by_id
{
	ULONG ulBoardID;
	int (*build_ddt_by_id) (LPDeviceDescription pDD, const ULONG BoardID);
};

struct ddt_map_by_name
{
	char uddt_name[MAX_NAME_LEN];
	int (*build_ddt_by_name) (LPDeviceDescription pDD, const CHAR * BoardName);
};

/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

/*!定义版本信息*/
char szVersion_DDT_20080603[] = "1.0.0.941 Build " __DATE__;

/**************************************************************************
*                           局部函数原型                                  *
**************************************************************************/

/*!
* \brief
*  根据设备型号名称来获取设备能力描述表
* 
* \param pDD
*  存放设备能力的缓冲区
* \param szTypeName
*  设备型号名称
*
* \return
*  返回0表示成功， 其它值表示失败
* 
* \see
*  EDVRS_DDT_GetDeviceDescriptionByID.
*/
DDT_API INT EDVRS_DDT_GetDeviceDescriptionByName(OUT LPDeviceDescription pDD, IN const char *szTypeName)
{

	return ERR_DDT_NOT_IMPLEMENT;
}


/*!
* \brief
*  根据BoardID来获取设备能力描述表
* 
* \param pDD
*  存放设备能力的缓冲区
* \param BoardID
*  设备板型ID
* 
* \return
*  返回0表示成功， 其它值表示失败
* 
* \see
*  EDVRS_DDT_GetDeviceDescriptionByName.
*/
DDT_API INT EDVRS_DDT_GetDeviceDescriptionByID(OUT LPDeviceDescription pDD, IN const ULONG BoardID)
{
	INT nRet = ERR_DDT_NOT_SUPPORTED;

	unsigned int iItemIndex = 0;

	static ULONG LastBoardID = 0;

	if (NULL == pDD)
		return ERR_DDT_INVALID_PARAM;

	if (BoardID <= 0)
		return ERR_DDT_INVALID_PARAM;

	for (iItemIndex = 0; iItemIndex < gstuBoardInfoSize; iItemIndex++)
	{
		if (BoardID == gstuBoardInfo[iItemIndex].dwBoardId)
		{
			nRet = BuildDeviceDescriptionByID(pDD, BoardID);
			if (BoardID != LastBoardID)
			{
				LastBoardID = BoardID;
//				DumpDeviceInfo(pDD);
			}
			break;
		}
	}

	return nRet;
}

/*!
* \brief
*  启动设备描述表枚举功能
* 
* \see
*  EDVRS_DDT_EnumDeviceDescripiontTable.
*/
DDT_API VOID EDVRS_DDT_EnumDeviceDescriptionTableStart()
{

}

/*!
* \brief
*  获取设备描述表枚举信息
* 
* \param
*  存放设备能力的缓冲区
*
* \return
*  返回0表示成功， 其它值表示失败
*/
DDT_API INT EDVRS_DDT_EnumDeviceDescripiontTable(OUT LPDeviceDescription pDD)
{
	return ERR_DDT_NOT_IMPLEMENT;
}



/*!
* \brief
*  停止设备描述表枚举功能
* 
* \see
*  EDVRS_DDT_EnumDeviceDescripiontTable.
*/
DDT_API VOID EDVRS_DDT_EnumDeviceDescriptionTableStop()
{
}

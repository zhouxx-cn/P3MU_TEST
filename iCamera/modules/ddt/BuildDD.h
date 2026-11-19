
/*********************************************************************
* 版权所有 (C)2008, 武汉恒亿电子科技发展有限公司。
* 
* 文件名称： BuildDD.h
* 文件标识： 暂无
* 内容摘要： 描述内嵌设备描述信息的接口
* 其它说明： 。
* 当前版本： 1.0.0
* 作    者： 张林 (zl@higheasyrd.com)
* 完成日期： 2008.6.3
* 
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
**********************************************************************/
#ifndef __HIGHEASY_EDVRS_BUILDDD_20080603__
#define __HIGHEASY_EDVRS_BUILDDD_20080603__

int BuildDeviceDescriptionByID(LPDeviceDescription pDD, const ULONG nBoardID);


VOID DumpDeviceInfo(const LPDeviceDescription pDD);
#endif

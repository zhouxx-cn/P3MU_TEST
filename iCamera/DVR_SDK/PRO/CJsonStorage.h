#ifndef __CJSON_STORAGE_H__
#define __CJSON_STORAGE_H__

#include <stdio.h>

int Json_GetSdInfo(char *nBuffer, int nBufferLenth, int nTestType);

int Json_SdFormat(char *nBuffer, int nBufferLenth, int nTestType);

#ifdef __MODULE_4G_SUPPORT__

/*@breif Json_4GBuiltCardTest 获取内置卡产测数据
 *@param 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Json_4GBuiltCardTest(char *nBuffer,int nBufferLenth);

/*@breif Json_4GExternalCardTest 获取外置卡产测数据
 *@param 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Json_4GExternalCardTest(char *nBuffer,int nBufferLenth);

int Json_4GCardInfo(char *nBuffer,int nBufferLenth, int nTestType);

int Json_4GSignalQuality(char *nBuffer, int nBufferLenth, int nTestType);

#endif

#endif

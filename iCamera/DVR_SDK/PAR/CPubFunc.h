#ifndef __CPUBFUNC_H__
#define __CPUBFUNC_H__


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <wchar.h>
#include <time.h>
#include <math.h>
#include <stddef.h>
#include <assert.h>
#include <sys/time.h>
#include "global_constant.h"




/*@brief     StringExistInFile 查看文件中是否存在字符串
 *@param[in] string      欲查找的字符串
 *@param[in] stringSize  字符串大小(建议使用strlen(string)+1)
 *@param[in] fp          文件指针
 *@return    字符串存在返回1, 否则返回0
 */
int StringExistInFile(const char * string, int stringSize, FILE * fp);
/*@brief      GetIniSectionItem 获取ini文件键值内容
 *@param[in]  FileName ini文件路径 
 *@param[in]  Section  中括号引用的字段
 *@param[in]  Item     预查找的键
 *@param[out] Item     获取到的值
 *@return    成功返回0，错误返回-1
 *@note      当项目在文件中有多个时，返回第一个项目的值(包括段和键)
 * 			 支持缺省，如果没有找到该段或者键值，则直接返回NULL
 * 			 支持等号空格，返回字符串会原样输出(可以包括空格)
 * 			 允许不同段下面的键值不同
 *           不支持注释
 * 			 仅支持全字匹配，如下ini配置文件:
 * 			 ABD12345=123
 * 			 ABD1=TEST
 * 			 在查找的时候，如果查找的键值为ABD1，则得到的值为123，而不是TEST
 */
int GetIniSectionItem(const char* FileName, const char* Section, const char* Item, char* pValue, int valueSize);

/* 获取当前使用的网卡名称 */
int GetCurrNetCardType(char *ifname, int strSize);

/**************************************************************************
* 函数名称：int test_lite_big()
* 功能描述： 获取大小段模式。
* 输入参数： 无
* 输出参数： 无
* 返 回 值：     小端返回1，大端返回0
* 其它说明： 无
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 08/02/21	     V1.0	    cwf	      创建
**************************************************************************/
int test_lite_big();

//功能:在文件长度为insertPoint的地方插入一段内容content，copyPoint后面的内容
//拷贝到content后面。insertPoint  <= copyPoint,一行小于1024个字节
int insertDateToFile(const char *FileName, long insertPoint, long copyPoint, const char * content);

#endif

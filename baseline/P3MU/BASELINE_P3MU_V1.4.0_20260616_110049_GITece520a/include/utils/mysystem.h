
#ifndef __MYSYSTEM_H__
#define __MYSYSTEM_H__

/*!
* \brief
* 执行shell命令的接口函数.
* 
* \param cmdstring
* shell命令串.s
* 
* \returns
* 执行成功返回0，否则返回非0.
*/
//#define system mysystem

#ifdef __cplusplus
extern "C"
{
#endif
	int mysystem(const char *cmdstring);
#ifdef __cplusplus
}
#endif

#endif

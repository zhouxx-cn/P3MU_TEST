
#ifndef __MYSYSTEM_H__
#define __MYSYSTEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*** 
 * @description: 执行shell命令的接口函数.
 * @param {char} *cmdstring hell命令串
 * @return {int} 执行成功返回0，否则返回非0.
 */
int mysystem(const char *cmdstring);

#ifdef __cplusplus
}
#endif

#endif // __MYSYSTEM_H__

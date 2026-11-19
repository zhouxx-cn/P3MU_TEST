/*!
*\file   CUser.h
*\brief  用户类CUser定义
*\author yuk@higheasyrd.com
*\date	  2009/04/10
*\version 0.1
* 功能:支持用户的信息的获取和修改
* 说明:主要供CUserManager, CUserDao和MPU使用
*/

#ifndef __CUSER_H_
#define __CUSER_H_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include "PUB/global_macro.h"
#include "CHANNEL_ACTION.h"
#include "LOCAL_ACTION.h"
#include "REMOTE_ACTION.h"
#include "RNL/idvr_rnl.h"

/**************************************************************************
 *                     宏定义                                             *
 **************************************************************************/

//!\brief 计时开始宏
//!\param 循环执行的次数
#if 1
#define BEGIN_TIME(count)
#define END_TIME
#else
#define BEGIN_TIME(count)  \
{\
   struct timeval tv;\
   struct timeval tv_end;\
   int TEST_COUNT = 0;\
   gettimeofday(&tv, NULL);\
   for (TEST_COUNT = 0; TEST_COUNT < count; TEST_COUNT++)\
   {

//!\brief 计时结束宏
#define END_TIME \
   }\
   gettimeofday(&tv_end, NULL);\
   printf("[%s %d]:usec=%ld\n", \
   __FILE__, __LINE__,\
   tv_end.tv_sec*1000000 + tv_end.tv_usec - tv.tv_sec*1000000 - tv.tv_usec);\
}
#endif
/**************************************************************************
 *                     CUser 类定义                                           *
 **************************************************************************/

//!\brief 用户基本信息
//!用户名、密码、是否禁用、email、mac、用户权限，提供权限设置
// 和获取接口
class CUser 
{
	friend class CUserManager;
	friend class CUserDao;
	friend class CUserDaoEx;
public:
	//##ModelId=499CCB480342
	~CUser();
	
	//!\brief 新创建一个用户
	//!\param strUsername 用户名
	//!\param strPassword 密码
	//!\return NULL表示创建失败 其他返回CUser *指针
	//##ModelId=49BDB96C02BF
	static CUser * GetInstance(const char * strUsername, char * strPassword);

	//!\brief 获取用户名
	//!\param strUsername 用户名缓冲区
	//!\param nLength 缓冲区长度
	//!\return 0获取成功 -1获取失败
	//##ModelId=499BC0270074
	int GetUserName(char *strUsername, int nLength) const;

	//!\brief 获取密码
	//!\param strPassword 密码字符串缓冲区
	//!\param nLength 缓冲区长度
	//!\return 0获取成功 -1获取失败
	//##ModelId=499BC0510088
	int GetPassword(char *strPassword, int nLength) const;

	//!\brief 设置密码
	//!\param strPassword 新密码字符串
	//!\return 0设置成功 -1设置失败
	//##ModelId=499BC06B00A4
	int SetPassword(const char *strPassword);

	//!\brief 获取用户是否被启用
	//!\return 0表示已禁用 1表示已启用
	//##ModelId=499BC18302FF
	int GetEnable() const;

	//!\brief 设置用户是否被启用
	//!\param bEnable 0表示不启用,1表示启用
	//##ModelId=499BC1A000D0
	int SetEnable(int bEnable);

	//!\brief 设置用户Email
	//!\param strEmail 邮箱名
	//!\return 0表示设置成功,-1表示设置失败
	//##ModelId=499CCA5801B7
	int SetEmailAddress(char *strEmail);

	//!\brief 获取用户Email
	//!\param strEmail 邮箱名缓冲区
	//!\param nLength 缓冲区长度
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=499CCA6B01DC
	int GetEmailAddress(char *strEmail, int nLength) const;

	//!\brief 设置绑定的物理地址
	//!param strMac 物理地址字符串
	//!return 0表示设置成功 -1表示设置失败
	//##ModelId=499CCA8300BE
	int SetBindMacAddress(unsigned char byteMac[MAC_ADDRESS_BYTES]);

	//!\brief 获取绑定的物理地址
	//!\param strMac 物理地址缓冲区
	//!\param nLength 缓冲区长度
	//!\return 0表示获取成功 -1表示获取失败
	//##ModelId=499CCA9401F9
	int GetBindMacAddress(unsigned char byteMac[MAC_ADDRESS_BYTES]) const;

	//!\brief 获取通道相关操作权限
	//!\param action 操作代码，参见 CHANNEL_ACTION
	//!\param nChannel 通道号
	//!\param bLocal 本地/远程
	//!\return 0表示有权限 1表示无权限
	//##ModelId=49ACF31B005D
	int GetChannelPrivilege(CHANNEL_ACTION action, int nChannel, int bRemote) const;

	//!\brief 设置通道相关操作权限
	//!\param bEnable 0表示有权限 -1表示无权限
	//!\param action 操作代码，参见 CHANNEL_ACTION
	//!\param nChannel 通道号
	//!\param bLocal 本地/远程
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=49ACF31B0196
	int SetChannelPrivilege(int bEnable, CHANNEL_ACTION action, int nChannel, int bRemote);

	//!\brief 获取本地相关操作权限
	//!\param action 操作代码 参见 LOCAL_ACTION
	//!\return 0表示无权限 1表示有权限
	//##ModelId=49ACF31B032C
	int GetLocalPrivilege(LOCAL_ACTION action) const;

	//!\brief 设置本地相关操作权限
    //!\param bEnable 0表示有权限 -1表示无权限
	//!\param action 操作代码 参见 LOCAL_ACTION
	//!\return 0设置成功 -1表示设置失败
	//##ModelId=49ACF31B03B9
	int SetLocalPrivilege(int bEnable, LOCAL_ACTION action);

	//!\brief 获取远程相关操作权限
	//!\param action 操作代码 参见 REMOTE_ACTION
	//!\return 0表示无权限 1表示有权限
	//##ModelId=49ACF31C00AB
	int GetRemotePrivilege(REMOTE_ACTION action) const;

	//!\brief 设置远程相关操作权限
    //!\param bEnable 0表示有权限 -1表示无权限
	//!\param action 操作代码 参见 REMOTE_ACTION
	//!\return 0设置成功 -1表示设置失败
	//##ModelId=49ACF31C0138
	int SetRemotePrivilege(int bEnable, REMOTE_ACTION action);

	//!\brief 获取远程相关操作读权限
	//!\param action 操作代码 参见 REMOTE_ACTION_V1
	//!\return 0表示无权限 1表示有权限
	//##ModelId=49ACF31C00AB
    int GetRemoteReadPrivilege(REMOTE_ACTION_V1 action) const;

	//!\brief 获取远程相关操作写权限
	//!\param action 操作代码 参见 REMOTE_ACTION_V1
	//!\return 0表示无权限 1表示有权限
	//##ModelId=49ACF31C00AB
    int GetRemoteWritePrivilege(REMOTE_ACTION_V1 action) const;

	//!\brief 设置远程相关操作读权限
    //!\param bEnable 1表示启用权限 ；0表示取消权限
	//!\param action 操作代码 参见 REMOTE_ACTION_V1
	//!\return 0设置成功 -1表示设置失败
	//##ModelId=49ACF31C0138
    int SetRemoteReadPrivilege(int bEnable, REMOTE_ACTION_V1 action);

	//!\brief 设置远程相关操作写权限
    //!\param bEnable 1表示启用权限 ；0表示取消权限
	//!\param action 操作代码 参见 REMOTE_ACTION_V1
	//!\return 0设置成功 -1表示设置失败
	//##ModelId=49ACF31C0138
    int SetRemoteWritePrivilege(int bEnable, REMOTE_ACTION_V1 action);

	void Print();

private:
	//##ModelId=499CCB4301E7
	CUser();

	//!\brief 重载赋值操作符
	CUser &operator=(const CUser &objUser);

private:
	//!\brief 用户名缓冲区长度
	const static int USERNAME_BUF_LEN = MAX_USERNAME_LEN + 4;

	//!\brief 密码缓冲区长度
	const static int PASSWORD_BUF_LEN = MAX_PASSWORD_LEN + 4;

	//!\brief EMAIL缓冲区长度
	const static int EMAIL_BUF_LEN = MAX_EMAIL_LEN + 4;

	//!\brief 用户Id
	int m_nUserId;
	
	//!\brief 用户名
	//##ModelId=49ACF613008C
	char m_szUsername[USERNAME_BUF_LEN];

	//!\brief 密码
	//##ModelId=49ACF61300CB
	char m_szPassword[PASSWORD_BUF_LEN];

	//!\brief email地址
	//##ModelId=49ACF61300FA
	char m_szEmail[EMAIL_BUF_LEN];

	//!\brief 绑定的mac
	//##ModelId=49ACF6130177
	unsigned char m_szMacAddress[MAC_ADDRESS_BYTES];

	//!\brief 是否禁用
	//##ModelId=49ACF61301B5
	int m_bEnable;

	//!\brief 预览权限,[0]本地,[1]远程
	//##ModelId=49ACF61301F4
	unsigned long m_dwPreview[2];

	//!\brief 回放权限,[0]本地,[1]远程
	//##ModelId=49ACF6130232
	unsigned long m_dwPlayback[2];

	//!\brief 云台控制权限,[0]本地,[1]远程
	//##ModelId=49ACF6130271
	unsigned long m_dwPtzCtrl[2];

	//!\brief 手动录像权限,[0]本地,[1]远程
	//##ModelId=49ACF61302AF
	unsigned long m_dwManualRecord[2];

	//!\brief 本地权限
	//##ModelId=49ACF61302EE
	unsigned long m_dwLocal;

	//!\brief 远程权限
	//##ModelId=49ACF613032C
	unsigned long m_dwRemote;

	//!\brief 远程权限V1:读权限
	//##ModelId=49ACF613032C
	unsigned long m_dwRemoteR;

	//!\brief 远程权限V1：写权限
	//##ModelId=49ACF613032C
	unsigned long m_dwRemoteW;

	CUser *pNext;
};

#endif /* _INC_CUSER_499BBFC2010F_INCLUDED */

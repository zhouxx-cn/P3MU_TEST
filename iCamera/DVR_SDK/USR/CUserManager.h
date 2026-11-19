// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUSERMANAGER_499CB8180136_INCLUDED
#define _INC_CUSERMANAGER_499CB8180136_INCLUDED
#include "HANDLE.h"
#include "CUser.h"
#include "PUB/tagAlertInfo.h"
#include "PUB/tagActionInfo.h"
#include "MPU_PUB_API.h"

typedef int (*ILLEGEACCESS_CALLBACK)(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo);

//!\brief 用户管理接口类
//##ModelId=499CB8180136
class CUserManager 
{
public:
	//!\brief 
	//获取CUserManager实例,若CUserManager实例第一次创建则从用
	//户信息配置文件加载用户信息，初始化用户列表
	//!\return NULL表示获取失败；其他为CUserManager实例指针
	//##ModelId=499CCB5500E8
	static CUserManager * USR_GetInstance();

	//\brief 释放CUserManager资源，清除用户列表
	//\return 无
	//##ModelId=499CCB610230
	static void USR_DestroyInstance();

	//!\brief 获取用户模块模式
	//!\return   0为普通工作模式，1为高级工作模式，失败为-1
	int USR_GetWorkMode();

	//!\brief       设置用户管理的工作模式
	//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
	//!\return  成功为0，失败为-1	
	int USR_SetWorkMode(int nWorkMode);
	
	//!\brief 重置管理员密码为888888
	//!\param 无
	//!\return 成功为0 -1为失败
	int USR_ResetAdminPassword();

    int CheckPasswd(char szPasswd[]);
    
    unsigned char X1ToChar(char c);

    unsigned char X2ToChar(char c0, char c1);
    
    int CheckUser_Passwd(char pszUserName[], char pszPasswd[],char szUserName[],char szPasswd[]);

	//!\brief 
	//用户登陆校验，可在网络sdk的用户登录回调函数中调用，检验
	//用户是否能合法登陆
	//!\param strUsername用户名
	//!\param strPassword密码
	//!\param byteMac MAC地址,本地用户登陆为NULL
	//!\return NULL表示失败 其他表示用户句柄
	//##ModelId=49AFB35401E4
	HANDLE USR_Login(char * strUsername, char * strPassword, unsigned char byteMac[MAC_ADDRESS_BYTES] = NULL);

	//!\brief 添加新的用户
	//!\param pUser  指向要添加的新用户的指针
	//!\return INVALID_HANDLE 表示添加失败 其他表示添加的用户的句柄
	//##ModelId=499CCB850354
	HANDLE USR_AddUser(CUser *pUser);

	//!\brief 从用户列表删除用户
	//!\param hUser 用户句柄
	//!\return 0表示删除成功 -1表示删除失败
	//##ModelId=499CCB9A0105
	int USR_RemoveUser(HANDLE hUser);

	//!\brief 保存所有用户信息
	//!\param pUser 用户句柄
	//!\return 0保存成功 其他保存失败错误码
	int USR_SaveUser(HANDLE hUser);

	//!\brief 获取用户信息
	//!\param hUser 用户句柄
	//!\return CUser*   NULL表示获取失败，其他为CUser指针
	//##ModelId=499CCF0803D9
	CUser* USR_GetUser(HANDLE hUser);

	//!\brief		获取密码
	//!\param	pszUserName         用户名
	//!\param	pszPassword         密码字符串缓冲区
	//!\param	nLength             缓冲区长度
	//!\return	0获取成功 -1获取失败
	int USR_GetPasswordByName(char *pszUserName, char *pszPassword, int nLength);

	//!\brief 获取用户总数
	//!\return -1为获取失败，其他为用户总数
	//##ModelId=49BDA66F0128
	int USR_GetUserCount();

	//!\brief 获取用户句柄数组
	//!\param phUserList 用户句柄数组头指针
	//!\param nListLength 用户句柄数组长度
	//!\return -1表示获取失败,其他表示获取的用户句柄数
	//##ModelId=49BDA8720203
	int USR_GetUserList(HANDLE * phUserList, int nListLength);

	//!\brief 开启/禁止非法访问异常联动动作
	//!\param int Action    !      ACTION_SCREEN 屏幕显示
	//	!      ACTION_BEEP 声音提示
	//	!      ACTION_ALARMOUT 报警输出
	//	!      ACTION_SENDTOSERVER 上传报警服务器
	//	!      ACTION_EMAIL 联动Email
	//	!      ACTION_SMS 联动短信
	//!\ nArg TRUE为开启，FALSE为禁止, 联动报警输出时按位表示联动的报警输出通道
	//!\return 成功为0，失败为错误码
	int USR_SetAlarmAction(tagActionType Action, int nArg);

	//!\brief 获取非法访问联动参数
	//!\param tagActionType 联动动作类型
	//!\return TRUE表示启用该动作, FALSE表示禁用该动作, ACTION为联动报警输出时返回值按位表示联动的报警输出通道
	int USR_GetAlarmAction(tagActionType Action);

	//!\brief 设置用户非法访问的回调
	//!\param AlarmType 报警类型
	//!\param CallBack 设置的回调函数, NULL表示不回调
	//!\return 0表示设置成功
	int USR_SetAlarmCallBack(int AlarmType, int (*CallBack)(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo));

	//!\brief 驱动用户模块根据配置调用非法访问的回调
	//!\param bIllege TRUE表示检测到非法访问 FALSE表示用户成功登陆
	//!\return 0表示成功,其他表示错误码
	int USR_DealIllegeAccess(BOOL bIllege);

	int USR_GetChanNum();
	//!\brief 获取超级密码
	//!\return	 0获取成功，获取失败为-1
	int USR_GetSuperPassword(char *pszSuperPassword);

private:
	//!\brief 加载所有用户
	//!\return 0表示加载成功 -1表示加载失败
	int LoadUser();

	//!\brief 获取指定的用户实例
	CUser * GetUser(HANDLE hUser);
	
	//!\brief 恢复用户的默认权限和MAC地址
	int ResumeDefault();

	//##ModelId=499CCB2A02F9
	CUserManager();

	//##ModelId=499CCB3A01A7
	~CUserManager();

private:
	//##ModelId=49BE185C02C2
	static CUserManager *m_pInstance;

	//##ModelId=49A6443D00AB
	CUser m_tUserList[IDVR_MAX_USER];

	int m_nUserCount;

	int m_nAlarmType;

	ILLEGEACCESS_CALLBACK m_fnIllegeAccess;

	//!\brief 超级密码
	//##ModelId=49ACF61300CB
	char m_szSuperPassword[MAX_PASSWORD_LEN+4];

	//!\brief DES加密的超级密码
    char m_szDesSuperPwd[16 + 1];

	int m_nActionParam[10];

	int m_nWorkMode;

	int m_nChannelCount;

	pthread_mutex_t m_mtxUser;
};

#endif /* _INC_CUSERMANAGER_499CB8180136_INCLUDED */

/*!
*\file   CUserDaoEx.cpp
*\brief  用户文本数据库访问控制类函数定义
*\author yuk@higheasyrd.com
*\date	  2009/04/14
*\version 0.1
*/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//#include <iostream>
#include <fstream>
#include "idvr_errno.h"
#include "CUserDaoEx.h"
#include "USRTrace.h"
// #include "sys_banned.h"
#include "global_gather.h"
/******************************************************************************
*				静态成员初始化												  *
******************************************************************************/
//初始化实例为NULL
CUserDaoEx *CUserDaoEx::m_pInstance= NULL;

char *CUserDaoEx::m_szBindFile = NULL;

/******************************************************************************
*				函数定义												  *
******************************************************************************/

CUserDaoEx::CUserDaoEx(): m_nUserCount(0), m_nUserIndex(0)
{
}

CUserDaoEx::~CUserDaoEx()
{
	if (m_szBindFile != NULL)
	{
		free(m_szBindFile);
		m_szBindFile = NULL;
	}
}

//!\brief 获取类的唯一实例
//!\return 成功则返回实例的指针,否则返回NULL
CUserDaoEx *CUserDaoEx::GetInstance()
{
	//未创建唯一实例,则新建
	if (NULL == m_pInstance)
	{
		m_pInstance = new CUserDaoEx();
	}
	
	//否则直接返回
	return m_pInstance;
}

//!\brief 销毁类的实例
//!\param pInstance 指向实例的指针
//!\return 无
void CUserDaoEx::DestroyInstance(CUserDaoEx *pInstance)
{
	//检查参数
	if (pInstance != NULL)
	{
		delete pInstance;
		pInstance = NULL;
	}
	//参数有误
	else
	{
		USR_ERROR("[%s]ERR: para error, pInstance=%p", __FUNCTION__, pInstance);
	}
}

//!\brief 初始化用户数据库 
//!\param szUsrDbFileName 指定数据库的文件名，
//! 如果没有该文件,系统会自动创建文件
int CUserDaoEx::BindDb(const char *szUsrDbFileName, int reset)
{   
	//数据库文件名不能为空
	if (NULL == szUsrDbFileName)
	{
		USR_ERROR("[%s]ERR:para err, szUsrDbFileName=%s", __FUNCTION__, szUsrDbFileName);
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	//判断是否已经绑定到一个用户数据库
	if (m_szBindFile != NULL)
	{
		USR_ERROR("[%s]ERR:para err, already bind a usr file", __FUNCTION__);
		return -1;
	}

	m_szBindFile = strdup(szUsrDbFileName);

	//打开用户数据库,如果没有该文件则创建该文件
    FILE *fp = NULL;
    fp = fopen(m_szBindFile, "ab");
    if (!fp)
    {
		USR_ERROR("[%s]ERR:open or create usr db %s failed %s", __FUNCTION__, m_szBindFile,szUsrDbFileName);
		return IDVR_USR_ERROR_DB_OPEN_FAILED;
    }
    fclose(fp);

	USR_DBG("User Data base Initialize Success\n");

	//读取所有用户信息
	if (reset)
	    ReadOldUser();
	else
	    ReadUsers();
	    
	return (int)0;
}

//!\brief 添加新的用户
//!\param objUser 要添加的用户信息
//!\return 0表示添加成功 其他表示错误码
int CUserDaoEx::InsertUser(const CUser &objUser)
{
	//判断用户是否存在
	if (HaveUser(objUser.m_szUsername))
	{
		USR_DBG("User Already Exsits\n");
		return IDVR_USR_ERROR_USER_EXSIT_ALREADY;
	}

	m_tUsers[m_nUserCount] = objUser;
	m_tUsers[m_nUserCount].m_nUserId = m_nUserIndex + 1;
	m_nUserCount++;
	m_nUserIndex++;

	//将用户信息写入文件
	WriteUsers();
	USR_DBG("Insert User Success\n");

	return (int)0;
}

//!\brief 获取所有用户信息
//!\param pUserList 用户列表缓冲区
//!\param nCount 缓冲区
//!\return 获得用户的个数
int CUserDaoEx::GetUsers(CUser *pUserList, int nCount)
{
	if ((pUserList == 0) || (nCount == 0))
	{
		USR_ERROR("[%s]ERR:para error, pUserList=%p, nCount=%d", __FUNCTION__, pUserList, nCount);
		return -1;
	}
	
	int i = 0;
    
	while (i < m_nUserCount)
	{
		if (i < nCount)
		{
			pUserList[i] = m_tUsers[i];
		}
		else
		{
			i++;
			break;
		}
		i++;
	}

	USR_DBG("Get %d User\n", i);
	return (int)i;
}


//!\brief 保存指定用户信息
//!\param objUser 要保存的用户信息
//!\return 0表示保存成功 其他表示错误码
int CUserDaoEx::SaveUser(const CUser &objUser)
{ 
	if ((-1 == objUser.m_nUserId) || !HaveUser(objUser.m_szUsername))
	{
		USR_ERROR("[%s]ERR:User %s Not Exsit", __FUNCTION__, objUser.m_szUsername);
		return IDVR_USR_ERROR_USER_NOEXSIT;
	}

	//插入列表
	int i = 0;
	for (i = 0; i < m_nUserCount; i++)
	{
		if (m_tUsers[i].m_nUserId == objUser.m_nUserId)
		{
			m_tUsers[i] = objUser;
			break;
		}
	}

	//保存用户信息
	WriteUsers();
	for (i = 0; i < m_nUserCount; i++)
	{
		USR_DBG("UserId=%d, Username=%s\n", m_tUsers[i].m_nUserId, m_tUsers[i].m_szUsername);
	}
	return (int)0;
}

//!\brief 查询某个用户是否存在
//!\param szUsername 用户名
//!\return true表示有该用户 false 表示没有该用户
bool CUserDaoEx::HaveUser(const char *szUserName)
{
	if (NULL == szUserName)
	{
		USR_ERROR("[%s]ERR:para error, szUserName=%s", __FUNCTION__, szUserName);
		return false;
	}

	int i = 0;
	
	//循环比较用户名
	for (i = 0; i < m_nUserCount; i++)
	{
		if (0 == strcmp(szUserName, m_tUsers[i].m_szUsername))
		{
			return true;
		}
	}

	USR_DBG("No User whose Username=%s\n", szUserName);
	return (bool)false;
}

//!\brief 删除指定用户
//!\param nUserId 要删除的用户的ID
//!\return true表示删除成功 false表示删除错误
int CUserDaoEx::DeleteUser(int nUserId)
{
	USR_DBG("nUserId = %d\n", nUserId);
	if (-1 == nUserId)
	{
		USR_ERROR("[%s]ERR:para error, nUserId=%d", __FUNCTION__, nUserId);
		return IDVR_ERROR_PARAMETER;
	}

	for (int i = 0; i < m_nUserCount; i++)
	{
		if (m_tUsers[i].m_nUserId == nUserId)
		{
			// 使用拷贝赋值运算符逐个移动对象
            for (int j = i; j < m_nUserCount - 1; j++)
            {
                m_tUsers[j] = m_tUsers[j + 1]; // 使用CUser的拷贝赋值运算符
            }

			m_nUserCount--;
			WriteUsers();
			return 0;
		}
	}
	return (int)IDVR_USR_ERROR_USER_NOEXSIT;
}

//加解密
void getXOR(unsigned char *string_in, unsigned char *string_out, int len)
{    
    //memcpy(string_out, string_in, len);
    //return ;
    
    int i = 0;
    int key = 0x7;
    
    for (i=0;i<len;i++)
    {
        string_out[i] = string_in[i] ^ key;//acEncryption[i];
    }
}

void str_trim_crlf(char *str) //去除\r\n
{
	char *p = &str[strlen(str)-1];
	while (*p == '\r' || *p == '\n')
		*p-- = '\0';
}

int CUserDaoEx::ReadOldUser()
{
    FILE *fp = NULL;
    char acUserName[64] = {0};
    char acUserPwd[64] = {0};
    char buf[128] = {0}; 
    char *ptr = NULL;
    int i = 0;
    
    fp = fopen(UserFile, "r");
    if (!fp)
    {
        USR_ERROR("[%s]ERR: open file error, file name=%s", __FUNCTION__, UserFile);
		return -1;
    }

    while (!feof(fp))
    {
        //一次读取一个用户
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), fp);
        
        ptr = strchr(buf, ':');
        if (ptr)
        {   
            memset(acUserName, 0, sizeof(acUserName));
            snprintf(acUserName, ptr-buf+1, "%s", buf);

            memset(acUserPwd, 0, sizeof(acUserPwd));
            snprintf(acUserPwd, sizeof(acUserPwd), "%s", ptr+1);         
        }
        else
            continue;

        str_trim_crlf(acUserPwd);

        m_tUsers[i].m_nUserId = i + 1;
        USR_DBG("userid=%d\n", m_tUsers[i].m_nUserId);

        memcpy(&(m_tUsers[i].m_szUsername), acUserName, CUser::USERNAME_BUF_LEN);
        USR_DBG("usrname=%s\n", m_tUsers[i].m_szUsername);

        memcpy(&(m_tUsers[i].m_szPassword), acUserPwd, CUser::PASSWORD_BUF_LEN);
        USR_DBG("password=%s\n", m_tUsers[i].m_szPassword);

        //fprintf(stderr, "[%s][%d] ********userid=[%d] username=[%s] userpwd=[%s]*****\r\n",__FUNCTION__,__LINE__, m_tUsers[i].m_nUserId, acUserName, acUserPwd);

        memset(&(m_tUsers[i].m_szEmail), 0 , CUser::EMAIL_BUF_LEN);

        memset(&(m_tUsers[i].m_szMacAddress), 0 , MAC_ADDRESS_BYTES);

        m_tUsers[i].m_bEnable = 1;

        m_tUsers[i].m_dwPreview[0] = -1;
        m_tUsers[i].m_dwPreview[1] = -1;

        m_tUsers[i].m_dwPlayback[0] = -1;
        m_tUsers[i].m_dwPlayback[1] = -1;

        m_tUsers[i].m_dwPtzCtrl[0] = -1;
        m_tUsers[i].m_dwPtzCtrl[1] = -1;

        m_tUsers[i].m_dwManualRecord[0] = -1;
        m_tUsers[i].m_dwManualRecord[1] = -1;

        m_tUsers[i].m_dwLocal = -1;
        
        m_tUsers[i].m_dwRemote = -1;

        i++;
        
        m_nUserCount++;
	    m_nUserIndex++;
	  
        //fprintf(stderr, "[%s][%d] ********m_nUserCount=[%d] m_nUserIndex=[%d]*****\r\n",__FUNCTION__,__LINE__, m_nUserCount, m_nUserIndex);
        WriteUsers();
    }


	//如果有效用户数据个数为0
	//则清空冗余的无效数据
	if (0 == m_nUserCount)
    {
        USR_DBG("user file err\n");
        m_nUserIndex = 0;   
        m_nUserCount = 0;
        fwrite((const char *)&m_nUserCount, sizeof(m_nUserCount), 1, fp);
        fwrite((const char *)&m_nUserIndex, sizeof(m_nUserIndex), 1, fp);
        fclose(fp);
        return 0;
    }
	
    fclose(fp);
    return i;
}

//!\brief 将所有的用户读到数组中
//!\return 0表示读取成功 -1表示读取失败
int CUserDaoEx::ReadUsers()
{
	std::fstream fs;
	fs.open(m_szBindFile, std::ios_base::in|std::ios_base::out);
    char acNewBuf[64] = {0};
    
	if (!fs)
	{
		USR_ERROR("[%s]ERR: open file error, file name=%s", __FUNCTION__, m_szBindFile);
		return IDVR_USR_ERROR_DB_OPEN_FAILED;
	}
	fs.read((char *)&m_nUserCount, sizeof(m_nUserCount));

	if (fs.gcount() == 0)
	{
		USR_DBG("initialize file\n");
		m_nUserIndex = 0;	
		m_nUserCount = 0;
		fs.write((const char *)&m_nUserCount, sizeof(m_nUserCount));
		fs.write((const char *)&m_nUserIndex, sizeof(m_nUserIndex));
		fs.close();
		return 0;
	}

	fs.read((char *)&m_nUserIndex, sizeof(m_nUserIndex));
	int i = 0;
	char buf[UserSize()];
	char *p = buf;
	while (1)
	{
		//一次读取一个用户
		fs.read((char *)buf, UserSize());
		p = buf;

		//文件读取完毕
		if (fs.eof()|| (fs.gcount() < UserSize()))
		{
			m_nUserCount = i;
			USR_DBG("read %d users\n", i);
			break;
		}
		memcpy(&(m_tUsers[i].m_nUserId), p, sizeof(int));
		p += sizeof(int);
		//printf("userid=%d\n", m_tUsers[i].m_nUserId);

        memset(acNewBuf, 0, sizeof(acNewBuf));
	    getXOR((unsigned char*)p, (unsigned char*)acNewBuf, strlen(p));
		memcpy(&(m_tUsers[i].m_szUsername), acNewBuf, CUser::USERNAME_BUF_LEN);
		p += CUser::USERNAME_BUF_LEN;
		//printf("usrname=%s\n", m_tUsers[i].m_szUsername);

        memset(acNewBuf, 0, sizeof(acNewBuf));
	    getXOR((unsigned char*)p, (unsigned char*)acNewBuf, strlen(p));
		memcpy(&(m_tUsers[i].m_szPassword), acNewBuf, CUser::PASSWORD_BUF_LEN);
		p += CUser::PASSWORD_BUF_LEN;
		//printf("password=%s\n", m_tUsers[i].m_szPassword);

		memcpy(&(m_tUsers[i].m_szEmail), p, CUser::EMAIL_BUF_LEN);
		p += CUser::EMAIL_BUF_LEN;

		memcpy(&(m_tUsers[i].m_szMacAddress), p, MAC_ADDRESS_BYTES);
		p += MAC_ADDRESS_BYTES;

		memcpy(&(m_tUsers[i].m_bEnable), p, sizeof(int));
		p += sizeof(int);

		memcpy(&(m_tUsers[i].m_dwPreview), p, sizeof(unsigned long) * 2);
		p += sizeof(unsigned long) * 2;

		memcpy(&(m_tUsers[i].m_dwPlayback), p, sizeof(unsigned long) * 2);
		p += sizeof(unsigned long) * 2;

		memcpy(&(m_tUsers[i].m_dwPtzCtrl), p, sizeof(unsigned long) * 2);
		p += sizeof(unsigned long) * 2;

		memcpy(&(m_tUsers[i].m_dwManualRecord), p, sizeof(unsigned long) * 2);
		p += sizeof(unsigned long) * 2;

		memcpy(&(m_tUsers[i].m_dwLocal), p, sizeof(unsigned long));
		p += sizeof(unsigned long);

		memcpy(&(m_tUsers[i].m_dwRemote), p, sizeof(unsigned long));
		p += sizeof(unsigned long);
		i++;
	}
	fs.close();
	return i;
}

//!\brief 将所有的用户写入到文件中
//!\return 0表示写入成功 -1表示写入失败
int CUserDaoEx::WriteUsers()
{
	std::fstream fs;
	fs.open(m_szBindFile, std::ios_base::out | std::ios_base::binary);
	if (!fs)
	{
		USR_ERROR("[%s]ERR:open file failed", __FUNCTION__);
		return -1;
	}
	//写入用户数和用户索引
	fs.write((const char *)&m_nUserCount, sizeof(m_nUserCount));
	fs.write((const char *)&m_nUserIndex, sizeof(m_nUserIndex));
	int i = 0;

	char acNewBuf[64] = {0};
	//循环写所有用户
	for (i = 0; i < m_nUserCount; i++)
	{
		fs.write((char *)&(m_tUsers[i].m_nUserId), sizeof(int));
		//fs.write((char *)&(m_tUsers[i].m_szUsername), CUser::USERNAME_BUF_LEN);
		//fs.write((char *)&(m_tUsers[i].m_szPassword), CUser::PASSWORD_BUF_LEN);

		memset(acNewBuf, 0, sizeof(acNewBuf));
	    getXOR((unsigned char*)m_tUsers[i].m_szUsername, (unsigned char*)acNewBuf, strlen(m_tUsers[i].m_szUsername));
		fs.write((char *)acNewBuf, CUser::USERNAME_BUF_LEN);
		
		memset(acNewBuf, 0, sizeof(acNewBuf));
	    getXOR((unsigned char*)m_tUsers[i].m_szPassword, (unsigned char*)acNewBuf, strlen(m_tUsers[i].m_szPassword));
		fs.write((char *)acNewBuf, CUser::PASSWORD_BUF_LEN);

		fs.write((char *)&(m_tUsers[i].m_szEmail), CUser::EMAIL_BUF_LEN);
		fs.write((char *)&(m_tUsers[i].m_szMacAddress), MAC_ADDRESS_BYTES);
		fs.write((char *)&(m_tUsers[i].m_bEnable), sizeof(int));
		fs.write((char *)&(m_tUsers[i].m_dwPreview), sizeof(unsigned long) * 2);
		fs.write((char *)&(m_tUsers[i].m_dwPlayback), sizeof(unsigned long) * 2);
		fs.write((char *)&(m_tUsers[i].m_dwPtzCtrl), sizeof(unsigned long) * 2);
		fs.write((char *)&(m_tUsers[i].m_dwManualRecord), sizeof(unsigned long) * 2);
		fs.write((char *)&(m_tUsers[i].m_dwLocal), sizeof(unsigned long));
		fs.write((char *)&(m_tUsers[i].m_dwRemote), sizeof(unsigned long));
		USR_DBG("write %d users\n", i + 1);
	}
	fs.close();

	return 0;
}

//!\brief 返回一个用户信息在文件中所占的字节数
//!\return 用户字节数 目前为346字节
int inline CUserDaoEx::UserSize()
{
	return sizeof(int) + CUser::USERNAME_BUF_LEN + CUser::PASSWORD_BUF_LEN
		+ CUser::EMAIL_BUF_LEN + MAC_ADDRESS_BYTES + sizeof(int) + 10 * sizeof(unsigned long);
}



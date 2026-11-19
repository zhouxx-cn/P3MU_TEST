
/*!
*\file   CConfig.h
*\brief  配置管理类
*\author yuk@higheasyrd.com
*\date	  2009/03/23
*\version 0.1
* 功能:支持配置文件的加载、恢复、备份，增加、删除、修改、获取配置
*/

#ifndef _INC_CCONFIG_499BC98601C8_INCLUDED
#define _INC_CCONFIG_499BC98601C8_INCLUDED

#include <stdio.h>
#include <pthread.h>
#include "HANDLE.h"
#include "idvr_errno.h"

//!\brief   参数管理模块    配置文件读写类
class CConfig
{
  public:
	//!\brief   获取dvr配置实例
	//!\return  dvr配置的实例指针   
	static CConfig *GetInstance();

	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   nDefault        缺省值
	//!\return  返回整形参数值
	int GetValue(const char *pszSection, const char *pszKey, int nDefault);

	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   nDefault        缺省值
	//!\return  返回长整形参数值
	long long GetValueLong(const char *pszSection, const char *pszKey, long long nDefault);

	//!\brief   设置配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   nValue          整形参数值
	//!\return  0表示成功，其它为错误码
	int SetValue(const char *pszSection, const char *pszKey, int nValue);
	
	//!\brief   设置配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   nValue          长整形参数值
	//!\return  0表示成功，其它为错误码
	int SetValueLong(const char *pszSection, const char *pszKey, long long nValue);

	//!\brief   释放配置信息内存空间
	//!\return  0表示成功，其它为错误码
	int UnloadFile();

	int LockBakFile();

    /* 禁止保存参数的操作，一般用在完全恢复默认的时候，使用该操作之后，系统将不能将内存参数写入配置文件 */
    int ForbiddenSaveFile();

    /* 解除禁止保存参数的操作，暂时未用到，使用该操作之后，解除ForbiddenSaveFile的锁定 */
    int UnForbiddenSaveFile();

	//!\brief   恢复默认配置,本函数只用于配置文件已正常加载情况下的恢复参数
	//!恢复默认前会备份先前加载的配置文件,备份文件名为文件名+-bak
	//!\param   pszDefFileName      默认配置文件名称（含路径）
	//!\param   pszFlagFile 标记不恢复的项的文件
	//!\return  0表示成功，其它为错误码 
	int ResumeDefCfg(const char *pszDefFileName, const char *pszFlagFile = NULL);

	//!\brief 升级配置文件
	//! 升级配置文件前会备份先前加载的配置文件，备份文件名为文件名+-bak
	//!\param pszNewCfgFileName 升级的配置文件名称
	//!\param pszFlagFile 标记更新的项的文件,为NULL则将新配置文件中的值添加到原配置文件中
	//!\return 0表示成功,其他为错误码
	int UpdateCfg(const char *pszNewCfgFileName, const char *pszFlagFile = NULL);

	//!\brief   获取新实例
	//!\return  新的实例指针    
	static CConfig *NewInstance();

	//!\brief   释放实例
	static void ReleaseInstance(CConfig * pInstance);

	//!\brief   读取配置文件
	//!         自动识别文件类型
	//!\param   pszFileName     配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int LoadFile(const char *pszFileName);

	int LoadFileRO(const char *pszFileName);

	//!\brief   保存配置信息到原文件
	//!\return  0表示成功，其它为错误码
	int SaveFile();

	int SaveFileReal(int bNotNeedSaveBak = 0);

	int CheckIfNeedSaveFile();
	
	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   pszDefault      缺省值
	//!\param   pValue          返回字符串型参数值
	//!\param   nSize           buf空间大小
	//!\return  
	//本接口字符串型参数值到pValue指向的buf，并返回pValue
	char *GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

	//!\brief   设置配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   pValue          字符串型参数值
	//!\return  0表示成功，其它为错误码
	int SetValue(const char *pszSection, const char *pszKey, const char *pValue);

	//!\brief   另存配置信息到其它文件
	//!\param   pszFileName     配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int SaveAsFile(const char *pszFileName);

	//!\brief   配置参数替换到别的配置文件中
	//!\param   pach    修改配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int ReplaceFileCfg(const char *pach);

    //!\brief   检测配置中是否有sec区段名
    //!\param   sec    区段名
    //!\return  true - 存在，false - 无
    bool CheckSectionExist(const char *sec);

    //!\brief   检测配置中是否有sec区段名对应的key的字段
    //!\param   sec    区段名
    //!\param   key    字段名
    //!\return  true - 存在，false - 无
    bool CheckKeyExist(const char *sec, const char *key);

public:
	  struct tagSection;
	int m_nNeedSaveFileFlag;
	int m_ifSaveSoon;
	int m_iCfgDataChanged;//true 标记数据已经改变   
	int m_bReadOnlyCfgFlag;

  protected:
	  CConfig();

	  CConfig(CConfig & cfg);

	 ~CConfig();

	//!\brief   读取ini文件
	//!         自动识别文件完整性
	//!\param   pszFileName     配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int LoadIniFile(const char *pszFileName);

	int LoadIniFileRO(const char *pszFileName);

	//!\brief   读取xml文件
	//!         自动识别文件完整性
	//!\param   pszFileName     配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int LoadXmlFile(const char *pszFileName);

	//!\brief   保存配置信息到Ini文件
	//!\param   pszFileName     配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int SaveIniFile(const char *pszFileName);

	//!\brief   保存配置信息到xml文件
	//!\param   pszFileName     配置文件名称（含路径）
	//!\return  0表示成功，其它为错误码
	int SaveXmlFile(const char *pszFileName);

  private:
	//!\brief 获取配置文件类型
	//!\param pszFileName 文件名
	//!\return 后缀为.xml的为xml文件返回1,其他为.ini文件返回0
	static int GetFileType(const char *pszFileName);

	static int GetFileTypeRO(const char *pszFileName);

	//!\brief 读取一个section
	//!\param fp 文件指针
	//!\param pSect section指针
	//!\return 0表示读取正确，其他表示错误码
	//!\sa ENCONFIG 错误码
	int ReadIniSection(FILE * fp, tagSection * pSect);

	//!\brief 从配置文件的一行中获取section名称
	//!\param pszSectionName section名称缓冲区指针
	//!\param szBuf 一行字符串
	//!\return 0表示获取成功,其他表示获取失败
	int GetIniSectionName(char **pszSectionName, char *szBuf);

	//!\brief 从配置文件的一行获取key名称和值
	//!\param szBuf 一行字符串
	//!\param pszKey key名字符串缓冲区指针
	//!\param pszValue key值字符串缓冲区指针
	//!\return 0表示获取成功 其他表示获取失败
	int GetIniKey(char *szBuf, char **pszKey, char **pszValue);

	//!\brief 释放加载过程中占用的内存空间
	int FreeMemory();

	//!\brief 查找一个section
	//!\param pszSection section名
	//!\return NULL表示查找失败 其他表示section指针
	tagSection *SearchSection(const char *pszSection);

	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   pszDefault      缺省值
	//!\param   pValue          返回字符串型参数值
	//!\param   nSize           buf空间大小
	//!\return  
	//本接口字符串型参数值到pValue指向的buf，并返回pValue
	char *InternalGetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

	//!\brief   设置配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   pValue          字符串型参数值
	//!\return  0表示成功，其它为错误码
	int InternalSetValue(const char *pszSection, const char *pszKey, const char *pValue);

	//!\brief 备份文件
	static int BackupFile(const char *pszFileName);

	//!\brief 添加新的section,内容为空
	tagSection *AddSection();

	tagSection *InsertSection();

	void Copy(CConfig & newCfg);

	static int IsFileExsit(const char *pszFileName);

	void Print();

	static int Lock();

	static int Unlock();

    static int LockSave();

    static int UnlockSave();

private:
	static CConfig *m_pEdvrCfg;
	static CConfig *m_pCustomRoCfg;
	static char *m_strCfgFile;
	HANDLE hCfg;
	char *m_strFile;
	tagSection *m_pSection;
	int m_nSectionCount;
	int m_nSectionAlloced;
	int m_nType;
	int m_nCurrentLine;
	int m_uninitStartFlag;	
	int m_bForbidSaveFileFlag;
	static pthread_mutex_t m_mtxConfig;
	static pthread_mutex_t m_mtxSaveFile;
	static pthread_t m_SaveFileThread;
	static unsigned int m_NewCount; //用来计数config类的实例个数，不含单例
};

#endif							/* __CONFIG_H_ */

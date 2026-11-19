// Copyright (C) 1991 - 1999 Rational Software Corporation

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <errno.h>
#include "usb.h"
#include "idvr_errno.h"
#include "CPtzManager.h"
#include "CPtzAdjuster.h"
#include "CPtzDev.h"
#include "CProtocol.h"
#include "PTZTrace.h"
//#include "hisdk.h"
#include "zwsdk.h"
#include "PUB/macro_pthread.h"
#include "PUB/boottime.h"
#include "CTransparentManager.h"
#include "CPtzEletronic.h"
#include "PAR/CConfig.h"
#include "Authority.h"

#include "mysystem.h"
#include "motoLib.h"
#include "common_pub.h"
#include "MPU_AVL_API.h"
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
#include "CZoomManager.h"
#endif

//!\brief 获取配置器
#define PTZRESET_GETCONFIG(key, value)								\
do																	\
{																	\
	CConfig *pConfig = CConfig::GetInstance();						\
	if (NULL == pConfig)											\
	{																\
		return IDVR_ERROR_CONFIGFILE;								\
	}																\
	value = pConfig->GetValue(PTZRESET_SECTION, key, 0);		\
} while(0)


typedef enum tagPTZState
{
	PTZ_STATE_STOP = 0x0,
	PTZ_STATE_NORMAL,
	PTZ_STATE_PRESET,
	PTZ_STATE_CRUISE,
	PTZ_STATE_TRACK,
	PTZ_STATE_AUTOSCAN,
	PTZ_STATE_AUX,
	PTZ_STATE_MENU
} PTZ_STATE;

#if 0
#define STOP_AUTO(nChannel, dwAddress)\
do {\
	switch(m_u16PtzStatus[nChannel][0])\
	{\
	case PTZ_STATE_CRUISE:\
		m_aProtocolLib[nChannel]->objProtocol.PtzAutoCtrl(nChannel, dwAddress, TTY_PTZ_SETROUTE, 0, 0, m_u16PtzStatus[nChannel][1], 0, 0, 0);\
		m_u16PtzStatus[nChannel][0] = PTZ_STATE_STOP;\
		break;\
	case PTZ_STATE_TRACK:\
		m_aProtocolLib[nChannel]->objProtocol.PtzAutoCtrl(nChannel, dwAddress, TTY_PTZ_TRACKCALL, 0, 0, m_u16PtzStatus[nChannel][1], 0, 0, 0);\
		m_u16PtzStatus[nChannel][0] = PTZ_STATE_STOP;\
		break;\
	case PTZ_STATE_AUTOSCAN:\
		m_aProtocolLib[nChannel]->objProtocol.PtzAutoCtrl(nChannel, dwAddress, TTY_PTZ_AUTO, 0, 0, m_u16PtzStatus[nChannel][1], 0, 0, 0);\
		m_u16PtzStatus[nChannel][0] = PTZ_STATE_STOP;\
		break;\
	default:\
		break;\
	}\
} while (0)

#define CHECK_AUTO_PARAM(nChannel, dwAutoCode, dwCruiseIndex, bScan)\
do {\
if (bScan)\
{\
	if (((TTY_PTZ_SETROUTE == dwAutoCode) && (PTZ_STATE_CRUISE == m_u16PtzStatus[nChannel][0]) && (dwCruiseIndex == m_u16PtzStatus[nChannel][1]))\
		|| ((TTY_PTZ_TRACKCALL == dwAutoCode) && (PTZ_STATE_TRACK == m_u16PtzStatus[nChannel][0]) && (dwCruiseIndex == m_u16PtzStatus[nChannel][1]))\
		|| ((TTY_PTZ_AUTO == dwAutoCode) && (PTZ_STATE_AUTOSCAN == m_u16PtzStatus[nChannel][0]) && (dwCruiseIndex == m_u16PtzStatus[nChannel][1])))\
	{\
		return 0;\
	}\
}\
else\
{\
	if (((TTY_PTZ_SETROUTE == dwAutoCode) && ((PTZ_STATE_CRUISE != m_u16PtzStatus[nChannel][0]) || (dwCruiseIndex != m_u16PtzStatus[nChannel][1])))\
	|| ((TTY_PTZ_TRACKCALL == dwAutoCode) && ((PTZ_STATE_TRACK != m_u16PtzStatus[nChannel][0]) || (dwCruiseIndex != m_u16PtzStatus[nChannel][1])))\
	|| ((TTY_PTZ_AUTO == dwAutoCode) && ((PTZ_STATE_AUTOSCAN != m_u16PtzStatus[nChannel][0]) || (dwCruiseIndex != m_u16PtzStatus[nChannel][1]))))\
	{\
		return 0;\
	}\
}\
} while (0)

#define CHECK_PRESET_PARAM(nChannel, dwPresetIndex)\
do {\
	if ((PTZ_STATE_PRESET == m_u16PtzStatus[nChannel][0]) && (dwPresetIndex == m_u16PtzStatus[nChannel][1]))\
	{\
		return 0;\
	}\
} while(0)

#else
#define STOP_AUTO(nChannel, dwAddress)

#define CHECK_AUTO_PARAM(nChannel, dwAutoCode, dwCruiseIndex, bScan)

/*do {\
	if (!bScan)\
	{\
		if (((TTY_PTZ_SETROUTE == dwAutoCode) && ((PTZ_STATE_CRUISE != m_u16PtzStatus[nChannel][0]) || (dwCruiseIndex != m_u16PtzStatus[nChannel][1])))\
			|| ((TTY_PTZ_TRACKCALL == dwAutoCode) && ((PTZ_STATE_TRACK != m_u16PtzStatus[nChannel][0]) || (dwCruiseIndex != m_u16PtzStatus[nChannel][1])))\
			|| ((TTY_PTZ_AUTO == dwAutoCode) && ((PTZ_STATE_AUTOSCAN != m_u16PtzStatus[nChannel][0]) || (dwCruiseIndex != m_u16PtzStatus[nChannel][1]))))\
		{\
			STOP_AUTO(nChannel, dwAddress);\
		}\
	}\
} while (0)*/
#define CHECK_PRESET_PARAM(nChannel, dwPresetIndex)
#endif

#define CheckPreset(nChannel, nPresetIndex)																\
do {																									\
	int nTotalPresetNum = 0;																			\
	m_aProtocolLib[nChannel]->objProtocol.GetTechnicalData(&nTotalPresetNum, NULL, NULL, NULL, NULL);	\
	if ((int)nPresetIndex >= nTotalPresetNum)															\
	{																									\
		ret = -1;																				\
	}																									\
} while (0)

#define CheckCruise(nChannel, nCruiseIndex)																\
do {																									\
	int nTotalCruiseNum = 0;																			\
	m_aProtocolLib[nChannel]->objProtocol.GetTechnicalData(NULL, &nTotalCruiseNum, NULL, NULL, NULL);	\
	if ((int)dwCruiseIndex >= nTotalCruiseNum)															\
	{																									\
		return -1;																						\
	}																									\
} while (0)

#define CheckTrack(nChannel, nTrackIndex)																\
do {																									\
	int nTotalTrackNum = 0;																				\
	m_aProtocolLib[nChannel]->objProtocol.GetTechnicalData(NULL, NULL, &nTotalTrackNum, NULL, NULL);	\
	if ((int)nTrackIndex >= nTotalTrackNum)																\
	{																									\
		return -1;																						\
	}																									\
} while (0)

#define CheckAuto(nChannel, nAutoIndex)																	\
do {																									\
	if (!m_aProtocolLib[nChannel]->objProtocol.HasFunction(PTZ_FLAG_AUXAUTO))							\
	{																									\
		return -1;																						\
	}																									\
	int nTotalAutoNum = 0;																				\
	m_aProtocolLib[nChannel]->objProtocol.GetTechnicalData(NULL, NULL, NULL, &nTotalAutoNum, NULL);		\
	if ((int)nAutoIndex >= nTotalAutoNum)																\
	{																									\
		return -1;																						\
	}																									\
} while (0)

#define CheckChannel(nChannel, max) (((nChannel >= 0) || (nChannel < max)))

//!\brief 当速度为为0时取默认速度级别6
#define DEAL_SPEED_0(dwSpeed) (dwSpeed = ((!dwSpeed) ? 6 : dwSpeed))

#define RESET_TIMER(exp, nCh)			\
do										\
{										\
	if (exp)							\
	{									\
		m_pAdjuster->ResetTimer(nCh);	\
	}									\
} while(0)

#define UART_BAUDRATE_START_VAL	115200

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
#define PTZ_IS_MOTOR_CONTROL_CODE(nPtzCode) \
({ \
    int _ret = 0;\
    if (TTY_PTZ_ZOOMIN     != nPtzCode && TTY_PTZ_ZOOMOUT    != nPtzCode \
     && TTY_PTZ_ZOOM_AUTO  != nPtzCode && TTY_PTZ_ZOOM_STOP  != nPtzCode \
     && TTY_PTZ_IRIS_LARGE != nPtzCode && TTY_PTZ_IRIS_SMALL != nPtzCode \
     && TTY_PTZ_IRIS_AUTO  != nPtzCode && TTY_PTZ_IRIS_STOP  != nPtzCode \
     && TTY_PTZ_FOCUS_NEAR != nPtzCode && TTY_PTZ_FOCUS_FAR  != nPtzCode \
     && TTY_PTZ_FOCUS_AUTO != nPtzCode && TTY_PTZ_FOCUS_STOP != nPtzCode \
     ){\
         _ret = 1;\
     }\
     _ret;\
})
#endif

typedef struct tagSoftwareCruiseParam
{
	int nChannel;
	int nCruiseIndex;
} SoftwareCruiseParam, *LPSoftwareCruiseParam;

struct tagProtocolLib
{
  public:
	tagProtocolLib();
	~tagProtocolLib();

	//!\brief 检查协议是否是合法的协议
	//!\param szPath 协议存放的路径
	//!\return true表示合法 false表示不合法
	static bool IsValid(char *szPath);

	//!\brief 加载协议
	int Open(char *szLibPath);

	//!\brief 卸载协议
	int Close();
	//！\brief 截取字符串
	char* ExtractSubString(const char* input, char *output, const char* start_marker, const char* end_marker);

	void *hLib;

	CProtocol objProtocol;
};

tagProtocolLib::tagProtocolLib()
{
	hLib = NULL;
}

tagProtocolLib::~tagProtocolLib()
{
	if (hLib != NULL)
	{
		dlclose(hLib);
	}
}

bool tagProtocolLib::IsValid(char *szPath)
{
	ProtocolLib objLib;

	int ret = objLib.Open(szPath);

	if (0 == ret)
	{
		objLib.Close();
	}

	return (0 == ret);
}

int tagProtocolLib::Open(char *szLibPath)
{
	void *hNewLib = NULL;
	char SymString[128] = {0};
	char szProtocol[64] = { 0 };
	CProtocol objProtocolTmp;
	if(hLib)//如果之前有打开的，则先关闭
	{
		Close();
	}
	hNewLib = dlopen(szLibPath, RTLD_NOW);
	PTZ_DBG("dlopen Protocol %s, hNewLib=0x%x\n", szLibPath, hNewLib);

	if (hNewLib == NULL)
	{
		PTZ_ERROR("Error opening library: %s\n", dlerror());
		return -1;
	}

	// 手册说明,调用dlsym之前先清掉错误信息
	char *pError = dlerror();

	memset(SymString, '\0', sizeof(SymString));
	
	ExtractSubString(szLibPath, szProtocol,"IntDll/lib",".so");

#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "PtzNormalCtrl");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "PtzNormalCtrl");
#endif
	objProtocolTmp.PtzNormalCtrl = (DllPtzDevNormalCtrl) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "PtzAutoCtrl");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "PtzAutoCtrl");
#endif
	objProtocolTmp.PtzAutoCtrl = (DllPtzDevAutoCtrl) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "PtzPresetCtrl");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "PtzPresetCtrl");
#endif
	objProtocolTmp.PtzPresetCtrl = (DllPtzDevPresetCtrl) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "PtzAuxCtrl");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "PtzAuxCtrl");
#endif
	objProtocolTmp.PtzAuxCtrl = (DllPtzDevAuxCtrl) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "PtzMenuCtrl");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "PtzMenuCtrl");
#endif
	objProtocolTmp.PtzMenuCtrl = (DllPtzDevMenuCtrl) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "HasFunction");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "HasFunction");
#endif
	objProtocolTmp.HasFunction = (DllPtzDevHasFunction) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "SetCallback");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "SetCallback");
#endif
	objProtocolTmp.SetCallback = (DllPtzDevSetCallback) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "GetTechnicalData");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "GetTechnicalData");
#endif
	objProtocolTmp.GetTechnicalData = (DLLPtzDevGetTechnicalData) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}

	if (objProtocolTmp.HasFunction(PTZ_FLAG_AUXELETRONIC))
	{
		memset(SymString, '\0', sizeof(SymString));
	#if defined (__MUSL__)
		snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "GetEletronicValue");
	#else
		snprintf(SymString, sizeof(SymString), "%s%s", "", "GetEletronicValue");
	#endif
		objProtocolTmp.GetEletronicValue = (DLLPtzDevGetEletronicValue) dlsym(hNewLib, SymString);
		pError = dlerror();
		if (pError != NULL)
		{
			PTZ_ERROR(pError);
			dlclose(hNewLib);
			return -1;
		}

		memset(SymString, '\0', sizeof(SymString));
	#if defined (__MUSL__)
		snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "SetEletronicValue");
	#else
		snprintf(SymString, sizeof(SymString), "%s%s", "", "SetEletronicValue");
	#endif
		objProtocolTmp.SetEletronicValue = (DLLPtzDevSetEletronicValue) dlsym(hNewLib, SymString);
		pError = dlerror();
		if (pError != NULL)
		{
			PTZ_ERROR(pError);
			dlclose(hNewLib);
			return -1;
		}
	}

	memset(SymString, '\0', sizeof(SymString));
#if defined (__MUSL__)
	snprintf(SymString, sizeof(SymString), "%s%s", szProtocol, "GetVersion");
#else
	snprintf(SymString, sizeof(SymString), "%s%s", "", "GetVersion");
#endif
	objProtocolTmp.GetVersion = (DLLPtzDevGetVersion) dlsym(hNewLib, SymString);
	pError = dlerror();
	if (pError != NULL)
	{
		PTZ_ERROR(pError);
		dlclose(hNewLib);
		return -1;
	}
	else
	{
		objProtocol.PtzNormalCtrl = objProtocolTmp.PtzNormalCtrl;
		objProtocol.PtzPresetCtrl = objProtocolTmp.PtzPresetCtrl;
		objProtocol.PtzAutoCtrl = objProtocolTmp.PtzAutoCtrl;
		objProtocol.PtzAuxCtrl = objProtocolTmp.PtzAuxCtrl;
		objProtocol.PtzMenuCtrl = objProtocolTmp.PtzMenuCtrl;
		objProtocol.SetCallback = objProtocolTmp.SetCallback;
		objProtocol.GetTechnicalData = objProtocolTmp.GetTechnicalData;
		objProtocol.HasFunction = objProtocolTmp.HasFunction;
		objProtocol.GetEletronicValue = objProtocolTmp.GetEletronicValue;
		objProtocol.SetEletronicValue = objProtocolTmp.SetEletronicValue;
		objProtocol.GetVersion = objProtocolTmp.GetVersion;

		void *hOldLib = hLib;

		hLib = hNewLib;
		if (hOldLib != NULL)
		{
			//PTZ_INFO("Closing hOldLib = 0x%X\n", (unsigned int)hOldLib);
			//dlclose(hOldLib);
			//PTZ_INFO("Closed hOldLib = 0x%X\n", (unsigned int)hOldLib);
		}
		PTZ_DBG("Open Protocol %s\n", objProtocol.GetVersion());
	}

	return IDVR_OK;
}

int tagProtocolLib::Close()
{
	if (hLib != NULL)
	{
		objProtocol.PtzAutoCtrl = NULL;
		objProtocol.PtzNormalCtrl = NULL;
		objProtocol.PtzAutoCtrl = NULL;
		objProtocol.PtzPresetCtrl = NULL;
		objProtocol.PtzMenuCtrl = NULL;
		objProtocol.HasFunction = NULL;
		objProtocol.SetCallback = NULL;
		objProtocol.GetTechnicalData = NULL;
		objProtocol.GetVersion = NULL;
		dlclose(hLib);
		hLib = NULL;
	}

	return 0;
}

char* tagProtocolLib::ExtractSubString(const char* input, char *output, const char* start_marker, const char* end_marker)
{
	if (!input || !start_marker || !end_marker) {
        return NULL; // 输入参数无效
    }
	if (!output) {
        return NULL; // 缓存区失败
    }
    // 查找开始标记
    const char* start_pos = strstr(input, start_marker);
    if (!start_pos) {
        return NULL; // 开始标记未找到
    }

    // 移动到开始标记的结尾，准备查找结束标记
    start_pos += strlen(start_marker);

    // 查找结束标记
    const char* end_pos = strstr(start_pos, end_marker);
    if (!end_pos) {
        return NULL; // 结束标记未找到
    }

    // 计算中间字符串的长度
    size_t middle_len = end_pos - start_pos;

    // 复制中间字符串到新分配的内存
    strncpy(output, start_pos, middle_len);
    output[middle_len] = '\0'; // 添加终止符

    return output;
}

CPtzManager *CPtzManager::m_pInstance = NULL;

unsigned short CPtzManager::m_u16PtzStatus[MAX_PTZ_CHANNEL_NUM][2];

pthread_mutex_t CPtzManager::m_mtxSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

pthread_cond_t CPtzManager::m_condSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

int CPtzManager::m_nRunningSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

pthread_t CPtzManager::m_tSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

CPtzManager::CPtzManager():m_nEnableProtocol(0), m_nDisableProtocol(0), m_nChannelCount(0), m_pAdjuster(NULL)
{
	m_nChannelCount =ZWSys_GetTotalEncChannels();//1;

	int i = 0;

	for (i = 0; i < MAX_PTZ_CHANNEL_NUM; i++)
	{
		m_aProtocolLib[i] = NULL;
		m_aCPtzDev[i] = NULL;
		pthread_mutex_init(&m_mtxSoftwareCruise[i], NULL);
		pthread_cond_init(&m_condSoftwareCruise[i], NULL);
		m_nRunningSoftwareCruise[i] = -1;

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
		//编码器句柄
		m_pEncHandle[i] = NULL;
#endif
	}

    pthread_mutex_init(&m_mtxCruise, NULL);
    memset(&m_CruiseCfg, 0, sizeof(m_CruiseCfg));

	for (i = 0; i < (int) MAX_PROTOCOL_NUM; i++)
	{
		memset(m_aszProtocol[0][i], 0, MAX_PROTONAME_LEN + 2);
		memset(m_aszProtocol[1][i], 0, MAX_PROTONAME_LEN + 2);
	}

	m_nProtocolCount[0] = 0;
	m_nProtocolCount[1] = 0;

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	m_iPTZCtrlStatue = 0;

	CConfig *pConfig = CConfig::GetInstance();

	if(NULL != pConfig)
	{
		m_iNormalCtrlPreset = pConfig->GetValue("PTZ_Set_0", "NormalCtrlPreset", -1);
		m_iStayTimeSec = pConfig->GetValue("HumanShapeCfg", "TrackStayTimeSec", 60);        
        m_iWatchCareType   = pConfig->GetValue("PTZ_Set_0", "WatchCareType", 0);
        m_iWatchCareEnable = pConfig->GetValue("PTZ_Set_0", "WatchCareEnable", 0);
        m_iWatchCareReTime = pConfig->GetValue("PTZ_Set_0", "WatchCareReTime", 15);
	}
	m_iCalibrationStartFlag = 0;

    CConfig *pCfg = CConfig::NewInstance();
    int s32Ret = pCfg->LoadFile(CUSTOM_ABILITY_FILE);
    if (0 == s32Ret)
    {
        m_bSupportMotor = pCfg->GetValue("FunctionSupport", "IsSupportMotor", 0);
		m_bSupportMotor	= (m_bSupportMotor == 1) ? 1 : 0;
		m_bSupportVertical = pCfg->GetValue("FunctionSupport", "IsSupportVertical", 1);
		m_bSupportVertical = (m_bSupportVertical == 1) ? 1 : 0;
		m_bSupportCustomSpeed = pCfg->GetValue("FunctionSupport", "IsCustomSpeed", 0);
		m_iCustomOneStepSpeedV = pCfg->GetValue("FunctionSupport", "IsCustomOneStepSpeedV", 3);
		m_iCustomOneStepSpeedH = pCfg->GetValue("FunctionSupport", "IsCustomOneStepSpeedH", 3);
		m_bSupportSwap = pCfg->GetValue("FunctionSupport", "IsSupportSwapLRPTZChannel", 0);
    }
    CConfig::ReleaseInstance(pCfg);

	m_bManualMovePtzFlag = 0;
    m_nManualMoveTimeBegin = 0;

    m_inited = 0;
#endif
}

CPtzManager::~CPtzManager()
{
	int i = 0;

	for (i = 0; i < MAX_PTZ_CHANNEL_NUM; i++)
	{
		if (m_aProtocolLib[i] != NULL)
		{
			delete m_aProtocolLib[i];
			m_aProtocolLib[i] = NULL;
		}

		if (m_aCPtzDev[i] != NULL)
		{
			delete m_aCPtzDev[i];
			m_aCPtzDev[i] = NULL;
		}
		pthread_mutex_destroy(&m_mtxSoftwareCruise[i]);
	}

    pthread_mutex_destroy(&m_mtxCruise);
}

//##ModelId=49A4E1A00271
CPtzManager *CPtzManager::PTZ_GetInstance()
{
	// TODO: Add your specialized code here.
	if (NULL == m_pInstance)
	{
		m_pInstance = new CPtzManager();
		if (NULL != m_pInstance)
		{
			int i = 0;

			for (i = 0; i < MAX_PTZ_CHANNEL_NUM; i++)
			{
				m_u16PtzStatus[i][0] = 0;
				m_u16PtzStatus[i][1] = 0;
			}
		}
	}

	return (CPtzManager *) m_pInstance;
}

//##ModelId=49A4E1D502BF
void CPtzManager::PTZ_DestroyInstance(CPtzManager * pInstance)
{
	//只通知串口不再工作，不销毁实例
	CSerialDev::UnInit();
	// TODO: Add your specialized code here.
	//if (pInstance == m_pInstance)
	//{
	//  delete pInstance;
	//}
}

int CPtzManager::PTZ_Init(char *szInternalPath, char *szExternalPath)
{
	CSerialDev::Init();

	if ((NULL == szInternalPath)
		|| (NULL == szExternalPath)
		|| (strlen(szInternalPath) > PROTOCOL_PATH_MAX)
		|| (strlen(szExternalPath) > PROTOCOL_PATH_MAX) 
		|| (0 == strcmp(szInternalPath, szExternalPath)))
	{
		return IDVR_ERROR_PARAMETER;
	}
	
	CTransparentManager *pTPManager = CTransparentManager::GetInstance();

	if (NULL == pTPManager)
	{
		PTZ_INFO("透明通道初始化失败，不可用\n");
	}

	char szCurDir[DOCNAME_MAX];

	//记住当前目录,读完协议之后,恢复到该目录
	getcwd(szCurDir, DOCNAME_MAX);

	//获取内部协议名列表
	int ret = GetProtocolList(m_aszProtocol[0], &(m_nProtocolCount[0]), szInternalPath, MAX_PROTOCOL_NUM);

	if (ret != 0)
	{
		//获取失败
		chdir(szCurDir);
		return IDVR_PTZ_ERROR_LOAD_INTERNAL_PROTOCOL;
	}

	chdir(szCurDir);
	//记录内部协议目录
	strncpy(m_szPath[0], szInternalPath, sizeof(m_szPath[0]) - 1);
	PTZ_DBG("Internal Dir: %s\n", m_szPath[0]);

	//获取外部协议列表
	ret = GetProtocolList(m_aszProtocol[1], &(m_nProtocolCount[1]), szExternalPath, MAX_PROTOCOL_NUM);

	if (ret != 0)
	{
		chdir(szCurDir);
		return IDVR_PTZ_ERROR_LOAD_EXTERNAL_PROTOCOL;
	}

	PTZ_INFO("m_nProtocolCount[0] = %d, m_nProtocolCount[1] = %d, m_nEnableProtocol[0] = %d, m_nDisableProtocol[1] = %d", m_nProtocolCount[0],
			 m_nProtocolCount[1], m_nEnableProtocol, m_nDisableProtocol);
	int i = 0;

	for (i = 0; i < m_nProtocolCount[1]; i++)
	{
		//去掉外部协议中以'_'开头的
		if ('_' == m_aszProtocol[1][i][0])
		{
			memset(m_aszProtocol[1][i], 0, MAX_PROTONAME_LEN + 2);
			m_nDisableProtocol--;
		}
		else
		{
			//去掉与内部协议相同的外部协议
			int nIndex = GetProtocolIndex(m_aszProtocol[1][i], 0);

			if (-1 != nIndex)
			{
				memset(m_aszProtocol[1][i], 0, MAX_PROTONAME_LEN + 2);
				m_nEnableProtocol--;
			}
		}
	}

	int j = 1;

	for (i = 0; ((i < m_nProtocolCount[1]) && (j <= m_nProtocolCount[1])); i++, j++)
	{
		if ('\0' == m_aszProtocol[1][i][0])
		{
			for (; j < m_nProtocolCount[1]; j++)
			{
				if ('\0' != m_aszProtocol[1][j][0])
				{
					strncpy(m_aszProtocol[1][i], m_aszProtocol[1][j], sizeof(m_aszProtocol[1][i]) - 1);
					memset(m_aszProtocol[1][j], 0, MAX_PROTONAME_LEN + 2);
					break;
				}
			}

			PTZ_DBG("Ext Dll Is Invalid i = %d, j = %d\n", i, j);

			if (j >= m_nProtocolCount[1])
			{
				m_nProtocolCount[1] = i;
				break;
			}
		}
	}

	strncpy(m_szPath[1], szExternalPath, sizeof(m_szPath[1]) - 1);
	PTZ_DBG("External Dir: %s\n", m_szPath[1]);

	chdir(szCurDir);

#ifndef NDEBUG
	getcwd(szCurDir, DOCNAME_MAX);
	PTZ_DBG("Current Dir: %s\n", szCurDir);
#endif

	PTZ_DBG("m_nProtocolCount[0] = %d, m_nProtocolCount[1] = %d, m_nEnableProtocol[0] = %d, m_nDisableProtocol[1] = %d", m_nProtocolCount[0],
			m_nProtocolCount[1], m_nEnableProtocol, m_nDisableProtocol);

	m_bDorEnable = 0;

	//初始化解码器参数,并加载云台协议
	ret = InitDev();
	if (0 != ret)
	{
		return ret;
	}

	//读取云台巡航线路
	ret = LoadCruise();
	m_pAdjuster = CPtzAdjuster::GetInstance();
	if (NULL == m_pAdjuster)
	{
		PTZ_ERROR("Get PtzAdjuster Instance Failed\n");
	}
	ret = m_pAdjuster->Init(this, m_nChannelCount);
	ZWSys_SetSerialDataCallBack(FnSendRawData);
	#if defined (__IS_SOPPORT_ZOOM_OSD__)
	ZWSys_SetSerialDataWriteCallBack(FnRecvRawData);
	#endif

	PTZParam_t ptzParam = {0};
	ptzParam.baudRate = PTZ_GetBaudrate(0);
	ZWSys_SetSerialCfg(&ptzParam);

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
	for (i = 0; i < m_nChannelCount; ++i)
	{
		char name[64];

		snprintf(name, sizeof (name), "/mnt/mtd/config/eptz%d.cfg", i);

		//读取坐标
		FILE *fp = fopen(name, "r");

		if (NULL != fp)
		{
			char fbuf[16] = { 0 };
			unsigned long pos[2];
			fread(fbuf, sizeof(char), sizeof(fbuf), fp);
			sscanf(fbuf, "%lu,%lu", &pos[0], &pos[1]);
			// fscanf(fp, "%lu,%lu", &pos[0], &pos[1]);
			fclose(fp);

			PTZ_EletronicCtrl(i, TTY_PTZ_SET_POS, (unsigned int*)pos);
		}
	}
#endif

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	m_inited = 1;
#endif
	return ret;
}

//##ModelId=49A37005030D
int CPtzManager::PTZ_GetProtocol(char *szProtocol, int nLength, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if ((NULL == szProtocol) || (0 == nLength))
	{
		return IDVR_ERROR_PARAMETER;
	}
	// TODO: Add your specialized code here.

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault\n");
		return -1;
	}

	int ret = m_aCPtzDev[nChannel]->GetProtocol(szProtocol, nLength);

	return (int) ret;
}

//##ModelId=49A49A110251
int CPtzManager::PTZ_SetProtocol(char *szProtocol, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == szProtocol)
	{
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault\n");
		return -1;
	}

	char szOldProtocol[MAX_PROTONAME_LEN + 1];

	m_aCPtzDev[nChannel]->GetProtocol(szOldProtocol, MAX_PROTONAME_LEN + 1);
	if (0 == strcmp(szProtocol, szOldProtocol))
	{
		return 0;
	}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	if (0 == strncmp(szOldProtocol, "Gpio", sizeof(szOldProtocol))
		|| 0 == strncmp(szOldProtocol, "Gpio_Pelcod", sizeof(szOldProtocol)))	
	{
        PTZ_StopMotorCruise(0);
	}
#endif

	int nIndex = GetProtocolIndex(szProtocol, 0);

	int ret = 0;

	char szProtocolFile[PROTOCOL_PATH_MAX + PROTOCOL_NAME_MAX + 32];

	//该协议为内部协议
	if (nIndex != -1)
	{
		if ('_' == m_aszProtocol[0][nIndex][0])
		{
			return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
		}
		ret = m_aCPtzDev[nChannel]->SetProtocol(szProtocol);
		if (0 == ret)
		{
			//更换协议，停止云台动作
			PTZ_NormalCtrl(TTY_PTZ_STOP, 0, nChannel);
			snprintf(szProtocolFile,sizeof(szProtocolFile), "%s/lib%s.so", m_szPath[0], szProtocol);
			m_aProtocolLib[nChannel]->Open(szProtocolFile);
			//重新设置回调
			m_aProtocolLib[nChannel]->objProtocol.SetCallback(FnSendData, FnRecvData, StartSoftwareCruise, StopSoftwareCruise);
			InitEletronicType(nChannel);
		}
		return ret;
	}

	nIndex = GetProtocolIndex(szProtocol, 1);
	//该协议为外部协议
	if (nIndex != -1)
	{
		ret = m_aCPtzDev[nChannel]->SetProtocol(szProtocol);
		if (0 == ret)
		{
			//更换协议，停止云台动作
			PTZ_NormalCtrl(TTY_PTZ_STOP, 0, nChannel);
			snprintf(szProtocolFile,sizeof(szProtocolFile), "%s/lib%s.so", m_szPath[1], szProtocol);
			//加载协议
			m_aProtocolLib[nChannel]->Open(szProtocolFile);
			//重新设置回调
			m_aProtocolLib[nChannel]->objProtocol.SetCallback(FnSendData, FnRecvData, StartSoftwareCruise, StopSoftwareCruise);
		}
	}

	// TODO: Add your specialized code here.
	//协议不存在
	return (int) IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
}

int CPtzManager::PTZ_GetTechnicalData(int nChannel, int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
{
	int nChannelCount = m_nChannelCount;

	if (NULL != PresetCount)
	{
		*PresetCount = 0;
	}
	if (NULL != CruiseCount)
	{
		*CruiseCount = 0;
	}
	if (NULL != TrackCount)
	{
		*TrackCount = 0;
	}
	if (NULL != AutoCount)
	{
		*AutoCount = 0;
	}
	if (NULL != Baudrate)
	{
		*Baudrate = 0;
	}
	if (!CheckChannel(nChannel, nChannelCount))
	{
		return (int) -1;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault\n");
		return (int) -1;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		PTZ_ERROR("Detect No Protocol\n");
		return (int) -1;
	}

	// TODO: Add your specialized code here.
	if (NULL == m_aProtocolLib[nChannel]->objProtocol.GetTechnicalData)
	{
		return (int) -1;
	}

	m_aProtocolLib[nChannel]->objProtocol.GetTechnicalData(PresetCount, CruiseCount, TrackCount, AutoCount, Baudrate);

	return (int) 0;

}

//##ModelId=49B71BF3038A
unsigned long CPtzManager::PTZ_GetAddress(int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return (unsigned long) -1;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return (unsigned long) -1;
	}

	// TODO: Add your specialized code here.
	int ret = m_aCPtzDev[nChannel]->GetAddress();

	return (int) ret;
}

//##ModelId=49B71BFD02FD
int CPtzManager::PTZ_SetAddress(unsigned long dwAddress, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return -1;
	}

	// TODO: Add your specialized code here.
	int ret = m_aCPtzDev[nChannel]->SetAddress(dwAddress);

	return (int) ret;
}

//##ModelId=49B71C070280
unsigned long CPtzManager::PTZ_GetBaudrate(int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return (unsigned long) -1;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return (unsigned long) -1;
	}

	unsigned long ret = m_aCPtzDev[nChannel]->GetBaudRate();

	// TODO: Add your specialized code here.
	return (unsigned long) ret;
}

//##ModelId=49B71C170186
int CPtzManager::PTZ_SetBaudrate(unsigned long dwBaudrate, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return -1;
	}

	unsigned long ret = m_aCPtzDev[nChannel]->SetBaudRate(dwBaudrate);
	PTZParam_t ptzParam;
	ptzParam.baudRate = dwBaudrate;
	ZWSys_SetSerialCfg(&ptzParam);

	// TODO: Add your specialized code here.
	return (int) ret;
}

int CPtzManager::PTZ_GetEletronicProp(int nPropIndex, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return (int) -1;
	}

	if (!PTZ_GetType(nChannel))
	{
		return IDVR_ERROR_UNSUPPORT;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return (int) -1;
	}

	int ret = m_aCPtzEletronic[nChannel]->GetEletronicProp(nPropIndex);

	return (int) ret;
}

int CPtzManager::PTZ_SetEletronicProp(int nValue, int nPropIndex, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (!PTZ_GetType(nChannel))
	{
		return IDVR_ERROR_UNSUPPORT;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return -1;
	}

	int ret = m_aProtocolLib[nChannel]->objProtocol.SetEletronicValue(nChannel, m_aCPtzDev[nChannel]->GetAddress(), nPropIndex, nValue, 0);

	if (DLL_OUTPUT_OK == ret)
	{
		ret = m_aCPtzEletronic[nChannel]->SetEletronicProp(nPropIndex, nValue);
	}
	else
	{
		ret = -1;
	}

	return (int) ret;
}

int CPtzManager::PTZ_ResetAllEletronicProp(int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (!PTZ_GetType(nChannel))
	{
		return IDVR_ERROR_UNSUPPORT;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return -1;
	}

	int ret = m_aProtocolLib[nChannel]->objProtocol.PtzAuxCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), TTY_AUX_RESETPARAM, 0);

	if (DLL_ERROR != ret)
	{
		ret = m_aCPtzEletronic[nChannel]->ResetAllEletronicProp();
	}
	else
	{
		ret = -1;
	}

	return (int) ret;
}

int CPtzManager::PTZ_GetType(int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return 0;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return 0;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return 0;
	}

	return m_aCPtzEletronic[nChannel]->GetType();
}

int CPtzManager::PTZ_GetTemperature(int nChannel, int *pnValue)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (!PTZ_GetType(nChannel))
	{
		return IDVR_ERROR_UNSUPPORT;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return -1;
	}

	int ret = m_aProtocolLib[nChannel]->objProtocol.GetEletronicValue(nChannel, m_aCPtzDev[nChannel]->GetAddress(), eStateTemperature, pnValue, NULL);

	return (DLL_OUTPUT_OK == ret) ? 0 : -1;
}

int CPtzManager::PTZ_GetCoordinate(int nChannel, int *pnHorizontalAngle, int *pnVerticalAngle, int *pnMultiple, int *pnOrientation)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (!PTZ_GetType(nChannel))
	{
		return IDVR_ERROR_UNSUPPORT;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault");
		return -1;
	}

	int nValue1 = 0;

	int nValue2 = 0;

	int ret =
		m_aProtocolLib[nChannel]->objProtocol.GetEletronicValue(nChannel, m_aCPtzDev[nChannel]->GetAddress(), eStateCoordinate, &nValue1, &nValue2);
	if (DLL_OUTPUT_OK == ret)
	{
		*pnHorizontalAngle = (short) (nValue1 >> 16);
		*pnVerticalAngle = (short) (nValue1 & 0xFFFF);
		*pnMultiple = (short) (nValue2 >> 16);
		*pnOrientation = (short) (nValue2 & 0xFFFF);
		return 0;
	}
	else
	{
		return -1;
	}
}

int CPtzManager::PTZ_OpenCom(int nChannel)
{
	if (!CheckChannel(nChannel, m_nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault\n");
		return -1;
	}

	return m_pInstance->m_aCPtzDev[nChannel]->Open();
}

int CPtzManager::PTZ_Get485Config(int nChannel, int *pnAddress, int *pnProtocol, int *pnBaudRate)
{
/*
	PTZParam_t T;
	
	MP4Sys_GetPTZParam(&T);
	*pnAddress = T.addr;
	*pnProtocol = T.protocol;
	*pnBaudRate = T.band;
	return 0;
	

	if (!CheckChannel(nChannel, m_nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (!PTZ_GetType(nChannel))
	{
		return IDVR_ERROR_UNSUPPORT;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if ((NULL == m_aCPtzDev[nChannel]) || (NULL == m_aCPtzEletronic[nChannel]))
	{
		PTZ_ERROR("PTZ Was Not Init, Memory Fault\n");
		return -1;
	}
*/
	int nValue1 = 0;

	int nValue2 = 0;

	int ret = m_aProtocolLib[nChannel]->objProtocol.GetEletronicValue(nChannel, m_aCPtzDev[nChannel]->GetAddress(), eState485Com, &nValue1, &nValue2);

	if (DLL_OUTPUT_OK == ret)
	{
		*pnAddress = nValue1 & 0xFF;
		*pnProtocol = (nValue1 >> 8) & 0xFF;
		*pnBaudRate = (nValue1 >> 16) & 0xFF;
		return 0;
	}
	else
	{
		return -1;
	}
}

//##ModelId=49B46A4C0222
int CPtzManager::PTZ_GetProtocolList(char pProtocolList[][256], int nLength, int bEnable, int bExternal[])
{
	if ((pProtocolList == NULL) || (bEnable < 0) || (bEnable > 1))
	{
		return -1;
	}

	// TODO: Add your specialized code here.
	int i = 0;

	int nCount = 0;

	while ((i < m_nProtocolCount[0]) && (nCount < nLength))
	{
		if (((1 == bEnable) && ('_' == m_aszProtocol[0][i][0])) || ((0 == bEnable) && ('_' != m_aszProtocol[0][i][0])))
		{
			i++;
			continue;
		}

		if (1 == bEnable)
		{
			strncpy(pProtocolList[nCount], m_aszProtocol[0][i], sizeof(pProtocolList[nCount]) - 1);
		}
		else
		{
			strncpy(pProtocolList[nCount], m_aszProtocol[0][i] + 1, sizeof(pProtocolList[nCount]) - 1);
		}
		if (NULL != bExternal)
		{
			bExternal[nCount] = 0;
		}
		i++;
		nCount++;
	}

	if ((0 == bEnable) || (nCount == nLength))
	{
		return nCount;
	}

	i = 0;
	while ((i < m_nProtocolCount[1]) && (nCount < nLength))
	{
		strncpy(pProtocolList[nCount], m_aszProtocol[1][i], sizeof(pProtocolList[nCount]) - 1);
		if (NULL != bExternal)
		{
			bExternal[nCount] = 1;
		}
		i++;
		nCount++;
	}

	return (int) nCount;
}

//##ModelId=49B47F0B0399
int CPtzManager::PTZ_GetProtocolCount(int bEnable)
{
	if ((bEnable < 0) || (bEnable > 1))
	{
		return -1;
	}
	// TODO: Add your specialized code here.

	return (int) ((1 == bEnable) ? m_nEnableProtocol : m_nDisableProtocol);
}

//##ModelId=49B46A6201B5
int CPtzManager::PTZ_InstallProtocol(char *strSrcPath)
{
	PTZ_DBG("Install Protocol %s\n", strSrcPath);
	if (NULL == strSrcPath)
	{
		return IDVR_ERROR_PARAMETER;
	}

	char *pFileName = GetFileNameFromPath(strSrcPath);

	char szProtocol[MAX_PROTONAME_LEN + 20];

	char szDstPath[DOCNAME_MAX];

	int ret = 0;

	//内部协议
	if (pFileName == strSrcPath)
	{
		snprintf(szProtocol, sizeof(szProtocol), "_%s", pFileName);
		int nIndex = GetProtocolIndex(szProtocol, 0);

		PTZ_DBG("szProtocol=%s, nIndex = %d\n", szProtocol, nIndex);
		if (-1 == nIndex)
		{
			return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
		}

		strncpy(m_aszProtocol[0][nIndex], pFileName, sizeof(m_aszProtocol[0][nIndex]) - 1);
		char szSrcPath[DOCNAME_MAX];

		snprintf(szSrcPath,sizeof(szSrcPath), "%s/lib_%s.so", m_szPath[0], pFileName);
		snprintf(szDstPath,sizeof(szDstPath), "%s/lib%s.so", m_szPath[0], pFileName);
		PTZ_DBG("rename %s to %s\n", szSrcPath, szDstPath);
		ret = rename(szSrcPath, szDstPath);
		m_nEnableProtocol++;
		m_nDisableProtocol--;
	}
	else
	{
		if (m_nProtocolCount[1] >= (int) MAX_PROTOCOL_NUM)
		{
			return IDVR_PTZ_ERROR_PROTOCOL_TOOMUCH;
		}

		ret = CheckProtocol(strSrcPath, szProtocol);
		if (0 != ret)
		{
			return ret;
		}

		snprintf(szDstPath,sizeof(szDstPath), "%s/%s", m_szPath[1], pFileName);
		ret = CopyFileFromUsb(strSrcPath, szDstPath, 0);
		if (0 == ret)
		{
			strncpy(m_aszProtocol[1][m_nProtocolCount[1]], szProtocol, sizeof(m_aszProtocol[1][m_nProtocolCount[1]]) - 1);
			m_nEnableProtocol++;
			m_nProtocolCount[1]++;
		}
	}

	// TODO: Add your specialized code here.
	return (int) ret;
}

//##ModelId=49B46A6F003E
int CPtzManager::PTZ_UninstallProtocol(char *szProtocol, char *szDstPath)
{
	PTZ_DBG("Protocol=%s, szDstPath = %s\n", szProtocol, szDstPath);
	if (NULL == szProtocol)
	{
		return IDVR_ERROR_PARAMETER;
	}

	int nChannelCount = m_nChannelCount;

	//判断协议是否被某个通道所使用
	for (int i = 0; i < nChannelCount; i++)
	{
		if (m_aCPtzDev[i]->UseProtocol(szProtocol))
		{
			return IDVR_PTZ_ERROR_PROTOCOL_IS_USED;
		}
	}

	// TODO: Add your specialized code here.
	// 判断协议是否存在
	int nIndex = GetProtocolIndex(szProtocol, 0);

	if (nIndex != -1)
	{
		if ('_' == m_aszProtocol[0][nIndex][0])
		{
			return 0;
		}
		char strSrcPath[DOCNAME_MAX];

		char strDstPath[DOCNAME_MAX];

		char szInternal[50];

		snprintf(szInternal,sizeof(szInternal), "_%s", szProtocol);
		snprintf(strSrcPath,sizeof(strSrcPath), "%s/lib%s.so", m_szPath[0], szProtocol);
		snprintf(strDstPath,sizeof(strDstPath), "%s/lib%s.so", m_szPath[0], szInternal);
		PTZ_DBG("rename %s to %s", strSrcPath, strDstPath);

		rename(strSrcPath, strDstPath);
		strncpy(m_aszProtocol[0][nIndex], szInternal, sizeof(m_aszProtocol[0][nIndex]) - 1);
		m_nEnableProtocol--;
		m_nDisableProtocol++;
		return 0;
	}

	nIndex = GetProtocolIndex(szProtocol, 1);
	if (nIndex != -1)			//找到该协议在外部协议中
	{
		char szSrcPathName[DOCNAME_MAX];

		snprintf(szSrcPathName,sizeof(szSrcPathName), "%s/lib%s.so", m_szPath[1], szProtocol);

		int ret = 0;

		if ((NULL == szDstPath) || (0 == strlen(szDstPath)))
		{
			ret = unlink(szSrcPathName);
		}
		else if (strstr(szDstPath, "/usb/sd") != szDstPath)
		{
			PTZ_ERROR("Invalid USB Path %s\n", szDstPath);
			return -1;
		}
		else
		{
			char szDstPathName[DOCNAME_MAX];

			snprintf(szDstPathName,sizeof(szDstPathName), "%s/lib%s.so", szDstPath, szProtocol);
			ret = CopyFileToUsb(szSrcPathName, szDstPathName, 1);
		}

		if (0 == ret)
		{
			int i = 0;

			for (i = nIndex; i < m_nProtocolCount[1] - 1; i++)
			{
				strncpy(m_aszProtocol[1][i], m_aszProtocol[1][i + 1], sizeof(m_aszProtocol[1][i]) - 1);
			}
			memset(m_aszProtocol[1][m_nProtocolCount[1] - 1], 0, MAX_PROTONAME_LEN + 2);
			m_nEnableProtocol--;
			m_nProtocolCount[1]--;
		}
		else
		{
			return ret;
		}

		return 0;
	}

	PTZ_ERROR("Can not Find This Protocol %s\n", szProtocol);
	return (int) -1;
}

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
//!\brief   电子PTZ控制
//!\param   dwPtzCode       云台控制码 见TTY_MPU_PTZ_CODE
//!\param   pdwParamList    控制参数
//!\param   nChannel            云台对应的通道号
//!\return  0表示成功，其他表示错误码
//!\sa TTY_MPU_PTZ_CODE
int CPtzManager::PTZ_EletronicCtrl(int nChannel, unsigned long dwPtzCode, unsigned int *pdwParamList)
{
	int nRet = 0;

	if (!CheckChannel(nChannel, m_nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_pEncHandle[nChannel])
	{
		m_pEncHandle[nChannel] = ZWSys_EncChannelOpen(nChannel);

		if (NULL == m_pEncHandle[nChannel])
		{
			return IDVR_ERROR_HISDK;
		}
	}
/*
	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			if (0 != MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_UP, pdwParamList))
			{
				nRet = IDVR_ERROR_HISDK;
			}
			break;

		case TTY_PTZ_DOWN:
			if (0 != MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_DOWN, pdwParamList))
			{
				nRet = IDVR_ERROR_HISDK;
			}
			break;

		case TTY_PTZ_LEFT:
			if (0 != MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_LEFT, pdwParamList))
			{
				nRet = IDVR_ERROR_HISDK;
			}
			break;

		case TTY_PTZ_RIGHT:
			if (0 != MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_RIGHT, pdwParamList))
			{
				nRet = IDVR_ERROR_HISDK;
			}
			break;

		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
			{
				unsigned long pos[2];

				//停止电子PTZ动作
				if (0 != MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_STOP, pdwParamList))
				{
					nRet = IDVR_ERROR_HISDK;
				}

				//记录当前坐标
				if (0 == MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_GETPOS, (unsigned int*)pos))
				{
					char name[32];

					snprintf(name, sizeof (name), "/usr/local/etc/eptz%d.cfg", nChannel);

					//读取坐标
					FILE *fp = fopen(name, "w");

					if (NULL != fp)
					{
						fprintf(fp, "%lu,%lu", pos[0], pos[1]);
						fclose(fp);
					}
				}
			}
			break;

		case TTY_PTZ_SET_POS:
			if (0 != MP4Sys_EncSetEPTZ(m_pEncHandle[nChannel], EPTZ_SETPOS, pdwParamList))
			{
				nRet = IDVR_ERROR_HISDK;
			}
			break;

		default:
			PTZ_ERROR("unsupported eletronic ptz code: %lu\n", dwPtzCode);
			nRet = IDVR_ERROR_PARAMETER_INVALID_POINTER;
			break;
	}
	*/

	return nRet;
}
#endif

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
//!\brief	设置ptz 水平默认值
//!\param	nChannel  通道号 	nHorizontalAngle  水平默认值
//!\return	-1为失败
int CPtzManager::PTZ_SetHorizontalAngle(int nChannel, int nHorizontalAngle)
{
	m_aCPtzDev[nChannel]->SetHorizontalAngle(nHorizontalAngle);
	
	return 0;
}

//!\brief   设置ptz 纵向默认值
//!\param   nChannel  通道号     nVerticalAngle  纵向默认值
//!\return  -1为失败
int CPtzManager::PTZ_SetVerticalAngle(int nChannel, int nVerticalAngle)
{
	m_aCPtzDev[nChannel]->SetVerticalAngle(nVerticalAngle);

	return 0;
}

//!\brief	设置ptz 水平反向
//!\param	nChannel  通道号 	nHorizontalReverse  水平反向
//!\return	-1为失败
int CPtzManager::PTZ_SetHorizontalReverse(int nChannel, int nHorizontalReverse)
{
	m_aCPtzDev[0]->SetHorizontalReverse(nHorizontalReverse,nChannel);
	
	return 0;
}

//!\brief   设置ptz 纵向反向
//!\param   nChannel  通道号     nVerticalReverse  纵向反向
//!\return  -1为失败
int CPtzManager::PTZ_SetVerticalReverse(int nChannel, int nVerticalReverse)
{
	m_aCPtzDev[0]->SetVerticalReverse(nVerticalReverse,nChannel);

	return 0;
}

//!\brief   设置ptz 水平步长
//!\param   nChannel  通道号     nHorizontalStep  水平步长
//!\return  -1为失败
int CPtzManager::PTZ_SetHorizontalStep(int nChannel, int nHorizontalStep)
{
	m_aCPtzDev[0]->SetHorizontalStep(nHorizontalStep,nChannel);
	
	return 0;
}

//!\brief   设置ptz 纵向步长
//!\param   nChannel  通道号     nVerticalStep  纵向步长
//!\return  -1为失败
int CPtzManager::PTZ_SetVerticalStep(int nChannel, int nVerticalStep)
{
	m_aCPtzDev[0]->SetVerticalStep(nVerticalStep,nChannel);

	return 0;
}

//!\brief   设置ptz 水平速度
//!\param   nChannel  通道号     nHorizontalStep  水平速度
//!\return  -1为失败
int CPtzManager::PTZ_SetMaxSpeedH(int nChannel, int nMaxSpeedH)
{
	m_aCPtzDev[0]->SetMaxSpeedH(nMaxSpeedH,nChannel);
	
	return 0;
}

//!\brief   设置ptz 纵向速度
//!\param   nChannel  通道号     nVerticalStep  纵向速度
//!\return  -1为失败
int CPtzManager::PTZ_SetMaxSpeedV(int nChannel, int nMaxSpeedV)
{
	m_aCPtzDev[0]->SetMaxSpeedV(nMaxSpeedV,nChannel);

	return 0;
}



//设置用户定制云台垂直最大步长
int CPtzManager::PTZ_SetUserVMaxStep(int nChannel, int nUserVMaxStep)
{
	m_aCPtzDev[nChannel]->SetUserVMaxStep(nUserVMaxStep);

	return 0;
}

//设置产测状态到云台模块
int CPtzManager::PTZ_SetProductTestMode(int nTestMode)
{
	MOTO_SetProductTestMode(nTestMode);

	return 0;
}

//获取产测状态
int CPtzManager::PTZ_GetProductTestMode()
{
	return MOTO_GetProductTestMode();
}

//!\brief   设置ptz 支持垂直跟踪
//!\param   nChannel  通道号     SupportVertical  垂直跟踪
//!\return  -1为失败
int CPtzManager::PTZ_SetSupportVertical(int nChannel, int nSupportVertical)
{
	m_bSupportVertical = nSupportVertical;

	return 0;
}

//!\brief   设置ptz 支持云台
//!\param   nChannel  通道号     SupporMotor  支持云台
//!\return  -1为失败
int CPtzManager::PTZ_SetSupporMotor(int nChannel, int nSupporMotor)
{
	m_bSupportMotor = nSupporMotor;

	return 0;
}

//!\brief   设置ptz 支持电机交换
//!\param   nSupporSwap  支持电机交换
//!\return  -1为失败
int CPtzManager::PTZ_SetPTZDevSwapLRPTZChannel(int nSupporSwap)
{
	m_bSupportSwap = nSupporSwap;

	return 0;
}


#ifdef __AGINGTEST__
int CPtzManager::PTZ_GetMaxStep (unsigned int nChannel, int *pnHMaxStep, int *pnVMaxStep)
{
	if (nChannel < 0 || nChannel >= MAX_PTZ_CHANNEL_NUM || NULL == pnHMaxStep || NULL == pnVMaxStep)
	{
		PTZ_ERROR ("nChannel<=%d> not valid, max = %d\n", nChannel, MAX_PTZ_CHANNEL_NUM);
		return -1;
	}
	int nX = 0, nY = 0;
	m_aCPtzDev[0]->GetMaxStep (& nX, & nY,nChannel);

	*pnHMaxStep = nX;
	*pnVMaxStep = nY;

	return 0;
}
#endif

//设置看守位（由客户端配置）
int CPtzManager::PTZ_SetNormalCtrlPreset(int nChannel, int nNormalCtrlPreset)
{
    if (nChannel < 0 || nChannel >= MAX_PTZ_CHANNEL_NUM)
    {
        PTZ_ERROR("nChannel<=%d> not valid, max = %d\n", nChannel, MAX_PTZ_CHANNEL_NUM);
        return -1;
    }

    if (m_iWatchCareType != 0)
    {
        PTZ_ERROR("SetNormalCtrlPreset Failed! WatchCareType = %d\n", m_iWatchCareType);
        return -1;
    }

	int nRet =0;
	int nPos_h = 0, nPos_v = 0;

	char szSectionName[20] = {0};
	CConfig *pConfig = CConfig::GetInstance();
	
	if (nNormalCtrlPreset > 256)
	{
		return -1;
	}

	if (nNormalCtrlPreset == m_iNormalCtrlPreset)
	{
		return 0;
	}

    /* 修改看守位后，需要重新更新标志 */
    struct timeval NowTime;
    gettimeofday(&NowTime, NULL);
    m_nManualMoveTimeBegin = NowTime.tv_sec;
    m_bManualMovePtzFlag = 1;

	m_iNormalCtrlPreset = ((nNormalCtrlPreset < 0) ? -1 : nNormalCtrlPreset);
	if (m_iNormalCtrlPreset < 0)
    {
        snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
        pConfig->SetValue(szSectionName, "NormalCtrlPreset", m_iNormalCtrlPreset);
        return 0;
    }

	nRet = MOTO_GetPresetPos(m_iNormalCtrlPreset, &nPos_h, &nPos_v);
	if (0 != nRet)
    {   
		return -1;
	}
    
    m_aCPtzDev[nChannel]->SetHorizontalAngle(nPos_h);
    m_aCPtzDev[nChannel]->SetVerticalAngle(nPos_v);

    snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", nChannel);
    pConfig->SetValue(szSectionName, "NormalCtrlPreset", m_iNormalCtrlPreset);
	return 0;
}

//获得看守位（客户端调用）
int CPtzManager::PTZ_GetNormalCtrlPreset(int nChannel, int *nNormalCtrlPreset)
{
	if (NULL == nNormalCtrlPreset)
	{
		return -1;
	}

	*nNormalCtrlPreset = m_iNormalCtrlPreset;
	return 0;
}

// 新版本看守位使能
int CPtzManager::PTZ_SetWatchCareEnable(int nChannel, int iEnable)
{
    if (nChannel < 0 || nChannel >= MAX_PTZ_CHANNEL_NUM)
    {
        PTZ_ERROR("nChannel<=%d> not valid, max = %d\n", nChannel, MAX_PTZ_CHANNEL_NUM);
        return -1;
    }

    if (iEnable)
    {
    	int nPos_h = 0, nPos_v = 0;
        if (0 == MOTO_GetPos(&nPos_h, &nPos_v))
        {
            PTZ_WARN("SetWatchCareEnable nPos_h %d nPos_v %d\n", nPos_h, nPos_v);
            m_aCPtzDev[nChannel]->SetHorizontalAngle(nPos_h);
            m_aCPtzDev[nChannel]->SetVerticalAngle(nPos_v);
        }
    }
    else
    {
    	int nPos_h = 0, nPos_v = 0;
        //需要清除转动标识
        if (PTZ_GetManualMovePtzFlag())
        {
            PTZ_ClearManualMovePtzFlag();
			if (0 == MOTO_GetPos(&nPos_h, &nPos_v))
			{
				PTZ_WARN("SetWatchCareEnable nPos_h %d nPos_v %d\n", nPos_h, nPos_v);
				m_aCPtzDev[nChannel]->SetHorizontalAngle(nPos_h);
				m_aCPtzDev[nChannel]->SetVerticalAngle(nPos_v);
			}
        }
    }

    if (iEnable == m_iWatchCareEnable)
    {
        return 0;
    }

	char szSectionName[32] = {0};
    CConfig *pConfig = CConfig::GetInstance();

    snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
	pConfig->SetValue(szSectionName, "WatchCareEnable", iEnable);
    m_iWatchCareEnable = iEnable;

    return 0;
}

//获取看守位使能
int CPtzManager::PTZ_GetWatchCareEnable(int nChannel, int *piEnable)
{
    if (NULL == piEnable)
    {
        return -1;
    }

    *piEnable = m_iWatchCareEnable;
    return 0;
}

//设置回归看守位时间
int CPtzManager::PTZ_SetWatchCareReTime(int nChannel, int iTimes)
{
    if (nChannel < 0 || nChannel >= MAX_PTZ_CHANNEL_NUM)
    {
        PTZ_ERROR("nChannel<=%d> not valid, max = %d\n", nChannel, MAX_PTZ_CHANNEL_NUM);
        return -1;
    }

    char szSectionName[32] = {0};
    CConfig *pConfig = CConfig::GetInstance();

    if (iTimes < 10 || iTimes > 30)
    {
        return -1;
    }

    if (iTimes == m_iWatchCareReTime)
    {
        return 0;
    }

    snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
    pConfig->SetValue(szSectionName, "WatchCareReTime", iTimes);
    m_iWatchCareReTime = iTimes;
    return 0;
}

int CPtzManager::PTZ_GetWatchCareReTime(int nChannel, int *piTime)
{
    if (NULL == piTime)
    {
        return -1;
    }

    *piTime = m_iWatchCareReTime;
    return 0;
}

int CPtzManager::PTZ_GetWatchCareType(int nChannel, int *nType)
{
    if (NULL == nType)
    {
        return -1;
    }

    *nType = m_iWatchCareType;
    return 0;
}

//校准（可由客户端配置）
int CPtzManager::PTZ_MotoPosCalibration(int nCh)
{
	// 水平位置
	int nHorizontalAngle = 0;
	// 垂直位置
	int nVerticalAngle = 0;

	char szSectionName[20] = {0};
	
	if (!CheckChannel(nCh, m_nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	CConfig *pConfig = CConfig::GetInstance();

    PTZ_StopMotorCruise(0);
	ZWSys_SetCalibrationStatue(1);	

	snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", nCh);
	nHorizontalAngle = pConfig->GetValue(szSectionName, "HorizontalAngle", 0);
	nVerticalAngle = pConfig->GetValue(szSectionName, "VerticalAngle", 0);

	m_iCalibrationStartFlag = 1;
	
	MOTO_SelfCheckBack(nHorizontalAngle, nVerticalAngle);
	
	return 0;
}


int CPtzManager::PTZ_IsCalibrationStart()
{
	return m_iCalibrationStartFlag;
}


int CPtzManager::PTZ_SetCalibrationReset()
{
	m_iCalibrationStartFlag = 0;
	return 0;
}


//判断当前位置和预置位是否一致，来判断是否自检完成（在自检后使用有效）
//自检+移动到原始位置的阶段 0 - 自检阶段  1 - 移动到初始位置中 2 - 完成操作 
int CPtzManager::PTZ_GetSelfCheckState(int *Status)
{
	if(NULL == Status)
	{
		return 0;
	}

	MOTO_GetSelfCheckState(Status);

	return 0;
}

int CPtzManager::PTZ_NormalCtrlToPreset(int nChannel)
{
	int nRet = 0;
	
	nRet = PTZ_NormalCtrlToPos(nChannel);

	return nRet;
}

//到初始位置
int CPtzManager::PTZ_NormalCtrlToPos(int nChannel)
{
	// 水平位置
	int nHorizontalAngle;
	// 垂直位置
	int nVerticalAngle;

	CConfig *pConfig = CConfig::GetInstance();

	char szSectionName[20] = {0};

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
	nHorizontalAngle = pConfig->GetValue(szSectionName, "HorizontalAngle", 0);
	nVerticalAngle = pConfig->GetValue(szSectionName, "VerticalAngle", 0);
	
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	if (0 == IsSupportBuiltInMotor())
	{
		return 0;
	}

	MOTO_ToPos(nHorizontalAngle, nVerticalAngle);
#endif

    PTZ_WARN("NormalCtrlToPos [%d-%d]\n", nHorizontalAngle, nVerticalAngle);
    return 0;
}

//跟踪用
int CPtzManager::PTZ_PreNormalCtrlToPosExt(int nChannel, int nHPos, int nVPos)
{
	int needMove = 0;
	int BoardInfo = 0;
	char boardIDSection[24] = {0};
	int MirrorMode = 0;

	//获取新的配置文件读取实例
	CConfig *pCfg = CConfig::GetInstance();
	ZWSys_GetBoardInfo(&BoardInfo);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);
	
	if (NULL != pCfg)
	{
		//0-正常 1-水平镜像 2-垂直翻转 3-中心翻转
		MirrorMode = pCfg->GetValue(boardIDSection, "VideoOrientation", 0);
		if ((0 == m_aCPtzDev[nChannel]->GetVerticalReverse(nChannel)
				&& (2 == MirrorMode || 3 == MirrorMode))
			|| (1 == m_aCPtzDev[nChannel]->GetVerticalReverse(nChannel)
				&& (0 == MirrorMode || 1 == MirrorMode)))
		{
			nVPos = -nVPos;
		}
		
		if ((0 == m_aCPtzDev[nChannel]->GetHorizontalReverse(nChannel)
				&& (1 == MirrorMode || 3 == MirrorMode))
			|| (1 == m_aCPtzDev[nChannel]->GetHorizontalReverse(nChannel)
				&& (0 == MirrorMode || 2 == MirrorMode)))
		{
			nHPos = -nHPos;
		}
		
	#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
		if (0 == IsSupportBuiltInMotor())
		{
			return 0;
		}

		//垂直电机往上是减小
		nVPos = -nVPos;

		if (0 == m_bSupportVertical)//不支持垂直跟踪
		{
			nVPos = 0;
		}
		
		needMove = MOTO_PreToPosExt(nHPos, nVPos);

	#endif

	}
	
	return needMove;
}

int CPtzManager::PTZ_NormalCtrlToPosExt(int nChannel, int nHPos, int nVPos)
{
	int BoardInfo = 0;
	char boardIDSection[24] = {0};
	int MirrorMode = 0;

	//获取新的配置文件读取实例
	CConfig *pCfg = CConfig::GetInstance();

	//根据版型来修改默认配置参数
	//目前代码图像方向是保存在
	//版型相关这个段里面的
	ZWSys_GetBoardInfo(&BoardInfo);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);
	
	if (NULL != pCfg)
	{
		//0-正常 1-水平镜像 2-垂直翻转 3-中心翻转
		MirrorMode = pCfg->GetValue(boardIDSection, "VideoOrientation", 0);
		if ((0 == m_aCPtzDev[nChannel]->GetVerticalReverse(nChannel)
				&& (2 == MirrorMode || 3 == MirrorMode))
			|| (1 == m_aCPtzDev[nChannel]->GetVerticalReverse(nChannel)
				&& (0 == MirrorMode || 1 == MirrorMode)))
		{
			nVPos = -nVPos;
		}
		
		if ((0 == m_aCPtzDev[nChannel]->GetHorizontalReverse(nChannel)
				&& (1 == MirrorMode || 3 == MirrorMode))
			|| (1 == m_aCPtzDev[nChannel]->GetHorizontalReverse(nChannel)
				&& (0 == MirrorMode || 2 == MirrorMode)))
		{
			nHPos = -nHPos;
		}
		
	#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
		if (0 == IsSupportBuiltInMotor())
		{
			return 0;
		}

		//垂直电机往上是减小
		nVPos = -nVPos;

		if (0 == m_bSupportVertical)//不支持垂直跟踪
		{
			nVPos = 0;
		}

		if (0 == m_bSupportVertical)
		{
			MOTO_ToPos_Product(nHPos, 0);
		}
		else
		{
			MOTO_ToPos_Product(nHPos, nVPos);
		}
	#endif

	}
	
	return 0;
}

int CPtzManager::PTZ_SetTrackSpeed(int speedH, int speedV, int nChannel)
{
	m_pAdjuster->SetTrackSpeed(speedH, speedV, nChannel);
	return 0;
}

int CPtzManager::PTZ_ReturnToOriginalSpeed(int nChannel)
{
	m_pAdjuster->ReturnToOriginalSpeed(nChannel);
	return 0;
}

int CPtzManager::IsSupportBuiltInMotor()
{
    //获取获取电机能力 false-PTZ功能无效
    if (0 == m_bSupportMotor)
    {
        return 0;
    }

	return 1;
}

int CPtzManager::IsSupportTrackVertical()
{
    return m_bSupportVertical;
}

int CPtzManager::IsSuportCustomSpeed()
{
	return m_bSupportCustomSpeed;
}

int CPtzManager::PTZ_GetPTZCtrlStatue(int *nPTZCtrlStatue)
{
	if(NULL == nPTZCtrlStatue)
	{
		return -1;	
	}

	*nPTZCtrlStatue = m_iPTZCtrlStatue;

	return 0;	
}

int CPtzManager::PTZ_SetPTZCtrlStatue(int nPTZCtrlStatue)
{
	if((nPTZCtrlStatue < 0) || (nPTZCtrlStatue > TIME_AFTER_NORMAL_CTRL))
	{
		return -1;
	}

	m_iPTZCtrlStatue = nPTZCtrlStatue;
	return 0;	
}
#endif

//##ModelId=49B472B10186
int CPtzManager::PTZ_NormalCtrl(unsigned long dwPtzCode, unsigned long dwSpeed, int nChannel)
{
    PTZ_DBG("NormalCtrl [%lu] dwSpeed[%lu] nChannel[%d]\n", dwPtzCode, dwSpeed, nChannel);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    if (0 == IsSuportPtz())
    {
        return 0;
    }

    //休眠状态下，则不让转动云台
    if (m_bDorEnable)
    {
        return 0;
    }
	
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(0, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aProtocolLib[0])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}
	if (NULL == m_aCPtzDev[0])
	{
		PTZ_ERROR("Memory Fault");
		return -1;
	}
    //当控制PTZ的时候，人形跟踪停两秒
    PTZ_SetPTZCtrlStatue(TIME_AFTER_NORMAL_CTRL);

	char szStrTmp[48] = {0};
	int BoardInfo = 0;
	char boardIDSection[24] = {0};
	int MirrorMode = 0;
	int ret = 0;

	//当前是内置云台
	m_aCPtzDev[0]->GetProtocol(szStrTmp, sizeof(szStrTmp));
	if (0 == strncmp(szStrTmp, "Gpio", sizeof(szStrTmp)) || 0 == strncmp(szStrTmp, "Gpio_Pelcod", sizeof(szStrTmp))) 
	{
		if (0 == m_bSupportMotor)
		{
			PTZ_ERROR("not support built-in motor");
			return 0;
		}
		if(nChannel == 0)
        //停止巡航
        PTZ_StopMotorCruise(0);
	}

	//获取新的配置文件读取实例
	CConfig *pCfg = CConfig::GetInstance();

	//根据版型来修改默认配置参数
	//目前代码图像方向是保存在
	//版型相关这个段里面的
	ZWSys_GetBoardInfo(&BoardInfo);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);

	//图像方向:0-正常 1-水平镜像 2-垂直翻转 3-中心翻转
	MirrorMode = pCfg->GetValue(boardIDSection, "VideoOrientation", 0);
	
	memset(szStrTmp, 0, sizeof(szStrTmp));
	m_aCPtzDev[0]->GetProtocol(szStrTmp, sizeof(szStrTmp));
	//当前是内置云台
	if (0 == strncmp(szStrTmp, "Gpio", sizeof(szStrTmp))
		|| 0 == strncmp(szStrTmp, "Gpio_Pelcod", sizeof(szStrTmp))) 
	{
		//速度还原
		PTZ_ReturnToOriginalSpeed(0);
	}

	//结合电机反向及图像方向配置判断
	//是否需要将云台控制方向进行反转
	//MirrorMode:0-正常 1-水平镜像 2-垂直翻转 3-中心翻转
	if ((0 == m_aCPtzDev[0]->GetVerticalReverse(nChannel) && (2 == MirrorMode || 3 == MirrorMode))
		|| (1 == m_aCPtzDev[0]->GetVerticalReverse(nChannel) && (0 == MirrorMode || 1 == MirrorMode)))
	{
		if (TTY_PTZ_UP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_DOWN;
		}
		else if (TTY_PTZ_DOWN == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_UP;
		}
		else if (TTY_PTZ_UP_STOP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_DOWN_STOP;
		}
		else if (TTY_PTZ_DOWN_STOP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_UP_STOP;
		}
		else if (TTY_PTZ_UP_ONESTEP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_DOWN_ONESTEP;
		}
		else if (TTY_PTZ_DOWN_ONESTEP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_UP_ONESTEP;
		}
	}

	if ((0 == m_aCPtzDev[0]->GetHorizontalReverse(nChannel)
			&& (1 == MirrorMode || 3 == MirrorMode))
		|| (1 == m_aCPtzDev[0]->GetHorizontalReverse(nChannel)
			&& (0 == MirrorMode || 2 == MirrorMode)))
	{
		if (TTY_PTZ_LEFT == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_RIGHT;
		}
		else if (TTY_PTZ_RIGHT == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_LEFT;
		}
		else if (TTY_PTZ_LEFT_STOP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_RIGHT_STOP;
		}
		else if (TTY_PTZ_RIGHT_STOP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_LEFT_STOP;
		}
		else if (TTY_PTZ_LEFT_ONESTEP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_RIGHT_ONESTEP;
		}
		else if (TTY_PTZ_RIGHT_ONESTEP == dwPtzCode)
		{
			dwPtzCode = TTY_PTZ_LEFT_ONESTEP;
		}
	}
		
#endif	

#if 0
	char acKeyName[50];
	int bEnable;
	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ"_%02d", 0);
	PTZRESET_GETCONFIG(acKeyName, bEnable);

	if(!bEnable)
	{
		PTZ_ERROR("PTZ is disable!\n");
		return -1;
	}
#endif
	DEAL_SPEED_0(dwSpeed);
	StopSoftwareCruise(0);

	//清空临时巡航线路
	m_aCPtzDev[0]->ClearTempCruise();

	STOP_AUTO(0, m_aCPtzDev[0]->GetAddress());

	m_u16PtzStatus[0][0] = PTZ_STATE_NORMAL;
	
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	void *hChannelHandle = ZWSys_EncChannelOpen(0);
	if (hChannelHandle == NULL)
	{
		PTZ_ERROR("ChannelOpen fail, !!!!!!!!!\n");
    }

	//多云台适配左右电机交换
	if(m_bSupportSwap == 1)
	{
#if (SYSTEM_SUPPORT_CHANNEL == 3)
		if (nChannel == 1) nChannel = 2;
		else if (nChannel == 2) nChannel = 1;

#elif (SYSTEM_SUPPORT_CHANNEL == 4)
		if (nChannel == 2) nChannel = 3;
		else if (nChannel == 3) nChannel = 2;
#endif
	}
	
	if((TTY_PTZ_UP_ONESTEP == dwPtzCode) || (TTY_PTZ_DOWN_ONESTEP == dwPtzCode) 
		|| (TTY_PTZ_RIGHT_ONESTEP == dwPtzCode) || (TTY_PTZ_LEFT_ONESTEP == dwPtzCode))
	{
		if((m_iCustomOneStepSpeedV <= 0))
		{
			m_iCustomOneStepSpeedV = 1;
		}

		if((m_iCustomOneStepSpeedH <= 0))
		{
			m_iCustomOneStepSpeedH = 1;
		}
		
		if((TTY_PTZ_UP_ONESTEP == dwPtzCode) || (TTY_PTZ_DOWN_ONESTEP == dwPtzCode))
		{
			ret = m_aProtocolLib[0]->objProtocol.PtzNormalCtrl(nChannel, m_aCPtzDev[0]->GetAddress(), dwPtzCode, m_iCustomOneStepSpeedH);
		}
		else
		{
			ret = m_aProtocolLib[0]->objProtocol.PtzNormalCtrl(nChannel, m_aCPtzDev[0]->GetAddress(), dwPtzCode, m_iCustomOneStepSpeedV);
		}
	}
	else
	{
		ret = m_aProtocolLib[0]->objProtocol.PtzNormalCtrl(nChannel, m_aCPtzDev[0]->GetAddress(), dwPtzCode, dwSpeed);
	}

	if ((TTY_PTZ_UP == dwPtzCode) || (TTY_PTZ_DOWN == dwPtzCode)
		|| (TTY_PTZ_LEFT == dwPtzCode) || (TTY_PTZ_RIGHT == dwPtzCode)
		|| (TTY_PTZ_LEFTUP == dwPtzCode) || (TTY_PTZ_LEFTDOWN == dwPtzCode)
		|| (TTY_PTZ_RIGHTUP == dwPtzCode) || (TTY_PTZ_RIGHTDOWN == dwPtzCode))
	{
		if(nChannel == 0)
			ZWSys_UpdatePtzStatus(hChannelHandle, ePtzStatus_Moving);
	}
	else if((TTY_PTZ_UP_STOP == dwPtzCode) || (TTY_PTZ_DOWN_STOP == dwPtzCode)
		|| (TTY_PTZ_LEFT_STOP == dwPtzCode) || (TTY_PTZ_RIGHT_STOP == dwPtzCode)
		|| (TTY_PTZ_LEFTUP_STOP == dwPtzCode) || (TTY_PTZ_LEFTDOWN_STOP == dwPtzCode)
		|| (TTY_PTZ_RIGHTUP_STOP == dwPtzCode) || (TTY_PTZ_RIGHTDOWN_STOP == dwPtzCode)
		|| (TTY_PTZ_STOP == dwPtzCode))
    {
    	if(nChannel == 0)
        	ZWSys_UpdatePtzStatus(hChannelHandle, ePtzStatus_Stop);
    }
#else
	int ret = m_aProtocolLib[0]->objProtocol.PtzNormalCtrl(nChannel, m_aCPtzDev[0]->GetAddress(), dwPtzCode, dwSpeed);
#endif

    PTZ_INFO("my add: PtzNormalCtrl return %d\n", ret);
	RESET_TIMER((DLL_OUTPUT_OK == ret), 0);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	//若无看守位，上下左右的位置设为初始位置(临时看守位)
	if((TTY_PTZ_UP_STOP == dwPtzCode) || (TTY_PTZ_DOWN_STOP == dwPtzCode)
		|| (TTY_PTZ_LEFT_STOP == dwPtzCode) || (TTY_PTZ_RIGHT_STOP == dwPtzCode)
		|| (TTY_PTZ_LEFTUP_STOP == dwPtzCode) || (TTY_PTZ_LEFTDOWN_STOP == dwPtzCode)
		|| (TTY_PTZ_RIGHTUP_STOP == dwPtzCode) || (TTY_PTZ_RIGHTDOWN_STOP == dwPtzCode)
		|| (TTY_PTZ_STOP == dwPtzCode) || (TTY_PTZ_UP_ONESTEP == dwPtzCode)
		|| (TTY_PTZ_DOWN_ONESTEP == dwPtzCode) || (TTY_PTZ_RIGHT_ONESTEP == dwPtzCode)
		|| (TTY_PTZ_LEFT_ONESTEP == dwPtzCode) || (TTY_PTZ_UP == dwPtzCode)
		|| (TTY_PTZ_DOWN == dwPtzCode) || (TTY_PTZ_LEFT == dwPtzCode)
		|| (TTY_PTZ_RIGHT == dwPtzCode))
	{
		if (m_aProtocolLib[0]->objProtocol.HasFunction(PTZ_FLAG_AUXELETRONIC))
		{
			if((TTY_PTZ_RIGHT_ONESTEP == dwPtzCode)
				|| (TTY_PTZ_LEFT_ONESTEP == dwPtzCode))
			{
				usleep(25*1000*m_iCustomOneStepSpeedV); //匹配电机的速度，避免配置没有保存
			}
			else if((TTY_PTZ_UP_ONESTEP == dwPtzCode)
				|| (TTY_PTZ_DOWN_ONESTEP == dwPtzCode))
			{
				usleep(25*1000*m_iCustomOneStepSpeedH); //匹配电机的速度，避免配置没有保存
			}
			else
			{
				usleep(10*1000); // 停止10ms，获取当前位置
			}

			if ((m_iWatchCareType == 0 && m_iNormalCtrlPreset < 0) || 
                (m_iWatchCareType && m_iWatchCareEnable == 0))  //未设置看护位，保存下临时看守位
			{
				int nHorizontalAngle = 0;
				int nVerticalAngle = 0;
				m_aProtocolLib[0]->objProtocol.GetEletronicValue(nChannel, m_aCPtzDev[0]->GetAddress(), eStateCoordinate, &nHorizontalAngle, &nVerticalAngle);
				m_aCPtzDev[0]->SetHorizontalAngle(nHorizontalAngle);
				m_aCPtzDev[0]->SetVerticalAngle(nVerticalAngle);
				PTZ_DBG("[CPtzManager::PTZ_NormalCtrl] nHorizontalAngle=%d, nVerticalAngle=%d\n", nHorizontalAngle, nVerticalAngle);
			}

			if ((m_iWatchCareType == 0 && m_iNormalCtrlPreset >= 0) ||
                 (m_iWatchCareType && m_iWatchCareEnable))
			{
				if ((TTY_PTZ_UP == dwPtzCode) || (TTY_PTZ_DOWN == dwPtzCode)
					|| (TTY_PTZ_LEFT == dwPtzCode) || (TTY_PTZ_RIGHT == dwPtzCode)
					|| (TTY_PTZ_LEFTUP == dwPtzCode) || (TTY_PTZ_LEFTDOWN == dwPtzCode)
					|| (TTY_PTZ_RIGHTUP == dwPtzCode) || (TTY_PTZ_RIGHTDOWN == dwPtzCode)
					|| (TTY_PTZ_UP_ONESTEP == dwPtzCode) || (TTY_PTZ_DOWN_ONESTEP == dwPtzCode)
					|| (TTY_PTZ_RIGHT_ONESTEP == dwPtzCode) || (TTY_PTZ_LEFT_ONESTEP == dwPtzCode))
				{
					struct timeval NowTime;
					gettimeofday(&NowTime, NULL);
					m_nManualMoveTimeBegin = NowTime.tv_sec;
					m_bManualMovePtzFlag = 1;
				}
			}
		}
	}
#endif

	// TODO: Add your specialized code here.	

	return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
}

//##ModelId=49B472BB01A5
int CPtzManager::PTZ_PresetCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed,
								unsigned long dwDwellTime, int nChannel)
{
	if (0 == IsSuportPtz())
	{
		return 0;
	}

	//休眠状态下，则不让转动云台
	if (m_bDorEnable)
	{
		return 0;
	}

	char szSectionName[20] = {0};
	CConfig *pConfig = CConfig::GetInstance();
    
	int nChannelCount = m_nChannelCount;
	int bEnable = 0;
	char acKeyName[50] = {0};
	
	int ret = 0;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault");
		return -1;
	}

	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ"_%02d", nChannel);
	PTZRESET_GETCONFIG(acKeyName, bEnable);
	if(!bEnable)
	{
		PTZ_ERROR("PTZ is disable!\n");
		return -1;
	}
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    PTZ_StopMotorCruise(nChannel);
#endif

	DEAL_SPEED_0(dwSpeed);
	StopSoftwareCruise(nChannel);
	if (TTY_PTZ_PRESETCALL == dwPtzCode)
	{
		CHECK_PRESET_PARAM(nChannel, dwPresetIndex);

		STOP_AUTO(nChannel, m_aCPtzDev[nChannel]->GetAddress());
		m_u16PtzStatus[nChannel][0] = PTZ_STATE_PRESET;
		m_u16PtzStatus[nChannel][1] = dwPresetIndex;
	}
	else
	{
		STOP_AUTO(nChannel, m_aCPtzDev[nChannel]->GetAddress());
	}
	
	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETCALL:
		case TTY_PTZ_PRESETSET:
		case TTY_PTZ_PRESETMOVE:
			{
				CheckPreset(nChannel, dwPresetIndex);
				m_aCPtzDev[nChannel]->ClearTempCruise();
		#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
				if (TTY_PTZ_PRESETMOVE == dwPtzCode)
				{
				    if ((unsigned int)m_iNormalCtrlPreset == dwPresetIndex && m_iWatchCareType == 0)
                    {
                        m_iNormalCtrlPreset = -1;
                        snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
                        pConfig->SetValue(szSectionName, "NormalCtrlPreset", m_iNormalCtrlPreset);
                    }
				}
                else if (TTY_PTZ_PRESETCALL == dwPtzCode)
                {
                    struct timeval NowTime;
                    gettimeofday(&NowTime, NULL);
                    m_nManualMoveTimeBegin = NowTime.tv_sec;
                    m_bManualMovePtzFlag = 1;
                }
		#endif
				break;
			}
		case TTY_PTZ_ROUTESET:
			CheckCruise(nChannel, dwCruiseIndex);
			ret = m_aCPtzDev[nChannel]->SetCruise(dwCruiseIndex);
			break;
		case TTY_PTZ_ROUTEEND:
			CheckCruise(nChannel, dwCruiseIndex);
			PTZ_DBG("Set Route %ld End\n", dwCruiseIndex);
			pthread_mutex_lock(&m_mtxSoftwareCruise[nChannel]);
			ret = m_aCPtzDev[nChannel]->SaveCruise(dwCruiseIndex);
			pthread_mutex_unlock(&m_mtxSoftwareCruise[nChannel]);

			if (ret != -1)
			{
				ret =
					m_aProtocolLib[nChannel]->objProtocol.PtzPresetCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), dwPtzCode, dwPresetIndex,
																		dwCruiseIndex, dwSpeed, dwDwellTime);
				SaveCruise();
				return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
			}
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			CheckPreset(nChannel, dwPresetIndex);
			CheckCruise(nChannel, dwCruiseIndex);
			if (0 == dwDwellTime)
			{
				dwDwellTime = 1;
			}
			ret = m_aCPtzDev[nChannel]->AddCruisePoint(dwCruiseIndex, dwPresetIndex, dwSpeed, dwDwellTime);
			break;
		case TTY_PTZ_ROUTEREMOVE:
			CheckCruise(nChannel, dwCruiseIndex);
			ret =
				m_aProtocolLib[nChannel]->objProtocol.PtzPresetCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), dwPtzCode, dwPresetIndex,
																	dwCruiseIndex, dwSpeed, dwDwellTime);
			m_aCPtzDev[nChannel]->ClearCruise(dwCruiseIndex);
			SaveCruise();
			RESET_TIMER((DLL_OUTPUT_OK == ret), nChannel);
			return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
			break;
		case TTY_PTZ_ROUTEREMOVEPOINT:
			CheckPreset(nChannel, dwPresetIndex);
			CheckCruise(nChannel, dwCruiseIndex);
			m_aCPtzDev[nChannel]->ClearTempCruise();
			break;
		case TTY_PTZ_TRACKSET:
		case TTY_PTZ_TRACKEND:
			CheckTrack(nChannel, dwCruiseIndex);
			m_aCPtzDev[nChannel]->ClearTempCruise();
			break;
		case TTY_PTZ_AUTOSET:
		case TTY_PTZ_AUTOEND:
			CheckAuto(nChannel, dwCruiseIndex);
			m_aCPtzDev[nChannel]->ClearTempCruise();
			break;
		case TTY_PTZ_WATCHCAREGOTO:
            if (PTZ_GetManualMovePtzFlag())
            {
                PTZ_NormalCtrlToPreset(nChannel);
                PTZ_ClearManualMovePtzFlag();
            }
            return 0;
		default:
			m_aCPtzDev[nChannel]->ClearTempCruise();
			break;
	}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	// 避免NVR让设备巡航过程中，触发了跟踪导致再次巡航时速度变慢的问题
	// 底层支持速度为 [0,9]
	if (dwSpeed < 0)
	{
		MOTO_SetSpeed(0, 0,nChannel);
	}
	else if (dwSpeed > 9)
	{
		MOTO_SetSpeed(9, 9,nChannel);
	}
	else
	{
		MOTO_SetSpeed(dwSpeed, dwSpeed,nChannel);
	}
#endif

	ret = m_aProtocolLib[nChannel]->objProtocol.PtzPresetCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), dwPtzCode, dwPresetIndex, dwCruiseIndex,
															dwSpeed, dwDwellTime);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    if (TTY_PTZ_PRESETCALL == dwPtzCode && DLL_OUTPUT_OK == ret)
    {
        if ((m_iWatchCareType == 0 && m_iNormalCtrlPreset < 0) || 
            (m_iWatchCareType && m_iWatchCareEnable == 0))  //未设置看护位，保存下临时看守位
        {
            m_aCPtzDev[nChannel]->SavePresetAsNewAngle(dwPresetIndex);
        }
    }
#endif

	RESET_TIMER((DLL_OUTPUT_OK == ret), nChannel);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	// 因为巡航，既有call，也有set。
	PTZ_ControlFuncEx(dwPtzCode, dwPresetIndex, nChannel);
#else
	if (TTY_PTZ_PRESETCALL == dwPtzCode)
	{
		PTZ_ControlFuncEx(dwPtzCode, dwPresetIndex, nChannel);
	}
#endif

	return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
}

//!\brief 预置点控制其他功能，比如灯板、AI智能目标框、语音联动等
//!\param dwPresetIndex 预置点号,实际值是减了1的
int CPtzManager::PTZ_ControlFuncEx(unsigned long dwPtzCode, unsigned long dwPresetIndex, int nChannel)
{
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	int i = 0;
	int nHorizontalAngle = 0;
	int nVerticalAngle = 0;
	char szStrTmp[48] = {0};
	char szSectionName[20] = {0};
	CConfig *pConfig = CConfig::GetInstance();

	// 巡航的code有点特殊，其他预置点功能暂时只能是通过call
	if ((TTY_PTZ_PRESETSET == dwPtzCode) && (ePtzMotorCruise != dwPresetIndex))
	{
		return 0;
	}

	m_aCPtzDev[nChannel]->GetProtocol(szStrTmp, sizeof(szStrTmp));

	/*当前是内置云台*/
	if (0 == strncmp(szStrTmp, "Gpio", sizeof(szStrTmp)) || 0 == strncmp(szStrTmp, "Gpio_Pelcod", sizeof(szStrTmp))) 
	{
		if (m_bSupportMotor)
		{
			//巡航
			if (ePtzMotorCruise == dwPresetIndex)
			{
				if (TTY_PTZ_PRESETSET == dwPtzCode)
				{
                    PTZ_StopMotorCruise(nChannel);
					return 0;
				}

				if (TTY_PTZ_PRESETCALL == dwPtzCode)
				{
                    PTZ_StartMotorCruise(nChannel);
					return 0;
				}
			}

			//看守位：调用78--跟踪停止时60S后回看守位  ;调用79--跟踪停止时30S后回看守位
			//并将当前位置设置为看守位
			if ((ePtzMotorToWatch60S == dwPresetIndex || ePtzMotorToWatch30S ==dwPresetIndex) && (m_iWatchCareType == 0))
			{
				m_aProtocolLib[nChannel]->objProtocol.GetEletronicValue(nChannel, m_aCPtzDev[nChannel]->GetAddress(), eStateCoordinate, &nHorizontalAngle, &nVerticalAngle);
				m_aCPtzDev[nChannel]->SetHorizontalAngle(nHorizontalAngle);
				m_aCPtzDev[nChannel]->SetVerticalAngle(nVerticalAngle);
				if (ePtzMotorToWatch60S == dwPresetIndex)
				{
					m_iStayTimeSec = 60;
				}
				else if (ePtzMotorToWatch30S == dwPresetIndex)
				{
					m_iStayTimeSec = 30;
				}

				m_iNormalCtrlPreset = 0;
				snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
				pConfig->SetValue(szSectionName, "NormalCtrlPreset", m_iNormalCtrlPreset);

				pConfig->SetValue("HumanShapeCfg", "TrackStayTimeSec", m_iStayTimeSec);
				pConfig->SaveFile();
				return 0;
			}

            //取消看守
			if ((ePtzMotorRemovePreset == dwPresetIndex) && (m_iWatchCareType == 0))
			{
				for (i = 0;i < 15; i++)//暂只支持16个
				{
					MOTO_RemovePreset(i);
					m_iNormalCtrlPreset = -1;
				}
				m_iStayTimeSec = 10;

				snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
				pConfig->SetValue(szSectionName, "NormalCtrlPreset", m_iNormalCtrlPreset);

				pConfig->SetValue("HumanShapeCfg", "TrackStayTimeSec", m_iStayTimeSec);
				pConfig->SaveFile();
				return 0;
			}

			//自检
			if (ePtzMotorSelfCheck == dwPresetIndex)
			{
				PTZ_MotoPosCalibration(0);
				return 0;
			}
		}
	}
#endif

	return 0;
}

//##ModelId=49B472C200EA
int CPtzManager::PTZ_AutoCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex1, unsigned long dwPresetIndex2, unsigned long dwCruiseIndex,
							  unsigned long dwSpeed, unsigned long dwDwellTime, int bScan, int nChannel)
{
	int nChannelCount = m_nChannelCount;
	char szStrTmp[48] = {0};

	int ret = 0;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault");
		return -1;
	}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	m_aCPtzDev[nChannel]->GetProtocol(szStrTmp, sizeof(szStrTmp));
	//当前是内置云台
	if (0 == strncmp(szStrTmp, "Gpio", sizeof(szStrTmp)) || 0 == strncmp(szStrTmp, "Gpio_Pelcod", sizeof(szStrTmp)))	
	{
		//停止巡航
        PTZ_StopMotorCruise(0);
	}
#endif

	char acKeyName[50];
	int bEnable;
	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ"_%02d", nChannel);
	PTZRESET_GETCONFIG(acKeyName, bEnable);
	if(!bEnable)
	{
		PTZ_ERROR("PTZ is disable!\n");
		return -1;
	}

	DEAL_SPEED_0(dwSpeed);
	StopSoftwareCruise(nChannel);
	//清空临时巡航线路
	m_aCPtzDev[nChannel]->ClearTempCruise();

	unsigned long dwAddress = m_aCPtzDev[nChannel]->GetAddress();

	CHECK_AUTO_PARAM(nChannel, dwPtzCode, dwCruiseIndex, bScan);
	if (bScan)
	{
		STOP_AUTO(nChannel, dwAddress);
	}

	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:
			CheckCruise(nChannel, dwCruiseIndex);
			ret =
				m_aProtocolLib[nChannel]->objProtocol.PtzAutoCtrl(nChannel, dwAddress, TTY_PTZ_SETROUTE, dwPresetIndex1, dwPresetIndex2,
																  dwCruiseIndex, dwSpeed, dwDwellTime, bScan);
			if (bScan)
			{
				m_u16PtzStatus[nChannel][0] = PTZ_STATE_CRUISE;
			}
			break;
		case TTY_PTZ_TRACKCALL:
			CheckTrack(nChannel, dwCruiseIndex);
			ret =
				m_aProtocolLib[nChannel]->objProtocol.PtzAutoCtrl(nChannel, dwAddress, TTY_PTZ_TRACKCALL, dwPresetIndex1, dwPresetIndex2,
																  dwCruiseIndex, dwSpeed, dwDwellTime, bScan);
			if (bScan)
			{
				m_u16PtzStatus[nChannel][0] = PTZ_STATE_TRACK;
			}
			break;
		case TTY_PTZ_AUTO:
			CheckAuto(nChannel, dwCruiseIndex);
			ret =
				m_aProtocolLib[nChannel]->objProtocol.PtzAutoCtrl(nChannel, dwAddress, TTY_PTZ_AUTO, dwPresetIndex1, dwPresetIndex2, dwCruiseIndex,
																  dwSpeed, dwDwellTime, bScan);
			if (bScan)
			{
				m_u16PtzStatus[nChannel][0] = PTZ_STATE_AUTOSCAN;
			}
			break;
		default:
			PTZ_INFO("UnKnown Command 0x%02X\n", (unsigned char) dwPtzCode);
			return -1;
			break;
	}
	m_u16PtzStatus[nChannel][1] = dwCruiseIndex;
	if (!bScan)
	{
		m_u16PtzStatus[nChannel][0] = PTZ_STATE_STOP;
	}

	RESET_TIMER((DLL_OUTPUT_OK == ret), nChannel);
	// TODO: Add your specialized code here.
	return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
}

//##ModelId=49B472CE036B
int CPtzManager::PTZ_AuxCtrl(unsigned long dwPtzCode, int bOn, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault");
		return -1;
	}

	char acKeyName[50];
	int bEnable;
	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ"_%02d", nChannel);
	PTZRESET_GETCONFIG(acKeyName, bEnable);
	if(!bEnable)
	{
		PTZ_ERROR("PTZ is disable!\n");
		return -1;
	}
	int ret = m_aProtocolLib[nChannel]->objProtocol.PtzAuxCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), dwPtzCode, bOn);

	RESET_TIMER((DLL_OUTPUT_OK == ret), nChannel);
	// TODO: Add your specialized code here.
	return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
}

//##ModelId=49B472DC02CE
int CPtzManager::PTZ_MenuCtrl(unsigned long dwPtzCode, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	if (!CheckChannel(nChannel, nChannelCount))
	{
		return IDVR_ERROR_CHANNELNO;
	}

	if (NULL == m_aProtocolLib[nChannel])
	{
		return IDVR_PTZ_ERROR_PROTOCOL_NO_EXIST;
	}

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault");
		return -1;
	}

	char acKeyName[50];
	int bEnable;
	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ"_%02d", nChannel);
	PTZRESET_GETCONFIG(acKeyName, bEnable);
	if(!bEnable)
	{
		PTZ_ERROR("PTZ is disable!\n");
		return -1;
	}

	StopSoftwareCruise(nChannel);
	int ret = m_aProtocolLib[nChannel]->objProtocol.PtzMenuCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), dwPtzCode);

	RESET_TIMER((DLL_OUTPUT_OK == ret), nChannel);

	// TODO: Add your specialized code here.
	return (int) ((DLL_OUTPUT_OK == ret) ? 0 : -1);
}

//##ModelId=49BB2D3B0251
int CPtzManager::PTZ_HasFunction(int nPtzFlag, int nChannel)
{
	int nChannelCount = m_nChannelCount;

	int ret = 0;

	if (NULL == m_aCPtzDev[nChannel])
	{
		PTZ_ERROR("Memory Fault");
		return -1;
	}

	if (!CheckChannel(nChannel, nChannelCount))
	{
		ret = 0;
	}
	else if (NULL == m_aProtocolLib[nChannel])
	{
		ret = 0;
	}
	else
	{
		ret = m_aProtocolLib[nChannel]->objProtocol.HasFunction(nPtzFlag);
	}

	// TODO: Add your specialized code here.
	return (int) ret;
}

int CPtzManager::PTZ_IsValidProtocol(char *szFileName, char *pProtocolName, int nLength)
{
	char szProtocolName[MAX_PROTONAME_LEN + 20];

	if (0 == CheckProtocol(szFileName, szProtocolName))
	{
		//缓冲区不够，截断字符串
		int len = strlen(szProtocolName);

		if (nLength <= len)
		{
			strncpy(pProtocolName, szProtocolName, nLength - 1);
			pProtocolName[nLength - 1] = '\0';
			//返回协议名实际长度
			return len;
		}

		//一切正常
		strncpy(pProtocolName, szProtocolName, sizeof(pProtocolName) - 1);
		return 1;
	}

	return 0;
}

int CPtzManager::GetProtocolList(char pProtocol[][MAX_PROTONAME_LEN + 2], int *nLength, char *szPath, int nMax)
{
	DIR *dirCurrent;

	struct dirent *dirEntry;

	struct stat statFile;

	char szFileName[DOCNAME_MAX*2] = { 0 };

	char szProtocolName[MAX_PROTONAME_LEN + 20] = { 0 };

	*nLength = 0;

	//读目录
	if ((dirCurrent = opendir(szPath)) == NULL)
	{
		PTZ_ERROR("Open Protocol Dir %s Error\n", szPath);
		return -1;
	}
	else
	{
		PTZ_DBG("Open Protocol Dir %s Success\n", szPath);
	}

	//进入目录
	chdir(szPath);

	while ((*nLength < nMax) && (*nLength < (int) MAX_PROTOCOL_NUM) && (NULL != (dirEntry = readdir(dirCurrent))))
	{
		lstat(dirEntry->d_name, &statFile);
		if (S_ISREG(statFile.st_mode))
		{
			//检查文件名是否合法
			int ret = IsValidProtocolFileName(dirEntry->d_name, szProtocolName, MAX_PROTONAME_LEN + 20);

			//不合法
			if (0 == ret)
			{
				PTZ_INFO("Ignore File %s\n", dirEntry->d_name);
				continue;
			}

			snprintf(szFileName, sizeof(szFileName), "%s/%s", szPath, dirEntry->d_name);
			//检查是否是合法的协议
			if (!tagProtocolLib::IsValid(szFileName))
			{
				PTZ_INFO("Invalid Protocol %s\n", szFileName);
				continue;
			}

			//是卸载的协议
			if (2 == ret)
			{
				m_nDisableProtocol += 1;
			}
			//是可用协议
			else if (1 == ret)
			{
				m_nEnableProtocol += 1;
			}
			else				//运行不到此处
			{
				continue;
			}

			strncpy(pProtocol[*nLength], szProtocolName, sizeof(pProtocol[*nLength]) - 1);
			PTZ_DBG("Read Protocol: %s, nIndex = %d\n", pProtocol[*nLength], *nLength);
			*nLength += 1;
		}
	}

	closedir(dirCurrent);
	return (int) 0;
}

int CPtzManager::InitDev()
{
	int nChannelCount = m_nChannelCount;

	int i = 0;

	char szProtocol[50];

	sem_init(&m_semCruise, 0, 0);
	for (i = 0; i < nChannelCount; i++)
	{
		m_aCPtzDev[i] = new CPtzDev(i);
		if (NULL == m_aCPtzDev[i])
		{
			return IDVR_ERROR_ALLOC;
		}

		//初始化基本参数
		m_aCPtzDev[i]->Init(i);

		int nResult = pthread_create(&m_tSoftwareCruise[i], NULL, SoftwareCruise, (void *) i);

		if (0 == nResult)
		{
			sem_wait(&m_semCruise);
			pthread_detach(m_tSoftwareCruise[i]);
		}
	}
	sem_destroy(&m_semCruise);

	for (i = 0; i < nChannelCount; i++)
	{
		m_aCPtzDev[i]->GetProtocol(szProtocol, 50);
		if (0 == strlen(szProtocol))
		{
			//没有任何协议可用
			m_aProtocolLib[i] = NULL;
		}
		else
		{
			m_aProtocolLib[i] = new tagProtocolLib();
			if (NULL == m_aProtocolLib[i])
			{
				return IDVR_ERROR_ALLOC;
			}

			int ret = LoadProtocol(m_aProtocolLib[i], szProtocol);

			if (IDVR_OK == ret)
			{
				m_aProtocolLib[i]->objProtocol.SetCallback(FnSendData, FnRecvData, StartSoftwareCruise, StopSoftwareCruise);
				m_aCPtzEletronic[i] = new CPtzEletronic(i);

				if (NULL == m_aCPtzEletronic[i])
				{
					return IDVR_ERROR_ALLOC;
				}
				InitEletronicType(i);
			}
			else
			{
				delete m_aProtocolLib[i];

				m_aProtocolLib[i] = NULL;
			}
		}
	}
	return 0;
}

int CPtzManager::GetProtocolIndex(char *szProtocol, int bExternal)
{
	int i = 0;

	for (i = 0; i < m_nProtocolCount[bExternal]; i++)
	{
		if ((0 == strcmp(szProtocol, m_aszProtocol[bExternal][i]))
			|| (('_' == m_aszProtocol[bExternal][i][0]) && (0 == strcmp(m_aszProtocol[bExternal][i] + 1, szProtocol))))
		{
			return i;
		}
	}

	return -1;
}

int CPtzManager::IsValidProtocolFileName(char *szProtocolFileName, char *szProtocolName, int nLength)
{
	bool bUninstalled = false;

	//前缀必须为lib
	if (0 != strncmp(szProtocolFileName, "lib", 3))
	{
		return 0;
	}

	//后缀必须为.so
	char *postfix = strrchr(szProtocolFileName, PROTOCOL_POSTFIX[0]);

	if ((NULL == postfix) || (0 != strcmp(postfix, PROTOCOL_POSTFIX)))
	{
		return 0;
	}

	//为卸载协议,第四个字符为'_',第五个字符不能为'_'
	if (UNINSTALL_FLAG == *(szProtocolFileName + 3))
	{
		if (UNINSTALL_FLAG == *(szProtocolFileName + 4))
		{
			return 0;
		}
		bUninstalled = true;
	}

	char *protocol = (szProtocolFileName + 3);

	int nlen = postfix - protocol - (bUninstalled ? 1 : 0);	//liba.so 4-3

	if ((nlen <= 0) /*lib.so, lib_.so */  || (nlen >= (int) MAX_PROTONAME_LEN))
	{
		return 0;
	}

	//协议名中只能包含字母数字和下划线
	char *p = NULL;

	for (p = protocol; p < postfix; p++)
	{
		if (!(((*p >= 'a') && (*p <= 'z')) || ((*p >= 'A') && (*p <= 'Z')) || ((*p >= '0') && (*p <= '9')) || (*p == '_')))
		{
			return 0;
		}
	}

	strncpy(szProtocolName, protocol, nLength - 1);
	szProtocolName[postfix - protocol] = '\0';
	PTZ_DBG("szProtocolName = %s, protocol = %s\n", szProtocolName, protocol);

	return bUninstalled ? 2 /*协议名最开始带'_'标记 */ : 1 /*合法协议 */ ;
}

bool CPtzManager::IsProtocolExist(char *szProtocol)
{
	int nIndex = GetProtocolIndex(szProtocol, 0);

	if (-1 != nIndex)
	{
		if ('_' == m_aszProtocol[0][nIndex][0])
		{
			return false;
		}

		return true;
	}

	nIndex = GetProtocolIndex(szProtocol, 1);
	if (-1 != nIndex)
	{
		return true;
	}

	return false;
}

int CPtzManager::CheckProtocol(char *szFileName, char szProtocolName[MAX_PROTONAME_LEN + 20])
{
	struct stat statBuf;

	int ret = stat(szFileName, &statBuf);

	if (0 != ret)
	{
		return IDVR_ERROR_OPEN_FILE;
	}

	//非普通文件
	if (!S_ISREG(statBuf.st_mode))
	{
		return IDVR_PTZ_ERROR_PROTOCOL_INVALID_FORMAT;
	}

	//获取文件名
	char *pName = GetFileNameFromPath(szFileName);

	//判断文件名是否合法
	if (1 != IsValidProtocolFileName(pName, szProtocolName, MAX_PROTONAME_LEN + 20))
	{
		return IDVR_PTZ_ERROR_PROTOCOL_INVALID_FORMAT;
	}

	//检查是否是合法的动态链接库
	if (ProtocolLib::IsValid(szFileName))
	{
		//内部协议中已有该协议
		int nIndex = GetProtocolIndex(szProtocolName, 0);

		if (-1 != nIndex)
		{
			return IDVR_PTZ_ERROR_PROTOCOL_ALREADY_EXIST;
		}

		//外部协议中有该协议
		nIndex = GetProtocolIndex(szProtocolName, 1);
		if (-1 != nIndex)
		{
			return IDVR_PTZ_ERROR_PROTOCOL_ALREADY_EXIST;
		}

		//协议正常
		return 0;
	}
	else
	{
		return IDVR_PTZ_ERROR_PROTOCOL_INVALID_FORMAT;
	}
}

char *CPtzManager::GetFileNameFromPath(char *szPath)
{
	char *pFileName = strrchr(szPath, '/');

	if (NULL == pFileName)
	{
		return szPath;
	}

	return pFileName + 1;
}

int CPtzManager::CopyFileToUsb(char *szSrcPath, char *szDstPath, int bRemove)
{
#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
	return IDVR_ERROR_USB_OPEN_FILE;
#else
	//读取文件
	int fdSrc = open(szSrcPath, O_RDONLY);

	if (fdSrc < 0)
	{
		PTZ_ERROR("Open Src File %s Filed\n", szSrcPath);
		return IDVR_ERROR_OPEN_FILE;
	}

	PTZ_DBG("SrcPath = %s, DstPath = %s\n", szSrcPath, szDstPath);
	int fdDst = EDVR_USB_BufferTransferStart(szDstPath);

	if (fdDst < 0)
	{
		PTZ_ERROR("{USB} Start Transfer File %s Filed\n", szDstPath);
		return IDVR_ERROR_USB_OPEN_FILE;
	}

	struct stat statSrc;

	stat(szSrcPath, &statSrc);
	char szBuffer[40 * 1024];

	int nReadBytes = 0;

	int nTotalReadBytes = 0;

	while (1)
	{
		nReadBytes = read(fdSrc, szBuffer, 40 * 1024);
		if (nReadBytes <= 0)
		{
			break;
		}

		nTotalReadBytes += nReadBytes;
		int ret = EDVR_USB_BufferTransfer(fdDst, szBuffer, nReadBytes);

		if (ret == -1)
		{
			PTZ_ERROR("{USB} Buffer Transfer File %s Failed\n", szDstPath);
			close(fdSrc);
			EDVR_USB_BufferTransferStop(fdDst);
			unlink(szDstPath);
			return IDVR_ERROR_USB_WRITE_FILE;
		}
	}

	close(fdSrc);
	EDVR_USB_BufferTransferStop(fdDst);
	if ((nTotalReadBytes != statSrc.st_size) || (0 == nTotalReadBytes))
	{
		PTZ_ERROR("Real Src Size = %ld, Read = %d", statSrc.st_size, nTotalReadBytes);
		unlink(szDstPath);
		return IDVR_ERROR_READ_FILE;
	}

	if (1 == bRemove)
	{
		unlink(szSrcPath);
	}

	return 0;
#endif
}

int CPtzManager::CopyFileFromUsb(char *szSrcPath, char *szDstPath, int bRemove)
{
	int fdSrc = open(szSrcPath, O_RDONLY);

	if (-1 == fdSrc)
	{
		return IDVR_ERROR_USB_OPEN_FILE;
	}

	int fdDst = open(szDstPath, O_RDWR | O_CREAT | O_TRUNC | O_SYNC);

	if (-1 == fdDst)
	{
		return IDVR_ERROR_OPEN_FILE;
	}

	struct stat statSrc;

	stat(szSrcPath, &statSrc);
	unsigned char szBuffer[40 * 1024];

	int nReadBytes = 0;

	int nTotalReadBytes = 0;

	while (1)
	{
		nReadBytes = read(fdSrc, szBuffer, 40 * 1024);
		if (nReadBytes <= 0)
		{
			break;
		}

		nTotalReadBytes += nReadBytes;
		int ret = write(fdDst, szBuffer, nReadBytes);

		if (-1 == ret)
		{
			close(fdDst);
			close(fdSrc);
			return IDVR_ERROR_WRITE_FILE;
		}
	}

	close(fdSrc);
	close(fdDst);
	if (nTotalReadBytes != statSrc.st_size)
	{
		//读取错误
		unlink(szDstPath);
		return IDVR_ERROR_USB_READ_FILE;
	}

	if (1 == bRemove)
	{
		unlink(szSrcPath);
	}

	return 0;
}

int CPtzManager::LoadProtocol(LPProtocolLib pProtocol, char *szProtocol)
{
	char szProtocolPath[256];

	int nIndex = GetProtocolIndex(szProtocol, 0);

	if (nIndex != -1)
	{
		snprintf(szProtocolPath,sizeof(szProtocolPath), "%s/lib%s.so", m_szPath[0], szProtocol);
	}
	else
	{
		nIndex = GetProtocolIndex(szProtocol, 1);
		if (-1 == nIndex)
		{
			return -1;
		}

		snprintf(szProtocolPath,sizeof(szProtocolPath), "%s/lib%s.so", m_szPath[1], szProtocol);
	}

	PTZ_DBG("Load Protocol %s\n", szProtocolPath);
	int ret = pProtocol->Open(szProtocolPath);

	if (ret != 0)
	{
		PTZ_ERROR("Load PTZ DLL %s Error\n", szProtocol);
		return IDVR_PTZ_ERROR_PROTOCOL_INVALID_FORMAT;
	}

	return IDVR_OK;
}

int CPtzManager::FnSendRawData(unsigned char *pData, unsigned int uLength)
{
	//根据定制的波特率来确定是否发送数据
	CPtzManager *pCPtzMana = NULL;
	static int nUartBaudrateStartVal = -1;
	int nRet = 0;
	
	//获取新的配置文件读取实例
	if (-1 == nUartBaudrateStartVal)
	{
	    CConfig *pcfg = CConfig::NewInstance();	
		nRet = pcfg->LoadFileRO(CUSTOM_INFO_FILE);
		if (0 == nRet)
		{
			nUartBaudrateStartVal = pcfg->GetValue("AFDevCfg", "UartBaudrateStartVal", UART_BAUDRATE_START_VAL);
		}
		else
		{
	        nUartBaudrateStartVal = UART_BAUDRATE_START_VAL;
	    }
		CConfig::ReleaseInstance(pcfg);
	}

	//根据定制的波特率来确定是否发送数据
	pCPtzMana = PTZ_GetInstance();
	if (NULL != pCPtzMana)
	{
		if (pCPtzMana->PTZ_GetBaudrate(0) < (unsigned int)nUartBaudrateStartVal)
		{
			return 0;
		}
	}

	return FnSendData(0, (void *)pData, uLength);
}

int CPtzManager::FnSendData(unsigned int nChannel, void *pBuffer, int nLength)
{
	return m_pInstance->m_aCPtzDev[nChannel]->SendData(pBuffer, nLength);
}

int CPtzManager::FnRecvRawData(unsigned char *pData, unsigned int uLength)
{	
	return FnRecvData(0, (void *)pData, uLength);
}

int CPtzManager::FnRecvData(unsigned int nChannel, void *pBuffer, int nLength)
{
	return m_pInstance->m_aCPtzDev[nChannel]->RecvData(pBuffer, nLength);
}

int CPtzManager::StartSoftwareCruise(unsigned int nChannel, unsigned int dwCruise)
{
	//PTZ_DBG("Call Software Cruise, nChannel = %u, nCruiseIndex=%u\n", nChannel, dwCruise);
	PTZ_DBG("Call Software Cruise, nChannel = %u, nCruiseIndex=%u\n", nChannel, dwCruise);

	pthread_mutex_lock(&m_mtxSoftwareCruise[nChannel]);
	if (0 == m_pInstance->m_aCPtzDev[nChannel]->GetCruise(dwCruise)->nPointCount)
	{
		pthread_mutex_unlock(&m_mtxSoftwareCruise[nChannel]);
		return 0;
	}

	if ((int) dwCruise == m_pInstance->m_aCPtzDev[nChannel]->GetCurCruise())
	{
		pthread_mutex_unlock(&m_mtxSoftwareCruise[nChannel]);
		return -1;
	}
	else if (m_pInstance->m_aCPtzDev[nChannel]->GetCurCruise() >= 0)
	{
		//停止巡航
		m_pInstance->m_aCPtzDev[nChannel]->SetCurCruise(-1);
		pthread_cond_signal(&m_condSoftwareCruise[nChannel]);
	}

	m_pInstance->m_aCPtzDev[nChannel]->SetCurCruise((int) dwCruise);
	//获取速度
	int nSpeed = m_pInstance->m_aCPtzDev[nChannel]->GetSpeed();

	//获取滞留时间
	int nTime = 0;

	//获取预置点
	int nPresetId = m_pInstance->m_aCPtzDev[nChannel]->GetPresetId();

	//调用第一个预置点
	m_pInstance->m_aProtocolLib[nChannel]->objProtocol.PtzPresetCtrl(nChannel, m_pInstance->m_aCPtzDev[nChannel]->GetAddress(), TTY_PTZ_PRESETCALL,
																	 nPresetId, 0, nSpeed, nTime);

	//PTZ_INFO("Start Cruise, Channel = %d\n", nChannel);
	pthread_cond_signal(&m_condSoftwareCruise[nChannel]);
	pthread_mutex_unlock(&m_mtxSoftwareCruise[nChannel]);

	return 0;
}

int CPtzManager::StopSoftwareCruise(unsigned int nChannel)
{
	PTZ_DBG("Stop Software Cruise\n");

	pthread_mutex_lock(&m_mtxSoftwareCruise[nChannel]);
	if (-1 == m_pInstance->m_aCPtzDev[nChannel]->GetCurCruise())
	{
		//PTZ_INFO("Cruise Is Stopped, Channel = %d\n", nChannel);
		pthread_mutex_unlock(&m_mtxSoftwareCruise[nChannel]);
		return 0;
	}
	m_pInstance->m_aCPtzDev[nChannel]->SetCurCruise(-1);
	//PTZ_INFO("Stop Cruise, Channel = %d\n", nChannel);
	pthread_cond_signal(&m_condSoftwareCruise[nChannel]);
	pthread_mutex_unlock(&m_mtxSoftwareCruise[nChannel]);

	return 0;
}

void *CPtzManager::SoftwareCruise(void *arg)
{
	PRT_TID();
	BEGINRUN_THREAD;
	int nChannel = (int) arg;

	CPtzDev *pDev = m_pInstance->m_aCPtzDev[nChannel];

	sem_post(&m_pInstance->m_semCruise);

	struct timespec ts_now;

	struct timeval tv_now;

	pthread_mutex_lock(&(m_pInstance->m_mtxSoftwareCruise[nChannel]));

	int nPresetId = 0;

	int nTime = 0;

	unsigned long nSpeed = 6;

	int nRet = 0;

	while (1)
	{
		//等待开始巡航命令
		pthread_cond_wait(&(m_pInstance->m_condSoftwareCruise[nChannel]), &(m_pInstance->m_mtxSoftwareCruise[nChannel]));
		//开始失败，继续等待
		if (-1 == pDev->GetCurCruise())
		{
			//PTZ_INFO("Stopped Software Cruise, nChannel = %d\n", nChannel);
			continue;
		}
		nTime = pDev->GetDwellTime();
		while (1)
		{
			gettimeofday(&tv_now, NULL);
			ts_now.tv_sec = tv_now.tv_sec + nTime;
			ts_now.tv_nsec = tv_now.tv_usec * 1000;
			//等待超时
			nRet = pthread_cond_timedwait(&(m_pInstance->m_condSoftwareCruise[nChannel]), &(m_pInstance->m_mtxSoftwareCruise[nChannel]), &ts_now);
			//退出巡航
			if (-1 == pDev->GetCurCruise())
			{
				break;
			}
			else if (0 == nRet)
			{
				nTime = pDev->GetDwellTime();
				continue;
			}
			//获取下一个预置点
			pDev->NextPoint();
			nPresetId = pDev->GetPresetId();
			nSpeed = pDev->GetSpeed();
			nTime = pDev->GetDwellTime();
			m_pInstance->m_aProtocolLib[nChannel]->objProtocol.PtzPresetCtrl(nChannel, pDev->GetAddress(), TTY_PTZ_PRESETCALL, nPresetId, 0, nSpeed,
																			 nTime);
		}
	}
	pthread_mutex_unlock(&(m_pInstance->m_mtxSoftwareCruise[nChannel]));
	ENDRUN_THREAD;
	return NULL;
}

int CPtzManager::LoadCruise()
{
	int i = 0;

	int ret = 0;

	const int nChannelCount = m_nChannelCount;

	int fdCruise = open("/mnt/mtd/config/PTZCruise.cfg", O_CREAT | O_RDONLY);

	if (fdCruise > 0)
	{
		for (i = 0; i < nChannelCount; i++)
		{
			int j = 0;

			for (j = 0; j < MAX_CRUISE_COUNT; j++)
			{
				ret = read(fdCruise, m_aCPtzDev[i]->GetCruise(j), sizeof (Cruise));
				if (ret < (int) (sizeof (Cruise)))
				{
					if (0 != ret)
					{
						PTZ_ERROR("Load Cruise Failed, fdCruise = %d\n", fdCruise);
					}
					ret = -1;
					break;
				}
			}

			if (-1 == ret)
			{
				break;
			}
		}
	}
	else
	{
		ret = -1;
	}

	if (-1 == ret)
	{
		for (i = 0; i < nChannelCount; i++)
		{
			int j = 0;

			for (j = 0; j < MAX_CRUISE_COUNT; j++)
			{
				m_aCPtzDev[i]->ClearCruise(j);
			}
		}
	}

	close(fdCruise);
	return 0;
}

int CPtzManager::SaveCruise()
{
	int i = 0;

	int ret = 0;

	const int nChannelCount = m_nChannelCount;

	int fdCruise = open("/mnt/mtd/config/PTZCruise.cfg", O_CREAT | O_WRONLY);

	if (fdCruise < 0)
	{
		PTZ_ERROR("Save Cruise Failed, fdCruise = %d\n", fdCruise);
		return -1;
	}

	for (i = 0; i < nChannelCount; i++)
	{
		int j = 0;

		for (j = 0; j < MAX_CRUISE_COUNT; j++)
		{
			ret = write(fdCruise, m_aCPtzDev[i]->GetCruise(j), sizeof (Cruise));
			if (ret < 0)
			{
				PTZ_ERROR("Save Cruise Failed, fdCruise = %d\n", fdCruise);
				close(fdCruise);
				return -1;
			}
		}
	}
	close(fdCruise);
	return 0;
}

void CPtzManager::InitEletronicType(unsigned int nChannel)
{
	int nDevType = 0;

	if (m_aProtocolLib[nChannel]->objProtocol.HasFunction(PTZ_FLAG_AUXELETRONIC))
	{
		int nMainType = 0;

		int nSubType = 0;

		m_aProtocolLib[nChannel]->objProtocol.GetEletronicValue(nChannel, m_aCPtzDev[nChannel]->GetAddress(), eStateType, &nMainType, &nSubType);
		nDevType = ((nMainType << 16) | nSubType);
	}

	m_aCPtzEletronic[nChannel]->Init(nDevType);
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)

int CPtzManager::SetTrackStayTime(int nStayTimeSec)
{
	CConfig *pConfig = CConfig::GetInstance();

	if((nStayTimeSec < 0) || (nStayTimeSec > 600))
	{
		return -1;
	}

	if(nStayTimeSec == m_iStayTimeSec)
	{
		return 0;
	}

	m_iStayTimeSec = nStayTimeSec;

	pConfig->SetValue("HumanShapeCfg", "TrackStayTimeSec", m_iStayTimeSec);
	pConfig->SaveFile();

	return 0;
}

int CPtzManager::GetTrackStayTime(int *nStayTimeSec)
{
	if(NULL == nStayTimeSec)
	{
		return -1;
	}

	*nStayTimeSec = m_iStayTimeSec;
	return 0;
}

int CPtzManager::PTZ_IsMotoStatus(int *iStatus)
{
	MOTO_GetState(iStatus);
	return 0;
}

int CPtzManager::PTZ_GetStartStatus(int *iStatus)
{
	MOTO_GetStartState(iStatus);
	return 0;
}
int CPtzManager::PTZ_IsAdaptProtocol(int nChannel)
{
	if (NULL != m_aCPtzDev[nChannel])
	{
		char szStrTmp[48] = {0};
		m_aCPtzDev[nChannel]->GetProtocol(szStrTmp, sizeof(szStrTmp));
		if (0 == strcmp(szStrTmp, "Gpio") || 0 == strcmp(szStrTmp, "Gpio_Pelcod"))
		{
			return 1;
		}
	}

	return 0;
}

void CPtzManager::PTZ_GetIfInit(int *pIfInit)
{
	if(pIfInit != NULL)
	{
		*pIfInit = m_inited;
	}
}

void *thread_do_cruise(void *param)
{
    PRT_TID();

	int rc = 0;
    int texit = 0;
    //int Defaultspeed = 8;
    CPtzManager *pMgr = (CPtzManager *)param;
	PTZ_CRUISE_INFO *cruise_cfg = &pMgr->m_CruiseCfg;
    GO_CRUISE_NODE_INFO node = {0};

	pthread_detach(pthread_self());

    //巡航开始提高云台转动速度
    //MOTO_SetSpeed(Defaultspeed, Defaultspeed);

	cruise_cfg->cruise_status = 1;

	if (cruise_cfg->idx >= cruise_cfg->validSize)
	{
		cruise_cfg->idx = 0;
	}
    node.bStop = &(cruise_cfg->bStop);

	while (cruise_cfg->idx < cruise_cfg->validSize)
	{
        node.idx = cruise_cfg->node[cruise_cfg->idx].presetIdx;
        node.retentionTime = cruise_cfg->node[cruise_cfg->idx].nRetentionTime;

        rc = MOTO_GoCruiseNode(&node);
		if (-2 == rc)
		{
			usleep(200*1000);
            cruise_cfg->nCntTime += 200;
		}
		else if (-1 == rc)
		{
			PTZ_WARN("move time out!\n");
		}
		else if (1 == rc)
		{
			break;
		}

        // 巡航时间到，需要退出巡航
        cruise_cfg->nCntTime += node.nCntTime;
        if (cruise_cfg->nCntTime >=  (cruise_cfg->nAllTime*60*1000))
        {
            texit = 1;
            break;
        }

		if (cruise_cfg->idx >= (cruise_cfg->validSize-1))
		{
			cruise_cfg->idx = 0;
		}
		else
		{
			cruise_cfg->idx++;
		}
	}

    //如果是由于时间巡航停止，则需要保存配置
    if (cruise_cfg->bStop == 0 && texit == 1)
    {
        pthread_mutex_lock(&(pMgr->m_mtxCruise));

        void *hChannelHandle = ZWSys_EncChannelOpen(0);
        if (hChannelHandle != NULL)
        {
            ZWSys_UpdatePtzStatus(hChannelHandle, ePtzStatus_Stop);
        }

        // 更新配置
        CConfig *pcfg = CConfig::GetInstance();
        if (pcfg != NULL)
        {
            pcfg->SetValue("PTZ_Advance", "PTZ_MotorCruiseEnable", 0);
            pcfg->SaveFile();
        }
        cruise_cfg->cruise_enable = 0;

        pthread_mutex_unlock(&(pMgr->m_mtxCruise));
    }

    if ((pMgr->m_iWatchCareType == 0 && pMgr->m_iNormalCtrlPreset < 0) || 
        (pMgr->m_iWatchCareType && pMgr->m_iWatchCareEnable == 0))
    {
        /* 无看守位，巡航结束保存当前位置 */
        pMgr->m_aCPtzDev[0]->SavePresetAsNewAngle(node.idx);
    }
    else
    {
        /* 有看守位，巡航结束回到预置点 */
        pMgr->PTZ_NormalCtrlToPos(0);
    }

    //巡航结束恢复云台转速
    //Defaultspeed = pMgr->m_pAdjuster->GetSpeed(0);
    //MOTO_SetSpeed(Defaultspeed, Defaultspeed);

	cruise_cfg->tid_cruise = 0;
	cruise_cfg->cruise_status = 0;

    PTZ_INFO("The cruise thread is exited!\n");
	return NULL;
}

// 设置巡航参数，调用前务必先停止巡航
int CPtzManager::SetMotorCruise()
{
    int nRetentionTime = 0;
    int pos_h = 0;
    int pos_v = 0;
    int rc = 0;
    
    CConfig *pcfg = CConfig::GetInstance();
    if (pcfg == NULL)
    {
        return -1;
    }

    if (m_CruiseCfg.cruise_status != 0)
    {
        m_CruiseCfg.validSize = 0;
        return -1;
    }

    m_CruiseCfg.idx = 0;
    m_CruiseCfg.nCntTime = 0;
    m_CruiseCfg.validSize = 0;
    m_CruiseCfg.nAllTime = pcfg->GetValue("PTZ_Advance", "PTZ_MotorCruiseTime", 30); 
    nRetentionTime = pcfg->GetValue("PTZ_Advance", "PTZ_MotorCruiseRetentionTime", 10);

    for (int i = 0; i < 8; i++)
    {
        rc = MOTO_GetPresetPos(i, &pos_h, &pos_v);
        if (0 == rc)
        {
            m_CruiseCfg.node[m_CruiseCfg.validSize].presetIdx = i;
            m_CruiseCfg.node[m_CruiseCfg.validSize].speed = 5; // 速度暂时是无效的
            m_CruiseCfg.node[m_CruiseCfg.validSize].nRetentionTime = nRetentionTime;
            m_CruiseCfg.validSize++;
        }
    }
    return 0;
}

int CPtzManager::StartMotorCruise()
{
    int ret = 0;

	pthread_mutex_lock(&m_mtxCruise);
    if (m_CruiseCfg.cruise_enable == 1)
    {
        pthread_mutex_unlock(&m_mtxCruise);
        return 0;
    }

    // 开启巡航线程
	ret = pthread_create(&m_CruiseCfg.tid_cruise, NULL, thread_do_cruise, this);
	if (ret != 0)
    {
		PTZ_ERROR("[CPtzManager:%s] pthread_create failed (%d)\n", __FUNCTION__, ret);
        pthread_mutex_unlock(&m_mtxCruise);
        return -1;
    }

    void *hChannelHandle = ZWSys_EncChannelOpen(0);
    if (hChannelHandle != NULL)
    {
        ZWSys_UpdatePtzStatus(hChannelHandle, ePtzStatus_Moving);
    }

    // 更新配置
    CConfig *pcfg = CConfig::GetInstance();
    if (pcfg != NULL)
    {
        pcfg->SetValue("PTZ_Advance", "PTZ_MotorCruiseEnable", 1);
        pcfg->SaveFile();
    }
    m_CruiseCfg.cruise_enable = 1;

    pthread_mutex_unlock(&m_mtxCruise);

    PTZ_INFO("Cruise start successful!\n");
    return ret;
}

int CPtzManager::StopMotorCruise()
{
	int cnt = 10;

	pthread_mutex_lock(&m_mtxCruise);
    if (m_CruiseCfg.cruise_enable == 0)
    {
        pthread_mutex_unlock(&m_mtxCruise);
        return 0;
    }

	// 关闭巡航，等待关闭状态, 退出
	m_CruiseCfg.bStop = 1;
	do {
		if (0 == m_CruiseCfg.cruise_status)
		{
			m_CruiseCfg.bStop = 0;
			break;
		}
		usleep(500*1000);
	} while(cnt-- > 0);

    void *hChannelHandle = ZWSys_EncChannelOpen(0);
    if (hChannelHandle != NULL)
    {
        ZWSys_UpdatePtzStatus(hChannelHandle, ePtzStatus_Stop);
    }

    // 更新配置
    CConfig *pcfg = CConfig::GetInstance();
    if (pcfg != NULL)
    {
        pcfg->SetValue("PTZ_Advance", "PTZ_MotorCruiseEnable", 0);
        pcfg->SaveFile();
    }
    m_CruiseCfg.cruise_enable = 0;

    pthread_mutex_unlock(&m_mtxCruise);
    PTZ_INFO("Cruise stop successful!\n");
    return 0;
}


int CPtzManager::PTZ_StartMotorCruise(int nChannel)
{
    if (nChannel > MAX_PTZ_CHANNEL_NUM)
    {
        return -1;
    }

    if (NULL == m_aCPtzDev[nChannel])
    {
        return -1;
    }

    char szStrTmp[48] = {0};
	m_aCPtzDev[nChannel]->GetProtocol(szStrTmp, sizeof(szStrTmp));
	if (0 == strcmp(szStrTmp, "Gpio") || 0 == strcmp(szStrTmp, "Gpio_Pelcod"))
	{
        // 停止巡航
        if (m_CruiseCfg.cruise_status != 0)
        {
            StopMotorCruise();
        }

        // 初始化巡航配置
        SetMotorCruise();

        // 未设置云台预置点
        if (m_CruiseCfg.validSize <= 0)
        {
            return -1;
        }

        // 开始巡航
        StartMotorCruise();
    }

	return 0;
}

int CPtzManager::PTZ_StopMotorCruise(int nChannel)
{
    if (nChannel > MAX_PTZ_CHANNEL_NUM)
    {
        return -1;
    }
    
	if (NULL == m_aCPtzDev[nChannel])
	{
        return -1;
    }

    if (m_CruiseCfg.cruise_enable == 0)
    {
        return 0;
    }

    char szStrTmp[48] = {0};
	m_aCPtzDev[nChannel]->GetProtocol(szStrTmp, sizeof(szStrTmp));
	if (0 == strcmp(szStrTmp, "Gpio") || 0 == strcmp(szStrTmp, "Gpio_Pelcod"))
	{
		// 停止巡航
		StopMotorCruise();
	}

	return 0;
}

int CPtzManager::PTZ_GetCuiseStatue(int *nCruiseStatue)
{
	if (NULL == nCruiseStatue)
	{
		return -1;
	}

    *nCruiseStatue = m_CruiseCfg.cruise_status;
	return 0;
}

//获取手动控制云台标识
int CPtzManager::PTZ_GetManualMovePtzFlag()
{
	return m_bManualMovePtzFlag;
}

//清除手动控制云台标识
void CPtzManager::PTZ_ClearManualMovePtzFlag()
{
	m_bManualMovePtzFlag = 0;
}


//获取手动控制云台开始时间
long CPtzManager::PTZ_GetManualMoveTimeBegin()
{
	return m_nManualMoveTimeBegin;
}

//设置手动控制云台开始时间
void CPtzManager::PTZ_SetManualMoveTimeBegin(long nTimes)
{
	m_nManualMoveTimeBegin = nTimes;
}
#endif

int CPtzManager::IsSuportPtz()
{
    //获取获取电机能力 false-PTZ功能无效
    if (0 == m_bSupportMotor)
    {
        return 0;
    }

	return 1;
}

int CPtzManager::PTZ_PrivacyMaskPreset(int nChannel, int AwakeMode)
{
	if (nChannel < 0 || nChannel >= MAX_PTZ_CHANNEL_NUM)
	{
		PTZ_ERROR ("nChannel<=%d> not valid, max = %d\n", 
						nChannel, MAX_PTZ_CHANNEL_NUM);
		return -1;
	}

	if (0 == IsSuportPtz())
	{
		return 0;
	}

	m_aCPtzDev[nChannel]->PrivacyMaskPreset(AwakeMode);

	return 0;
}
int CPtzManager::UpdateDorStatus(int bDorEnable)
{
	if (m_bDorEnable == bDorEnable)
    {
        return 0;
    }
    m_bDorEnable = bDorEnable;

    /* 唤醒 */
    if (m_bDorEnable == 0)
    {
        /* 移出限位点 */
        PTZ_PrivacyMaskPreset(0, !m_bDorEnable);
    }
    /* 休眠 */
    else
    {
        /* 停止巡航 */
        PTZ_StopMotorCruise(0);
        /* 移动至限位点 */
        PTZ_PrivacyMaskPreset(0, !m_bDorEnable);
    }

    return 0;
}

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
//获取电机当前状态
int CPtzManager::PTZ_GetMotorState(int *state)
{
	return MOTO_GetState(state);
}
#endif



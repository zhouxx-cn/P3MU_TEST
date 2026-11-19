#ifndef __CSOUND_DETEDT__H__
#define __CSOUND_DETEDT__H__

#include <pthread.h>
#include "global_macro.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "CConfig.h"
#include "zwsdk.h"
#include "RECT.h"
#include "AVL_global.h"



typedef struct tagSoundDetectParam
{
    int iSwitch;
    int iSentive;
}SOUND_DETECT_PARAM_S;
class CActionAlarm;

class CSoundDetectManager
{
public:
    #define MAX_SOUND_PORT 1
    #define SUCCESS        0
    #define FAILED         (-1)

public:
	CSoundDetectManager(int nCh, void *hHandle);
	~CSoundDetectManager();
	int Init(int nMaxCh, int nMaxAlarmOut);
	void UnInit(void);
	//int PutEvent(void* param, void* param2 = NULL);
	int SetSoundDetectCfg(int nCh, SOUND_DETECT_PARAM_S stParam);
	int GetSoundDetectCfg(int nCh, SOUND_DETECT_PARAM_S *stParam);
    int PushEvent( int bSoundStatus);
	static int SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));

    int IsEnableAction(int Action);
    
    int SetPushEventIntvCnt(int nPushEventIntv);
	int GetPushEventIntvCnt(int *nPushEventIntv);

	//检测声音
	int CheckSound();
private:
	int m_iChannel;
    void *m_henc;
	int m_SoundDetectEnable[MAX_SOUND_PORT];
    int m_SoundDetectSentive[MAX_SOUND_PORT];

	//是否有声音
	int m_bisSound;
	int m_lastSound;

	//配置文件
	CConfig *pCConfig;
	//报警类型
    static int m_AlarmType;
	//动作处理
	CActionAlarm *m_pActionAlarm;
	//回调函数
    static ALARMCALLBACK m_FxnCallBack;
};


#endif // __CPAYMANAGER_H__ 

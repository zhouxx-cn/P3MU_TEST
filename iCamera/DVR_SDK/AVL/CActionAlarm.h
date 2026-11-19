
/*!
*\file		CActionAlarm.h
*\brief	报警动作处理
*\author	周庆丰
*\date		2009/04/24
*/

#ifndef _CACTIONALARM_H_
#define _CACTIONALARM_H_

#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "CConfig.h"
#include "AVL_global.h"
#include "MPU_AVL_API.h"

typedef int (*MotionCallBack) (int);
typedef int (*ALARMCALLBACK) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);

typedef struct _tagACTION_
{
	int bEnable;				//32 bits 每位表示一种动作使能
	int bRecordCH;				//32 bits 每位表示一通道,录像 与视频通道相关
	int bSnapshotCH;			//32 bits 抓图 与视频通道相关
	int bDopCH;					//32 bits 报警输出 与报警输出个数相关
	int bScreenCH;				//32 bits 屏显 与视频通道相关
	int bPtzCH;					//32 bits PTZ 与视频通道相关
	int iPtzAction[MAX_ACTION_CH_NUM];	//H16-动作(0-无动作，1-预置点,2-巡航，3-轨迹)，L16-序号(预置点)

} __attribute__ ((packed)) ACTION_T;

class CActionAlarm
{
  public:
	CActionAlarm();

	//!\brief 初始化
	//!\param ch表通道号,section表示配置区域名称,pkeystr表示配置key名的前缀
	//!\param iMaxCh最大编码通道;iMaxAlarmOut最大报警输出编号
	//!\return 0-成功,其它失败
	int Init(int ch, const char *section, const char *pkeystr, int iMaxCh = 16, int iMaxAlarmOut = 8);

	//!\brief 启用某个报警联动动作
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\praram int bEnable false 为禁止联动 true 为开启联动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEFDF0009
	int EnableAction(int Action, int bEnable);

	//!\brief 判断某个报警联动动作是否开启
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\return false 为禁止联动 true 为开启联动
	//##ModelId=49BDEFDF0081
	int IsEnableAction(int Action);

	//!\brief 获取某个报警联动录像参数
	//!\param dwChannelFlag 联动的通道标记 
	//每一位表示联动一个通道 1表示联动通道1 
	//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功，-1表示获取失败
	//##ModelId=49B7187602BF
	int GetRecordParam(int *dwChannelFlag);

	//!\brief 设置某个报警联动录像参数
	//!\param dwChannelFlag 联动录像通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=49B718640232
	int SetRecordParam(int dwChannelFlag);

	//!\brief 
	//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
	//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
	//!\param nPort 报警联动输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B5CE9A030D
	int GetSnapshotParam(int *dwChannelFlag);

	//!\brief 
	//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
	//c::SetCfg设定
	//!\param dwChannelFlag 联动拍照通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示失败
	//##ModelId=49B5CDFE00BB
	int SetSnapshotParam(int dwChannelFlag);

	//!\brief 
	//获取屏显配置
	//!\param dwChannelFlag 
	//!\param nPort 报警联动输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B5CE9A030D
	int GetScreenParam(int *dwChannelFlag);

	//!\brief 
	//设置屏显配置
	//!\param dwChannelFlag  
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示失败
	//##ModelId=49B5CDFE00BB
	int SetScreenParam(int dwChannelFlag);

	//!\brief 获取某一某个报警通道联动某一云台的动作参数
	//!\param nAction 联动动作 
	//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
	//!\param nIndex 联动预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功, -1表示获取失败
	//##ModelId=49B718B202DE

	int GetPtzParam(int nChannel, int *nAction, int *nIndex);


	//!\brief 设置某个报警通道联动某一个云台的动作
	//!\param nAction 联动的动作 0表示无动作 1联动预置点 
	//2联动巡航 3联动轨迹
	//!\param nIndex 联动的预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718BD0261
	int SetPtzParam(int nChannel, int nAction, int nIndex = 0);

	//!\brief 获取某个报警联动报警输出的参数
	//!\param dwOutPortFlag 存储输出通道标记的缓冲区
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718DD030D
	int GetDopParam(int *dwOutPortFlag);

	//!\brief 设置某个报警联动报警输出的参数
	//!\param dwOutPortFlag 设置输出通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718ED00AB
	int SetDopParam(int dwOutPortFlag);

	int SetPushEventIntvCnt(int nAlarmType, int nPushEventIntv);

	int GetPushEventIntvCnt(int nAlarmType, int *nPushEventIntv);

    //!\brief 更新联动动作间隔计数值
    //!\param nAlarmType 报警类型,nAlarmStatus报警状态(1报警 0消警)
    //!\return 0成功，否则失败
    int UpdateActionIntvCnt(int nAlarmType, int nAlarmStatus);

	//!\brief 检查（处理）动作，并回调
	//!\param m_AlarmType 报警类型,m_FnxCallBack 回调函数
	//!\return 0成功，否则失败
	int CheckAction(int Chn,int m_AlarmType, ALARMCALLBACK m_FxnCallBack, int alarmStatus=0);

  protected:
  private:

  private:

	//通道号
	int m_iChannel;

	//动作
	ACTION_T m_tAction;			//


	//配置文件
	CConfig *pCConfig;
	//动作key前缀
	char pKeyPrefix[80];
	//区域
	char pSection[80];

	//最大通道能力
	int m_MaxCh;

	//最大报警输出能力
	int m_MaxAlarmOut;

    //推送间隔 0-motion 1-humanShape 2-alarmIn
	int m_nPushEventIntv[3];
	//推送计数
	int m_nPushEventIntvCnt[3];
	//推送使能
	BOOL b_nPushEventEnable[3];

  private:
	//!\brief 载入设置
	int LoadParam();

	//!\brief 扫描字符串
	//!\param char *pbuff 缓冲
	//!\param int *start_pos 开始位置
	//!\param char sep 分隔符
	//!\return 返回以16进制转换好的数据
	int GetSepValue(char *pbuff, int *start_pos, char sep);

	//!brief 保存配置
	int SaveParam();
};
#endif

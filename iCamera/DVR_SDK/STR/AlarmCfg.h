
/*!
*\file   AlarmCfg.h 
*\brief  报警参数的配置及读取
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#ifndef __ALARMCFG_H__
#define __ALARMCFG_H__

#include <pthread.h>
#include "pthreadmacro.h"
#include "debug.h"
#include "CBAlert.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"


class CAlarm
{

  public:

	//!\brief 发送报警时联动处理
	//!\param int Type 硬盘错误类型
	//!\param CBAlert callabck 回调指针
	//!\return 成功为0，失败为错误码
	int PushAlarm(void *org, struct tagAlertInfo *Info = NULL);

	//!\brief 设置硬盘错误回调。全部硬盘出现错误时，struct tagAlertInfo保留字为1,部分硬盘错误时为0.
	//!\param int Type 硬盘错误类型
	//!\param CBAlert callabck 回调指针
	//!\return 成功为0，失败为错误码
	int SetCallBack(int AlarmType, CBAlert callabck);

	//!\brief 开启/禁止某个报警联动动作
	//!\praram int AlarmType 报警类型。VIDEO_LOST 
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\bool bEnahle true为开启，false为禁止
	//!\return 成功为0，失败为错误码
	//##ModelId=49BDBA5C02A0
	int EnableAction(int AlarmType, tagActionType Type, bool bEnahle);

	//!\brief 判断某个报警联动动作是否开启
	//!\praram int AlarmType 报警类型。
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\return false 为禁止联动 true 为开启联动
	//##ModelId=49BDBA5C02A0
	bool IsEnableAction(int AlarmType, tagActionType Type);

	//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
	//!\param int nCh 通道
	//!\praram int AlarmType 报警类型
	//!\praram tagActionType Action   动作类型
	//!\praram int AlarmCh 联动动作涉及到通道。
	//!\praram bool bEnable 启用/不启用该通道的配置
	//!\praram int AlarmCh_ActionNo 对于预置点、轨迹、巡航线路输入上述编号
	//!\return  成功为0，失败为-1
	int SetActionChParam(int AlarmType, tagActionType Action, int AlarmCh, bool bEnable, int AlarmCh_ActionNo);


	//!\brief 获取某个联动动作涉及的通道或者编号
	//!\praram int AlarmType 报警类型
	//!\praram tagActionType Action 动作类型
	//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
	//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
	//!\return  成功为0，失败为-1
	int GetActionChParam(int AlarmType, tagActionType Action, bool * szCh, int *szReserved = NULL);

	//!\brief 初始该配置的段名称
	//!\praram char *section 段名称
	//!\return  成功为0,失败为-1
	int Init(const char *section);

	//!\brief 读取配置
	//!\praram 无
	//!\return  成功为0,失败为-1
	int ReadCfg();
  private:

	//联动动作关于通道部分的信息
	struct __CHINFO__
	{
		bool bEnable;
		short No;
		short Type;
	};    

	pthread_mutex_t mutex;
	char m_section[20];

	//联动动作构体
	typedef struct __ACTION_CFG__
	{
		bool bEnable;			//该动作是否被禁止
		struct __CHINFO__ *ChInfo;
	} tagActionCfg;

	int m_nAlarmType;

	//回调函数
	CBAlert m_AlarmCallBack;

	//联动动作的配置
	tagActionCfg *m_tagAlarmActionCfg;

};

#endif

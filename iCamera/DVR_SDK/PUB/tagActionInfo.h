#ifndef __TAG_ACTIONTYPE_H_
#define __TAG_ACTIONTYPE_H_

//!联动动作类型
enum tagActionType
{
	//高2字节表示一个联动动作，低2字节表示动作子类
	//子类间如触发同一通道则互斥,否则不互斥,低层处理互斥.
	//动作类别与子类都是从1开始；对子类字节为0的表示总开关。
	//子类的动作无法单独禁止，只能通过主类禁止。
	//!通知GUI(记录日志等处理，所有报警都必须通知GUI)
	ACTION_GUI = 0x0,
	//!录像
	ACTION_RECORD = 0x10000,
	//!报警输出
	ACTION_ALARMOUT = 0x20000,
	//!联动屏幕显示
	ACTION_SCREEN = 0x30000,
	//!联动声音报警
	ACTION_BEEP = 0x40000,
	//!联动上传报警服务器
	ACTION_SENDTOSERVER = 0x50000,
	//!联动Email
	ACTION_EMAIL = 0x60000,
	//!联动短信
	ACTION_SMS = 0x70000,
	//!联动云台
	ACTION_PTZ = 0x80000,		//低2字节为0,此类型可控制其所有子类动作
	//!联动预置点
	ACTION_PTZ_PRESET = 0x80001,
	//!联动巡航
	ACTION_PTZ_CRUISE = 0x80002,
	//!联动轨迹
	ACTION_PTZ_TRACK = 0x80003,
	//!联动线扫
	ACTION_PTZ_SCAN = 0x80004,
	//!联动抓图
	ACTION_SNAPSHOT = 0x90000,
	//!联动FTP上传
	ACTION_FTP = 0xa0000,
	//!联动推送
	ACTION_PUSH = 0xb0000,
	//!联动灯光报警
	ACTION_LIGHTALARM = 0xd0000,

};

//!\brief 联动动作信息
//##ModelId=49BB272E00DA
struct tagActionInfo
{
  public:
	//!\brief 联动动作类型
	//##ModelId=49BB27620232
	int tActionType;

	//!\brief 通道号
	//##ModelId=49BB28040271
	int nChannel;

	//!\brief dwReserved 
	//保留位，联动类型为PTZ联动时可存储预置点号、轨迹号、巡航
	//线路号
	//##ModelId=49BB280A01F4
	unsigned long dwReserved;

};

typedef struct tagTimeArea
{
    int nHour;                              /*!< 时 */
    int nMinute;                            /*!< 分 */
    int nSecond;                            /*!< 秒 */
} stuTimeArea, *pTimeArea;

typedef struct tagTimeSegment
{
    stuTimeArea struStartTime;
    stuTimeArea struStopTime;
}stuTimeSegment, *pTimeSegment;

typedef struct tagAlarmOutTimeSchedule
{
    int nTimeMode; /*!< 0:全天 1:白天 2:晚上 3:自定义 */
    stuTimeSegment struTimeSegment[6];  /*!< 仅自定义起效 */
}stuAlarmOutTimeSchedule, *pAlarmOutTimeSchedule;


#endif

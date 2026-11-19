#ifndef __TAG_ALERTINFO_H_
#define __TAG_ALERTINFO_H_

//!\brief 报警和异常警告类型
enum enumAlertType
{
	ALERT_TYPE_ALARMIN,		 				/*!< 报警输入                          (EventAlarmIn)         */
	ALERT_TYPE_MOTION,		 				/*!< 移动侦测                                   */
	ALERT_TYPE_ENCODEERROR,	 				/*!< 编码异常                                   */
	ALERT_TYPE_DISKERROR,	 				/*!< 硬盘错误                                   */
	ALERT_TYPE_DISKFULL,	 				/*!< 硬盘满                                     */
	ALERT_TYPE_IPCONFLICT,	 				/*!< IP冲突                                     */
	ALERT_TYPE_ILLEGEACCESS, 				/*!< 非法访问                                   */
	ALERT_TYPE_RETICLEDISCONNECT, 			/*!< 网线断                                     */
	ALERT_TYPE_VIDEOLOST,		  			/*!< 视频丢失                                   */
	ALERT_TYPE_VIDEOEXCEPTION,	  			/*!< 视频异常                                   */
	ALERT_TYPE_DISKGROUP_ERROR,	  			/*!< 盘组异常                                   */
	ALERT_TYPE_IMODE,			  			/*!< 待商议(内部使用)                           */
	ALERT_TYPE_IPCHANGED,		  			/*!< IP地址变化,用于发送HELLO消息(内部使用)     */
	ALERT_TYPE_REMOTE_ALARMIN,	  			/*!< 远程报警输入                               */
	ALERT_TYPE_HUMANSHAPE,		  			/*!< 人形检测                                   */
	ALERT_TYPE_FACEDETECT,		  			/*!< 人脸检测                                   */
	ALERT_TYPE_FACERECO,		  			/*!< 人脸识别                                   */
	ALERT_TYPE_PIR,				  			/*!< PIR                              (EventPIR)     */
	ALERT_TYPE_FACETEMP,		  			/*!< 人脸体温                                   */
	ALERT_TYPE_FACEMASK,		  			/*!< 人脸口罩                                   */
	ALERT_TYPE_TRIPWIRE,		  			/*!< 绊线检测                                   */
	ALERT_TYPE_FENCEDETECT,		  			/*!< 电子围栏                                   */
	ALERT_TYPE_HUMANCOUNT,		  			/*!< 客流统计                                   */
	ALERT_TYPE_CAR_LICENSE,		  			/*!< 车牌识别                                   */
	ALERT_TYPE_SOUNDDETECT,		  			/*!< 声音侦测                   (EventVoice)      */
	ALERT_TYPE_HARDWARERESET,	  			/*!< 硬件复位                                   */
	ALERT_TYPE_CAR,				  			/*!< 车形                                       */
	ALERT_TYPE_ALARMOUT,		  			/*!< 报警输出事件(Onvif需要)                    */
	ALERT_TYPE_ABNORMAL,		  			/*!< 设备异常                                   */
	ALERT_TYPE_CHANNEL_STATES,	  			/*!< 通道状态变更（EventOVDChannelState）	*/
	ALERT_TYPE_IRCUT,			  			/*!< 光敏事件                                   */
	ALERT_TYPE_OPENDOORBYKEY = 0x1000, 		/*!< 按键开门信号                               */
	ALERT_TYPE_OPENDOORBYPASSWD,	   		/*!< 密码开门信号                               */
	ALERT_TYPE_OPENDOORBYCARD,		   		/*!< 人脸检测                                   */
	ALERT_TYPE_OPENDOORBYQR,		   		/*!< 磁卡开门信号                               */
	ALERT_TYPE_EBIKE_DETECT,		   		/*!< 电动车                                     */
	ALERT_TYPE_BICICLE_DETECT,		   		/*!< 自行车                                     */
	ALERT_TYPE_OFFDUTY_DETECT,		   		/*!< 离岗侦测                                   */
	ALERT_TYPE_VEHICLE_DETECT,		   		/*!< 机动车侦测                                   */
	ALERT_TYPE_REMOTE_UPDATE_NOTIFY,   		/*!< 远程升级通知                                  */
	ALERT_TYPE_FIRE_DETECT, 			  	/*!< 火焰侦测(中间层内部使用)                      */
	ALERT_TYPE_PASSENGERFLOWHUMAN_DETECT, 	/*!< 客流统计人形事件                                   */
	ALERT_TYPE_OPENDOORBYHEALTH,		  	/*!< 健康检测开门信号           */
	ALERT_TYPE_VIDEO_SHELTER,			  	/*!< 视频遮挡事件                      */
	ALERT_TYPE_FACE_SNAPSHOT,			  	/*!< 人脸抓拍事件 */
	ALERT_TYPE_MASK_DETECT,				  	/*!< 口罩检测事件 */
	ALERT_TYPE_OFFDUTY_TOTAL,			  	/*!< 离岗统计*/
	ALERT_TYPE_PASSENGERFLOWHUMAN_TOTAL,  	/*!< 客流统计*/
	ALERT_TYPE_SMART_TRACK,				  	/*!< 智能跟踪*/
	ALERT_TYPE_LICENSE_PLATE_CAP,		  	/*!< 车牌抓拍*/
	ALERT_TYPE_LONG_AWAY,		  			/*!< 长久离家                                   */
};

//!\brief 报警信息
//##ModelId=49BB271F036B
struct tagAlertInfo
{
  public:
	//!\brief 报警类型
	//##ModelId=49BB273B0251
	int AlertType;

	//!\brief 通道号
	//##ModelId=49BB27490109
	int nChannel;

	//!\brief 保留信息 
	//##ModelId=49BDCEEE01F4
	unsigned long dwReserved;

};
#endif

#ifndef _TAG_BAKSTATE_H_
#define _TAG_BAKSTATE_H_

//!\brief 备份装态
typedef enum
{
	//!\brief 正在备份
	BAK_RUNNING = 1,

	//!\brief 备份出现异常,已退出
	BAK_EXCEPTION = 2,

	//!\brief 手动退出备份
	BAK_MANUALSTOP = 3,

	//!\brief 备份正常结束
	BAK_STOP = 4,

	//!\brief 备份数据达到最新
	BAK_TIMEOVER = 5,

	//!\breif 硬盘出现错误
	BAK_DISKERR = 6,

	//!\brief 备份开始失败
	BAK_STARTERR = 7,

	//!\brief 未知错误
	BAK_UNKNOWNERR = 8,

	//!\brief 准备文件
	BAK_PREPARE = 9,

	//!\brief 准备文件失败
	BAK_PREPAREERR = 10,

	//!\brief 找不到指定的刻录机
	BAK_RECORDERLOST = 11,

	//!\brief 开始刻录
	BAK_STARTRECORD = 12,

	//!\brief 不支持不能识别的碟片
	BAK_UNSUPPORTDISK = 13,

	//!\brief 不能识别的碟片
	BAK_UNKNOWNDISK = 14,

	//!\brief 不能写的碟片
	BAK_UNWRITEDISK = 15,

	//!\biref 刻录失败
	BAK_RECORDFAILED = 16
} BAK_STATE;

#endif

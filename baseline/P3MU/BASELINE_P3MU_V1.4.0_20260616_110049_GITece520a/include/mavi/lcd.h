#ifndef _LCD_H_
#define _LCD_H_

#include "xrsdef.h"
#include "xrsstd.h"

typedef enum 
{
    LCD_ERROR_START = COMP_ERROR_START, 
	LCD_ERROR_UNINIT,									/* 模块未初始化*/
	LCD_ERROR_REINIT,									/* 模块重复初始化*/
    LCD_ERROR_SDKAPIABN,								/* SDK API调用失败*/
} LCD_ERROR_E;

#define LCD_TRACE_ERR(str, args...)  TRACE_ERR(COMP_LCD, str, ##args)
#define LCD_TRACE_WARN(str, args...) TRACE_WARN(COMP_LCD, str, ##args)
#define LCD_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_LCD, str, ##args)
#define LCD_TRACE_INFO(str, args...) TRACE_INFO(COMP_LCD, str, ##args)
#define LCD_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_LCD, str, ##args)

typedef struct
{
    UINT32 Width;  /* LCD显示宽度 */
    UINT32 Height;   /* LCD显示高度 */
    UINT32 Resvd[2];   /* 保留位 */
} LCD_CONF_S, *PLCD_CONF_S;
#define LCD_CONF_S_LEN sizeof(LCD_CONF_S)

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: handle: 模块句柄(此处无效)
 *           LcdConf：模块配置信息
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Lcd_Open(SINT32 handle, LCD_CONF_S LcdConf);

/******************************************************************************
 * 函数功能: 启动模块
 * 输入参数: handle: 模块句柄(此处无效)
 *           priority: 线程优先级
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Lcd_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数功能: 停止模块
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Lcd_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 向LCD发送RGB565帧数据
 * 输入参数: handle: 模块句柄(此处无效)
 *           pData: RGB565数据
 *           size: 数据大小
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Lcd_SendFrame(SINT32 handle, UINT8 *pData, UINT32 Size);

/******************************************************************************
 * 函数功能: 向LCD发送RGB565帧数据
 * 输入参数: handle: 模块句柄(此处无效)
 *           Enable: 背光使能
 *           Level: 背光等级(0~100)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Lcd_SetBackLight(SINT32 handle, UINT32 Enable, UINT32 Level);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Lcd_Close(SINT32 handle);
#endif /* _LCD_H_ */

#ifndef __XRS_LCD_H__
#define __XRS_LCD_H__

#if defined(__IS_SUPPORT_LCD_MANAGER__)

#include "xrsdef.h"

SINT32 Xrs_Lcd_StartLCD(void *hChannelHandle);

SINT32 Xrs_Lcd_StopLCD(void);

SINT32 Xrs_Lcd_RefreshFrame(char *pData, UINT32 DataSize);

SINT32 Xrs_Lcd_SetBackLightLevel(int level);

SINT32 Xrs_Lcd_StartVdec(void *hChannelHandle);

SINT32 Xrs_Lcd_StopVdec(void);

SINT32 Xrs_Lcd_CreateVdecChn(int nType);

SINT32 Xrs_Lcd_SendStreamToChn(int nType, char *pData, UINT32 DataSize);

SINT32 Xrs_Lcd_DestoryVdecChn(int nType);

#endif // __IS_SUPPORT_LCD_MANAGER__
#endif // __XRS_LCD_H__

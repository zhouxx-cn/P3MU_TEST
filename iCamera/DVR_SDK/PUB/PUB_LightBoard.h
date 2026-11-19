
/**
 * @file PUB_LightBoard.h
 * @date 20200423
 * @brief 中间层 灯板相关 公共函数
 */

#ifndef PUB_LIGHTBOARD_H__
#define PUB_LIGHTBOARD_H__

#include <zwsdk.h>

#ifdef __cplusplus
extern "C" {
#endif 

/* 灯板能力 */
typedef struct 
{
    int bSupportColorNight;         // 全彩夜视
    int bSupportIntelligentNight;   // 智能夜视
    int bSupportVariableInfrared;   // 无极红外
    int bSupportVariableWhite;      // 无极白光
    int bSupportFaceNoExposure;     // 人脸无曝光
    int bSupportLicensePlate;       // 车牌模式
    int VariableInfraredValueFixed; 
    int VariableWhiteFixed;         
} PUB_LightBoard_Capbility;


//灯板类型
typedef enum enLightBoardType
{
    LightBoardType_NormalInfrared = 1,              //硬光敏普通红外
    LightBoardType_SingleStatusLed,                 //卡片机单色灯灯板
    LightBoardType_InfiniteWhiteLight,              //无极白光
    LightBoardType_SoftlightSensitiveInfrared  = 4, //软光敏红外
    LightBoardType_DoubleHardLightSensitive = 6,    //硬光敏双光
    LightBoardType_DoubleSoftLightSensitive = 7,    //软光敏双光源
    LightBoardType_BUFF,
} LightBoardType;

/**
 * @brief 获取灯板能力
 * @param [IN] eLBType 灯板类型
 * @param [OUT] psCap 灯板能力
 * @return 成功返回0，失败返回-1
 * @note 未定义的灯板类型会返回失败
 */
int PUB_LightBoard_GetCapbility (LightBoardType eLBType, PUB_LightBoard_Capbility *psCap);

#ifdef __cplusplus
}
#endif 

#endif // PUB_LIGHTBOARD_H__

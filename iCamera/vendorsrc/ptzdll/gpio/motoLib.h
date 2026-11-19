/*!
 *\file		motoLib.h
 *\brief	电机控制模块
 *\author	simonlay
 *\date		2019年6月4日 9:42:10
 */
#ifndef __H_MOTO__
#define __H_MOTO__


#ifdef __cplusplus
extern "C" {
#endif

/*@breif DesEncryption DES加密解密操作
 *@param[out] Out 加密或者解密之后的缓冲区
 *@param[in]  In 需要加密或者解密的缓冲区
 *@param[in]  datalen In的长度
 *@param[in]  Key 密钥
 *@param[in]  keylen Key密钥长度
 *@param[in]  Type 操作类型: 0->解密  1->加密
 *@return 成功返回1 失败返回0
 */
int MOTO_SelfTest(int Chn);
int MOTO_Stop(int Chn);
int MOTO_SetAddr();
int MOTO_Left(int nSteps,unsigned int Chn);
int MOTO_Right(int nSteps,unsigned int Chn);
int MOTO_Up(int nSteps,unsigned int Chn);
int MOTO_Down(int nSteps,unsigned int Chn);
int MOTO_GetPos(int *pHPos, int *pVPos);
int MOTO_ToPos(int nHPos, int nVPos);
int MOTO_ToStartPos(int nHPos, int nVPos);
int MOTO_SetMaxPos(int nHPos, int nVPos,unsigned int Chn);

int MOTO_DropLeft(int nSteps,unsigned int Chn);
int MOTO_DropRight(int nSteps,unsigned int Chn);
int MOTO_DropUp(int nSteps,unsigned int Chn);
int MOTO_DropDown(int nSteps,unsigned int Chn);

int MOTO_ToPosExt(int nHPos, int nVPos);
int MOTO_GetState(int *pState);
int MOTO_GetStartState(int *pState);
int MOTO_SetSpeed(unsigned int nHSpeed,unsigned int nVSpeed,unsigned int Chn);

int MOTO_Init();
int MOTO_SetPreset(unsigned long dwPresetIndex);
//获取预置点坐标
int MOTO_GetPresetPos(unsigned long dwPresetIndex, int *pHPos, int *pVPos);
int MOTO_CallPreset(unsigned long dwPresetIndex);
int MOTO_RemovePreset(unsigned long dwPresetIndex);
int MOTO_SelfTestLoop();

#ifdef __cplusplus
}
#endif

#endif // end of __H_MOTO__

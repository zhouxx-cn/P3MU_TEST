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

#if DEVICE_SUPPORT_PTZ_NUM <=1
#define MOTO_DEVICE_NAME_0 "motor"
#else
#define MOTO_DEVICE_NAME_0 "motor0"
#define MOTO_DEVICE_NAME_1 "motor1"
#define MOTO_DEVICE_NAME_2 "motor2"
#endif

enum {
    NO = 0,
    YES = 1
};

typedef struct
{
    int idx;		             // 巡航预置位序号
    int *bStop;                  // 停止信号
    int nCntTime;                // 返回所耗费时间(MS)
    int retentionTime;           // 预置位停留时间(S)
} GO_CRUISE_NODE_INFO;

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
int MOTO_Left(int nSteps,unsigned int Chn);
int MOTO_Right(int nSteps,unsigned int Chn);
int MOTO_Up(int nSteps,unsigned int Chn);
int MOTO_Down(int nSteps,unsigned int Chn);
int MOTO_GetPos(int *pHPos, int *pVPos);
int MOTO_ToPos(int nHPos, int nVPos);
int MOTO_ToPos_Product(int nHPos, int nVPos);

int MOTO_ToStartPos(int nHPos, int nVPos);
int MOTO_SetMaxPos(int nHPos, int nVPos,unsigned int Chn);
int MOTO_SetPos(int nHPos, int nVPos,unsigned int Chn);
int MOTO_DropLeft(int nSteps,unsigned int Chn);
int MOTO_DropRight(int nSteps,unsigned int Chn);
int MOTO_DropUp(int nSteps,unsigned int Chn);
int MOTO_DropDown(int nSteps,unsigned int Chn);


/**
 * @brief 人形检查移动预处理
 *        判断当前位置是否需要电机转动(误差控在2步以内)
 *
 * @param nHPos 
 * @param nVPos 
 * @return 1:需要转动 0:不需要转动
 */
int MOTO_PreToPosExt(int nHPos, int nVPos);
int MOTO_ToPosExt(int nHPos, int nVPos);
int MOTO_SelfCheckBack(int nHPos, int nVPos);
int MOTO_GetSelfCheckState(int *pState);
int MOTO_GetState(int *pState);
int MOTO_GetStartState(int *pState);
void MOTO_SetSpeed(unsigned int nSpeedh, unsigned int nSpeedv,unsigned int Chn);
int MOTO_SetSpeedType(int nHPos, int nVPos);

int MOTO_Init();
int MOTO_SaveNowPos();

int MOTO_SetPreset(unsigned long dwPresetIndex);
//获取预置点坐标
int MOTO_GetPresetPos(unsigned long dwPresetIndex, int *pHPos, int *pVPos);
int MOTO_CallPreset(unsigned long dwPresetIndex);
int MOTO_RemovePreset(unsigned long dwPresetIndex);
int MOTO_SelfTestLoop();

#ifdef __MICRO_POWER_SUPPORT__
//创建云台移动标志
void MOTO_CreateMoveFlag(void);

//删除云台移动标志
void MOTO_RemoveMoveFlag(void);

//获取云台移动标志
int MOTO_GetMoveFlag(void);
#endif

int MOTO_SetUserVMaxPos(int nVPos);
int MOTO_SetVerticalReverse(int nReverse,unsigned int Chn);
int MOTO_SetProductTestMode(int nTestMode);
int MOTO_GetProductTestMode();

int MOTO_GoCruiseNode(GO_CRUISE_NODE_INFO *node);
int MOTO_ToPos_AgingTest(int nHPos, int nVPos,unsigned int Chn);

#ifdef __cplusplus
}
#endif

#endif // end of __H_MOTO__

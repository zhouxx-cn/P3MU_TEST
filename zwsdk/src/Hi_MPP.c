#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "Hi_MPP.h"
#include "EncoderLib.h"
#include "cJSON.h"
#include "lightctrl.h"
#include "halmod.h"
#include "AISDK_API.h"
#include "xtrace.h"
#include "zwsdk.h"
#include "Zoom.h"
#include "Xrs_Lcd.h"
#include "inifile.h"

/* 全局宏定义 */
#define AUDIO_FRM_SIZE	320
#define SMART_P_GOP_RATIO (10)

#define ALGO_ACTIONLESS_TARGET_DISTANCELEN	(0.3)
#define ALGO_ACTIONLESS_TARGET_ZOOM_W		(0.3)
#define ALGO_ACTIONLESS_TARGET_ZOOM_H		(0.3)

/* 全局变量 */
int g_subSyncFlag[SUPPORT_MAX_SENSOR_COUNT] = {0};
static int g_DrawRectFlag[ALGO_CHANNEL][SECOND_STREAM+1] = {0};  //创建画框标记
#if defined(__IS_SUPPORT_12FPS__)
static int g_nCurSensorFrame = 12;                                      //当前Sensor帧率
#else
static int g_nCurSensorFrame = 15;                                      //当前Sensor帧率
#endif

#ifdef __IS_SUPPORT_JZ_CRY_DETECT__
//声音侦测缓冲区信息
typedef struct stAudioDetectBufInf
{
    char bValid;
    unsigned char ULawBuf[320];
    signed   short	PcmBuf[640];
    pthread_mutex_t m_muLock;
} AudioDetectBufInf;

static AudioDetectBufInf s_AudioDetectBufInf =
{
    0,
    {0},
    {0},
    PTHREAD_MUTEX_INITIALIZER
};
#endif


static BOOL g_bHalModinited = FALSE;                                    //硬件模块初始化标记
static unsigned int mainStreamShowTime[SUPPORT_MAX_SENSOR_COUNT] = {0}; //用于音视频同步
static int g_rggain = 0;                                                //叠加信息
static int g_bggain = 0;
static UINT32 g_gain = 0;
static UINT32 g_Exposure = 0;
static ISP_TUNE_FLICKER_MODE_E gCurAnTiStatus = ISP_TUNE_FLICKER_50HZ;
static human_struct g_HumanResult[ALGO_CHANNEL] = {0};                                //人形检测结果
static pthread_mutex_t g_human_result_lock;

/* 内部函数声明 */
static int AencDataNeedClear(CPlatformChannelContext *pPlatCtx);
static int CheckAencDataIfNeedClear(CPlatformChannelContext *pPlatCtx, SINT32 size, SINT8 *data);
#if defined (__PRODUCT_G43TA__) || defined (__PRODUCT_G84F__)||defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
static int ImageCustom(void);
#endif
/* 外部函数声明 */
extern unsigned G711A_TO_G711U(unsigned char* dst, const unsigned char* src, size_t srcSize);

/*********************************人形检测不动不检过滤结果获取*****************************************/
#if 1
#if	defined (__PRODUCT_P4__) || defined(__PRODUCT_P4U__)
#define FILTACTIONLESS_DISTANCE_RATIO (0.01)        /* 中心点偏差的比例的阈值 */
#else
#define FILTACTIONLESS_DISTANCE_RATIO (0.15)        /* 中心点偏差的比例的阈值 */
#endif
#define FILTACTIONLESS_MAX_CHECK_NUM (10)           /* 不动不检最大过滤人形个数 */
#define FILTACTIONLESS_ADDLIB_CNT (5)               /* 不动不检数据库新增数据连续次数 */
#define FILTACTIONLESS_ONE_CLRLIB_CNT (50)             /* 不动不检数据库清除数据且只保留最后出现人形数据连续次数 */
#define FILTACTIONLESS_CLRLIB_CNT (600)             /* 不动不检数据库清除数据连续次数 */
#define FILTACTIONLESS_DEFAULT_VALID_COUNT (50)    /* 不动不检单个人形信息有效次数，根据函数调用频率调整 */
#define __FILTACTIONLESS_DBG_ON__ (1)

/* 单人形目标数据 */
typedef struct human_single
{
    RECT_S ValidRect;   //人形目标位置信息
    int ValidCount;     //人形目标有效次数
}human_single;

/* 人形目标数据库 */
typedef struct pick_human
{
    int nValidNum;    //数据库中有效数
    human_single nHumanInfo[FILTACTIONLESS_MAX_CHECK_NUM];
}human_libry;

/***************************人形不动不检函数***************************************************
须知：
1、最大人形过滤个数为MAX_CHECK_NUM，超过则认为有移动，返回1
2、函数内保存一个静态数据库pic_human_str，保留之前的人形数据
3、数据库pic_human_str对每个人形数据保留DEFAULT_VALID_COUNT次
4、DEFAULT_VALID_COUNT根据外部调用频率调整
5、返回值：0(未移动) 1(有移动)

逻辑：
1、新入参人形个数（humanNum）小于等于0，返回0

2、新入参人形个数（humanNum）大于数据库中有效个数，判断是否满足
连续次数大于NEW_ADDLIB_NUM，小于则直接返回0。大于则将入参的每个人形
数据与数据库中的每个数据进行对比，更新数据库，必返回1

3、新入参人形个数（humanNum）小于等于数据库中有效个数，将入参的每个人形
数据与数据库中的每个数据进行对比，更新数据库，都能匹配到则返回0，否则返回1
*****************************************************************************************************/
static int GetFiltActionLessResult(int Chn,const RECT_S rectInfo[HCHD_DETECT_MAX], const int humanNum,int PtzStatus)
{
#if defined(__PRODUCT_P3MU__)
    return ((humanNum <= 0) ? 0 : 1);
#endif

    int nResult = 0, nChangeNum = 0;
    unsigned int i, j;
    int nRect1CenterX = 0, nRect1CenterY = 0, nRect2CenterX = 0, nRect2CenterY = 0;
    double nCurDistance = 0;
    static int sAddCnt[ALGO_CHANNEL] = {0},sClrCnt[ALGO_CHANNEL] = {0},sResrveCnt[ALGO_CHANNEL] = {0};
    static human_libry pic_human_str[ALGO_CHANNEL] = {0};
    static human_single last_human_str[ALGO_CHANNEL] = {0};

    static int Last_ptzstatus = 0;
    static human_single Before_MovingStr = {0};
    int IsCompareLib[FILTACTIONLESS_MAX_CHECK_NUM] = {0};
    const float nMaxDistance = sqrt((ALGO_STREAM_WIDTH-0)*(ALGO_STREAM_WIDTH-0) + (ALGO_STREAM_HEIGHT-0)*(ALGO_STREAM_HEIGHT-0)); // 算法通道分辨率计算最大距离
#if __FILTACTIONLESS_DBG_ON__
    int IsCompareCur[FILTACTIONLESS_MAX_CHECK_NUM] = {0};
#endif

    //进入不动不检逻辑前，先判断当前云台是否处于运动状态，如果处于运动状态则直接退出且清空数据库
    //并且记录进入运动前的检测结果，在运动停止后作为不动不检的最后一个判断条件
    //多云台项目主要针对大球机通道0
    if(Chn == 0)
    {
        if(PtzStatus == ePtzStatus_Moving)
        {
            if(Last_ptzstatus != PtzStatus)
            {
                memset(&pic_human_str[Chn], 0, sizeof(pic_human_str[Chn]));
                memset(&Before_MovingStr,0,sizeof(human_single));
                memcpy(&Before_MovingStr,&last_human_str[Chn],sizeof(human_single));
            }
            Last_ptzstatus = PtzStatus;
            return 1;
        }
        else if(PtzStatus == ePtzStatus_Stop)
        {
            if(Last_ptzstatus != PtzStatus)
            {
                memset(&pic_human_str[Chn], 0, sizeof(pic_human_str[Chn]));
                memcpy(&(pic_human_str[Chn].nHumanInfo[0]), &Before_MovingStr, sizeof(human_single));
                memset(&Before_MovingStr, 0, sizeof(Before_MovingStr));
            }
            Last_ptzstatus = PtzStatus;
        }
    }


    //1、新入参人形个数小于等于0，返回0
    if (humanNum <= 0)
    {
        sClrCnt[Chn]++;
        //连续超过60S未检测到人形则全部清空
        if (sClrCnt[Chn] > FILTACTIONLESS_CLRLIB_CNT)
        {
            sClrCnt[Chn] = 0;
            memset(&pic_human_str[Chn], 0, sizeof(pic_human_str[Chn]));
            sResrveCnt[Chn] = 0;
        }
        //连续超过5S且未达到60S未检测到人形则清空数据库，保留最后出现的一个数据
        else if(sClrCnt[Chn] > FILTACTIONLESS_ONE_CLRLIB_CNT && sResrveCnt[Chn] == 0)
        {
            memset(&pic_human_str[Chn], 0, sizeof(pic_human_str[Chn]));
            memcpy(&(pic_human_str[Chn].nHumanInfo[0]), &last_human_str[Chn], sizeof(human_single));
            memset(&last_human_str[Chn], 0, sizeof(last_human_str[Chn]));
            sResrveCnt[Chn] = 1;
        }

        return 0;
    }
    sClrCnt[Chn] = 0;
    sResrveCnt[Chn] = 0;
    //2、基础数据库count计数值-1
    if (pic_human_str[Chn].nValidNum > 0)
    {
        for (i = 0;i < FILTACTIONLESS_MAX_CHECK_NUM;i++)
        {
            if (pic_human_str[Chn].nHumanInfo[i].ValidCount > 0)
            {
                pic_human_str[Chn].nHumanInfo[i].ValidCount--;
                if (pic_human_str[Chn].nHumanInfo[i].ValidCount == 0)
                {
                    pic_human_str[Chn].nValidNum--;
                }
            }
        }
    }
    //3、新入参人形个数大于数据库中有效人形个数，判断是否更新
    if (humanNum > pic_human_str[Chn].nValidNum)
    {
        if (++sAddCnt[Chn] <= FILTACTIONLESS_ADDLIB_CNT)
        {
            return 0;
        }
    }
    sAddCnt[Chn] = 0;

    //4、将当前检测结果与基础库内一一做对比
    for (i = 0; i < humanNum; i++)
    {
        int flag = 0, min_pos = 0;

        nRect1CenterX = rectInfo[i].s32X + (rectInfo[i].u32Width/2);
        nRect1CenterY = rectInfo[i].s32Y + (rectInfo[i].u32Height/2);

        for (j = 0; j < FILTACTIONLESS_MAX_CHECK_NUM; j++)
        {
            if (pic_human_str[Chn].nHumanInfo[j].ValidCount == 0)
            {
                min_pos = j;
                continue;
            }

            if (pic_human_str[Chn].nHumanInfo[j].ValidCount < pic_human_str[Chn].nHumanInfo[min_pos].ValidCount)
            {
                min_pos = j;
            }

            if (IsCompareLib[j] == 1)
            {
                continue;
            }

            nRect2CenterX = pic_human_str[Chn].nHumanInfo[j].ValidRect.s32X + (pic_human_str[Chn].nHumanInfo[j].ValidRect.u32Width/2);
            nRect2CenterY = pic_human_str[Chn].nHumanInfo[j].ValidRect.s32Y + (pic_human_str[Chn].nHumanInfo[j].ValidRect.u32Height/2);

            /* 计算两点之间距离 */
            nCurDistance = sqrt((nRect2CenterX-nRect1CenterX)*(nRect2CenterX - nRect1CenterX) + (nRect2CenterY-nRect1CenterY)*(nRect2CenterY-nRect1CenterY));
            if (nCurDistance < (nMaxDistance * FILTACTIONLESS_DISTANCE_RATIO))
            {
                flag = 1;
                IsCompareLib[j] = 1;
#if __FILTACTIONLESS_DBG_ON__
                IsCompareCur[i] = 1;
#endif
                break;
            }
        }

        //本次结果与数据库都不匹配
        if (flag == 0)
        {
            if (pic_human_str[Chn].nHumanInfo[min_pos].ValidCount == 0)
            {
                pic_human_str[Chn].nValidNum++;
            }

            IsCompareLib[min_pos] = 1;
            pic_human_str[Chn].nHumanInfo[min_pos].ValidCount = FILTACTIONLESS_DEFAULT_VALID_COUNT;
            memcpy(&(pic_human_str[Chn].nHumanInfo[min_pos].ValidRect), &(rectInfo[i]), sizeof(RECT_S));
            last_human_str[Chn].ValidCount = FILTACTIONLESS_DEFAULT_VALID_COUNT;
            memcpy(&last_human_str[Chn].ValidRect, &(rectInfo[i]), sizeof(RECT_S));
        }
        else
        {
            pic_human_str[Chn].nHumanInfo[j].ValidCount = FILTACTIONLESS_DEFAULT_VALID_COUNT;
            nChangeNum += 1;
        }
    }
    if (nChangeNum < humanNum)
    {
        nResult = 1;
    }

#if __FILTACTIONLESS_DBG_ON__
    if (access("/tmp/printf1",F_OK) == 0)
    {
        ZW_ERROR("nResult[%d] nChangeNum[%d] humanNum[%d] nValidNum[%d]\n", nResult, nChangeNum, humanNum, pic_human_str[Chn].nValidNum);
    }
    for (i = 0; i < FILTACTIONLESS_MAX_CHECK_NUM; i++)
    {
        if (access("/tmp/printf2",F_OK) == 0)
        {
            if (pic_human_str[Chn].nHumanInfo[i].ValidCount > 0 && IsCompareLib[i] == 0)
            {
                ZW_ERROR("[zxx_for_test 111] i=[%d] ValidCount[%d] X*Y[%d-%d] W*H[%d-%d]\n", i, pic_human_str[Chn].nHumanInfo[i].ValidCount,
                    pic_human_str[Chn].nHumanInfo[i].ValidRect.s32X, pic_human_str[Chn].nHumanInfo[i].ValidRect.s32Y,
                    pic_human_str[Chn].nHumanInfo[i].ValidRect.u32Width, pic_human_str[Chn].nHumanInfo[i].ValidRect.u32Height);
            }

            if (i < humanNum && IsCompareCur[i] == 0)
            {
                ZW_ERROR("[zxx_for_test 222] i=[%d] X*Y[%d-%d] W*H[%d-%d]\n", i, rectInfo[i].s32X, rectInfo[i].s32Y,
                    rectInfo[i].u32Width, rectInfo[i].u32Height);
            }
        }

        if (access("/tmp/printf3",F_OK) == 0)
        {
            ZW_ERROR("i=[%d] ValidCount[%d] X*Y[%d-%d] W*H[%d-%d]\n", i, pic_human_str[Chn].nHumanInfo[i].ValidCount,
                pic_human_str[Chn].nHumanInfo[i].ValidRect.s32X, pic_human_str[Chn].nHumanInfo[i].ValidRect.s32Y,
                pic_human_str[Chn].nHumanInfo[i].ValidRect.u32Width, pic_human_str[Chn].nHumanInfo[i].ValidRect.u32Height);
        }
    }
#endif

    return nResult;
}

#endif

/*********************************人形跟踪过滤结果获取*****************************************/
#if 1
#define HUMANTRACK_CHANGE_AREA_RATIO (1.5)  //操作跟踪目标面积的多少倍后切换
#if defined(__PRODUCT_G85T__) || defined(__PRODUCT_W85T__) || defined (__PRODUCT_G84F__)||defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
#define HUMANTRACK_CLEAR_RESULT_CNT (4)    //跟踪目标丢失多少帧后切换
#else
#define HUMANTRACK_CLEAR_RESULT_CNT (2)    //跟踪目标丢失多少帧后切换
#endif
 /*
  * func ： 从算法给到的所有目标中，找到最适合跟踪的那个坐标;
  * input：
  *     srcResult ： 算法给到的所有检测目标
  *     suitResult:  计算出来适合跟踪的那个目标
  * return ： 成功返回 0 ； 失败返回 -1
  *
 1.第一个跟踪出现后记录下trackID A;
 2.下次结果如果有trackID A
     A.是面积最大的，继续跟踪A；
     B.不是面积最大的，但是最大trackID B的面积没有超过A的HUMANTRACK_CHANGE_AREA_RATIO倍数，还是继续跟踪A；
     C.不是面积最大的，最大trackID B的面积超过A的HUMANTRACK_CHANGE_AREA_RATIO倍数，那切换为跟踪B，并更新跟踪的trackID；
 3.下次结果没有trackID A，记录丢失trackID A的帧数
     A.如果没有超过HUMANTRACK_CLEAR_RESULT_CNT，则云台保持不动；
     B.如果超过了HUMANTRACK_CLEAR_RESULT_CNT，则重新挑选最大面积的trackID；
  *
  */
 int GetHumanTrackTargetResult(const human_struct *srcResult)
 {
    static uint32_t sCurTrackId = 0; // 跟踪目标ID, 0表示没有跟踪目标
    static uint32_t sClearCnt = 0;   // 没人或者多人中不含上次检测的对象，就进行++；

    int nArea = 0;
    int nResultIndex = -1;
    int TrackHumanArea = 0;     //本次结果里跟踪目标的面积
    int TrackHumanIndex = -1;   //本次结果里跟踪目标的索引
    int MaxHumanArea = 0;       //本次结果里最大的目标面积
    int MaxHumanAreaIndex = -1; //本次结果里面积最大的目标的索引

    /* 遍历结果，筛选面积最大，同时判断是否存在上次跟踪的TrackID */
    for (int i = 0; i < srcResult->num; i++)
    {
        nArea = srcResult->rectInfo[i].u32Width * srcResult->rectInfo[i].u32Height;
        if (nArea > MaxHumanArea)
        {
            MaxHumanArea = nArea;
            MaxHumanAreaIndex = i;
        }
        if (sCurTrackId == srcResult->trackId[i])
        {
            TrackHumanIndex = i;
            TrackHumanArea = nArea;
        }
    }

    if (MaxHumanArea <= 0 || MaxHumanAreaIndex == -1)
    {
        goto END;
    }

    //没有跟踪目标
    if (0 == sCurTrackId)
    {
        //挑选跟踪目标(最大)
        sCurTrackId = srcResult->trackId[MaxHumanAreaIndex];
        sClearCnt = 0;
        nResultIndex = MaxHumanAreaIndex;
    }
    else
    {
        //如果存在则判断是否需要切换跟踪目标
        if (TrackHumanIndex != -1)
        {
            sClearCnt = 0;
            if (MaxHumanArea > (TrackHumanArea * HUMANTRACK_CHANGE_AREA_RATIO))
            {
                sCurTrackId = srcResult->trackId[MaxHumanAreaIndex];
                nResultIndex = MaxHumanAreaIndex;
            }
            else
            {
                nResultIndex = TrackHumanIndex;
            }
        }
        else //如果不存在则更新超时计数
        {
            if ((++sClearCnt) > HUMANTRACK_CLEAR_RESULT_CNT) //如果超时则切换到本次结果里面积最大的目标
            {
                sCurTrackId = srcResult->trackId[MaxHumanAreaIndex];
                sClearCnt = 0;
                nResultIndex = MaxHumanAreaIndex;
            }
        }
    }

END:
    return nResultIndex;
 }

#endif

#if 0
static int AI_COM_WriteFile4Mem(const char* buf, int len, const char* filename)
{
    int fd = -1;
    char path[128] = {0};
    int writeRet = 0;

    if (NULL == buf || NULL == filename)
    {
        return -1;
    }

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s", filename);

    fd = open(path, O_RDWR | O_CREAT|O_TRUNC, 0x775);
    if (fd < 0) {
        ZW_INFO("open %s failed:%s\n", path, strerror(errno));
        return -2;
    }

    writeRet = write(fd, buf, len);
    if (writeRet != len) {
        ZW_INFO("write frame failed(%d,%d)\n", writeRet, len);
        return -3;
    }

    close(fd);
    fd = -1;

    return writeRet;
}

static void SaveYuvData(char *pdate,uint32_t len,char *name)
{
    char path[256] = {0};
    static unsigned int count = 0;

    if (NULL == pdate || NULL == name)
    {
        ZW_ERROR("pdate or name is null\n");
        return;
    }

    struct tm tm;
    struct timeval tv_usec;

    char sz_time[32] = {0};
    time_t now;

    now = time(NULL);
    localtime_r((const time_t *)&now, &tm);
    gettimeofday(&tv_usec, NULL);

    snprintf(sz_time, 32, "%4d-%02d-%02d_%02d-%02d-%02d-%03ld",
             tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,(tv_usec.tv_usec/1000));

    snprintf(path, sizeof(path),"/nfsroot/test_pic/%s-%d-%s.h265", sz_time,count++,name);

    AI_COM_WriteFile4Mem(pdate,len,path);
}
#endif

void GetCurSensorFrame(UINT32 *sensorFrame)
{
    *sensorFrame = g_nCurSensorFrame;
}

/* 用于跟平台之间做类型转换 */
static VOSD_CHNTYPE_E __KS_VOSD_CHN_TYPE(DEV_SUPPORT_SENSOR SensorIndex, ENCODE_STREAM_E eStm)
{
    VOSD_CHNTYPE_E eChnType = VOSD_CHNTYPE_BUTT;

    if (FIRST_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MAIN;
            break;
        case SECOND_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MINOR;
            break;
        case FOURTH_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_SNAP;
            break;
        default:
            eChnType = VOSD_CHNTYPE_BUTT;
            break;
        }
    }
#if	SYSTEM_SUPPORT_CHANNEL >= 2

    else if (SEC_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MAIN_SEC;
            break;
        case SECOND_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MINOR_SEC;
            break;
        case FOURTH_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_SNAP_SEC;
            break;
        default:
            eChnType = VOSD_CHNTYPE_BUTT;
            break;
        }
    }
#endif
#if	SYSTEM_SUPPORT_CHANNEL >= 3
    else if (THIRD_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MAIN_THIRD;
            break;
        case SECOND_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MINOR_THIRD;
            break;
        case FOURTH_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_SNAP_THIRD;
            break;
        default:
            eChnType = VOSD_CHNTYPE_BUTT;
            break;
        }
    }
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 4
    else if (FOURTH_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MAIN_FOURTH;
            break;
        case SECOND_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_MINOR_FOURTH;
            break;
        case FOURTH_STREAM:
            eChnType = VOSD_CHNTYPE_VENC_SNAP_FOURTH;
            break;
        default:
            eChnType = VOSD_CHNTYPE_BUTT;
            break;
        }
    }
#endif
    return eChnType;
}

static VENC_CHNTYPE_E __KS_VENC_CHN_TYPE(DEV_SUPPORT_SENSOR SensorIndex, ENCODE_STREAM_E eStm)
{
    VENC_CHNTYPE_E eChnType = VENC_CHNTYPE_BUTT;

    if (FIRST_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VENC_CHNTYPE_MAIN;
            break;
        case SECOND_STREAM:
            eChnType = VENC_CHNTYPE_MINOR;
            break;
        default:
            ZW_ERROR("NOW ONLY SUPPORT Main and Sub Stream [%d]\n", SensorIndex);
            break;
        }
    }
#if	SYSTEM_SUPPORT_CHANNEL >= 2

    else if (SEC_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VENC_CHNTYPE_MAIN_SEC;
            break;
        case SECOND_STREAM:
            eChnType = VENC_CHNTYPE_MINOR_SEC;
            break;
        default:
            ZW_ERROR("NOW ONLY SUPPORT Main and Sub Stream [%d]\n", SensorIndex);
            break;
        }
    }
#endif
#if	SYSTEM_SUPPORT_CHANNEL >= 3

    else if (THIRD_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VENC_CHNTYPE_MAIN_THIRD;
            break;
        case SECOND_STREAM:
            eChnType = VENC_CHNTYPE_MINOR_THIRD;
            break;
        default:
            ZW_ERROR("NOW ONLY SUPPORT Main and Sub Stream [%d]\n", SensorIndex);
            break;
        }
    }
#endif
#if	SYSTEM_SUPPORT_CHANNEL >= 4

    else if (FOURTH_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VENC_CHNTYPE_MAIN_FOURTH;
            break;
        case SECOND_STREAM:
            eChnType = VENC_CHNTYPE_MINOR_FOURTH;
            break;
        default:
            ZW_ERROR("NOW ONLY SUPPORT Main and Sub Stream [%d]\n", SensorIndex);
            break;
        }
    }
#endif
    return eChnType;
}

/* END - 用于跟平台之间做类型转换 */

/**
********************************************************************************
 * @函数功能:
 * @作者    : HL.LIU
 * @param[in] pNetType
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int Hi_MPP_GetNetType(nic_mode_t * pNetType)
{
    SystemInfo_t *pSysInfo = NULL;
    nic_mode_t nicMode = eNicWire;

    HY_Res_GetSystemInfo(&pSysInfo);
    switch(pSysInfo->m_NicStatus)
    {
        case eXRS_NETCARD_WIRED_Unknown:
            nicMode = eNicWire;
        break;
        case eXRS_NETCARD_WIRED_1:
            nicMode = eNicWire;
        break;
        case eXRS_NETCARD_WIFI:
            nicMode = eNicWireless;
        break;
        case eXRS_NETCARD_4G:
            nicMode = eNic4G;
        break;
        default:
            ZW_ERROR("cur NET card error!!\n");
        break;
    }
    *pNetType = nicMode;

    return 0;
}

/**
********************************************************************************
 * @函数功能: 根据抗闪及网卡类型获取最大帧率
 * @作者    : HL.LIU
 * @param[in] *pMaxFrameRate
 * @返回值  :
********************************************************************************
*/
int Hi_MPP_GetMaxFrameRateByAntiflashAndNetType(int *pMaxFrameRate)
{
    nic_mode_t NetType;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    Hi_MPP_GetNetType(&NetType);

    switch(NetType)
    {
        case eNicWire:
            *pMaxFrameRate = (pSysInfo->m_Frequ_HZ == CMOS_FREQUENCY_60HZ) ? 30 : 25;
        break;

        case eNicWireless:
            *pMaxFrameRate = (pSysInfo->m_Frequ_HZ == CMOS_FREQUENCY_60HZ) ? 15 : 15;
        break;

        case eNic4G:
            *pMaxFrameRate = (pSysInfo->m_Frequ_HZ == CMOS_FREQUENCY_60HZ) ? 15 : 15;
        break;

        default:
            ZW_ERROR("cur NET card error!!\n");
            *pMaxFrameRate = (pSysInfo->m_Frequ_HZ == CMOS_FREQUENCY_60HZ) ? 30 : 25;
        break;
    }

    ZW_DEBUG("NetType=%s maxFrameRate=%d\n", (NetType ? "Wire" : "Wireless"), *pMaxFrameRate);

    return 0;
}

/**
********************************************************************************
 * @函数功能: 获取GOP值，根据输入的帧率及当前网卡类型
 * @作者    : HL.LIU
 * @param[in] frameRate
 * @param[in] *pGop
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int Hi_MPP_GetGOPValueByNetType(int frameRate, UINT16 *pGop)
{
    nic_mode_t NetType;

    Hi_MPP_GetNetType(&NetType);

    switch(NetType)
    {
        case eNicWire:
            *pGop = frameRate * 4;
        break;

        case eNicWireless:
            *pGop = frameRate * 4;
        break;

        case eNic4G:
            *pGop = frameRate * 6;
        break;

        default:
            ZW_ERROR("cur NET card error!!\n");
            *pGop = frameRate * 4;
        break;
    }

    ZW_DEBUG("NetType=%s pGop=%d\n", (NetType ? "Wire" : "Wireless"), *pGop);

    return 0;
}

//获取不同分辨率下不同VBR等级下的最大码率
static int GetVideoMaxBPS(CODEC_PARA_S *pstCodecParam, uint16_t *puMinRate, uint16_t *puMaxRate)
{
    int nRet = 0;

    //暂未调整minbps 先用Max的
    *puMinRate = 1024;
    *puMaxRate = 1024;

    return nRet;
}

static int GetVideoMaxMjpegBPS(CODEC_PARA_S *pstCodecParam, uint16_t *puMinRate, uint16_t *puMaxRate)
{
    int nRet = 0;

    *puMinRate = 1024;
    *puMaxRate = 1024;

    return nRet;
}

int Hi_MPP_GetVideoVBROrAvbrMaxBitRate(CODEC_PARA_S *pParam, int nBitRateCtlType, uint16_t *nMinBitBate, uint16_t *nMaxBitBate)
{
    CODEC_PARA_S *pstCodecParam = pParam;
    int ret = 0;

    if(pParam == NULL)
    {
        ZW_ERROR("get max BitRate error !!!\n");
        return -1;
    }

    if((ENCODE_BITRATE_CTRLMODE_AVBR == nBitRateCtlType) || (ENCODE_BITRATE_CTRLMODE_VBR == nBitRateCtlType))
    {
        if(LtMjpegVideo == pstCodecParam->EncType)
        {
            ret = GetVideoMaxMjpegBPS(pstCodecParam, nMinBitBate, nMaxBitBate);
        }
        else {
            ret = GetVideoMaxBPS(pstCodecParam, nMinBitBate, nMaxBitBate);
        }
    }
    else {
        ZW_ERROR(" nBitRateCtlType not vbr or avbr!!!\n");
        return -1;
    }
    return ret;
}

/**
********************************************************************************
 * @函数功能: 根据目标码率计算出变码率下的最小跟最大码率
 * @作者    : HL.LIU
 * @param[in] targetBitrate
 * @param[in] minBitrate
 * @param[in] maxBitrate
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int Hi_MPP_GetBitrateRangeInVbrByTargetBitrate(int targetBitrate, int *pMinBitrate, int *pMaxBitrate)
{
    SystemInfo_t *pSysInfo = NULL;
    nic_mode_t newNetType = eNicend;

    HY_Res_GetSystemInfo(&pSysInfo);
    Hi_MPP_GetNetType(&newNetType);

    if(pMinBitrate)
    {
        *pMinBitrate = targetBitrate * 0.4;
    }

    if(pMaxBitrate)
    {
        if (eNic4G == newNetType)
        {
            *pMaxBitrate = targetBitrate * 1.0;
        }
        else
        {
            //海思平台AVBR没有最大码率参数，基线使用MAX值给的平台
            *pMaxBitrate = targetBitrate * 2;
        }
    }

    return 0;
}

int Hi_MPP_SwitchAOMode(CPlatformChannelContext *pChContext, int mode)
{
    if (pChContext->m_AudioDecMode == mode)
    {
        return 0;
    }

    return 0;
}

/**
 * @description: 音频编码回调函数
 * @param {SINT32} stype 码流类型(STREAM_TYPE_E)
 * @param {SINT32} idx	通道号
 * @param {SINT8} *pHead 数据头(PMSSTD_S),!!!对讲时为NULL!!!
 * @param {SINT32} Headsize 数据头大小
 * @param {SINT8} *data 数据
 * @param {SINT32} size 数据大小
 * @return {*}
 */
static SINT32 _AudioData_Callback(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, SINT8 *data, SINT32 size)
{
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    STREAM_TYPE_E eStmType = (STREAM_TYPE_E)stype;
    PMSSTD_S pstHead = (PMSSTD_S)pHead;
    int s32Ret = 0;
    static UINT32 nAudioTS = 0;
    static UINT16 nAudioTMS = 0;
    int nTDifValue = 0;
    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);

    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;

    if (!pPlatCtx->m_pstAudioRb || !pPlatCtx->m_pstTalkAudioRb)
    {
        return 0;
    }

    XTRACE_MUTEX_LOCK(&pChCtx->m_AudioLock);
    // 包模式，组数据
    if (STREAM_TYPE_AUDIO_AENC == eStmType)
    {
        if (pSysInfo->m_bAduioInStart && pSysInfo->m_bAudioInited)
        {
            // 通道0音频预览
            if (0 == idx)
            {
                // 跳过头信息，单生产者单消费者无需加锁
                if (RingBuffer_Avail(pPlatCtx->m_pstAudioRb) < size)
                {
                    // ZW_INFO("ringbuf size = %d ; in = %d ; out = %d \n",pPlatCtx->m_pstAudioRb->size,pPlatCtx->m_pstAudioRb->in,pPlatCtx->m_pstAudioRb->out);
                    // ZW_INFO("datalen = %d\n",size);
                    // ZW_INFO("[AUDIO] Audio Ringbuf full, drop data!\n");
                    RingBuffer_Reset(pPlatCtx->m_pstAudioRb);
                    XTRACE_MUTEX_UNLOCK(&pChCtx->m_AudioLock);
                    return 0;
                }
                nTDifValue = (pstHead->TimeSec - nAudioTS) * 1000 + (pstHead->TimeMSec - nAudioTMS);
                if(((nTDifValue > 0) && (nTDifValue > AI_FRM_MIN_INTERVAL))||((nTDifValue < 0) && ((nTDifValue + AI_FRM_MIN_INTERVAL) < 0)))
                {
                    // ZW_INFO("[AUDIO] ts--mts = %d--%d| size = %d  data: %#x %#x %#x %#x\n",
                           // pstHead->TimeSec, pstHead->TimeMSec, size,data[0],data[10],data[20],data[30]);
                    if(CheckAencDataIfNeedClear(pPlatCtx, size, data))
                    {
                        ZW_INFO("[AUDIO] Audio Ringbuf full, drop data!\n");
                    }
                }
                nAudioTS = pstHead->TimeSec;
                nAudioTMS = pstHead->TimeMSec;
            }
            else if (1 == idx) // 通道1对讲
            {
                // 跳过头信息，单生产者单消费者无需加锁
                if (RingBuffer_Avail(pPlatCtx->m_pstTalkAudioRb) < size)
                {
                    ZW_INFO("[AUDIO] Audio Ringbuf full, drop data!\n");
                    RingBuffer_Reset(pPlatCtx->m_pstTalkAudioRb);
                    XTRACE_MUTEX_UNLOCK(&pChCtx->m_AudioLock);
                    return 0;
                }

                s32Ret = RingBuffer_In(pPlatCtx->m_pstTalkAudioRb, data, size);
                if (s32Ret != size)
                {
                    ZW_INFO("[AUDIO] Audio Ringbuf full, incomplete data put, reset it!\n");
                    RingBuffer_Reset(pPlatCtx->m_pstTalkAudioRb);
                }
            }
        }
    }
    else
    {
        ZW_ERROR("Unsupport Stream Type %d\n", eStmType);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_AudioLock);
        return 0;
    }

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_AudioLock);

    return 0;
}

static AUDIO_TYPE_E _AudioType_Convert(AudioEncodeMode_t eAudioEncType)
{
    AUDIO_TYPE_E eAudioTyp= AUDIO_TYPE_BUTT;

    switch (eAudioEncType)
    {
    case eCodecG711U:
        eAudioTyp = AUDIO_G711U;
        break;

    case eCodecG711A:
        eAudioTyp = AUDIO_G711A;
        break;

    default:
        ZW_ERROR("Unsupport AudioEncodeMode_t:%d, SET-TO-DEFAULT G711U\n", eAudioEncType);
        eAudioTyp = AUDIO_G711U;
        break;
    }

    return eAudioTyp;
}

static int audio_ai_init(void *hChannelHandle)
{
    int s32Ret = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    AI_CONF_S stAiCfg = {1, 1, {0}, {0}};
    stAiCfg.PlatCfg.HsPlat.VqeV2Enable = 1;
    AUDIO_CONF_S stAiChnConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_BUTT, 320,3};

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    s32Ret = Ai_Open(0, stAiCfg, AUDIO_DEV_CFG_MODE_T);
    if (s32Ret)
    {
        ZW_ERROR("Ai_Open failed with %#x\n", s32Ret);
        return -1;
    }

    s32Ret = Ai_Start(0, 0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ai_Start Failed!\n");
        return -1;
    }

    s32Ret = Ai_Chn_Open(0, AI_CHTYPE_TALK, 0, stAiChnConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ai_Chn_Open Failed!\n");
        return -1;
    }

    s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, pPlatCtx->m_AudioInVol);
    if (s32Ret < 0)
    {
        ZW_ERROR("[AUDIO] Ai_Set_Volume Failed!\n");
        return -1;
    }

    AencDataNeedClear(pPlatCtx);
    s32Ret = Ai_Set_Gain(0, AUDIO_DEV_AI_TALK, 0, 15);
    if (s32Ret < 0)
    {
        ZW_ERROR("[AUDIO] Ai_Set_Gain Failed!\n");
        return -1;
    }

    s32Ret = Ai_Chn_Set_Aec(0, 0, XRS_FALSE);
    return 0;
}

static int audio_aenc_init(void *hChannelHandle)
{
    int s32Ret = 0;
    CChannelContext *pChCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);

    s32Ret = Aenc_Open(2, (VData_Callback)_AudioData_Callback, NULL);
    if (s32Ret)
    {
         ZW_ERROR("Aenc_Open failed with %#x\n", s32Ret);
         return -1;
    }

    s32Ret = Aenc_Start(0, 0);
    if (s32Ret)
    {
         ZW_ERROR("[AUDIO] Aenc_Start Failed!\n");
         return -1;
    }

    // 音频预览编码格式随上层设置
    s32Ret = Aenc_Chn_Open(0, 0, _AudioType_Convert(pChCtx->m_AudioCodec),320);
    if (s32Ret)
    {
         ZW_ERROR("[AUDIO] Aenc_Chn_Open Ch0 Failed!\n");
         return -1;
    }

     // 对讲固定使用G711U
    s32Ret = Aenc_Chn_Open(0, 1, AUDIO_G711U,320);
    if (s32Ret)
    {
         ZW_ERROR("[AUDIO] Aenc_Chn_Open Ch1 Failed!\n");
         return -1;
    }
    return 0;
}

static int audio_adec_init(void *hChannelHandle)
{
    int s32Ret = 0;
    ADEC_CHN_CONF_S stAdecConf = {0};

    s32Ret = Adec_Open(0, 1);
    if (s32Ret)
    {
        ZW_ERROR("Ao_Open failed with %#x\n", s32Ret);
        return -1;
    }

    s32Ret = Adec_Start(0, 0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Start Failed!\n");
        return -1;
    }

    //音频解码目前只用G711U
    stAdecConf.AudioType = AUDIO_G711U;
    stAdecConf.AdecSMode = ADEC_SMODE_FRAME;
    s32Ret = Adec_Chn_Open(0, 0, stAdecConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Adec_Chn_Open Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_ao_init(void *hChannelHandle)
{
    int s32Ret = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    AUDIO_CONF_S stAoConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 320, 3};

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    s32Ret = Ao_Open(0, stAoConf, AUDIO_DEV_CFG_MODE_T);
    if (s32Ret)
    {
        ZW_ERROR("Ao_Open failed with %#x\n", s32Ret);
        return -1;
    }

    s32Ret = Ao_Start(0, 0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Start Failed!\n");
        return -1;
    }

    s32Ret = Ao_Set_Volume(0, AO_DEV_CVBS, pPlatCtx->m_AudioOutVol);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Set_Volume Failed!\n");
        return -1;
    }

    s32Ret = Ao_Set_Gain(0, AO_DEV_CVBS, 25);
    if (s32Ret < 0)
    {
        ZW_ERROR("[AUDIO] Ao_Set_Gain Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_ai_unInit()
{
    int s32Ret = 0;

    s32Ret = Ai_Chn_Close(0, AI_CHTYPE_TALK, 0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ai_Chn_Close Failed!\n");
        return -1;
    }

    s32Ret = Ai_Stop(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ai_Chn_Close Failed!\n");
        return -1;
    }

    s32Ret = Ai_Close(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ai_Chn_Close Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_ao_unInit()
{
    int s32Ret = 0;

    s32Ret = Ao_Stop(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Chn_Close Failed!\n");
        return -1;
    }

    s32Ret = Ao_Close(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Chn_Close Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_adec_unInit()
{
    int s32Ret = 0;

    s32Ret = Adec_Chn_Close(0, 0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Adec_Chn_Close Failed!\n");
        return -1;
    }

    s32Ret = Adec_Stop(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Adec_Stop Failed!\n");
        return -1;
    }

    s32Ret = Adec_Close(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Adec_Close Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_aenc_unInit()
{
    int s32Ret = 0;

    s32Ret = Aenc_Chn_Close(0, 0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Chn_Close Ch0 Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Chn_Close(0, 1);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Chn_Close Ch1 Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Stop(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Stop Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Close(0);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Close Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_bind()
{
    int s32Ret = 0;
    AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};
    AENC_BIND_CONF_S stAeBindConf = {0, AUDIO_DEV_AI_TALK, 0, 0};
    AENC_BIND_CONF_S stAeTalkBindConf = {0, AUDIO_DEV_AI_TALK, 1, 0};

    s32Ret = Ao_Chn_Bind(0, stAoBindConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Chn_Bind Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Chn_Bind(0, stAeBindConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Chn_Bind Ch0 Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Chn_Bind(0, stAeTalkBindConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Chn_Bind Ch1 Failed!\n");
        return -1;
    }

    return 0;
}

static int audio_unbind()
{
    int s32Ret = 0;
    AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};
    AENC_BIND_CONF_S stAeBindConf = {0, AUDIO_DEV_AI_TALK, 0, 0};
    AENC_BIND_CONF_S stAeTalkBindConf = {0, AUDIO_DEV_AI_TALK, 1, 0};

    s32Ret = Ao_Chn_UnBind(0, stAoBindConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Ao_Chn_UnBind Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Chn_UnBind(0, stAeBindConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Chn_UnBind Ch0 Failed!\n");
        return -1;
    }

    s32Ret = Aenc_Chn_UnBind(0, stAeTalkBindConf);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] Aenc_Chn_UnBind Ch1 Failed!\n");
        return -1;
    }

    return 0;
}

/**
 * @description: 音频初始化
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {int} 0 - 成功，非0 - 错误
 */
int Hi_MPP_AudioInit(void *hChannelHandle)
{
    int s32Ret = 0;

    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext*)hChannelHandle;

    // 如果音频已经初始化了，则直接返回成功
    if (TRUE == pSysInfo->m_bAudioInited)
        return 0;

    ZW_INFO("[AUDIO] Start Init Audio!\n");

    if (!pPlatCtx->m_pstAudioRb)
    {
        pPlatCtx->m_pstAudioRb = RingBuffer_Malloc(4096);
        if (!pPlatCtx->m_pstAudioRb)
        {
            ZW_ERROR("RingBuffer_Malloc for audio-in failed!\n");
        }
    }

    if (!pPlatCtx->m_pstTalkAudioRb)
    {
        pPlatCtx->m_pstTalkAudioRb = RingBuffer_Malloc(4096);
        if (!pPlatCtx->m_pstTalkAudioRb)
        {
            ZW_ERROR("RingBuffer_Malloc for audio-in failed!\n");
        }
    }

    s32Ret = audio_ai_init(pPlatCtx);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_ai_init Failed!\n");
        return -1;
    }

    s32Ret = audio_aenc_init(pPlatCtx);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_aenc_init Failed!\n");
        return -1;
    }

    s32Ret = audio_ao_init(pPlatCtx);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_ao_init Failed!\n");
        return -1;
    }

    s32Ret = audio_adec_init(pPlatCtx);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_adec_init Failed!\n");
        return -1;
    }

    s32Ret = audio_bind();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_bind Failed!\n");
        return -1;
    }

    pSysInfo->m_bAudioInited = TRUE;

    ZW_INFO("[AUDIO] Init Finished!\n");

    return 0;
}

int Hi_MPP_AudioUninit(CPlatformChannelContext *pPlatCtx)
{
    int s32Ret = 0;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 如果音频未初始化，直接返回*/
    if (pSysInfo->m_bAudioInited == FALSE)
        return 0;

    ZW_INFO("START Hi_MPP_AudioUninit!\n");

    s32Ret = audio_unbind();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_unbind Failed!\n");
        return -1;
    }

    s32Ret = audio_ai_unInit();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_ai_unInit Failed!\n");
        return -1;
    }

    s32Ret = audio_aenc_unInit();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_aenc_unInit Failed!\n");
        return -1;
    }

    s32Ret = audio_ao_unInit();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_ao_unInit Failed!\n");
        return -1;
    }

    s32Ret = audio_adec_unInit();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_adec_unInit Failed!\n");
        return -1;
    }

    if (pPlatCtx->m_pstAudioRb)
    {
        RingBuffer_Free(pPlatCtx->m_pstAudioRb);
        pPlatCtx->m_pstAudioRb = NULL;
    }

    if (pPlatCtx->m_pstTalkAudioRb)
    {
        RingBuffer_Free(pPlatCtx->m_pstTalkAudioRb);
        pPlatCtx->m_pstTalkAudioRb = NULL;
    }

    pSysInfo->m_bAudioInited = FALSE;

    ZW_INFO("Hi_MPP_AudioUninit SUCCESS!\n");

    return s32Ret;

    return 0;
}

/**
 * @brief  修改音频编码格式
 * @param  pPlatCtx
 * @return int
 * @note   修改音频编码格式不需要初始化及反初始化AI、AO，AO反初始化时间较长
 */
int Hi_MPP_ChangeAudioEnc(CPlatformChannelContext *pPlatCtx)
{
    int s32Ret = 0;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 如果音频未初始化，直接返回*/
    if (pSysInfo->m_bAudioInited == FALSE)
        return 0;

    ZW_INFO("START Hi_MPP_ChangeAudioEnc!\n");

    s32Ret = audio_unbind();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_unbind Failed!\n");
        return -1;
    }

    s32Ret = audio_aenc_unInit();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_aenc_unInit Failed!\n");
        return -1;
    }

    s32Ret = audio_adec_unInit();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_adec_unInit Failed!\n");
        return -1;
    }

    s32Ret = audio_aenc_init(pPlatCtx);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_aenc_init Failed!\n");
        return -1;
    }

    s32Ret = audio_adec_init(pPlatCtx);
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_adec_init Failed!\n");
        return -1;
    }

    s32Ret = audio_bind();
    if (s32Ret)
    {
        ZW_ERROR("[AUDIO] audio_bind Failed!\n");
        return -1;
    }

    ZW_INFO("Hi_MPP_ChangeAudioEnc SUCCESS!\n");

    return s32Ret;
}

/* 编码反初始化 */
int Hi_MPP_DeInit(void)
{
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);
    SINT32 s32Ret = 0;

    s32Ret = Visp_Stop_SceneAuto(0);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Visp_Stop_SceneAuto fail");

#if defined(__IS_SUPPORT_LCD_MANAGER__)
    s32Ret = Xrs_Lcd_StopVdec();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Xrs_Lcd_StopVdec fail");

    s32Ret = Xrs_Lcd_StopLCD();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Xrs_Lcd_StopLCD fail");
#endif

    /*编码模块反初始化*/
    s32Ret = Hi_MPP_DeInitVdaChn();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitVdaChn fail");

    s32Ret = Hi_MPP_DeInitEncChn();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitEncChn fail");

    s32Ret = Hi_MPP_DeInitVenc();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitVenc fail");

    s32Ret = Hi_MPP_DeInitVpss();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitVpss fail");

    s32Ret = Hi_MPP_DeInitVda();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitVda fail");

    s32Ret = Hi_MPP_DeInitOsd();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitOsd fail");

    if (false == pChCtx->m_bChangeCodePara)
    {
        s32Ret = Hi_MPP_DeInitIsp();
        DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitIsp fail");

        s32Ret = Hi_MPP_DeInitVi();
        DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInitVi fail");
    }

    ZW_DEBUG("Hi_MPP_DeInit SUCCESS\n");
    return s32Ret;
}

/**
 * @description: 将上层的编码类型转换成平台的类型
 * @param {UINT32} srcEncType上层设置的编码类型
 * @param {UINT32} *desEncType平台的编码类型
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ConverEncTypeToPlatform(UINT32 srcEncType, UINT32 *desEncType)
{
    *desEncType = VPAYLOAD_TYPE_H264;
    switch(srcEncType)
    {
        case LtH264Video:
        case LtH264PlusVideo:
            *desEncType = VPAYLOAD_TYPE_H264;
            break;
        case LtH265Video:
        case LtH265PlusVideo:
            *desEncType = VPAYLOAD_TYPE_H265;
            break;
        case LtMjpegVideo:
            *desEncType = VPAYLOAD_TYPE_MJPEG;
            break;
        default :
            return -1;
    }

    return 0;
}

/**
********************************************************************************
 * @函数功能: 设置超大帧丢帧策略（设置编码参数后需要调一下）
 * @输入参数: hChannelHandle:通道句柄
 *           ChnType:通道类型
 * 			 VencChn:编码通道
 * @输出参数: 无
 * @返回值  : 0:成功
 *           非0:失败
********************************************************************************
*/
SINT32 Hi_MPP_Set_SuperFrameStrategy(VENC_CHNTYPE_E ChnType, SINT32 VencChn)
{
    SINT32 s32Ret = 0;
    VENC_SUPER_FRAME_STRATEGY_S Strategy = {0};

    //设置超大帧丢帧策略接口
    Strategy.super_frame_mode = VENC_SUPER_FRAME_DISCARD;
    Strategy.reencode_priority = VENC_REENCODE_FRAME_BITS_FIRST;
#if	SYSTEM_SUPPORT_CHANNEL <=2
    if (ChnType == VENC_CHNTYPE_MAIN || ChnType == VENC_CHNTYPE_MAIN_SEC)
#else
    if (ChnType == VENC_CHNTYPE_MAIN || ChnType == VENC_CHNTYPE_MAIN_SEC || ChnType == VENC_CHNTYPE_MAIN_THIRD || ChnType == VENC_CHNTYPE_MAIN_FOURTH)
#endif
    {
        Strategy.i_frame_bits_threshold = (ENC_MAIN_STREAM_BUF_SIZE * 8) * 0.80; //海思底层要达到此帧大小才会触发调整逻辑，所以要比帧大小小
    }
    else
    {
        Strategy.i_frame_bits_threshold = (ENC_SUB_STREAM_BUF_SIZE * 8) * 0.95; //海思底层要达到此帧大小才会触发调整逻辑，所以要比帧大小小
    }
    Strategy.p_frame_bits_threshold = Strategy.i_frame_bits_threshold / 2;
    Strategy.b_frame_bits_threshold = Strategy.p_frame_bits_threshold;

    s32Ret = Venc_Chn_SetSuperFrameStrategy(0, ChnType, VencChn, Strategy);
    if (s32Ret < 0)
    {
        ZW_ERROR("Set failed(%#x) eChnType[%d] VencChn[%d]\n", s32Ret, ChnType, VencChn);
        return -1;
    }

    return 0;
}

int Hi_MPP_SetQPAttr(VENC_CHNTYPE_E ChnType, SINT32 VencChn)
{
    int nRet = 0;
#if 1
    VENC_QP_CONF_V2_S QpAttr = {0};

    nRet = Venc_Chn_Get_QpAttr(0, ChnType, VencChn, &QpAttr);
    if (nRet == 0)
    {
        QpAttr.bEnbaleQp = 1;
        QpAttr.u32MinQpDelta = 1;
        Venc_Chn_Set_QpAttr(0, ChnType, VencChn, QpAttr);
    }
#endif
    return nRet;
}

/**
 * @description: 修改编码的帧率(降帧时不进行帧控, 则编码帧率默认跟随 sensor 帧率; 不降帧时设置回用户的配置)
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} bCtrlFps  是否进行帧控
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ChangeEncFps(void *hChannelHandle, int bCtrlFps)
{
    return 0;
}

/**
 * @description: 改变 sensor 的帧率
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} nChn  通道号
 * @param {FLOAT32} SensorFps  帧率,控制 VI 的帧率
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ChangeSensorFps(void *hChannelHandle, int nChn, FLOAT32 SensorFps)
{
    /* 1114：不支持动态修改VI帧率，待后续HS解决后放开 */
    SINT32 RetVal = 0;
    RetVal = Vi_Chn_Set_Attr(0, nChn, SensorFps);
    g_nCurSensorFrame = SensorFps;
    if (RetVal < 0)
    {
        ZW_ERROR("Vi_Chn_Set_Attr SensorFps=%f nChn %d Failed\n", SensorFps, nChn);
        return -1;
    }
    return 0;
}

/**
 * @brief  统计平均码率
 * @param  hChannelHandle 平台通道句柄
 * @param  eFrameType 码流类型
 * @param  s32FrameSize 帧大小
 * @return int 返回0为正常，返回其他值为异常
 */
int Hi_MPP_GetAvgBitrate(void *hChannelHandle, STREAM_TYPE_E eFrameType, int s32FrameSize)
{
    CPlatformChannelContext *pPlatCtx = NULL;
    static unsigned long long u64StartTime = 0;
    static unsigned long long u64EndTime = 0;
    static unsigned long long u64Bitrate[MAX_STREAM_COUNT][MAX_STATISTIC_TIMES] = {{0}};
    static unsigned int u32CurTime = 0;
    int i = 0;
    int j = 0;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    u64StartTime = get_ms();
    if (u64StartTime - u64EndTime <= 1000)
    {
        u64Bitrate[eFrameType][u32CurTime] += s32FrameSize;
    }
    else
    {
        u64EndTime = u64StartTime;
        u32CurTime++;
        u32CurTime = (u32CurTime >= MAX_STATISTIC_TIMES) ? 0 : u32CurTime;

        // 计算最近10s的码率平均值
        pPlatCtx->m_MppRCInfo[i].m_bit_rate = 0;
        for (i = 0; i < MAX_STREAM_COUNT; i++)
        {
            for (j = 0; j < MAX_STATISTIC_TIMES; j++)
            {
                pPlatCtx->m_MppRCInfo[i].m_bit_rate += u64Bitrate[i][j];
            }
            // 计算平均码率大小，单位KB
            pPlatCtx->m_MppRCInfo[i].m_bit_rate = pPlatCtx->m_MppRCInfo[i].m_bit_rate / MAX_STATISTIC_TIMES / 1024;
            // 清除下一秒将要累加的值
            u64Bitrate[i][u32CurTime] = 0;
        }
    }

    return 0;
}

int Hi_MPP_ReadVideoStream(void *hChannelHandle, VENC_CHNTYPE_E eStmTyp, MSSTD_S *head, char *data, SINT32 size)
{
    CPlatformChannelContext		*pPlatCtx = NULL;
    SystemInfo_t				*pSysInfo = NULL;
    struct timeval	CurTime = {0};
    static ENCODE_FRAME_TYPE_E eFrameType;
    static ENCODE_TYPE_E eEncodeType = ENCODE_TYPE_H264;
    static UINT32 lastFrameInterval[VENC_CHNTYPE_BUTT] = {0};

    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext*)hChannelHandle;

    /* 设备休眠中，推送休眠帧 */
    if (0 != pSysInfo->m_iDormancyState)
    {

#if	SYSTEM_SUPPORT_CHANNEL <=1
        if (VENC_CHNTYPE_SNAP == eStmTyp)
#elif SYSTEM_SUPPORT_CHANNEL <=2
        if (VENC_CHNTYPE_SNAP == eStmTyp ||
        VENC_CHNTYPE_SNAP_SEC == eStmTyp)
#elif	SYSTEM_SUPPORT_CHANNEL <=3

        if (VENC_CHNTYPE_SNAP == eStmTyp ||
        VENC_CHNTYPE_SNAP_SEC == eStmTyp ||
        VENC_CHNTYPE_SNAP_THIRD == eStmTyp)
#elif SYSTEM_SUPPORT_CHANNEL <=4
        if (VENC_CHNTYPE_SNAP == eStmTyp ||
            VENC_CHNTYPE_SNAP_SEC == eStmTyp ||
            VENC_CHNTYPE_SNAP_THIRD == eStmTyp ||
            VENC_CHNTYPE_SNAP_FOURTH == eStmTyp)
#endif
        {
            pPlatCtx->m_stStreamData.bFinish = FALSE;
            return -1;
        }

        if (MSSTD_STREAM_FRAME_VIDEO_I == head->FrameType)
            eFrameType = I_FRAME;
        else
            eFrameType = P_FRAME;

        eEncodeType = ENCODE_TYPE_H265;
        /*写入完整帧*/
        if (eFrameType == I_FRAME && pSysInfo->pDormancyIFrameBuffer != NULL)
        {
            pPlatCtx->m_stStreamData.datasz = pSysInfo->m_DormancyIFrameBufferSize;
            if ((ENC_MAIN_STREAM_BUF_SIZE) < pPlatCtx->m_stStreamData.datasz  + sizeof(H264DATAPACKETHEADER))
            {
                ZW_ERROR("Frame too BIG, drop, buffer size = %d!!\n", pPlatCtx->m_stStreamData.datasz);
                return -1;
            }

#ifdef __USE_CIRCLEQUEUE__
            /* 动态分配，避免拷贝 */
            pSysInfo->pVideoBuffer = malloc(pPlatCtx->m_stStreamData.datasz + sizeof(H264DATAPACKETHEADER));
            if (NULL == pSysInfo->pVideoBuffer)
            {
                ZW_ERROR("pVideoBuffer malloc failed size = %d.\n",pPlatCtx->m_stStreamData.datasz  + sizeof(H264DATAPACKETHEADER));
                return -1;
            }
#endif
            memcpy(pSysInfo->pVideoBuffer + sizeof(H264DATAPACKETHEADER), (void *)(pSysInfo->pDormancyIFrameBuffer), pSysInfo->m_DormancyIFrameBufferSize);
        }
        else if (eFrameType == P_FRAME && pSysInfo->pDormancyPFrameBuffer != NULL)
        {
            pPlatCtx->m_stStreamData.datasz = pSysInfo->m_DormancyPFrameBufferSize;
            if ((ENC_MAIN_STREAM_BUF_SIZE) < pPlatCtx->m_stStreamData.datasz  + sizeof(H264DATAPACKETHEADER))
            {
                ZW_ERROR("Frame too BIG, drop, buffer size = %d!!\n", pPlatCtx->m_stStreamData.datasz);
                return -1;
            }

#ifdef __USE_CIRCLEQUEUE__
            /* 动态分配，避免拷贝 */
            pSysInfo->pVideoBuffer = malloc(pPlatCtx->m_stStreamData.datasz + sizeof(H264DATAPACKETHEADER));
            if (NULL == pSysInfo->pVideoBuffer)
            {
                ZW_ERROR("pVideoBuffer malloc failed size = %d.\n",pPlatCtx->m_stStreamData.datasz  + sizeof(H264DATAPACKETHEADER));
                return -1;
            }
#endif
            memcpy(pSysInfo->pVideoBuffer + sizeof(H264DATAPACKETHEADER), (void *)(pSysInfo->pDormancyPFrameBuffer), pSysInfo->m_DormancyPFrameBufferSize);
        }

        gettimeofday(&CurTime,NULL);
        pPlatCtx->m_stStreamData.TimeSec = head->TimeSec;
        pPlatCtx->m_stStreamData.TimeMSec = head->TimeMSec;
        pPlatCtx->m_stStreamData.SYS_TimeSec = CurTime.tv_sec;
        pPlatCtx->m_stStreamData.SYS_TimeMSec = CurTime.tv_usec / 1000;
        pPlatCtx->m_stStreamData.eStream = eStmTyp;
        pPlatCtx->m_stStreamData.u32VencChn = 0;
        pPlatCtx->m_stStreamData.eFrameType = eFrameType;
        pPlatCtx->m_stStreamData.eStreamType = eEncodeType;
        pPlatCtx->m_stStreamData.framerate = head->FrateRate;
        pPlatCtx->m_stStreamData.data = pSysInfo->pVideoBuffer;
        pPlatCtx->m_stStreamData.bFinish = TRUE;
        pPlatCtx->m_stStreamData.SecFrmType = head->SecFrmType;
    }
    else
    {
#if	SYSTEM_SUPPORT_CHANNEL <=2
        if (VENC_CHNTYPE_MAIN == eStmTyp || VENC_CHNTYPE_MINOR == eStmTyp ||
            VENC_CHNTYPE_MAIN_SEC == eStmTyp || VENC_CHNTYPE_MINOR_SEC == eStmTyp )
#else
        if (VENC_CHNTYPE_MAIN == eStmTyp || VENC_CHNTYPE_MINOR == eStmTyp ||
            VENC_CHNTYPE_MAIN_SEC == eStmTyp || VENC_CHNTYPE_MINOR_SEC == eStmTyp ||
            VENC_CHNTYPE_MAIN_THIRD == eStmTyp || VENC_CHNTYPE_MINOR_THIRD == eStmTyp ||
            VENC_CHNTYPE_MAIN_FOURTH == eStmTyp || VENC_CHNTYPE_MINOR_FOURTH == eStmTyp)
#endif
        {
#if 0
            /* Just For Test */
            Hi_MPP_GetAvgBitrate(hChannelHandle, eStmTyp, size);
#endif

            if (MSSTD_STREAM_FRAME_VIDEO_I == head->FrameType)
                eFrameType = I_FRAME;
            else
                eFrameType = P_FRAME;

            if (MSSTD_ALGORITHM_VIDEO_H265_GENERAL == head->Algorithm)
                eEncodeType = ENCODE_TYPE_H265;
            else if (MSSTD_ALGORITHM_VIDEO_H264_STANDARD == head->Algorithm)
                eEncodeType = ENCODE_TYPE_H264;

            if ((ENC_MAIN_STREAM_BUF_SIZE) < size + sizeof(H264DATAPACKETHEADER))
            {
                ZW_ERROR("Frame too BIG, drop, buffer size = %d!!\n", size);
                return -1;
            }

#ifdef __USE_CIRCLEQUEUE__
            /* 动态分配，避免拷贝 */
            pSysInfo->pVideoBuffer = malloc(size + sizeof(H264DATAPACKETHEADER));
            if (NULL == pSysInfo->pVideoBuffer)
            {
                ZW_ERROR("pVideoBuffer malloc failed size = %d.\n",size + sizeof(H264DATAPACKETHEADER));
                return -1;
            }
#endif
            gettimeofday(&CurTime,NULL);
            memcpy(pSysInfo->pVideoBuffer + sizeof(H264DATAPACKETHEADER), data, size);
            pPlatCtx->m_stStreamData.TimeSec = head->TimeSec;
            pPlatCtx->m_stStreamData.TimeMSec = head->TimeMSec;
            pPlatCtx->m_stStreamData.SYS_TimeSec = CurTime.tv_sec;
            pPlatCtx->m_stStreamData.SYS_TimeMSec = CurTime.tv_usec / 1000;
            pPlatCtx->m_stStreamData.eStream = eStmTyp;
            pPlatCtx->m_stStreamData.u32VencChn = 0;
            pPlatCtx->m_stStreamData.eFrameType = eFrameType;
            pPlatCtx->m_stStreamData.eStreamType = eEncodeType;
            pPlatCtx->m_stStreamData.framerate = head->FrateRate;
            pPlatCtx->m_stStreamData.data = pSysInfo->pVideoBuffer;
            pPlatCtx->m_stStreamData.datasz = size;
            pPlatCtx->m_stStreamData.bFinish = TRUE;
            pPlatCtx->m_stStreamData.SecFrmType = head->SecFrmType;
        }
#if	SYSTEM_SUPPORT_CHANNEL <=2
        else if (VENC_CHNTYPE_SNAP == eStmTyp || VENC_CHNTYPE_SNAP_SEC == eStmTyp )
#else
        else if (VENC_CHNTYPE_SNAP == eStmTyp || VENC_CHNTYPE_SNAP_SEC == eStmTyp ||
             VENC_CHNTYPE_SNAP_THIRD == eStmTyp || VENC_CHNTYPE_SNAP_FOURTH == eStmTyp)
#endif

        {
            if (MSSTD_STREAM_FRAME_VIDEO_I == head->FrameType)
                eFrameType = I_FRAME;
            else
                eFrameType = P_FRAME;

            if (MSSTD_ALGORITHM_VIDEO_JPEG_STANDARD == head->Algorithm)
                eEncodeType = ENCODE_TYPE_MJPEG;
            else
                eEncodeType = ENCODE_TYPE_MJPEG;

            if (pSysInfo->m_MJPEGBufferSize < size + sizeof(H264DATAPACKETHEADER))
            {
                ZW_ERROR("MJPEG Frame too BIG, drop, buffer size = %d, needed = %d !!\n", pSysInfo->m_MJPEGBufferSize, size);
                return -1;
            }

#ifdef __USE_CIRCLEQUEUE__
            //动态分配，避免拷贝
            pSysInfo->pVideoBuffer = malloc(size + sizeof(H264DATAPACKETHEADER));
            if (NULL == pSysInfo->pVideoBuffer)
            {
                ZW_ERROR("pVideoBuffer malloc failed size = %d.\n",size + sizeof(H264DATAPACKETHEADER));
                return -1;
            }
#endif
            gettimeofday(&CurTime,NULL);
            memcpy(pSysInfo->pVideoBuffer + sizeof(H264DATAPACKETHEADER), data, size);
            pPlatCtx->m_stStreamData.TimeSec = head->TimeSec;
            pPlatCtx->m_stStreamData.TimeMSec = head->TimeMSec;
            pPlatCtx->m_stStreamData.SYS_TimeSec = CurTime.tv_sec;
            pPlatCtx->m_stStreamData.SYS_TimeMSec = CurTime.tv_usec / 1000;
            pPlatCtx->m_stStreamData.eStream = eStmTyp;
            pPlatCtx->m_stStreamData.u32VencChn = 0;
            pPlatCtx->m_stStreamData.eFrameType = eFrameType;
            pPlatCtx->m_stStreamData.eStreamType = eEncodeType;
            pPlatCtx->m_stStreamData.framerate = head->FrateRate;
            pPlatCtx->m_stStreamData.data = pSysInfo->pVideoBuffer;
            pPlatCtx->m_stStreamData.datasz = size;
            pPlatCtx->m_stStreamData.bFinish = TRUE;
            pPlatCtx->m_stStreamData.SecFrmType = head->SecFrmType;
        }
        else
        {
            pPlatCtx->m_stStreamData.bFinish = FALSE;
            return -1;
        }
    }
    pPlatCtx->m_stStreamData.u32FrameInterval = pPlatCtx->m_stStreamData.TimeSec * 1000 + pPlatCtx->m_stStreamData.TimeMSec - lastFrameInterval[eStmTyp];
    lastFrameInterval[eStmTyp] = pPlatCtx->m_stStreamData.TimeSec * 1000 + pPlatCtx->m_stStreamData.TimeMSec;

    return 0;
}

/* 读取流回调 */
int Hi_MPP_ReadStream(void *hChannelHandle,
                             char **pFirstDataBuf,
                               int  *pFirstLength,
                               int  *pRealTime,
                               int  *pRealTime_Usec,
                               int  *pFrameType,
                             int  encType,
                               int  stream,
                               int  SensorIndex)
{
    struct timeval	CurTime;
    H264DATAPACKETHEADER FrameHead;
    CPlatformChannelContext *pPlatCtx;
    SystemInfo_t		    *pSysInfo;
    XRS_StreamData		    *pstStreamData;
    SINT32 RealTime, RealTime_USec, MJPEGGetDateSleep;
    static int MJPEGGetDateSleepBak = 0;
    static int MJPEGtDateSize = 0;
    static int MJPEGtDateSizeBak = 0;
#if (MAX_CH_NUM == 1)
    static UINT32 inValue[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT] = {{0,0,0,0}};
#elif (MAX_CH_NUM == 2)
    static UINT32 inValue[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT] = {{0,0,0,0},{0,0,0,0}};
#elif (MAX_CH_NUM == 3)
    static UINT32 inValue[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT] = {{0,0,0,0},{0,0,0,0},{0,0,0,0}};
#elif (MAX_CH_NUM == 4)
    static UINT32 inValue[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
#endif

    memset(&FrameHead, 0, sizeof(FrameHead));
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    HY_Res_GetSystemInfo(&pSysInfo);
    pstStreamData = &(pPlatCtx->m_stStreamData);

    /* 如果是人形帧，则无需从底层获取，直接用之前jpg流缓存的图片 */
    if ((PktIAFrames == *pFrameType))
    {
        goto doAIFrame;
    }

    /* 获取帧类型, 计算时间(显示一帧数据需要的时间) */
    FrameHead.uDataPacketStartCode	= FRAME_START_CODE;

    switch(pstStreamData->eStreamType)
    {
        case ENCODE_TYPE_H264: 	FrameHead.byLoadType = H264; break;
        case ENCODE_TYPE_H265: 	FrameHead.byLoadType = H265; break;
        case ENCODE_TYPE_MJPEG: FrameHead.byLoadType = MJPEG; break;
        default: break;
    }

    /* main stream */
    if (FIRST_STREAM == stream)
    {
        inValue[SensorIndex][stream] += 90000 * pstStreamData->u32FrameInterval / 1000;
        FrameHead.uShowTime = inValue[SensorIndex][stream];
        FrameHead.uShowTime |= 1;

        mainStreamShowTime[SensorIndex] = FrameHead.uShowTime;
    }
    else
    {
        if(g_subSyncFlag[SensorIndex] && SECOND_STREAM == stream)	//每秒同步一次子码流和主码流的showtime
        {
            if (mainStreamShowTime[SensorIndex] > inValue[SensorIndex][stream])
                inValue[SensorIndex][stream] = mainStreamShowTime[SensorIndex];
            else
                inValue[SensorIndex][stream] += 1;

            g_subSyncFlag[SensorIndex] = 0;
        }
        else
        {
            inValue[SensorIndex][stream] += 90000 * pstStreamData->u32FrameInterval / 1000;
        }

        FrameHead.uShowTime = inValue[SensorIndex][stream];
        FrameHead.uShowTime |= 1;
    }

    switch(stream)
    {
        case FIRST_STREAM:
            FrameHead.uFrameNo = pPlatCtx->m_FrameNo[SensorIndex] + 1;
            pPlatCtx->m_FrameNo[SensorIndex] = FrameHead.uFrameNo;
            FrameHead.uReserved = 0;
            break;
        case SECOND_STREAM:
            FrameHead.uFrameNo = pPlatCtx->m_FrameNoAux[SensorIndex] + 1;
            pPlatCtx->m_FrameNoAux[SensorIndex] = FrameHead.uFrameNo;
            FrameHead.uReserved = 0;
            break;
        case FOURTH_STREAM:
            FrameHead.uFrameNo = pPlatCtx->m_FrameNoFourth[SensorIndex] + 1;
            pPlatCtx->m_FrameNoFourth[SensorIndex] = FrameHead.uFrameNo;
            break;
        default:
            break;
    }
#if (SYSTEM_SUPPORT_CHANNEL >= 2)

    if (SensorIndex == SEC_SENSOR)
    {
        FrameHead.uReserved = 1;
    }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    else if (SensorIndex == THIRD_SENSOR)
    {
        FrameHead.uReserved = 2;
    }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
    else if (SensorIndex == FOURTH_SENSOR)
    {
        FrameHead.uReserved = 3;
    }
#endif

    if(pstStreamData->SecFrmType == MSSTD_STREAM_FRAME_IFRM_VIRT)
    {
        FrameHead.uReserved += SMARTP_VIFrame;
    }

    FrameHead.uOverloadLen = pstStreamData->datasz;
    FrameHead.byFrameType = (I_FRAME == pstStreamData->eFrameType)?PktIFrames:PktPFrames;

doAIFrame:
    gettimeofday(&CurTime,NULL);
    RealTime = CurTime.tv_sec;
    RealTime_USec = CurTime.tv_usec;

    if (PktIAFrames != *pFrameType)
    {
        //视频数据已经在 Hi_MPP_ReadVideoStream 填充，后面不需要填充头数据
        FrameHead.u32RealTimeSec = RealTime;
        FrameHead.u32RealTimeUSec = RealTime_USec;
        memcpy(pstStreamData->data, (char*)&FrameHead, sizeof(H264DATAPACKETHEADER));

        *pFirstDataBuf = pstStreamData->data;
        *pFirstLength = FrameHead.uOverloadLen + sizeof(H264DATAPACKETHEADER);
        *pFrameType = FrameHead.byFrameType;
        *pRealTime = RealTime;
        *pRealTime_Usec = RealTime_USec;

        //每秒从MJPEG流拷贝一帧到人形缓存
        MJPEGGetDateSleep = HY_Res_SDK_GetTimeSinceBoot();
        if(FOURTH_STREAM == stream && ((MJPEGGetDateSleep - MJPEGGetDateSleepBak) >= 1 || (0 == MJPEGGetDateSleepBak)))
        {
            MJPEGtDateSize = pstStreamData->datasz;

            if (pSysInfo->m_MJPEGBufferSize >= pstStreamData->datasz)
            {
                /* 拷贝数据需要跳过保留的头数据 */
                memset(pSysInfo->pMJPEGBuffer, 0, pSysInfo->m_MJPEGBufferSize);
                memcpy(pSysInfo->pMJPEGBuffer, pstStreamData->data+sizeof(H264DATAPACKETHEADER), pstStreamData->datasz);
                MJPEGGetDateSleepBak = MJPEGGetDateSleep;
                MJPEGtDateSizeBak = MJPEGtDateSize;
            }
            else
            {
                MJPEGtDateSize = MJPEGtDateSizeBak;
                ZW_INFO("MJPEGtDateSize %d \n", pstStreamData->datasz);
            }
        }
    }
    else
    {
        *pFirstLength = MJPEGtDateSize;
        *pFirstDataBuf = pSysInfo->pMJPEGBuffer;
        *pRealTime = RealTime;
        *pRealTime_Usec = RealTime_USec;
    }

    return 0;
}

 //!\brief 获取当前需要跟踪的目标；选取一个最大人形跟踪直到消失，再重新选取下个目标跟踪
 //!\praram in srcResult 算法给的所有目标
 //!\praram out maxResult 需要跟踪的目标
 //!\return 当前最大的人形的面积
int Hi_MPP_GetMaxTargetResult(void *hChannelHandle, human_struct *srcResult, human_struct *maxResult)
{
    int i = 0;
    int j = 0;
    int maxResultIndex = 0;
    int rectMax = 0;
    int rectNow = 0;
    RECT_S rectTemp = {0};
    int trackTemp = 0;
    int moveTemp = 0;

    //1. 框(按面积)降序排列
    if (srcResult->num > 1)
    {
        for (i = 1; i <= srcResult->num - 1; i++)
        {
            for (j = 0; j <= srcResult->num - 1 - i; j++)
            {
                rectMax = srcResult->rectInfo[j].u32Width * srcResult->rectInfo[j].u32Height;
                rectNow = srcResult->rectInfo[j+1].u32Width * srcResult->rectInfo[j+1].u32Height;
                if(rectMax < rectNow)
                {
                    rectTemp = srcResult->rectInfo[j];
                    trackTemp = srcResult->trackId[j];
                    moveTemp = srcResult->IsMove[j];

                    srcResult->rectInfo[j] = srcResult->rectInfo[j+1];
                    srcResult->trackId[j] = srcResult->trackId[j+1];
                    srcResult->IsMove[j] = srcResult->IsMove[j+1];

                    srcResult->rectInfo[j+1] = rectTemp;
                    srcResult->trackId[j+1] = trackTemp;
                    srcResult->IsMove[j+1] = moveTemp;
                }
            }
        }
    }

#if 0	//暂时不做跟track逻辑，因track易断。直接跟面积最大的
    // 2. 找需要跟的目标。
    // 2.1跟过。找到正在跟的track，匹配到则跟。
    if ((0 != maxResult->rectInfo[maxResultIndex].u32Width) && (0 != maxResult->rectInfo[maxResultIndex].u32Height)
        && (0 != maxResult->rectInfo[maxResultIndex].s32X) && (0 != maxResult->rectInfo[maxResultIndex].s32Y))
    {
          for (i = 0; i < srcResult->num; i++)
          {
            if (srcResult->trackId[i] == maxResult->trackId[maxResultIndex])
            {
                maxResult->rectInfo[maxResultIndex].s32X = srcResult->rectInfo[i].s32X;
                maxResult->rectInfo[maxResultIndex].s32Y = srcResult->rectInfo[i].s32Y;
                maxResult->rectInfo[maxResultIndex].u32Width = srcResult->rectInfo[i].u32Width;
                maxResult->rectInfo[maxResultIndex].u32Height = srcResult->rectInfo[i].u32Height;

                maxResult->valid = srcResult->valid;
                //只会有一个结果
                maxResult->num = 1;

                goto RET;
            }
        }
    }
    else //2.2 首次跟，找到需要跟的track。分先围栏/越界触发的跟踪；还是仅有跟踪功能。
    {
        // 2.2.1 围栏/越界触发的跟踪
        if (pPlatChCtx->m_coexistEventTrack.trackNum)
        {
            for (i = 0; i < srcResult->num; i++)

            {
                for (j = 0; j < pPlatChCtx->m_coexistEventTrack.trackNum; j++)
                {
                    if (srcResult->trackId[i] == pPlatChCtx->m_coexistEventTrack.trackId[j])
                    {
                        maxResult->rectInfo[maxResultIndex].s32X = srcResult->rectInfo[i].s32X;
                        maxResult->rectInfo[maxResultIndex].s32Y = srcResult->rectInfo[i].s32Y;
                        maxResult->rectInfo[maxResultIndex].u32Width = srcResult->rectInfo[i].u32Width;
                        maxResult->rectInfo[maxResultIndex].u32Height = srcResult->rectInfo[i].u32Height;
                        maxResult->trackId[maxResultIndex] =  srcResult->trackId[i];

                        //重置 围栏/越界的事件数据
                        memset(&pPlatChCtx->m_coexistEventTrack, 0, sizeof(coexistEventTrack_struct));

                        maxResult->valid = srcResult->valid;
                        //只会有一个结果
                        maxResult->num = 1;

                        goto RET;
                    }
                }
            }
        }
        else//2.2.2 只开了跟踪功能，第一次直接用当前最大的
        {
            maxResult->rectInfo[maxResultIndex].s32X = srcResult->rectInfo[maxResultIndex].s32X;
            maxResult->rectInfo[maxResultIndex].s32Y = srcResult->rectInfo[maxResultIndex].s32Y;
            maxResult->rectInfo[maxResultIndex].u32Width = srcResult->rectInfo[maxResultIndex].u32Width;
            maxResult->rectInfo[maxResultIndex].u32Height = srcResult->rectInfo[maxResultIndex].u32Height;
            maxResult->trackId[maxResultIndex] =  srcResult->trackId[maxResultIndex];
        }
    }

    //3. track没找到
    maxResult->num = 0;

RET:
#endif
    //2. 直接跟当前目标中面积最大的
    maxResult->rectInfo[maxResultIndex].s32X = srcResult->rectInfo[maxResultIndex].s32X;
    maxResult->rectInfo[maxResultIndex].s32Y = srcResult->rectInfo[maxResultIndex].s32Y;
    maxResult->rectInfo[maxResultIndex].u32Width = srcResult->rectInfo[maxResultIndex].u32Width;
    maxResult->rectInfo[maxResultIndex].u32Height = srcResult->rectInfo[maxResultIndex].u32Height;
    maxResult->trackId[maxResultIndex] =  srcResult->trackId[maxResultIndex];
    maxResult->IsMove[maxResultIndex] =  srcResult->IsMove[maxResultIndex];

    maxResult->valid = srcResult->valid;
    //只会有一个结果或者没结果
    if (srcResult->num > 0)
    {
        maxResult->num = 1;
    }
    else
    {
        maxResult->num = 0;
    }

    return srcResult->rectInfo[maxResultIndex].u32Width * srcResult->rectInfo[maxResultIndex].u32Height;
}

int Hi_MPP_SetAudioInVol(void *hChannelHandle, int vol, int IsClearCache, int m_IntercomMode)
{
    SINT32 s32Ret = 0;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    SystemInfo_t *pSysInfo = NULL;

    int ProductTest = HY_Enc_GetProductMode();

    HY_Res_GetSystemInfo(&pSysInfo);
    //m_IntercomMode :2为半双工对讲时，需要屏蔽MIC，防止对讲结束的回声
    if (m_IntercomMode == 2 && ProductTest ==0)
    {
        vol = 0;
        if ((s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, vol)) < 0)
        {
            ZW_ERROR("[AUDIO] Ai_Set_Volume Failed!\n");
            return -1;
        }
        return 0;
    }
    if (vol < 0 || vol > 100)
    {
        return -1;
    }

    if (FALSE == pSysInfo->m_bAudioInited)
    {
        return 0;
    }

    pPlatCtx->m_AudioInVol = vol;

    ZW_INFO("m_IntercomMode = %d m_AudioInVol = %d\n",m_IntercomMode, pPlatCtx->m_AudioInVol);

    if (vol == 0)
    {
        if ((s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, vol)) < 0)
        {
            ZW_ERROR("[AUDIO] Ai_Set_Volume Failed!\n");
            return -1;
        }
        return 0;
    }
    //半双工MIC音量映射逻辑
    if (m_IntercomMode == 0)
    {
        /* 恢复AI音量 */
        if (vol != 0)
        {
            /* 90 以上线性映射 90 到 100 之间的值，确保默认值 90 对应最优效果 95*/
            if (vol >= 90)
            {
                vol = (int)(round(0.5 * (vol - 90) + pPlatCtx->m_HalfAudioInVol));
            }
        }

    }
    //全双工MIC音量映射逻辑
    else if(m_IntercomMode == 1)
    {
        /* 恢复AI音量 */
        if (vol != 0)
        {
            vol = (int)(round(0.5 * (pPlatCtx->m_AudioInVol - 90) + pPlatCtx->m_FullAudioInVol));
        }
    }
    if (vol < 60) vol = 60;
    if (vol > 100) vol = 100;
    ZW_DEBUG("[AUDIO] audio int vol[%d] Full[%d] Half[%d] \n",vol,pPlatCtx->m_FullAudioInVol,pPlatCtx->m_HalfAudioInVol);
    if ((s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, vol)) < 0)
    {
        ZW_ERROR("[AUDIO] Ai_Set_Volume Failed!\n");
        return -1;
    }
    if (1 == IsClearCache)
    {
        AencDataNeedClear(pPlatCtx);
    }

    ZW_DEBUG("[AUDIO] Hi_MPP_SetAudioInVol m_IntercomMode[%d] vol=[%d]\n", m_IntercomMode, vol);
    return 0;
}

/**
********************************************************************************
 * @函数功能: 因海思AI降噪算法对音量调整后会有几秒降帧失效的问题，所以调整音量后要清除掉数据
 * @作者    : HL.LIU
 * @param[in] *pPlatCtx
 * @返回值  :
 * @其他    :
********************************************************************************
*/
static int AencDataNeedClear(CPlatformChannelContext *pPlatCtx)
{
    int Second = 3;//需要清除数据的秒数
    AUDIO_SAMPLE_RATE_E currSampleRate = XRS_AUDIO_SAMPLE_RATE_8000;
    if(NULL == pPlatCtx)
    {
        ZW_ERROR("AencDataNeedClear param is NULL");
        return -1;
    }
    //TODO:关联实际生效的配置
    switch(currSampleRate)
    {
        case XRS_AUDIO_SAMPLE_RATE_8000:
            pPlatCtx->m_AIAudioDataNeadClearSize = 8000 * Second;
            break;

        case XRS_AUDIO_SAMPLE_RATE_16000:
            pPlatCtx->m_AIAudioDataNeadClearSize = 16000 * Second;
            break;

        default:
            pPlatCtx->m_AIAudioDataNeadClearSize = 8000 * Second;
    }

    //暂不清数据
    //pPlatCtx->m_AIAudioDataNeadClearSize = 0;

    ZW_DEBUG("m_AIAudioDataNeadClearSize = %d\n", pPlatCtx->m_AIAudioDataNeadClearSize);

    return 0;
}

static int CheckAencDataIfNeedClear(CPlatformChannelContext *pPlatCtx, SINT32 size, SINT8 *data)
{
    CChannelContext 	*pChCtx = NULL;
    int s32Ret = 0;
    if(NULL == pPlatCtx || NULL == data)
    {
        ZW_ERROR("CheckAencDataIfNeedClear param is NULL");
        return -1;
    }
    HY_Enc_GetEncChInfo(&pChCtx);
    if(pPlatCtx->m_AIAudioDataNeadClearSize > 0)
    {
        SINT8 *pBuff = malloc(size);

        if(NULL != pBuff)
        {
            if (eCodecG711U == pChCtx->m_AudioCodec)
            {
                memset(pBuff, 0xff, size);
            }
            else
            {
                memset(pBuff, 0x0, size);
            }
            s32Ret = RingBuffer_In(pPlatCtx->m_pstAudioRb, pBuff, size);
            if (s32Ret != size)
            {
                ZW_INFO("[AUDIO] Audio Ringbuf full, incomplete data put, reset it!\n");
                RingBuffer_Reset(pPlatCtx->m_pstAudioRb);
            }
            free(pBuff);
        }
        pPlatCtx->m_AIAudioDataNeadClearSize -= size;
    }
    else
    {
        s32Ret = RingBuffer_In(pPlatCtx->m_pstAudioRb, data, size);
        if (s32Ret != size)
        {
            ZW_INFO("[AUDIO] Audio Ringbuf full, incomplete data put, reset it!\n");
            RingBuffer_Reset(pPlatCtx->m_pstAudioRb);
        }
    }
    return 0;
}

/*** 对讲模式切换不更新音量缓存 ***/
int Hi_MPP_SetAudioOutVol(void *hChannelHandle, int vol, int IsUpdateVoiceCache, int m_IntercomMode)
{
    SINT32	s32Ret = 0;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);

    if (vol < 0 || vol > 100)
        return -1;

    if (FALSE == pSysInfo->m_bAudioInited)
        return 0;

    if (1 == IsUpdateVoiceCache)
    {
        printf("Hi_MPP_SetAudioOutVol vol = %d\n",vol);
        pPlatCtx->m_AudioOutVol = vol;
    }

    if (0 == vol)
    {
        if ((s32Ret = Ao_Set_Mute(0, AO_DEV_CVBS, XRS_TRUE)) < 0)
        {
            ZW_ERROR("[AUDIO] Ao_Set_Mute Failed!\n");
            return -1;
        }
        return 0;
    }
    /* 半双工对讲 */
    if(m_IntercomMode == 0)
    {
        if (vol != 0)
        {
            vol = (int)(round(0.32 *(vol-95) + pPlatCtx->m_HalfAudioOutVol));
        }
        /* 海思SDK喇叭音量60以下，听不到设备发声，故作此映射 */
        /* 保证音量值在 60 到 100 之间 */
        vol = ((vol < 60) ? 60 : vol);
        vol = ((vol > 100) ? 100 : vol);
    }
    /* 全双工对讲 */
    else if (m_IntercomMode == 1)
    {
            /* 降低AO音量 */
        vol = (int)(round(0.32 *(vol-95) + pPlatCtx->m_FullAudioOutVol));
        /* 海思SDK喇叭音量60以下，听不到设备发声，故作此映射 */
        /* 保证音量值在 60 到 100 之间 */
        vol = ((vol < 60) ? 60 : vol);
        vol = ((vol > 100) ? 100 : vol);
    }
    /*声音报警映射*/
    else if(m_IntercomMode == 2)
    {
        /* 音量 >= 80 或为 0 时保持原值，否则映射到 60-80 范围 */
        if (vol > 0 && vol < 80)
        {
            vol = (int)(round(60 + vol * 20.0 / 80));
        }
    }

    ZW_DEBUG("[AUDIO] audio out vol[%d] Full[%d] Half[%d]\n",vol,pPlatCtx->m_FullAudioOutVol,pPlatCtx->m_HalfAudioOutVol);

    if ((s32Ret = Ao_Set_Mute(0, AO_DEV_CVBS, XRS_FALSE)) < 0)
    {
        ZW_ERROR("[AUDIO] Ao_Set_Mute Failed!\n");
        return -1;
    }

    if ((s32Ret = Ao_Set_Volume(0, AO_DEV_CVBS, vol)) < 0)
    {
        ZW_ERROR("[AUDIO] Ao_Set_Volume Failed!\n");
        return -1;
    }

    return 0;
}

int Hi_MPP_SetAudioMappingVol(void *hChannelHandle, int vol, int Type)
{
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    switch(Type)
    {
        case FULLIN:
            if(pPlatCtx->m_FullAudioInVol != vol)
                {pPlatCtx->m_FullAudioInVol = vol;}
            break;
        case FULLOUT:
            if(pPlatCtx->m_FullAudioOutVol != vol)
                {pPlatCtx->m_FullAudioOutVol = vol;}
            break;
        case HALFIN:
            if(pPlatCtx->m_HalfAudioInVol != vol)
                {pPlatCtx->m_HalfAudioInVol = vol;}
            break;
        case HALFOUT:
            if(pPlatCtx->m_HalfAudioOutVol != vol)
                {pPlatCtx->m_HalfAudioOutVol = vol;}
            break;
    }
    ZW_DEBUG("[AUDIO] Hi_MPP_SetAudioMappingVol Type[%d] vol=[%d]\n", Type, vol);
    return 0;
}


// 音频数据处理，组帧
int Hi_MPP_ReadAudioStream(void *hChannelHandle, StreamInfo_t *pstAudioInfo, StreamInfo_t *pstTalkingInfo, int *audioSyncFlag)
{
    SINT32	s32Ret = 0;
    CPlatformChannelContext		*pPlatCtx = NULL;
    CChannelContext		*pChCtx = NULL;
    CTalkingChannel_t	*pTalkingCh = NULL;
    SystemInfo_t		*pSysInfo = NULL;

    H264DATAPACKETHEADER	FrameHead = {0};
    static unsigned long inValueAudio = 0;
    static unsigned long inValueAudioAAC = 0;
    char buffer[AUDIO_FRM_SIZE] = {0};
    char talkBuffer[AUDIO_FRM_SIZE] = {0};
    static int nSyncCnt = 0;
    static int nSyncDefTSCnt = 0;
    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    HY_Res_GetSystemInfo(&pSysInfo);

    if (!pPlatCtx->m_pstAudioRb || !pPlatCtx->m_pstTalkAudioRb)
    {
        return -1;
    }

    // 数据长度不足一帧，直接返回
    if (RingBuffer_Len(pPlatCtx->m_pstAudioRb) < AUDIO_FRM_SIZE
        || RingBuffer_Len(pPlatCtx->m_pstTalkAudioRb) < AUDIO_FRM_SIZE)
    {
        return 1;
    }

    // 取出音频预览数据
    if((s32Ret = RingBuffer_Out(pPlatCtx->m_pstAudioRb, buffer, AUDIO_FRM_SIZE)) != AUDIO_FRM_SIZE)
    {
        RingBuffer_Reset(pPlatCtx->m_pstAudioRb);
        return 1;
    }

#ifdef __IS_SUPPORT_JZ_CRY_DETECT__
        /* 拷贝用于声音侦测的PCM数据 */
        //尝试加锁
        if (pChCtx->m_stAudioDetectCfg.nEnable)
        {
            if (0 == pthread_mutex_trylock(&s_AudioDetectBufInf.m_muLock))
            {
                //成功加锁则将音频数据写入缓冲区（供声音侦测）
                memset(s_AudioDetectBufInf.ULawBuf, 0, AUDIO_FRM_SIZE);
                memcpy(s_AudioDetectBufInf.ULawBuf, buffer, AUDIO_FRM_SIZE);
                s_AudioDetectBufInf.bValid = 1;
                pthread_mutex_unlock(&s_AudioDetectBufInf.m_muLock);
            }
        }
#endif


    // 取出对讲数据
    if((s32Ret = RingBuffer_Out(pPlatCtx->m_pstTalkAudioRb, talkBuffer, AUDIO_FRM_SIZE)) != AUDIO_FRM_SIZE)
    {
        RingBuffer_Reset(pPlatCtx->m_pstTalkAudioRb);
        return 1;
    }

    // 跳过帧头位置
    memcpy(pstAudioInfo->pBuf + sizeof(FrameHead), buffer, AUDIO_FRM_SIZE);
    pstAudioInfo->WritePos = AUDIO_FRM_SIZE + sizeof(FrameHead);

    // 对讲直接放裸数据
    if ((pTalkingCh->m_bStart == TRUE) && (pSysInfo->m_pTalkingCallBack != NULL))
    {
        memcpy(pstTalkingInfo->pBuf, talkBuffer, AUDIO_FRM_SIZE);
        pstTalkingInfo->WritePos = AUDIO_FRM_SIZE;
    }

    /* 获取帧类型、计算时间戳 */
    FrameHead.uDataPacketStartCode	= FRAME_START_CODE;
    FrameHead.byLoadType = LtG711AudioULaw;

    if(eCodecG711A == (AudioEncodeMode_t)pChCtx->m_AudioCodec)
    {
        FrameHead.byLoadType = LtG711AudioALaw;
    }
    else if (eCodecG711U == (AudioEncodeMode_t)pChCtx->m_AudioCodec)
    {
        FrameHead.byLoadType = LtG711AudioULaw;
    }
    else
    {
        FrameHead.byLoadType = LtG711AudioULaw;
    }
    if (*audioSyncFlag) // 每秒钟和主码流的PTS时间戳同步一次
    {
        nSyncCnt++;
        if(nSyncCnt >= 60)
        {
            if (1)//(0 == aeChn)
            {
                if (mainStreamShowTime[FIRST_SENSOR] > inValueAudio)
                {
                    if(mainStreamShowTime[FIRST_SENSOR] - inValueAudio > AI_FRM_MS_INTERVAL)
                    {
                        inValueAudio = mainStreamShowTime[FIRST_SENSOR];
                    }
                    else {
                        inValueAudio += (mainStreamShowTime[FIRST_SENSOR] - inValueAudio)/2;
                    }
                    FrameHead.uShowTime = inValueAudio;
                }
                else
                {
                    inValueAudio += 1;
                    FrameHead.uShowTime = inValueAudio;
                }

                FrameHead.uShowTime |= 1;
            }
            else
            {
                if (mainStreamShowTime[FIRST_SENSOR] > inValueAudioAAC)
                {
                    FrameHead.uShowTime = mainStreamShowTime[FIRST_SENSOR];
                    inValueAudioAAC = FrameHead.uShowTime;
                }
                else
                {
                    inValueAudioAAC += 1;
                    FrameHead.uShowTime = inValueAudioAAC;
                }

                FrameHead.uShowTime |= 1;
            }
            nSyncCnt = 0;
        }
        else {
            if(mainStreamShowTime[FIRST_SENSOR] - inValueAudio > AI_FRM_MS_INTERVAL)
            {
                inValueAudio = mainStreamShowTime[FIRST_SENSOR];
            }
            else {
                if(mainStreamShowTime[FIRST_SENSOR] > inValueAudio)
                {
                    inValueAudio += (mainStreamShowTime[FIRST_SENSOR] - inValueAudio)/2;
                }
                else {
                    inValueAudio += 1;
                }
            }
            FrameHead.uShowTime = inValueAudio;
            FrameHead.uShowTime|=1;
        }
        *audioSyncFlag = 0;
    }
    else
    {
        if(mainStreamShowTime[FIRST_SENSOR] >= inValueAudio)
        {
            inValueAudio += AI_FRM_MS_INTERVAL;         // 实际测试出来，发现一秒钟有 30帧 ，有点问题， 8K 16 单通道的
            nSyncDefTSCnt = 0;
        }
        else {
            inValueAudio += 3200;   // 正常间隔的 9/10
            nSyncDefTSCnt++;
            if(nSyncDefTSCnt == 25)
            {
                nSyncDefTSCnt = 0;
                inValueAudio = mainStreamShowTime[FIRST_SENSOR];
            }
        }
        FrameHead.uShowTime = inValueAudio;
        FrameHead.uShowTime|=1;
    }

    // 获取当前时间戳
    struct timeval	CurTime;
    gettimeofday(&CurTime,NULL);
    FrameHead.u32RealTimeSec = CurTime.tv_sec;
    FrameHead.u32RealTimeUSec = CurTime.tv_usec;

    FrameHead.uFrameNo = pPlatCtx->m_FrameNoAudio + 1;
    pPlatCtx->m_FrameNoAudio = FrameHead.uFrameNo;

    FrameHead.byFrameType	 = PktAudioFrames;
    FrameHead.uOverloadLen = AUDIO_FRM_SIZE;
    memcpy(pstAudioInfo->pBuf, &FrameHead, sizeof(FrameHead));

    return 0;
}

/***
 * @description: 向音频解码通道发送一帧数据
 * @param {void} *hChannelHandle 平台句柄
 * @param {char} *pbuf 帧数据地址
 * @param {int} framesize 帧大小
 * @param {int} audioDataType 音频数据类型
 * @param {int} audioDecMode 音频数据编码类型
 * @return {int} 0-成功 !0-失败
 */
int Hi_MPP_SendFrameToAdev(void *hChannelHandle, char *pbuf, int framesize, int audioDataType, int audioDecMode)
{
    SINT32 s32Ret = 0;
    unsigned char pAudio[1024] = {0};
    SystemInfo_t *pSysInfo = NULL;
    ADEC_WRITE_MODE_E eAdecWriteMode = ADEC_WRITE_BLOCKING;
    ADEC_WRITE_FINISH_E eAdecWriteFlag;
    AO_WRITE_FINISH_E   eAoWriteFlag;

    HY_Res_GetSystemInfo(&pSysInfo);

    // 音频未初始化直接返回
    if (0 == pSysInfo->m_TotalAudioDecChannel || FALSE == pSysInfo->m_bAudioInited)
    {
        return 0;
    }

    /* 播报语音文件尾和对讲结束后, 将标志置为 AUDIO_WRITE_END 结束播放,触发关闭 3A 的回调 */
    switch (audioDataType) {
    case AUDIO_DATA_TYPE_VALID:
        eAdecWriteFlag = ADEC_WRITE_ING;
        eAoWriteFlag  = AO_WRITE_ING;
        break;
    case AUDIO_DATA_TYPE_FILE_END:
    case AUDIO_DATA_TYPE_INVALID:
        eAdecWriteFlag = ADEC_WRITE_END;
        eAoWriteFlag  = AO_WRITE_END;
        break;
    default:
        eAdecWriteFlag = ADEC_WRITE_END;
        eAoWriteFlag  = AO_WRITE_END;
        ZW_ERROR("Get The ERROR audioDataType Please Check\n");
        break;
    }

    if (0 != framesize)
    {
        if (eCodecG711A == audioDecMode)   //G711A
        {
            G711A_TO_G711U(pAudio, (unsigned char *)pbuf, framesize);
        }
        else if (eCodecPCM == audioDecMode) //PCM
        {
            memcpy(pAudio, pbuf, framesize);
        }
        else if (eCodecG711U == audioDecMode) // G711U
        {
            memcpy(pAudio, pbuf, framesize);
        }
        else
        {
            ZW_ERROR("[AUDIO] Unsupport Audio Enocde Type!\n");
            return -1;
        }
    }

    /* 当 framesize 为 0 , WriteFlag 为 AUDIO_WRITE_END 时, 不会播放数据, 并且会触发播放结束回调 */
    eAdecWriteMode = ADEC_WRITE_BLOCKING;

    if (eCodecPCM == audioDecMode) //PCM
    {
        s32Ret = Ao_Chn_Write(0, 0, pAudio, framesize, eAdecWriteMode, eAoWriteFlag);
    }
    else
    {
        s32Ret = Adec_Chn_Write(0, 0, pAudio, framesize, eAdecWriteMode, eAdecWriteFlag);
    }

    if (s32Ret < 0)
    {
        ZW_ERROR("[Audio] Adec_Chn_Write Failed! %#x\n", s32Ret);
        return s32Ret;
    }

    return 0;
}

int GetOSDFontSizeByRes(int width, int height, int *scaleWidth, int *scaleHeight)
{
    if (width > 800)
    {
        if (width > 2880)
        {
            *scaleWidth = 7;
            *scaleHeight = 7;
        }
        else if(width > 1920)
        {
            if (height > 1440) //5MP
            {
                if (height > 1624) // 4:3
                {
                    *scaleWidth = 6;
                    *scaleHeight = 6;
                }
                else // 16:9
                {
                    *scaleWidth = 5;
                    *scaleHeight = 5;
                }
            }
            else//300w, 400w
            {
                *scaleWidth = 4;
                *scaleHeight = 4;
            }
        }
        else
        {
            if (height == 1080) //1080P
            {
                *scaleWidth = 3;
                *scaleHeight = 3;
            }
            else
            {
                *scaleWidth = 2;
                *scaleHeight = 2;
            }
        }
    }
    else if (width >= 704 && width <= 800)
    {
        *scaleWidth = 2;
        *scaleHeight = 2;
    }
    else if (width > 352 && width < 704)
    {
        if (width == 640)
        {
            *scaleWidth = 1;
            *scaleHeight = 1;
        }
        else
        {
            *scaleWidth = 1;
            *scaleHeight = 1;
        }
    }
    else
    {
        *scaleWidth = 1;
        *scaleHeight = 1;
    }

    return 0;
}

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
int Hi_MPP_GetZoomBitmap(void *hChannelHandle, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm)
{
    int nRet = 0;
    int i, j, k, m, n, o;
    char ZOOM[16] = {0};
    char Font[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'X','.',' '};
    int BlockNum = 0;
    unsigned char *pSrc, *pDst;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t *pSysInfo = NULL;
    int scaleWidth = 0, scaleHeight = 0;
    void *pZoomTime = NULL;
    int stride = 0;
    int line_index = 0;
    int bitmap_index = 0;
    int offset_byte = 0;
    int offset_bit = 0;
    int offset_byte_src = 0;
    int offset_bit_src = 0;
    unsigned char color = 0;
    double nZoomValue = 1.0;


    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    HY_Enc_GetEncChInfo(&pChCtx);
    HY_Res_GetSystemInfo(&pSysInfo);

    ZoomOSD_t *m_ZoomOsdInfo = &pChCtx->m_ZoomOsdInfo;

    XTRACE_MUTEX_LOCK(&pChCtx->m_ZoomValueLock);
    nZoomValue = pChCtx->m_ZoomRealValue;
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_ZoomValueLock);

    snprintf(ZOOM, sizeof(ZOOM),"%2.1fX",nZoomValue);

    if(NULL == m_ZoomOsdInfo->pWhiteFontInfo)
    {
        ZW_ERROR("Hi_MPP_GetTimeBitmap buffer err: , pWhiteFontInfo = %p\n", m_ZoomOsdInfo->pWhiteFontInfo);
        return -1;
    }

    GetOSDFontSizeByRes(pChCtx->m_PictureRes[eStm].m_PicWidth, pChCtx->m_PictureRes[eStm].m_PicHeight, &scaleWidth, &scaleHeight);

    ZW_DEBUG("[ZOOM][TIME] eStm = %d,scaleWidth = %d, scaleHeight = %d\n", eStm, scaleWidth, scaleHeight);

    BlockNum = pChCtx->ZoomBlockNum[eStm];

    //ZW_DEBUG("TIME:%s\n",TIME);

    VOSD_OVERYLAY_S tt;
    memset(&tt,0,sizeof(VOSD_OVERYLAY_S));

    nRet = Hi_MPP_SetBitMapAttrToGetAddrFromPlat(hChannelHandle, nSenIndex, eStm, ZOOM_TYPE, &tt, 0);
    if (nRet < 0)
    {
        ZW_ERROR("Hi_MPP_SetBitMapAttrToGetAddrFromPlat failed(%d)\n", nRet);
        return -2;
    }

    stride = tt.Color;
    pZoomTime = tt.pData;
    pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pZoomBitmap = pZoomTime;

    pDst = (unsigned char *)(pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pZoomBitmap);

    for (i = 0;i < stride * pPlatCtx->ZoomBitmapHeight[eStm];i++)
    {
        pDst[i] = 0;
    }

    /* 拼装BitMap */
    for (i = 0; i < BlockNum; i++)
    {
        // 暂不支持宏块亮度判断，使用描边来解决不同场景下OSD的显示辨识度
        // 只刷新变化的部分，当下一个字符有变化时，上一个字符也要重新赋值，不然描边会出现问题
        //if ((pChCtx->BlockAscValue[eStm][i] != TIME[i]) || ((i + 1 < BlockNum) && pChCtx->BlockAscValue[eStm][i + 1] != TIME[i + 1]))
        {
            /* 搜寻字符点阵信息编号 */
            for (j = 0; j < 13; j++)
            {
                if (Font[j] == ZOOM[i])
                    break;
            }

            if (j < 13) // 15个关键字
            {
                pSrc = (unsigned char *)m_ZoomOsdInfo->pWhiteFontInfo + m_ZoomOsdInfo->FontWidth * m_ZoomOsdInfo->FontHeight * j / BYTE_PIXELS;

                // 列
                for (k = 0; k < m_ZoomOsdInfo->FontHeight; k++)
                {
                    //行
                    for (m = 0; m < m_ZoomOsdInfo->FontWidth; m++)
                    {
                        offset_byte_src = (m) / BYTE_PIXELS;
                        offset_bit_src = ((m) % BYTE_PIXELS) * PIXEL_BITS;
                        color = (pSrc[offset_byte_src] >> offset_bit_src) & BYTE_ROUND;

                        // 像素放大，列
                        for (n = 0; n < scaleHeight; n++)
                        {
                            // 行
                            for (o = 0; o < scaleWidth; o++)
                            {
                                bitmap_index = ((k * scaleHeight + n) * pPlatCtx->ZoomBitmapWidth[eStm]) + (m+m_ZoomOsdInfo->FontWidth * i)*scaleWidth + o;
                                line_index = bitmap_index / pPlatCtx->ZoomBitmapWidth[eStm];
                                offset_byte = line_index * stride + (bitmap_index % pPlatCtx->ZoomBitmapWidth[eStm]) / BYTE_PIXELS;
                                offset_bit = (bitmap_index % BYTE_PIXELS) * PIXEL_BITS;

                                pDst[offset_byte] |= (color << offset_bit);
                            }
                        }
                    }
                    pSrc += m_ZoomOsdInfo->FontWidth / BYTE_PIXELS;
                }
            }

            pChCtx->BlockAscValue[eStm][i] = ZOOM[i];
        }
    }

    // 描个边
    font_stroke(pPlatCtx->ZoomBitmapWidth[eStm], pPlatCtx->ZoomBitmapHeight[eStm], pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pZoomBitmap, COLOR_WITHE, COLOR_BLACK, 1,stride);

    return 0;
}
#endif



/**
 * @description: 组装时间OSD的点阵信息，ARGB1555
 * @param {void} *hChannelHandle 平台句柄
 * @param {time_t} *pSysTime 需要显示的时间
 * @param {int} scaleWidth 宽度放大倍数
 * @param {int} scaleHeight 高度放大倍数
 * @param {STREAM_TYPE_E} eStm 码流类型
 * @return {int} 0-成功，非0-失败
 */
int Hi_MPP_GetTimeBitmap(void *hChannelHandle, time_t *pSysTime, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm)
{
    int nRet = 0;
    int i, j, k, m, n, o;
    struct tm *tnow;
    struct tm tnow_r;
    char TIME[64] = {0};
    char Font[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'P', 'M', ':', '-'};
    int BlockNum = 0;
    unsigned char *pSrc, *pDst;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t *pSysInfo = NULL;
    SysTimeOSD_t *pSysTimeInfo = NULL;
    XRS_StreamAttr *pstStmAttr = NULL;
    int scaleWidth = 0, scaleHeight = 0;
    void *pDataTime = NULL;
    int stride = 0;
    int line_index = 0;
    int bitmap_index = 0;
    int offset_byte = 0;
    int offset_bit = 0;
    int offset_byte_src = 0;
    int offset_bit_src = 0;
    unsigned char color = 0;

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    HY_Enc_GetEncChInfo(&pChCtx);
    HY_Res_GetSystemInfo(&pSysInfo);
    pSysTimeInfo = &pChCtx->m_TimeOSDInfo;
    pstStmAttr = &pPlatCtx->m_stStreamAttr[nSenIndex][eStm];

    // 如果通道没有OSD能力则直接返回
    if (!pstStmAttr->bEnable || !pstStmAttr->bUseOSD)
    {
        return 0;
    }

    if(NULL == pSysTimeInfo->pWhiteFontInfo)
    {
        ZW_ERROR("Hi_MPP_GetTimeBitmap buffer err: , pWhiteFontInfo = %p\n", pSysTimeInfo->pWhiteFontInfo);
        return -1;
    }

    GetOSDFontSizeByRes(pChCtx->m_PictureRes[eStm].m_PicWidth, pChCtx->m_PictureRes[eStm].m_PicHeight, &scaleWidth, &scaleHeight);

    //ZW_DEBUG("[OSD][TIME] eStm = %d,scaleWidth = %d, scaleHeight = %d\n", eStm, scaleWidth, scaleHeight);

    localtime_r(pSysTime, &tnow_r);
    tnow = &tnow_r;
    BlockNum = pChCtx->TimeBlockNum[eStm];

    if (pSysTimeInfo->bHour24)
    {
        if (pSysTimeInfo->DisMod == Mode_YYYY_MM_DD)
        {
            snprintf(TIME, sizeof(TIME),
                    "%04d-%02d-%02d %02d:%02d:%02d",
                    1900 + (int)tnow->tm_year,
                    (int)tnow->tm_mon + 1,
                    (int)tnow->tm_mday,
                    (int)tnow->tm_hour,
                    (int)tnow->tm_min,
                    (int)tnow->tm_sec);
        }
        else if (pSysTimeInfo->DisMod == Mode_MM_DD_YYYY)
        {
            snprintf(TIME, sizeof(TIME),
                    "%02d-%02d-%04d %02d:%02d:%02d",
                    (int)tnow->tm_mon + 1,
                    (int)tnow->tm_mday,
                    1900 + (int)tnow->tm_year,
                    (int)tnow->tm_hour,
                    (int)tnow->tm_min,
                    (int)tnow->tm_sec);
        }
        else if (pSysTimeInfo->DisMod == Mode_DD_MM_YYYY)
        {
            snprintf(TIME, sizeof(TIME),
                    "%02d-%02d-%04d %02d:%02d:%02d",
                    (int)tnow->tm_mday,
                    (int)tnow->tm_mon + 1,
                    1900 + (int)tnow->tm_year,
                    (int)tnow->tm_hour,
                    (int)tnow->tm_min,
                    (int)tnow->tm_sec);
        }
    }
    else
    {
        if (pSysTimeInfo->DisMod == Mode_YYYY_MM_DD)
        {
            if (tnow->tm_hour >= 12)
            {
                snprintf(TIME, sizeof(TIME),
                        "%04d-%02d-%02d PM %02d:%02d:%02d",
                        1900 + (int)tnow->tm_year,
                        (int)tnow->tm_mon + 1,
                        (int)tnow->tm_mday,
                        ((int)tnow->tm_hour) == 12 ? 12 : ((int)tnow->tm_hour - 12),
                        (int)tnow->tm_min,
                        (int)tnow->tm_sec);
            }
            else
            {
                snprintf(TIME, sizeof(TIME),
                        "%04d-%02d-%02d AM %02d:%02d:%02d",
                        1900 + (int)tnow->tm_year,
                        (int)tnow->tm_mon + 1,
                        (int)tnow->tm_mday,
                        ((int)tnow->tm_hour) == 0 ? 12 : ((int)tnow->tm_hour),
                        (int)tnow->tm_min,
                        (int)tnow->tm_sec);
            }
        }
        else if (pSysTimeInfo->DisMod == Mode_MM_DD_YYYY)
        {
            if (tnow->tm_hour >= 12)
            {
                snprintf(TIME, sizeof(TIME),
                        "%02d-%02d-%04d PM %02d:%02d:%02d",
                        (int)tnow->tm_mon + 1,
                        (int)tnow->tm_mday,
                        1900 + (int)tnow->tm_year,
                        ((int)tnow->tm_hour) == 12 ? 12 : ((int)tnow->tm_hour - 12),
                        (int)tnow->tm_min,
                        (int)tnow->tm_sec);
            }
            else
            {
                snprintf(TIME, sizeof(TIME),
                        "%02d-%02d-%04d AM %02d:%02d:%02d",
                        (int)tnow->tm_mon + 1,
                        (int)tnow->tm_mday,
                        1900 + (int)tnow->tm_year,
                        ((int)tnow->tm_hour) == 0 ? 12 : ((int)tnow->tm_hour),
                        (int)tnow->tm_min,
                        (int)tnow->tm_sec);
            }
        }
        else if (pSysTimeInfo->DisMod == Mode_DD_MM_YYYY)
        {
            if (tnow->tm_hour >= 12)
            {
                snprintf(TIME, sizeof(TIME),
                        "%02d-%02d-%04d PM %02d:%02d:%02d",
                        (int)tnow->tm_mday,
                        (int)tnow->tm_mon + 1,
                        1900 + (int)tnow->tm_year,
                        ((int)tnow->tm_hour) == 12 ? 12 : ((int)tnow->tm_hour - 12),
                        (int)tnow->tm_min,
                        (int)tnow->tm_sec);
            }
            else
            {
                snprintf(TIME, sizeof(TIME),
                        "%02d-%02d-%04d AM %02d:%02d:%02d",
                        (int)tnow->tm_mday,
                        (int)tnow->tm_mon + 1,
                        1900 + (int)tnow->tm_year,
                        ((int)tnow->tm_hour) == 0 ? 12 : ((int)tnow->tm_hour),
                        (int)tnow->tm_min,
                        (int)tnow->tm_sec);
            }
        }
    }

    //ZW_DEBUG("TIME:%s\n",TIME);

    VOSD_OVERYLAY_S tt;
    memset(&tt,0,sizeof(VOSD_OVERYLAY_S));

    nRet = Hi_MPP_SetBitMapAttrToGetAddrFromPlat(hChannelHandle, nSenIndex, eStm, TIME_TYPE, &tt, 0);
    if (nRet < 0)
    {
        ZW_ERROR("Hi_MPP_SetBitMapAttrToGetAddrFromPlat failed(%d)\n", nRet);
        return -2;
    }

    stride = tt.Color;
    pDataTime = tt.pData;
    pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pTimeBitmap = pDataTime;

    pDst = (unsigned char *)(pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pTimeBitmap);

    for (i = 0;i < stride * pPlatCtx->TimeBitmapHeight[eStm];i++)
    {
        pDst[i] = 0;
    }

    /* 拼装BitMap */
    for (i = 0; i < BlockNum; i++)
    {
        // 暂不支持宏块亮度判断，使用描边来解决不同场景下OSD的显示辨识度
        // 只刷新变化的部分，当下一个字符有变化时，上一个字符也要重新赋值，不然描边会出现问题
        //if ((pChCtx->BlockAscValue[eStm][i] != TIME[i]) || ((i + 1 < BlockNum) && pChCtx->BlockAscValue[eStm][i + 1] != TIME[i + 1]))
        {
            /* 搜寻字符点阵信息编号 */
            for (j = 0; j < 15; j++)
            {
                if (Font[j] == TIME[i])
                    break;
            }

            if (j < 15) // 15个关键字
            {
                pSrc = (unsigned char *)pSysTimeInfo->pWhiteFontInfo + pSysTimeInfo->FontWidth * pSysTimeInfo->FontHeight * j / BYTE_PIXELS;

                // 列
                for (k = 0; k < pSysTimeInfo->FontHeight; k++)
                {
                    //行
                    for (m = 0; m < pSysTimeInfo->FontWidth; m++)
                    {
                        offset_byte_src = (m) / BYTE_PIXELS;
                        offset_bit_src = ((m) % BYTE_PIXELS) * PIXEL_BITS;
                        color = (pSrc[offset_byte_src] >> offset_bit_src) & BYTE_ROUND;

                        // 像素放大，列
                        for (n = 0; n < scaleHeight; n++)
                        {
                            // 行
                            for (o = 0; o < scaleWidth; o++)
                            {
                                bitmap_index = ((k * scaleHeight + n) * pPlatCtx->TimeBitmapWidth[eStm]) + (m+pSysTimeInfo->FontWidth * i)*scaleWidth + o;
                                line_index = bitmap_index / pPlatCtx->TimeBitmapWidth[eStm];
                                offset_byte = line_index * stride + (bitmap_index % pPlatCtx->TimeBitmapWidth[eStm]) / BYTE_PIXELS;
                                offset_bit = (bitmap_index % BYTE_PIXELS) * PIXEL_BITS;

                                pDst[offset_byte] |= (color << offset_bit);
                            }
                        }
                    }
                    pSrc += pSysTimeInfo->FontWidth / BYTE_PIXELS;
                }
            }

            pChCtx->BlockAscValue[eStm][i] = TIME[i];
        }
    }

    // 描个边
    font_stroke(pPlatCtx->TimeBitmapWidth[eStm], pPlatCtx->TimeBitmapHeight[eStm], pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pTimeBitmap, COLOR_WITHE, COLOR_BLACK, 1,stride);

    return 0;
}

static int Hi_MPP_GetCurResStr(int width, int height, char *buf, int len)
{
    if (NULL == buf)
    {
        return -1;
    }

    snprintf(buf, len, "%dX%-4d", width, height);

    return 0;
}

static int Hi_MPP_GetBitrateStr(void *hChannelHandle, ENCODE_STREAM_E eStm, char *buf, int bufLen)
{
    CPlatformChannelContext *pPlatCtx = NULL;

    if ((NULL == hChannelHandle) || (NULL == buf))
    {
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    snprintf(buf, bufLen, "%5dKB", pPlatCtx->m_MppRCInfo[eStm].m_bit_rate);

    return 0;
}

static int Hi_MPP_GetFlowStr(void *hChannelHandle, char *buf, int bufLen)
{
    CPlatformChannelContext *pPlatCtx = NULL;
    XRS_STATISTICS	*pstStat = NULL;

    if ((NULL == hChannelHandle) || (NULL == buf))
    {
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    pstStat = &pPlatCtx->m_stStatistics;

    // 计数达到最大MAX_OSD_FLOW_DATA后，从1开始，避免数据溢出导致显示异常。
    pstStat->inFlow = pstStat->inFlow - (pstStat->inFlow / MAX_OSD_FLOW_DATA) * MAX_OSD_FLOW_DATA;
    pstStat->outFlow = pstStat->outFlow - (pstStat->outFlow / MAX_OSD_FLOW_DATA) * MAX_OSD_FLOW_DATA;
    snprintf(buf, bufLen, "IN:%05d OUT:%05d", pstStat->inFlow, pstStat->outFlow);

    return 0;
}

static int Hi_MPP_GetLightInfoStr(void *hChannelHandle, char *buf, int bufLen)
{
    CChannelContext 	*pChCtx = NULL;

    if(NULL == buf)
    {
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChCtx);

    //增益比EV值要大
    if(g_gain < 1024)
    {
        snprintf(buf, bufLen, "%4d %4d %3d %3d %2d %d %d %d", g_gain, g_Exposure, g_rggain, g_bggain, g_nCurSensorFrame, pChCtx->m_FinishingGrade, pChCtx->m_HighTempFlag, pChCtx->m_DayNightStutas);
    }
    else if(g_gain < 1024 * 1024)
    {
        snprintf(buf, bufLen, "%4dK %4dK %3d %3d %2d %d %d %d", g_gain/1024, g_Exposure/1024, g_rggain, g_bggain, g_nCurSensorFrame, pChCtx->m_FinishingGrade, pChCtx->m_HighTempFlag, pChCtx->m_DayNightStutas);
    }
    else
    {
        snprintf(buf, bufLen, "%4dKK %3dKK %3d %3d %2d %d %d %d", g_gain/1024/1024, g_Exposure/1024/1024, g_rggain, g_bggain, g_nCurSensorFrame, pChCtx->m_FinishingGrade, pChCtx->m_HighTempFlag, pChCtx->m_DayNightStutas);
    }

    return 0;
}

static int Hi_MPP_GetOSDKeywordValue(void *hChannelHandle, ENCODE_STREAM_E eStm,int type, char *buf, int bufLen)
{
    CChannelContext		*pChCtx = NULL;

    if ((NULL == hChannelHandle) || (NULL == buf))
    {
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChCtx);

    switch(type)
    {
        case OSDV2_KW_RES:
            Hi_MPP_GetCurResStr(pChCtx->m_PictureRes[eStm].m_PicWidth, pChCtx->m_PictureRes[eStm].m_PicHeight, buf, bufLen);
            break;

        case OSDV2_KW_BPS:
            Hi_MPP_GetBitrateStr(hChannelHandle, eStm, buf, bufLen);
            break;

        case OSDV2_KW_FLOW:
            Hi_MPP_GetFlowStr(hChannelHandle, buf, bufLen);
            break;

        case OSDV2_KW_LGT:
            Hi_MPP_GetLightInfoStr(hChannelHandle, buf, bufLen);
            break;

        default:
            break;
    }
    return 0;
}

static int GetKeywordFontIdx(char *font, int fontNum, char c)
{
    int idx = 0;
    for (idx = 0; idx < fontNum; idx++)
    {
        if(c == font[idx])
        {
            break;
        }
    }
    if (idx >= fontNum)
    {
        idx = 19;
    }
    return idx;
}

int Hi_MPP_GetOSDV2Bitmap(void *hChannelHandle)
{
    char Font[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'K', 'B', 'I', 'N', 'O', 'U', 'T', ':', ' '};
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    int i = 0, j = 0, k = 0, l = 0, w = 0, h = 0, c = 0;
    int TotalWidth = 0, TotalHeight = 0;
    unsigned char *pDst2 = NULL;
    unsigned char *pSrcWhite = NULL;
    OSDV2TextLineInfo_t *lineInfo = NULL;
    int lineNum = 0;
    int FontWidth = 0, FontHeight = 0;

    ENCODE_STREAM_E eStm = FIRST_STREAM;
    XRS_StreamAttr *pstStmAttr = NULL;
    char keywordBuf[32] = {0};
    int fontIdx = 0;
    int ScaleWidth = 0, ScaleHeight = 0;
    int maxLineWidth = 0;

    void *pDataText = NULL;
    int nNeedClearLastLine = 0;
    int nClearStart = 0;
    int line_index = 0;
    int bitmap_index = 0;
    int offset_byte = 0;
    int offset_bit = 0;
    int offset_byte_src = 0;
    int offset_bit_src = 0;
    unsigned char color = 0;
    int stride = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    TotalWidth = pChCtx->TextOSDRect_Src.right - pChCtx->TextOSDRect_Src.left;
    TotalHeight = pChCtx->TextOSDRect_Src.bottom - pChCtx->TextOSDRect_Src.top;
    TotalWidth = (TotalWidth + 0xF) & 0xFFF0;

    lineInfo = pChCtx->m_TextOSDV2Info.lineInfo;
    lineNum = pChCtx->m_TextOSDV2Info.lineNum;

    pSrcWhite = (unsigned char *)pChCtx->m_TextOSDV2Info.pWhiteFontInfo;

    FontHeight = pChCtx->m_TextOSDV2Info.FontHeight;
    FontWidth = pChCtx->m_TextOSDV2Info.FontWidth;


    int nRet = 0;
    int bErrFlag = 0;
    VOSD_OVERYLAY_S tt;
    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        for (eStm = FIRST_STREAM; eStm < MAX_STREAM_COUNT; eStm++)
        {
            if (eStm != FIRST_STREAM && eStm != SECOND_STREAM && eStm != FOURTH_STREAM)
            {
                continue;
            }

#if defined __ISP_IMAGE_DEBUG__
            if (eStm != FIRST_STREAM)
            {
                continue;
            }
#endif
            pstStmAttr = &pPlatCtx->m_stStreamAttr[nSenIndex][eStm];

            if (!pstStmAttr->bEnable || !pstStmAttr->bUseOSD)
                continue;

            memset(&tt,0,sizeof(VOSD_OVERYLAY_S));

            nRet = Hi_MPP_SetBitMapAttrToGetAddrFromPlat(hChannelHandle,nSenIndex,eStm,TEXT_TYPE,&tt,0);
            if (nRet < 0)
            {
                ZW_ERROR("Hi_MPP_SetBitMapAttrToGetAddrFromPlat failed(%d)\n", nRet);
                bErrFlag = 1;
                break;
            }

            stride = tt.Color;
            pDataText = tt.pData;
            pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pTextBitmap = pDataText;

            GetOSDFontSizeByRes(pChCtx->m_PictureRes[eStm].m_PicWidth, pChCtx->m_PictureRes[eStm].m_PicHeight, &ScaleWidth, &ScaleHeight);

            pDst2 = (unsigned char *)(pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pTextBitmap);

            //找到最大行宽
            for(i = 0; i < lineNum; i++)
            {
                maxLineWidth = (lineInfo[i].pixWidth > maxLineWidth) ? lineInfo[i].pixWidth : maxLineWidth;
            }

            if (nNeedClearLastLine)
            {
                nClearStart = (lineNum - 1) * stride * ScaleHeight * 16;
                for (i = 0;i < stride * ScaleHeight * 16;i++)
                {
                    pDst2[nClearStart + i] = 0;
                }
            }
            // 行
            for (l = 0; l < lineNum; l++)
            {
                if (0 == nNeedClearLastLine)
                {
                    if (lineInfo[l].keywordNum > 0)
                    {
                        nClearStart = l * stride * ScaleHeight * 16;
                        for (i = 0;i < stride * ScaleHeight * 16;i++)
                        {
                            pDst2[nClearStart + i] = 0;
                        }
                    }
                }
                int alignmentLength = 0;//偏移长度
                //当前OSD在右边
                if(pChCtx->TextOSDRect_Src.left > (VIDEO_WIDTH / 2))
                {
                    //当前行字符总个数小于最大行字符个数，则右对齐
                    if( lineInfo[l].pixWidth < maxLineWidth)
                    {
                        alignmentLength = maxLineWidth - lineInfo[l].pixWidth;
                    }
                }
                // 关键字
                for (k = 0; k < lineInfo[l].keywordNum; k++)
                {
                    // 查找keyword类型，获取keyword对应的值
                    OSDV2TextKeyword_t *keyword = &lineInfo[l].keyWords[k];
                    int pos = keyword->pos;

                    memset(keywordBuf, 0, 32);

                    Hi_MPP_GetOSDKeywordValue(hChannelHandle, eStm, keyword->type, keywordBuf, sizeof(keywordBuf));

                    for (c = 0; c < (lineInfo[l].keyWords[k].pixWidth >> 3); c++)
                    {
                        fontIdx = GetKeywordFontIdx(Font, 20, keywordBuf[c]);

                        for (h = 0; h < FontHeight; h++)
                        { // 行 16
                            for (w = 0; w < FontWidth; w++)
                            {
                                if (fontIdx >= 0)
                                {
                                    offset_byte_src = ((fontIdx * FontHeight + h) * FontWidth + w) / BYTE_PIXELS;
                                    offset_bit_src = (((fontIdx * FontHeight + h) * FontWidth + w) % BYTE_PIXELS) * PIXEL_BITS;
                                    color = (pSrcWhite[offset_byte_src] >> offset_bit_src) & BYTE_ROUND;

                                    // 像素拷贝
                                    for (i = 0; i < ScaleHeight; i++)
                                    {
                                        for (j = 0; j < ScaleWidth; j++)
                                        {
                                            bitmap_index = ((((l * FontHeight + h) * ScaleHeight + i) * TotalWidth + pos + alignmentLength + c * FontWidth + w) * ScaleWidth + j);
                                            line_index = bitmap_index / (TotalWidth * ScaleWidth);
                                            offset_byte = line_index * stride + (bitmap_index % (TotalWidth * ScaleWidth)) / BYTE_PIXELS;
                                            offset_bit = (bitmap_index % BYTE_PIXELS) * PIXEL_BITS;
                                            pDst2[offset_byte] |= (color << offset_bit);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 描个边
            font_stroke(TotalWidth*ScaleWidth, TotalHeight*ScaleHeight, pDst2, COLOR_WITHE, COLOR_BLACK, 1,stride);

            Hi_MPP_SetOSD(pPlatCtx, nSenIndex, eStm, TEXT_TYPE, pChCtx->m_bEnableTextOSD);
        }

    }

    if (bErrFlag)
    {
        return -2;
    }

    return 0;
}

/**
 * @description: 初始化OSD模块
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 0-成功，其他-失败
 */
SINT32 Hi_MPP_InitOsd(void *hChannelHandle)
{
    SINT32 Ret = 0;
    VIDEO_CHIP_CONF_S ChipConf = {0};
    VOSD_CONF_S VosdConf = {0};

    Ret = Vosd_Open(1, ChipConf, VosdConf);
    if (Ret != 0) {
        ZW_ERROR("Vosd_Open FAIL(%d)\n", Ret);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_InitOsd Success\n");
    return 0;
}

//!\brief  反初始化osd，销毁在vpe上创建的rgn的handle
//!\return  成功-0，失败为-1
int Hi_MPP_DeInitOsd(void)
{
    int s32Ret = 0;

    s32Ret = Vosd_Close(0);
    if (s32Ret < 0)
    {
        ZW_ERROR("[OSD] Vosd_Close failed with %#x\n", s32Ret);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_DeInitOsd SUCCESS\n");
    return 0;
}

/**
 * @description: 启动OSD模块并设置相关词参数
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 0-成功，其他-失败
 */
SINT32 Hi_MPP_StartOSD(void *hChannelHandle)
{
    SINT32 s32Ret = 0;

    s32Ret = Vosd_Start(0, 0);
    if (s32Ret < 0)
    {
        ZW_ERROR("[OSD] Vosd_Start failed with %#x\n", s32Ret);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_StartOSD Success\n");
    return 0;
}

/**
 * @description: 停止OSD模块并设置相关词参数
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 0-成功，其他-失败
 */
SINT32 Hi_MPP_StopOSD(void *hChannelHandle)
{
    SINT32 s32Ret = 0;

    s32Ret = Vosd_Stop(0);
    if (s32Ret < 0)
    {
        ZW_ERROR("[OSD] Vosd_Stop failed with %#x\n", s32Ret);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_StopOSD Success\n");
    return 0;
}

int Hi_MPP_SetBitMapAttrToGetAddrFromPlat(void *hChannelHandle, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm, ENCODE_OSD_TYPE eOsdType, VOSD_OVERYLAY_S *stOverylay, int update)
{
    int ret = 0;
    VOSD_CHNTYPE_E eChnType = 0;
    SINT32 ps32OsdID = -1;
    RECTH *pRecth = NULL;
    CPlatformChannelContext 	*pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    if (TIME_TYPE == eOsdType)
    {
         ps32OsdID = pPlatCtx->m_stStreamAttr[nSenIndex][eStm].s32TimeOsdID;
         pRecth = &pPlatCtx->m_stStreamAttr[nSenIndex][eStm].TimeOSDRect;
    }
    else if (TEXT_TYPE == eOsdType)
    {
        ps32OsdID = pPlatCtx->m_stStreamAttr[nSenIndex][eStm].s32TextOsdID;
        pRecth = &pPlatCtx->m_stStreamAttr[nSenIndex][eStm].TextOSDRect;
    }
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    else if (ZOOM_TYPE == eOsdType)
    {
        ps32OsdID = pPlatCtx->m_stStreamAttr[nSenIndex][eStm].s32ZoomOsdID;
        pRecth = &pPlatCtx->m_stStreamAttr[nSenIndex][eStm].ZoomOSDRect;
    }
#endif
    else
    {
        ZW_ERROR("[OSD] Unsupport OSD type %d!\n", eOsdType);
        return -1;
    }

    if (update)
    {
        pRecth->left = stOverylay->Rect.x;
        pRecth->right = stOverylay->Rect.w;
        pRecth->top = stOverylay->Rect.y;
        pRecth->bottom = stOverylay->Rect.h;
    }
    else
    {
        stOverylay->Rect.x = pRecth->left;
        stOverylay->Rect.w = pRecth->right;
        stOverylay->Rect.y = pRecth->top;
        stOverylay->Rect.h = pRecth->bottom;
    }

    stOverylay->Rect.x = (stOverylay->Rect.x+1) & 0xfffe;
    stOverylay->Rect.w = (stOverylay->Rect.w+1) & 0xfffe;
    stOverylay->Rect.y = (stOverylay->Rect.y+1) & 0xfffe;
    stOverylay->Rect.h = (stOverylay->Rect.h+1) & 0xfffe;

    eChnType = __KS_VOSD_CHN_TYPE(nSenIndex, eStm);
    if (eChnType == VOSD_CHNTYPE_BUTT)
    {
        ZW_ERROR("[OSD] Chn Unsupport OSD [%d] [%d]!\n", nSenIndex, eStm);
        return -1;
    }

    //ZW_DEBUG("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat eStm(%d)  eOsdType(%d)\n",eStm,eOsdType);
    //ZW_DEBUG("[BASEVOSD] Vosd_Chn_SetBitMapAttrToGetAddrFromPlat VosdChnType(%d) Chn(%d)  \n",eChnType,ps32OsdID);
    //ZW_DEBUG("x(%d)  y(%d) w(%d) h(%d) start\n",stOverylay->Rect.x,stOverylay->Rect.y,stOverylay->Rect.w,stOverylay->Rect.h);
    ret = Vosd_Chn_SetBitMapAttrToGetAddrFromPlat(0, ps32OsdID, eChnType, stOverylay);
    if (ret)
    {
        ZW_ERROR("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat eStm(%d)  eOsdType(%d)\n",eStm,eOsdType);
        ZW_ERROR("[BASEVOSD] Vosd_Chn_SetBitMapAttrToGetAddrFromPlat VosdChnType(%d) Chn(%d)  \n",eChnType,ps32OsdID);
        ZW_ERROR("x(%d)  y(%d) w(%d) h(%d) start\n",stOverylay->Rect.x,stOverylay->Rect.y,stOverylay->Rect.w,stOverylay->Rect.h);
        ZW_ERROR("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat fail(%d)\n", ret);
        return -1;
    }

    if (update)
    {//创建时，设置为透明
        memset(stOverylay->pData,0,stOverylay->Rect.w * stOverylay->Rect.h / 4);
    }

    return 0;
}

/***
 * @description: 设置OSD的叠加内容和使能
 * @param {void} *hChannelHandle 平台句柄
 * @param {STREAM_TYPE_E} eStm 码流类型
 * @param {ENCODE_OSD_TYPE} eOsdType OSD类型
 * @param {BOOL} bEn 是否使能
 * @return {SINT32}
 */
SINT32 Hi_MPP_SetOSD(void *hChannelHandle, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm, ENCODE_OSD_TYPE eOsdType, BOOL bEn)
{
    SINT32 s32Ret = 0;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext 	*pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    XRS_StreamAttr *pstStreamAttr = NULL;
    VOSD_OVERYLAY_S stOverylay = {0};
    SINT32 *ps32OsdID = NULL;
    VOSD_CHNTYPE_E eChnType = 0;
    BOOL *pbCreateFlag = NULL;

    if (eOsdType >= BUTT_TYPE)
    {
        ZW_ERROR("Not support osd type %d\n", eOsdType);
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChCtx);

    eChnType = __KS_VOSD_CHN_TYPE(nSenIndex, eStm);
    if (eChnType == VOSD_CHNTYPE_BUTT)
    {
        ZW_ERROR("[OSD] Chn Unsupport OSD [%d] [%d]!\n", nSenIndex, eStm);
        return -1;
    }

    pstStreamAttr = &(pPlatCtx->m_stStreamAttr[nSenIndex][eStm]);
    //ZW_DEBUG("[OSD] bEnable(%d)  bUseOSD(%d) bEn(%d) eChnType(%d) eStm(%d) eOsdType(%d)\n", pstStreamAttr->bEnable,pstStreamAttr->bUseOSD,bEn,eChnType,eStm,eOsdType);

    // 通道未使能或者无OSD
    if (!pstStreamAttr->bEnable || !pstStreamAttr->bUseOSD || VOSD_CHNTYPE_BUTT == eChnType)
    {
        return 0;
    }

    if (TIME_TYPE == eOsdType)
    {
        ps32OsdID = &pstStreamAttr->s32TimeOsdID;
        pbCreateFlag = &pstStreamAttr->m_bTimeOSDCreated;
    }
    else if (TEXT_TYPE == eOsdType)
    {
        stOverylay.Rect.x = pChCtx->m_TextOSDRect[eStm].left;
        stOverylay.Rect.y = pChCtx->m_TextOSDRect[eStm].top;
        stOverylay.Rect.w = pChCtx->m_TextOSDRect[eStm].right - pChCtx->m_TextOSDRect[eStm].left;
        stOverylay.Rect.h = pChCtx->m_TextOSDRect[eStm].bottom - pChCtx->m_TextOSDRect[eStm].top;
        stOverylay.pData = pPlatCtx->m_stStreamAttr[FIRST_SENSOR][eStm].pTextBitmap;
        ps32OsdID = &pstStreamAttr->s32TextOsdID;
        pbCreateFlag = &pstStreamAttr->m_bTextOSDCreated;
    }
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    else if (ZOOM_TYPE == eOsdType)
    {
        stOverylay.Rect.x = pChCtx->m_ZoomOSDRect[eStm].left;
        stOverylay.Rect.y = pChCtx->m_ZoomOSDRect[eStm].top;
        stOverylay.Rect.w = pChCtx->m_ZoomOSDRect[eStm].right - pChCtx->m_ZoomOSDRect[eStm].left;
        stOverylay.Rect.h = pChCtx->m_ZoomOSDRect[eStm].bottom - pChCtx->m_ZoomOSDRect[eStm].top;
        stOverylay.pData = pPlatCtx->m_stStreamAttr[FIRST_SENSOR][eStm].pZoomBitmap;
        ps32OsdID = &pstStreamAttr->s32ZoomOsdID;
        pbCreateFlag = &pstStreamAttr->m_bZoomOSDCreated;
    }
#endif
    else
    {
        ZW_ERROR("[OSD] Unsupport OSD type %d!\n", eOsdType);
        return -1;
    }

    //ZW_DEBUG("[OSD] eStm = %d, bEn = %d, eOsdType = %d, ID = %d, eChnType = %d\n", eStm, bEn, eOsdType, *ps32OsdID, eChnType);
    if (TIME_TYPE != eOsdType && (0 == stOverylay.Rect.w || 0 == stOverylay.Rect.h) && ZOOM_TYPE != eOsdType)
    {
        bEn = FALSE;
    }

    if (bEn)
    {
        //使能开启的前提下，如果是时间OSD，则在同一个地址上直接更新内容
        //if (FALSE == *pbCreateFlag || TIME_TYPE == eOsdType)
        {
            //ZW_DEBUG("[BASEVOSD] Vosd_Chn_UpdateBitMapDataByPlatAddr  VosdChnType(%d) Chn(%d) start\n", eChnType,*ps32OsdID);
            s32Ret = Vosd_Chn_UpdateBitMapDataByPlatAddr(0, *ps32OsdID, eChnType, 1);
            if (s32Ret < 0)
            {
                ZW_ERROR("[OSD][eOsdType=%d] Vosd_Chn_UpdateBitMapDataByPlatAddr fail(%d)\n", eOsdType, s32Ret);
                return -1;
            }
            *pbCreateFlag = TRUE;
        }
    }
    else
    {
        // 判断区域是否创建，未创建则不清除，防止报错
        if (*pbCreateFlag)
        {
            ZW_DEBUG("[BASEVOSD] Vosd_Chn_DelBitMap VosdChnType(%d) Chn(%d) start\n", eChnType,*ps32OsdID);
            s32Ret = Vosd_Chn_DelBitMap(0, eChnType, *ps32OsdID);
            if (s32Ret < 0)
            {
                ZW_ERROR("[OSD][eOsdType=%d] Vosd_Chn_DelBitMap Failed, s32Ret = %d\n", eOsdType, s32Ret);
                return -1;
            }
            *pbCreateFlag = FALSE;
        }
    }

    return 0;
}

/**
 * @description: 打开 VDA 模块
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVda(void *hChannelHandle)
{
#if (USE_VDA)
    SINT32 s32Ret = 0;
    VDA_CONF_S  m_VdaConf = {0};
    CPlatformChannelContext	*pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    s32Ret = Vda_Open(pPlatCtx->m_MaxViChn, pPlatCtx->m_ChipConf, m_VdaConf);
    if (s32Ret < 0)
    {
        ZW_ERROR("Vda_Open failed, ret=%d\n", s32Ret);
        return s32Ret;
    }

    s32Ret = Vda_Start(0, 0, 0);
    if (s32Ret < 0)
    {
        ZW_ERROR("Vda_Start failed, ret=%d\n", s32Ret);
        return s32Ret;
    }

    ZW_DEBUG("Vda Hi_MPP_InitVda success\n");
#endif

    return 0;
}

/**
 * @description: 关闭 VDA 模块
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVda(void)
{
#if (USE_VDA)
    SINT32 s32Ret = 0;

    s32Ret = Vda_Stop(0);
    if (s32Ret < 0)
    {
        ZW_ERROR("Vda_Stop Failed\n");
        return -1;
    }

    s32Ret = Vda_Close(0);
    if (s32Ret < 0)
    {
        ZW_ERROR("Vda_Close Failed\n");
        return -1;
    }
    ZW_DEBUG("Hi_MPP_DeInitVda Success\n");
#endif

    return 0;
}

/**
 * @description: 创建移动检测和遮挡检测的通道
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVdaChn(void *hChannelHandle)
{
#if (USE_VDA)
    UINT32 i = 0;
    SINT32 s32Ret = 0;
    VDA_CHN_ATTR_S VdaChnAttr;
    CPlatformChannelContext	*pPlatCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    memset(&VdaChnAttr, 0, sizeof(VdaChnAttr));

    for (i = 0; i < MaxSns; i++)
    {
        // 从子码流取帧
        VdaChnAttr.Width = pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.width;
        VdaChnAttr.Height = pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.height;
        VdaChnAttr.VdaChnIdx = i;

        s32Ret = Vda_Chn_Open(0, VdaChnAttr);
        if (s32Ret < 0)
        {
            ZW_ERROR("Vda_Chn_Open failed, ret=%d, Chn=%d\n", s32Ret, VdaChnAttr.VdaChnIdx);
               return -1;
        }
    }

    ZW_DEBUG("Hi_MPP_InitVdaChn success\n");
#endif

    return 0;
}

/**
 * @description: 销毁移动侦测和遮挡检测的通道
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVdaChn(void)
{
#if (USE_VDA)
    UINT32 i = 0;
    SINT32 s32Ret = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    for (i = 0; i < MaxSns; i++)
    {
        s32Ret = Vda_Chn_Close(0, i);
        if (s32Ret < 0)
        {
            ZW_ERROR("Vda_Chn_Close Failed, ret=%d, Chn=%u\n", s32Ret, i);
            return -1;
        }
    }

    ZW_DEBUG("Vda_Chn_Close SUCCESS\n");
#endif

    return 0;
}

/**
 * @description: 使能移动侦测
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_StartMD(void *hChannelHandle)
{
#if (USE_VDA)
    UINT32 i = 0;
    SINT32 s32Ret = 0;
    CChannelContext *pChCtx = NULL;
    VDA_MD_PARAM_S m_MdParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    HY_Enc_GetEncChInfo(&pChCtx);

    for (i = 0; i < MaxSns; i++)
    {
        if ((s32Ret = Vda_Chn_Get_MdParam(0, i, &m_MdParam)) < 0)
        {
            ZW_ERROR("MD Vda_Chn_Get_MdParam Failed, ret=%d, Chn=%u\n", s32Ret, i);
            return -1;
        }

        m_MdParam.enable = 1;
        // 应用层 高-2 中-3 低-4
        switch (pChCtx->MotionGrade)
        {
            case 2:
                m_MdParam.sen = VDA_SENSI_LEVEL5;
                break;
            case 3:
                m_MdParam.sen = VDA_SENSI_LEVEL3;
                break;
            case 4:
                m_MdParam.sen = VDA_SENSI_LEVEL1;
                break;
            default:
                // ZW_ERROR("Motion Sensitive Set Failed\n");
                break;
        }

        if ((s32Ret = Vda_Chn_Set_MdParam(0, i, m_MdParam)) < 0)
        {
            ZW_ERROR("MD Vda_Chn_Set_MdParam Failed, ret=%d, Chn=%u\n", s32Ret, i);
            return -1;
        }
    }

    ZW_DEBUG("MD Hi_MPP_StartMD success\n");
#endif

    return 0;
}

/**
 * @description: 关闭移动侦测
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_StopMD(void *hChannelHandle)
{
#if (USE_VDA)
    UINT32 i = 0;
    SINT32 s32Ret = 0;
    VDA_MD_PARAM_S m_MdParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    for (i = 0; i < MaxSns; i++)
    {
        if ((s32Ret = Vda_Chn_Get_MdParam(0, i, &m_MdParam)) < 0)
        {
            ZW_ERROR("Vda_Chn_Set_MdParam failed, ret=%d, Chn=%u\n", s32Ret, i);
            return s32Ret;
        }

        m_MdParam.enable = 0;
        if ((s32Ret = Vda_Chn_Set_MdParam(0, i, m_MdParam)) < 0)
        {
            ZW_ERROR("Vda_Chn_Set_MdParam failed, ret=%d, Chn=%u\n", s32Ret, i);
            return s32Ret;
        }
    }

    ZW_DEBUG("MD Hi_MPP_StopMD success\n");
#endif

    return 0;
}

/**
 * @description: 设置移动侦测灵敏度
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} MDIntervel 检测时间间隔(无效)
 * @param {int} motionSensitivityLev 灵敏度（2 - 高，3 - 中， 4 - 低）
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_MDSetPrecision(void *hChannelHandle, int MDIntervel, int motionSensitivityLev)
{
#if (USE_VDA)
    UINT32 i = 0;
    SINT32 s32Ret = 0;
    VDA_MD_PARAM_S m_MdParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    for (i = 0; i < MaxSns; i++)
    {
        s32Ret = Vda_Chn_Get_MdParam(0, i, &m_MdParam);
        if (s32Ret < 0)
        {
            ZW_ERROR("Vda_Chn_Get_MdParam failed, ret=%d, Chn=%u\n", s32Ret, i);
            return -1;
        }

        // 应用层 高-2 中-3 低-4
        switch (motionSensitivityLev)
        {
            case 2:
                m_MdParam.sen = VDA_SENSI_LEVEL5;
                break;
            case 3:
                m_MdParam.sen = VDA_SENSI_LEVEL3;
                break;
            case 4:
                m_MdParam.sen = VDA_SENSI_LEVEL1;
                break;
            default:
                // ZW_ERROR("Motion Sensitive Set Failed\n");
                break;
        }

        s32Ret = Vda_Chn_Set_MdParam(0, i, m_MdParam);
        if (s32Ret != 0)
        {
            ZW_ERROR("Vda_Chn_Set_MdParam failed, ret=%d, Chn=%u\n", s32Ret, i);
            return s32Ret;
        }
    }

    ZW_DEBUG("Change MD Sensitivity Success\n");
#endif

    return 0;
}

/**
 * @description: 获取移动侦测结果
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} MotionAreas 检测区域总数(无效)
 * @param {int} MotionGrade 检测灵敏度(无效)
 * @param {int} MotionRectStride 宏块个数(无效)
 * @param {char} *MotionRect 设置的宏块是否启动检测(无效)
 * @param {char} *pMotion 用户设置的宏块个数(无效)
 * @param {int} *motion 移动检测的结果(1:有运动,0:无运动)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_MotionAnalyzer(void *hChannelHandle, int MotionAreas, int MotionGrade,
    int MotionRectStride, char *pMotion, int *motion ,int *Chn)
{
    UINT32 m_Chn = 0;
    SINT32 i = 0, j = 0, s32Ret = 0;
    SINT32 m = 0, n = 0;
    VDA_MD_AREADATA_S m_MdData = {0};
    VDA_MD_PARAM_S m_MdParam = {0};
    int s32MatchNum = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("PARAM CANNOT be NULL\n");
        return -1;
    }

    for (m_Chn = 0; m_Chn < MaxSns; m_Chn++)
    {
        s32Ret = Vda_Chn_Get_MdParam(0, m_Chn, &m_MdParam);
        if (s32Ret < 0)
        {
            ZW_ERROR("Vda_Chn_Get_MdParam failed, ret=%d, Chn=%u\n", s32Ret, m_Chn);
            return -1;
        }

        if ((s32Ret = Vda_Chn_Get_MdData(0, m_Chn, &m_MdData)) < 0)
        {
            ZW_ERROR("[ERROR]: Vda_Chn_Get_MdData failed, ret=%d, Chn=%u\n", s32Ret, m_Chn);
            return s32Ret;
        }

        //底层宏块转换，转化为12*16的，与上层对齐
        for (i = 0; i < m_MdParam.MbCnt[1]; i++)//6   //8
        {
            for (j = 0; j < m_MdParam.MbCnt[0]; j++)//8   //16
            {
                // 判断用户设置的宏块是否在移动的宏块中
                if (1 == m_MdData.MArea[i*m_MdParam.MbCnt[0]+j])
                {
                    for (m = (SINT32)(i*(12.0f / m_MdParam.MbCnt[1]));m < (SINT32)((i+1)*(12.0f / m_MdParam.MbCnt[1])) ;m++)
                    {
                        for(n = (SINT32)(j*(16.0f / m_MdParam.MbCnt[0]));n < (SINT32)((j+1)*(16.0f / m_MdParam.MbCnt[0])) ;n++)
                        {
                            if (m < 12 && n < 16)
                            {
                                if (pMotion[m*16+n])
                                {
                                    s32MatchNum++;
                                }
                            }
                        }
                    }
                }
            }
        }

        ZW_DEBUG("BlkNum = %d, s32MatchNum = %d\n", m_MdData.BlkNum, s32MatchNum);
        if (s32MatchNum > 1)
        {
            //ZW_DEBUG("Get Motion Detect Event\n");
            *motion = 1;
            *Chn = m_Chn;
            return 0;
        }
        else
        {
            //ZW_DEBUG("NO have Motion Detect Event\n");
            *motion = 0;
        }
    }
    return 0;
}

/**
 * @description:设置抗闪强度
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} strength 用户设置的抗闪强度
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetAntiFlickerStrengthParam(void *hChannelHandle, int strength)
{
    SINT32 s32Ret = 0;
    ISP_TUNE_AE_PARAM_S AeParam = {0};
    CPlatformChannelContext  *pPlatCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    pPlatCtx = (CPlatformChannelContext*)hChannelHandle;
    ZW_INFO("=============into Hi_MPP_SetAntiFlickerStrengthParam===========[%d]\n", strength);
    s32Ret = Hi_MPP_GetAEParam(hChannelHandle, &AeParam);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_GetAEParam failed\n");
    }

    switch (strength)
    {
        //关闭
        case 0:
            AeParam.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_OFF;
            break;
        //弱
        case 1:
        case 50:
            AeParam.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_LOW;
            break;
        //强
        case 2:
        case 100:
            AeParam.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_HIGH;
            break;
        default:
            ZW_ERROR("FlickerMode error\n");
            return -1;
    }

    ZW_INFO("Anti Flicker Strength %d \n", AeParam.FlickerStrength);

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneAutoExposure(nSenIndex, &AeParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneAutoExposure fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }
    memcpy(&pPlatCtx->m_AeParam, &AeParam, sizeof(ISP_TUNE_AE_PARAM_S));

    ZW_DEBUG("Hi_MPP_SetAntiFlickerStrengthParam SUCCESS\n");

    return s32Ret;

}

int Hi_MPP_SetLocalIrcutMode(void *hChannelHandle, int mode)
{
    return 0;
}

/* 用饱和度模拟设置彩转黑*/
int Hi_MPP_SimulateColor2Grey(void *hChannelHandle, int enable)
{
    CPlatformChannelContext 	*pChannelContext = (CPlatformChannelContext 	*)hChannelHandle;

    pChannelContext->m_DayNightMode = enable;
    return 0;
}

int Hi_MPP_VO_StopDev(UINT32 VoDev)
{
    return 0;
}

int Hi_MPP_VO_StopLayer(UINT32 VoLayer)
{
    return 0;
}

int Hi_MPP_VO_StopChn(UINT32 VoLayer, VO_MODE_E enMode)
{
    return 0;
}

/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
int Hi_MPP_VENC_Stop(void *hChannelHandle, UINT32 VencChn)
{
    return 0;
}

//!\brief  使能人形功能（需要先进行参数配置关闭人形实际并未反初始化人形算法
//!\praram in hChannelHandle:
//!\praram in bEnable
//!\return  成功为0，失败为-1
int Hi_MPP_SetHumanTrackEn(void *hChannelHandle, int bEnable)
{
    int nRet = 0;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext*)hChannelHandle;

    ZW_DEBUG("Call Hi_MPP_SetHumanTrackEn %d\n", bEnable);
    if (pPlatCtx->m_bHumanTrackEn != bEnable)
    {
        pPlatCtx->m_bHumanTrackEn = bEnable;
    }

    return nRet;
}

/**
 * @description: 关闭人形算法并反初始化
 * @param {void} * hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetHumanDetectDisEnable(void *hChannelHandle)
{
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext*)hChannelHandle;

    HY_Enc_GetEncChInfo(&pChCtx);

    if (1 == pPlatCtx->m_bHumanInited)
    {
        pPlatCtx->m_bHumanEnable = 0;
    }
    pChCtx->m_bStartHuman = 0;

    ZW_DEBUG("Call Hi_MPP_SetHumanDetectDisEnable\n");

    return 0;
}

#if (USE_ALGO)
static int ReadFile(char *szPath, char *szBuffer, int s32Len)
{
    FILE *fp = NULL;

    fp = fopen(szPath, "rb");
    if (NULL != fp)
    {
        fread(szBuffer, s32Len, 1, fp);
        fclose(fp);
    }

    return 0;
}

static int SetCfgToJsonByBool_key(cJSON *jsonRoot, char *szKey, int bValue)
{
    if (NULL != cJSON_GetObjectItem(jsonRoot, szKey))
    {
        cJSON_ReplaceItemInObject(jsonRoot, szKey, cJSON_CreateBool(bValue));
    }
    else
    {
        cJSON_AddBoolToObject(jsonRoot, szKey, bValue);
    }

    return 0;
}

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)

static int SetCfgToJsonByDouble_key(cJSON *jsonRoot, char *szKey, double fValue)
{
    if (NULL != cJSON_GetObjectItem(jsonRoot, szKey))
    {
        cJSON_ReplaceItemInObject(jsonRoot, szKey, cJSON_CreateNumber(fValue));
    }
    else
    {
        cJSON_AddNumberToObject(jsonRoot, szKey, fValue);
    }

    return 0;
}

static int SetCfgToJsonByDouble(cJSON *jsonRoot, char *szSection, char *szKey, double dValue)
{
    cJSON *jsonNode = NULL;

    jsonNode = cJSON_GetObjectItem(jsonRoot, szSection);
    if (NULL != jsonNode)
    {
        if (NULL != cJSON_GetObjectItem(jsonNode, szKey))
        {
            cJSON_ReplaceItemInObject(jsonNode, szKey, cJSON_CreateNumber(dValue));
        }
        else
        {
            cJSON_AddNumberToObject(jsonNode, szKey, dValue);
        }
    }
    else
    {
        cJSON *jsonItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(jsonItem, szKey, dValue);
        cJSON_AddItemToObject(jsonRoot, szSection, jsonItem);
    }

    return 0;
}

static void getCurRatio()
{
    double tmpRatio = 1.0f;
    double CurRatio = 1.0f;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext * pChCtx = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);

    Zoom_GetDragZoomRatio(&CurRatio);

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
    tmpRatio = CurRatio;
    pSysInfo->m_CurRatio = tmpRatio;
#else
    tmpRatio = (CurRatio - DRAG_ZOOM_MIN_RATIO) * (DRAG_ZOOM_MAX_RATIO-DRAG_ZOOM_MIN_RATIO) / (pChCtx->m_ZoomUserMaxRatio-DRAG_ZOOM_MIN_RATIO) + DRAG_ZOOM_MIN_RATIO;
    pSysInfo->m_CurRatio = sqrt(tmpRatio);
#endif

}
#endif

/*
 * 中心点在区域内则判断为在区域内
 */
static int pnpoly(float x, float y, POLYGON_FLOAT *reg)
{
    int i, j,c = 0;
    int len = reg->nValidNum;
    for (i = 0, j = (len-1); i < len; j = i++)
    {
        float b2 = (reg->point[j].x-reg->point[i].x) * (y-reg->point[i].y) / (reg->point[j].y-reg->point[i].y) + reg->point[i].x;
        if ( ((y < reg->point[i].y) != (y < reg->point[j].y)) && (x < b2))
        {
            c = !c;
        }
    }
    return c;
}

//灵敏度过滤,根据区域大小占比来
static int CheckTargetSenstive(RECT_S *rect_array, int iSenstive)
{
    return 1;
    if(NULL == rect_array)
    {
        ZW_ERROR("input param is error\n");
        return 0;
    }

    int iSenstiveRatio = 0;
    int iRatio = 0;

    switch (iSenstive)
    {
        case 95://最高
            iSenstiveRatio = 5;
            break;
        case 90://较高 APP高
            iSenstiveRatio = 10;
            break;
        case 80://高
            iSenstiveRatio = 20;
            break;
        case 60://中 APP中
            iSenstiveRatio = 30;
            break;
        case 30://较低 APP低
            iSenstiveRatio = 160;
            break;
        case 10://低
            iSenstiveRatio = 200;
            break;
        default:
            iSenstiveRatio = 30;
            break;
    }

    iRatio = 10000*(rect_array->u32Width*rect_array->u32Height)/(HUMAN_DETECT_W*HUMAN_DETECT_H);
    if(iRatio > iSenstiveRatio)
    {
        return 1;
    }

    return 0;
}

static int CheckTargetInArea(RECT_S *rect_array, AI_DETECT_DETECT_AREA_TYPE type)
{
    int i = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);
    POLYGON_FLOAT f_area = {0};
    float x = 0, y = 0;

    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;

    x = (rect_array->s32X + rect_array->u32Width/2.0) * 1.0;
    y = (rect_array->s32Y + rect_array->u32Height/2.0) * 1.0;

    memset(&f_area, 0, sizeof(POLYGON_FLOAT));

    f_area.nValidNum = pPlatCtx->m_HumanDetectCfg.rcDetectRegion[0].nValidNum;
    for (i = 0; i < f_area.nValidNum; i++)
    {
        f_area.point[i].x = pPlatCtx->m_HumanDetectCfg.rcDetectRegion[0].point[i].x * 1.0 * HUMAN_DETECT_W / MAX_HUMAN_SHAPE_REGION_WIDTH;
        f_area.point[i].y = pPlatCtx->m_HumanDetectCfg.rcDetectRegion[0].point[i].y * 1.0 * HUMAN_DETECT_H / MAX_HUMAN_SHAPE_REGION_HEIGHT;
    }

    if (1 == pnpoly(x, y, &f_area))
    {
        return 1;
    }

    return 0;
}


#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 2
static int EventProcess(int Chn,E_AI_EVENT_TYPE eEventType, void* pData, void* userData)
{
#else
static int EventProcess(E_AI_EVENT_TYPE eEventType, void* pData, void* userData)
{
    int Chn = 0;
#endif
    int i = 0;
    int Cnt = 0;
    unsigned int nObjectSize = 0;
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);

    ZW_DEBUG("eEventType:[%d] Chn[%d]\n", eEventType, Chn);

     switch (eEventType)
    {
        case AI_EVENT_OBJDETECT:
        {
            RECT_S rectInfo;
            AISDK_DEF_S_EVENT_OBJDETECT *pOBJResult = (AISDK_DEF_S_EVENT_OBJDETECT*)pData;
            nObjectSize = (pOBJResult->uObjNum > HCHD_DETECT_MAX ? HCHD_DETECT_MAX : pOBJResult->uObjNum);

            if (Chn >= ALGO_CHANNEL)
            {
                ZW_ERROR("g_nAlgoChnId [%d] Failed!!!!!!\n", Chn);
                break;
            }

            XTRACE_MUTEX_LOCK(&g_human_result_lock);
            for (i = 0; i < nObjectSize; i++)
            {
                if (2 != pOBJResult->uObjCategory[i])
                {
                    continue;
                }
                rectInfo.s32X      = pOBJResult->stObjRect[i].left;
                rectInfo.s32Y      = pOBJResult->stObjRect[i].top;
                rectInfo.u32Width  = pOBJResult->stObjRect[i].right - pOBJResult->stObjRect[i].left;
                rectInfo.u32Height = pOBJResult->stObjRect[i].bottom - pOBJResult->stObjRect[i].top;

                //区域过滤
                if (CheckTargetInArea(&rectInfo, AREA_TYPE_ALERT_AREA) == 0)
                {
                    continue;
                }

                // 灵敏度过滤
                if(CheckTargetSenstive(&rectInfo, (int)pChCtx->m_stHumanShapeCfgV2.nSenstive) == 0)
                {
                    continue;
                }

                memcpy(&(g_HumanResult[Chn].rectInfo[Cnt]), &rectInfo, sizeof(RECT_S));
                g_HumanResult[Chn].IsMove[Cnt] = pOBJResult->uStatusActionless[i];
                g_HumanResult[Chn].trackId[Cnt] = pOBJResult->u64ObjId[i];
                g_HumanResult[Chn].nSnsId = Chn;
                Cnt++;
            }
            g_HumanResult[Chn].num = Cnt;
            XTRACE_MUTEX_UNLOCK(&g_human_result_lock);
            break;
        }
		
        default:
        {
            ZW_ERROR("Not Support Event Type(%d)\n", eEventType);
            break;
        }
    }

    return 0;
}

static int FullFrame(int frameid, unsigned long long virAddr, unsigned long long phyAddr, YUV_FRAME_INFO_S* pstFrame)
{
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;

    struct timeval  tv = {0};

    gettimeofday(&tv, NULL);

    pstFrame->nStatus = 0;
    pstFrame->nFrameId = frameid;
    pstFrame->uYuvFormat = 2;
    pstFrame->nWidth = pPlatCtx->m_stVpssChnAttr[0][AI_STREAM].VpssParam.width;
    pstFrame->nHeight = pPlatCtx->m_stVpssChnAttr[0][AI_STREAM].VpssParam.height;
    pstFrame->u32RealTimeSec = tv.tv_sec;
    pstFrame->u32RealTimeUSec = tv.tv_usec;
    pstFrame->uFrameBufVir = virAddr;
    pstFrame->uFrameBufPhy = phyAddr;

    return 0;
}
#endif

//设置人形侦测配置
int Hi_MPP_SetHumanDetectCfg(void *hChannelHandle)
{
#if (USE_ALGO)
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext*)hChannelHandle;

    char *jsonStr = NULL;
    cJSON *jsonRoot = NULL;
    char jsonBuffer[4096];
    int s32Ret = 0;

    int nEnable = pPlatCtx->m_bHumanEnable;
    double dScoreThr = (double)pPlatCtx->m_HumanDetectCfg.dScoreThr;

    memset(jsonBuffer, 0, sizeof(jsonBuffer));
    // 把json文件内容读取到buffer中
    ReadFile(ALGO_JSON_PATH, jsonBuffer, 4096);

    for (int SnsIdx = 0; SnsIdx < ALGO_CHANNEL; SnsIdx++)
    {
        jsonRoot = cJSON_Parse(jsonBuffer);
        if (jsonRoot == NULL)
        {
            ZW_ERROR("cJSON_Parse error\n");
            return -1;
        }

        cJSON *start_param = cJSON_CreateObject();

#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 2
        cJSON_AddNumberToObject(start_param, HUMAN_DETECT_START_PARAM_AICHN_NUM, ALGO_CHANNEL);
        cJSON_AddItemToObject(jsonRoot, HUMAN_DETECT_START_PARAM, start_param);

#else
        cJSON_AddNumberToObject(start_param, "AiChnNum", ALGO_CHANNEL);
        cJSON_AddItemToObject(jsonRoot, "start_param", start_param);
#endif

        SetCfgToJsonByBool_key(jsonRoot, HUMAN_DETECT_PUSH_VEHICLE_DETECT, 0);
        if (nEnable) {
            SetCfgToJsonByBool_key(jsonRoot, HUMAN_DETECT_PUSH_HUMAN_DETECT, 1);
        }
        else {
            SetCfgToJsonByBool_key(jsonRoot, HUMAN_DETECT_PUSH_HUMAN_DETECT, 0);
        }

        cJSON *human = cJSON_CreateObject();
        cJSON_AddNumberToObject(human, HUMAN_DETECT_ALGO_PARAM_HUMAN_THRESHOLD, (double)dScoreThr);
        cJSON_AddNumberToObject(human, HUMAN_DETECT_ALGO_PARAM_HUMAN_TRACK_DIE_FRAMENUM, 8);

        cJSON *AlgoParam = cJSON_CreateObject();
        cJSON_AddItemToObject(AlgoParam, HUMAN_DETECT_ALGO_PARAM_HUMAN, human);
        cJSON_AddItemToObject(jsonRoot, HUMAN_DETECT_ALGO_PARAM, AlgoParam);

        SetCfgToJsonByDouble(jsonRoot, JSON_BODY_DETECT, JSON_BODY_DETECT_QUALITY, (double)80);
        SetCfgToJsonByDouble_key(jsonRoot, HUMAN_DETECT_EN_FILT_ACTIONLESS, 1);
        SetCfgToJsonByDouble_key(jsonRoot, HUMAN_DETECT_FILT_ACTIONLESS_TARGET_DISTANCELEN, ALGO_ACTIONLESS_TARGET_DISTANCELEN);
        SetCfgToJsonByDouble_key(jsonRoot, HUMAN_DETECT_FILT_ACTIONLESS_TARGET_ZOOM_W, ALGO_ACTIONLESS_TARGET_ZOOM_W);
        SetCfgToJsonByDouble_key(jsonRoot, HUMAN_DETECT_FILT_ACTIONLESS_TARGET_ZOOM_H, ALGO_ACTIONLESS_TARGET_ZOOM_H);

        // 打印json文件
        jsonStr = cJSON_Print(jsonRoot);
        //ZW_INFO("SnsIdx=[%d], jsonStr=%s\n", SnsIdx, jsonStr);

#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 2
        s32Ret = AISDK_Cfg(SnsIdx, AI_SCENE_HD, AICFG_TYPE_JSON, jsonStr, strlen(jsonStr)+1);
#else
        s32Ret = AISDK_Cfg(AI_SCENE_HD, AICFG_TYPE_JSON, jsonStr, strlen(jsonStr) + 1);
#endif

        if (s32Ret)
        {
            ZW_ERROR("AISDK_Cfg failed, ret=%#x, Chn[%d]\n", s32Ret, SnsIdx);
        }

        free(jsonStr);
        jsonStr = NULL;

        cJSON_Delete(jsonRoot);
        jsonRoot = NULL;
    }
#endif
    return 0;
}

//!\brief  使能人形功能（需要先进行参数配置关闭人形实际并未反初始化人形算法
//!\praram in hChannelHandle:
//!\praram in bEnable
//!\return  成功为0，失败为-1
int Hi_MPP_SetHumanDetectEnable(void *hChannelHandle, int bEnable)
{
    int nRet = 0;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext*)hChannelHandle;

    ZW_DEBUG("Call Hi_MPP_SetHumanDetectEnable %d\n", bEnable);
    if(pPlatCtx->m_bHumanEnable != bEnable)
    {
        if(bEnable)
        {
            pPlatCtx->m_bHumanEnable = 1;
            Hi_MPP_SetHumanDetectCfg(hChannelHandle);
        }
        else
        {
            pPlatCtx->m_bHumanEnable = 0;
            Hi_MPP_SetHumanDetectCfg(hChannelHandle);
        }
    }

    return nRet;
}

/**
 * @brief  设置算法参数
 * @param  hChannelHandle 平台编码句柄
 * @return int 等于0为正常，其他值为异常
 */
static int LoadAlgoParame(void *hChannelHandle)
{
    CChannelContext *pChCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);
#if (USE_ALGO)
    ZW_INFO("LoadAlgoParame start\n");

    if (pChCtx->m_bStartMidMotion)
    {
        Hi_MPP_StartMD(hChannelHandle);
    }
    else
    {
        Hi_MPP_StopMD(hChannelHandle);
    }

    ZW_INFO("LoadAlgoParame end\n");
#endif
    return 0;
}


static int Ai_Algo_Initial(void *hChannelHandle, E_AI_SCENE eAiScene)
{
#if (USE_ALGO)
    SINT32 ret = 0;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext*)hChannelHandle;
    AISDK_DEF_INIT_PARAM stInitParam = {0};

    ZW_INFO("Ai_Algo_Initial start %x\n",eAiScene);

    if (0 == pPlatCtx->m_bAiAlgoInited)
    {
        memset(&stInitParam,0,sizeof(stInitParam));
        strncpy(stInitParam.model_path,ALGO_MODELS_DIR,sizeof(stInitParam.model_path) - 1);
        stInitParam.ai_stream_h = ALGO_STREAM_HEIGHT;
        stInitParam.ai_stream_w = ALGO_STREAM_WIDTH;

        ZW_INFO("AISDK_Init nScene=0x%x\n", eAiScene);
        ret = AISDK_Init(AICFG_TYPE_STRUCT, (char*)&stInitParam, sizeof(stInitParam));
        if (0 != ret)
        {
            ZW_ERROR("AISDK_Init failed ret:%d\n", ret);
            return -1;
        }

        /* AISDK_Start 支持多通道算法需要pCfgBuf json格式的入参 */
        {
            // 创建根对象
            cJSON *root = cJSON_CreateObject();
            // 创建start_param对象
            cJSON *start_param = cJSON_CreateObject();

            // 向start_param对象中添加AiChnNum字段
            if (!cJSON_AddNumberToObject(start_param, "AiChnNum", ALGO_CHANNEL)) {
                cJSON_Delete(start_param);
                cJSON_Delete(root);
            }

            // 将start_param对象添加到根对象中
            if (!cJSON_AddItemToObject(root, "start_param", start_param)) {
                cJSON_Delete(start_param);
                cJSON_Delete(root);
            }
            char *json_str = cJSON_Print(root);

            ZW_INFO("AISDK_Init nScene=0x%x, json_str=[%s]\n", eAiScene, json_str);
            ret = AISDK_Start(eAiScene, AICFG_TYPE_JSON, json_str, 0);

            // 释放资源
            cJSON_Delete(root);
            free(json_str);

            if (0 != ret)
            {
                ZW_ERROR("[%s %d] AISDK_Start fail, ret = %#x\n", __FUNCTION__, __LINE__, ret);
                pthread_mutex_unlock(&pPlatCtx->m_humanWorklock);
                return -1;
            }
        }

        ret = AISDK_RegisterEventCallback(EventProcess, (void *)0xabcd1234);
        if (0 != ret)
        {
            ZW_ERROR("[%s %d] AISDK_RegisterEventCallback fail, ret = %#x\n", __FUNCTION__, __LINE__, ret);
            return -1;
        }

        pPlatCtx->m_bAiUseingAlgo = eAiScene;
        pPlatCtx->m_bAiAlgoInited = 1;
        Hi_MPP_SetHumanDetectCfg(hChannelHandle);
    }

    ZW_INFO("Ai_Algo_Initial end\n");
#endif

    return 0;
}

static int Ai_Algo_Uninitial()
{
#if (USE_ALGO)
        int ret = 0;
        ret = AISDK_Stop();
        if (0 != ret)
        {
            ZW_ERROR("[%s %d] AISDK_Uninit fail, ret = %#x\n", __FUNCTION__, __LINE__, ret);
            return -1;
        }
        ret = AISDK_Uninit();
        if (0 != ret)
        {
            ZW_ERROR("[%s %d] AISDK_Uninit fail, ret = %#x\n", __FUNCTION__, __LINE__, ret);
            return -1;
        }
    ZW_INFO("Ai_Algo_Uninitial end\n");
#endif

    return 0;
}

int Hi_MPP_AlgoInit(void *hChannelHandle)
{
    int s32Ret = -1;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx= NULL;
    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    XTRACE_MUTEX_LOCK(&pPlatCtx->m_humanResultsLock);
    s32Ret = Ai_Algo_Initial(pPlatCtx, AI_SCENE_HD);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Xrs_Algo_Initial fail");
    XTRACE_MUTEX_UNLOCK(&pPlatCtx->m_humanResultsLock);
    return s32Ret;
}

int Hi_MPP_AlgounInit()
{
    int s32Ret = 0;
    s32Ret = Ai_Algo_Uninitial();
    return s32Ret;
}



//!\brief  人形用户配置，支持多边形区域
//!\praram in hChannelHandle 句柄
//!\return  成功为0，失败为-1
int Hi_MPP_SetIVPCfgV2(void *hChannelHandle, HumanShapeCfgV2 *pHumanDetectUserCfg)
{
    int nRet = 0;
    CPlatformChannelContext *pChContext = (CPlatformChannelContext*)hChannelHandle;

    pChContext->m_HumanDetectCfg.nSenstive = pHumanDetectUserCfg->nSenstive;
    pChContext->m_HumanDetectCfg.nDetectRegionNum = pHumanDetectUserCfg->nDetectRegionNum;
    memcpy(&pChContext->m_HumanDetectCfg.rcDetectRegion[0], &pHumanDetectUserCfg->rcDetectRegion[0], sizeof(POLYGON) * MAX_HUMAN_SHAPE_REGION_NUM);

    switch (pHumanDetectUserCfg->nSenstive)
    {
        case 2://高
            pChContext->m_HumanDetectCfg.dScoreThr = 0.35;
            break;
        case 3://中
            pChContext->m_HumanDetectCfg.dScoreThr = 0.55;
            break;
        case 4://低
            pChContext->m_HumanDetectCfg.dScoreThr = 0.90;
            break;
        default:
            pChContext->m_HumanDetectCfg.dScoreThr = 0.55;
    }
    Hi_MPP_SetHumanDetectCfg(hChannelHandle);

    return nRet;
}

int Hi_MPP_GetBitrateInfo(void *hChannelHandle, int dataArr[], int arrlen)
{
    int i = 0;
    if((NULL == hChannelHandle) || NULL == dataArr || arrlen < 0)
    {
        return ERR_INVALID_ARGUMENT;
    }
    CPlatformChannelContext *pPlatChnCxt = (CPlatformChannelContext*)hChannelHandle;

    for (i = 0; i < (arrlen>MAX_STREAM_COUNT?MAX_STREAM_COUNT:arrlen); i++)
    {
        dataArr[i] = pPlatChnCxt->m_MppRCInfo[i].m_bit_rate;
    }

    return 0;
}

void *Hi_MPP_HDThread(void * lpParameter)
{
#if (USE_ALGO)
    PRT_TID_NAME("HDThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    if (NULL == lpParameter)
    {
        ZW_ERROR("lpParameter CANNOT be NULL\n");
        return NULL;
    }

    ThreadInfo_t *pParent = (ThreadInfo_t *)lpParameter;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    UINT32 sleepUs = 1 * 1000;/*fps=8*/
    VIDEO_FRM_ALG_S VFrmAlg = {0};
    SINT32 RtVal = 0;
    YUV_FRAME_INFO_S stFrameInfo = {0};
    unsigned int Chn = 0;
    HY_Enc_GetEncChInfo(&pChCtx);
    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    memset(pPlatCtx->m_humanResults,0,sizeof(pPlatCtx->m_humanResults));
    sleep(2);   //确保在加载效果文件之后跑人形算法

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        usleep(sleepUs);
        if (1 == pSysInfo->m_bIsQrScanEnable)
        {
            continue;
        }
        //系统完全起来后再设置,否则先保存使能状态
        if ((pChCtx->m_IrcutSwitchFlag) && (0 == pPlatCtx->m_bAiAlgoInited))
        {
            for(int i = 0;i < ALGO_CHANNEL;i++)
            {
                pChCtx->m_bHumanShapeState[i] = FALSE;
                if(i == 0)
                pChCtx->m_bHumanStateEx = pChCtx->m_bHumanShapeState[i];
            }
            pChCtx->m_bHumanStateIsValue = TRUE;
            //由于算法初始化在配置下发之后，这里算法初始化完后，需要重新设置一遍到AISDK中
            LoadAlgoParame(pPlatCtx);
        }

        if (1 == pPlatCtx->m_bAiAlgoInited)
        {
            for (Chn = 0; Chn < ALGO_CHANNEL; Chn++)
            {
                if (0 == pPlatCtx->m_stVpssChnAttr[Chn][AI_STREAM].VpssParam.SrcChn)
                    continue;

                XTRACE_MUTEX_LOCK(&pChCtx->m_InitVPSSLock);
                RtVal = Vpss_Chn_Get_Frm(0,pPlatCtx->m_VPSSGroupNum[Chn], pPlatCtx->m_stVpssChnAttr[Chn][AI_STREAM].VpssParam.SrcChn, &VFrmAlg, 1000);
                if (RtVal)
                {
                    ZW_ERROR("Vpss_Chn_Get_Frm FAIL(%d) Chn(%d)\n",RtVal,Chn);
                    XTRACE_MUTEX_UNLOCK(&pChCtx->m_InitVPSSLock);
                    continue;
                }

                if (Chn == 0)
                {
                    if(1 == pSysInfo->m_bSupportMotor && pPlatCtx->m_bHumanTrackEn == 1)
                    {
                        pSysInfo->m_pMotoSavePosCallback();
                    }

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
                    getCurRatio();
#endif
                }

                //叠加马赛克，以IVS方式获取nv12数据需要16对齐偏移，这里需要申请一块连续的内存存放yuv数据
                FullFrame(VFrmAlg.u64SeqNum, VFrmAlg.u64VirAddr[0], VFrmAlg.u64PhyAddr[0], &stFrameInfo);

                //SaveYuvData((char *)stFrameInfo.uFrameBufVir,stFrameInfo.nWidth * stFrameInfo.nHeight * 3 /2,"test");
                //ZW_INFO("nWidth(%d) nHeight(%d) u32FrameSize(%d)\n",stFrameInfo.nWidth,stFrameInfo.nHeight,VFrmAlg.u32FrameSize);
                if (0 != stFrameInfo.uFrameBufVir && 0 != stFrameInfo.uFrameBufPhy)
                {
                    #if (USE_ALGO)
                    // ZW_DEBUG("Send to AISDK!\n");

#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 2
                    RtVal = AISDK_SendDetectYUV(Chn,&stFrameInfo);
#else
                    RtVal = AISDK_SendDetectYUV(&stFrameInfo);
#endif
                    // ZW_DEBUG("Send to AISDK out!\n");
                    if (RtVal)
                    {
                        ZW_ERROR("AISDK_SendDetectYUV FAIL(%d) Chn(%d)\n",RtVal,Chn);
                    }
                    #endif
                }
                Vpss_Chn_Rel_Frm(0, pPlatCtx->m_VPSSGroupNum[Chn],pPlatCtx->m_stVpssChnAttr[Chn][AI_STREAM].VpssParam.SrcChn, &VFrmAlg);
                //ZW_DEBUG("sleep:%lld-", Time1-Time4);
                XTRACE_MUTEX_UNLOCK(&pChCtx->m_InitVPSSLock);
                sleepUs = 1 * 1000;
            }
        }
        else /* IVP初始化失败或者人脸、人形检测功能都关闭 */
        {
            XTRACE_MUTEX_LOCK(&pPlatCtx->m_humanResultsLock);
            for(int Sns = 0;Sns < ALGO_CHANNEL;Sns++)
            {
                pPlatCtx->m_humanResults[Sns].num = 0;
                pPlatCtx->m_humanResults[Sns].valid = 0;
            }
            XTRACE_MUTEX_UNLOCK(&pPlatCtx->m_humanResultsLock);

            sleepUs = 10 * 1000;
        }
    }

    ZW_DEBUG("Hi_MPP_HDThread Exit \n");
#endif

    return NULL;
}

int Hi_MPP_LoadAlgoParame(void *hChannelHandle)
{
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("Params CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    LoadAlgoParame(pPlatCtx);

    return 0;
}

static int DrawLineColorSeletct(void *hChannelHandle, int nDayNightMode, int haveTarget)
{
    int nColor = 0;

    if (0 == haveTarget)
    {
        nColor = VCOLOR_RGBA_TURQUOISE;
    }
    else
    {
        nColor = VCOLOR_RGBA_RED;
    }

    return nColor;
}

/* 用于跟平台之间做类型转换 */
static VOSD_CHNTYPE_E __COMMON_VOSD_CHN_TYPE(DEV_SUPPORT_SENSOR Sns, ENCODE_STREAM_E eStream)
{
    VOSD_CHNTYPE_E eChnType = VOSD_CHNTYPE_BUTT;

    //MJPEG流只存在于主Sensor
    if (eStream == FOURTH_STREAM && Sns >= ALGO_CHANNEL)
    {
        return eChnType;
    }

    switch (Sns)
    {
    case FIRST_SENSOR:
        {
            switch (eStream)
            {
                case FIRST_STREAM:  eChnType = VOSD_CHNTYPE_VENC_MAIN;  break;
                case SECOND_STREAM: eChnType = VOSD_CHNTYPE_VENC_MINOR; break;
                case FOURTH_STREAM: eChnType = VOSD_CHNTYPE_VENC_SNAP;  break;
                default: break;
            }
        }
        break;
#if SYSTEM_SUPPORT_CHANNEL >= 2
    case SEC_SENSOR:
        {
            switch (eStream)
            {
                case FIRST_STREAM:  eChnType = VOSD_CHNTYPE_VENC_MAIN_SEC;  break;
                case SECOND_STREAM: eChnType = VOSD_CHNTYPE_VENC_MINOR_SEC; break;
                case FOURTH_STREAM: eChnType = VOSD_CHNTYPE_VENC_SNAP_SEC;  break;
                default: break;
            }
        }
        break;
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 3
    case THIRD_SENSOR:
        {
            switch (eStream)
            {
                case FIRST_STREAM:  eChnType = VOSD_CHNTYPE_VENC_MAIN_THIRD;  break;
                case SECOND_STREAM: eChnType = VOSD_CHNTYPE_VENC_MINOR_THIRD; break;
                case FOURTH_STREAM: eChnType = VOSD_CHNTYPE_VENC_SNAP_THIRD;  break;
                default: break;
            }
        }
        break;
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 4
    case FOURTH_SENSOR:
        {
            switch (eStream)
            {
                case FIRST_STREAM:  eChnType = VOSD_CHNTYPE_VENC_MAIN_FOURTH;  break;
                case SECOND_STREAM: eChnType = VOSD_CHNTYPE_VENC_MINOR_FOURTH; break;
                case FOURTH_STREAM: eChnType = VOSD_CHNTYPE_VENC_SNAP_FOURTH;  break;
                default: break;
            }
        }
        break;
#endif
    default:
        break;
    }

    if (eChnType == VOSD_CHNTYPE_BUTT)
    {
        //ZW_ERROR("Parameter Error! Sns[%d] eStream[%d]\n", Sns, eStream);
    }

    return (VOSD_CHNTYPE_E)eChnType;
}


/**
 * @brief  清除所有画框画线信息
 * @return int 0:正常, -1:接口报错
 */
int Hi_MPP_ClearDrawLineInfo(int ClearType)
{
    int i = 0;
    VFACE_FRAME_S VFaceFrm = {0};
    int VosdChnType = 0;
    int Sns = 0;
    for (Sns = 0; Sns < ALGO_CHANNEL; Sns++)
    {
        for (i = 0; i <= SECOND_STREAM; i++)
        {
             VosdChnType = __COMMON_VOSD_CHN_TYPE(Sns, i);
            if (VOSD_CHNTYPE_BUTT == VosdChnType)
            {
                continue;
            }
            memset(&VFaceFrm,0,sizeof(VFACE_FRAME_S));
            if(ClearType)
            {
                g_DrawRectFlag[Sns][i] = 0;
                Vosd_Chn_Del_Rect(0, VosdChnType, 0);
            }
            else
            {
                if(g_DrawRectFlag[Sns][i])
                {
                    g_DrawRectFlag[Sns][i] = 0;
                    Vosd_Chn_Del_Rect(0, VosdChnType, 0);
                }
            }
        }
    }
    return 0;
}

/**
 * @description: 子码流采用在线的方式进行画线画框(主子码流清除画框都在这里处理)
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} *pHumanAreaOverlayEnable 人形目标框叠加使能
 * @param {int} *pDetectAreaOverlayEnable 布防框叠加使能
 * @param {int} *pCarAreaOverlayEnable 车型框叠加使能
 * @param {int} *pFireAreaOverlayEnable 火焰框叠加使能
 * @param {int} motionDetectEnable 移动侦测使能
 * @param {int} motioning 是否正在移动
* @param {int}  callCycleMs 调用的周期
 * @return {SINT32} 正常返回0，异常返回<0
 */
int Hi_MPP_ReadyDrawLineInfo(void *hChannelHandle, int *pHumanAreaOverlayEnable, int motionDetectEnable, int motioning, int callCycleMs)
{
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext*)hChannelHandle;
    CChannelContext *pChCtx = NULL;
    int s32Ret = 0;
    VFACE_FRAME_S VFaceFrm = {0};
    static human_struct alertResults[ALGO_CHANNEL] = {0};
    human_struct targetResults[ALGO_CHANNEL] = {0};
    int i = 0, j = 0;
    int nSenIndex = 0;
    int VosdChnType = 0;
    int	nowDayNightMode = 0;
    UINT32 ifHaveTarget = 0;
    static int sDelayShowTargetTimeMs[ALGO_CHANNEL] = {0}; //目标消失后框延迟显示的时间
    HY_Enc_GetEncChInfo(&pChCtx);

    #if 1
    if (((0 == motionDetectEnable)
        && (0 == *(pHumanAreaOverlayEnable + FIRST_STREAM))
        && (0 == *(pHumanAreaOverlayEnable + SECOND_STREAM))
        && (0 == *(pHumanAreaOverlayEnable + THIRD_STREAM)))
        || 0 == pChCtx->m_HumanDetectSchedule)
    {
        /* 清除所有框 */
        goto onlyClearLine;
    }
    #endif

    /* 取警戒类功能结果 */
    XTRACE_MUTEX_LOCK(&pPlatCtx->m_humanResultsLock);
    for (nSenIndex = FIRST_SENSOR; nSenIndex < ALGO_CHANNEL; nSenIndex++)
    {
        if (1 == pPlatCtx->m_humanResults[nSenIndex].valid)
        {
            sDelayShowTargetTimeMs[nSenIndex] = 0;
            memcpy(&alertResults[nSenIndex], &pPlatCtx->m_humanResults[nSenIndex], sizeof(human_struct));
            pPlatCtx->m_humanResults[nSenIndex].valid = 0;//表示数据已取
        }
    }
    XTRACE_MUTEX_UNLOCK(&pPlatCtx->m_humanResultsLock);

    for (nSenIndex = FIRST_SENSOR; nSenIndex < ALGO_CHANNEL; nSenIndex++)
    {
        if (alertResults[nSenIndex].num > 0)
        {
            alertResults[nSenIndex].num = alertResults[nSenIndex].num > HCHD_DETECT_MAX ? HCHD_DETECT_MAX : alertResults[nSenIndex].num;
            memcpy(targetResults[nSenIndex].rectInfo, alertResults[nSenIndex].rectInfo, sizeof(RECT_S) * alertResults[nSenIndex].num);
            targetResults[nSenIndex].num = alertResults[nSenIndex].num;
        }

        /* 画框结果暂留5次,避免用完一次清除一次框一直闪,现在结果是通过回调通知方式,没有人时清空数据在此处处理 */
        sDelayShowTargetTimeMs[nSenIndex]++;
        if (5 == sDelayShowTargetTimeMs[nSenIndex])
        {
            sDelayShowTargetTimeMs[nSenIndex] = 0;

            pthread_mutex_lock(&pPlatCtx->m_humanResultsLock);
            memset(&(pPlatCtx->m_humanResults[nSenIndex]), 0, sizeof(human_struct));
            pthread_mutex_unlock(&pPlatCtx->m_humanResultsLock);

            memset(&(alertResults[nSenIndex]), 0, sizeof(human_struct));
        }
    }

    /* 当前白天夜晚状态 */
    nowDayNightMode = pChCtx->m_DayNightStutas;
    /*4通道OSD叠加，需要遍历4次*/
    for(nSenIndex = 0;nSenIndex < ALGO_CHANNEL;nSenIndex++)
    {
        /* 更新目标框及布防框数据 */
        for (i = 0; i <= SECOND_STREAM; i++)
        {
            VosdChnType = __COMMON_VOSD_CHN_TYPE(nSenIndex, i);
            if (VOSD_CHNTYPE_BUTT == VosdChnType)
            {
                continue;
            }

            /* 目标框处理 */
            if (targetResults[nSenIndex].num > 0)
            {
                VFaceFrm.RectCnt = 0;
                VFaceFrm.LineW = (i == 0) ? MAIN_STREAM_LINE_THICK : SUB_STREAM_LINE_THICK;
                for (j = 0; j < targetResults[nSenIndex].num; j++)
                {
                    if (VFaceFrm.RectCnt >= MAX_VFACE_FRAME_NUM)
                    {
                        break;
                    }

                    /* 过滤全0的目标框 */
                    if ((0 == targetResults[nSenIndex].rectInfo[j].s32X && 0 == targetResults[nSenIndex].rectInfo[j].s32Y
                        && 0 == targetResults[nSenIndex].rectInfo[j].u32Width && 0 == targetResults[nSenIndex].rectInfo[j].u32Height))
                    {
                        continue;
                    }

                    VFaceFrm.Color[VFaceFrm.RectCnt] = DrawLineColorSeletct(pPlatCtx, nowDayNightMode, ifHaveTarget);
                    VFaceFrm.Rect[VFaceFrm.RectCnt].x = targetResults[nSenIndex].rectInfo[j].s32X * pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.width / HUMAN_DETECT_W;
                    VFaceFrm.Rect[VFaceFrm.RectCnt].y = targetResults[nSenIndex].rectInfo[j].s32Y * pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.height / HUMAN_DETECT_H;
                    VFaceFrm.Rect[VFaceFrm.RectCnt].w = targetResults[nSenIndex].rectInfo[j].u32Width * pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.width / HUMAN_DETECT_W;
                    VFaceFrm.Rect[VFaceFrm.RectCnt].h = targetResults[nSenIndex].rectInfo[j].u32Height * pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.height / HUMAN_DETECT_H;

                    if (VFaceFrm.Rect[VFaceFrm.RectCnt].x  + VFaceFrm.LineW >= pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.width - 1)
                    {
                        VFaceFrm.Rect[VFaceFrm.RectCnt].x  = pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.width - VFaceFrm.LineW - 1;
                    }

                    if (VFaceFrm.Rect[VFaceFrm.RectCnt].y  + VFaceFrm.LineW >= pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.height - 1)
                    {
                        VFaceFrm.Rect[VFaceFrm.RectCnt].y  = pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.height - VFaceFrm.LineW - 1;
                    }

                    if (VFaceFrm.Rect[VFaceFrm.RectCnt].x + VFaceFrm.Rect[VFaceFrm.RectCnt].w  + VFaceFrm.LineW >= pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.width - 1)
                    {
                        VFaceFrm.Rect[VFaceFrm.RectCnt].w  = pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.width - VFaceFrm.LineW - 1 - VFaceFrm.Rect[VFaceFrm.RectCnt].x;
                    }

                    if (VFaceFrm.Rect[VFaceFrm.RectCnt].y  + VFaceFrm.Rect[VFaceFrm.RectCnt].h  + VFaceFrm.LineW >= pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.height - 1)
                    {
                        VFaceFrm.Rect[VFaceFrm.RectCnt].h  = pPlatCtx->m_stVencChnAttr[nSenIndex][i].EncParam.height - VFaceFrm.LineW - 1 - VFaceFrm.Rect[VFaceFrm.RectCnt].y;
                    }

                    VFaceFrm.Rect[VFaceFrm.RectCnt].x = VFaceFrm.Rect[VFaceFrm.RectCnt].x / 2 * 2;
                    VFaceFrm.Rect[VFaceFrm.RectCnt].y = VFaceFrm.Rect[VFaceFrm.RectCnt].y / 2 * 2;
                    VFaceFrm.Rect[VFaceFrm.RectCnt].w = VFaceFrm.Rect[VFaceFrm.RectCnt].w / 2 * 2;
                    VFaceFrm.Rect[VFaceFrm.RectCnt].h = VFaceFrm.Rect[VFaceFrm.RectCnt].h / 2 * 2;

                    ZW_DEBUG("[stream:%d] [max:%d] [color:%d] [width:%d] [rect%d:%d-%d-%d-%d]\n", i, targetResults[nSenIndex].num, VFaceFrm.Color[VFaceFrm.RectCnt],
                         VFaceFrm.LineW, VFaceFrm.RectCnt, VFaceFrm.Rect[VFaceFrm.RectCnt].x, VFaceFrm.Rect[VFaceFrm.RectCnt].y,
                         VFaceFrm.Rect[VFaceFrm.RectCnt].w, VFaceFrm.Rect[VFaceFrm.RectCnt].h);
                    VFaceFrm.RectCnt++;
                    if(VFaceFrm.RectCnt >= MAX_VFACE_FRAME_NUM)
                        {break;}
                }
            }
            else
            {
                VFaceFrm.RectCnt = 0;
                memset(VFaceFrm.Rect, 0, sizeof(VRECT_S) * MAX_VFACE_FRAME_NUM);
            }

            // 现在事件使用回调方式，无法清零越界结果，所以在使用完后清零
            if (g_DrawRectFlag[nSenIndex][i])
            {
                Vosd_Chn_Del_Rect(0, VosdChnType, 0);
                g_DrawRectFlag[nSenIndex][i] = 0;
            }
            s32Ret = Vosd_Chn_Draw_Rect(0, VosdChnType, 0, VFaceFrm);
            if (s32Ret < 0)
            {
                ZW_ERROR("[OSD] Vosd_Chn_Draw_Rect UPDATE Failed, s32Ret = %d\n", s32Ret);
                return -1;
            }

            g_DrawRectFlag[nSenIndex][i] = 1;
        }
    }
    return 0;

onlyClearLine:
    s32Ret = Hi_MPP_ClearDrawLineInfo(0);

    return s32Ret;
}

/*************************************编码相关接口*******************************************/
/**
 * @description: 初始化编码通道句柄
 * @param {void *} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_initHandle(void **hChannelHandle)
{
    CPlatformChannelContext *pPlatCtx = NULL;

    pPlatCtx = (CPlatformChannelContext *)malloc(sizeof(CPlatformChannelContext));
    if (NULL == pPlatCtx)
    {
        ZW_ERROR("pPlatCtx MALLOC failed\n");
        return -1;
    }

    *hChannelHandle = (void*)pPlatCtx;
    memset(*hChannelHandle, 0, sizeof(CPlatformChannelContext));

    XTRACE_MUTEX_INIT(&g_human_result_lock, NULL);
    XTRACE_MUTEX_INIT(&pPlatCtx->m_humanResultsLock, NULL);
    XTRACE_MUTEX_INIT(&pPlatCtx->m_humanResult2Black.m_humanResultsBlackModeLock, NULL);
    XTRACE_MUTEX_INIT(&pPlatCtx->m_humanWorklock, NULL);

    return 0;
}

/**
 * @description: 销毁编码通道句柄
 * @param {void *} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeinitHandle(void **hChannelHandle)
{
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext*)(*hChannelHandle);

    if (NULL == pPlatCtx)
    {
        ZW_ERROR("pPlatCtx CANNOT be NULL\n");
        return -1;
    }

    XTRACE_MUTEX_DESTORY(&g_human_result_lock);
    XTRACE_MUTEX_DESTORY(&pPlatCtx->m_humanResultsLock);
    XTRACE_MUTEX_DESTORY(&pPlatCtx->m_humanResult2Black.m_humanResultsBlackModeLock);
    XTRACE_MUTEX_DESTORY(&pPlatCtx->m_humanWorklock);

    free(*hChannelHandle);

    *hChannelHandle = NULL;

    return 0;
}

/**
 * @description: VI模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVi(void *hChannelHandle)
{
    SINT32 i = 0;
    SINT32 RtVal = 0;
    VI_CONF_S ViConf = {0};
    VI_CHN_CONF_S ViChnConf = {0};
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t    *pSysInfo = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }
    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    ViConf.MaxViChn = pPlatCtx->m_MaxViChn;
    ViConf.MaxSnsNum = pPlatCtx->m_MaxViChn;
    for (i = 0; i < ViConf.MaxViChn; i++)
    {
        if (i >= MAX_VISP_SENSOR_NUM)
        {
            ZW_ERROR("InitVi Failed, MAX SENSOR NUM [%d]\n", i);
            return -1;
        }
        ViConf.SnsConf[i].Mode = VISP_SENSOR_MODE_RGB;
#if defined (__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
        ViConf.SnsConf[i].Type = VISP_SENSOR_JXK06A_MIPI_4M_30FPS_10BIT_2LANE;
#elif defined (__PRODUCT_P3MU__)
        ViConf.SnsConf[i].Type = VISP_SENSOR_GC20C3_MIPI_2M_30FPS_10BIT_2LANE;
#elif defined (__PRODUCT_G43TA__)||defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__)|| defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
        ViConf.SnsConf[i].Type = VISP_SENSOR_GC20C3_MIPI_2M_30FPS_10BIT;
#else
        ViConf.SnsConf[i].Type = VISP_SENSOR_JXF38PA_MIPI_2M_30FPS_10BIT;
#endif
        switch (i)
        {
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
            case FIRST_SENSOR:
#if defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__)|| defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
                ViConf.SnsConf[i].BusId = 0; ViConf.SnsConf[i].SnsAddr = 0x20;
#else
                ViConf.SnsConf[i].BusId = 0; ViConf.SnsConf[i].SnsAddr = ((ViConf.MaxViChn == 3) ? 0x88 : 0x8C);
#endif
                 break;
            case SEC_SENSOR:
#if defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__)
                ViConf.SnsConf[i].BusId = 1; ViConf.SnsConf[i].SnsAddr = 0x20;
#elif defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
				ViConf.SnsConf[i].BusId = 1; ViConf.SnsConf[i].SnsAddr = 0x62;
#else
                ViConf.SnsConf[i].BusId = 2; ViConf.SnsConf[i].SnsAddr = ((ViConf.MaxViChn == 3) ? 0x80 : 0x84);
#endif
                break;
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
            case THIRD_SENSOR:
#if defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
                ViConf.SnsConf[i].BusId = 0; ViConf.SnsConf[i].SnsAddr = 0x62; break;
#else
                ViConf.SnsConf[i].BusId = 0; ViConf.SnsConf[i].SnsAddr = 0x80; break;
#endif
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
            case FOURTH_SENSOR:
#if defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__)
                ViConf.SnsConf[i].BusId = 1; ViConf.SnsConf[i].SnsAddr = 0x62; break;
#else
                ViConf.SnsConf[i].BusId = 2; ViConf.SnsConf[i].SnsAddr = 0x80; break;
#endif
#endif
            default: break;
        }
    }

    if ((RtVal = Vi_Open(ViConf.MaxViChn, pPlatCtx->m_ChipConf, ViConf)) < 0)
    {
        ZW_ERROR("Vi_Open Failed(%#x)\n", RtVal);
        return -1;
    }

    if ((RtVal = Vi_Start(0, 0)) < 0)
    {
        ZW_ERROR("Vi_Start Failed(%#x)\n", RtVal);
        Vi_Stop(0);
        return -1;
    }

    for (i = 0; i < ViConf.MaxViChn; i++)
    {
        ViChnConf.width = pSysInfo->m_VIChnWidth[i];
        ViChnConf.height = pSysInfo->m_VIChnHeight[i];
        ViChnConf.MainFrame = pSysInfo->m_VIChnFrmRate[i];
        ZW_DEBUG("Vi_Chn_Open  nChn(%d) MainFrame(%d)\n",i,pSysInfo->m_VIChnFrmRate[i]);

        if ((RtVal = Vi_Chn_Open(0, i, ViChnConf)) != 0)
        {
            ZW_ERROR("Vi_Chn_Open Failed(%#x)\n", RtVal);
            Vi_Stop(0);
            Vi_Close(0);
            return -1;
        }
    }

    ZW_DEBUG("Hi_MPP_InitVi Success\n");
    return RtVal;
}

/**
 * @description: VI模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVi(void)
{
    int i = 0;
    SINT32 RtVal = 0;
    int MaxViChn = HY_Res_GetMaxSensorCnt();

    for (i = 0; i < MaxViChn; i++)
    {
        if ((RtVal = Vi_Chn_Close(0, i)) < 0)
        {
            ZW_ERROR("Vi_Chn_Close failed [%d][%d]\n", i, RtVal);
            return -1;
        }
    }

    if ((RtVal = Vi_Stop(0)) < 0)
    {
        ZW_ERROR("Vi_Stop failed [%d]\n", RtVal);
        return -1;
    }

    if ((RtVal = Vi_Close(0)) < 0)
    {
        ZW_ERROR("Vi_Close failed [%d]\n", RtVal);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_DeInitVi Success\n");

    return RtVal;
}

/**
 * @brief  VPSS通道初始化
 * @param  hChannelHandle 编码通道句柄
 * @return SINT32 正常返回0
 */
SINT32 Hi_MPP_InitVpssChn(void *hChannelHandle)
{
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    //主码流
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam.SrcChn  = 0;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam.width   = pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].u32Width;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam.height  = pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].u32Height;
    //pPlatCtx->m_stVpssChnAttr[FIRST_STREAM].VpssParam.OutFrmRate	= (pSysInfo->m_Frequ_HZ == CMOS_FREQUENCY_60HZ) ? 30 : 25;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam.OutFrmRate	= 15;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam.Depth 		= 0;

    //子码流
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.SrcChn 		= 1;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.width       	= pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].u32Width;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.height      	= pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].u32Height;
    //pPlatCtx->m_stVpssChnAttr[SECOND_STREAM].VpssParam.OutFrmRate	= (pSysInfo->m_Frequ_HZ == CMOS_FREQUENCY_60HZ) ? 30 : 25;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.OutFrmRate	= 15;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam.Depth 		= 1;

    //算法通道
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.SrcChn 		= 2;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.width       	= ALGO_STREAM_WIDTH;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.height      	= ALGO_STREAM_HEIGHT;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.OutFrmRate	= ALGO_STREAM_FPS;
    pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.Depth 		= 1;
    /* 次Sensor默认使用主sensor参数，直接拷贝 */
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
    memcpy(&(pPlatCtx->m_stVpssChnAttr[SEC_SENSOR][FIRST_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
    memcpy(&(pPlatCtx->m_stVpssChnAttr[SEC_SENSOR][SECOND_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
    memcpy(&(pPlatCtx->m_stVpssChnAttr[SEC_SENSOR][AI_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    /* 次Sensor默认使用主sensor参数，直接拷贝 */
    memcpy(&(pPlatCtx->m_stVpssChnAttr[THIRD_SENSOR][FIRST_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
    memcpy(&(pPlatCtx->m_stVpssChnAttr[THIRD_SENSOR][SECOND_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
    memcpy(&(pPlatCtx->m_stVpssChnAttr[THIRD_SENSOR][AI_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
    /* 次Sensor默认使用主sensor参数，直接拷贝 */
    memcpy(&(pPlatCtx->m_stVpssChnAttr[FOURTH_SENSOR][FIRST_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][FIRST_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
    memcpy(&(pPlatCtx->m_stVpssChnAttr[FOURTH_SENSOR][SECOND_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][SECOND_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
    memcpy(&(pPlatCtx->m_stVpssChnAttr[FOURTH_SENSOR][AI_STREAM].VpssParam), &(pPlatCtx->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam), sizeof(VPSS_CHN_ATTR_S));
#endif

    return 0;
}

/**
 * @description: VPSS模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVpss(void *hChannelHandle)
{
    SINT32 RtVal = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    VPSS_GRP_ATTR_S VpssGrpAttr = {0};
    VPSS_CHN_ATTR_S VpssChnAttr = {0};
    SystemInfo_t    *pSysInfo = NULL;

    unsigned int i = 0, j = 0;
    int GrpNum = 0, ChnNum = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    memset(&VpssGrpAttr, 0, sizeof(VpssGrpAttr));
    memset(&VpssChnAttr, 0, sizeof(VpssChnAttr));

    if ((RtVal = Vpss_Open(3, pPlatCtx->m_ChipConf)) < 0)
    {
        ZW_ERROR("Vpss_Open failed(%#x)\n", RtVal);
        return -1;
    }
    for (i = 0; i < MaxSns; i++)
    {
        GrpNum = i;
        VpssGrpAttr.Width = pSysInfo->m_VIChnWidth[i];
        VpssGrpAttr.Height = pSysInfo->m_VIChnHeight[i];
        VpssGrpAttr.EnMcf = 0;
        VpssGrpAttr.EnScale = 0;

        if ((RtVal = Vpss_Grp_Open(0, GrpNum, &VpssGrpAttr)) < 0)
        {
            ZW_ERROR("Vpss_Grp_Open failed(%#x)\n", RtVal);
            Vpss_Close(0);
            return -1;
        }
        pPlatCtx->m_VPSSGroupNum[i] = GrpNum;

        for (j = 0; j < MAX_STREAM_COUNT; j++ )
        {
#if defined __ISP_IMAGE_DEBUG__
            if (j != FIRST_STREAM)
            {
                continue;
            }
#endif
            if ((j != FIRST_STREAM) && (j != SECOND_STREAM) && (j != AI_STREAM))
            {
                continue;
            }
            ChnNum = pPlatCtx->m_stVpssChnAttr[i][j].VpssParam.SrcChn;
            VpssChnAttr.width = pPlatCtx->m_stVpssChnAttr[i][j].VpssParam.width;
            VpssChnAttr.height = pPlatCtx->m_stVpssChnAttr[i][j].VpssParam.height;
            VpssChnAttr.Depth = pPlatCtx->m_stVpssChnAttr[i][j].VpssParam.Depth;

            if ((RtVal = Vpss_Chn_Open(0, GrpNum, ChnNum, VpssChnAttr)) < 0)
            {
                ZW_ERROR("Vpss_Chn_Open failed(%#x) i=%d\n", RtVal, i);
                goto close_vpss_chn;
            }
            pPlatCtx->m_VPSSChnNum[i][j] = ChnNum;
        }
    }

    ZW_DEBUG("Hi_MPP_InitVpss Success\n");
    return 0;

close_vpss_chn:
    for (; i >= 0; i--)
    {
        for (--j; j >= 0; j--)
        {
#if defined __ISP_IMAGE_DEBUG__
            if (j != FIRST_STREAM)
            {
                continue;
            }
#endif
            if ((j != FIRST_STREAM) && (j != SECOND_STREAM) && (j != AI_STREAM))
            {
                continue;
            }
            Vpss_Chn_Close(0, pPlatCtx->m_VPSSGroupNum[i], pPlatCtx->m_VPSSChnNum[i][j]);
            pPlatCtx->m_VPSSChnNum[i][j] = -1;
        }
        Vpss_Grp_Close(0, pPlatCtx->m_VPSSGroupNum[i]);
        pPlatCtx->m_VPSSGroupNum[i] = -1;
    }
    return -1;
}

/**
 * @description: VPSS模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVpss(void)
{
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    SINT32	RtVal = 0;
    int i = 0, j = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
        for (i = 0; i < MaxSns; i++)
        {
            int j_start, j_end, j_step;

            if (i == 0)
            {
                j_start = MAX_STREAM_COUNT - 1;
                j_end = -1;
                j_step = -1;
            }
            else
            {
                j_start = 0;
                j_end = MAX_STREAM_COUNT;
                j_step = 1;
            }

            for (j = j_start; j != j_end; j += j_step)
            {
    #if defined __ISP_IMAGE_DEBUG__
                if (j != FIRST_STREAM)
                {
                    continue;
                }
    #endif
                if ((j != FIRST_STREAM) && (j != SECOND_STREAM) && (j != AI_STREAM))
                {
                    continue;
                }

                if ((RtVal = Vpss_Chn_Close(0, pPlatCtx->m_VPSSGroupNum[i], pPlatCtx->m_VPSSChnNum[i][j])) < 0)
                {
                    ZW_ERROR("Vpss_Chn_Close failed(%#x) i=%d j=%d\n", RtVal, i, j);
                    //return -1;
                }
                pPlatCtx->m_VPSSChnNum[i][j] = -1;
            }

            if ((RtVal = Vpss_Grp_Close(0, pPlatCtx->m_VPSSGroupNum[i])) < 0)
            {
                ZW_ERROR("Vpss_Grp_Close failed(%#x) i=%d j=%d\n", RtVal, i, j);
                //return -1;
            }
            pPlatCtx->m_VPSSGroupNum[i] = -1;
        }


    if ((RtVal = Vpss_Close(0)) < 0)
    {
        ZW_ERROR("Vpss_Close failed\n");
        return -1;
    }

    ZW_DEBUG("Hi_MPP_DeInitVpss Success\n");
    return RtVal;
}

int Hi_MPP_StreamTypeSwap(VENC_CHNTYPE_E eChnType, DEV_SUPPORT_SENSOR *Sns, ENCODE_STREAM_E *eStream, SystemInfo_t *pSysInfo)
{
       SINT32 RtVal = 0;
        int sns_idx = -1;  /* 0=FIRST, 1=SEC, 2=THIRD, 3=FOURTH */

        switch (eChnType)
        {
        case VENC_CHNTYPE_MAIN:
            sns_idx = 0;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR:
            sns_idx = 0;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP:
            sns_idx = 0;   *eStream = FOURTH_STREAM;  break;

#if (SYSTEM_SUPPORT_CHANNEL == 2)
        case VENC_CHNTYPE_MAIN_SEC:
            sns_idx = 1;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR_SEC:
            sns_idx = 1;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP_SEC:
            sns_idx = 1;   *eStream = FOURTH_STREAM;  break;

#elif (SYSTEM_SUPPORT_CHANNEL == 3)
        /* 三目整机为了适配四目整机，需要将SEC和THIRD通道做交换 */
        case VENC_CHNTYPE_MAIN_SEC:
            sns_idx = 2;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR_SEC:
            sns_idx = 2;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP_SEC:
            sns_idx = 2;   *eStream = FOURTH_STREAM;  break;

        case VENC_CHNTYPE_MAIN_THIRD:
            sns_idx = 1;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR_THIRD:
            sns_idx = 1;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP_THIRD:
            sns_idx = 1;   *eStream = FOURTH_STREAM;  break;

#elif (SYSTEM_SUPPORT_CHANNEL == 4)
        case VENC_CHNTYPE_MAIN_SEC:
            sns_idx = 1;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR_SEC:
            sns_idx = 1;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP_SEC:
            sns_idx = 1;   *eStream = FOURTH_STREAM;  break;

        case VENC_CHNTYPE_MAIN_THIRD:
            sns_idx = 2;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR_THIRD:
            sns_idx = 2;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP_THIRD:
            sns_idx = 2;   *eStream = FOURTH_STREAM;  break;

        case VENC_CHNTYPE_MAIN_FOURTH:
            sns_idx = 3;   *eStream = FIRST_STREAM;   break;
        case VENC_CHNTYPE_MINOR_FOURTH:
            sns_idx = 3;   *eStream = SECOND_STREAM;  break;
        case VENC_CHNTYPE_SNAP_FOURTH:
            sns_idx = 3;   *eStream = FOURTH_STREAM;  break;
#endif

        default:
            ZW_ERROR("eChnType Not Support [%d]\n", eChnType);
            RtVal = -1;
            break;
        }

        if (1 == pSysInfo->m_bSptSwapLRViewChannel || access("/mnt/mtd/sensor_swap", F_OK) == 0)
        {
#if (SYSTEM_SUPPORT_CHANNEL == 3)
            if (sns_idx == 1) sns_idx = 2;
            else if (sns_idx == 2) sns_idx = 1;

#elif (SYSTEM_SUPPORT_CHANNEL == 4)
            if (sns_idx == 2) sns_idx = 3;
            else if (sns_idx == 3) sns_idx = 2;
#endif
        }

        switch (sns_idx)
        {
        case 0: *Sns = FIRST_SENSOR;  break;
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
        case 1: *Sns = SEC_SENSOR;	  break;
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
        case 2: *Sns = THIRD_SENSOR;  break;
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
        case 3: *Sns = FOURTH_SENSOR; break;
#endif
        default:
            return -1;
        }

        if (*eStream == FOURTH_STREAM && *Sns >= ALGO_CHANNEL)
            RtVal = -1;

        return RtVal;

}

/**
 * @description: 取流回调
 * @param {SINT32} stype 视频流类型(STREAM_TYPE_E)
 * @param {SINT32} idx 通道号,对应前端的sensor号
 * @param {SINT8} *pHead 码流头缓冲区(MSSTD_S)
 * @param {SINT32} Headsize 码流头长度(MSSTD_S_LEN)
 * @param {char} *data 数据接收缓冲区
 * @param {SINT32} size 数据接收缓冲区大小
 * @return {SINT32} 正常返回0
 */
static SINT32 _Video_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, char *data, SINT32 size)
{
    SINT32 RtVal = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t			*pSysInfo = NULL;
    CChannelContext 		*pChnCtx  = NULL;
    DEV_SUPPORT_SENSOR SensorIndex;
    ENCODE_STREAM_E eStream;
    HY_Enc_GetEncChInfo(&pChnCtx);
    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext*)pChnCtx->pPlatformChannelhandle;

    if (Hi_MPP_ReadVideoStream(pPlatCtx, (VENC_CHNTYPE_E) stype, (MSSTD_S *)pHead, data, size))
    {
        return 0;
    }
    if (!pPlatCtx->m_stStreamData.bFinish)
    {
        return 0;
    }
    Hi_MPP_StreamTypeSwap((VENC_CHNTYPE_E)stype, &SensorIndex, &eStream, pSysInfo);

    if (pSysInfo->m_pStreamReadCallback != NULL && pChnCtx->m_bRecord)
    {
        if (FIRST_STREAM == eStream)
        {
            XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
            pChnCtx->bMainStreamOK[SensorIndex] = TRUE;
            pSysInfo->m_pStreamReadCallback(0);
            pChnCtx->bMainStreamOK[SensorIndex] = FALSE;
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);
        }
        else if (SECOND_STREAM == eStream)
        {
            if ((pChnCtx->m_bRecordAux) && (LtMjpegVideo != pChnCtx->m_AuxEncType))
            {
                XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
                pChnCtx->bAuxStreamOK[SensorIndex] = TRUE;
                pSysInfo->m_pStreamReadCallback(0);
                pChnCtx->bAuxStreamOK[SensorIndex] = FALSE;
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);
            }
        }
        else if (FOURTH_STREAM == eStream)
        {
            if (pChnCtx->m_bSupportFourthStream && pChnCtx->m_bRecordFourth)
            {
                XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
                pChnCtx->bFourthStreamOK[SensorIndex] = TRUE;
                pSysInfo->m_pStreamReadCallback(0);
                pChnCtx->bFourthStreamOK[SensorIndex] = FALSE;
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);
            }
        }
#ifdef __USE_CIRCLEQUEUE__
        else if (pSysInfo->pVideoBuffer)
        {
            free(pSysInfo->pVideoBuffer);
            pSysInfo->pVideoBuffer = NULL;
        }
#endif

    }
#ifdef __USE_CIRCLEQUEUE__
    else if (pSysInfo->pVideoBuffer)
    {
        free(pSysInfo->pVideoBuffer);
        pSysInfo->pVideoBuffer = NULL;
    }
#endif
    return RtVal;
}

/**
 * @description: Venc模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVenc(void *hChannelHandle)
{
    SINT32 RtVal = 0;
    VENC_CONF_S VencConf = {0};
    CPlatformChannelContext *pPlatCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    if ((RtVal = Venc_Open(MaxSns*3, pPlatCtx->m_ChipConf, VencConf, NULL, NULL)) < 0)
    {
        ZW_ERROR("Venc_Open failed(%#x)\n", RtVal);
        return -1;
    }

    /* 注册取流回调 */
    RtVal =  Venc_SetChnCallback(VENC_CHNTYPE_MAIN,      (VData_Callback)_Video_Get_DataFunc,NULL);
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MINOR,     (VData_Callback)_Video_Get_DataFunc,NULL);
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_SNAP,      (VData_Callback)_Video_Get_DataFunc,NULL);
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MAIN_SEC,  (VData_Callback)_Video_Get_DataFunc,NULL);
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MINOR_SEC, (VData_Callback)_Video_Get_DataFunc,NULL);
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_SNAP_SEC,  (VData_Callback)_Video_Get_DataFunc,NULL);
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MAIN_THIRD,  (VData_Callback)_Video_Get_DataFunc,NULL);
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MINOR_THIRD, (VData_Callback)_Video_Get_DataFunc,NULL);
    RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_SNAP_THIRD,  (VData_Callback)_Video_Get_DataFunc,NULL);
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
    if (MaxSns == SUPPORT_MAX_SENSOR_COUNT)
    {
        RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MAIN_FOURTH,  (VData_Callback)_Video_Get_DataFunc,NULL);
        RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_MINOR_FOURTH, (VData_Callback)_Video_Get_DataFunc,NULL);
        RtVal |= Venc_SetChnCallback(VENC_CHNTYPE_SNAP_FOURTH,  (VData_Callback)_Video_Get_DataFunc,NULL);
    }
#endif
    if ((RtVal = Venc_Start(0, 0)) < 0)
    {
        ZW_ERROR("Venc_Start failed(%#x)\n", RtVal);

        Venc_Close(0);

        return -1;
    }

    ZW_DEBUG("Hi_MPP_InitVenc Success\n");

    return RtVal;
}

/**
 * @description: Venc模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVenc(void)
{
    SINT32	RtVal = 0;

    ZW_DEBUG("Venc_Stop \n");
    if ((RtVal = Venc_Stop(0)) < 0)
    {
        ZW_ERROR("Venc_Stop failed [%d]\n", RtVal);
        return -1;
    }

    ZW_DEBUG("Venc_Close \n");
    if ((RtVal = Venc_Close(0)) < 0)
    {
        ZW_ERROR("Venc_Close failed [%d]\n", RtVal);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_DeInitVenc Success\n");
    return RtVal;
}

/**
 * @description: ISP模块初始化并加载模型,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitIsp(void *hChannelHandle)
{
    SINT32 RtVal = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    int MaxChn = HY_Res_GetMaxSensorCnt();

    VISP_CONF_S Conf = {
        .SnsNum = MaxChn,
    };

    VISP_CONTROL_CALLBACK_S ControlCb = {0};

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    if ((RtVal = Visp_Open(MaxChn, pPlatCtx->m_ChipConf, ControlCb)) < 0)
    {
        ZW_ERROR("Visp_Open failed [%d]\n", RtVal);
        return -1;
    }

    if ((RtVal = Visp_Start(0, Conf)) < 0)
    {
        ZW_ERROR("Visp_Start failed [%d]\n", RtVal);
        Visp_Close(0);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_InitIsp Success\n");
    return RtVal;
}

/**
 * @description: ISP模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitIsp(void)
{
    SINT32 RtVal = 0;

    if ((RtVal = Visp_Stop(0)) < 0)
    {
        ZW_ERROR("Visp_Stop failed\n");
        return -1;
    }

    if ((RtVal = Visp_Close(0)) < 0)
    {
        ZW_ERROR("Visp_Close failed\n");
        return -1;
    }

    ZW_DEBUG("Hi_MPP_DeInitIsp Success\n");
    return RtVal;
}

/**
 * @description: 创建视频输入通道
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitViChn(void *hChannelHandle)
{
    SINT32 RtVal = 0;
    VI_CHN_CONF_S m_ViChnConf = {0};
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    m_ViChnConf.width = pPlatCtx->m_VIOutMaxWidth;
    m_ViChnConf.height = pPlatCtx->m_VIOutMaxHeight;
    m_ViChnConf.MainFrame = 30;
    m_ViChnConf.ViSrcType = 0;
    m_ViChnConf.RealTimeMode = 0;

    ZW_DEBUG("VI Out W=%d, H=%d\n", m_ViChnConf.width, m_ViChnConf.height);

    if ((RtVal = Vi_Chn_Open(0, 0, m_ViChnConf)) < 0)
    {
        ZW_ERROR("Vi_Chn_Open failed\n");
        return -1;
    }

    ZW_DEBUG("Vi_Chn_Open success!!!\n");

    return RtVal;
}

/**
 * @description: 销毁视频输入通道
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitViChn(void)
{
    SINT32 RtVal = 0;

    if ((RtVal = Vi_Chn_Close(0, 0)) < 0)
    {
        ZW_ERROR("Vi_Chn_Close failed\n");
        return -1;
    }
    ZW_DEBUG("Vi_Chn_Close success!!!\n");

    return RtVal;
}

/**
 * @description: 创建编码通道，并进行Venc通道绑定
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitEncChn(void *hChannelHandle)
{
    int RtRal = 0;
    VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
    VENC_BIND_CONF_S BindConf = {0};
    int i = 0, j = 0, ChnNum = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t    *pSysInfo = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    for (i = 0; i < MaxSns; i++)
    {
        for (j = 0; j < MAX_STREAM_COUNT; j++)
        {
            if ((j != FIRST_STREAM) && (j != SECOND_STREAM) && (j != FOURTH_STREAM))
            {
                continue;
            }
            switch (ChnNum) {
                case  0: ChnType = VENC_CHNTYPE_MAIN;           break;
                case  1: ChnType = VENC_CHNTYPE_MINOR;          break;
                case  2: ChnType = VENC_CHNTYPE_SNAP;           break;
                case  3: ChnType = VENC_CHNTYPE_MAIN_SEC;       break;
                case  4: ChnType = VENC_CHNTYPE_MINOR_SEC;      break;
                case  5: ChnType = VENC_CHNTYPE_SNAP_SEC;       break;
#if SYSTEM_SUPPORT_CHANNEL >= 3
                case  6: ChnType = VENC_CHNTYPE_MAIN_THIRD;     break;
                case  7: ChnType = VENC_CHNTYPE_MINOR_THIRD;    break;
                case  8: ChnType = VENC_CHNTYPE_SNAP_THIRD;     break;
                case  9: ChnType = VENC_CHNTYPE_MAIN_FOURTH;    break;
                case  10: ChnType = VENC_CHNTYPE_MINOR_FOURTH;  break;
                case  11: ChnType = VENC_CHNTYPE_SNAP_FOURTH;   break;
#endif
                default: break;
            }

#if defined __ISP_IMAGE_DEBUG__
            if (j != FIRST_STREAM)
            {
                continue;
            }

            switch (i) {
                case  FIRST_SENSOR:     ChnType = VENC_CHNTYPE_MAIN;           break;
                case  SEC_SENSOR:       ChnType = VENC_CHNTYPE_MAIN_SEC;       break;
                case  THIRD_SENSOR:     ChnType = VENC_CHNTYPE_MAIN_THIRD;     break;
                case  FOURTH_SENSOR:    ChnType = VENC_CHNTYPE_MAIN_FOURTH;    break;
                default: break;
            }
#endif

            ZW_DEBUG("Venc_Chn_Open(%d) SrcFrmRate(%d) FrameRate(%d) \n",ChnType,pSysInfo->m_VIChnFrmRate[i],pPlatCtx->m_stVencChnAttr[i][j].EncParam.FrameRate);

            /* Res: 使用mipi swicth的这组pipeline，平台默认是sensor帧率的2倍，因此需要做以下调整 */
#if SYSTEM_SUPPORT_CHANNEL <=2
            RtRal = Venc_Chn_Open(0, ChnType, ChnNum, pPlatCtx->m_stVencChnAttr[i][j].EncParam, pSysInfo->m_VIChnFrmRate[i]);
#else
            if ((MaxSns == 3) && (i == SEC_SENSOR))
            {
                RtRal = Venc_Chn_Open(0, ChnType, ChnNum, pPlatCtx->m_stVencChnAttr[i][j].EncParam, pSysInfo->m_VIChnFrmRate[i]);
            }
            else
            {
                RtRal = Venc_Chn_Open(0, ChnType, ChnNum, pPlatCtx->m_stVencChnAttr[i][j].EncParam, pSysInfo->m_VIChnFrmRate[i] * 2);
            }
#endif

            if (RtRal)
            {
                ZW_ERROR("Venc_Chn_Open chn(%d) FAIL(%#x)\n", ChnType, RtRal);
                return -1;
            }
            pPlatCtx->m_MppEncChNum[i][j] = ChnNum;

            RtRal = Venc_Chn_Start(0, ChnType, ChnNum, 0);
            if (RtRal)
            {
                Venc_Chn_Close(0, ChnType, ChnNum);
                VENC_TRACE_ERR("Venc_Chn_Start chn(%d) FAIL(%d)\n", ChnType, RtRal);
                return -1;
            }

            BindConf.ChnType = ChnType;
            BindConf.VencSrcGrp = pPlatCtx->m_VPSSGroupNum[i];

#if SYSTEM_SUPPORT_CHANNEL <= 2
            if (ChnType == VENC_CHNTYPE_SNAP ||
                ChnType == VENC_CHNTYPE_SNAP_SEC)
#else
            if (ChnType == VENC_CHNTYPE_SNAP ||
                ChnType == VENC_CHNTYPE_SNAP_SEC ||
                ChnType == VENC_CHNTYPE_SNAP_THIRD ||
                ChnType == VENC_CHNTYPE_SNAP_FOURTH)
#endif
            {
                BindConf.VencSrcChn = pPlatCtx->m_VPSSChnNum[i][SECOND_STREAM];
            }
            else
            {
                BindConf.VencSrcChn = pPlatCtx->m_VPSSChnNum[i][j];
            }

            RtRal = Venc_Chn_Bind(0, ChnType, BindConf);
            if (RtRal)
            {
                ZW_ERROR("Venc_Chn_Bind chn(%d) (%d) (%d) FAIL(%#x)\n", BindConf.ChnType, BindConf.VencSrcGrp, BindConf.VencSrcChn, RtRal);
                Venc_Chn_Stop(0, ChnType, ChnNum);
                Venc_Chn_Close(0, ChnType, ChnNum);
                return -1;
            }

            if (j == FIRST_STREAM)
            {
                Hi_MPP_SetQPAttr(ChnType, ChnNum);
            }

            if (j == FIRST_STREAM || j == SECOND_STREAM)
            {
                Hi_MPP_Set_SuperFrameStrategy(ChnType, ChnNum);
            }

            ChnNum++;
        }
    }

    ZW_DEBUG("Hi_MPP_InitEncChn success\n");

    return 0;
}

/**
 * @description: 销毁编码通道
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitEncChn()
{
    SINT32 RtVal = 0;
    VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
    VENC_BIND_CONF_S BindConf = {0};
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    int i = 0, j = 0, ChnNum = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    for (i = 0; i < MaxSns; i++)
    {
        for (j = 0; j < MAX_STREAM_COUNT; j++)
        {
            if ((j != FIRST_STREAM) && (j != SECOND_STREAM) && (j != FOURTH_STREAM))
            {
                continue;
            }

            /* 只使能主sensor的MJPEG VENC流         */
            if ((i != FIRST_SENSOR) && (j == FOURTH_STREAM))
            {
                //continue;
            }

            ChnNum = pPlatCtx->m_MppEncChNum[i][j];

            switch (ChnNum) {
                case  0: ChnType = VENC_CHNTYPE_MAIN;           break;
                case  1: ChnType = VENC_CHNTYPE_MINOR;          break;
                case  2: ChnType = VENC_CHNTYPE_SNAP;           break;
                case  3: ChnType = VENC_CHNTYPE_MAIN_SEC;       break;
                case  4: ChnType = VENC_CHNTYPE_MINOR_SEC;      break;
                case  5: ChnType = VENC_CHNTYPE_SNAP_SEC;       break;
#if SYSTEM_SUPPORT_CHANNEL >= 3
                case  6: ChnType = VENC_CHNTYPE_MAIN_THIRD;     break;
                case  7: ChnType = VENC_CHNTYPE_MINOR_THIRD;    break;
                case  8: ChnType = VENC_CHNTYPE_SNAP_THIRD;     break;
                case  9: ChnType = VENC_CHNTYPE_MAIN_FOURTH;    break;
                case  10: ChnType = VENC_CHNTYPE_MINOR_FOURTH;  break;
                case  11: ChnType = VENC_CHNTYPE_SNAP_FOURTH;   break;
#endif
                default: break;
            }


#if defined __ISP_IMAGE_DEBUG__
            if (j != FIRST_STREAM)
            {
                continue;
            }

            switch (i) {
                case  FIRST_SENSOR:     ChnType = VENC_CHNTYPE_MAIN;           break;
                case  SEC_SENSOR:       ChnType = VENC_CHNTYPE_MAIN_SEC;       break;
                case  THIRD_SENSOR:     ChnType = VENC_CHNTYPE_MAIN_THIRD;     break;
                case  FOURTH_SENSOR:    ChnType = VENC_CHNTYPE_MAIN_FOURTH;    break;
                default: break;
            }
#endif

            BindConf.ChnType = ChnType;
            BindConf.VencSrcGrp = pPlatCtx->m_VPSSGroupNum[i];


#if SYSTEM_SUPPORT_CHANNEL <=2
            if (ChnType == VENC_CHNTYPE_SNAP ||
            ChnType == VENC_CHNTYPE_SNAP_SEC )
#else
            if (ChnType == VENC_CHNTYPE_SNAP ||
            ChnType == VENC_CHNTYPE_SNAP_SEC ||
            ChnType == VENC_CHNTYPE_SNAP_THIRD ||
            ChnType == VENC_CHNTYPE_SNAP_FOURTH)
#endif

            {
                BindConf.VencSrcChn = pPlatCtx->m_VPSSChnNum[i][SECOND_STREAM];
            }
            else
            {
                BindConf.VencSrcChn = pPlatCtx->m_VPSSChnNum[i][j];
            }

            ZW_DEBUG("Venc_Chn_UnBind ChnType(%d) VencSrcChn(%d)\n", ChnType, BindConf.VencSrcChn);
            RtVal = Venc_Chn_UnBind(0, ChnType, BindConf);
            if (RtVal)
            {
                ZW_ERROR("Venc_Chn_UnBind chn(%d) FAIL(%#x)\n", ChnType, RtVal);
                return -1;
            }

            ZW_DEBUG("Venc_Chn_Stop ChnType(%d) VencChn(%d)\n", ChnType, ChnNum);
            RtVal = Venc_Chn_Stop(0, ChnType, ChnNum);
            if (RtVal)
            {
                VENC_TRACE_ERR("Venc_Chn_Stop chn(%d) FAIL(%#x)\n", ChnType, RtVal);
            }

            ZW_DEBUG("Venc_Chn_Stop ChnType(%d) VencChn(%d)\n", ChnType, ChnNum);
            RtVal =  Venc_Chn_Close(0, ChnType, ChnNum);
            if (RtVal)
            {
                ZW_ERROR("Venc_Chn_Close chn(%d) FAIL(%#x)\n", ChnType, RtVal);
                return -1;
            }
        }
    }

    ZW_DEBUG("Hi_MPP_DeInitEncChn success\n");
    return RtVal;
}

/**
 * @description: 强制I帧
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ForceIFrame(void *hChannelHandle, int nSensorIndex)
{
    SINT32   s32Ret = 0;
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    if (nSensorIndex == FIRST_SENSOR)
    {
        /* 主码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret = Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN, pPlatCtx->m_MppEncChNum[FIRST_SENSOR][FIRST_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNum = %d", pPlatCtx->m_MppEncChNum[FIRST_SENSOR][FIRST_STREAM]);
            ZW_DEBUG("[FIRST_SENSOR] MAIN: Force I frame get result = %d\n", s32Ret);
        }

        /* 辅码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR, pPlatCtx->m_MppEncChNum[FIRST_SENSOR][SECOND_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNumAux = %d", pPlatCtx->m_MppEncChNum[FIRST_SENSOR][SECOND_STREAM]);
            ZW_DEBUG("[FIRST_SENSOR] SUB: Force I frame get result = %d\n", s32Ret);
        }
    }
#if SYSTEM_SUPPORT_CHANNEL >=2
    else if (nSensorIndex == SEC_SENSOR)
    {
        /* 主码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[SEC_SENSOR][FIRST_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret = Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN_SEC, pPlatCtx->m_MppEncChNum[SEC_SENSOR][FIRST_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNum = %d", pPlatCtx->m_MppEncChNum[SEC_SENSOR][FIRST_STREAM]);
            ZW_DEBUG("[SEC_SENSOR] MAIN: Force I frame get result = %d\n", s32Ret);
        }

        /* 辅码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[SEC_SENSOR][SECOND_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR_SEC, pPlatCtx->m_MppEncChNum[SEC_SENSOR][SECOND_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNumAux = %d", pPlatCtx->m_MppEncChNum[SEC_SENSOR][SECOND_STREAM]);
            ZW_DEBUG("[SEC_SENSOR] SUB: Force I frame get result = %d\n", s32Ret);
        }
    }
#endif
#if SYSTEM_SUPPORT_CHANNEL >=3
    else if (nSensorIndex == THIRD_SENSOR)
    {
        /* 主码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[THIRD_SENSOR][FIRST_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret = Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN_THIRD, pPlatCtx->m_MppEncChNum[THIRD_SENSOR][FIRST_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNum = %d", pPlatCtx->m_MppEncChNum[THIRD_SENSOR][FIRST_STREAM]);
            ZW_DEBUG("[THIRD_SENSOR] MAIN: Force I frame get result = %d\n", s32Ret);
        }

        /* 辅码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[THIRD_SENSOR][SECOND_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR_THIRD, pPlatCtx->m_MppEncChNum[THIRD_SENSOR][SECOND_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNumAux = %d", pPlatCtx->m_MppEncChNum[THIRD_SENSOR][SECOND_STREAM]);
            ZW_DEBUG("[THIRD_SENSOR] SUB: Force I frame get result = %d\n", s32Ret);
        }
    }
#endif
#if SYSTEM_SUPPORT_CHANNEL >=4
    else if (nSensorIndex == FOURTH_SENSOR)
    {
        /* 主码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[FOURTH_SENSOR][FIRST_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret = Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN_FOURTH, pPlatCtx->m_MppEncChNum[FOURTH_SENSOR][FIRST_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNum = %d", pPlatCtx->m_MppEncChNum[FOURTH_SENSOR][FIRST_STREAM]);
            ZW_DEBUG("[FOURTH_SENSOR] MAIN: Force I frame get result = %d\n", s32Ret);
        }

        /* 辅码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[FOURTH_SENSOR][SECOND_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR_FOURTH, pPlatCtx->m_MppEncChNum[FOURTH_SENSOR][SECOND_STREAM]);
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNumAux = %d", pPlatCtx->m_MppEncChNum[FOURTH_SENSOR][SECOND_STREAM]);
            ZW_DEBUG("[FOURTH_SENSOR] SUB: Force I frame get result = %d\n", s32Ret);
        }
    }
#endif
    else
    {
        /* 主码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret =  Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN, pPlatCtx->m_MppEncChNum[FIRST_SENSOR][FIRST_STREAM]);
#if SYSTEM_SUPPORT_CHANNEL >=2
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN_SEC, pPlatCtx->m_MppEncChNum[SEC_SENSOR][FIRST_STREAM]);
#endif
#if SYSTEM_SUPPORT_CHANNEL >=3
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN_THIRD, pPlatCtx->m_MppEncChNum[THIRD_SENSOR][FIRST_STREAM]);
#endif
#if SYSTEM_SUPPORT_CHANNEL >=4
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MAIN_FOURTH, pPlatCtx->m_MppEncChNum[FOURTH_SENSOR][FIRST_STREAM]);
#endif
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNum = %d", 0);
            ZW_INFO("MAIN: Force I frame get result = %d\n", s32Ret);
        }

        /* 辅码流强制I帧*/
        if (pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.VPayloadType != VPAYLOAD_TYPE_MJPEG)
        {
            s32Ret =  Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR, pPlatCtx->m_MppEncChNum[FIRST_SENSOR][SECOND_STREAM]);
#if SYSTEM_SUPPORT_CHANNEL >=2
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR_SEC, pPlatCtx->m_MppEncChNum[SEC_SENSOR][SECOND_STREAM]);
#endif
#if SYSTEM_SUPPORT_CHANNEL >=3
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR_THIRD, pPlatCtx->m_MppEncChNum[THIRD_SENSOR][SECOND_STREAM]);
#endif
#if SYSTEM_SUPPORT_CHANNEL >=4
            s32Ret |= Venc_Chn_Request_IFrm(0, VENC_CHNTYPE_MINOR_FOURTH, pPlatCtx->m_MppEncChNum[FOURTH_SENSOR][SECOND_STREAM]);
#endif
            DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Venc_Chn_Request_IFrm, m_ENCChNumAux = %d", 1);
            ZW_INFO("SUB: Force I frame get result = %d\n", s32Ret);
        }
    }

    ZW_PRINTF("ForceIFrame nSensorIndex:[%d] result:[%d]\n", nSensorIndex, s32Ret);
    return s32Ret;
}

/**
 * @description: 改变编码分辨率
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {ENCODE_STREAM_E} eStream编码通道号
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SwitchPicFormat(void *hChannelHandle, ENCODE_STREAM_E eStream)
{
    SINT32 Ret = 0;
    SINT32 i = 0, ChnNum = 0, GrpNum = 0;
    VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
    VENC_BIND_CONF_S BindConf;
    VPSS_CHN_ATTR_S VpssChnAttr;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    memset(&BindConf, 0, sizeof(BindConf));
    memset(&VpssChnAttr, 0, sizeof(VpssChnAttr));

    CPlatformChannelContext *pPlatCtx = NULL;
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    SystemInfo_t		*pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    if (pPlatCtx->m_bEncoderIsStart != TRUE)
    {
        ZW_DEBUG(" m_bEncoderIsStart Not True\n");
        return 0;
    }

    if (eStream != FIRST_STREAM && eStream != SECOND_STREAM)
    {
        return 0;
    }

    for (i = 0; i < MaxSns; i++)
    {
        GrpNum = pPlatCtx->m_VPSSGroupNum[i];
        ChnNum = pPlatCtx->m_MppEncChNum[i][eStream];

        ChnType = __KS_VENC_CHN_TYPE(i, eStream);
        if (ChnType == VENC_CHNTYPE_BUTT)
        {
            continue;
        }

        BindConf.ChnType = ChnType;
        BindConf.VencSrcGrp = GrpNum;
        if (eStream == FOURTH_STREAM)
        {
            BindConf.VencSrcChn = pPlatCtx->m_VPSSChnNum[i][SECOND_STREAM];
        }
        else
        {
            BindConf.VencSrcChn = pPlatCtx->m_VPSSChnNum[i][eStream];
        }

        Ret = Venc_Chn_UnBind(0, ChnType, BindConf);
        if (Ret) {
            ZW_ERROR("Venc_Chn_UnBind chn(%d) FAIL(%#x)\n", BindConf.ChnType, Ret);
            return -1;
        }

        Ret = Venc_Chn_Stop(0, ChnType, ChnNum);
        if (Ret != 0) {
            ZW_ERROR("Venc_Chn_Get_EncodeParam failed! [%d] [%d] [%d]\n", Ret, ChnType, ChnNum);
            return -1;
        }

        Ret = Venc_Chn_Close(0, ChnType, ChnNum);
        if (Ret != 0) {
            ZW_ERROR("Venc_Chn_Get_EncodeParam failed! [%d] [%d] [%d]\n", Ret, ChnType, ChnNum);
            return -1;
        }

        Ret = Vpss_Chn_Close(0, GrpNum, pPlatCtx->m_VPSSChnNum[i][eStream]);
        if (Ret != 0) {
            ZW_ERROR("Vpss_Chn_Close failed! [%d] [%d]\n", Ret, GrpNum);
            return -1;
        }

        pPlatCtx->m_stVpssChnAttr[i][eStream].VpssParam.width  = pPlatCtx->m_stStreamAttr[i][eStream].u32Width;
        pPlatCtx->m_stVpssChnAttr[i][eStream].VpssParam.height = pPlatCtx->m_stStreamAttr[i][eStream].u32Height;

        VpssChnAttr.width = pPlatCtx->m_stVpssChnAttr[i][eStream].VpssParam.width;
        VpssChnAttr.height = pPlatCtx->m_stVpssChnAttr[i][eStream].VpssParam.height;
        VpssChnAttr.Depth = pPlatCtx->m_stVpssChnAttr[i][eStream].VpssParam.Depth;

        ZW_PRINTF("PicFormat Change ------ i[%d] GrpNum[%d] ChnNum[%d] ChnType[%d] VPSSChnNum[%d] w*h[%d*%d] [%d]\n",
            i, GrpNum, ChnNum, ChnType, pPlatCtx->m_VPSSChnNum[i][eStream], VpssChnAttr.width, VpssChnAttr.height, VpssChnAttr.Depth);

        Ret = Vpss_Chn_Open(0, GrpNum, pPlatCtx->m_VPSSChnNum[i][eStream], VpssChnAttr);
        if (Ret != 0) {
            ZW_ERROR("Vpss_Chn_Close failed! [%d] [%d] [%d]\n", Ret, GrpNum, pPlatCtx->m_VPSSChnNum[i][eStream]);
            return -1;
        }

#if SYSTEM_SUPPORT_CHANNEL <=2
        // 切换分辨率，编码类型必须要重新创建通道
            Ret = Venc_Chn_Open(0, ChnType, ChnNum, pPlatCtx->m_stVencChnAttr[i][eStream].EncParam, pSysInfo->m_VIChnFrmRate[i]);

#else
        // 切换分辨率，编码类型必须要重新创建通道
        /* Res: 使用mipi swicth的这组pipeline，平台默认是sensor帧率的2倍，因此需要做以下调整 */
        if ((MaxSns == 3) && (i == SEC_SENSOR)) {
            Ret = Venc_Chn_Open(0, ChnType, ChnNum, pPlatCtx->m_stVencChnAttr[i][eStream].EncParam, pSysInfo->m_VIChnFrmRate[i]);
        }
        else {
            Ret = Venc_Chn_Open(0, ChnType, ChnNum, pPlatCtx->m_stVencChnAttr[i][eStream].EncParam, pSysInfo->m_VIChnFrmRate[i] * 2);
        }
#endif
        if (Ret != 0) {
            ZW_ERROR("Venc_Chn_Open failed! [%d] [%d] [%d]\n", Ret, ChnType, ChnNum);
            return -1;
        }

        Ret = Venc_Chn_Start(0, ChnType, ChnNum, 0);
        if (Ret != 0) {
            ZW_ERROR("Venc_Chn_Start failed![%d] [%d] [%d]\n", Ret, ChnType, ChnNum);
            return -1;
        }

        Ret = Venc_Chn_Bind(0, ChnType, BindConf);
        if (Ret) {
            ZW_ERROR("Venc_Chn_Bind chn(%d) FAIL(%d)\n", BindConf.ChnType, Ret);
            return -1;
        }
    }

    return 0;
}

#if defined (__MODULE_4G_SUPPORT__)
int Hi_MPP_AdjustEncPram(const SystemInfo_t *pSysInfo, const ENCODE_STREAM_E Chn, const VENC_ENCODEPARAM_S *pEncParam, VENC_ENCODEPARAM_S *SrcParam)
{
    switch (pSysInfo->nCurAdustEncPramStatus)
    {
        case STATUS_PRODUCT_TEST:
            {
                /* 产测模式下编码参数 */
                SrcParam->stVencQpConf.bEnbaleQp = 1;
                SrcParam->stVencQpConf.u32MaxQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MAX_QP:AUX_STREAM_MAX_QP);
                SrcParam->stVencQpConf.u32MinQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MIN_QP_IN_PRODUCT_MODE:AUX_STREAM_MIN_QP_IN_PRODUCT_MODE);
                SrcParam->stVencQpConf.u32MaxIQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MAX_IQP:AUX_STREAM_MAX_IQP);
                SrcParam->stVencQpConf.u32MinIQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MIN_IQP_IN_PRODUCT_MODE:AUX_STREAM_MIN_IQP_IN_PRODUCT_MODE);

                if (Chn == FIRST_STREAM)
                {
                    SrcParam->BitRate = (SrcParam->BitRate > MAIN_STREAM_MAX_BITRATE_IN_PRODUCT_MODE) ? MAIN_STREAM_MAX_BITRATE_IN_PRODUCT_MODE : SrcParam->BitRate;
                    SrcParam->MaxBitRate = (SrcParam->MaxBitRate > MAIN_STREAM_MAX_BITRATE_IN_PRODUCT_MODE) ? MAIN_STREAM_MAX_BITRATE_IN_PRODUCT_MODE : SrcParam->MaxBitRate;
                }
                else if (Chn == SECOND_STREAM)
                {
                    SrcParam->BitRate = (SrcParam->BitRate > AUX_STREAM_MAX_BITRATE_IN_PRODUCT_MODE) ? AUX_STREAM_MAX_BITRATE_IN_PRODUCT_MODE : SrcParam->BitRate;
                    SrcParam->MaxBitRate = (SrcParam->MaxBitRate > AUX_STREAM_MAX_BITRATE_IN_PRODUCT_MODE) ? AUX_STREAM_MAX_BITRATE_IN_PRODUCT_MODE : SrcParam->MaxBitRate;
                }

                SrcParam->SmartEncEnable = 0;
            }
            break;
        case STATUS_CHINAMOBILE:
        case STATUS_SEND_DATA_FAIL:
            {
                /* 移动网络下编码参数 */
                SrcParam->stVencQpConf.bEnbaleQp = 1;
                SrcParam->stVencQpConf.u32MaxQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MAX_QP:AUX_STREAM_MAX_QP);
                SrcParam->stVencQpConf.u32MinQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MIN_QP_IN_CHINA_MOBILE:AUX_STREAM_MIN_QP_IN_CHINA_MOBILE);
                SrcParam->stVencQpConf.u32MaxIQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MAX_IQP:AUX_STREAM_MAX_IQP);
                SrcParam->stVencQpConf.u32MinIQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MIN_IQP_IN_CHINA_MOBILE:AUX_STREAM_MIN_IQP_IN_CHINA_MOBILE);

                if (Chn == FIRST_STREAM)
                {
                    SrcParam->BitRate = (SrcParam->BitRate > MAIN_STREAM_MAX_BITRATE_IN_CHINA_MOBILE) ? MAIN_STREAM_MAX_BITRATE_IN_CHINA_MOBILE : SrcParam->BitRate;
                    SrcParam->MaxBitRate = (SrcParam->MaxBitRate > MAIN_STREAM_MAX_BITRATE_IN_CHINA_MOBILE) ? MAIN_STREAM_MAX_BITRATE_IN_CHINA_MOBILE : SrcParam->MaxBitRate;
                }
                else if (Chn == SECOND_STREAM)
                {
                    SrcParam->BitRate = (SrcParam->BitRate > AUX_STREAM_MAX_BITRATE_IN_CHINA_MOBILE) ? AUX_STREAM_MAX_BITRATE_IN_CHINA_MOBILE : SrcParam->BitRate;
                    SrcParam->MaxBitRate = (SrcParam->MaxBitRate > AUX_STREAM_MAX_BITRATE_IN_CHINA_MOBILE) ? AUX_STREAM_MAX_BITRATE_IN_CHINA_MOBILE : SrcParam->MaxBitRate;
                }

                SrcParam->SmartEncEnable = pEncParam->SmartEncEnable;
            }
            break;
        case STATUS_NORMAL:
        default:
            {
                /* 标准模式下编码参数 */
                SrcParam->stVencQpConf.bEnbaleQp = 1;
                SrcParam->stVencQpConf.u32MaxQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MAX_QP:AUX_STREAM_MAX_QP);
                SrcParam->stVencQpConf.u32MinQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MIN_QP:AUX_STREAM_MIN_QP);
                SrcParam->stVencQpConf.u32MaxIQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MAX_IQP:AUX_STREAM_MAX_IQP);
                SrcParam->stVencQpConf.u32MinIQp = ((Chn == FIRST_STREAM)?MAIN_STREAM_MIN_IQP:AUX_STREAM_MIN_IQP);

                SrcParam->BitRate = pEncParam->BitRate;
                SrcParam->MaxBitRate = pEncParam->BitRate;

                SrcParam->SmartEncEnable = pEncParam->SmartEncEnable;
            }
            break;
    }

    return 0;
}
#endif

int Hi_MPP_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval, int *nKeyFrameInterval)
{
    int s32ret = 0;

    VENC_ENCODEPARAM_S SrcParam = {0};
    SINT32 SrcFrmRate = 0;

    s32ret = Venc_Chn_Get_EncodeParam(0, VENC_CHNTYPE_MAIN, 0, &SrcParam , &SrcFrmRate);
    if (s32ret < 0 && NULL == nKeyFrameInterval && NULL == nFrameRateInterval)
    {
        ZW_ERROR("Hi_MPP_GetRealKeyFrameInterval failed!s32ret=0x%x,channel=%d,\n",s32ret,nChannel);
        return -1;

    }
    *nKeyFrameInterval = SrcParam.KeyInterval;
    *nFrameRateInterval = SrcParam.FrameRate;
    return 0;
}

/**
 * @description: 设置编码参数(主要是设置编码参数，不涉及分辨率)
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {CODEC_PARA_S} *pParam 用户设置的配置
 * @param {int} Chn 编码通道号
 * @param {EncodeType_t} encType 编码类型
 * @param {int} iSetFrom :1 内部动态修改  0:上层设置
 * @param {int} SlowFrameMode :低帧率模式
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetEncPara(void *hChannelHandle, CODEC_PARA_S *pParam, ENCODE_STREAM_E Chn, int iSetFrom, int SlowFrameMode)
{
    SINT32 s32Ret = 0;
    UINT32 eEncType = -1;
    VENC_ENCODEPARAM_S SrcParam = {0};
    SINT32 SrcFrmRate = 0;

    VENC_CHNTYPE_E eChnType = 0;
    VENC_ENCODEPARAM_S *pEncParam = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    SystemInfo_t		*pSysInfo = NULL;
    SINT32 maxFrameRate = 25;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    HY_Res_GetSystemInfo(&pSysInfo);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    if (NULL == hChannelHandle || NULL == pParam)
    {
        ZW_ERROR("PARAM CANNOT be NULL\n");
        return -1;
    }

#if defined (__MODULE_4G_SUPPORT__)
    if (iSetFrom == 0 && pSysInfo->nCurAdustEncPramStatus != STATUS_NORMAL)
    {
        ZW_PRINTF("CurAdustEncPramStatus Not Normal!\n");
        return 0;
    }
#endif

    if (Chn != FIRST_STREAM && Chn != SECOND_STREAM)
    {
        ZW_ERROR("PARAM Chn Err [%d]\n", Chn);
        return -1;
    }

    s32Ret = Hi_MPP_ConverEncTypeToPlatform(pParam->EncType, &eEncType);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_ConverEncTypeToPlatform failed\n");
        return -1;
    }

    for (int i = 0; i < MaxSns; i++)
    {
        eChnType = __KS_VENC_CHN_TYPE(i, Chn);
        if (eChnType == VENC_CHNTYPE_BUTT)
        {
            continue;
        }

        pEncParam = &pPlatCtx->m_stVencChnAttr[i][Chn].EncParam;

        //避免频繁重置编码参数
        if (pEncParam->BitRate != pParam->BitRate || pEncParam->BitRateType != pParam->BitRateCtlMode
            || pEncParam->FrameRate != pParam->FrameRate || pEncParam->Quality != pParam->PicQualityLever
            || pEncParam->VPayloadType != eEncType || 1 == iSetFrom)
        {
            pEncParam->BitRate = pParam->BitRate;
            pEncParam->BitRateType = pParam->BitRateCtlMode;

            if (pEncParam->FrameRate != pParam->FrameRate)
            {
                // 这里的设置编码属性，使用上一次的帧率进行设置，然后本次的帧率修改在设置帧率的地方进行设置
                pEncParam->FrameRate = pParam->FrameRate ? pParam->FrameRate:((pSysInfo->m_Frequ_HZ==CMOS_FREQUENCY_60HZ) ? 30 : 25);
                Hi_MPP_GetGOPValueByNetType(pParam->FrameRate, &pEncParam->KeyInterval);
            }
            pEncParam->Quality = pParam->PicQualityLever;
            pEncParam->VPayloadType = eEncType;

            s32Ret = Venc_Chn_Get_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][Chn], &SrcParam, &SrcFrmRate);
            if (s32Ret < 0)
            {
                ZW_ERROR("Venc_Chn_Get_EncodeParam failed Chn[%d][%d][%d]\n", eChnType, i, Chn);
                return -1;
            }

            Hi_MPP_GetMaxFrameRateByAntiflashAndNetType(&maxFrameRate);
            if(1 == iSetFrom)
            {
                memcpy(&SrcParam, pEncParam, sizeof(VENC_ENCODEPARAM_S));
                SrcParam.FrameRate = SrcParam.FrameRate > maxFrameRate ? maxFrameRate : SrcParam.FrameRate;
                Hi_MPP_GetGOPValueByNetType(SrcParam.FrameRate, &SrcParam.KeyInterval);
                ZW_DEBUG("Chn[%d][%d][%d] SrcFrmRate(%d) VencFrameRate(%d) UserSetVencFrameRate(%d) BitRateType(%d) BitRate(%d) MaxBitRate(%d) \n",
                     eChnType, i, Chn, SrcFrmRate, SrcParam.FrameRate, pEncParam->FrameRate, SrcParam.BitRateType, SrcParam.BitRate, SrcParam.MaxBitRate);
            }
            else
            {
                pEncParam->FrameRate = pEncParam->FrameRate > maxFrameRate ? maxFrameRate : pEncParam->FrameRate;
                Hi_MPP_GetGOPValueByNetType(pParam->FrameRate, &pEncParam->KeyInterval);
                ZW_DEBUG("Chn[%d][%d][%d]  SrcFrmRate(%d) VencFrameRate(%d) UserSetVencFrameRate(%d) BitRateType(%d) BitRate(%d) \n",
                     eChnType, i, Chn, SrcFrmRate, pEncParam->FrameRate, pEncParam->FrameRate, pEncParam->BitRateType,pEncParam->BitRate);
                memcpy(&SrcParam, pEncParam, sizeof(VENC_ENCODEPARAM_S));
            }

            if (SLOWFRAME_SCENE_FORCESLOW == SlowFrameMode)
            {
                pEncParam->FrameRate = SLOWFRAME_ENCODER_RATE;
                pEncParam->KeyInterval = pEncParam->FrameRate * SLOWFRAME_GOP_RATIO;
                SrcParam.FrameRate = SLOWFRAME_ENCODER_RATE;
                SrcParam.KeyInterval= SrcParam.FrameRate * SLOWFRAME_GOP_RATIO;
            }

            if (SrcParam.SmartEncEnable)
            {
                int j = 0;
                for (j = SMART_P_GOP_RATIO; j >= 0; j--)
                {
                    /* BgInterval必须满足两个条件：1、GOP的整数倍 2、BgInterval/FrameRate小于等于60 */
                    if (((SrcParam.KeyInterval*j) / SrcParam.FrameRate) <= 60)
                    {
                        pEncParam->BgInterval = pEncParam->KeyInterval*j;
                        SrcParam.BgInterval = SrcParam.KeyInterval*j;
                        break;
                    }
                }
            }
            SrcParam.BitRate = pEncParam->BitRate;
            SrcParam.MaxBitRate = pEncParam->BitRate * 4 / 3;

            #if defined (__MODULE_4G_SUPPORT__)
            Hi_MPP_AdjustEncPram(pSysInfo, Chn, pEncParam, &SrcParam);
            #endif

            ZW_DEBUG("Chn[%d][%d][%d] BitRate=%d MaxBitRate=%d, FrameRate=%d, KeyInterval=%d\n", eChnType, i, Chn, SrcParam.BitRate, SrcParam.MaxBitRate, SrcParam.FrameRate, SrcParam.KeyInterval);
            s32Ret = Venc_Chn_Set_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][Chn], SrcParam, SrcFrmRate);
            if (s32Ret < 0)
            {
                ZW_ERROR("Venc_Chn_Set_EncodeParam failed, Chn[%d][%d][%d]\n", eChnType, i, Chn);
                return -1;
            }

            if (Chn == FIRST_STREAM)
            {
                Hi_MPP_SetQPAttr(eChnType, pPlatCtx->m_MppEncChNum[i][Chn]);
            }

            if (Chn == FIRST_STREAM || Chn == SECOND_STREAM)
            {
                Hi_MPP_Set_SuperFrameStrategy(eChnType, pPlatCtx->m_MppEncChNum[i][Chn]);
            }

            //重置编码后将RC还原至用户设置,避免强制CBR后由于RC模式不一致频繁重置编码
            pEncParam->BitRateType = pParam->BitRateCtlMode;
        }
    }

    ZW_DEBUG("Chang Encode Param SUCCESS Chn[%d]\n", Chn);
    return 0;
}

/**
********************************************************************************
 * @函数功能: 用户修改编码的帧率， 同时修改GOP
 * @输入参数: hChannelHandle:通道句柄
 *           ChnType:通道类型
 *           FrmRate:编码帧率
 * @输出参数: 无
 * @返回值  : 0:成功
 *           非0:失败
 * @历史    :
 *           2018-08-01  HL.LIU   新建
********************************************************************************
*/
SINT32 Hi_MPP_SetVencFrameRateV2(void *hChannelHandle, ENCODE_STREAM_E ChnType, int FrmRate, int slowframemode)
{
    int nRet = 0;
    int SrcFrmRate = 0;
#if defined(__IS_SUPPORT_12FPS__)
    int maxFrameRate = 12;
#else
    int maxFrameRate = 15;
#endif
    VENC_CHNTYPE_E eChnType = 0;
    VENC_ENCODEPARAM_S EncParam = {0};
    VENC_ENCODEPARAM_S *pEncParam = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("Venc_Chn_Set_FrmRate hChannelHandle is NULL\n");
        return -1;
    }
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    //Hi_MPP_GetMaxFrameRateByAntiflashAndNetType(&maxFrameRate);
    FrmRate = FrmRate > maxFrameRate ? maxFrameRate : FrmRate;
    for (int i = 0; i < MaxSns; i++)
    {
        eChnType = __KS_VENC_CHN_TYPE(i, ChnType);
        if (eChnType == VENC_CHNTYPE_BUTT)
        {
            continue;
        }

        nRet = Venc_Chn_Get_EncodeParam(0, eChnType, 0, &EncParam, &SrcFrmRate);
        if (nRet)
        {
            ZW_ERROR("Venc_Chn_Get_EncodeParam FAIL(%d)\n", nRet);
            return nRet;
        }

        pEncParam = &pPlatCtx->m_stVencChnAttr[i][eChnType].EncParam;

        if (SLOWFRAME_SCENE_FORCESLOW == slowframemode)
        {
            pEncParam->FrameRate = SLOWFRAME_ENCODER_RATE;
            pEncParam->KeyInterval = pEncParam->FrameRate * SLOWFRAME_GOP_RATIO;
        }
        else
        {
            pEncParam->FrameRate = FrmRate;
            Hi_MPP_GetGOPValueByNetType(pEncParam->FrameRate, &pEncParam->KeyInterval);
        }

        if (EncParam.SmartEncEnable)
        {
            int j = 0;
            for (j = SMART_P_GOP_RATIO; j >= 0; j--)
            {
                /* BgInterval必须满足两个条件：1、GOP的整数倍 2、BgInterval/FrameRate小于等于60 */
                if (((pEncParam->KeyInterval*j) / pEncParam->FrameRate) <= 60)
                {
                    pEncParam->BgInterval = pEncParam->KeyInterval*j;
                    EncParam.BgInterval= pEncParam->BgInterval;
                    break;
                }
            }
        }
        EncParam.FrameRate = pEncParam->FrameRate;
        EncParam.KeyInterval= pEncParam->KeyInterval;

        nRet = Venc_Chn_Set_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][ChnType], EncParam, SrcFrmRate);
        if (nRet)
        {
            ZW_ERROR("Venc_Chn_Set_EncodeParam FAIL(%d)\n", nRet);
            return nRet;
        }

        if (ChnType == FIRST_STREAM)
        {
            Hi_MPP_SetQPAttr(eChnType, pPlatCtx->m_MppEncChNum[i][ChnType]);
        }
    }

    return 0;
}

/**
 * @description: 更新用户设置的分辨率
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {ResolutionParam_t} *m_EncAttr用户设置的主码流分辨率
 * @param {ResolutionParam_t} *m_EncAttrAux用户设置的子码流分辨率
 * @param {int} ifCaptureThird是否支持第三码流
 * @param {ResolutionParam_t} *m_EncAttrThird用户设置的第三码流分辨率
 * @param {int} ifCaptureFourth是否支持第四码流
 * @param {ResolutionParam_t} *m_EncAttrFourth用户设置的第四码流分辨率
 * @return {SINT32} 成功 0, 失败 -1;
 */
SINT32 Hi_MPP_UpdatePictureFormat(void *hChannelHandle,
                                          ResolutionParam_t *m_EncAttr,
                                          ResolutionParam_t *m_EncAttrAux,
                    int ifCaptureFourth,  ResolutionParam_t *m_EncAttrFourth)
{
    CPlatformChannelContext *pPlatCtx = NULL;
    CChannelContext *pChCtx = NULL;
    int nOsdChan = 0;

    HY_Enc_GetEncChInfo(&pChCtx);

    if (NULL == pChCtx)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    if (NULL == hChannelHandle || NULL == m_EncAttr || NULL == m_EncAttrAux)
    {
        ZW_ERROR("Params CANNOT be NULL\n");
        return -1;
    }

    if (1 == ifCaptureFourth && NULL == m_EncAttrFourth)
    {
        ZW_ERROR("THird or Fourth Params CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    pPlatCtx->m_VIOutMaxWidth = MAIN_STREAM_WIDTH_MAX;
    pPlatCtx->m_VIOutMaxHeight = MAIN_STREAM_HEIGHT_MAX;

    // FIRST_STREAM
    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].bEnable = TRUE;
    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].u32Width = m_EncAttr->m_PicWidth;
    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].u32Height = m_EncAttr->m_PicHeight;

    /* 由于主码流修改编码参数会重新设置尺寸,所以也需要更新分辨率 */
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.width = m_EncAttr->m_PicWidth;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.height = m_EncAttr->m_PicHeight;

    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].bUseOSD = TRUE;
    if (pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].bUseOSD)
    {
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].s32TimeOsdID = nOsdChan++;
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].s32TextOsdID = nOsdChan++;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FIRST_STREAM].s32ZoomOsdID = nOsdChan++;
#endif
    }

    nOsdChan = 0;
    // SECOND_STREAM
    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].bEnable = TRUE;
    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].u32Width = m_EncAttrAux->m_PicWidth;
    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].u32Height = m_EncAttrAux->m_PicHeight;

    /* 由于子码流修改编码参数会重新设置尺寸,所以也需要更新分辨率 */
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.width = m_EncAttrAux->m_PicWidth;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.height = m_EncAttrAux->m_PicHeight;

    pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].bUseOSD = TRUE;
    if (pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].bUseOSD)
    {
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].s32TimeOsdID = nOsdChan++;
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].s32TextOsdID = nOsdChan++;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][SECOND_STREAM].s32ZoomOsdID = nOsdChan++;
#endif
    }

    if (ifCaptureFourth)
    {
        // FOURTH_STREAM 抓图码流
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].bEnable = TRUE;

        if (SameAsMainStream == pChCtx->m_CaptureType)
        {
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].u32Width = m_EncAttr->m_PicWidth;
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].u32Height = m_EncAttr->m_PicHeight;
            /* 由于MJPEG修改编码参数会重新设置尺寸,所以也需要更新分辨率 */
            pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.width = m_EncAttr->m_PicWidth;
            pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.height = m_EncAttr->m_PicHeight;
        }
        else if (SameAsAuxStream == pChCtx->m_CaptureType)
        {
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].u32Width = m_EncAttrAux->m_PicWidth;
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].u32Height = m_EncAttrAux->m_PicHeight;

            /* 由于子码流修改编码参数会重新设置尺寸,所以也需要更新分辨率 */
            pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.width = m_EncAttrAux->m_PicWidth;
            pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.height = m_EncAttrAux->m_PicHeight;
        }

        nOsdChan = 0;
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].bUseOSD = TRUE;
        if (pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].bUseOSD)
        {
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].s32TimeOsdID = nOsdChan++;
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].s32TextOsdID = nOsdChan++;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
            pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].s32ZoomOsdID = nOsdChan++;
#endif
        }
    }
    else
    {
        pPlatCtx->m_stStreamAttr[FIRST_SENSOR][FOURTH_STREAM].bEnable = FALSE;
    }
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
    /* 次Sensor使用主Sensor参数，直接拷贝 */
    memcpy(&(pPlatCtx->m_stStreamAttr[SEC_SENSOR]), &(pPlatCtx->m_stStreamAttr[FIRST_SENSOR]), sizeof(XRS_StreamAttr)*MAX_STREAM_COUNT);
    memcpy(&(pPlatCtx->m_stVencChnAttr[SEC_SENSOR]), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR]), sizeof(XRS_VENC_ChnAttr)*MAX_STREAM_COUNT);
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    memcpy(&(pPlatCtx->m_stStreamAttr[THIRD_SENSOR]), &(pPlatCtx->m_stStreamAttr[FIRST_SENSOR]), sizeof(XRS_StreamAttr)*MAX_STREAM_COUNT);
    memcpy(&(pPlatCtx->m_stVencChnAttr[THIRD_SENSOR]), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR]), sizeof(XRS_VENC_ChnAttr)*MAX_STREAM_COUNT);
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
    memcpy(&(pPlatCtx->m_stStreamAttr[FOURTH_SENSOR]), &(pPlatCtx->m_stStreamAttr[FIRST_SENSOR]), sizeof(XRS_StreamAttr)*MAX_STREAM_COUNT);
    memcpy(&(pPlatCtx->m_stVencChnAttr[FOURTH_SENSOR]), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR]), sizeof(XRS_VENC_ChnAttr)*MAX_STREAM_COUNT);
#endif
    return 0;
}

/**
 * @description: 将用户设置编码参数 XRS_VENC_ChnAttr 中
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {CODEC_PARA_S} *m_EncAttr用户设置的主码流编码参数
 * @param {CODEC_PARA_S} *m_EncAttrAux用户设置的子码流编码参数
 * @param {int} ifCaptureThird是否支持第三码流
 * @param {CODEC_PARA_S} *m_EncAttrThird用户设置的第三码流编码参数
 * @param {int} ifCaptureFourth是否支持第四码流
 * @param {CODEC_PARA_S} *m_EncAttrFourth用户设置的第四码流编码参数
 * @return {SINT32} 成功 0, 失败 -1;
 */
SINT32 Hi_MPP_InitUserEncodeParam(void *hChannelHandle,
                                          CODEC_PARA_S *m_EncAttr,
                                          CODEC_PARA_S *m_EncAttrAux,
                    int ifCaptureFourth,  CODEC_PARA_S *m_EncAttrFourth)
{
    SINT32 s32Ret = 0;
    UINT32 eEncType = 0, eEncTypeAux = 0, eEncTypeFourth = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    CChannelContext *pChCtx = NULL;
    SystemInfo_t		*pSysInfo = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);
    HY_Res_GetSystemInfo(&pSysInfo);

    if (NULL == pChCtx)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    if (NULL == hChannelHandle || NULL == m_EncAttr || NULL == m_EncAttrAux)
    {
        ZW_ERROR("Params CANNOT be NULL\n");
        return -1;
    }

    if (1 == ifCaptureFourth && NULL == m_EncAttrFourth)
    {
        ZW_ERROR("THird or Fourth Params CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    // FIRST_STREAM
    s32Ret = Hi_MPP_ConverEncTypeToPlatform(m_EncAttr->EncType, &eEncType);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_ConverEncTypeToPlatform failed\n");
        return -1;
    }

    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.VPayloadType 	= eEncType;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.width      	    = m_EncAttr->PictureRes.m_PicWidth;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.height      	= m_EncAttr->PictureRes.m_PicHeight;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.BitRate     	= m_EncAttr->BitRate ? m_EncAttr->BitRate:INIT_MAIN_BPS;
    m_EncAttr->FrameRate = m_EncAttr->FrameRate ? m_EncAttr->FrameRate:((pSysInfo->m_Frequ_HZ==CMOS_FREQUENCY_60HZ) ? 30 : 25);
    Hi_MPP_GetGOPValueByNetType(m_EncAttr->FrameRate, (UINT16 *)(&m_EncAttr->KeyFrameInterval));
    if (SLOWFRAME_SCENE_FORCESLOW == pChCtx->m_iSlowFrameMode)
    {
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.FrameRate   = SLOWFRAME_ENCODER_RATE;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.KeyInterval = SLOWFRAME_ENCODER_RATE*SLOWFRAME_GOP_RATIO;
    }
    else
    {
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.FrameRate   = m_EncAttr->FrameRate;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.KeyInterval = m_EncAttr->KeyFrameInterval;
    }
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.Quality     	= m_EncAttr->PicQualityLever;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.BitRateType 	= m_EncAttr->BitRateCtlMode;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.bOffLine    	= 0;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.bEnbaleQp = 1;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.u32MaxQp = MAIN_STREAM_MAX_QP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.u32MinQp = MAIN_STREAM_MIN_QP;//太大，恒定码率达不到
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.u32MaxIQp = MAIN_STREAM_MAX_IQP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.u32MinIQp = MAIN_STREAM_MIN_IQP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.u8MaxPictureQuality = 41;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.stVencQpConf.u16MaxPictureSize = ENC_MAIN_STREAM_BUF_SIZE / 1024;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.StreamBufSize = ENC_MAIN_STREAM_BUF_SIZE;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.MaxBitRate    = pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.BitRate * 4 / 3;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.SmartEncEnable = (m_EncAttr->EncType == LtH265PlusVideo) ? 1 : 0;

    if (pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.SmartEncEnable == 1)
    {
        int j = 0;
        for (j = SMART_P_GOP_RATIO; j >= 0; j--)
        {
            /* BgInterval必须满足两个条件：1、GOP的整数倍 2、BgInterval/FrameRate小于等于60 */
            if (((pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.KeyInterval*j) / pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.FrameRate) <= 60)
            {
                pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.BgInterval = j*pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam.KeyInterval;
                break;
            }
        }
    }

    /* 次Sensor默认使用主sensor参数，直接拷贝 */
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
    memcpy(&(pPlatCtx->m_stVencChnAttr[SEC_SENSOR][FIRST_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    memcpy(&(pPlatCtx->m_stVencChnAttr[THIRD_SENSOR][FIRST_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
    memcpy(&(pPlatCtx->m_stVencChnAttr[FOURTH_SENSOR][FIRST_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FIRST_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif

    // SECOND_STREAM
    s32Ret = Hi_MPP_ConverEncTypeToPlatform(m_EncAttrAux->EncType, &eEncTypeAux);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_ConverEncTypeToPlatform failed\n");
        return -1;
    }
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.VPayloadType = eEncTypeAux;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.width       = m_EncAttrAux->PictureRes.m_PicWidth;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.height      = m_EncAttrAux->PictureRes.m_PicHeight;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.BitRate     = m_EncAttrAux->BitRate ? m_EncAttrAux->BitRate:INIT_AUX_BPS;
    //m_EncAttrAux->FrameRate ? m_EncAttrAux->FrameRate:MAX_AUX_FPS;
    Hi_MPP_GetGOPValueByNetType(m_EncAttrAux->FrameRate, (UINT16 *)(&m_EncAttrAux->KeyFrameInterval));
    if (SLOWFRAME_SCENE_FORCESLOW == pChCtx->m_iSlowFrameMode)
    {
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.FrameRate   = SLOWFRAME_ENCODER_RATE;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.KeyInterval = SLOWFRAME_ENCODER_RATE*SLOWFRAME_GOP_RATIO;
    }
    else
    {
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.FrameRate   = m_EncAttrAux->FrameRate;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.KeyInterval = m_EncAttrAux->KeyFrameInterval;
    }
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.Quality     = m_EncAttrAux->PicQualityLever;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.BitRateType = m_EncAttrAux->BitRateCtlMode;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.bOffLine    = 0;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.bEnbaleQp = 1;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.u32MaxQp = AUX_STREAM_MAX_QP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.u32MinQp = AUX_STREAM_MIN_QP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.u32MaxIQp = AUX_STREAM_MAX_IQP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.u32MinIQp = AUX_STREAM_MIN_IQP;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.u8MaxPictureQuality = 41;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.MaxBitRate    = pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.BitRate * 4 / 3;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.stVencQpConf.u16MaxPictureSize = ENC_SUB_STREAM_BUF_SIZE / 1024;
    pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam.StreamBufSize = ENC_SUB_STREAM_BUF_SIZE;

    /* 次Sensor默认使用主sensor参数，直接拷贝 */
#if	 SYSTEM_SUPPORT_CHANNEL >=2
    memcpy(&(pPlatCtx->m_stVencChnAttr[SEC_SENSOR][SECOND_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
#if	SYSTEM_SUPPORT_CHANNEL >=3
    memcpy(&(pPlatCtx->m_stVencChnAttr[THIRD_SENSOR][SECOND_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
#if  SYSTEM_SUPPORT_CHANNEL >=4
    memcpy(&(pPlatCtx->m_stVencChnAttr[FOURTH_SENSOR][SECOND_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][SECOND_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
    if (ifCaptureFourth)
    {
        // FOURTH_STREAM
        s32Ret = Hi_MPP_ConverEncTypeToPlatform(m_EncAttrFourth->EncType, &eEncTypeFourth);
        if (s32Ret < 0)
        {
            ZW_ERROR("Hi_MPP_ConverEncTypeToPlatform failed\n");
            return -1;
        }
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.VPayloadType = eEncTypeFourth;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.width       = m_EncAttrFourth->PictureRes.m_PicWidth;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.height      = m_EncAttrFourth->PictureRes.m_PicHeight;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.BitRate     = m_EncAttrFourth->BitRate ? m_EncAttrFourth->BitRate:INIT_MAIN_BPS;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.FrameRate   = m_EncAttrFourth->FrameRate;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.Quality     = m_EncAttrFourth->PicQualityLever;
        Hi_MPP_GetGOPValueByNetType(m_EncAttrFourth->FrameRate, &pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.KeyInterval);
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.BitRateType = BITRATE_TYPE_FIXQP;//m_EncAttrFourth->BitRateCtlMode;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.bOffLine    = 0;   /* 离线 */
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.stVencQpConf.bEnbaleQp = 1;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.stVencQpConf.u32MaxQp = 51;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.stVencQpConf.u32MinQp = 32;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.stVencQpConf.u32MaxIQp = 51;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.stVencQpConf.u32MinIQp = 28;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.MjpegFollowType = VENC_MJPEG_FOLLOW_MINOR;
        pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam.StreamBufSize = ENC_SUB_STREAM_BUF_SIZE;

        /* 次Sensor默认使用主sensor参数，直接拷贝 */
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
        memcpy(&(pPlatCtx->m_stVencChnAttr[SEC_SENSOR][FOURTH_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
        memcpy(&(pPlatCtx->m_stVencChnAttr[THIRD_SENSOR][FOURTH_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
        memcpy(&(pPlatCtx->m_stVencChnAttr[FOURTH_SENSOR][FOURTH_STREAM].EncParam), &(pPlatCtx->m_stVencChnAttr[FIRST_SENSOR][FOURTH_STREAM].EncParam), sizeof(VENC_ENCODEPARAM_S));
#endif
    }

    return 0;
}

/**
 * @description: 设置用户参数
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} boardInfo 版型
 * @param {CODEC_PARA_S} *m_EncAttr用户设置的主码流编码参数
 * @param {CODEC_PARA_S} *m_EncAttrAux用户设置的子码流编码参数
 * @param {int} ifCaptureThird是否支持第三码流
 * @param {CODEC_PARA_S} *m_EncAttrThird用户设置的第三码流编码参数
 * @param {int} ifCaptureFourth是否支持第四码流
 * @param {CODEC_PARA_S} *m_EncAttrFourth用户设置的第四码流编码参数
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetUserParam(void *hChannelHandle, int boardInfo,
                                    CODEC_PARA_S *m_EncAttr,
                                    CODEC_PARA_S *m_EncAttrAux,
              int ifCaptureFourth,CODEC_PARA_S *m_EncAttrFourth)
{
    SINT32 RetVal = 0;
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle || NULL == m_EncAttr || NULL == m_EncAttrAux || NULL == m_EncAttrFourth)
    {
        ZW_ERROR("Params CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    pPlatCtx->m_BoardInfo = boardInfo;

    /*更新用户设置的分辨率*/
    RetVal = Hi_MPP_UpdatePictureFormat(pPlatCtx, &m_EncAttr->PictureRes, &m_EncAttrAux->PictureRes, ifCaptureFourth, &m_EncAttrFourth->PictureRes);
    if (RetVal < 0)
    {
        ZW_ERROR("Hi_MPP_UpdatePictureFormat Failed\n");
        return -1;
    }

    /*初始化用户设置的编码参数*/
    RetVal = Hi_MPP_InitUserEncodeParam(pPlatCtx, m_EncAttr, m_EncAttrAux, ifCaptureFourth, m_EncAttrFourth);
    if (RetVal < 0)
    {
        ZW_ERROR("Hi_MPP_InitUserEncodeParam Failed\n");
        return -1;
    }

    pPlatCtx->m_VIOutMaxWidth  = MAIN_STREAM_WIDTH_MAX;
    pPlatCtx->m_VIOutMaxHeight = MAIN_STREAM_HEIGHT_MAX;

    pPlatCtx->m_MaxViChn = HY_Res_GetMaxSensorCnt();
    pPlatCtx->m_ChipConf.TgId = 0;
    pPlatCtx->m_ChipConf.ChipNum = 0;

    pPlatCtx->m_AudioInVol = 95;
    pPlatCtx->m_AudioOutVol = 85;

#if defined(__PRODUCT_G85T__)
    pPlatCtx->m_FullAudioInVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioIn", 92, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_FullAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioOut", 93, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioInVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioIn", 90, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioOut", 83, CUSTOM_CONFIG_FILE);
#elif defined(__PRODUCT_W85T__)
    pPlatCtx->m_FullAudioInVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioIn", 95, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_FullAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioOut", 93, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioInVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioIn", 90, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioOut", 83, CUSTOM_CONFIG_FILE);
#elif defined(__PRODUCT_P4__) || defined(__PRODUCT_P4U__)
    pPlatCtx->m_FullAudioInVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioIn", 95, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_FullAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioOut", 95, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioInVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioIn", 90, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioOut", 95, CUSTOM_CONFIG_FILE);
#elif defined(__PRODUCT_P3MU__)
    pPlatCtx->m_FullAudioInVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioIn", 90, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_FullAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioOut", 89, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioInVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioIn", 91, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioOut", 93, CUSTOM_CONFIG_FILE);
#else
    pPlatCtx->m_FullAudioInVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioIn", 90, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_FullAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "FullAudioOut", 95, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioInVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioIn", 90, CUSTOM_CONFIG_FILE);
    pPlatCtx->m_HalfAudioOutVol = (unsigned int)read_profile_int("AudioMapping", "HalfAudioOut", 95, CUSTOM_CONFIG_FILE);
#endif
    ZW_INFO(" Audio Mapping [%d][%d][%d][%d]\n",pPlatCtx->m_FullAudioInVol,pPlatCtx->m_FullAudioOutVol,pPlatCtx->m_HalfAudioInVol,pPlatCtx->m_HalfAudioOutVol);

    return 0;
}

/**
 * @description: 切换图像参数后 ISP 需要更新用户设置
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ResetIspParam(void)
{
#if 0
    SINT32 s32Ret = -1;
    CChannelContext *pChCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);

    ZW_INFO("Start Hi_MPP_ResetIspParam...\n");

    s32Ret = Hi_MPP_SetDefogStrengthParam(pChCtx->pPlatformChannelhandle, pChCtx->FogStrength);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetDefogStrengthParam failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetDefogStrengthParam ... \n");

    s32Ret = Hi_MPP_SetImgPara(pChCtx->pPlatformChannelhandle, pChCtx->Bright, pChCtx->Contrast, pChCtx->Hue, pChCtx->Saturation, pChCtx->Sharpness);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetImgPara failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetImgPara ... \n");

    s32Ret = Hi_MPP_SetWhiteBalance(pChCtx->pPlatformChannelhandle, pChCtx->WhiteBalance.WBEnable,  pChCtx->WhiteBalance.WBWorkMode, pChCtx->WhiteBalance.RedGain, pChCtx->WhiteBalance.BlueGain);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetWhiteBalance failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetWhiteBalance ... \n");

    s32Ret = Hi_MPP_SetMCTFStrength(pChCtx->pPlatformChannelhandle, pChCtx->MCTFStrength);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetMCTFStrength failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetMCTFStrength ... \n");

    s32Ret = Hi_MPP_EnableBacklightMode(pChCtx->pPlatformChannelhandle, pChCtx->m_BacklightEnable);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_EnableBacklightMode failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_EnableBacklightMode ... \n");

    s32Ret = Hi_MPP_SetLocalExpMode(pChCtx->pPlatformChannelhandle, pChCtx->m_WDRLevel);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetLocalExpMode failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetLocalExpMode ... \n");

    s32Ret = Hi_MPP_SetAEParam(pChCtx->pPlatformChannelhandle, pChCtx->AEParam.antiFlicker, pChCtx->AEParam.MinShutterTime, pChCtx->AEParam.MaxShutterTime, pChCtx->AEParam.sensorGain, pChCtx->m_ae_mode);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetAEParam failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetAEParam ... \n");

    s32Ret = Hi_MPP_SetAntiFlickerStrengthParam(pChCtx->pPlatformChannelhandle, pChCtx->m_antiFlickerStrength);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetAntiFlickerStrengthParam failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_INFO("Hi_MPP_SetAntiFlickerStrengthParam ... \n");

    s32Ret = Hi_MPP_SetLocalImageMode(pChCtx->pPlatformChannelhandle, pChCtx->m_LocalImageMode);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_SetLocalImageMode failed, s32Ret = %#x\n", s32Ret);
        return -1;
    }
    ZW_DEBUG("Hi_MPP_ResetIspParam SUCCESS\n");

#else
    /* AIOT设备用户暂不支持修改图像参数，直接返回 */
    SINT32 s32Ret = 0;
#endif
    return s32Ret;
}


/**
 * @description: 重启编码器
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_RestartEnc(void *hChannelHandle)
{
    SINT32 s32Ret = -1;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    ZW_DEBUG("Hi_MPP_RestartEnc In-------\n");

    s32Ret = Hi_MPP_StopOSD(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_StopOSD fail");

    //isp初始化标记关闭
    pPlatCtx->m_bISPInitIsOver = FALSE;

    /* 反初始化编码各个模块 */
    s32Ret = Hi_MPP_DeInit();
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_DeInit fail");

    // /* 重新初始化编码 */
    if (false == pChCtx->m_bChangeCodePara)
    {
        s32Ret = Hi_MPP_InitVi(pChCtx->pPlatformChannelhandle);
        DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVi fail");

        s32Ret = Hi_MPP_InitIsp(pChCtx->pPlatformChannelhandle);
        DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitIsp fail");
    }
    pPlatCtx->m_bISPInitIsOver = TRUE;

    s32Ret = Hi_MPP_InitOsd(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitOsd fail");

    s32Ret = Hi_MPP_InitVda(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVda fail");

    s32Ret = Hi_MPP_InitVpssChn(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVpssChn fail");

    s32Ret = Hi_MPP_InitVpss(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVpss fail");

    s32Ret = Hi_MPP_InitVenc(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVenc fail");

    s32Ret = Hi_MPP_InitEncChn(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitEncChn fail");

#if defined (__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH_JXK06A, 0);
#elif defined(__PRODUCT_G85T__) || defined(__PRODUCT_W85T__)
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH_F38PA, 0);
#elif defined(__PRODUCT_G43TA__) || defined (__PRODUCT_G84F__)||defined (__PRODUCT_W84F__)|| defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH_GC20C3, 0);
#else
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH, 0);
#endif

    if (s32Ret < 0)
    {
        ZW_ERROR("Visp_Start_SceneAuto failed\n");
        Visp_Stop_SceneAuto(0);
        return -1;
    }

    s32Ret = Hi_MPP_InitVdaChn(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVdaChn fail");

    s32Ret = Hi_MPP_StartOSD(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_StartOSD fail");


#if defined(__IS_SUPPORT_LCD_MANAGER__)
    s32Ret = Xrs_Lcd_StartLCD(pChCtx->pPlatformChannelhandle);

    s32Ret = Xrs_Lcd_StartVdec(pChCtx->pPlatformChannelhandle);
#endif

    return 0;
}

/*************************************ISP相关接口*******************************************/
/**
 * @description: 设置去雾强度
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} strength 用户设置的去雾强度
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetDefogStrengthParam(void *hChannelHandle, int strength)
{
#if (USE_VISP)
    SINT32 s32Ret = 0;
    ISP_TUNE_DEHAZE_PARAM_S  m_DehazeParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (strength < 0)
    {
        ZW_ERROR("get strength less 0 error!\n");
        strength = 0;
    }
    else if (strength > 100)
    {
        ZW_ERROR("get strength bigger 100 error!\n");
        strength = 100;
    }

    m_DehazeParam.DehazeMode = ISP_TUNE_DEHAZE_ON;
    m_DehazeParam.DehazeLevel = strength;

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneDehaze(nSenIndex, &m_DehazeParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneDehaze fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("Hi_MPP_SetDefogStrengthParam SUCCESS\n");
#endif
    return 0;
}

/**
 * @description: 设置图像参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} luma 亮度
 * @param {int} Contrast 对比度
 * @param {int} hue 色度
 * @param {int} Saturation 饱和度
 * @param {int} Sharpness 锐度
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32  Hi_MPP_SetImgPara(void *hChannelHandle, int Luma, int Contrast, int Hue, int Saturation, int Sharpness)
{
    SINT32 s32Ret = 0;
#if (USE_VISP)
    ISP_TUNE_IMAGE_STYLE_PARAM_S m_ImageStyleParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

#ifdef SUPPORT_ALGO_DEBUG
    // 算法参数配置，主要用于调试
    char fileName[256] = {0};

    snprintf(fileName, sizeof(fileName), "%s/%s", SPEECH_ALARM_CUSTOM_FILE_DIR, ALGO_DEBUG_FLAG_FILE);
    if (0 == access(fileName, F_OK))
    {
        Luma = 50;
        Contrast = 50;
        LoadAlgoParame(hChannelHandle);
    }
#endif
    m_ImageStyleParam.Luma = Luma;
    m_ImageStyleParam.Contrast = Contrast;
    m_ImageStyleParam.Hue = Hue;
    m_ImageStyleParam.Saturation = Saturation;
    m_ImageStyleParam.Sharpen = Sharpness;

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneImageStyle(nSenIndex, &m_ImageStyleParam);
        if (s32Ret < 0)
        {
            ZW_ERROR("Visp_Set_TuneImageStyle failed [%d] [%x]\n", nSenIndex, s32Ret);
            return -1;
        }
    }

    ZW_DEBUG("Visp_Set_TuneImageStyle SUCCESS\n");
#endif
    return s32Ret;
}

/**
 * @description: 设置白平衡参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} enable 是否使能白平衡(除了自动白平衡为0,其他全为1)
 * @param {int} wbMode 白平衡模式 0:白炽灯,1:暖光灯,2:日光灯,3:自然光,4:手动白平衡,5:自动白平衡,6:锁定白平衡
 * @param {int} redGain 红增益  0-100
 * @param {int} blueGain 蓝增益  0-100
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetWhiteBalance(void *hChannelHandle, int enable, int wbMode, int redGain, int blueGain)
{
    SINT32 s32Ret = 0;
    ISP_TUNE_AWB_PARAM_S m_AwbParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    if (enable)
    {
        m_AwbParam.AwbMode = ISP_TUNE_WB_MANUAL;
        switch (wbMode)
        {
            // Rgain:0x113-275 Bgain:0x352-850
            case WB_MODE_INCANDS:
                m_AwbParam.AwbCustom.Rgain = 275;
                m_AwbParam.AwbCustom.Bgain = 850;
                break;
            // Rgain:0x145-325 Bgain:0x2D5-725
            case WB_MODE_WARM:
                m_AwbParam.AwbCustom.Rgain = 325;
                m_AwbParam.AwbCustom.Bgain = 725;
                break;
            // Rgain:0x100-256 Bgain:0x100-256
            // Rgain:0x190-400 Bgain:0x258-600
            case WB_MODE_FLUORES:
                m_AwbParam.AwbCustom.Rgain = 400;
                m_AwbParam.AwbCustom.Bgain = 600;
                break;
            // Rgain:0x1D6-470 Bgain:0x1DB-475
            case WB_MODE_NATURAL:
                m_AwbParam.AwbCustom.Rgain = 470;
                m_AwbParam.AwbCustom.Bgain = 475;
                break;
            case WB_MODE_MANUAL:
                m_AwbParam.AwbCustom.Rgain = redGain * 10;
                m_AwbParam.AwbCustom.Bgain = blueGain * 10;
                break;
            case WB_MODE_LOCK:
                m_AwbParam.AwbMode = ISP_TUNE_WB_AUTO;
                break;
            default:
                ZW_ERROR("GEt WB MODE failed\n");
                break;
        }
    }
    else
    {
        m_AwbParam.AwbMode = ISP_TUNE_WB_AUTO;
    }
    ZW_INFO("mode = %d | Rgain = %d | Bgain = %d \n",m_AwbParam.AwbMode, m_AwbParam.AwbCustom.Rgain, m_AwbParam.AwbCustom.Bgain);

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneAutoWhiteBalance(nSenIndex, &m_AwbParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneAutoWhiteBalance fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("WHITE BALANCE SET SUCCESS!\n");

    return 0;
}

/**
 * @description: 设置降噪参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} MCTFStrength 降噪强度0:自动,1:弱,2:一般,3:强
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetMCTFStrength(void *hChannelHandle, int MCTFStrength)
{
    SINT32 s32Ret = 0;
#if (USE_VISP)
    ISP_TUNE_NR_PARAM_S m_NrParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    m_NrParam.Nr2dEnable = ISP_TUNE_TRUE;

    switch (MCTFStrength)
    {
        case 0:
            m_NrParam.Nr2dLevel = 50;
            break;
        case 1:
            m_NrParam.Nr2dLevel = 25;
            break;
        case 2:
            m_NrParam.Nr2dLevel = 75;
            break;
        case 3:
            m_NrParam.Nr2dLevel = 100;
            break;
        default:
            ZW_ERROR("Get MCTFStrength failed\n");
            break;
    }

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneNoiseReduction(nSenIndex, &m_NrParam);
        if(0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneNoiseReduction fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("Tune Noise Set SUCCESS!\n");
#endif
    return s32Ret;
}

/**
 * @description: 设置背光补偿使能
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} enable 使能 0:关,1:开
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_EnableBacklightMode(void *hChannelHandle, int enable)
{
    SINT32 s32Ret = 0;
#if (USE_VISP)
    ISP_TUNE_EXT_AE_PARAM_S m_ExtAeParam = {0};
    CChannelContext *pChCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChCtx);

    pChCtx->m_BacklightEnable = enable;

    if (enable)
    {
        m_ExtAeParam.Level = 50;
    }
    else
    {
        m_ExtAeParam.Level = 0;
    }

    m_ExtAeParam.ExtAeMode = ISP_TUNE_EXT_AE_BLC;

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneExtendAutoExposure(nSenIndex, &m_ExtAeParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneExtendAutoExposure fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("AE_BLC Set SUCCESS!\n");
#endif
    return s32Ret;
}

/**
 * @description: 设置 ISP 数字宽动态参数
 * @param {int} IspWdrMode 数字宽动态强度0:关闭,1:弱,2:一般,3:强,4:自动
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetDigitalWDRMode(int IspWdrMode)
{
    SINT32 s32Ret = 0;
#if (USE_VISP)
    ISP_TUNE_EXT_AE_PARAM_S m_ExtAeParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    switch (IspWdrMode)
    {
        case 0:
            m_ExtAeParam.Level = 0;
            break;
        case 1:
            m_ExtAeParam.Level = 25;
            break;
        case 2:
            m_ExtAeParam.Level = 50;
            break;
        case 3:
            m_ExtAeParam.Level = 100;
            break;
        case 4:
            m_ExtAeParam.Level = 50;
            break;
        default:
            ZW_ERROR("Get IspWdrMode failed\n");
            break;
    }

    m_ExtAeParam.ExtAeMode = ISP_TUNE_EXT_AE_HDR;

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneExtendAutoExposure(nSenIndex, &m_ExtAeParam);
        if(0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneExtendAutoExposure fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("Set Digital WDR Param Success! level = %d\n", m_ExtAeParam.Level);
#endif
    return s32Ret;
}

/**
 * @description: 设置 sensor 宽动态模式
 * @param {int} SensorWdrMode Sensor 宽动态强度 0: SDR, 1:WDR
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetSensorWDRMode(int SensorWdrMode)
{
    SINT32 s32Ret = 0;
    VISP_IMAGE_PARAM_S m_ImageParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    //s32Ret = Visp_Get_ImageParam(0, &m_ImageParam);
    if(0 != s32Ret)
    {
        ZW_ERROR("Visp_Get_ImageParam fail return 0x%x!\n", s32Ret);
        return -1;
    }
    ZW_DEBUG("Get WDR Level = %d Mode = %d\n", m_ImageParam.WdrLevel, SensorWdrMode);
    /* KS: VISP_WDR_LEVEL_1: SDR, VISP_WDR_LEVEL_2: WDR, VISP_WDR_LEVEL_3: WDR+SDR not support */
    switch (SensorWdrMode)
    {
        case 0:
            m_ImageParam.WdrLevel = VISP_WDR_LEVEL_1;
            break;
        case 1:
            m_ImageParam.WdrLevel = VISP_WDR_LEVEL_2;
            break;
        default:
            ZW_ERROR("Set Sensor WDR Failed [level:%d]\n", SensorWdrMode);
            break;
    }

    ZW_DEBUG("Set WDR Level = %d\n", m_ImageParam.WdrLevel);

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_ImageParam(nSenIndex, &m_ImageParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_ImageParam fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("Set Sensor WDR Param Success level = %d!\n", m_ImageParam.WdrLevel);
    return s32Ret;
}

/**
 * @description: 设置宽动态模式 (数字宽动态 + sensor 宽动态)
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} ExpMode 宽动态强度0:关闭,1:弱,2:一般,3:强,4:自动
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetLocalExpMode(void *hChannelHandle, int ExpMode)
{
    SINT32 s32Ret = 0;

    s32Ret = Hi_MPP_SetDigitalWDRMode(ExpMode);
    if (0 != s32Ret)
    {
        ZW_ERROR("Hi_MPP_SetDigitalWDRMode fail return 0x%x!\n", s32Ret);
        return -1;
    }

    ZW_DEBUG("Hi_MPP_SetLocalExpMode SUCCESS (%d)\n", ExpMode);

    return s32Ret;
}

/**
********************************************************************************
 * @函数功能: Visp_Get_SceneFps接口会返回AE的最大曝光，需要应用来更新
 * @作者    : HL.LIU
 * @param[in] *hChannelHandle
 * @param[in] maxShutterTime
 * @返回值  :
 * @其他    :
********************************************************************************
*/
SINT32 Hi_MPP_UpdateAEMaxExposureTime(void *hChannelHandle, FLOAT32 maxShutterTime)
{
    SINT32 s32Ret = 0;
    CPlatformChannelContext  *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
    if ((maxShutterTime > 0.01) && ((maxShutterTime - pPlatCtx->m_AeParam.MaxExposureTime > 0.01) || (maxShutterTime - pPlatCtx->m_AeParam.MaxExposureTime < -0.01)))
    {
        pPlatCtx->m_AeParam.MaxExposureTime = maxShutterTime;

        for (int i = 0; i<SYSTEM_SUPPORT_CHANNEL; i++)
        {
            s32Ret =  Visp_Set_TuneAutoExposure(i, &pPlatCtx->m_AeParam);
        }

        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneAutoExposure fail return 0x%x!\n", s32Ret);
            return -1;
        }

        ZW_DEBUG("Hi_MPP_UpdateAEMaxExposureTime Param to %f SUCCESS\n", maxShutterTime);
    }

    return s32Ret;
}

/**
 * @description: 设置曝光参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {SINT32} antiFlicker 防闪烁模式(0:50Hz, 1:60Hz, 2:关闭)
 * @param {UINT32} minShutterTime 曝光最低时间(无效参数)
 * @param {UINT32} maxShutterTime 曝光时间:25~10000
 * @param {UINT32} sensorGain 增益:0~42
 * @param {UINT32} ae_mode 曝光模式(0:自动,1:手动)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetAEParam(void *hChannelHandle, SINT32 antiFlicker, UINT32 minShutterTime, UINT32 maxShutterTime, UINT32 sensorGain, SINT32 ae_mode)
{
    SINT32 s32Ret = 0;
    int nCh = 0;
    ISP_TUNE_AE_PARAM_S AeParam = {0};
    CChannelContext     *pChannelContext = NULL;
    CPlatformChannelContext  *pPlatCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    HY_Enc_GetEncChInfo(&pChannelContext);

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    switch (antiFlicker)
    {
        case CMOS_FREQUENCY_50HZ:
            AeParam.FlickerMode = ISP_TUNE_FLICKER_50HZ;
            break;
        case CMOS_FREQUENCY_60HZ:
            AeParam.FlickerMode = ISP_TUNE_FLICKER_60HZ;
            break;
        case CMOS_FREQUENCY_CLOSE:
            AeParam.FlickerMode = ISP_TUNE_FLICKER_OFF;
            break;
        default:
            ZW_ERROR("Get antiFlicker failed\n");
            return -1;
    }

    switch (pChannelContext->m_antiFlickerStrength)
    {
        //关闭
        case 0:
            AeParam.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_OFF;
            break;
        //弱
        case 1:
        case 50:
            AeParam.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_LOW;
            break;
        //强
        case 2:
        case 100:
            AeParam.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_HIGH;
            break;
        default:
            ZW_ERROR("FlickerMode error\n");
            return -1;
    }

    if (AE_MODE_AUTO == ae_mode)
    {
        /*备注:自动模式时, 曝光时间和曝光增益不生效, 亮度值会生效*/
        AeParam.AeMode = ISP_TUNE_AE_AUTO;
        AeParam.MinExposureTime = 0;
        if(0 == pPlatCtx->m_AeParam.MaxExposureTime)
        {
            AeParam.MaxExposureTime = 129;
        }
        else
        {
            AeParam.MaxExposureTime = pPlatCtx->m_AeParam.MaxExposureTime;
        }

    }
    else if (AE_MODE_MANUAL == ae_mode)
    {
        AeParam.ExposureTime = ((FLOAT32) 1 / maxShutterTime * 100);
        ZW_INFO("AeParam.ExposureTime = %f --- sensorGain = %d\n", AeParam.ExposureTime, sensorGain);
        switch(sensorGain)
        {
            case 6:
                AeParam.ExposureGain = 1;
                break;
            case 12:
                AeParam.ExposureGain = 2;
                break;
            case 18:
                AeParam.ExposureGain = 4;
                break;
            case 24:
                AeParam.ExposureGain = 8;
                break;
            case 30:
                AeParam.ExposureGain = 16;
                break;
            case 36:
                AeParam.ExposureGain = 32;
                break;
            case 42:
                AeParam.ExposureGain = 64;
                break;
            default:
                AeParam.ExposureGain = 100;
                break;
        }
        AeParam.AeMode = ISP_TUNE_AE_MANUAL;
    }
    AeParam.ExposureSpeed = 50;
    AeParam.ExposureLuma = 50;
    AeParam.ExposureRect.StartX = 0;
    AeParam.ExposureRect.StartY = 0;
    AeParam.ExposureRect.Width  = 10000;
    AeParam.ExposureRect.Height = 10000;

    ZW_DEBUG("AE ExposureTime = %f ExposureTimeRage[%f, %f] Gain = [%d] %d\n", AeParam.ExposureTime, AeParam.MinExposureTime, AeParam.MaxExposureTime, sensorGain, AeParam.ExposureGain);
    for (nCh = 0; nCh < MaxSns; nCh++)
    {
        s32Ret = Visp_Set_TuneAutoExposure(nCh, &AeParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneAutoExposure fail return 0x%x! [%d]\n", s32Ret, nCh);
            return -1;
        }
    }
    memcpy(&pPlatCtx->m_AeParam, &AeParam, sizeof(ISP_TUNE_AE_PARAM_S));

    gCurAnTiStatus = AeParam.FlickerMode;

    ZW_DEBUG("Set AE Param SUCCESS\n");

    return s32Ret;
}

/**
 * @brief  获取AE参数
 * @param  hChannelHandle 编码通道句柄
 * @param  pstAeParam AE参数结构体
 * @return SINT32 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetAEParam(void *hChannelHandle, ISP_TUNE_AE_PARAM_S *pstAeParam)
{
    CPlatformChannelContext  *pPlatCtx = NULL;

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;
#if 0
    switch (pChCtx->AEParam.antiFlicker)
    {
        case CMOS_FREQUENCY_50HZ:
            pstAeParam->FlickerMode = ISP_TUNE_FLICKER_50HZ;
            break;
        case CMOS_FREQUENCY_60HZ:
            pstAeParam->FlickerMode = ISP_TUNE_FLICKER_60HZ;
            break;
        case CMOS_FREQUENCY_CLOSE:
            pstAeParam->FlickerMode = ISP_TUNE_FLICKER_OFF;
            break;
        default:
            ZW_ERROR("Get antiFlicker failed\n");
    }
    switch (pChCtx->m_antiFlickerStrength)
    {
        case 0:
            pstAeParam->FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_OFF;
            break;
        case 1:
        case 50:
            pstAeParam->FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_LOW;
            break;
        case 2:
        case 100:
            pstAeParam->FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_HIGH;
            break;
        default:
            ZW_ERROR("FlickerMode error\n");
            return -1;
    }
    if (AE_MODE_AUTO == pChCtx->m_ae_mode)
    {
        /*备注:自动模式时, 曝光时间和曝光增益不生效, 亮度值会生效*/
        pstAeParam->AeMode = ISP_TUNE_AE_AUTO;
    }
    else if (AE_MODE_MANUAL == pChCtx->m_ae_mode)
    {
        pstAeParam->ExposureTime = ((FLOAT32) 1 / pChCtx->AEParam.MaxShutterTime * 100);

        switch(pChCtx->AEParam.sensorGain)
        {
            case 6:
                pstAeParam->ExposureGain = 1;
                break;
            case 12:
                pstAeParam->ExposureGain = 2;
                break;
            case 18:
                pstAeParam->ExposureGain = 4;
                break;
            case 24:
                pstAeParam->ExposureGain = 8;
                break;
            case 30:
                pstAeParam->ExposureGain = 16;
                break;
            case 36:
                pstAeParam->ExposureGain = 32;
                break;
            case 42:
                pstAeParam->ExposureGain = 64;
                break;
            default:
                pstAeParam->ExposureGain = 100;
                break;
        }
        pstAeParam->AeMode = ISP_TUNE_AE_MANUAL;
    }

    pstAeParam->ExposureSpeed = 50;
    pstAeParam->ExposureLuma = 50;
    pstAeParam->ExposureRect.StartX = 0;
    pstAeParam->ExposureRect.StartY = 0;
    pstAeParam->ExposureRect.Width  = 10000;
    pstAeParam->ExposureRect.Height = 10000;
#else
    memcpy(pstAeParam, &pPlatCtx->m_AeParam, sizeof(ISP_TUNE_AE_PARAM_S));
#endif
    return 0;
}

/**
 * @description: 设置本地图像模式
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} mode 0:标准模式,1:人脸无曝光模式,2:车牌模式
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetLocalImageMode(void *hChannelHandle, int mode)
{
#if (USE_VISP)
    SINT32 s32Ret = 0;
    ISP_TUNE_EXT_AE_PARAM_S m_ExtAeParam = {0};

    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    HY_Enc_GetEncChInfo(&pChCtx);

    pPlatCtx->m_ImageMode = mode;

    if (pChCtx->m_BacklightEnable)
    {
        m_ExtAeParam.ExtAeMode = ISP_TUNE_EXT_AE_BLC;
        m_ExtAeParam.Level = 50;
    }
    else
    {
        if (0 == pPlatCtx->m_ImageMode) /*标准模式*/
        {
            m_ExtAeParam.ExtAeMode = ISP_TUNE_EXT_AE_OFF;
        }
        else if (1 == pPlatCtx->m_ImageMode) /*人脸无曝光模式*/
        {
            m_ExtAeParam.ExtAeMode = ISP_TUNE_EXT_AE_HLC;
            m_ExtAeParam.Level = 50;
        }
    }

    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        s32Ret = Visp_Set_TuneExtendAutoExposure(nSenIndex, &m_ExtAeParam);
        if (0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneExtendAutoExposure fail return 0x%x! [%d]\n", s32Ret, nSenIndex);
            return -1;
        }
    }

    ZW_DEBUG("HLC Set SUCCESS!\n");
#endif
    return 0;
}

/**
 * @description: 设置镜像翻转
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} flipRotate 翻转角度 0:正常, 1:中心, 2:左右,3:上下
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetFlipRotateMode(void *hChannelHandle, int flipRotate)
{
    SINT32 s32Ret = 0;
    XRS_HVFLIP_E eFlipRotateMode_ch0 = XRS_FLIP_NORMAL_MODE;
    XRS_HVFLIP_E eFlipRotateMode_ch1 = XRS_FLIP_NORMAL_MODE;
    XRS_HVFLIP_E eFlipRotateMode = XRS_FLIP_NORMAL_MODE;
    unsigned int LensOrientation = 0;
    SINT32 nChn = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    LensOrientation = pSysInfo->m_LensOrientation;

    switch (flipRotate)
    {
        case 0:
            eFlipRotateMode_ch0 = XRS_FLIP_NORMAL_MODE;
            eFlipRotateMode_ch1 = XRS_FLIP_HV_MODE;
            break;
        case 1:
            eFlipRotateMode_ch0 = XRS_FLIP_HV_MODE;
            eFlipRotateMode_ch1 = XRS_FLIP_NORMAL_MODE;
            break;
        case 2:
            eFlipRotateMode_ch0 = XRS_FLIP_H_MODE;
            eFlipRotateMode_ch1 = XRS_FLIP_V_MODE;
            break;
        case 3:
            eFlipRotateMode_ch0 = XRS_FLIP_V_MODE;
            eFlipRotateMode_ch1 = XRS_FLIP_H_MODE;
            break;
        default :
            ZW_ERROR("not support flipRotate = %d !\n", flipRotate);
            return -1;
    }

    // 球机 控制翻转
    if((LensOrientation >> 0) & 1)
    {
        switch (eFlipRotateMode_ch0)
        {
            case XRS_FLIP_NORMAL_MODE:
                eFlipRotateMode_ch0 = XRS_FLIP_HV_MODE;
                break;
            case XRS_FLIP_HV_MODE:
                eFlipRotateMode_ch0 = XRS_FLIP_NORMAL_MODE;
                break;
            case XRS_FLIP_H_MODE:
                eFlipRotateMode_ch0 = XRS_FLIP_V_MODE;
                break;
            case XRS_FLIP_V_MODE:
                eFlipRotateMode_ch0 = XRS_FLIP_H_MODE;
                break;
            default:
                break;
        }
    }

    // 枪机 控制翻转
    if((LensOrientation >> 1) & 1)
    {
        switch (eFlipRotateMode_ch1)
        {
            case XRS_FLIP_NORMAL_MODE:
                (eFlipRotateMode_ch1) = XRS_FLIP_HV_MODE;
                break;
            case XRS_FLIP_HV_MODE:
                (eFlipRotateMode_ch1) = XRS_FLIP_NORMAL_MODE;
                break;
            case XRS_FLIP_H_MODE:
                (eFlipRotateMode_ch1) = XRS_FLIP_V_MODE;
                break;
            case XRS_FLIP_V_MODE:
                (eFlipRotateMode_ch1) = XRS_FLIP_H_MODE;
                break;
            default:
                break;
        }
    }
    for (nChn = 0; nChn < MaxSns; nChn++)
    {
        if (FIRST_SENSOR == nChn)
        {
            eFlipRotateMode = eFlipRotateMode_ch0;
        }
        else
        {
            /* 副板无需单独定制 */
            eFlipRotateMode = eFlipRotateMode_ch1;
        }
        s32Ret = Visp_Set_TuneHVFLIP(nChn, eFlipRotateMode);
        if (s32Ret < 0)
        {
            ZW_ERROR("Visp_Set_TuneHVFLIP failed nChn[%d]\n", nChn);
            return -1;
        }
    }

    return s32Ret;
}

/**
 * @description: 画面切换到黑白或者彩色
 * @param {int} DayOrNight 0:晚上 1:白天(黑白效果或者彩色效果)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SwitchScreenColor(int DayOrNight)
{
    SINT32 RetVal = 0;
    ISP_TUNE_COLOR_BLACK_PARAM_S m_ColorBlackParam = {0};
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (ISNIGHT != DayOrNight && ISDAY != DayOrNight)
    {
        ZW_ERROR("Get DayOrNight Failed %d\n", DayOrNight);
        return -1;
    }

    m_ColorBlackParam.ColorBlackMode = DayOrNight ? ISP_TUNE_DN_COLOR:ISP_TUNE_DN_WHITE_BLACK;
    m_ColorBlackParam.DelayTimeLevel = 50;
    m_ColorBlackParam.SensitivityLevel = 50;
    for (int i = 0; i < MaxSns; i++)
    {
        RetVal = Visp_Set_TuneColorBlackTransform(i, &m_ColorBlackParam);
        if (RetVal < 0)
        {
            ZW_ERROR("Visp_Set_TuneColorBlackTransform Failed [%d] [%d]\n", RetVal, i);
            return -1;
        }
    }

    ZW_DEBUG("Visp_Set_TuneColorBlackTransform SUCCESS [%s]\n", DayOrNight? "DAY":"NIGHT");
    return 0;
}

/**
 * @description: 通过实时亮度获取当前的日夜模式
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetCurrDayNightStatus(IR_ISP_INFO_S *LightIrInfo)
{
    SINT32 s32Ret = 0;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    ISP_TUNE_ALG_INFOR_S m_IspAlgParam = {0};
    ISP_TUNE_AWB_STATIS_INFO_S m_IspAWBParam = {0};

    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = pChCtx->pPlatformChannelhandle;

    XTRACE_MUTEX_LOCK(&pChCtx->m_CheckLumLock);
    s32Ret = Visp_Get_TuneCurrInfo(0, &m_IspAlgParam);
    if (s32Ret < 0)
    {
        ZW_ERROR("Visp_Get_TuneCurrInfo Failed\n");
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_CheckLumLock);
        return -1;
    }
    s32Ret = Visp_Get_AWBStatisInfo(0, &m_IspAWBParam);
    if (s32Ret < 0)
    {
        ZW_ERROR("Visp_Get_AWBStatisInfo Failed\n");
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_CheckLumLock);
        return -1;
    }
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_CheckLumLock);

    LightIrInfo->gain = m_IspAlgParam.AGain * ((float)m_IspAlgParam.Exposure / 10 / 1024) * ((float)m_IspAlgParam.ISPGain / 1024);
    LightIrInfo->luma = m_IspAlgParam.IcrParam.IcrAlgLight;
    LightIrInfo->ev = m_IspAlgParam.Exposure;
    LightIrInfo->rggain = m_IspAWBParam.RGain;
    LightIrInfo->bggain = m_IspAWBParam.BGain;

    //OSD叠加信息
    g_rggain = m_IspAWBParam.RGain;
    g_bggain = m_IspAWBParam.BGain;
    g_Exposure = m_IspAlgParam.Exposure;
    g_gain = LightIrInfo->gain;

    //DGain 有可能为0
    pPlatCtx->m_stStatistics.gain = m_IspAlgParam.AGain * ((float)m_IspAlgParam.Exposure / 1024) * ((float)m_IspAlgParam.ISPGain / 1024);
    pPlatCtx->m_stStatistics.luma = m_IspAlgParam.IcrParam.IcrAlgLight;

    ZW_DEBUG("Exposure:[%u] RGain:[%u] BGain[%u]\n", m_IspAlgParam.Exposure, m_IspAWBParam.RGain, m_IspAWBParam.BGain);
    ZW_DEBUG("gain:[%u] AGain:[%u] DGain[%u] ISPGain:[%u]\n", LightIrInfo->gain, m_IspAlgParam.AGain, m_IspAlgParam.DGain, m_IspAlgParam.ISPGain);
    return 0;
}

/*************************************硬件相关接口*******************************************/
/**
 * @description: 初始化KS HalMod模块
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_HalModInit()
{
    SINT32 RtVal = 0;

    if ((RtVal = HalMod_Open(NULL)) < 0)
    {
        if (0x803c0002 == RtVal)
        {
            g_bHalModinited = 1;
            return 0;
        }

        ZW_ERROR("HalMod_Open failed with %#x\n", RtVal);
        return -1;
    }

    if ((RtVal = HalMod_Start(0, 0)) < 0)
    {
        ZW_ERROR("HalMod_Start failed with %#x\n", RtVal);
        HalMod_Close(0);
        return -1;
    }

    g_bHalModinited = TRUE;

    return 0;
}

/**
 * @description: 反初始化KS HalMod
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_HalModDeInit()
{
    SINT32 RtVal = 0;

    if (!g_bHalModinited)
    {
        ZW_ERROR("Halmod NOT inited!\n");
        return -1;
    }

    if ((RtVal = HalMod_Close(0)) < 0)
    {
        ZW_ERROR("HalMod_Close failed with %#x\n", RtVal);
        return -1;
    }

    g_bHalModinited = FALSE;

    return 0;
}

/**
 * @description: 获取复位按键状态
 * @param {UINT8} u8KeySt
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetRestKeyStatus(KEY_STATUS_E* eKeySt)
{
    SINT32 RtVal = 0;
    HALMOD_DEF_KEY_ATTR_S stDefKey = {0};

    if (!g_bHalModinited)
    {
        ZW_ERROR("Halmod NOT inited!\n");
        return -1;
    }

    if ((RtVal = HalMod_Get_DefKeyAttr(0, &stDefKey)) < 0)
    {
        ZW_ERROR("HalMod_Close failed with %#x\n", RtVal);
        return -1;
    }

    // 当按键按下时，Stat为0，松开时为1
    //*eKeySt = stDefKey.Stat? KEY_RELE: KEY_PUSH;
    *eKeySt = stDefKey.Stat? KEY_PUSH: KEY_RELE;

    return 0;
}

/**
 * @description: 设置IRCUT模式，会直接切换IR-CUT
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {IRCUT_MODE_E} eMode IRCUT模式
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetIRCutMode(IRCUT_MODE_E eMode)
{
    SINT32 RtVal = 0;
    HALMOD_IRCUT_MODE_E eIRCutMode = HALMOD_IRCUT_MODE_NIGHT;

    if (!g_bHalModinited)
    {
        ZW_ERROR("Halmod NOT inited!\n");
        return -1;
    }

    eIRCutMode = (IRCUT_MODE_NIGHT == eMode) ? HALMOD_IRCUT_MODE_NIGHT : HALMOD_IRCUT_MODE_DAY;
#if defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
	RtVal  = HalMod_Set_IrCut_Mode(1, eIRCutMode);
	RtVal |= HalMod_Set_IrCut_Mode(2, eIRCutMode);
	RtVal  = HalMod_Set_IrCut_Mode(3, eIRCutMode);
#else
#if (SYSTEM_SUPPORT_CHANNEL >=1)
    RtVal  = HalMod_Set_IrCut_Mode(0, eIRCutMode);
#endif

#if (SYSTEM_SUPPORT_CHANNEL >=2)
    RtVal |= HalMod_Set_IrCut_Mode(1, eIRCutMode);
#endif

#if (SYSTEM_SUPPORT_CHANNEL >=3)
    RtVal  = HalMod_Set_IrCut_Mode(2, eIRCutMode);
#endif

#if (SYSTEM_SUPPORT_CHANNEL >=4)
    RtVal |= HalMod_Set_IrCut_Mode(3, eIRCutMode);
#endif
#endif
    if (RtVal < 0)
    {
        ZW_ERROR("HalMod_Set_IrCut_Mode failed with %#x\n", RtVal);
        return -1;
    }

    return 0;
}

// 灯光控制脚
/**
 * @description: 设置白光补光灯
 * @param {UINT32} u32Duty 占空比，0-关闭，100-最亮
 * @param {UINT32} u32FrqKHZ PWM频率
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetWhiteLight(UINT32 u32Duty, UINT32 u32FrqKHZ)
{
    SINT32 RtVal = 0;
    HALMOD_LIGHT_ATTR_S stLightAttr = {0};

    stLightAttr.Type = LIGHT_TYPE_WHITE;
    stLightAttr.Chn = 0;
    stLightAttr.FrqKHZ = u32FrqKHZ;
    stLightAttr.Data = u32Duty;
    stLightAttr.Action = u32Duty ? HALMOD_LIGHT_ACTION_ON : HALMOD_LIGHT_ACTION_OFF;

    if ((RtVal = HalMod_Set_LightAttr(0, stLightAttr)) < 0)
    {
        ZW_ERROR("HalMod_Set_LightAttr failed with %#x\n", RtVal);
        return -1;
    }

    return 0;
}

/**
 * @description: 设置红光补光灯
 * @param {UINT32} u32Duty 占空比，0-关闭，100-最亮
 * @param {UINT32} u32FrqKHZ PWM频率
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetRedLight(UINT32 u32Duty, UINT32 u32FrqKHZ)
{
    SINT32 RtVal = 0;
    SystemInfo_t *pSysInfo = NULL;
    HALMOD_LIGHT_ATTR_S stLightAttr = {0};

    HY_Res_GetSystemInfo(&pSysInfo);

    //当是无极红单灯板时，此时控"白光pwm"引脚（同以前所有产品的接线）
    if (NO_LDR_STEPLESS_INFRARED_BOARD == pSysInfo->m_LightBoardTypeID)
    {
        stLightAttr.Type = LIGHT_TYPE_WHITE;
    }
    else
    {
        stLightAttr.Type = LIGHT_TYPE_IR;
    }
    stLightAttr.Chn = 0;
    stLightAttr.FrqKHZ = u32FrqKHZ;
    stLightAttr.Data = u32Duty;
    stLightAttr.Action = u32Duty ? HALMOD_LIGHT_ACTION_ON : HALMOD_LIGHT_ACTION_OFF;

    if ((RtVal = HalMod_Set_LightAttr(0, stLightAttr)) < 0)
    {
        ZW_ERROR("HalMod_Set_LightAttr failed with %#x\n", RtVal);
        return -1;
    }

    return 0;
}

// IR
/**
 * @description: 获取硬光敏电平状态
 * @param {SINT32} *pDayNightStatus 1:白天 0:夜晚
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetIRStateLevel(UINT32 *pDayNightStatus)
{
    SINT32 RtVal = 0;
    SINT32 StateLevel = -1;

    if ((RtVal = HalMod_Get_LSensorStat(0, &StateLevel)) < 0)
    {
        ZW_ERROR("HalMod_Get_LSensorStat failed with %#x\n", RtVal);
        return -1;
    }

    //gpio型
    if (ISDAY == StateLevel)
    {
        *pDayNightStatus = ISDAY;
    }
    else if (ISNIGHT == StateLevel)
    {
        *pDayNightStatus = ISNIGHT;
    }

    return 0;
}

/**
 * @description: 获取一键呼叫按键状态
 * @param {UINT8} u8KeySt
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetCallKeyStatus(KEY_STATUS_E* eCallKeySt, KEY_STATUS_E* eHangupKeySt)
{
    SINT32 RtVal = 0;

    if (!g_bHalModinited)
    {
        ZW_ERROR("Halmod NOT inited!\n");
        return -1;
    }

#if defined (__PRODUCT_P3MU__)
    HALMOD_KEYCALL_ATTR_S stCallKey = {0};

    if (HalMod_Get_KeyCallAttr(0, &stCallKey) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Get_KeyCallAttr failed!\n");
        return -1;
    }
    if (stCallKey.Type) /* ADC方式，取值电压值 */
    {
        if (stCallKey.Stat <= 7)
        {
            *eCallKeySt = KEY_PUSH;
            *eHangupKeySt = KEY_RELE;
        }
        else if (stCallKey.Stat <= 25)
        {
            *eCallKeySt = KEY_RELE;
            *eHangupKeySt = KEY_PUSH;
        }
        else
        {
            *eCallKeySt = KEY_RELE;
            *eHangupKeySt = KEY_RELE;
        }

        return 0;
    }

    /* GPIO方式，取值0或1 */
    *eCallKeySt = !stCallKey.Stat ? KEY_PUSH : KEY_RELE;

    if (HalMod_Get_KeyCallAttr(1, &stCallKey) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Get_KeyCallAttr failed!\n");
        return -1;
    }
    *eHangupKeySt = !stCallKey.Stat ? KEY_PUSH : KEY_RELE;

    //临时调试使用
    if (access("/tmp/keytest", F_OK) == 0) {
        *eHangupKeySt = *eCallKeySt;
    }
#elif defined (__PRODUCT_P4U__)||defined (__PRODUCT_P4__)
    HALMOD_KEYCALL_ATTR_S stCallKey = {0};

    if ((RtVal = HalMod_Get_KeyCallAttr(0, &stCallKey)) < 0)
    {
        ZW_ERROR("HalMod_Close failed with %#x\n", RtVal);
        return -1;
    }

    // 当按键按下时，Stat为0，松开时为1
    *eCallKeySt = stCallKey.Stat ? KEY_PUSH : KEY_RELE;
    *eHangupKeySt = *eCallKeySt;
#else
    HALMOD_DEF_KEY_ATTR_S stCallKey = {0};
    if ((RtVal = HalMod_Get_KeyCallAttr(0, &stCallKey)) < 0)
    {
        ZW_ERROR("HalMod_Close failed with %#x\n", RtVal);
        return -1;
    }

    // 当按键按下时，Stat为0，松开时为1
    *eCallKeySt = stCallKey.Stat ? KEY_PUSH : KEY_RELE;
    *eHangupKeySt = *eCallKeySt;
#endif

    return RtVal;
}

// 报警输出
/**
 * @description: 设置报警输出
 * @param {UINT32} u32Port 报警输出端口
 * @param {BOOL} bAlarm	是否报警
 * @return {SINT32} 0-成功 !=0 失败
 */
SINT32 Hi_MPP_SetAlarmOut(UINT32 u32Port, BOOL bAlarm)
{
    SINT32 RtVal = 0;
    HALMOD_ALARM_ATTR_S stAlarmAttr = {0};
    CChannelContext *pChnCtx  = NULL;

    HY_Enc_GetEncChInfo(&pChnCtx);

#ifdef __ONE_CLICK_CALL__
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    //报警输出和一件呼叫不会同时存在，一键呼叫如果打开则报警输出无效
    if (1 == pSysInfo->m_bSupportKeyToCall)
    {
        return 0;
    }
#endif

    stAlarmAttr.Chn = u32Port;
    stAlarmAttr.Type = HALMOD_ALARM_TYPE_OUT;
    if ((RtVal = HalMod_Get_AlarmAttr(0, &stAlarmAttr)) < 0)
    {
        ZW_ERROR("HalMod_Get_AlarmAttr failed with %#x\n", RtVal);
        return -1;
    }

    if (1 == pChnCtx->m_AlarmOutDefault)
    {
        stAlarmAttr.InType = HALMOD_ALARM_ALWAYS_OPEN;
        bAlarm = bAlarm ? 1 : 0;
    }
    else
    {
        stAlarmAttr.InType = HALMOD_ALARM_ALWAYS_CLOSE;
        bAlarm = bAlarm ? 0 : 1;
    }

    // 状态未改变
    if ((bAlarm && HALMOD_ALARM_STAT_HIGH == stAlarmAttr.Stat) // 报警输出使能
        || (!bAlarm && HALMOD_ALARM_STAT_LOW == stAlarmAttr.Stat)) // 报警输出失能
    {
        return 0;
    }

    stAlarmAttr.Chn = u32Port;
    stAlarmAttr.Stat = bAlarm;

    if ((RtVal = HalMod_Set_AlarmAttr(0, stAlarmAttr)) < 0)
    {
        ZW_ERROR("HalMod_Set_AlarmAttr failed with %#x\n", RtVal);
        return -1;
    }

    return 0;
}

// 报警输入
/**
 * @description: 获取报警输入状态
 * @param {BOOL} *bAlarm 输出报警输入状态，TRUE-报警，FLASE-正常
 * @return {SINT32} 0-成功，!0-失败
 */
SINT32 Hi_MPP_GetAlarmIn(BOOL *bAlarm)
{
    SINT32 RtVal = 0;
    HALMOD_ALARM_ATTR_S stAlarmAttr = {0};

    stAlarmAttr.Chn = 0;
    stAlarmAttr.Type = HALMOD_ALARM_TYPE_IN;
    if ((RtVal = HalMod_Get_AlarmAttr(0, &stAlarmAttr)) < 0)
    {
        ZW_ERROR("HalMod_Get_AlarmAttr failed with %#x\n", RtVal);
        return -1;
    }

    if (HALMOD_ALARM_STAT_LOW == stAlarmAttr.Stat)
    {
        *bAlarm = FALSE;
    }
    else
    {
        *bAlarm = TRUE;
    }

    return 0;
}

// 喇叭输出使能
/**
 * @description: 扩展板喇叭输出使能控制
 * @param {BOOL} bSpkEn TRUE-使能，FLASE-失能
 * @return {SINT32} 0-成功，!0-失败
 */
SINT32 Hi_MPP_SetSpeakerEn(BOOL bSpkEn)
{
    SINT32 RtVal = 0;

    if ((RtVal = HalMod_Set_SpeakerStat(0, bSpkEn?1:0)) < 0)
    {
        ZW_ERROR("HalMod_Set_SpeakerStat failed with %#x\n", RtVal);
        return -1;
    }

    return 0;
}

/**
 * @description: 获取设备得唯一CHIP ID
 * @param {SINT8} *buffer 缓冲区
 * @param {SINT32} len 缓冲区长度
 * @return {SINT32} 0-成功， 非0-失败
 */
SINT32 Hi_MPP_GetDevChipID(SINT8 *buffer, SINT32 len)
{
    SINT32 s32Ret = 0;
    HALMOD_DEVINFO_ATTR_S stDevInfo = {0};

    if (!buffer || len < HALMOD_CHIP_ID_DEV_LEN)
    {
        ZW_ERROR("Buffer Can not be NULL AND Len Must big then %d\n", HALMOD_CHIP_ID_DEV_LEN);
        return -1;
    }

    if (!g_bHalModinited)
    {
        ZW_ERROR("Halmod NOT inited!\n");
        return -1;
    }

    memset(buffer, 0, len);

    s32Ret = HalMod_Get_DevInfoAttr(0, &stDevInfo);
    if (s32Ret < 0)
    {
        ZW_ERROR("HalMod_Get_DevInfoAttr failed!\n");
        return -1;
    }

    ZW_INFO("ID: %s\n", stDevInfo.ChipId);

    // 拷贝并转换成大写
    for (int i = 0; i < HALMOD_CHIP_ID_DEV_LEN; i++)
    {
        if (stDevInfo.ChipId[i] >= 'a' && stDevInfo.ChipId[i] <= 'z')
        {
            buffer[i] = stDevInfo.ChipId[i] - 32;
        }
        else
        {
            buffer[i] = stDevInfo.ChipId[i];
        }
    }

    return 0;
}

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记 0: 正常, 1: 错误
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetSysStartTag(int Stat)
{
#if 0
    SINT32 s32Ret = 0;

    s32Ret = HalMod_Set_StartTag_Stat(0, Stat);
    if (s32Ret < 0)
    {
        ZW_ERROR("HalMod_Set_StartTag_Stat Failed\n");
        return -1;
    }

    ZW_DEBUG("HalMod_Set_StartTag_Stat SUCCESS\n");
#endif
    return 0;
}

static void Hi_MPP_QrScanTask(SystemInfo_t *pSysInfo, CChannelContext *pChContext)
{
    int ret = HI_SUCCESS;
    CPlatformChannelContext *pPlatformChannelContext = NULL;
    VDA_QR_FRAME_INFO_S qrFrm;
    VDA_QR_RESULT_INFO_S result;
    VIDEO_FRM_ALG_S s_VFrmAlg;

    memset(&qrFrm, 0, sizeof(qrFrm));
    memset(&result, 0, sizeof(result));
    memset(&s_VFrmAlg, 0, sizeof(s_VFrmAlg));

    pPlatformChannelContext = (CPlatformChannelContext *)pChContext->pPlatformChannelhandle;

    /* 1、获取图片并解析二维码内容 */
    XTRACE_MUTEX_LOCK(&pChContext->m_InitVPSSLock);
    ret = Vpss_Chn_Get_Frm(0, pPlatformChannelContext->m_VPSSGroupNum[FIRST_SENSOR],pPlatformChannelContext->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.SrcChn, &s_VFrmAlg, 1000);
    if (ret)
    {
        XTRACE_MUTEX_UNLOCK(&pChContext->m_InitVPSSLock);
        ZW_ERROR("Error(%d - %x):Vpss_Chn_Get_Frm!\n", ret, ret);
        return;
    }
    qrFrm.Width = s_VFrmAlg.u32Width;
    qrFrm.Height = s_VFrmAlg.u32Height;
    qrFrm.Size = s_VFrmAlg.u32Width * s_VFrmAlg.u32Height;
    qrFrm.pFrmBuf = (UINT8 *)(uintptr_t)(s_VFrmAlg.u64VirAddr[0]);

    VDA_DECODE_QR_INFO(&qrFrm, &result);

    Vpss_Chn_Rel_Frm(0, pPlatformChannelContext->m_VPSSGroupNum[FIRST_SENSOR], pPlatformChannelContext->m_stVpssChnAttr[FIRST_SENSOR][AI_STREAM].VpssParam.SrcChn, &s_VFrmAlg);
    XTRACE_MUTEX_UNLOCK(&pChContext->m_InitVPSSLock);

    ZW_DEBUG("[%s][%d][mj]=payload[%s],payloadLen[%d],state = %d ==  %p\n", __FUNCTION__, __LINE__, result.payload,result.payloadLen,result.state,pSysInfo->m_pQrScanCallback);

    /* 2、回调中间层处理 */
#if defined (__IS_SUPPORT_WLAN__)
    if ((strstr((char *)result.payload,"S=") != NULL) && (strstr((char *)result.payload,"&P=") != NULL) &&(strstr((char *)result.payload,"T=") != NULL) && (strstr((char *)result.payload,"A=") != NULL))
#elif defined (__MODULE_4G_SUPPORT__)
    if ((strstr((char *)result.payload,"V8=") != NULL) && (strstr((char *)result.payload,"&RN=") != NULL) && (strstr((char *)result.payload,"&U=") != NULL))
#endif
    {
        if (VDA_QR_STATE_DECODE_SUCCESS == result.state && result.payloadLen > 0)
        {
            if (NULL != pSysInfo->m_pQrScanCallback)
            {
                ZW_DEBUG("[%s][%d][mj]=payload[%s],payloadLen[%d]==\n",__FUNCTION__, __LINE__, result.payload, result.payloadLen);
                pSysInfo->m_pQrScanCallback(result.payload, result.payloadLen);
            }
        }
    }

    return;
}

void Hi_MPP_HandleHumanResultTask(SystemInfo_t *pSysInfo, CChannelContext *pChCtx)
{
    int MoveStatus = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    human_struct *pTargetResult = NULL;

    CChannelContext *pChContext = NULL;
    HY_Enc_GetEncChInfo(&pChContext);

    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    static int LastHumanNum[ALGO_CHANNEL] = {0};
    XTRACE_MUTEX_LOCK(&g_human_result_lock);
    static int human_statecount[ALGO_CHANNEL] = {0};
    int HumanNoneCount = 0;
    if (pPlatCtx->m_bHumanEnable == 0)
    {
        XTRACE_MUTEX_UNLOCK(&g_human_result_lock);
        return ;
    }

    for (int Chn = 0; Chn < ALGO_CHANNEL; Chn++)
    {
        /* 人形检测不动不检过滤 */
        if (GetFiltActionLessResult(Chn, g_HumanResult[Chn].rectInfo, g_HumanResult[Chn].num,pChContext->m_enPtzStatus) == 1)
        {
            MoveStatus = 1;
        }

        /* 此处逻辑在于修复开启跟踪后，人形目标处于跟踪中心位置，易被不动不检逻辑过滤掉，增加下面逻辑 */
        {
            //连续两帧未检测到人形则标记加一，作为清空灯光状态的标志位
            if ( (g_HumanResult[Chn].num <= 0) && LastHumanNum[Chn] <= 0)
            {
                HumanNoneCount++;
                LastHumanNum[Chn] = g_HumanResult[Chn].num;
                memset(&g_HumanResult[Chn], 0, sizeof(g_HumanResult[Chn]));
                continue;
            }

            if (MoveStatus == 1 || pChContext->m_enPtzStatus == ePtzStatus_Moving)
            {
                human_statecount[Chn] = 0;
            }
            LastHumanNum[Chn] = g_HumanResult[Chn].num;
            /* 不动不检过滤 */
            if (MoveStatus == 0 && g_HumanResult[Chn].num > 0 && human_statecount[Chn] < 600 && pPlatCtx->m_bHumanTrackEn != 0)
            {
                for(int i = 0 ; i < g_HumanResult[Chn].num; i++ )
                {
                    if(((g_HumanResult[Chn].rectInfo[i].s32X+(g_HumanResult[Chn].rectInfo[i].u32Width/2) - ALGO_STREAM_WIDTH/2) <= 50)
                        &&((g_HumanResult[Chn].rectInfo[i].s32Y+(g_HumanResult[Chn].rectInfo[i].u32Height/2) - ALGO_STREAM_HEIGHT/2) <= 100))
                    {
                        MoveStatus = 1;
                        human_statecount[Chn]++;
                        break;
                    }
                }
            }
        }

        if (MoveStatus == 1)
        {
            pChCtx->m_bHumanStateEx = TRUE;
            /* 检测完成时，需要把此变量置为TRUE */
            pChCtx->m_bHumanStateIsValue = TRUE;

            if (pSysInfo->m_pStreamReadCallback != NULL)
            {
                XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
                pChCtx->m_bHumanShapeState[Chn] = TRUE;
                pSysInfo->m_pStreamReadCallback(0);
                pChCtx->m_bHumanShapeState[Chn] = FALSE;
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);
            }
        }

        if (1 == pPlatCtx->m_bHumanTrackEn && Chn == 0)
        {
            int TrackIndex = -1;
            /* 人形跟踪结果获取 */
            TrackIndex = GetHumanTrackTargetResult(&g_HumanResult[Chn]);

            if (TrackIndex != -1)
            {
                OverlayRectangle stOverLayRect;
                stOverLayRect.m_Color = 0x00FF00;
                stOverLayRect.m_Left = (g_HumanResult[Chn].rectInfo[TrackIndex].s32X * 10000) / ALGO_STREAM_WIDTH;
                stOverLayRect.m_Top = (g_HumanResult[Chn].rectInfo[TrackIndex].s32Y *10000) / ALGO_STREAM_HEIGHT;
                stOverLayRect.m_Right = stOverLayRect.m_Left + (g_HumanResult[Chn].rectInfo[TrackIndex].u32Width * 10000) / ALGO_STREAM_WIDTH;
                stOverLayRect.m_Bottom = stOverLayRect.m_Top + (g_HumanResult[Chn].rectInfo[TrackIndex].u32Height * 10000) / ALGO_STREAM_HEIGHT;
				printf("[CYADD] humanRect x[%d] y[%d]\n",(stOverLayRect.m_Right+stOverLayRect.m_Left)/2,(stOverLayRect.m_Top+stOverLayRect.m_Bottom)/2);
				//只有不处于睡眠模式才会调跟踪回调接口
                if (pSysInfo->m_iDormancyState == 0 && 1 == pChCtx->m_HumanDetectSchedule)
                {
                    // 跟踪回调
                    if (NULL != pSysInfo->m_pHumanTrackerCallback)
                    {
                        pSysInfo->m_pHumanTrackerCallback((char *)&stOverLayRect, sizeof(stOverLayRect));
                    }
                }
            }
        }

        /* 人形跟踪开启时，不动不检过滤针对画框显示不生效 */
        if ((MoveStatus == 1) || ((MoveStatus == 0) && (g_HumanResult[Chn].num > 0) && (1 == pPlatCtx->m_bHumanTrackEn)))
        {
            XTRACE_MUTEX_LOCK(&pPlatCtx->m_humanResultsLock);
            pTargetResult = &pPlatCtx->m_humanResults[Chn];
            if (NULL != pTargetResult)
            {
                memcpy(pTargetResult, &g_HumanResult[Chn], sizeof(human_struct));
                pTargetResult->valid = 1;
            }
            XTRACE_MUTEX_UNLOCK(&pPlatCtx->m_humanResultsLock);
        }

        memset(&g_HumanResult[Chn], 0, sizeof(g_HumanResult[Chn]));
        MoveStatus = 0;
    }
    if (pChCtx->m_bHumanStateIsValue == FALSE && HumanNoneCount == ALGO_CHANNEL)	//灯光线程使用后置FALSE
    {
        pChCtx->m_bHumanStateEx = FALSE;
    }
    XTRACE_MUTEX_UNLOCK(&g_human_result_lock);
    return;
}

static void Hi_MPP_MotionDectectTask(SystemInfo_t *pSysInfo, CChannelContext *pChCtx)
{
#if (USE_VDA)
    int motion = 0;
    LIGHT_CTX_S* lightCtx = NULL;
    static int nMotionEnableTimer = 0;
    int Chn = 0;
    lightCtx = &pChCtx->m_stLightCtx;

    if (nMotionEnableTimer > 0)
    {
        nMotionEnableTimer--;
    }

    XTRACE_MUTEX_LOCK(&pChCtx->m_MDLock);
    if (pChCtx->m_bStartMD)
    {
        if (0 == Hi_MPP_MotionAnalyzer(pChCtx->pPlatformChannelhandle,
            pChCtx->MotionAreas, pChCtx->MotionGrade,
            pChCtx->m_MotionRectStride, pChCtx->pUsrBlock, &motion , &Chn))
        {
            /* 云台运动时不检测移动侦测, 大约8S后还原移动侦测检测 */
            if ((ePtzStatus_Moving == pChCtx->m_enPtzStatus) || (TRUE == lightCtx->lightAlarmStReal))
            {
                nMotionEnableTimer = 16;
            }

            if (nMotionEnableTimer > 0)
            {
                motion = 0;
            }

            pChCtx->m_bMotionStateIsValue = TRUE;

            if (motion)
            {
                pChCtx->m_bMotionState = TRUE;
                pChCtx->m_bIsMotion = TRUE;
                pChCtx->m_bMotionStateEx = TRUE;
                if ((NULL != pSysInfo->m_pStreamReadCallback) && (pChCtx->m_bMotionState))
                {
                    XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
                    pChCtx->bMotionStateOK[Chn] = TRUE;
                    pSysInfo->m_pStreamReadCallback(0);
                    pChCtx->bMotionStateOK[Chn] = FALSE;
                    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);
                    pChCtx->m_bMotionState = FALSE;
                }
            }
            else
            {
                pChCtx->m_bMotionState = FALSE;
                pChCtx->m_bIsMotion = FALSE;
                pChCtx->m_bMotionStateEx = FALSE;
            }
        }

        if (0 == motion)
        {
            pChCtx->m_bMotionStateEx = FALSE;
        }
    }
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);
#endif

    return;
}

#ifdef __IS_SUPPORT_JZ_CRY_DETECT__
/**
 * 获取所有振幅之平均值 计算db (振幅最大值 2^16-1 = 65535 最大值是 96.32db)
 * 目前我们的音频输入固定采用的16bit位宽采样
 * 16 bit == 2字节 == short int
 * 无符号16bit：96.32=20*lg(65535);
 *
 * @param pcmdata PCM数据（转换成char类型，才可以按字节操作）
 * @param size pcmdata的大小
 * @return
 * 注意：这里检测的分贝数会受设备音频输入音量(增益)影响
 */
static int getAudioDbFromPcm(signed short *pcmdata, size_t size)
{
    int i = 0;
    int db = 0;
    short int value = 0;
    double sum = 0;
    int Null_count = 0;
    for (i = 0; i < size; i += 2)
    {
        //获取2个字节的大小（值）
        //注意：这里可能要考虑大小端问题
        memcpy(&value, pcmdata+i, 2);
        //绝对值求和
        //注意：若一次给的数据过多，此值可能溢出
        //针对这种情况需要做分批处理
        if(value == 0)
           {
            Null_count++;
        }
        sum += abs(value);
    }
    //求平均值（2个字节表示一个振幅，所以振幅个数为：size/2个）
    sum = sum / (size / 2.0 - Null_count);
    if (sum > 0)
    {
        //计算此段数据平均分贝
        db = (int)(20.0 * log10(sum));
    }
    return db;
}

//声音侦测工作线程
void SoundDetectWorkTask(void)
{
    int DBValue = 0;
    int pcmSize = 0;
    CChannelContext *pChContext = NULL;
    SystemInfo_t *pSysInfo = NULL;
    AudioDetectCbData stAudioDetectCbData = {0};
    //声音侦测各灵敏度等级阈值（基于音频输入音量95）
    //注意：音频输入音量(增益)对采集到的声音分贝有直接影响，
    //目前没有处理，认为输入音量是固定的，若后续需要修改
    //这里的阈值需要结合输入音量调整
#if	defined(__PRODUCT_P4__)||defined(__PRODUCT_P4U__)||defined(__PRODUCT_P3MU__)
    const int arrAudioDbThresholdSet_Normal[MAX_AUDIO_DETECT_SENSTIVE_NUM] = {66, 74, 77};
#else
    const int arrAudioDbThresholdSet_Normal[MAX_AUDIO_DETECT_SENSTIVE_NUM] = {56, 66, 76};
#endif
    const int *arrAudioDbThresholdSet = arrAudioDbThresholdSet_Normal;
    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChContext);

    pthread_mutex_lock(&s_AudioDetectBufInf.m_muLock);
    //判断音频缓冲区中是否存在有效数据
    if (s_AudioDetectBufInf.bValid)
    {
        //G711U数据解码为PCM
        pcmSize = G711U_DECODE_PCM(s_AudioDetectBufInf.PcmBuf, s_AudioDetectBufInf.ULawBuf, AUDIO_FRM_SIZE);
        //获取此段音频平均分贝数
        DBValue = getAudioDbFromPcm(s_AudioDetectBufInf.PcmBuf, pcmSize);
        //printf("getAudioDbFromPcm: %d\n",DBValue);
        //判断分贝数是否超过当前报警阈值
        if ((DBValue > arrAudioDbThresholdSet[MAX_AUDIO_DETECT_SENSTIVE_NUM-pChContext->m_stAudioDetectCfg.nSenstive]))
              //&& (pChContext->m_nSpeechPlayStatus == eSpeechPlayIdle))
        {
            ZW_DEBUG("DB=%d(>%d)Audio Alarm !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",
                DBValue, arrAudioDbThresholdSet[MAX_AUDIO_DETECT_SENSTIVE_NUM-pChContext->m_stAudioDetectCfg.nSenstive]);

            //调用声音侦测报警回调
            stAudioDetectCbData.bTrigger = 1;
            pthread_mutex_lock(&pSysInfo->m_muAudioDetectCallbackLock);
            if (NULL != pSysInfo->m_pAudioDetectCallback)
            {
                pSysInfo->m_pAudioDetectCallback(&stAudioDetectCbData, pSysInfo->m_pAudioDetectCallbackData);
            }
            pthread_mutex_unlock(&pSysInfo->m_muAudioDetectCallbackLock);
        }
        else
        {
            stAudioDetectCbData.bTrigger = 0;
        }

        //标志此段数据已处理
        s_AudioDetectBufInf.bValid = 0;
    }
    pthread_mutex_unlock(&s_AudioDetectBufInf.m_muLock);
}
#endif

#if !defined (__PRODUCT_G43TA__) && !defined (__PRODUCT_G84F__) && !defined (__PRODUCT_W84F__) && !defined (__PRODUCT_G63F__) && !defined (__PRODUCT_W63F__)

static void Hi_MPP_EpAntiflickerDectectTask(SystemInfo_t *pSysInfo, CChannelContext *pChCtx)
{
    int s32Ret = 0;
    ISP_TUNE_FLICKER_MODE_E nAnTiStatus = gCurAnTiStatus;

    ISP_TUNE_AE_PARAM_S AeParam = {0};
    ISP_TUNE_ALG_INFOR_S m_IspAlgParam = {0};

    s32Ret = Visp_Get_TuneCurrInfo(0, &m_IspAlgParam);
    if (s32Ret < 0)
    {
        ZW_ERROR("Visp_Get_TuneCurrInfo Failed\n");
        return;
    }

    s32Ret = Hi_MPP_GetAEParam(pChCtx->pPlatformChannelhandle, &AeParam);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_GetAEParam failed\n");
        return;
    }

    /* 180000: 开抗闪 */
    if (m_IspAlgParam.Exposure < 180000)
    {
        nAnTiStatus = AeParam.FlickerMode;
    }
    /* 270000: 关抗闪 */
    else if (m_IspAlgParam.Exposure > 270000)
    {
        nAnTiStatus = ISP_TUNE_FLICKER_OFF;
    }

    if (nAnTiStatus != gCurAnTiStatus)
    {
        int nCh;
        int MaxSns = HY_Res_GetMaxSensorCnt();

        if (nAnTiStatus == ISP_TUNE_FLICKER_OFF)
        {
            AeParam.FlickerMode = ISP_TUNE_FLICKER_OFF;
        }

        for (nCh = 0; nCh < MaxSns; nCh++)
        {
            s32Ret = Visp_Set_TuneAutoExposure(nCh, &AeParam);
            if (0 != s32Ret)
            {
                ZW_ERROR("Visp_Set_TuneAutoExposure fail return 0x%x! [%d]\n", s32Ret, nCh);
                continue;
            }
        }

        gCurAnTiStatus = nAnTiStatus;
    }

    return ;
}

#endif

/* 二维码扫描、移动侦测 */
void *Hi_MPP_MiscTaskThread(void *lpParameter)
{
    PRT_TID_NAME("MiscTaskThread");

    UINT32 FrameCnt = 0;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChContext = NULL;
    ThreadInfo_t *pParent = (ThreadInfo_t *)lpParameter;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChContext);

    for (int i = 0 ; i < ALGO_CHANNEL ; i++) {
        pChContext->bMotionStateOK[i] = 0;
    }

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        /* Task1: 二维码扫描 */
        if (1 == pSysInfo->m_bIsQrScanEnable)
        {
            /* 二维码扫描控制在3帧左右, 实测此函数阻塞600ms */
            if ((FrameCnt % 2) == 0)
            {
                Hi_MPP_QrScanTask(pSysInfo, pChContext);
            }
        }
        else
        {
            /* Task2: 处理人形结果 */
            Hi_MPP_HandleHumanResultTask(pSysInfo, pChContext);
            if ((FrameCnt % 5) == 0)
            {
                /* Task3: 移动侦测,控制在2帧左右 */
                Hi_MPP_MotionDectectTask(pSysInfo, pChContext);

#if defined (__PRODUCT_G43TA__) || defined (__PRODUCT_G84F__)|| defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
                ImageCustom();
#else
                /* Task4: 根据EV值开关抗闪模式 */
                Hi_MPP_EpAntiflickerDectectTask(pSysInfo, pChContext);
#endif

            }
        }

#ifdef __IS_SUPPORT_JZ_CRY_DETECT__
        if ((1 == pChContext->m_stAudioDetectCfg.nEnable) && (eSpeechPlayIdle == pChContext->m_nSpeechPlayStatus))
        {
            SoundDetectWorkTask();
        }
#endif

        FrameCnt++;
        usleep(40 * 1000);
    }
    return NULL;
}

#ifdef __IS_SUPPORT_HS_CRY_DETECT__
static int _bcd_event_callback(void *ctx)
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    AudioDetectCbData stAudioDetectCbData = {0};

    stAudioDetectCbData.bTrigger = 1;
    XTRACE_MUTEX_LOCK(&pSysInfo->m_muAudioDetectCallbackLock);
    if (NULL != pSysInfo->m_pAudioDetectCallback)
    {
        pSysInfo->m_pAudioDetectCallback(&stAudioDetectCbData, pSysInfo->m_pAudioDetectCallbackData);
    }
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_muAudioDetectCallbackLock);

    return 0;
}
#endif

int Hi_MPP_SetAudioDetectCfg(void *hChannelHandle, LpAudioDetectCfg pCfg)
{
#if (USE_VDA)
#ifdef __IS_SUPPORT_HS_CRY_DETECT__
    int ret = HI_SUCCESS;
    VDA_BCD_PARAM_S bcdParam = {0};

    //ret = Vda_Chn_Get_BcdParam(0, 0, &bcdParam);

    switch (pCfg->nSenstive)
    {
    case 1:
        bcdParam.alarmSen = 70;
        break;
    case 2:
        bcdParam.alarmSen = 80;
        break;
    case 3:
        bcdParam.alarmSen = 90;
        break;
    default:
        bcdParam.alarmSen = 80;
        break;
    }
    bcdParam.enable = !!(pCfg->nEnable);
    bcdParam.alarmPeriod = 3000;
    bcdParam.alarmCountThre = 1;
    bcdParam.alarmInterval = 0;
    bcdParam.callback = (fn_bcd_event_callback)_bcd_event_callback;

    ret = Vda_Chn_Set_BcdParam(0, 0, bcdParam);
    if (ret)
    {
        TRACE_ERR(COMP_VIDEOVDA, "Vda_Chn_Set_BcdParam\n");
        return -1;
    }
#else
        ZW_ERROR("NoSupprotAudioDetect\n");
#endif
#endif
    return 0;
}

int Hi_MPP_SwitchAecMode(void *hChannelHandle, int AecMode, int m_IntercomMode)
{
    int ret = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;

    if (AecMode == 0)
    {
        /* 半双工对讲 */
        //SPK设置
        Hi_MPP_SetAudioOutVol(hChannelHandle, pPlatCtx->m_AudioOutVol, 0, AecMode);
        //MIC设置
        Hi_MPP_SetAudioInVol(hChannelHandle, pPlatCtx->m_AudioInVol, 0, AecMode);
        /* 关闭AEC降噪 */
        ret = Ai_Chn_Set_Aec(0, 0, XRS_FALSE);
    }
    else if (AecMode == 1)
    {
        if (m_IntercomMode == 0)
        {
            /* 全双工对讲 */
            //SPK设置
            Hi_MPP_SetAudioOutVol(hChannelHandle, pPlatCtx->m_AudioOutVol, 0, 1);
            //MIC设置
            Hi_MPP_SetAudioInVol(hChannelHandle, pPlatCtx->m_AudioInVol, 0, 1);
        }
        else
        {
            /* 半双工对讲 */
            //SPK设置
            Hi_MPP_SetAudioOutVol(hChannelHandle, pPlatCtx->m_AudioOutVol, 0, 0);
            //MIC设置
            Hi_MPP_SetAudioInVol(hChannelHandle, pPlatCtx->m_AudioInVol, 0, 2);
        }
        /* 开启AEC降噪 */
        ret = Ai_Chn_Set_Aec(0, 0, XRS_TRUE);
    }

    return ret;
}


int Hi_MPP_GetWiFiInfo(void* hi_wifiInfo)
{
    int handle = 0;
    HALMOD_WIFI_INFO_S *pWifiInfo = NULL;
    pWifiInfo = (HALMOD_WIFI_INFO_S *)hi_wifiInfo;
    if (pWifiInfo == NULL)
    {
        ZW_ERROR("input para error\n");
        return -1;
    }

    if (HalMod_Get_WifiInfo(handle, pWifiInfo))
    {
        ZW_ERROR("=============ZWSys_SetLedRedValue get wifi info error===========\n");
        return -1;
    }

    return 0;
}

#if defined (__PRODUCT_G43TA__) || defined (__PRODUCT_G84F__) || defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)

/**
********************************************************************************
 * @函数功能: Visp_Get_SceneFps接口会返回AE的最大曝光，同时获取Visp_Get_TuneCurrInfo中的EV值
     将最大曝光进行设置，同时根据EV值修改抗闪模式
 * @作者    : YU.CHEN
 * @param[in] *hChannelHandle
 * @param[in] maxShutterTime
 * @返回值  :
 * @其他    :
********************************************************************************
*/
SINT32 Hi_MPP_CustomImageAEParm(void *hChannelHandle, FLOAT32 maxShutterTime, int IspParamEV)
{
    SINT32 s32Ret = 0;
    CPlatformChannelContext  *pPlatCtx = NULL;
    int NeedChangeAE = 0;
    if (NULL == hChannelHandle)
    {
        ZW_ERROR("hChannelHandle CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = (CPlatformChannelContext *)hChannelHandle;


    ISP_TUNE_AE_PARAM_S AeParam = {0};
    s32Ret = Hi_MPP_GetAEParam(pPlatCtx, &AeParam);
    if (s32Ret < 0)
    {
        ZW_ERROR("Hi_MPP_GetAEParam failed\n");
        return s32Ret;
    }

    ISP_TUNE_FLICKER_MODE_E nAnTiStatus = gCurAnTiStatus;
    /* 180000: 开抗闪 */
    if (IspParamEV < 180000)
    {
        AeParam.FlickerMode = pPlatCtx->m_AeParam.FlickerMode;
    }
    /* 270000: 关抗闪 */
    else if (IspParamEV > 270000)
    {
        AeParam.FlickerMode = ISP_TUNE_FLICKER_OFF;
    }
    if (nAnTiStatus != gCurAnTiStatus)
    {
        gCurAnTiStatus = nAnTiStatus;
        NeedChangeAE = 1;
    }

    if((maxShutterTime > 0.01) && ((maxShutterTime - AeParam.MaxExposureTime > 0.01) || (maxShutterTime - AeParam.MaxExposureTime < -0.01)))
    {
        AeParam.MaxExposureTime = maxShutterTime;
        NeedChangeAE = 1;
    }

    if(NeedChangeAE == 1)
    {
        for (int i = 0;i <SYSTEM_SUPPORT_CHANNEL;i++)
        {
                    s32Ret =  Visp_Set_TuneAutoExposure(i, &AeParam);
        }

        if(0 != s32Ret)
        {
            ZW_ERROR("Visp_Set_TuneAutoExposure fail return 0x%x!\n", s32Ret);
            return -1;
        }

        ZW_DEBUG("Hi_MPP_UpdateAEMaxExposureTime Param to %f SUCCESS\n", maxShutterTime);
    }

    return s32Ret;
}


/*G3H图像定制修改接口*/
static int ImageCustom(void)
{
    int s32Ret = 0;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = pChCtx->pPlatformChannelhandle;
    ISP_TUNE_ALG_INFOR_S m_IspAlgParam = {0};
#if defined(__IS_SUPPORT_12FPS__)
    int HightFrame = 12;
#else
    int HightFrame = 10;
#endif
    //确保ISP初始化成功
    if(FALSE == pPlatCtx->m_bISPInitIsOver)
    {
        ZW_ERROR("ISP init is not over...\n");
        return -1;
    }
    UINT32 slowFrame = 10;//慢快门帧率
    static int lastSlowFrame = 10;//上一次慢快门帧率
    ISP_SCENE_FPS_S sceneFps = {0};
    s32Ret = Visp_Get_SceneFps(0, &sceneFps);
    s32Ret = Visp_Get_TuneCurrInfo(0, &m_IspAlgParam);

    if(sceneFps.Fps > HightFrame)
    {
        sceneFps.Fps = HightFrame;
    }
    
    if(s32Ret == 0)
    {
        //1.设置实时帧率至sensor帧
        if (sceneFps.Fps >= 5 && sceneFps.Fps <= HightFrame)
        {
            slowFrame = sceneFps.Fps;
            if(lastSlowFrame != slowFrame)
            {
                ZW_DEBUG("******************* Inter Slow FrameMode! slowFrame=%d\n", slowFrame);
                for (int i = 0;i <SYSTEM_SUPPORT_CHANNEL;i++)
                s32Ret = Hi_MPP_ChangeSensorFps(pChCtx,i, (FLOAT32)slowFrame);
                if (s32Ret == 0)
                {
                    ZW_INFO("Visp_Get_SceneFps sceneFps.Fps=%d set sensor Fps\n",sceneFps.Fps);
                    ZW_DEBUG("m_EncAttr.FrameRate=%d, m_EncAttrAux.FrameRate=%d\n", pChCtx->m_EncAttr.FrameRate, pChCtx->m_EncAttrAux.FrameRate);
                    lastSlowFrame = slowFrame;
                }
                else
                {
                    ZW_ERROR("Hi_MPP_ChangeSensorFps Failed\n");
                }
            }
        }
        //2.根据接口返回参数设置最大曝光时间
        Hi_MPP_CustomImageAEParm(pPlatCtx, sceneFps.AeTime,m_IspAlgParam.Exposure);
    }
    else
    {
        ZW_ERROR("Visp_Get_SceneFps Failed s32Ret=%x,  sceneFps.Fps=%d\n", s32Ret, sceneFps.Fps);
    }
    return 0;
}

#endif

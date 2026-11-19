#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#include "drv_motor.h"
#include "global_macro.h"
#include "motoLib.h"
#include "LogManager.h"
#include "inifile.h"
#include "zwsdk.h"

/**
 * \def ABS(x)
 * \brief A macro returns the absolute value of \a x.
 */
#ifndef ABS
#define ABS(x)              (((x)<0) ? -(x) : (x))
#endif

#define MOTO_LOG(format...)           LOG_D(LOG_MODULE_PTZ,format)
#define MOTO_INFO(format...)          LOG_I(LOG_MODULE_PTZ,format)
#define MOTO_WARN(format...)          LOG_W(LOG_MODULE_PTZ,format)
#define MOTO_ERR(format...)           LOG_E(LOG_MODULE_PTZ,format)

#define MAX_PRESET_COUNT   8        /* 预置点数量,0~7共8个 */
#define MAX_TIMEOUT_GOPOS  120      /* 移动到指定位置的最大超时时间(秒) */

#define	MAX_CHANNEL_CNT SYSTEM_SUPPORT_CHANNEL

#if (DEVICE_SUPPORT_PTZ_NUM ==3) 
	#if (SYSTEM_SUPPORT_CHANNEL == 4)
	#define PTZ_MID		1
	#define PTZ_LERF 	2
	#define PTZ_RIGHT 	3
	#elif (SYSTEM_SUPPORT_CHANNEL == 3)
	#define PTZ_LERF 	1
	#define PTZ_RIGHT 	2
	#endif
#endif

#define PTZ_MAIN 	0


static const char *MOTO_CfgSection = "MOTO_Cfg";            /* 配置区名 */
static int g_moto_fd[MAX_CHANNEL_CNT] = {0};                                  /* 全局文件句柄 */
static int g_nHMaxPos[MAX_CHANNEL_CNT] = {0};                                /* 水平最大步长 */
static int g_nVMaxPos[MAX_CHANNEL_CNT] = {0};                                /* 垂直最大步长 */
static int g_UserVMaxPos = 256;                             /* 用户设置垂直最大步长 */
static int g_VerticalReverse[MAX_CHANNEL_CNT] = {0};                           /* 1:垂直马达反向 */
static int g_ProductTestMode = 0;                           /* 0:出厂模式---1:工厂模式 */
static int g_aMotoPresetEnable[MAX_PRESET_COUNT] = {0};
static struct motor_pos g_aMotoPreset[MAX_PRESET_COUNT] = {0};
pthread_mutex_t g_motorMutex = PTHREAD_MUTEX_INITIALIZER;
static struct motor_pos HD_pos={0};

static int s_nSpeedHMax = MOTO_SPEED_H_MAX;
static int s_nSpeedVMax = MOTO_SPEED_V_MAX;


int MOTO_SetUserVMaxPos(int nVPos)
{
    if (nVPos < 0 || nVPos > g_nVMaxPos[0]) 
    {
        g_UserVMaxPos = g_nVMaxPos[0];
        return -1;
    }

    g_UserVMaxPos = nVPos;
    return 0;
}

int MOTO_SetVerticalReverse(int nReverse, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

    g_VerticalReverse[Chn] = (nReverse > 0) ? 1 : 0;
    return 0;
}

int MOTO_SetProductTestMode(int nTestMode)
{
    g_ProductTestMode = (nTestMode > 0) ? 1 : 0;
    return 0;
}

int MOTO_GetProductTestMode()
{
    return g_ProductTestMode;
}

int MOTO_Ctrl(int Chn,int pwr,int log_level)
{
	if(Chn < 0)
	{
		MOTO_ERR("%s Chn is  [%d] ERR!!\n",__func__,Chn);
		return -1;
	}

	int ret = 0;
	struct motor_ctrl ctrl={0};
	ctrl.log_level = log_level;
	ctrl.pwr = pwr;
	ret = ioctl(g_moto_fd[Chn],MTDRV_CTRL,&ctrl);
	if(ret < 0)
	{
		MOTO_ERR("%s  fd[%d]-ret[%d]-pwr[%d]-log_level[%d] %s\n",__func__,g_moto_fd[Chn],ret,ctrl.pwr,ctrl.log_level,strerror(errno));
		return -1;
	}

    return 0;
}

int MOTO_Init_Handle()
{
#if DEVICE_SUPPORT_PTZ_NUM <=1
	if (g_moto_fd[PTZ_MAIN] <= 0)
    {
        g_moto_fd[PTZ_MAIN] = open("/dev/" MOTO_DEVICE_NAME_0, O_RDWR);
		MOTO_INFO("g_moto_fd[%d]=[%d] [%s]\n",PTZ_MAIN,g_moto_fd[PTZ_MAIN],strerror(errno));
        if (g_moto_fd[PTZ_MAIN] <= 0)
        {
        	MOTO_ERR("Open %s error!\n", MOTO_DEVICE_NAME_0);
            return -1;
        }
		MOTO_Ctrl(PTZ_MAIN,1,0);
    }
#else 
	if (g_moto_fd[PTZ_MAIN] <= 0)
    {
        g_moto_fd[PTZ_MAIN] = open("/dev/" MOTO_DEVICE_NAME_0, O_RDWR);
		MOTO_INFO("g_moto_fd[%d]=[%d] [%s]\n",PTZ_MAIN,g_moto_fd[PTZ_MAIN],strerror(errno));
        if (g_moto_fd[PTZ_MAIN] <= 0)
        {
        	MOTO_ERR("Open %s error!\n", MOTO_DEVICE_NAME_0);
            return -1;
        }
		MOTO_Ctrl(PTZ_MAIN,1,0);
    }
    if (g_moto_fd[PTZ_LERF] <= 0)
    {
        g_moto_fd[PTZ_LERF]= open("/dev/" MOTO_DEVICE_NAME_1, O_RDWR);
		MOTO_INFO("g_moto_fd[%d]=[%d] [%s]\n",PTZ_LERF,g_moto_fd[PTZ_LERF],strerror(errno));
        if (g_moto_fd[PTZ_LERF] <= 0)
        {
        	MOTO_ERR("Open %s error!\n", MOTO_DEVICE_NAME_1);
            return -1;
        }
		MOTO_Ctrl(PTZ_LERF,1,0);
    }
	if (g_moto_fd[PTZ_RIGHT] <= 0)
    {
        g_moto_fd[PTZ_RIGHT] = open("/dev/" MOTO_DEVICE_NAME_2, O_RDWR);
		MOTO_INFO("g_moto_fd[%d]=[%d] [%s]\n",PTZ_RIGHT,g_moto_fd[PTZ_RIGHT],strerror(errno));
        if (g_moto_fd[PTZ_RIGHT] <= 0)
        {
        	MOTO_ERR("Open %s error!\n", MOTO_DEVICE_NAME_2);
            return -1;
        }
		MOTO_Ctrl(PTZ_RIGHT,1,0);
    }
#endif
    return 0;
}

int MOTO_Init()
{
	int i = 0;
	char szSectionName[20] = {0};

	for (i = 0; i < MAX_PRESET_COUNT; ++i)
	{
		snprintf(szSectionName, sizeof(szSectionName), "%s_%d", MOTO_CfgSection, i);
		g_aMotoPresetEnable[i] = read_profile_int(szSectionName, "enable", 0, MOTO_FILE_CFG);
		g_aMotoPreset[i].hpos = read_profile_int(szSectionName, "hpos", 0, MOTO_FILE_CFG);
		g_aMotoPreset[i].vpos = read_profile_int(szSectionName, "vpos", 0, MOTO_FILE_CFG);
	}

    MOTO_Init_Handle();
	ZWSys_RegisterMotoSavePosCallback(MOTO_SaveNowPos);

	return 0;
}

int MOTO_SelfTestLoop()
{
	int ret =0;
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_SELF_TEST_LOOP);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_SelfTest(int Chn)
{
	int ret =0;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[Chn], MTDRV_SELF_TEST);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_GetPos(int *pHPos, int *pVPos)
{
    int ret =0;
    struct motor_pos pos={0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_POS, &pos);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}
	*pHPos = pos.hpos;
	*pVPos = pos.vpos;

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_SetMaxPos(int nHPos, int nVPos, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	g_nHMaxPos[Chn] = nHPos;
	g_nVMaxPos[Chn] = nVPos;

    int ret =0;
    struct motor_pos pos={0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    pos.hpos = nHPos;
    pos.vpos = nVPos;
    ret = ioctl(g_moto_fd[Chn], MTDRV_SET_MAXPOS, &pos);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_SetPos(int nHPos, int nVPos,unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

    int ret =0;
    struct motor_pos pos={0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    pos.hpos = nHPos;
    pos.vpos = nVPos;
    ret = ioctl(g_moto_fd[Chn], MTDRV_SET_POS, &pos);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}


int MOTO_ToPos(int nHPos, int nVPos)
{
    int ret =0;
    struct motor_pos pos={0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    pos.hpos = nHPos;
    pos.vpos = nVPos;
    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_TO_POS, &pos);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_SaveNowPos()
{
    int ret =0;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_POS, &HD_pos);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}
	MOTO_LOG("[CYADD] SaveNowPos HD_pos hpos[%d] vpos[%d] \n",HD_pos.hpos,HD_pos.vpos);
	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

// 人形跟踪使用,支持两个电机同时转动
int MOTO_ToPos_Product(int nHPos, int nVPos)
{
    int ret =0;
	int iState = YES;
    struct motor_pos pos={0};
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_STATE, &iState);
	if (0 != ret)
	{
		MOTO_ERR("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}
	if (NO == iState)
	{
		if (0 != access(SD_AGING_TEST_LIST_FILE_NAME, F_OK))
		{
			if (0 == HD_pos.hpos && 0 == HD_pos.vpos)
			{
				goto error1;
			}
		}
		
		//老化测试时由(0,0)到(Hmax,Vmax),给的值都是>=0的，不需要考虑当前位置，避免出现转到顶之后卡着不动的问题
		pos.hpos = HD_pos.hpos + nHPos;
		pos.vpos = HD_pos.vpos + nVPos;
		
		if (pos.hpos > g_nHMaxPos[0])
		{
		 	pos.hpos = g_nHMaxPos[0];
		}

		if (pos.hpos < 0)
		{
			pos.hpos = 0;
		}

		if (pos.vpos > g_nVMaxPos[0])
		{
		 	pos.vpos = g_nVMaxPos[0];
		}

		if (pos.vpos < ((g_nVMaxPos[0]-g_UserVMaxPos > 0) ? (g_nVMaxPos[0] - g_UserVMaxPos) : 0))
		{
			pos.vpos = ((g_nVMaxPos[0]-g_UserVMaxPos > 0) ? (g_nVMaxPos[0] - g_UserVMaxPos) : 0);
		}
		ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_TO_POS_PRODUCT, &pos);
		if(ret)
		{
			MOTO_ERR("MTDRV_TO_POS_PRODUCT error! [%d]\n", ret);
			goto error1;
		}
		MOTO_LOG("[MOTO_ToPos_Product] iState:NO\n");
	}
	else
	{
		MOTO_LOG("[MOTO_ToPos_Product] iState:YES\n");
	}

error1:
	pthread_mutex_unlock(&g_motorMutex);

    ret = 0;
	//如果电机实际未调整，则返回1
	if((pos.hpos == HD_pos.hpos) && (pos.vpos == HD_pos.vpos))
	{
		ret = 1;
	}
    
	return ret;
}


int MOTO_ToPos_AgingTest(int nHPos, int nVPos, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

    int ret =0;
    struct motor_pos pos={0};

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	pos.hpos = nHPos;
	pos.vpos = nVPos;
	
	if (pos.hpos > g_nHMaxPos[Chn])
	{
	 	pos.hpos = g_nHMaxPos[Chn];
	}
	else if (pos.hpos < 0)
	{
		pos.hpos = 0;
	}

	if (pos.vpos > g_nVMaxPos[Chn])
	{
	 	pos.vpos = g_nVMaxPos[Chn];
	}
	else if (pos.vpos <  0)
	{
		pos.vpos = 0;
	}

	if (g_moto_fd[Chn] > 0)
    {
    	ret = ioctl(g_moto_fd[Chn], MTDRV_TO_POS_PRODUCT, &pos);
    	if (ret)
    	{
    		MOTO_ERR("MTDRV_TO_POS_PRODUCT error! [%d]\n", ret);
    		goto error1;
    	}
    }

error1:
	pthread_mutex_unlock(&g_motorMutex);
    
	return ret;
}

int MOTO_SelfCheckBack(int nHPos, int nVPos)
{
    int ret = 0;
    struct motor_pos pos={0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    pos.hpos = nHPos;
    pos.vpos = nVPos;
    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_SELF_CHECK_BACK, &pos);
	if (0 != ret)
	{
		MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	MOTO_INFO("MOTO_SelfCheckBack \n\n");
	return ret;
}

int MOTO_ToStartPos(int nHPos, int nVPos)
{
    int ret =0;
    struct motor_pos pos={0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    pos.hpos = nHPos;
    pos.vpos = nVPos;
    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_TO_START_POS, &pos);

	pthread_mutex_unlock(&g_motorMutex);
    MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

int MOTO_Stop(int Chn)
{
	int ret =0;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[Chn], MTDRV_STOP);

	pthread_mutex_unlock(&g_motorMutex);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

int MOTO_Left(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	nRunStep = stCurrentPos.hpos;
	if (nSteps > nRunStep)
	{
		nSteps = nRunStep;
	}
	ret = ioctl(g_moto_fd[Chn], MTDRV_LEFT, &nSteps);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_Right(int nSteps,unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	nRunStep = g_nHMaxPos[Chn] - stCurrentPos.hpos;
	if (nSteps > nRunStep)
	{
		nSteps = nRunStep;
	}
	ret = ioctl(g_moto_fd[Chn], MTDRV_RIGHT, &nSteps);

	pthread_mutex_unlock(&g_motorMutex);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

int MOTO_Up(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));

    if ((g_VerticalReverse == 0))
    {
    	if(Chn == 0)
			nRunStep = ((MOTO_GetProductTestMode())?(stCurrentPos.vpos):(stCurrentPos.vpos - (g_nVMaxPos[Chn]-g_UserVMaxPos)));
		else
			nRunStep = stCurrentPos.vpos;
    }
    else
    {
        nRunStep = stCurrentPos.vpos;
    }


	if (nSteps > nRunStep)
	{
		nSteps = nRunStep;
	}
	ret = ioctl(g_moto_fd[Chn], MTDRV_UP, &nSteps);

	pthread_mutex_unlock(&g_motorMutex);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

int MOTO_Down(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));

	if ((g_VerticalReverse == 0))
	{
        nRunStep = g_nVMaxPos[Chn] - stCurrentPos.vpos;
	}
	else
	{
		if(Chn == 0)
		nRunStep = ((MOTO_GetProductTestMode())?(g_nVMaxPos[Chn] - stCurrentPos.vpos):(g_UserVMaxPos - stCurrentPos.vpos));
		else
		nRunStep = g_nVMaxPos[Chn] - stCurrentPos.vpos;
	}

	if (nSteps > nRunStep)
	{
		nSteps = nRunStep;
	}
    ret = ioctl(g_moto_fd[Chn], MTDRV_DOWN, &nSteps);

	pthread_mutex_unlock(&g_motorMutex);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

int MOTO_DropLeft(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	nRunStep = stCurrentPos.hpos;
	if (nRunStep > 0)
	{
	    ret = ioctl(g_moto_fd[Chn], MTDRV_LEFT, &nRunStep);
		MOTO_LOG("%s ret=%d,nRunStep[%d],g_moto_fd[%d]=[%d] %s\n",__func__,ret,nRunStep,Chn,g_moto_fd[Chn], strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_DropRight(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d,stCurrentPos.hpos[%d],g_moto_fd[%d]=[%d] %s\n",__func__,ret,stCurrentPos.hpos,Chn,g_moto_fd[Chn], strerror(errno));
	nRunStep = g_nHMaxPos[Chn] - stCurrentPos.hpos;
	if (nRunStep > 0)
	{
	    ret = ioctl(g_moto_fd[Chn], MTDRV_RIGHT, &nRunStep);
		MOTO_LOG("%s ret=%d,nRunStep[%d],g_moto_fd[%d]=[%d] %s\n",__func__,ret,nRunStep,Chn,g_moto_fd[Chn], strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_DropUp(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
    
    if ((g_VerticalReverse == 0))
    {
    	if(Chn == 0)
			nRunStep = ((MOTO_GetProductTestMode())?(stCurrentPos.vpos):(stCurrentPos.vpos - (g_nVMaxPos[Chn]-g_UserVMaxPos)));
		else
			nRunStep = stCurrentPos.vpos;
    }
    else
    {
        nRunStep = stCurrentPos.vpos;
    }

	if (nRunStep > 0)
	{
	    ret = ioctl(g_moto_fd[Chn], MTDRV_UP, &nRunStep);
		MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);	
	return ret;
}

int MOTO_DropDown(int nSteps, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return -1;
    }

	int ret =0;
	int nRunStep = 0;
	struct motor_pos stCurrentPos = {0};
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[Chn], MTDRV_GET_POS, &stCurrentPos);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));

	if ((g_VerticalReverse == 0))
	{
        nRunStep = g_nVMaxPos[Chn] - stCurrentPos.vpos;
	}
	else
	{
		if(Chn == 0)
			nRunStep = ((MOTO_GetProductTestMode())?(g_nVMaxPos[Chn] - stCurrentPos.vpos):(g_UserVMaxPos - stCurrentPos.vpos));
		else
			nRunStep = g_nVMaxPos[Chn] - stCurrentPos.vpos;
	}


	if (nRunStep > 0)
	{
	    ret = ioctl(g_moto_fd[Chn], MTDRV_DOWN, &nRunStep);
		MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_PreToPosExt(int nHPos, int nVPos)
{
	int rc = 0;
    int needMove = 0; // 1 - need move, 0 - no need move
    int needHMove = 0;
    int needVMove = 0;
	struct motor_pos stCurrentPos = {0};
	int movePos = 0;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return needMove;
    }
	rc = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_POS, &stCurrentPos);
	if (0 == rc)
	{
		if (0 != nHPos)
		{
			needHMove = 1;
		 	if ((stCurrentPos.hpos <= 0) || (stCurrentPos.hpos >= g_nHMaxPos[0]))
		 	{
			 	movePos = stCurrentPos.hpos + nHPos;
			 	if ((movePos < 0) || (movePos > g_nHMaxPos[0]))
			 	{
				 	needHMove = 0;
			 	}
		 	}
		}
		else
		{
			needHMove = 0;
		}
		if (0 != nVPos)
		{
			needVMove = 1;
		 	if ((stCurrentPos.vpos <= 0) || (stCurrentPos.vpos >= g_nVMaxPos[0]))
		 	{
			 	movePos = stCurrentPos.vpos + nVPos;
			 	if ((movePos < 0) || (movePos > g_nVMaxPos[0]))
			 	{
				 	needVMove = 0;
			 	}
		 	}
		}
		else
		{
			needVMove = 0;
		}
	}

	pthread_mutex_unlock(&g_motorMutex);

	if ((0 != needHMove) || (0 != needVMove))
	{
		needMove = 1;
	}
	return needMove;
}

#if defined (__PRODUCT_G85T__) || defined(__PRODUCT_W85T__) || defined(__PRODUCT_G84F__) || defined(__PRODUCT_G63F__) || defined(__PRODUCT_W84F__) || defined(__PRODUCT_W63F__)
// 人形跟踪使用,但不满足两个电机同时转动
int MOTO_ToPosExt(int nHPos, int nVPos)
{
    int ret =0;
	int iState = YES;
	static unsigned long ulRunningTime = 0; // 最后一次检测旋转时间
	struct motor_pos stCurrentPos = {0};
    struct motor_pos pos={0};

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_STATE, &iState);
	
	struct timeval NowTime;
	gettimeofday(&NowTime, NULL );

	unsigned long ulNowTime = NowTime.tv_sec*1000 + NowTime.tv_usec/1000;
	long lNoneMoveTime = (long)(ulNowTime - ulRunningTime);
	int timeInt = 50;
	if (NO == iState)
	{
		//MOTO_INFO("[MOTO_ToPosExt] iState:NO, ulNowTime:%u, ulRunningTime:%u, ABS(lNoneMoveTime):%d!\n", ulNowTime, ulRunningTime, ABS(lNoneMoveTime));
		if (ABS(lNoneMoveTime) > timeInt)
		{
			ulRunningTime = ulNowTime;
			ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_POS, &stCurrentPos);
			
			pos.hpos = stCurrentPos.hpos + nHPos;
			pos.vpos = stCurrentPos.vpos + nVPos;
			
			if ((int)pos.hpos > g_nHMaxPos[0])
			{
			 	pos.hpos = g_nHMaxPos[0];
			}

			if ((int)pos.hpos < 0)
			{
				pos.hpos = 0;
			}

			if ((int)pos.vpos > g_nVMaxPos[0])
			{
			 	pos.vpos = g_nVMaxPos[0];
			}

			if ((int)pos.vpos < 0)
			{
				pos.vpos = 0;
			}
			ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_TO_POS, &pos);
		}
	}
	else
	{
		MOTO_INFO("[MOTO_ToPosExt] iState:YES, ulNowTime:%u, ulRunningTime:%u, ABS(ulNowTime - ulRunningTime):%d!\n", ulNowTime, ulRunningTime, ABS((ulNowTime - ulRunningTime)));
		ulRunningTime = ulNowTime;
	}

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

#else

// 人形跟踪使用,但不满足两个电机同时转动
int MOTO_ToPosExt(int nHPos, int nVPos)
{
    int ret =0;
	struct motor_pos stCurrentPos = {0};
    struct motor_pos pos={0};

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

	//MOTO_INFO("[MOTO_ToPosExt] iState:NO, ulNowTime:%u, ulRunningTime:%u, ABS(lNoneMoveTime):%d!\n", ulNowTime, ulRunningTime, ABS(lNoneMoveTime));

	ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_POS, &stCurrentPos);
	
	pos.hpos = stCurrentPos.hpos + nHPos;
	pos.vpos = stCurrentPos.vpos + nVPos;
	
	if ((int)pos.hpos > g_nHMaxPos[0])
	{
	 	pos.hpos = g_nHMaxPos[0];
	}

	if ((int)pos.hpos < 0)
	{
		pos.hpos = 0;
	}

	if ((int)pos.vpos > g_nVMaxPos[0])
	{
	 	pos.vpos = g_nVMaxPos[0];
	}

	if ((int)pos.vpos < 0)
	{
		pos.vpos = 0;
	}
	ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_TO_POS, &pos);

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

#endif
int MOTO_GetState(int *pState)
{
    int ret =0;
	int iState = YES;
	
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_STATE, &iState);
	if (0 != ret)
	{
		MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	}

	*pState = iState;
	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_GetStartState(int *pState)
{
    int ret =0;
	int iState = YES;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }

    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_START_STATE, &iState);
	*pState = iState;

	pthread_mutex_unlock(&g_motorMutex);
    MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

int MOTO_GetSelfCheckState(int *pState)
{
    int ret =0;
	int iState = YES;

    /* H4W用__USE_KS_FS__宏返回，暂不清楚原因 */
	return 0;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }
	
    ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_GET_SELF_CHECK_STATE, &iState);
	*pState = iState;
	pthread_mutex_unlock(&g_motorMutex);

    MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
	return ret;
}

//设置云台速度上限
int MOTO_SetSpeedMax(unsigned int nSpeedHmax, unsigned int nSpeedVmax)
{
    if (nSpeedHmax < 1 || nSpeedVmax < 1)
    {
        MOTO_ERR("Speed param invalid: nSpeedHmax=%d nSpeedVmax=%d!\n", nSpeedHmax, nSpeedVmax);
        return -1;
    }

    if (nSpeedHmax > MOTO_SPEED_H_MAX)
    {
        nSpeedHmax = MOTO_SPEED_H_MAX;
    }
    if (nSpeedVmax > MOTO_SPEED_V_MAX)
    {
        nSpeedVmax = MOTO_SPEED_V_MAX;
    }
    
    s_nSpeedHMax = nSpeedHmax;
    s_nSpeedVMax = nSpeedVmax;

    MOTO_INFO("[PTZ_MAX_SPEED] s_nSpeedHMax=%d s_nSpeedVMax=%d\n", s_nSpeedHMax, s_nSpeedVMax);
    return 0;
}

void MOTO_SetSpeed(unsigned int nSpeedh, unsigned int nSpeedv, unsigned int Chn)
{
	if (Chn >= MAX_CHANNEL_CNT)
    {
        return;
    }

	int ret = 0;
	struct motor_speed speed = {0};
	speed.hspeed = nSpeedh;
	speed.vspeed = nSpeedv;

	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return;
    }
        /*垂直电机 1:64 最大速度会丢步, 需限制*/
    speed.hspeed = (nSpeedh >= s_nSpeedHMax ? s_nSpeedHMax:nSpeedh);
    speed.vspeed = (nSpeedv >= MOTO_SPEED_V_MAX ? MOTO_SPEED_V_MAX:nSpeedv);
	ret = ioctl(g_moto_fd[Chn], MTDRV_SET_SPEED_LEV_V2, &speed);
    if (ret)
    {
        MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));
    }

	pthread_mutex_unlock(&g_motorMutex);
    return;
}

//电机速度版本修改，根据步长判断
int MOTO_SetSpeedType(int nHPos, int nVPos)
{
	int ret = 0;
	struct motor_speed pos_speed = {0};
	pthread_mutex_lock(&g_motorMutex);
    if (MOTO_Init_Handle() != 0)
    {
        pthread_mutex_unlock(&g_motorMutex);
        return -1;
    }
	if(nHPos >= 800)
	{
		pos_speed.hspeed = MOTOR_SPEED_TYPE_1_25_64;
	}
	else
	{
		pos_speed.hspeed = MOTOR_SPEED_TYPE_1_64;
	}

	if(nVPos >= 500)
	{
		pos_speed.vspeed = MOTOR_SPEED_TYPE_1_25_64;
	}
	else
	{
		pos_speed.vspeed = MOTOR_SPEED_TYPE_1_64;
	}
	ret = ioctl(g_moto_fd[PTZ_MAIN], MTDRV_SET_SPEED_TYPE, &pos_speed);
	MOTO_LOG("%s ret=%d, %s\n",__func__,ret, strerror(errno));

	pthread_mutex_unlock(&g_motorMutex);
	return ret;
}

int MOTO_RemovePreset(unsigned long dwPresetIndex)
{
	if (dwPresetIndex >= MAX_PRESET_COUNT)
	{
		return -1;
	}
	if (0 != g_aMotoPresetEnable[dwPresetIndex])
	{
		char szSectionName[20];
		snprintf(szSectionName, sizeof(szSectionName), "%s_%ld", MOTO_CfgSection, dwPresetIndex);
		g_aMotoPresetEnable[dwPresetIndex] = 0;
		write_profile_int(szSectionName, "enable", 0, MOTO_FILE_CFG);
	}

	return 0;
}

int MOTO_CallPreset(unsigned long dwPresetIndex)
{
	if (dwPresetIndex >= MAX_PRESET_COUNT)
	{
		return -1;
	}
	if (0 != g_aMotoPresetEnable[dwPresetIndex])
	{
		MOTO_ToPos(g_aMotoPreset[dwPresetIndex].hpos, g_aMotoPreset[dwPresetIndex].vpos);
	}
	
	return 0;
}

int MOTO_SetPreset(unsigned long dwPresetIndex)
{
	if (dwPresetIndex >= MAX_PRESET_COUNT)
	{
		return -1;
	}
	if (0 == MOTO_GetPos(&g_aMotoPreset[dwPresetIndex].hpos, &g_aMotoPreset[dwPresetIndex].vpos))
	{
		char szSectionName[20];
		snprintf(szSectionName, sizeof(szSectionName), "%s_%ld", MOTO_CfgSection, dwPresetIndex);
		g_aMotoPresetEnable[dwPresetIndex] = 1;
		write_profile_int(szSectionName, "enable", 1, MOTO_FILE_CFG);
		write_profile_int(szSectionName, "hpos", g_aMotoPreset[dwPresetIndex].hpos, MOTO_FILE_CFG);
		write_profile_int(szSectionName, "vpos", g_aMotoPreset[dwPresetIndex].vpos, MOTO_FILE_CFG);
	}

	return 0;
}

//获取预置点坐标
int MOTO_GetPresetPos(unsigned long dwPresetIndex, int *pHPos, int *pVPos)
{
	if (dwPresetIndex >= MAX_PRESET_COUNT
		|| NULL == pHPos
		|| NULL == pVPos)
	{
		//参数无效
		return -1;
	}
	if (0 == g_aMotoPresetEnable[dwPresetIndex])
	{
		//未设置
		return 1;
	}

	*pHPos = g_aMotoPreset[dwPresetIndex].hpos;
	*pVPos = g_aMotoPreset[dwPresetIndex].vpos;
	
	return 0;
}

/*! 巡航移动至预置点
 * @param GO_CRUISE_NODE_INFO
 * @return -2 预置点未设置 -1 - 移动超时 0 - 到达指定位置 1 - 停止
 */
int MOTO_GoCruiseNode(GO_CRUISE_NODE_INFO *node)
{
	if (NULL == node)
	{
		MOTO_INFO("[%s] Param Err! node is NULL\n", __FUNCTION__);
		return 0;
	}
	int ret = -2; // -2 预置点未设置 -1 - 移动超时 0 - 到达指定位置 1 - 停止
	int cnt_num = 0;

	if (0 != *(node->bStop))
	{
		return 1;
	}
    node->nCntTime = 0;

	// preset is set
	if (g_aMotoPresetEnable[node->idx])
	{
		ret = -1;
		// set speed
		// target positon, get preset
		int target_h = g_aMotoPreset[node->idx].hpos;
		int target_v = g_aMotoPreset[node->idx].vpos;
		// current positon
		int current_h, current_v = 0;
		// goto preset
		MOTO_ToPos(target_h, target_v);
		// 250ms once check
		cnt_num = MAX_TIMEOUT_GOPOS * 4;
		do {
			if (0 != *(node->bStop))
			{
				ret = 1;
				break;
			}
			if (0 == MOTO_GetPos(&current_h, &current_v))
			{
				// TODO: consider deviation?
				if ((target_h == current_h) && (target_v == current_v))
				{
					ret = 0;
					break;
				}
			}
			usleep(250*1000);
            node->nCntTime += 250;
			cnt_num--;
		} while(cnt_num > 0);
		// residence time
		if (0 == ret)
		{
			cnt_num = node->retentionTime * 4;
			do {
				if (0 != *(node->bStop))
				{
					ret = 1;
					break;
				}
				usleep(250*1000);
                node->nCntTime += 250;
				cnt_num--;
			} while(cnt_num > 0);
		}
	}
	return ret;
}


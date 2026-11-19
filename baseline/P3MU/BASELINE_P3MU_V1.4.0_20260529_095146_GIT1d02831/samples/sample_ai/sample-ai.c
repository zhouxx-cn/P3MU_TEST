
#include <stdio.h>
#include "ai.h"

//内部接口：绑定源为AI时用到
extern SINT32 Ai_Chn_Bind(int aiChn, int dstDevType, int dstChn);
extern SINT32 Ai_Chn_UnBind(int aiChn, int dstDevType, int dstChn);
//内部接口：仅供测试验证输入ai文件内容
extern SINT32 Ai_Record_Test(int enable);

static void _Ai_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static int ai_init()
{
	int s32Ret = 0;
	AI_CONF_S stAiCfg = {1, 1, {0}, {0}};
	AUDIO_CONF_S stAiChnConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 320};

	
#ifdef __SUPPORT_JZ_MERT_ALGO__
	stAiCfg.PlatCfg.JzPlat.MertSize = 1024*1024;
#endif
	s32Ret = Ai_Open(0, stAiCfg, AUDIO_DEV_CFG_MODE_T);
	if (s32Ret)
	{
		AI_TRACE_ERR("Ai_Open failed with %#x\n", s32Ret);
		return -1;
	}

	s32Ret = Ai_Start(0, 0);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Start Failed!\n");
		return -1;
	}

	s32Ret = Ai_Chn_Open(0, AI_CHTYPE_TALK, 0, stAiChnConf);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Open Failed!\n");
		return -1;
	}

	s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, 90);
	if (s32Ret < 0)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Set_Volume Failed!\n");
		return -1;
	}

	return 0;
}

static int ai_unInit()
{
	int s32Ret = 0;

	s32Ret = Ai_Chn_Close(0, AI_CHTYPE_TALK, 0);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Ai_Stop(0);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Ai_Close(0);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Close Failed!\n");
		return -1;
	}

	return 0;
}

static int ai_record()
{
	char sCmd[8] = {0};
	int nCmd = 0;
	
	while(1)
	{
        printf("ai record test!(file path:./record_file.pcm) \n"
            "\t1 - start record\n"
            "\t2 - stop record\n"
            "\tq - quit\n");

		_Ai_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);

		switch (nCmd)
		{	
			case 1:
				Ai_Record_Test(1);
				break;
			case 2:
				Ai_Record_Test(0);
				break;
			default:
				AI_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	int volume = 0;
	
	ret = ai_init();
	if (ret)
	{
		AI_TRACE_ERR("ai_init FAIL(%d)\n",ret);
		return -1;
	}

	while(1)
	{
        printf("ai test!: \n"
            "\t1 - set ai volume\n"
            "\t2 - record to record_file.pcm\n"
            "\tq - quit\n");

		_Ai_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		AI_TRACE_INFO("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				printf("enter volume: \n");
				_Ai_Get_Cmd(sCmd);
				nCmd = atoi(sCmd);
				ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, nCmd);
				if (ret)
				{
					AI_TRACE_ERR("[AUDIO] Ai_Set_Volume Volume(%d) Failed!\n", nCmd);
				}
				Ai_Get_Volume(0, AUDIO_DEV_AI_TALK, 0, &volume);
				AI_TRACE_INFO("[AUDIO] Ai_Get_Volume Volume(%d)!\n", volume);
				break;
			case 2:
				ai_record();
				break;
			default:
				AI_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}


	ret = ai_unInit();
	if (ret)
	{
		AI_TRACE_ERR("ai_unInit FAIL(%d)\n",ret);
		return -1;
	}
	return 0;
}

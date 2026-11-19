
#include <stdio.h>
#include "ao.h"
#include "halmod.h"

static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}


static int ao_init()
{
	int s32Ret = 0;
	AUDIO_CONF_S stAoConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 640};
//	AO_BIND_CONF_S stAoBindConf = {0, AO_DEV_CVBS, AUDIO_DEV_ADEC, 0};
	
	s32Ret = Ao_Open(0, stAoConf, AUDIO_DEV_CFG_MODE_T);
	if (s32Ret)
	{
		AO_TRACE_ERR("Ao_Open failed with %#x\n", s32Ret);
		return -1;
	}

	s32Ret = Ao_Start(0, 0);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Start Failed!\n");
		return -1;
	}


	s32Ret = Ao_Set_Volume(0, AO_DEV_CVBS, 90);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Set_Volume Failed!\n");
		return -1;
	}

//	stAoBindConf.AoDev = AO_DEV_CVBS;
//	stAoBindConf.SrcDevType = AUDIO_DEV_ADEC;
//	s32Ret = Ao_Chn_Bind(0, stAoBindConf);
//	if (s32Ret)
//	{
//		AO_TRACE_ERR("[AUDIO] Ao_Chn_Bind Failed!\n");
//		return -1;
//	}

	return 0;
}

static int ao_unInit()
{
	int s32Ret = 0;
	AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};

//	s32Ret = Ao_Chn_UnBind(0, stAoBindConf);
//	if (s32Ret)
//	{
//		AO_TRACE_ERR("[AUDIO] Ao_Chn_UnBind Failed!\n");
//		return -1;
//	}

	s32Ret = Ao_Stop(0);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Ao_Close(0);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_Close Failed!\n");
		return -1;
	}

	return 0;
}

#define AO_TEST_PLAY_FILE "./ao_play.pcm"
#define AO_TEST_BUF_SIZE (320)

static int ao_play()
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int size = 0;
	int ret = 0;
	
	buf = (unsigned char *)malloc(10*AO_TEST_BUF_SIZE);
	if (NULL == buf) 
	{
		AO_TRACE_ERR("malloc size(%d) Failed!\n", AO_TEST_BUF_SIZE);
		return -1;
	}
	
	fp = fopen(AO_TEST_PLAY_FILE, "rb");
	if (NULL == fp)
	{
		AO_TRACE_ERR("fopen %s Failed!\n", AO_TEST_PLAY_FILE);
		free(buf);
		return -1;
	}

	while(1)
	{
		size = fread(buf, 1, AO_TEST_BUF_SIZE, fp);
		if (size != AO_TEST_BUF_SIZE)
		{
			AO_TRACE_ERR("ao play over!\n");
			break;
		}

		ret = Ao_Chn_Write(0, 0, buf, size, ADEC_WRITE_BLOCKING, AO_WRITE_ING);
		if (ret)
		{
			AO_TRACE_ERR("Ao_Chn_Write fail, exit!\n");
			break;
		}
	}

	free(buf);

	return 0;
}


int main(int argc, char *argv[])
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	XRS_BOOL mute = XRS_FALSE; 
	int Volume = 0;
	
	//开启喇叭输出
	HalMod_Set_SpeakerStat(0, 1);
	
	ret = ao_init();
	if (ret)
	{
		AO_TRACE_ERR("ai_init FAIL(%d)\n",ret);
		return -1;
	}

	while(1)
	{
        printf("ao test!: \n"
            "\t1 - set ao volume\n"
            "\t2 - set ao mute\n"
            "\t3 - play ao_play.pcm\n"
            "\tq - quit\n");

		_Test_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		AO_TRACE_INFO("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				printf("enter volume: \n");
				_Test_Get_Cmd(sCmd);
				nCmd = atoi(sCmd);
				ret = Ao_Set_Volume(0, AO_DEV_CVBS, nCmd);
				if (ret)
				{
					AO_TRACE_ERR("[AUDIO] Ao_Set_Volume Volume(%d) Failed!\n", nCmd);
				}

				Ao_Get_Volume(0, AO_DEV_CVBS, &Volume);
				AO_TRACE_ERR("[AUDIO] Ao_Get_Volume Volume(%d) \n", Volume);
				break;
			case 2:
				printf("enter mute:0-unMute;1-Mute \n");
				_Test_Get_Cmd(sCmd);
				nCmd = atoi(sCmd);
				
				ret = Ao_Set_Mute(0, AO_DEV_CVBS, nCmd);
				if (ret)
				{
					AO_TRACE_ERR("[AUDIO] Ao_Set_Mute mute(%d) Failed!\n", nCmd);
				}
				
				Ao_Get_Mute(0, AO_DEV_CVBS, &mute);
				AO_TRACE_ERR("[AUDIO] Ao_Get_Mute mute(%d) \n", mute);
				break;				
			case 3:
				ao_play();
				break;
			default:
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	ret = ao_unInit();
	if (ret)
	{
		AO_TRACE_ERR("ai_unInit FAIL(%d)\n",ret);
		return -1;
	}
	return 0;
}

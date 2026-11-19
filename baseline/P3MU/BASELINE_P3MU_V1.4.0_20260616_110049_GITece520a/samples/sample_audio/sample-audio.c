
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#include "ao.h"
#include "ai.h"
#include "aenc.h"
#include "adec.h"
#include "halmod.h"

#include <sys/time.h>

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif


static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}
static UINT64 get_ms()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (UINT64)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;
}

#define AENC_OUT_FILE "./aenc_record"
static int s_AencFileSave = 0;

static SINT32 _Audio_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, char *data, SINT32 size)
{
	static int fd[AENC_CHN_MAX] = {-1, -1, -1};
	char filePath[64] = {0};
	int writeRet = 0;

//	AENC_TRACE_INFO("[AUDIO] stype=%d,idx=%d,size=%d,s_AencFileSave=%d!\n", stype, idx, size,s_AencFileSave);
	if (s_AencFileSave)
	{
		if (-1 == fd[idx])
		{
			memset(filePath, 0, sizeof(filePath));
			snprintf(filePath, sizeof(filePath), "%s-%d", AENC_OUT_FILE, idx);
		
			fd[idx] = open(filePath, O_RDWR | O_CREAT, 0x644);
			if (fd[idx] < 0)
			{
				AENC_TRACE_ERR("open %s failed:%s\n", filePath, strerror(errno));
				return -1;
			}
		}

		writeRet = write(fd[idx], (void *)data, size);
		if (writeRet != size) 
		{
			AENC_TRACE_ERR("write frame failed(%d,%d)\n", writeRet, size);
			return -1;
		}
	}
	else 
	{
		if (-1 != fd[idx])
		{
			close(fd[idx]);
			fd[idx] = -1;
		}
	}
	
	return 0;
}

// #define __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
static int audio_ai_init()
{
	int s32Ret = 0;
	AI_CONF_S stAiCfg = {1, 1, {0}, {0}};
	AUDIO_CONF_S stAiChnConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_BUTT, 320, 10};
#ifdef __SUPPORT_AUDIO_VQE_HIGH_VERSION__
	stAiCfg.PlatCfg.HsPlat.VqeV2Enable = 1;
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
#ifdef __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
	//ai chn0:AAC编码通道绑定源,采用16K采样率
	stAiChnConf.enRate = XRS_AUDIO_SAMPLE_RATE_16000;
	s32Ret = Ai_Chn_Open(0, AI_CHTYPE_TALK, 0, stAiChnConf);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Open Failed!\n");
		return -1;
	}

	//ai chn1:G711编码通道绑定源,采用8K采样率
	stAiChnConf.enRate = XRS_AUDIO_SAMPLE_RATE_8000;
	s32Ret = Ai_Chn_Open(0, AI_CHTYPE_TALK, 1, stAiChnConf);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Open Failed!\n");
		return -1;
	}
#else
	s32Ret = Ai_Chn_Open(0, AI_CHTYPE_TALK, 0, stAiChnConf);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Open Failed!\n");
		return -1;
	}
#endif

	s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, 95);
	if (s32Ret < 0)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Set_Volume Failed!\n");
		return -1;
	}

	return 0;
}

static int audio_aenc_init(AUDIO_TYPE_E audioType)
{
	int s32Ret = 0;
	
	s32Ret = Aenc_Open(AENC_CHN_MAX, (VData_Callback)_Audio_Get_DataFunc, NULL);
	if (s32Ret)
	{
		AENC_TRACE_ERR("Aenc_Open failed with %#x\n", s32Ret);
		return -1;
	}

	s32Ret = Aenc_Start(0, 0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Start Failed!\n");
		return -1;
	}

#ifdef __SUPPORT_FAAC__
//ai chn0 采样率为16k，不需要再重采样
#ifndef __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
	if (AUDIO_AAC == audioType) {
		Aenc_Chn_Set_Resample(0, 0, XRS_TRUE, XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_SAMPLE_RATE_16000);
	}
#endif
#endif
	s32Ret = Aenc_Chn_Open(0, 0, audioType, 320);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Open(0, 1, AUDIO_G711U, 320);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Open(0, 2, AUDIO_G711A, 320);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}
	
	return 0;
}


static int audio_adec_init(AUDIO_TYPE_E audioType)
{
	int s32Ret = 0;
	ADEC_CHN_CONF_S stAdecConf = {0};
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	s32Ret = Adec_Open(0, 1);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("Ao_Open failed with %#x\n", s32Ret);
		return -1;
	}

	s32Ret = Adec_Start(0, 0);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("[AUDIO] Ao_Start Failed!\n");
		return -1;
	}

	stAdecConf.AudioType = audioType;
	stAdecConf.AdecSMode = ADEC_SMODE_FRAME;
	s32Ret = Adec_Chn_Open(0, 0, stAdecConf);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("[AUDIO] Adec_Chn_Open Failed!\n");
		return -1;
	}
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);

	return 0;
}


static int audio_ao_init()
{
	int s32Ret = 0;
	AUDIO_CONF_S stAoConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_3, 320, 10};
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
#ifdef __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
	stAoConf.enRate = XRS_AUDIO_SAMPLE_RATE_16000;
#endif
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
#ifdef __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
	s32Ret = Ao_Chn_Set_Resample(0, 0, 1, XRS_AUDIO_SAMPLE_RATE_8000);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Set_Volume Failed!\n");
		return -1;
	}
#endif
	//开启喇叭输出
	HalMod_Set_SpeakerStat(0, 1);
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}

static int audio_ai_unInit()
{
	int s32Ret = 0;
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
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
	AI_TRACE_ERR("[AUDIO] %s !\n", __FUNCTION__);
	return 0;
}


static int audio_ao_unInit()
{
	int s32Ret = 0;
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
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
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}


static int audio_adec_unInit()
{
	int s32Ret = 0;
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	s32Ret = Adec_Chn_Close(0, 0);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("[AUDIO] Adec_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Adec_Stop(0);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("[AUDIO] Adec_Stop Failed!\n");
		return -1;
	}

	s32Ret = Adec_Close(0);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("[AUDIO] Adec_Close Failed!\n");
		return -1;
	}
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}


static int audio_aenc_unInit()
{
	int s32Ret = 0;
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	s32Ret = Aenc_Chn_Close(0, 0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Close(0, 1);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Close(0, 2);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Stop(0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Stop Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Close(0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Close Failed!\n");
		return -1;
	}
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}

static int audio_bind()
{
	int s32Ret = 0;
	AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};
	AENC_BIND_CONF_S stAeBindConf0 = {0, AUDIO_DEV_AI_TALK, 0, 0};
//g711编码通道绑定源为ai chn1;aac编码通道绑定源为ai chn0
#ifdef __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
	AENC_BIND_CONF_S stAeBindConf1 = {1, AUDIO_DEV_AI_TALK, 1, 0};
	AENC_BIND_CONF_S stAeBindConf2 = {1, AUDIO_DEV_AI_TALK, 2, 0};
#else
	AENC_BIND_CONF_S stAeBindConf1 = {0, AUDIO_DEV_AI_TALK, 1, 0};
	AENC_BIND_CONF_S stAeBindConf2 = {0, AUDIO_DEV_AI_TALK, 2, 0};
#endif
	s32Ret = Ao_Chn_Bind(0, stAoBindConf);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_Bind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Bind(0, stAeBindConf0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Bind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Bind(0, stAeBindConf1);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Bind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Bind(0, stAeBindConf2);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Bind Failed!\n");
		return -1;
	}

	return 0;
}

static int audio_unbind()
{
	int s32Ret = 0;
	AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};
	AENC_BIND_CONF_S stAeBindConf0 = {0, AUDIO_DEV_AI_TALK, 0, 0};
	AENC_BIND_CONF_S stAeBindConf1 = {0, AUDIO_DEV_AI_TALK, 1, 0};
	AENC_BIND_CONF_S stAeBindConf2 = {0, AUDIO_DEV_AI_TALK, 2, 0};

	s32Ret = Ao_Chn_UnBind(0, stAoBindConf);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_UnBind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_UnBind(0, stAeBindConf0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_UnBind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_UnBind(0, stAeBindConf1);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_UnBind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_UnBind(0, stAeBindConf2);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_UnBind Failed!\n");
		return -1;
	}

	return 0;
}

static int audio_init(AUDIO_TYPE_E aencType, AUDIO_TYPE_E adecType)
{
	int s32Ret = 0;

	s32Ret = audio_ai_init();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_ai_init Failed!\n");
		return -1;
	}

	s32Ret = audio_aenc_init(aencType);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_aenc_init Failed!\n");
		return -1;
	}

	s32Ret = audio_adec_init(adecType);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_adec_init Failed!\n");
		return -1;
	}

	s32Ret = audio_ao_init();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_ao_init Failed!\n");
		return -1;
	}

	s32Ret = audio_bind();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_bind Failed!\n");
		return -1;
	}

	return 0;
}


static int audio_unInit()
{
	int s32Ret = 0;

	s32Ret = audio_unbind();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_unbind Failed!\n");
		return -1;
	}

	s32Ret = audio_ao_unInit();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_ao_unInit Failed!\n");
		return -1;
	}

	s32Ret = audio_adec_unInit();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_adec_unInit Failed!\n");
		return -1;
	}

	s32Ret = audio_aenc_unInit();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_aenc_unInit Failed!\n");
		return -1;
	}

	s32Ret = audio_ai_unInit();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] audio_ai_unInit Failed!\n");
		return -1;
	}


	return 0;
}

static int audio_volume_test()
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	XRS_BOOL mute = XRS_FALSE; 
	int Volume = 0;

	while(1)
	{
        printf("volume test!: \n"
            "\t1 - ai volume test\n"
            "\t2 - ao volume test\n"
            "\t3 - ao mute test\n"
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
				ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, nCmd);
				if (ret)
				{
					AO_TRACE_ERR("[AUDIO] Ai_Set_Volume Volume(%d) Failed!\n", nCmd);
				}

				Ai_Get_Volume(0, AUDIO_DEV_AI_TALK, 0, &Volume);
				AO_TRACE_ERR("[AUDIO] Ai_Get_Volume Volume(%d) \n", Volume);
				break;
			case 2:
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
			case 3:
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
			default:
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	return 0;
}

static int audio_gain_test()
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	int gain = 0;

	while(1)
	{
        printf("gain test!: \n"
            "\t1 - ai gain test\n"
            "\t2 - ao gain test\n"
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
				printf("enter gain: \n");
				_Test_Get_Cmd(sCmd);
				nCmd = atoi(sCmd);
				ret = Ai_Set_Gain(0, AUDIO_DEV_AI_TALK, 0, nCmd);
				if (ret)
				{
					AO_TRACE_ERR("[AUDIO] Ai_Set_Gain gain(%d) Failed!\n", nCmd);
				}

				Ai_Get_Gain(0, AUDIO_DEV_AI_TALK, 0, &gain);
				AO_TRACE_ERR("[AUDIO] Ai_Get_Gain gain(%d) \n", gain);
				break;
			case 2:
				printf("enter gain: \n");
				_Test_Get_Cmd(sCmd);
				nCmd = atoi(sCmd);
				ret = Ao_Set_Gain(0, AO_DEV_CVBS, nCmd);
				if (ret)
				{
					AO_TRACE_ERR("[AUDIO] Ao_Set_Volume gain(%d) Failed!\n", nCmd);
				}

				Ao_Get_Gain(0, AO_DEV_CVBS, &gain);
				AO_TRACE_ERR("[AUDIO] Ao_Get_Gain gain(%d) \n", gain);
				break;
			default:
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	return 0;
}

static int audio_resample_test()
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	XRS_BOOL enable = 0;
	AUDIO_SAMPLE_RATE_E InputRate = XRS_AUDIO_SAMPLE_RATE_8000;

    printf("resample enable: 0-disable;1-enable\n");
	_Test_Get_Cmd(sCmd);
	enable = (XRS_BOOL)atoi(sCmd);

	if (enable)
	{
	    printf("resample InputRate!: \n"
	        "\t0 - 8000\n"
	        "\t1 - 12000\n"
	        "\t2 - 11028\n"
	        "\t3 - 16000\n"
	        "\t4 - 22050\n"
	        "\t5 - 24000\n"
	        "\t6 - 32000\n"
	        "\t7 - 44100\n"
	        "\t8 - 48000\n"
	        "\t9 - 64000\n"
	        "\t10 - 96000\n");
		_Test_Get_Cmd(sCmd);
		
		nCmd = atoi(sCmd);
		switch (nCmd)
		{
			case 0:
				InputRate = XRS_AUDIO_SAMPLE_RATE_8000;
				break;
			case 1:
				InputRate = XRS_AUDIO_SAMPLE_RATE_12000;
				break;
			case 2:
				InputRate = XRS_AUDIO_SAMPLE_RATE_11025;
				break;
			case 3:
				InputRate = XRS_AUDIO_SAMPLE_RATE_16000;
				break;
			case 4:
				InputRate = XRS_AUDIO_SAMPLE_RATE_22050;
				break;
			case 5:
				InputRate = XRS_AUDIO_SAMPLE_RATE_24000;
				break;
			case 6:
				InputRate = XRS_AUDIO_SAMPLE_RATE_32000;
				break;
			case 7:
				InputRate = XRS_AUDIO_SAMPLE_RATE_44100;
				break;
			case 8:
				InputRate = XRS_AUDIO_SAMPLE_RATE_48000;
				break;
			case 9:
				InputRate = XRS_AUDIO_SAMPLE_RATE_64000;
				break;
			case 10:
				InputRate = XRS_AUDIO_SAMPLE_RATE_96000;
				break;
			default:
				AO_TRACE_ERR("unsupported type, %d\n", nCmd);
				return -1;
				
		}
	}
	
	ret = Ao_Chn_Set_Resample(0, 0, enable, InputRate);
	if (ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_Set_Resample enable(%d) InputRate(%d),Failed!\n", enable, InputRate);
	}
		
	return 0;
}

static int audio_aec_test()
{
	int ret = 0;
	char sCmd[8] = {0};
	XRS_BOOL enable = 0;

    printf("aec enable: 0-disable;1-enable\n");
	_Test_Get_Cmd(sCmd);
	enable = (XRS_BOOL)atoi(sCmd);
	
	ret = Ai_Chn_Set_Aec(0, 0, enable);
	if (ret)
	{
		AO_TRACE_ERR("[AUDIO] Ai_Chn_Set_Aec enable(%d) Failed!\n", enable);
	}
		
	return 0;
}

static int audio_encode_file_save()
{
	char sCmd[8] = {0};
	int nCmd = 0;
	char sCommond[32] = {0};

	while(1)
	{
        printf("aenc file save test!: %s\n"
            "\t1 - start aenc file out\n"
            "\t2 - stop aenc file out\n"
            "\tq - quit\n", AENC_OUT_FILE);

		_Test_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			s_AencFileSave = 0;
			break;
		}
		nCmd = atoi(sCmd);
		AO_TRACE_INFO("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				if (0 == access(AENC_OUT_FILE, 0))
				{
					snprintf(sCommond, sizeof(sCommond), "rm %s -f", AENC_OUT_FILE);
					system(sCommond);
				}
				
				s_AencFileSave = 1;
				printf("please speech\n");
				break;
			case 2:
				s_AencFileSave = 0;
				break;
			default:
				s_AencFileSave = 0;
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	return 0;
}

#define ADEC_IN_FILE "./aenc_record.g711"
#define ADEC_TEST_BUF_SIZE (320)

static int audio_decode_file()
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int size = 0;
	int ret = 0;
	int fileSize = 0;
	ADEC_WRITE_FINISH_E wrtFin;
	
	buf = (unsigned char *)malloc(10*ADEC_TEST_BUF_SIZE);
	if (NULL == buf) 
	{
		ADEC_TRACE_ERR("malloc size(%d) Failed!\n", ADEC_TEST_BUF_SIZE);
		return -1;
	}
	
	fp = fopen(ADEC_IN_FILE, "rb");
	if (NULL == fp)
	{
		ADEC_TRACE_ERR("fopen %s Failed!\n", ADEC_IN_FILE);
		free(buf);
		return -1;
	}

    fseek(fp,0,SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

	while(1)
	{
		size = fread(buf, 1, ADEC_TEST_BUF_SIZE, fp);
		if (size != ADEC_TEST_BUF_SIZE)
		{
			ADEC_TRACE_ERR("adec dec over!\n");
			break;
		}

		fileSize -= size;
		wrtFin = (fileSize < ADEC_TEST_BUF_SIZE) ? ADEC_WRITE_END : ADEC_WRITE_ING;

		ret = Adec_Chn_Write(0, 0, buf, size, ADEC_WRITE_BLOCKING, wrtFin);
		if (ret)
		{
			ADEC_TRACE_ERR("Adec_Chn_Write fail, exit!\n");
			break;
		}

	}

	fclose(fp);
	free(buf);
	
	return 0;
}

#define AO_IN_FILE "./far.pcm"
#define AO_READ_BUF_SIZE (640)
static int audio_read_file_to_ao()
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int size = 0;
	int ret = 0;
	int fileSize = 0;
	ADEC_WRITE_FINISH_E wrtFin;
	
	buf = (unsigned char *)malloc(10*ADEC_TEST_BUF_SIZE);
	if (NULL == buf) 
	{
		ADEC_TRACE_ERR("malloc size(%d) Failed!\n", ADEC_TEST_BUF_SIZE);
		return -1;
	}
	
	fp = fopen(ADEC_IN_FILE, "rb");
	if (NULL == fp)
	{
		ADEC_TRACE_ERR("fopen %s Failed!\n", ADEC_IN_FILE);
		free(buf);
		return -1;
	}

    fseek(fp,0,SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

	while(1)
	{
		size = fread(buf, 1, 640, fp);
		if (size != 640)
		{
			ADEC_TRACE_ERR("adec dec over!\n");
			break;
		}

		fileSize -= size;
		wrtFin = (fileSize < 640) ? ADEC_WRITE_END : ADEC_WRITE_ING;

		ret = Ao_Chn_Write(0, 0, buf, size, ADEC_WRITE_BLOCKING, wrtFin);
		if (ret)
		{
			ADEC_TRACE_ERR("Adec_Chn_Write fail, exit!\n");
			break;
		}

	}

	fclose(fp);
	free(buf);
	
	return 0;
}

#ifdef __SURPPORT_AMR_DEC__
#define ADEC_AMR_IN_FILE "./amr-test.amr"
#define ADEC_AMR_OUT_FILE "./amr-test.pcm"
static int audio_decode_amr_to_ao()
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int size = 0;
	int ret = 0;
	int fileSize = 0;
	ADEC_WRITE_FINISH_E wrtFin;

	ret = Adec_DecAmr2Pcm(0, (UINT8 *)ADEC_AMR_IN_FILE, (UINT8 *)ADEC_AMR_OUT_FILE);
	if (ret)
	{
		ADEC_TRACE_ERR("Adec_DecAmr2Pcm %#x Failed!\n", ret);
		return -1;
	}

	buf = (unsigned char *)malloc(10*ADEC_TEST_BUF_SIZE);
	if (NULL == buf) 
	{
		ADEC_TRACE_ERR("malloc size(%d) Failed!\n", ADEC_TEST_BUF_SIZE);
		return -1;
	}
	
	fp = fopen(ADEC_AMR_OUT_FILE, "rb");
	if (NULL == fp)
	{
		ADEC_TRACE_ERR("fopen %s Failed!\n", ADEC_AMR_OUT_FILE);
		free(buf);
		return -1;
	}

    fseek(fp,0,SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

	while(1)
	{
		size = fread(buf, 1, 640, fp);
		if (size != 640)
		{
			ADEC_TRACE_ERR("adec dec over!\n");
			break;
		}

		fileSize -= size;
		wrtFin = (fileSize < 640) ? ADEC_WRITE_END : ADEC_WRITE_ING;

		ret = Ao_Chn_Write(0, 0, buf, size, ADEC_WRITE_BLOCKING, wrtFin);
		if (ret)
		{
			ADEC_TRACE_ERR("Adec_Chn_Write fail, exit!\n");
			break;
		}

	}

	fclose(fp);
	free(buf);
	
	return 0;
}
#endif

static int audio_get_aenc_type(AUDIO_TYPE_E *type)
{
	char sCmd[8] = {0};
	int nCmd = 0;
	AUDIO_TYPE_E aencType = AUDIO_G711U;

	//while(1)
	{
        printf("choose audio enc type: default g711u\n"
            "\t1 - g711a\n"
            "\t2 - g711u\n"
            "\t3 - pcm\n"
            "\t4 - aac\n"
			"\tq - quit\n");
		_Test_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			return 0;
		}
		nCmd = atoi(sCmd);
		AO_TRACE_INFO("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				aencType = AUDIO_G711A;
				break;
			case 2:
				aencType = AUDIO_G711U;
				break;
			case 3:
				aencType = AUDIO_PCM;
				break;
			case 4:
				aencType = AUDIO_AAC;
				break;	
			default:
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	*type = aencType;

	return 0;

}

static int audio_get_adec_type(AUDIO_TYPE_E *type)
{
	char sCmd[8] = {0};
	int nCmd = 0;
	AUDIO_TYPE_E adecType = AUDIO_G711U;

	//while(1)
	{
        printf("choose audio dec type: default g711u\n"
            "\t1 - g711a\n"
            "\t2 - g711u\n"
			"\tq - quit\n");
		_Test_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			return 0;
		}
		nCmd = atoi(sCmd);
		AO_TRACE_INFO("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				adecType = AUDIO_G711A;
				break;
			case 2:
				adecType = AUDIO_G711U;
				break;
			default:
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	*type = adecType;

	return 0;

}


int main(int argc, char *argv[])
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	AUDIO_TYPE_E aencType = AUDIO_G711U;
	AUDIO_TYPE_E adecType = AUDIO_G711U;

//START:
	XTrace_Init(1, 2000, 2000);

	audio_get_aenc_type(&aencType);
	audio_get_adec_type(&adecType);

	ret = audio_init(aencType, adecType);
	if (ret)
	{
		AO_TRACE_ERR("audio_init FAIL(%d)\n",ret);
		return -1;
	}

	while(1)
	{
        printf("audio test!: \n"
            "\t1 - volume set\n"
            "\t2 - ai & aenc test\n"
            "\t3 - adec & ao test\n"
            "\t4 - gain set\n"
            "\t5 - ao resample set\n"
            "\t6 - ai aec set\n"
            "\t7 - decode amr to spk\n"
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
				audio_volume_test();
				break;
			case 2:
				audio_encode_file_save();
				break;
			case 3:
				printf("please ready %s\n", AENC_OUT_FILE);
				audio_decode_file();
				//audio_read_file_to_ao();
				break;
			case 4:
				audio_gain_test();
				break;
			case 5:
				audio_resample_test();
				break;
			case 6:
				audio_aec_test();
				break;
			case 7:
				#ifdef __SURPPORT_AMR_DEC__
				audio_decode_amr_to_ao();
				#endif
				break;
			default:
				AO_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	ret = audio_unInit();
	if (ret)
	{
		AO_TRACE_ERR("audio_unInit FAIL(%d)\n",ret);
		return -1;
	}

//	goto START;
	
	return 0;
}

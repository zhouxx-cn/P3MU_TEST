
#include <stdio.h>
#include "adec.h"
#include "halmod.h"
#include "imp_audio.h"
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


extern SINT32 Adec_Record_Test(int enable);

static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static int adec_init()
{
	int s32Ret = 0;
	ADEC_CHN_CONF_S stAdecConf = {0};
	
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

	stAdecConf.AudioType = AUDIO_G711U;
	stAdecConf.AdecSMode = ADEC_SMODE_FRAME;
	s32Ret = Adec_Chn_Open(0, 0, stAdecConf);
	if (s32Ret)
	{
		ADEC_TRACE_ERR("[AUDIO] Adec_Chn_Open Failed!\n");
		return -1;
	}

	return 0;
}

static int adec_unInit()
{
	int s32Ret = 0;

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

	return 0;
}
#define ADEC_IN_FILE "./aenc_record.g711u"
#define ADEC_TEST_BUF_SIZE (320)

static int adec_dec()
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int size = 0;
	int ret = 0;
	
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

	//内部测试使用
	Adec_Record_Test(1);

	while(1)
	{
		size = fread(buf, 1, ADEC_TEST_BUF_SIZE, fp);
		if (size != ADEC_TEST_BUF_SIZE)
		{
			ADEC_TRACE_ERR("adec dec over!\n");
			break;
		}
		//ADEC_TRACE_INFO("adec read size(%d)!\n",size);

		ret = Adec_Chn_Write(0, 0, buf, size, ADEC_WRITE_BLOCKING, ADEC_WRITE_ING);
		if (ret)
		{
			ADEC_TRACE_ERR("Adec_Chn_Write fail, exit!\n");
			break;
		}

		//ADEC_TRACE_INFO("adec_chn_write end\n");
	}

	fclose(fp);
	free(buf);
	
	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	
	ret = adec_init();
	if (ret)
	{
		ADEC_TRACE_ERR("adec_init FAIL(%d)\n",ret);
		return -1;
	}

	while(1)
	{
        printf("adec test!: \n"
            "\t1 - Adec aenc_record.g711u to ao_play.pcm\n"
            "\tq - quit\n");

		_Test_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			ADEC_TRACE_ERR("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		ADEC_TRACE_ERR("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				adec_dec();
				break;
			default:
				ADEC_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	ret = adec_unInit();
	if (ret)
	{
		ADEC_TRACE_ERR("adec_unInit FAIL(%d)\n",ret);
		return -1;
	}
	return 0;
}




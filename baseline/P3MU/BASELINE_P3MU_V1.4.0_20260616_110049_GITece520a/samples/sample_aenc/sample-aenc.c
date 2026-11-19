
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include "aenc.h"
#include "halmod.h"

#define AENC_OUT_FILE "./aenc_record.g711u"
#define AENC_IN_FILE "./ai_record.pcm"
#define AENC_IN_BUF_SIZE (800)

//内部测试使用
extern SINT32 Aenc_Chn_Write(SINT32 handle, SINT32 AencChn, UINT8 *pData, UINT32 size);


static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static SINT32 _Audio_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, char *data, SINT32 size)
{
	static int fd[2] = {-1, -1};
	char filePath[64] = {0};
	int writeRet = 0;

	//AENC_TRACE_INFO("[AUDIO] stype=%d,idx=%d,size=%d!\n", stype, idx, size);

	if (-1 == fd[idx])
	{
		memset(filePath, 0, sizeof(filePath));
		snprintf(filePath, sizeof(filePath), "%s", AENC_OUT_FILE);
	
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
	
	return 0;
}


static int aenc_init()
{
	int s32Ret = 0;
	
	s32Ret = Aenc_Open(2, (VData_Callback)_Audio_Get_DataFunc, NULL);
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

	s32Ret = Aenc_Chn_Open(0, 0, AUDIO_G711U);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}

	return 0;
}

static int aenc_unInit()
{
	int s32Ret = 0;
	AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close start!\n");
	s32Ret = Aenc_Chn_Close(0, 0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close Failed!\n");
		return -1;
	}
	AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close end!\n");
	s32Ret = Aenc_Stop(0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Stop Failed!\n");
		return -1;
	}
	AENC_TRACE_ERR("[AUDIO] Aenc_Stop end!\n");
	s32Ret = Aenc_Close(0);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Close Failed!\n");
		return -1;
	}
	AENC_TRACE_ERR("[AUDIO] Aenc_Close end!\n");
	return 0;
}

static int aenc_enc()
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int size = 0;
	int ret = 0;
	
	buf = (unsigned char *)malloc(10*AENC_IN_BUF_SIZE);
	if (NULL == buf) 
	{
		AENC_TRACE_ERR("malloc size(%d) Failed!\n", AENC_IN_BUF_SIZE);
		return -1;
	}
	
	fp = fopen(AENC_IN_FILE, "rb");
	if (NULL == fp)
	{
		AENC_TRACE_ERR("fopen %s Failed!\n", AENC_IN_FILE);
		free(buf);
		return -1;
	}

	while(1)
	{
		size = fread(buf, 1, AENC_IN_BUF_SIZE, fp);
		if (size != AENC_IN_BUF_SIZE)
		{
			AENC_TRACE_ERR("adec play over!\n");
			break;
		}
		//AENC_TRACE_ERR("aenc read size(%d)!\n",size);

		ret = Aenc_Chn_Write(0, 0, buf, size);
		if (ret)
		{
			AENC_TRACE_ERR("Ao_Chn_Write fail, exit!\n");
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
		
	ret = aenc_init();
	if (ret)
	{
		AENC_TRACE_ERR("aenc_init FAIL(%d)\n",ret);
		return -1;
	}

	while(1)
	{
        printf("aenc test!: \n"
            "\t1 - aenc ai_record.pcm to aenc_record.g711u\n"
            "\tq - quit\n");

		_Test_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			AENC_TRACE_INFO("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		AENC_TRACE_INFO("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				aenc_enc();
				break;
			default:
				AENC_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	ret = aenc_unInit();
	if (ret)
	{
		AENC_TRACE_ERR("adec_unInit FAIL(%d)\n",ret);
		return -1;
	}
	return 0;
}

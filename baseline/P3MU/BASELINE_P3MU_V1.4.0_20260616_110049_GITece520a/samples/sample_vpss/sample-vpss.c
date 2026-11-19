
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>

#include "vi.h"
#include "visp.h"
#include "vpss.h"
#include "venc.h"

#define FS_CHN_NUM_MAX		(3)
#define FS_MAIN_CHN_WIDTH	(2880)
#define FS_MAIN_CHN_HEIGHT	(1624)
#define FS_SUB_CHN_WIDTH	(800)
#define FS_SUB_CHN_HEIGHT	(448)
#if defined(BOARD_T4) || defined(BOARD_T4P) || defined(BOARD_T4S)
#define FS_AI_CHN_WIDTH		(640)//(1024)
#define FS_AI_CHN_HEIGHT	(384)//(576)
#else
#define FS_AI_CHN_WIDTH		(1920)
#define FS_AI_CHN_HEIGHT	(1088)
#endif
#define FS_MAIN_CHN_FPS		(30)
#define FS_SUB_CHN_FPS		(30)
#define FS_AI_CHN_FPS		(15)

int Sample_Vpss_InitVi()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S chipConf = {0};
	VI_CONF_S viConf = {0};

	ret = Vi_Open(1, chipConf, viConf);
	if (ret)
	{
		VPSS_TRACE_ERR("Vi_Open FAIL(%d)\n", ret);
		return -1;
	}
	
	ret = Vi_Start(0, 0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vi_Start FAIL(%d)\n", ret);
		return -1;
	}

	return 0;
}

int Sample_Vpss_DeInitVi()
{
	int ret = 0;

	ret = Vi_Stop(0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vi_Stop FAIL(%d)\n", ret);
		return -1;
	}

	ret = Vi_Close(0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vi_Open FAIL(%d)\n", ret);
		return -1;
	}

	return 0;
}

int Sample_Vpss_InitVisp()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0};
	VISP_CONTROL_CALLBACK_S ControlCb = {0};
	ISP_PLAT_CFG_S platCfg = {0};

	platCfg.JzPlat.IspOsdSize = 1.5 * 1024 * 1024;
	platCfg.JzPlat.IpuOsdSize = 256 * 1024;
	platCfg.JzPlat.MjpegSize = 768 * 1024;
	platCfg.JzPlat.SnrDefMode = 0;
	ret = Visp_Set_PlatCfgInfo(0, &platCfg);
	if (ret)
	{
		VPSS_TRACE_ERR("Visp_Set_PlatCfgInfo FAIL(%d)\n", ret);
		return -1;
	}

	ret = Visp_Open(1, ChipConf, ControlCb);
	if (ret)
	{
		VPSS_TRACE_ERR("Visp_Open FAIL(%d)\n", ret);
		return -1;
	}

	return 0;
}

int Sample_Vpss_DeInitVisp()
{
	int ret = 0;

	ret = Visp_Close(0);
	if (ret)
	{
		VPSS_TRACE_ERR("Visp_Close FAIL(%d)\n", ret);
		return -1;
	}
	
	return 0;
}

static MV_VPSS_CHN_ATTR_S s_FsChnAttr[FS_CHN_NUM_MAX] = {0};

static int Sample_Vpss_InitVpssChnAttr(int chn)
{
	if (0 == chn)
	{
		s_FsChnAttr[0].width = FS_MAIN_CHN_WIDTH;
		s_FsChnAttr[0].height = FS_MAIN_CHN_HEIGHT;
		s_FsChnAttr[0].OutFrmRate = 30;
		s_FsChnAttr[0].Depth = 2;
		s_FsChnAttr[0].Rotation = 0;//旋转后encode宽高可能要调换，否则encode帧率低，图像等未知，未测试
	}
	else if (1 == chn)
	{
		s_FsChnAttr[1].width = FS_SUB_CHN_WIDTH;
		s_FsChnAttr[1].height = FS_SUB_CHN_HEIGHT;
		s_FsChnAttr[1].OutFrmRate = 30;
		s_FsChnAttr[1].Depth = 3;
		s_FsChnAttr[1].Rotation = 0;
	}
	else if (2 == chn)
	{
		s_FsChnAttr[2].width = FS_AI_CHN_WIDTH;
		s_FsChnAttr[2].height = FS_AI_CHN_HEIGHT;
		s_FsChnAttr[2].OutFrmRate = 15;
		s_FsChnAttr[2].Depth = 1;
		s_FsChnAttr[2].Rotation = 0;
	}
	
	return 0;
}

int Sample_Vpss_InitVpss()
{
	int ret = 0;
	int vpssChn = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0};
	MV_VPSS_CHN_ATTR_S ChnAttr = {0};
	int i = 0;

	ret = Vpss_Open(0,  ChipConf);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Vpss_Start(0,  0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Start FAIL(%d)\n",ret);
		return -1;
	}
	
	for (i = 0; i < FS_CHN_NUM_MAX; i++ )
	{
		Sample_Vpss_InitVpssChnAttr(i);
		vpssChn = i;
		ChnAttr.width = s_FsChnAttr[i].width;
		ChnAttr.height = s_FsChnAttr[i].height;
		ChnAttr.OutFrmRate = s_FsChnAttr[i].OutFrmRate;
		ChnAttr.Depth = s_FsChnAttr[i].Depth;
		ChnAttr.Rotation = s_FsChnAttr[i].Rotation;
		ret = Vpss_Chn_Open(0, vpssChn, ChnAttr);
		if (ret)
		{
			VPSS_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
			return -1;
		}
	}

	return 0;
}

int Sample_Vpss_DeInitVpss()
{
	int ret = 0;
	int i = 0;

	for (i = 0; i < FS_CHN_NUM_MAX; i++)
	{
		ret = Vpss_Chn_Close(0, i);
		if (ret)
		{
			VPSS_TRACE_ERR("Vpss_Chn_Close FAIL(%d)\n",ret);
			return -1;
		}
	}

	ret = Vpss_Stop(0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Stop FAIL(%d)\n",ret);
		return -1;
	}

	ret = Vpss_Close(0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Close FAIL(%d)\n",ret);
		return -1;
	}

	return 0;
}

int Sample_Vpss_StreamOn()
{
	int ret = 0;
	int i = 0;

	for (i = 0; i < FS_CHN_NUM_MAX; i++) 
	{
		ret = Vpss_Chn_Enable(0, i);
		if (ret)
		{
			VPSS_TRACE_ERR("Vpss_Chn_Enable FAIL(%d)\n",ret);
			return -1;
		}

	}

	return 0;
}

int Sample_Vpss_StreamOff()
{
	int ret = 0;
	int i = 0;

	for (i = 0; i < FS_CHN_NUM_MAX; i++) 
	{	
		ret = Vpss_Chn_Disable(0, i);
		if (ret)
		{
			VPSS_TRACE_ERR("Vpss_Chn_Disable FAIL(%d)\n",ret);
			return -1;
		}
	}

	return 0;
}


int Sample_Vpss_ChangeResolution(int vpssChn, int width, int height)
{
	int ret = 0;
	MV_VPSS_CHN_ATTR_S ChnAttr = {0};

	ret = Vpss_Chn_Get_Attr(0, vpssChn, &ChnAttr);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Chn_Enable FAIL(%d)\n",ret);
		return -1;
	}

	VPSS_TRACE_ERR("Vpss_Chn_Get_Attr before size(%d*%d)\n",ChnAttr.width,ChnAttr.height);

	ChnAttr.width = width;
	ChnAttr.height = height;
	
	ret = Vpss_Chn_Set_Attr(0, vpssChn, ChnAttr);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Chn_Enable FAIL(%d)\n",ret);
		return -1;
	}

	ret = Vpss_Chn_Get_Attr(0, vpssChn, &ChnAttr);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Chn_Enable FAIL(%d)\n",ret);
		return -1;
	}

	VPSS_TRACE_ERR("Vpss_Chn_Get_Attr after size(%d*%d)\n",ChnAttr.width,ChnAttr.height);
	return 0;
}

int Sample_Vpss_GetVpssFrame(int vpssChn)
{
	int ret = 0;
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	char frameFilename[64] = {0};
	int fd = -1;

	ret = Vpss_Chn_Get_Frm(0, vpssChn, &VFrmAlg, 1000);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
		return -1;
	}

	VPSS_TRACE_ERR("get frame(chn%d %d*%d)\n", 0, VFrmAlg.u32Width, VFrmAlg.u32Height);

	//save frame
	memset(frameFilename, 0, sizeof(frameFilename));
	snprintf(frameFilename,sizeof(frameFilename),"/tmp/chn%d-w%d-h%d.nv12", vpssChn, VFrmAlg.u32Width, VFrmAlg.u32Height);

    fd = open(frameFilename, O_RDWR | O_CREAT, 0x644);
    if (fd < 0)
	{
        VPSS_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
		return -1;
    }
	
	if (1)//(2 != vpssChn)
	{
		if (write(fd, (void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32FrameSize) != VFrmAlg.u32FrameSize) 
		{
			VPSS_TRACE_ERR("chnNum=%d write frame failed\n", 0);
			return -1;
		}
	}
	else
	{
		//叠加马赛克，以IVS方式获取nv12数据需要16对齐偏移
		write(fd,(void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32Width * VFrmAlg.u32Height);
		write(fd,(void *)VFrmAlg.u64VirAddr[0] + VFrmAlg.u32Width * ((VFrmAlg.u32Height + 15) & ~15), VFrmAlg.u32Width * VFrmAlg.u32Height / 2);
	}
	
	Vpss_Chn_Rel_Frm(0, vpssChn, &VFrmAlg);
	close(fd);
	fd = -1;
	VPSS_TRACE_ERR("Sample_Vpss_GetVpssFrame end\n");
	return 0;
}

int Sample_Vpss_InitModule()
{
	int ret = 0;

	ret = Sample_Vpss_InitVi();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_InitVi FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vpss_InitVisp();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_InitVisp FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vpss_InitVpss();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_InitVisp FAIL(%d)\n", ret);
		return -1;
	}

	return ret;
}

int Sample_Vpss_DeInitModule()
{
	int ret = 0;

	ret = Sample_Vpss_DeInitVi();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_DeInitVi FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vpss_DeInitVisp();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_DeInitVisp FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vpss_DeInitVpss();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_DeInitVpss FAIL(%d)\n", ret);
		return -1;
	}

	return ret;
}

static void _Vpss_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

int Sample_MultiProcessGetFrame()
{
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	VIDEO_CHIP_CONF_S ChipConf = {0};
	int vpssChn = 2;
	int ret = 0;
	int stream_length = 0;
	int sq = 0;
	char command[64] = {0};
	char *yuvAddr = NULL;
	char *outAddr = NULL;
	VENC_YUV_INFO_S YuvInfo = {0};

	
	ret = Vpss_Open(0,	ChipConf);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
		return -1;
	}
//	yuvAddr = Venc_EncoderMemMalloc_MultiProc(FS_AI_CHN_WIDTH*FS_AI_CHN_HEIGHT*3/2, 256);
//	if (NULL == yuvAddr)
//	{
//		VPSS_TRACE_ERR("IMP_Encoder_VbmAlloc_Ex FAIL(%d)\n",ret);
//		return -1;
//	}
	outAddr = Venc_EncoderMemMalloc_MultiProc(256*1024, 256);
	if (NULL == outAddr)
	{
		VPSS_TRACE_ERR("IMP_Encoder_VbmAlloc_Ex FAIL(%d)\n",ret);
		return -1;
	}
	getchar();
	
	while (1)
	{
		ret = Vpss_Chn_Get_Frm_Ex(0, vpssChn, &VFrmAlg, 1000);
		if (ret)
		{
			VPSS_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
			return -1;
		}

		VPSS_TRACE_ERR("get frame(chn%d %d*%d), size=%d\n", vpssChn, VFrmAlg.u32Width, VFrmAlg.u32Height, VFrmAlg.u32FrameSize);
		VPSS_TRACE_ERR("get frame vir=0x%llx,phy=0x%llx,\n",  VFrmAlg.u64VirAddr[0],VFrmAlg.u64PhyAddr[0]);
		VPSS_TRACE_ERR("get frame phy->vir=0x%x\n",Venc_EncoderMemPhysToVirt((intptr_t)VFrmAlg.u64PhyAddr[0]));
//		memcpy(yuvAddr, (char*)VFrmAlg.u64VirAddr[0], VFrmAlg.u32FrameSize);
		YuvInfo.pData = Venc_EncoderMemPhysToVirt((intptr_t)VFrmAlg.u64PhyAddr[0]);
		YuvInfo.u32Width = VFrmAlg.u32Width;
		YuvInfo.u32Height = VFrmAlg.u32Height;
		YuvInfo.u32Size = VFrmAlg.u32FrameSize;
		
		ret = Venc_FrameEncToJpeg_MultiProc(0, YuvInfo, outAddr, 1024*1024, &stream_length, 25);
		if (ret < 0) 
		{
			printf("IMP_Encoder_InputJpege_Ex failed\n");
		} 
		else 
		{
			printf("stream_length %d\n", stream_length);
		}
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "/mnt/t41/outJpg-%d.jpg", sq);
		FILE *outFile = fopen(command, "wb");
		if (outFile)
		{
			fwrite(outAddr, 1, stream_length, outFile);
			fclose(outFile);
		}
		Vpss_Chn_Rel_Frm_Ex(0, vpssChn, &VFrmAlg);
		VPSS_TRACE_ERR("Sample_Vpss_GetVpssFrame end\n");
		usleep(1000);
	}

	ret = Vpss_Close(0);
	if (ret)
	{
		VPSS_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;

#ifdef MULTI_PROCESS_GET_FRAME_AND_ENC_JPG_TEST
	Sample_MultiProcessGetFrame();
	return 0;
#endif

	ret = Sample_Vpss_InitModule();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_InitModule FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Sample_Vpss_StreamOn();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_Init FAIL(%d)\n",ret);
		return -1;
	}

	while(1)
	{
        printf("vpss test\n"
            "\t1 - get ai vpss frame in /tmp/\n"
            "\tq - quit\n");

		_Vpss_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		VPSS_TRACE_ERR("nCmd(%d)\n", nCmd);
		
		switch (nCmd)
		{
			case 1:
				Sample_Vpss_GetVpssFrame(2);
				break;
			default:
				VPSS_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	ret = Sample_Vpss_StreamOff();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_Stream_Off FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Sample_Vpss_DeInitModule();
	if (ret)
	{
		VPSS_TRACE_ERR("Sample_Vpss_DeInitModule FAIL(%d)\n",ret);
		return -1;
	}
	
	return 0;
}

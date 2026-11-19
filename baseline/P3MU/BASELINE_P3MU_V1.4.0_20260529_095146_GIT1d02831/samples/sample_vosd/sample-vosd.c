
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>

#include "msstd.h"
#include "vi.h"
#include "visp.h"
#include "vpss.h"
#include "venc.h"
#include "vosd.h"
#include "vda.h"

#include "ao.h"
#include "ai.h"
#include "aenc.h"
#include "adec.h"
#include "halmod.h"

#include "mysystem.h"

#include "turbojpeg.h"
#include <jpeglib.h>

#include "imp_isp.h"
#include "imp_osd.h"

#define SUPPORT_RGB555LE
#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bgramapinfo.h"
#endif
//

#define FS_CHN_NUM_MAX		(3)
#if defined(BOARD_Y44L) || defined(BOARD_T4S) || defined(BOARD_T4PX)|| defined(BOARD_Y43LK)
#define FS_MAIN_CHN_WIDTH	(2560)
#define FS_MAIN_CHN_HEIGHT	(1440)
#elif defined(BOARD_R2)
#define FS_MAIN_CHN_WIDTH	(1920)
#define FS_MAIN_CHN_HEIGHT	(1080)
#elif defined(BOARD_T6) || defined(BOARD_T6X)
#define FS_MAIN_CHN_WIDTH	(3200)
#define FS_MAIN_CHN_HEIGHT	(1800)
#elif defined(BOARD_T8) || defined(BOARD_T8X)
#define FS_MAIN_CHN_WIDTH	(3840)
#define FS_MAIN_CHN_HEIGHT	(2160)
#else
#define FS_MAIN_CHN_WIDTH	(2880)
#define FS_MAIN_CHN_HEIGHT	(1624)
#endif

#if defined(BOARD_Y53L) || defined(BOARD_Y44L) || defined(BOARD_Y43LK)
#define FS_SUB_CHN_WIDTH	(640)
#define FS_SUB_CHN_HEIGHT	(360)
#elif defined(BOARD_T4PX)
#define FS_SUB_CHN_WIDTH	(800)
#define FS_SUB_CHN_HEIGHT	(448)
#else
#define FS_SUB_CHN_WIDTH	(704)
#define FS_SUB_CHN_HEIGHT	(576)
#endif
#if defined(BOARD_T4) || defined(BOARD_T4P) || defined(BOARD_T4S) || defined(BOARD_T4PX)
#define FS_AI_CHN_WIDTH		(640)//(1024)
#define FS_AI_CHN_HEIGHT	(352)//(576)
#elif defined(BOARD_R2) || defined(BOARD_P5A)
#define FS_AI_CHN_WIDTH		(1280)
#define FS_AI_CHN_HEIGHT	(720)
#else
#define FS_AI_CHN_WIDTH		(1920)
#define FS_AI_CHN_HEIGHT	(1088)
#endif
#define FS_MAIN_CHN_FPS		(30)
#define FS_SUB_CHN_FPS		(30)
#define FS_AI_CHN_FPS		(10)

#if defined(BOARD_T4) || defined(BOARD_T4P) || defined(BOARD_T4S) || defined(BOARD_T4PX)
#define FS_SUB_CHN_DEPTH	(1)
#define FS_AI_CHN_DEPTH		(1)
#else
#define FS_SUB_CHN_DEPTH	(3)
#define FS_AI_CHN_DEPTH		(1)
#endif

#if defined(BOARD_Y44L) || defined(BOARD_T4S) || defined(BOARD_T4PX)
#define ENC0_STREAM_BUF_SIZE		(512 * 1024)
#else
#define ENC0_STREAM_BUF_SIZE		(768 * 1024)
#endif

#if defined(BOARD_T5AI) || defined(BOARD_Y53L) || defined(BOARD_T6) || defined(BOARD_T6X)
#define ENC1_STREAM_BUF_SIZE		(512 * 1024)
#else
#define ENC1_STREAM_BUF_SIZE		(128 * 1024)
#endif

#if defined(BOARD_T4S) || defined(BOARD_T4PX) 
#define ISP_OSD_SIZE		(992 * 1024)
#define IPU_OSD_SIZE		(176 * 1024)
#define MJPEG_STREAM_SIZE	(128 * 1024)
#else
#define ISP_OSD_SIZE		(1.5 * 1024 * 1024)
#define IPU_OSD_SIZE		(256 * 1024)
#define MJPEG_STREAM_SIZE	(768 * 1024)
#endif

#define VENC_CHN_NUM_MAX	(3)

#define STREAM_FILE_SAVE_PATH		"/tmp"

static unsigned long long get_ms()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;
}

static unsigned long long get_us()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec * 1000 * 1000 + time_v.tv_usec;
}

static int Sample_Vosd_GetSystemCmdResult(const char *cmd, char *result, int len) 
{  
	struct timeval now;
	char fileName[64] = {0};
	char command2[256] = {0};
	int ret = 0;
	FILE *fp = NULL;

	if((NULL == cmd) 
		|| (0 == strlen(cmd)) 
		|| (len <= 0))
	{
		return -1;
	}

	gettimeofday(&now, NULL);

	bzero(fileName, sizeof(fileName));
	snprintf(fileName, sizeof(fileName), "/tmp/.SysCmdRes%ld", now.tv_usec);

	memset(result, 0, len); 
	bzero(command2, sizeof(command2));
	snprintf(command2, sizeof(command2), "%s > %s", cmd, fileName);

	mysystem(command2);
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		perror("fopen");
		ret = -1;
	}
	else
	{
		fread(result, sizeof(char), len, fp);
		
		if(fclose(fp))
		{
			perror("fclose");
			ret = -1;
		}
	}
	
	bzero(command2, sizeof(command2));
	snprintf(command2, sizeof(command2), "rm -rf %s", fileName);
	mysystem(command2);
	
	return ret; 							   
}


int Sample_Vosd_InitVi()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S chipConf = {0};
	VI_CONF_S viConf = {0};

	ret = Vi_Open(1, chipConf, viConf);
	if (ret)
	{
		VOSD_TRACE_ERR("Vi_Open FAIL(%d)\n", ret);
		return -1;
	}
	
	ret = Vi_Start(0, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vi_Start FAIL(%d)\n", ret);
		return -1;
	}

	return 0;
}

int Sample_Vosd_DeInitVi()
{
	int ret = 0;

	ret = Vi_Stop(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vi_Stop FAIL(%d)\n", ret);
		return -1;
	}

	ret = Vi_Close(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vi_Open FAIL(%d)\n", ret);
		return -1;
	}

	return 0;
}
#define ALIGN(x, a) (((x)+(a)-1) & ~((a)-1))
#define ALIGN2(x, a) (((x)-(a)+1) & ~((a)-1))
int Sample_Vosd_InitVisp()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0};
	VISP_CONTROL_CALLBACK_S ControlCb = {0};
	ISP_PLAT_CFG_S platCfg = {0};
	int nMemline = 400;
	char sMemline[4] = {0};

	//获取memline
	Sample_Vosd_GetSystemCmdResult("cat /usr/etc/load_media | grep ivdc_mem_line | awk -F'line=' '{print $2}'", sMemline, sizeof(sMemline));
	nMemline = atoi(sMemline);
	VOSD_TRACE_INFO("nMemline=%d\n", nMemline);

	platCfg.JzPlat.IspOsdSize = ISP_OSD_SIZE;
	platCfg.JzPlat.IpuOsdSize = IPU_OSD_SIZE;
	platCfg.JzPlat.MjpegSize = MJPEG_STREAM_SIZE;
#if defined(BOARD_T4) || defined(BOARD_T4P) || defined(BOARD_T4PX) || defined(BOARD_T4S)
	platCfg.JzPlat.SnrDefMode = 1;
#else
	platCfg.JzPlat.SnrDefMode = 0;
#endif
//	platCfg.JzPlat.WdrEnable = 1;
	platCfg.JzPlat.Fs0MemSize = ALIGN(FS_MAIN_CHN_WIDTH, 256) * nMemline * 3 /2;
	platCfg.JzPlat.Fs1MemSize = FS_SUB_CHN_DEPTH * ALIGN(FS_SUB_CHN_WIDTH, 16) * ALIGN(FS_SUB_CHN_HEIGHT, 16) * 3 / 2;//1059840;//
	VOSD_TRACE_ERR("platCfg.JzPlat.Fs1MemSize (%d)(%d-%d-%d)\n", platCfg.JzPlat.Fs1MemSize, FS_SUB_CHN_DEPTH, ALIGN(FS_SUB_CHN_WIDTH, 16),ALIGN(FS_SUB_CHN_HEIGHT, 16) );
	platCfg.JzPlat.Fs2MemSize = FS_AI_CHN_DEPTH * FS_AI_CHN_WIDTH * FS_AI_CHN_HEIGHT * 3 / 2;

	platCfg.JzPlat.Enc0StreamSize = ENC0_STREAM_BUF_SIZE;
	platCfg.JzPlat.Enc1StreamSize = ENC1_STREAM_BUF_SIZE;
	ret = Visp_Set_PlatCfgInfo(0, &platCfg);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_PlatMemAllocInfo FAIL(%d)\n", ret);
		return -1;
	}

	ret = Visp_Open(1, ChipConf, ControlCb);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Open FAIL(%d)\n", ret);
		return -1;
	}

	return 0;
}

int Sample_Vosd_DeInitVisp()
{
	int ret = 0;

	ret = Visp_Close(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Close FAIL(%d)\n", ret);
		return -1;
	}
	
	return 0;
}

static MV_VPSS_CHN_ATTR_S s_FsChnAttr[FS_CHN_NUM_MAX] = {0};

static int Sample_Vosd_InitVpssChnAttr(int chn)
{
	if (0 == chn)
	{
		s_FsChnAttr[0].width = FS_MAIN_CHN_WIDTH;
		s_FsChnAttr[0].height = FS_MAIN_CHN_HEIGHT;
		s_FsChnAttr[0].OutFrmRate = 20;
		s_FsChnAttr[0].Depth = 2;
		s_FsChnAttr[0].Rotation = 0;//旋转后encode宽高可能要调换，否则encode帧率低，图像等未知，未测试
	}
	else if (1 == chn)
	{
		s_FsChnAttr[1].width = FS_SUB_CHN_WIDTH;
		s_FsChnAttr[1].height = FS_SUB_CHN_HEIGHT;
		s_FsChnAttr[1].OutFrmRate = 20;
		s_FsChnAttr[1].Depth = FS_SUB_CHN_DEPTH;
		s_FsChnAttr[1].Rotation = 0;
	}
	else if (2 == chn)
	{
		s_FsChnAttr[2].width = FS_AI_CHN_WIDTH;
		s_FsChnAttr[2].height = FS_AI_CHN_HEIGHT;
		s_FsChnAttr[2].OutFrmRate = 15;
		s_FsChnAttr[2].Depth = FS_AI_CHN_DEPTH;
		s_FsChnAttr[2].Rotation = 0;
	}
	return 0;
}

int Sample_Vosd_InitVpss()
{
	int ret = 0;
	int vpssChn = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0};
	MV_VPSS_CHN_ATTR_S ChnAttr = {0};
	int i = 0;

	ret = Vpss_Open(0,  ChipConf);
	if (ret)
	{
		VOSD_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Vpss_Start(0,  0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vpss_Start FAIL(%d)\n",ret);
		return -1;
	}
	
	for (i = 0; i < FS_CHN_NUM_MAX; i++ )
	{
		Sample_Vosd_InitVpssChnAttr(i);
		vpssChn = i;
		ChnAttr.width = s_FsChnAttr[i].width;
		ChnAttr.height = s_FsChnAttr[i].height;
		ChnAttr.OutFrmRate = s_FsChnAttr[i].OutFrmRate;
		ChnAttr.Depth = s_FsChnAttr[i].Depth;
		ChnAttr.Rotation = s_FsChnAttr[i].Rotation;
		ret = Vpss_Chn_Open(0, vpssChn, ChnAttr);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
			return -1;
		}
	}

	return 0;
}

int Sample_Vosd_DeInitVpss()
{
	int ret = 0;
	int i = 0;

	for (i = 0; i < FS_CHN_NUM_MAX; i++)
	{
		ret = Vpss_Chn_Close(0, i);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Chn_Close FAIL(%d)\n",ret);
			return -1;
		}
	}

	ret = Vpss_Stop(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vpss_Stop FAIL(%d)\n",ret);
		return -1;
	}

	ret = Vpss_Close(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vpss_Close FAIL(%d)\n",ret);
		return -1;
	}

	return 0;
}

static int getAiFrameThread(void *arg)
{
	int ret = 0;
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	char frameFilename[128] = {0};
	int fd = -1;
//	VOSD_TRACE_INFO("Sample_Vpss_Get_Frame start\n");
	int cnt = 0;
	int vpssChn = 2;
	int frmrate_sp = 0;
	int statime_sp = 0;
	int saveChg = 0;
	
	while(1)
	{
		ret = Vpss_Chn_Get_Frm(0, vpssChn, &VFrmAlg, 1000);
		if (ret)
		{
			//VOSD_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
			continue;
		}

		VOSD_TRACE_INFO("get frame(chn%d %d*%d) id(%lld),vir(0x%llx),pthy(0x%llx)\n", 
			vpssChn, VFrmAlg.u32Width, VFrmAlg.u32Height, VFrmAlg.u64SeqNum, VFrmAlg.u64VirAddr[0], VFrmAlg.u64PhyAddr[0]);

		memset(frameFilename, 0, sizeof(frameFilename));

		if (1)//(0 == saveChg)
		{
			snprintf(frameFilename,sizeof(frameFilename),"%s/chn%d-w%d-h%d-seq%lld-chg%d.nv12", "/mnt/t41/nv12", 2, VFrmAlg.u32Width, VFrmAlg.u32Height,VFrmAlg.u64SeqNum,saveChg);

		    fd = open(frameFilename, O_RDWR | O_CREAT, 0x644);
		    if (fd < 0)
			{
		        VOSD_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
				return -1;
		    }
			VOSD_TRACE_ERR("write start\n");
			//叠加马赛克，以IVS方式获取nv12数据需要16对齐偏移
			write(fd,(void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32FrameSize);
			VOSD_TRACE_ERR("write end\n");
			saveChg = 1;
		}
		else
		{
			snprintf(frameFilename,sizeof(frameFilename),"%s/chn%d-w%d-h%d-seq%d-chg%d.nv12", "/mnt/t41/nv12", 2, VFrmAlg.u32Width, VFrmAlg.u32Height,cnt,saveChg);
	
			fd = open(frameFilename, O_RDWR | O_CREAT, 0x644);
			if (fd < 0)
			{
				VOSD_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
				return -1;
			}
			
			//叠加马赛克，以IVS方式获取nv12数据需要16对齐偏移
			write(fd,(void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32Width * VFrmAlg.u32Height);
			write(fd,(void *)VFrmAlg.u64VirAddr[0] + VFrmAlg.u32Width * ((VFrmAlg.u32Height + 15) & ~15), VFrmAlg.u32Width * VFrmAlg.u32Height / 2);
			
			saveChg = 0;
		}
		
		close(fd);
		fd = -1;
		//usleep(60*1000);
		Vpss_Chn_Rel_Frm(0, vpssChn, &VFrmAlg);
		///sleep(5);
		cnt++;
	}
	VOSD_TRACE_INFO("Sample_Vpss_Get_Frame end\n");
	return 0;
}

// NV12到TJSAMP_420转换函数
void nv12_to_tjsamp_420_2(unsigned char* nv12Data, int width, int height) {
    int imageSize = width * height;
    int chromaSize = imageSize / 4;

    unsigned char* uvData = (unsigned char*)malloc(imageSize/2);

    // 提取和重新排列UV分量
    for (int i = 0, j = 0; i < chromaSize; i++, j += 2) {
        uvData[i] = nv12Data[imageSize + j];
        uvData[chromaSize + i] = nv12Data[imageSize + j + 1];
    }

	memcpy(nv12Data + imageSize, uvData, imageSize / 2);

	free(uvData);
}

void NV12toI420(unsigned char* pNV12, int s32Width, int s32Height)
{
    int s32LenY = s32Width * s32Height;
    int s32LenU = s32LenY / 4;

    unsigned char* uv = pNV12 + s32LenY;
    unsigned char* v = pNV12 + s32LenY + s32LenU;
    unsigned char* u = uv;
    unsigned char* pUVBottomHalf = (unsigned char*)malloc(s32LenU);

    if (NULL == pUVBottomHalf)
    {
        return;
    }

    // 先备份后半部分UV数据，便于V数据部分可以直接写入到图像本身的内存
    memcpy(pUVBottomHalf, uv + s32LenU, s32LenU);

    int i = 0;
    // 转换前半部分的UV数据
    for (i = 0; i < s32LenU/2; i++)
    {
        u[i] = uv[i * 2];
        v[i] = uv[i * 2+1];
    }

    uv = pUVBottomHalf;
    // 转换后半部分数据
    for (i = 0; i < (s32LenU / 2); i++)
    {
        u[i + (s32LenU / 2)] = uv[i * 2];
        v[i + (s32LenU / 2)] = uv[(i * 2) + 1];
    }

    free(pUVBottomHalf);
}


#if 0
int EncJpgByTB(PVIDEO_FRM_ALG_S pVFrmAlg) 
{
    int width = 640;
    int height = 384;
    int imageSize = width * height;
    int chromaSize = imageSize / 4;
	int ret =0;
    // 读取NV12数据
//    FILE* file = fopen("./chn2-w640-h384-0.nv12", "rb");
//    if (file == NULL) {
//        printf("无法打开文件\n");
//        return 1;
//    }

//    fseek(file, 0, SEEK_END);
//    long fileSize = ftell(file);
//    rewind(file);

//    unsigned char* nv12Data = (unsigned char*)malloc(fileSize);
//    fread(nv12Data, 1, fileSize, file);
//    fclose(file);

    // 转换为YUV420
//    unsigned char* yuv420Data = (unsigned char*)malloc(imageSize + chromaSize * 2);

//	while(1)
	{
		UINT64  startTime = get_ms();
	    //nv12_to_tjsamp_420(nv12Data, yuv420Data, width, height);
	    NV12toI420((unsigned char*)pVFrmAlg->u64VirAddr[0], pVFrmAlg->u32Width, pVFrmAlg->u32Height);
		UINT64  endTime = get_ms();
		printf("nv12_to_tjsamp_420 cost time %lld ms\n", endTime - startTime);

//		usleep(10*1000);
	}
    // 保存YUV420数据到文件（仅用于演示）
//    FILE* outputFile = fopen("output1.yuv", "wb");
//    fwrite(yuv420Data, 1, imageSize + chromaSize * 2, outputFile);
//    fclose(outputFile);
	
	tjhandle tjHandle = NULL;
	unsigned char* jpegBuf = NULL;
	unsigned long  jpegLen = 0;
	int frmHeight = 384;
	int frmWidth = 640;

	tjHandle = tjInitCompress();
	if (NULL != tjHandle)
	{
		ret = tjCompressFromYUV(tjHandle, (unsigned char *)pVFrmAlg->u64VirAddr[0], 
		pVFrmAlg->u32Width, 16, pVFrmAlg->u32Height, TJSAMP_420,
		&jpegBuf, &jpegLen, 100, 0);
		if (0 == ret && 0 < jpegLen && NULL != jpegBuf)
		{
			printf("tb jpg success\n");
			int JpegStream= -1;
			JpegStream = open("tb4.jpeg", O_RDWR | O_CREAT | O_TRUNC, 0644);
			if (jpegLen != write(JpegStream, jpegBuf, jpegLen)) 
			{
				printf("stream write failed\n");
				return -1;
			}
			close(JpegStream);
			tjFree(jpegBuf);
			jpegBuf = NULL;
		}
		else
		{
			printf("tb jpg fail\n");
		}
	}


    // 释放资源
//    free(nv12Data);
//    free(yuv420Data);

    return 0;
}
#endif


int I420toNV12(unsigned char* pI420, int s32Size)
{
	int s32LenY = s32Size;
	int s32LenU = s32LenY / 4;
	int s32LenUV = s32LenY / 2;
	unsigned char* uv = NULL;
	unsigned char* v = pI420 + s32LenY + s32LenU;
	unsigned char* u = pI420 + s32LenY;
	unsigned char* pUVBottomHalf = (unsigned char*)malloc(s32LenUV);
	if (NULL == pUVBottomHalf)
	{
		return;
	}

	uv = pUVBottomHalf;
printf("test-%d\n", __LINE__);
	int i = 0;
	int j = 0;
	for(int i = 0,j = 0;i<s32LenU;i++,j+=2)
	{
//		memcpy(uv+j,u+i,1);//u分量
//		memcpy(uv+j+1,y+i,1);//v分量
		uv[j] = u[i];
		uv[j+1] = v[i];
	}
	printf("test-%d\n", __LINE__);
	int ret = memcmp(pI420 + s32LenY, uv, s32LenUV);
	printf("ret = %d\n", ret);
	memcpy(pI420 + s32LenY, uv, s32LenUV);
	printf("test-%d\n", __LINE__);

	free(pUVBottomHalf);
	
	printf("test-%d\n", __LINE__);
	return 0;

}

void yuvI420ToNV12(char *I420,int width, int height)

{
	int i = 0;
	int j = 0;
	unsigned int yLen = width * height;
	unsigned char* i420u = I420 + width * height;
	unsigned char* i420v = I420 + yLen*5/4;
	unsigned char* pUV = (unsigned char*)malloc(yLen/2);
	if (NULL == pUV)
	{
		return;
	}

	for( i = 0,j = 0;i<yLen/4;i++,j+=2)

	{	
		pUV[j] = i420u[i];
		pUV[j+1] = i420v[i];
	}

	memcpy(I420+yLen,pUV,yLen/2);
	

	free(pUV);
}

int Sample_Vosd_GetVpssFrame(int vpssChn)
{
	int ret = 0;
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	char frameFilename[64] = {0};
	int fd = -1;
	static int cnt = 0;
	char bitmapPath[64] = "./xywh_0_0_480_32";	
	VOSD_OVERYLAY_S osdOverLay;
	
	ret = Vpss_Chn_Get_Frm(0, vpssChn, &VFrmAlg, 1000);
	if (ret)
	{
		VOSD_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
		return -1;
	}

	VOSD_TRACE_ERR("get frame(chn%d %d*%d),size=%d\n", vpssChn, VFrmAlg.u32Width, VFrmAlg.u32Height, VFrmAlg.u32FrameSize);

	//save frame
	memset(frameFilename, 0, sizeof(frameFilename));
	snprintf(frameFilename,sizeof(frameFilename),"%s/chn%d-w%d-h%d-%d.nv12", STREAM_FILE_SAVE_PATH, vpssChn, VFrmAlg.u32Width, VFrmAlg.u32Height, cnt);

    fd = open(frameFilename, O_RDWR | O_CREAT, 0x644);
    if (fd < 0)
	{
        VOSD_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
		Vpss_Chn_Rel_Frm(0, vpssChn, &VFrmAlg);
		return -1;
    }
	
	if (1)//(2 != vpssChn)
	{
		if (write(fd, (void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32FrameSize) != VFrmAlg.u32FrameSize) 
		{
			VOSD_TRACE_ERR("chnNum=%d write frame failed\n", 0);
			Vpss_Chn_Rel_Frm(0, vpssChn, &VFrmAlg);
			return -1;
		}
	}
	else
	{
		//叠加马赛克，以IVS方式获取nv12数据需要16对齐偏移
		write(fd,(void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32Width * VFrmAlg.u32Height);
		write(fd,(void *)VFrmAlg.u64VirAddr[0] + VFrmAlg.u32Width * ((VFrmAlg.u32Height + 15) & ~15), VFrmAlg.u32Width * VFrmAlg.u32Height / 2);
	}

	memset(&osdOverLay, 0, sizeof(VOSD_OVERYLAY_S));
	osdOverLay.Rect.x = 0;
	osdOverLay.Rect.y = 0;
	osdOverLay.Rect.w = 480;
	osdOverLay.Rect.h = 32;
	osdOverLay.pData = (void*)malloc(480 * 32 * 2);
	printf("osdOverLay.pData=%p\n", osdOverLay.pData);
    FILE *osdFile = fopen(bitmapPath, "rb");
    if (osdFile == NULL) 
	{
        printf("fopen src file:%s failed\n", bitmapPath);
    }
	int readLen = fread(osdOverLay.pData, 1, 480 * 32 * 2, osdFile);
	printf("readLen =%d\n", readLen);
	fclose(osdFile);
	osdFile = NULL;

	//EncJpgByTB(&VFrmAlg) ;
//	NV12toI420((unsigned char *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32Width, VFrmAlg.u32Height) ;
	printf("FRAME:u64PhyAddr[0]=0x%llx, u64VirAddr=0x%llx\n", VFrmAlg.u64PhyAddr[0], VFrmAlg.u64VirAddr[0]);
	ret = Vosd_DrawBitMapInNV12(0, &VFrmAlg, &osdOverLay);
	printf("Vosd_DrawBitMapInNV12 ret=0x%x\n", ret);
	printf("FRAME:u64PhyAddr[0]=0x%llx, u64VirAddr=0x%llx\n", VFrmAlg.u64PhyAddr[0], VFrmAlg.u64VirAddr[0]);

    int osdStream = -1;
	osdStream = open("osd.yuv", O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (VFrmAlg.u32FrameSize != write(osdStream, (void *)VFrmAlg.u64VirAddr[0], VFrmAlg.u32FrameSize)) {
		printf("stream write failed\n");
		return -1;
	}
	close(osdStream);
	osdStream = -1;

	free(osdOverLay.pData);
	
	Vpss_Chn_Rel_Frm(0, vpssChn, &VFrmAlg);
	close(fd);
	fd = -1;
	VOSD_TRACE_ERR("Sample_Vpss_GetVpssFrame end\n");
	cnt++;
	return 0;
}

static SINT32 _Video_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, SINT8 *data, SINT32 size)
{
	int ret = 0;
	static int fd[3] = {-1, -1, -1};
	char frameFilename[64] = {0};
	MSSTD_S *pMSSTD = (MSSTD_S *)pHead;
	int writeRet = 0;
	return 0;
	if (MSSTD_ALGORITHM_VIDEO_JPEG_STANDARD == pMSSTD->Algorithm)
	{
		//jpeg图片保存
		//VOSD_TRACE_ERR("stype=%d,encType=%d,size=%d\n", stype, pMSSTD->Algorithm, size);
		memset(frameFilename, 0, sizeof(frameFilename));
		snprintf(frameFilename, sizeof(frameFilename), "%s/chn%d-seq%d.jpeg", STREAM_FILE_SAVE_PATH, stype, pMSSTD->FrmCnt);
		fd[stype] = open(frameFilename, O_RDWR | O_CREAT, 0x644);
		if (fd[stype] < 0)
		{
			VOSD_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
			return -1;
		}

		writeRet = write(fd[stype], (void *)data, size);
		if (writeRet != size) 
		{
			VOSD_TRACE_ERR("write frame failed(%d,%d)\n", writeRet, size);
			return -1;
		}

		close(fd[stype]);
		fd[stype] = -1;
	}
	else
	{	
	
//		return 0;
		//VOSD_TRACE_ERR("stype=%d,encType=%d,size=%d\n", stype, pMSSTD->Algorithm, size);
		//return 0;
		//视频流保存
		if (-1 == fd[stype])
		{
			memset(frameFilename, 0, sizeof(frameFilename));
			if (MSSTD_ALGORITHM_VIDEO_H265_GENERAL == pMSSTD->Algorithm)
			{
				snprintf(frameFilename, sizeof(frameFilename), "%s/chn%d.h265", STREAM_FILE_SAVE_PATH, stype);
			}
			else
			{
				snprintf(frameFilename, sizeof(frameFilename), "%s/chn%d.h264", STREAM_FILE_SAVE_PATH, stype);
			}

		    fd[stype] = open(frameFilename, O_RDWR | O_CREAT, 0x644);
		    if (fd[stype] < 0)
			{
		        VOSD_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
				return -1;
		    }
		}
		//VOSD_TRACE_ERR("frmRate(%d)-sec(%d)-msec(%d)\n",pMSSTD->FrateRate,pMSSTD->TimeSec,pMSSTD->TimeMSec);
		writeRet = write(fd[stype], (void *)data, size);
		if (writeRet != size) 
		{
			VOSD_TRACE_ERR("write frame failed(%d,%d)\n", writeRet, size);
			return -1;
		}

		
	}
	
	return 0;
}

static SINT32 _Video_Get_DataFunc1(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, SINT8 *data, SINT32 size)
{		
	return 0;
}

static int s_mjpegDataCom = 1;
static SINT32 _Video_Get_DataFunc2(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, SINT8 *data, SINT32 size)
{	
//	VOSD_TRACE_ERR("_Video_Get_DataFunc2 data common\n");
	s_mjpegDataCom = 1;
	return 0;
}



static int Sample_Vosd_InitVenc()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0};
	VENC_CONF_S VencConf = {0};

	ret = Venc_Open(1, ChipConf, VencConf, NULL, NULL);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Open FAIL(%d)\n",ret);
		return -1;
	}

	Venc_SetChnCallback(VENC_CHNTYPE_MAIN, _Video_Get_DataFunc);
	Venc_SetChnCallback(VENC_CHNTYPE_MINOR, _Video_Get_DataFunc1);
	Venc_SetChnCallback(VENC_CHNTYPE_SNAP, _Video_Get_DataFunc2);
	
	ret = Venc_Start(0, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Start FAIL(%d)\n",ret);
		return -1;
	}

	return 0;
}

static int Sample_Vosd_DeInitVenc()
{
	int ret = 0;

	ret = Venc_Stop(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Stop FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Venc_Close(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Close FAIL(%d)\n",ret);
		return -1;
	}

	return 0;
}

static VENC_ENCODEPARAM_S s_EncParam[VENC_CHN_NUM_MAX] = {0};
static int Sample_Vosd_InitVencChnAttr(int vencChn)
{
	//一条通路上，venc与fs分辨率需一致
	if (0 == vencChn)
	{
		s_EncParam[vencChn].VPayloadType = VPAYLOAD_TYPE_H264;
		s_EncParam[vencChn].width        = s_FsChnAttr[vencChn].width;
		s_EncParam[vencChn].height       = s_FsChnAttr[vencChn].height;
		s_EncParam[vencChn].BitRate      = 2048;
		s_EncParam[vencChn].MaxBitRate   = s_EncParam[vencChn].BitRate * 2;
		s_EncParam[vencChn].FrameRate    = FS_MAIN_CHN_FPS;
		s_EncParam[vencChn].Quality      = 0;
		s_EncParam[vencChn].KeyInterval  = s_EncParam[vencChn].FrameRate * 4;
		s_EncParam[vencChn].BitRateType  = BITRATE_TYPE_VBR;
		s_EncParam[vencChn].bOffLine     = 0;  
		s_EncParam[vencChn].stVencQpConf.bEnbaleQp = 1;
		s_EncParam[vencChn].stVencQpConf.u32MaxQp = 46;
		s_EncParam[vencChn].stVencQpConf.u32MinQp = 28;
		s_EncParam[vencChn].stVencQpConf.u32MaxIQp = 51;
		s_EncParam[vencChn].stVencQpConf.u32MinIQp = 28;
		s_EncParam[vencChn].SmartEncEnable = 1;
		s_EncParam[vencChn].StreamBufSize = ENC0_STREAM_BUF_SIZE;
	}
	else if (1 == vencChn)
	{
		s_EncParam[vencChn].VPayloadType = VPAYLOAD_TYPE_H264;
		s_EncParam[vencChn].width        = s_FsChnAttr[vencChn].width;
		s_EncParam[vencChn].height       = s_FsChnAttr[vencChn].height;
		s_EncParam[vencChn].BitRate      = 1024;
		s_EncParam[vencChn].MaxBitRate   = s_EncParam[vencChn].BitRate * 4 / 3;
		s_EncParam[vencChn].FrameRate    = FS_SUB_CHN_FPS;
		s_EncParam[vencChn].Quality      = 0;
		s_EncParam[vencChn].KeyInterval  = s_EncParam[vencChn].FrameRate * 4;
		s_EncParam[vencChn].BitRateType  = BITRATE_TYPE_CBR;
		s_EncParam[vencChn].bOffLine     = 0;  
		s_EncParam[vencChn].stVencQpConf.bEnbaleQp = 1;
		s_EncParam[vencChn].stVencQpConf.u32MaxQp = 46;
		s_EncParam[vencChn].stVencQpConf.u32MinQp = 28;
		s_EncParam[vencChn].stVencQpConf.u32MaxIQp = 51;
		s_EncParam[vencChn].stVencQpConf.u32MinIQp = 28;
		s_EncParam[vencChn].SmartEncEnable = 1;
		s_EncParam[vencChn].StreamBufSize = ENC1_STREAM_BUF_SIZE;
	}
	else if (2 == vencChn)
	{
		//MJPEG跟随类型
		#if defined(BOARD_T4) || defined(BOARD_T4P) || defined(BOARD_T4S) || defined(BOARD_T5AI)
		s_EncParam[vencChn].MjpegFollowType = VENC_MJPEG_FOLLOW_MAIN;
		#else
		s_EncParam[vencChn].MjpegFollowType = VENC_MJPEG_FOLLOW_MINOR;
		#endif
		s_EncParam[vencChn].VPayloadType = VPAYLOAD_TYPE_MJPEG;
		if (VENC_MJPEG_FOLLOW_MAIN == s_EncParam[vencChn].MjpegFollowType)	
		{
			s_EncParam[vencChn].width		 = s_EncParam[0].width;
			s_EncParam[vencChn].height		 = s_EncParam[0].height;
		}
		else
		{
			s_EncParam[vencChn].width		 = s_EncParam[1].width;
			s_EncParam[vencChn].height		 = s_EncParam[1].height;
		}

		s_EncParam[vencChn].BitRate      = 2048;
		s_EncParam[vencChn].FrameRate    = 5;
		s_EncParam[vencChn].Quality      = 25;
		s_EncParam[vencChn].KeyInterval  = s_EncParam[vencChn].FrameRate * 2;
		s_EncParam[vencChn].BitRateType  = BITRATE_TYPE_VBR;
		s_EncParam[vencChn].bOffLine     = 0;  
		s_EncParam[vencChn].stVencQpConf.bEnbaleQp = 1;
		s_EncParam[vencChn].stVencQpConf.u32MaxQp = 46;
		s_EncParam[vencChn].stVencQpConf.u32MinQp = 28;
		s_EncParam[vencChn].stVencQpConf.u32MaxIQp = 46;
		s_EncParam[vencChn].stVencQpConf.u32MinIQp = 28;
	}
	
	return 0;
}


static int Sample_Vosd_InitVencChn()
{
	int ret = 0;
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	VENC_BIND_CONF_S BindConf = {0};
	int i = 0;

	for (i = 0; i < VENC_CHN_NUM_MAX; i++)
	{
		Sample_Vosd_InitVencChnAttr(i);
		ChnType = (2 == i) ? VENC_CHNTYPE_SNAP : i;
		ret =  Venc_Chn_Open(0, ChnType, 0, s_EncParam[i], s_EncParam[i].FrameRate);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Open chn(%d) FAIL(%d)\n",ChnType,ret);
			return -1;
		}

		BindConf.ChnType = (2 == i) ? VENC_CHNTYPE_SNAP : (VENC_CHNTYPE_E)i;
		ret = Venc_Chn_Bind(0, 0, BindConf);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Bind chn(%d) FAIL(%d)\n",BindConf.ChnType,ret);
			return -1;
		}
	}

	return 0;
}

static int Sample_Vosd_DeInitVencChn()
{
	int ret = 0;
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	VENC_BIND_CONF_S BindConf = {0};
	int i = 0;

	BindConf.VencSrcType = VENC_SRC_TYPE_VIU;

	for (i = 0; i < VENC_CHN_NUM_MAX; i++)
	{
		BindConf.ChnType = (2 == i) ? VENC_CHNTYPE_SNAP : (VENC_CHNTYPE_E)i;
		ret = Venc_Chn_UnBind(0, 0, BindConf);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_UnBind chn(%d) FAIL(%d)\n",BindConf.ChnType,ret);
			return -1;
		}

		ChnType = (2 == i) ? VENC_CHNTYPE_SNAP : i;		
		ret =  Venc_Chn_Close(0, ChnType, 0);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Close chn(%d) FAIL(%d)\n",ChnType,ret);
			return -1;
		}
	}

	return 0;
}

static int Sample_Vosd_SetVencChnAttr(VENC_CHNTYPE_E ChnType, VENC_ENCODEPARAM_S *pEncParam)
{
	int ret = 0;
	int srcFrmRate = 0;
	int index = (VENC_CHNTYPE_SNAP == ChnType) ? 2 : ChnType;

	ret = Venc_Chn_Get_EncodeParam(0, ChnType, 0, &s_EncParam[index], &srcFrmRate);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Get_EncodeParam FAIL(%d)\n",ret);
		return -1;
	}

	s_EncParam[index].VPayloadType = pEncParam->VPayloadType;
	s_EncParam[index].FrameRate = pEncParam->FrameRate;
	s_EncParam[index].BitRateType = pEncParam->BitRateType;
	
	s_EncParam[index].BitRate = pEncParam->BitRate;
	s_EncParam[index].KeyInterval = pEncParam->KeyInterval;
	s_EncParam[index].stVencQpConf.bEnbaleQp = pEncParam->stVencQpConf.bEnbaleQp;
	s_EncParam[index].stVencQpConf.u32MaxQp = pEncParam->stVencQpConf.u32MaxQp;
	s_EncParam[index].stVencQpConf.u32MinQp = pEncParam->stVencQpConf.u32MinQp;
	s_EncParam[index].SmartEncEnable = 0;

	ret = Venc_Chn_Set_EncodeParam(0, ChnType, 0, s_EncParam[index], pEncParam->FrameRate);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Set_EncodeParam FAIL(%d)\n",ret);
		return -1;
	}
	
	return 0;
}

static int Sample_Vosd_SetVencResolution(VENC_CHNTYPE_E ChnType, int width, int height)
{
	int ret = 0;
	int index =  (VENC_CHNTYPE_SNAP == ChnType) ? 2 : ChnType;
	VENC_BIND_CONF_S BindConf = {0};

	if ((((VENC_MJPEG_FOLLOW_MAIN == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MAIN == ChnType))
		|| ((VENC_MJPEG_FOLLOW_MINOR == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MINOR == ChnType)))
		&& (VENC_CHN_NUM_MAX > 2))
	{
		ret = Venc_Chn_Stop(0, VENC_CHNTYPE_SNAP, 0);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Stop chn(%d) FAIL(%d)\n", ChnType, ret);
			return -1;
		}
	}

	ret = Venc_Chn_Stop(0, ChnType, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Stop chn(%d) FAIL(%d)\n", ChnType, ret);
		return -1;
	}

	if(VENC_CHNTYPE_SNAP != ChnType)
	{
		ret = Vpss_Chn_Disable(0, index);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Chn_Disable FAIL(%d)\n", ret);
			return -1;
		}

		BindConf.ChnType = ChnType;
		ret = Venc_Chn_UnBind(0, 0, BindConf);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_UnBind chn(%d) FAIL(%d)\n", BindConf.ChnType, ret);
			return -1;
		}
	}
	
	ret =  Venc_Chn_Close(0, ChnType, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Close chn(%d) FAIL(%d)\n", ChnType, ret);
		return -1;
	}

	s_mjpegDataCom = 0;

	if ((((VENC_MJPEG_FOLLOW_MAIN == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MAIN == ChnType))
		|| ((VENC_MJPEG_FOLLOW_MINOR == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MINOR == ChnType)))
		&& (VENC_CHN_NUM_MAX > 2))
	{
		ret =  Venc_Chn_Close(0, VENC_CHNTYPE_SNAP, 0);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Close chn(%d) FAIL(%d)\n", ChnType, ret);
			return -1;
		}
		
	}
	if(VENC_CHNTYPE_SNAP != ChnType)
	{
		ret = Vpss_Chn_Close(0, index);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Chn_Close FAIL(%d)\n", ret);
			return -1;
		}
		
		s_FsChnAttr[index].width = width;
		s_FsChnAttr[index].height = height;

		s_EncParam[index].width = width;
		s_EncParam[index].height = height;
	}
	else
	{
		if (VENC_MJPEG_FOLLOW_MAIN == s_EncParam[index].MjpegFollowType)	
		{
			s_EncParam[index].width	= s_EncParam[1].width;
			s_EncParam[index].height = s_EncParam[1].height;
			s_EncParam[index].MjpegFollowType = VENC_MJPEG_FOLLOW_MINOR;
		}
		else
		{
			s_EncParam[index].width	= s_EncParam[0].width;
			s_EncParam[index].height = s_EncParam[0].height;
			s_EncParam[index].MjpegFollowType = VENC_MJPEG_FOLLOW_MAIN;
		}
	}

	if ((((VENC_MJPEG_FOLLOW_MAIN == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MAIN == ChnType))
		|| ((VENC_MJPEG_FOLLOW_MINOR == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MINOR == ChnType)))
		&& (VENC_CHN_NUM_MAX > 2))
	{
		s_EncParam[2].width = width;
		s_EncParam[2].height = height;
	}
	if(VENC_CHNTYPE_SNAP != ChnType)
	{
		ret = Vpss_Chn_Open(0, index, s_FsChnAttr[index]);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Open FAIL(%d)\n",ret);
			return -1;
		}
	}
	ret =  Venc_Chn_Open(0, ChnType, 0, s_EncParam[index], s_EncParam[index].FrameRate);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Open chn(%d) FAIL(%d)\n",ChnType,ret);
		return -1;
	}

	if ((((VENC_MJPEG_FOLLOW_MAIN == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MAIN == ChnType))
		|| ((VENC_MJPEG_FOLLOW_MINOR == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MINOR == ChnType)))
		&& (VENC_CHN_NUM_MAX > 2))
	{
		ret =  Venc_Chn_Open(0, VENC_CHNTYPE_SNAP, 0, s_EncParam[2], s_EncParam[2].FrameRate);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Open chn(%d) FAIL(%d)\n",ChnType,ret);
			return -1;
		}
		
	}
	if(VENC_CHNTYPE_SNAP != ChnType)
	{
		BindConf.ChnType = ChnType;
		ret = Venc_Chn_Bind(0, 0, BindConf);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Bind chn(%d) FAIL(%d)\n",BindConf.ChnType,ret);
			return -1;
		}
		
		ret = Vpss_Chn_Enable(0, index);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Chn_Enable FAIL(%d)\n",ret);
			return -1;
		}
	}
	ret = Venc_Chn_Start(0, ChnType, 0, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Start chn(%d) FAIL(%d)\n",ChnType,ret);
		return -1;
	}

	if ((((VENC_MJPEG_FOLLOW_MAIN == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MAIN == ChnType))
		|| ((VENC_MJPEG_FOLLOW_MINOR == s_EncParam[2].MjpegFollowType) && (VENC_CHNTYPE_MINOR == ChnType)))
		&& (VENC_CHN_NUM_MAX > 2))
	{
		ret = Venc_Chn_Start(0, VENC_CHNTYPE_SNAP, 0, 0);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Start chn(%d) FAIL(%d)\n",ChnType,ret);
			return -1;
		}
		
	}
	
	return 0;
}

static int Sample_Vosd_AutoSetVencChnEncType(VENC_CHNTYPE_E ChnType)
{
	int ret = 0;
	int srcFrmRate = 0;
	int index = (VENC_CHNTYPE_SNAP == ChnType) ? 2 : ChnType;
	VOSD_TRACE_INFO("Sample_Venc_AutoSetVencChnEncType start\n");
	ret = Venc_Chn_Get_EncodeParam(0, ChnType, 0, &s_EncParam[index], &srcFrmRate);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Get_EncodeParam FAIL(%d)\n",ret);
		return -1;
	}

	if (VPAYLOAD_TYPE_H264 == s_EncParam[index].VPayloadType)
	{
		s_EncParam[index].VPayloadType = VPAYLOAD_TYPE_H265;
		s_EncParam[index].BitRateType = BITRATE_TYPE_ABR;
		VOSD_TRACE_ERR("index(%d):H264 TO H265\n", index);
	}
	else
	{
		s_EncParam[index].VPayloadType = VPAYLOAD_TYPE_H264;
		s_EncParam[index].BitRateType = BITRATE_TYPE_VBR;
		VOSD_TRACE_ERR("index(%d):H265 TO H264\n", index);
	}

	s_EncParam[index].FrameRate = srcFrmRate;

	ret = Venc_Chn_Set_EncodeParam(0, ChnType, 0, s_EncParam[index], srcFrmRate);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Set_EncodeParam FAIL(%d)\n",ret);
		return -1;
	}

	VOSD_TRACE_INFO("Sample_Venc_AutoSetVencChnEncType end\n");
	return 0;
}


int Sample_Vosd_StreamOn()
{
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	int ret = 0;
	int i = 0;

	for (i = 0; i < FS_CHN_NUM_MAX; i++) 
	{
		ret = Vpss_Chn_Enable(0, i);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Chn_Enable FAIL(%d)\n",ret);
			return -1;
		}

	}

	for (i = 0; i < VENC_CHN_NUM_MAX; i++)
	{
		ChnType = (2 == i) ? VENC_CHNTYPE_SNAP : i;
		ret = Venc_Chn_Start(0, ChnType, 0, 0);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Start chn(%d) FAIL(%d)\n",ChnType,ret);
			return -1;
		}
	}

	return 0;
}

int Sample_Vosd_StreamOff()
{
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	int ret = 0;
	int i = 0;

	for (i = VENC_CHN_NUM_MAX - 1; i >= 0; i--)
	{
		ChnType = (2 == i) ? VENC_CHNTYPE_SNAP : i;
		ret = Venc_Chn_Stop(0, ChnType, 0);
		if (ret)
		{
			VOSD_TRACE_ERR("Venc_Chn_Stop chn(%d) FAIL(%d)\n",ChnType,ret);
			return -1;
		}
	}

	for (i = 0; i < FS_CHN_NUM_MAX; i++) 
	{	
		ret = Vpss_Chn_Disable(0, i);
		if (ret)
		{
			VOSD_TRACE_ERR("Vpss_Chn_Disable FAIL(%d)\n",ret);
			return -1;
		}
	}

	return 0;
}


static int Sample_Vosd_InitOsd()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0}; 
	VOSD_CONF_S VosdConf = {0};
	long long startTime = get_ms();
	long long endTime = 0;

	ret = Vosd_Open(1, ChipConf, VosdConf);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	endTime = get_ms();
	VOSD_TRACE_INFO("Sample_Venc_Chn_Start end time=%lld\n",endTime-startTime);
	return 0;
}

static int Sample_Vosd_DeInitOsd()
{
	int ret = 0;

	ret = Vosd_Close(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Close FAIL(%d)\n",ret);
		return -1;
	}
	
	return 0;
}

static int Sample_Vosd_StartOsd()
{
	int ret = 0;
	long long startTime = get_ms();
	long long endTime = 0;

	ret = Vosd_Start(0, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	endTime = get_ms();
	VOSD_TRACE_INFO("Sample_Venc_Chn_Start end time=%lld\n",endTime-startTime);
	return 0;
}

static int Sample_Vosd_StopOsd()
{
	int ret = 0;
	long long startTime = get_ms();
	long long endTime = 0;

	ret = Vosd_Stop(0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	endTime = get_ms();
	VOSD_TRACE_INFO("Sample_Venc_Chn_Start end time=%lld\n",endTime-startTime);
	return 0;
}

static int Sample_Vosd_DrawRectAndLine()
{
	int ret = 0;
	VFACE_FRAME_S VFaceFrm = {0};
	VLINE_FRAME_S VLineFrm = {0};
	int handle[3] = {0};
	int i = 0;
	
	long long startTime = get_ms();
	long long endTime = 0;
	VFaceFrm.LineW = 10;
	VFaceFrm.RectCnt = 5;
	for (i = 0; i < VFaceFrm.RectCnt; i++)
	{
		VFaceFrm.Color[i] = 1;
		VFaceFrm.Rect[i].x = 50 * i;
		VFaceFrm.Rect[i].y = 50 * i;
		VFaceFrm.Rect[i].w = 50;	
		VFaceFrm.Rect[i].h = 50;
	}


	VLineFrm.LineW = 10;
	VLineFrm.PointCnt = 5;
	VLineFrm.Color[0] = 1;
	VLineFrm.Point[0].x1 = 10;
	VLineFrm.Point[0].y1 = 10;
	VLineFrm.Point[0].x2 = FS_MAIN_CHN_WIDTH-10;
	VLineFrm.Point[0].y2 = 10;
	
	VLineFrm.Color[1] = 1;
	VLineFrm.Point[1].x1 = FS_MAIN_CHN_WIDTH-10;
	VLineFrm.Point[1].y1 = 10;
	VLineFrm.Point[1].x2 = FS_MAIN_CHN_WIDTH-10;
	VLineFrm.Point[1].y2 = FS_MAIN_CHN_HEIGHT-10;

	VLineFrm.Color[2] = 1;
	VLineFrm.Point[2].x1 = FS_MAIN_CHN_WIDTH-10;
	VLineFrm.Point[2].y1 = FS_MAIN_CHN_HEIGHT-10;
	VLineFrm.Point[2].x2 = 10;
	VLineFrm.Point[2].y2 = FS_MAIN_CHN_HEIGHT-10;

	VLineFrm.Color[3] = 1;
	VLineFrm.Point[3].x1 = 10;
	VLineFrm.Point[3].y1 = FS_MAIN_CHN_HEIGHT-10;
	VLineFrm.Point[3].x2 = 10;
	VLineFrm.Point[3].y2 = 10;	

	VLineFrm.Color[4] = 1;
	VLineFrm.Point[4].x1 = 10;
	VLineFrm.Point[4].y1 = 10;
	VLineFrm.Point[4].x2 = FS_MAIN_CHN_WIDTH-10;
	VLineFrm.Point[4].y2 = FS_MAIN_CHN_HEIGHT-10;	

	ret = Vosd_Chn_Draw_Rect(-1, 0, VOSD_CHNTYPE_VENC_MAIN, VFaceFrm, VLineFrm, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}


	VLineFrm.LineW = 5;
	VLineFrm.PointCnt = 5;
	VLineFrm.Color[0] = 1;
	VLineFrm.Point[0].x1 = 0;
	VLineFrm.Point[0].y1 = 0;
	VLineFrm.Point[0].x2 = FS_SUB_CHN_WIDTH-1;
	VLineFrm.Point[0].y2 = 0;
	
	VLineFrm.Color[1] = 1;
	VLineFrm.Point[1].x1 = FS_SUB_CHN_WIDTH-1;
	VLineFrm.Point[1].y1 = 0;
	VLineFrm.Point[1].x2 = FS_SUB_CHN_WIDTH-1;
	VLineFrm.Point[1].y2 = FS_SUB_CHN_HEIGHT-1;

	VLineFrm.Color[2] = 1;
	VLineFrm.Point[2].x1 = FS_SUB_CHN_WIDTH-1;
	VLineFrm.Point[2].y1 = FS_SUB_CHN_HEIGHT-1;
	VLineFrm.Point[2].x2 = 0;
	VLineFrm.Point[2].y2 = FS_SUB_CHN_HEIGHT-1;

	VLineFrm.Color[3] = 1;
	VLineFrm.Point[3].x1 = 0;
	VLineFrm.Point[3].y1 = FS_SUB_CHN_HEIGHT-1;
	VLineFrm.Point[3].x2 = 0;
	VLineFrm.Point[3].y2 = 0;	

	VLineFrm.Color[4] = 1;
	VLineFrm.Point[4].x1 = 10;
	VLineFrm.Point[4].y1 = 10;
	VLineFrm.Point[4].x2 = FS_SUB_CHN_WIDTH-1;
	VLineFrm.Point[4].y2 = FS_SUB_CHN_HEIGHT-1;	
	
	VFaceFrm.RectCnt = 5;
	for (i = 0; i < VFaceFrm.RectCnt; i++)
	{
		VFaceFrm.Color[i] = 1;
		VFaceFrm.Rect[i].x = 50 * i;
		VFaceFrm.Rect[i].y = 50 * i;
		VFaceFrm.Rect[i].w = 50;	
		VFaceFrm.Rect[i].h = 50;
	}
	ret = Vosd_Chn_Draw_Rect(-1, 0, VOSD_CHNTYPE_VENC_MINOR, VFaceFrm, VLineFrm, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}

//	ret = Vosd_Chn_Draw_Rect(-1, 0, VOSD_CHNTYPE_VENC_AI, VFaceFrm, VLineFrm, 0);
//	if (ret)
//	{
//		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
//		return -1;
//	}

	return 0;
}


static int Sample_Vosd_DelRectAndLine()
{
	int ret = 0;
	VFACE_FRAME_S VFaceFrm = {0};
	VLINE_FRAME_S VLineFrm = {0};

	VFaceFrm.LineW = 0;
	VFaceFrm.RectCnt = 0;	

	ret = Vosd_Chn_Draw_Rect(-1, 0, VOSD_CHNTYPE_VENC_MAIN, VFaceFrm, VLineFrm, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}


	ret = Vosd_Chn_Draw_Rect(-1, 0, VOSD_CHNTYPE_VENC_MINOR, VFaceFrm, VLineFrm, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}

	return 0;
}


static int Sample_Vosd_DrawMosaic()
{
//	return 0;
	int ret = 0;
	VMOSAIC_REGION_S VMosaic = {0};

	VMosaic.RectCnt = 2;
	VMosaic.BlkSize = 20;
	VMosaic.Rect[0].x = 0;
	VMosaic.Rect[0].y = 0;
	VMosaic.Rect[0].w = 323;
	VMosaic.Rect[0].h = 151;

	VMosaic.Rect[1].x = 1165;
	VMosaic.Rect[1].y = 721;
	VMosaic.Rect[1].w = 712;
	VMosaic.Rect[1].h = 213;
	
	ret = Vosd_Chn_Set_MosaicParam(0, 0, VOSD_CHNTYPE_VENC_MAIN, VMosaic, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	VMosaic.RectCnt = 2;
	VMosaic.BlkSize = 20;
	VMosaic.Rect[0].x = 0;
	VMosaic.Rect[0].y = 0;
	VMosaic.Rect[0].w = 89;
	VMosaic.Rect[0].h = 42;

	VMosaic.Rect[1].x = 323;
	VMosaic.Rect[1].y = 199;
	VMosaic.Rect[1].w = 198;
	VMosaic.Rect[1].h = 60;

	ret = Vosd_Chn_Set_MosaicParam(0, 0, VOSD_CHNTYPE_VENC_MINOR, VMosaic, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}

#ifdef __DSP_GET_AI_FRAME_BY_PPL__
	VMosaic.RectCnt = 1;
	VMosaic.BlkSize = 20;
	VMosaic.Rect[0].x = 0;
	VMosaic.Rect[0].y = 0;
	VMosaic.Rect[0].w = 600;
	VMosaic.Rect[0].h = 600;

	ret = Vosd_Chn_Set_MosaicParam(0, 0, VOSD_CHNTYPE_VENC_AI, VMosaic, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
#endif
	return 0;
}

static int Sample_Vosd_DelMosaic()
{
//	return 0;
	int ret = 0;
	VMOSAIC_REGION_S VMosaic = {0};

	VMosaic.RectCnt = 0;

	ret = Vosd_Chn_Set_MosaicParam(0, 0, VOSD_CHNTYPE_VENC_MAIN, VMosaic, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Vosd_Chn_Set_MosaicParam(0, 0, VOSD_CHNTYPE_VENC_MINOR, VMosaic, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}

#ifdef __DSP_GET_AI_FRAME_BY_PPL__
	ret = Vosd_Chn_Set_MosaicParam(0, 0, VOSD_CHNTYPE_VENC_AI, VMosaic, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Open FAIL(%d)\n",ret);
		return -1;
	}
#endif
	return 0;
}

//出流后绘制，否则会报错
#define MAX_BITMAP_NUM  (6)
static int s_bmpFontHand[MAX_BITMAP_NUM] = {-1};

static int __DrawFontByBitmap(VRECT_S *Rect, char *path, VOSD_CHNTYPE_E type)
{
	int ret = 0;
	char *data = NULL;
	int fontLen = 1344000;
	FILE *fp = NULL;
	VOSD_OVERYLAY_S VosdOverLay = {0};
		
	fp = fopen(path, "r");
	if (NULL == fp)
	{
		VOSD_TRACE_ERR("fopen FAIL(%s)\n",path);
		return -1;
	}

    fseek(fp,0,SEEK_END);
    fontLen = ftell(fp);
    rewind(fp);

	data = (char *) malloc (fontLen);
	if (NULL == data)
	{
		VOSD_TRACE_ERR("malloc FAIL(%d)\n",fontLen);
		return -1;
	}

	fread(data, 1, fontLen, fp);		

	VosdOverLay.InvEnable = 0;
	VosdOverLay.pData = data;
	VosdOverLay.Rect.x = Rect->x;
	VosdOverLay.Rect.y = Rect->y;
	VosdOverLay.Rect.w = Rect->w;//20 * 16 ;
	VosdOverLay.Rect.h = Rect->h;//34;

	Vosd_Chn_SetBitMap(-1, 0, type, VosdOverLay);

	fclose(fp);
	fp = NULL;
	free(data);
	data = NULL;

	return 0;
}

static int Sample_Vosd_DrawFontByBitmap()
{
	int ret = 0;

	VRECT_S Rect = {0};
	
	Rect.x = 0;
	Rect.y = 0;
	Rect.w = 480;
	Rect.h = 32;
	__DrawFontByBitmap(&Rect, "./151_stream_1_textOSD_xywh_0_0_480_32", VOSD_CHNTYPE_VENC_MAIN);
	VOSD_TRACE_ERR("Draw STM_960_540_OSD_LOGO_TYPE_846_506_944_534_SIZE_10976 end\n");

//	Rect.x = 23;
//	Rect.y = 501;
//	Rect.w = 776;
//	Rect.h = 33;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_960_540_OSD_TEXT_TYPE_23_501_799_534_SIZE_51216.bin", VOSD_CHNTYPE_VENC_MAIN);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 455;
//	Rect.y = 321;
//	Rect.w = 480;
//	Rect.h = 179;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_960_540_OSD_TEXT_TYPE_455_321_935_500_SIZE_171840.bin", VOSD_CHNTYPE_VENC_MAIN);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 720;
//	Rect.y = 54;
//	Rect.w = 192;
//	Rect.h = 81;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_960_540_OSD_TEXT_TYPE_720_54_912_135_SIZE_31104.bin", VOSD_CHNTYPE_VENC_MAIN);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 640;
//	Rect.y = 16;
//	Rect.w = 304;
//	Rect.h = 32;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_960_540_OSD_TIME_TYPE_640_16_944_48_SIZE_19456.bin", VOSD_CHNTYPE_VENC_MAIN);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 558;
//	Rect.y = 334;
//	Rect.w = 66;
//	Rect.h = 18;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_640_360_OSD_LOGO_TYPE_558_334_624_352_SIZE_4752.bin", VOSD_CHNTYPE_VENC_MINOR);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 15;
//	Rect.y = 330;
//	Rect.w = 520;
//	Rect.h = 222;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_640_360_OSD_TEXT_TYPE_15_330_535_352_SIZE_22880.bin", VOSD_CHNTYPE_VENC_MINOR);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 303;
//	Rect.y = 210;
//	Rect.w = 320;
//	Rect.h = 119;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_640_360_OSD_TEXT_TYPE_303_210_623_329_SIZE_76160.bin", VOSD_CHNTYPE_VENC_MINOR);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 480;
//	Rect.y = 36;
//	Rect.w = 128;
//	Rect.h = 54;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_640_360_OSD_TEXT_TYPE_480_36_608_90_SIZE_13824.bin", VOSD_CHNTYPE_VENC_MINOR);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
//	Rect.x = 480;
//	Rect.y = 8;
//	Rect.w = 152;
//	Rect.h = 16;
//	__DrawFontByBitmap(&Rect, "/mnt/t41/bitmap/y53l/STM_640_360_OSD_TIME_TYPE_480_8_632_24_SIZE_4864.bin", VOSD_CHNTYPE_VENC_MINOR);
//	VOSD_TRACE_ERR("__DrawFontByBitmap end\n");
	return 0;
}

static int Sample_Vosd_DelFontByBitmap()
{
	int i = 0;

	for (i = 0; i < MAX_BITMAP_NUM; i++)
	{
		if (-1 != s_bmpFontHand[i])
		{
			Vosd_Chn_DelBitMap(s_bmpFontHand[0], 0, VOSD_CHNTYPE_VENC_MAIN);
			s_bmpFontHand[0] = -1;
		}
	}
	return 0;
}


static int s_strFontHand[2] = {-1};
//出流后绘制，否则会报错
static int Sample_Vosd_DrawFontByString()
{
	int ret = 0;
	VOSD_TEXT_OVERYLAY_S VosdOverLay = {0};
	UINT16 text3[512] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x5e,\
						0x41, 0x49, 0x20, 0x49, 0x50, 0x43, 0x20, 0x4b, 0x5e,\
						0x82af, 0x777f, 0x89c6, 0x6d4b, 0x8bd5,0x0,};
	UINT16 text7[512] = {0x82af, 0x777f, 0x89c6, 0x7ec8, 0x7aef, 0x4e8b, 0x4e1a, 0x90e8, 0x6444, 0x50cf, 0x82af, 0x777f, 0x89c6, 0x7ec8, 0x7aef, 0x5e, \
						0x82af, 0x777f, 0x89c6, 0x7ec8, 0x7aef, 0x4e8b, 0x4e1a, 0x90e8, 0x6444, 0x50cf, 0x82af, 0x777f, 0x89c6, 0x7ec8, 0x7aef,};
	UINT64 startTime = 0;
	UINT64 endTime = 0;

	VosdOverLay.InvEnable = 0;
	memcpy(VosdOverLay.strText, text7, sizeof(UINT16)*512);
	VosdOverLay.nX = 0;
	VosdOverLay.nY = 0;
	VosdOverLay.nPicWidth = FS_MAIN_CHN_WIDTH;
	VosdOverLay.nPicHeight = FS_MAIN_CHN_HEIGHT;
	startTime = get_ms();
	s_strFontHand[0] = Vosd_Chn_SetBitMapByText(-1, 0, VOSD_CHNTYPE_VENC_MAIN, VosdOverLay);
	endTime = get_ms();
	VOSD_TRACE_WARN("Vosd_Chn_SetBitMapByText-main-handle(%d),cost time:%lld ms\n\n", s_strFontHand[0], endTime-startTime);
		
//	sleep(1);
//	Vosd_Chn_DelBitMap(handle, 0, VOSD_CHNTYPE_VENC_MAIN);

	VosdOverLay.nPicWidth = FS_SUB_CHN_WIDTH;
	VosdOverLay.nPicHeight = FS_SUB_CHN_HEIGHT;
	startTime = get_ms();
	s_strFontHand[1] = Vosd_Chn_SetBitMapByText(-1, 0, VOSD_CHNTYPE_VENC_MINOR, VosdOverLay);
	endTime = get_ms();
	VOSD_TRACE_WARN("Vosd_Chn_SetBitMapByText-minor-handle(%d),cost time:%lld ms\n\n", s_strFontHand[1], endTime-startTime);

//	sleep(1);
//	Vosd_Chn_DelBitMap(handle, 0, VOSD_CHNTYPE_VENC_MINOR);
		
			
	return 0;

}

static int Sample_Vosd_DelFontByString()
{
	if (-1 != s_strFontHand[0])
	{
		Vosd_Chn_DelBitMap(s_strFontHand[0], 0, VOSD_CHNTYPE_VENC_MAIN);
		s_strFontHand[0] = -1;
	}

	if (-1 != s_strFontHand[1])
	{
		Vosd_Chn_DelBitMap(s_strFontHand[1], 0, VOSD_CHNTYPE_VENC_MINOR);
		s_strFontHand[1] = -1;
	}

	return 0;
}

//出流后绘制，否则会报错
static int s_bmpFontHandByAddr[2] = {-1, -1};

static int Sample_Vosd_DrawFontByPlatAddr()
{
	int ret = 0;
	int fontLen = 1344000;
	int fd = -1;
	VOSD_OVERYLAY_S VosdOverLay = {0};
	void *pData = NULL;

	VosdOverLay.InvEnable = 0;
	VosdOverLay.pData = NULL;
	VosdOverLay.Rect.x = 1300;
	VosdOverLay.Rect.y = 0;
	VosdOverLay.Rect.w = 1200;
	VosdOverLay.Rect.h = 560;

	ret = Vosd_Chn_SetBitMapAttrToGetAddrFromPlat(&s_bmpFontHandByAddr[0] , 0, VOSD_CHNTYPE_VENC_MAIN, &VosdOverLay);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat FAIL(%d)\n",ret);
		return -1;
	}
	VOSD_TRACE_INFO("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat-main-ret(%d)-handle(%d),DATA(%p)\n", ret, s_bmpFontHandByAddr[0], VosdOverLay.pData);
	
	fd = open("/mnt/t41/bitmap/7/stream_0_textOSD_xywh_0_0_1200_560", O_RDWR, 0777);
	if (-1 == fd)
	{
		VOSD_TRACE_ERR("open FAIL(%d)\n",fontLen);
		return -1;
	}
	
	fontLen = VosdOverLay.Rect.w * VosdOverLay.Rect.h * 2;
	read(fd, VosdOverLay.pData, fontLen);		

	close(fd);
	fd = -1;
	
	ret = Vosd_Chn_UpdateBitMapDataByPlatAddr(s_bmpFontHandByAddr[0], 0, VOSD_CHNTYPE_VENC_MAIN, 1);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Chn_UpdateBitMapDataByPlatAddr-ret(%d)\n", ret);
		return -1;
	}

	ret = Vosd_Chn_GetAddrFromPlatByHandle(s_bmpFontHandByAddr[0], 0, VOSD_CHNTYPE_VENC_MAIN, &pData);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat FAIL(%d)\n",ret);
		return -1;
	}
	VOSD_TRACE_INFO("Vosd_Chn_GetAddrFromPlatByHandle-main-ret(%d)-handle(%d),DATA(%p)\n", ret, s_bmpFontHandByAddr[0], pData);
	
	VosdOverLay.InvEnable = 0;
	VosdOverLay.pData = NULL;
	VosdOverLay.Rect.x = 0;
	VosdOverLay.Rect.y = 0;
	VosdOverLay.Rect.w = 480;
	VosdOverLay.Rect.h = 224;

	ret = Vosd_Chn_SetBitMapAttrToGetAddrFromPlat(&s_bmpFontHandByAddr[1], 0, VOSD_CHNTYPE_VENC_MINOR, &VosdOverLay);
	VOSD_TRACE_INFO("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat-minor-ret(%d)-handle(%d),DATA(%p)\n", ret, s_bmpFontHandByAddr[1], VosdOverLay.pData);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Chn_SetBitMapAttrToGetAddrFromPlat FAIL(%d)\n",ret);
		return -1;
	}
	
	fd = open("/mnt/t41/bitmap/7/stream_1_textOSD_xywh_0_0_480_224", O_RDWR, 0777);
	if (-1 == fd)
	{
		VOSD_TRACE_ERR("open FAIL(%d)\n",fontLen);
		return -1;
	}
	
	fontLen = VosdOverLay.Rect.w * VosdOverLay.Rect.h * 2;
	read(fd, VosdOverLay.pData, fontLen);		

	close(fd);
	fd = -1;
	ret = Vosd_Chn_UpdateBitMapDataByPlatAddr(s_bmpFontHandByAddr[1], 0, VOSD_CHNTYPE_VENC_MINOR, 1);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Chn_UpdateBitMapDataByPlatAddr-ret(%d)\n", ret);
		return -1;
	}

	ret = Vosd_Chn_GetAddrFromPlatByHandle(s_bmpFontHandByAddr[1], 0, VOSD_CHNTYPE_VENC_MINOR, &pData);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Chn_GetAddrFromPlatByHandle FAIL(%d)\n",ret);
		return -1;
	}
	VOSD_TRACE_INFO("Vosd_Chn_GetAddrFromPlatByHandle-minor-ret(%d)-handle(%d),DATA(%p)\n", ret, s_bmpFontHandByAddr[1], pData);
	
	return 0;
}

static int Sample_Vosd_DelFontByPlatAddr()
{
	if (-1 != s_bmpFontHandByAddr[0])
	{
		Vosd_Chn_DelBitMap(s_bmpFontHandByAddr[0], 0, VOSD_CHNTYPE_VENC_MAIN);
		s_bmpFontHandByAddr[0] = -1;
	}

	if (-1 != s_bmpFontHandByAddr[1])
	{
		Vosd_Chn_DelBitMap(s_bmpFontHandByAddr[1], 0, VOSD_CHNTYPE_VENC_MINOR);
		s_bmpFontHandByAddr[1] = -1;
	}

	return 0;
}

static int Sample_Vosd_InitVda()
{
	int ret = 0;
	VIDEO_CHIP_CONF_S ChipConf = {0};
	VDA_CONF_S VdaConf = {0};
	long long startTime = get_ms();
	long long endTime = 0;
	VOSD_TRACE_INFO("Sample_Vda_Init start\n");
	ret = Vda_Open(1, ChipConf, VdaConf);
	if (ret)
	{
		VOSD_TRACE_ERR("Vosd_Close FAIL(%d)\n",ret);
		return ret;
	}
	
	endTime = get_ms();
	VOSD_TRACE_INFO("Sample_Vda_Init end time=%lld\n",endTime-startTime);
	return 0;
}

static int Sample_Vosd_InitVdaChn()
{
	int ret = 0;
	VDA_CHN_ATTR_S VdaChnAttr =
    {
        0, 0, 0, 0, 0, 
		{0}, FS_SUB_CHN_WIDTH, FS_SUB_CHN_HEIGHT
    };
	VMOSAIC_REGION_S VMosaic;
	
    ret = Vda_Chn_Open(0, VdaChnAttr);
	if (ret < 0)
    {
        VOSD_TRACE_ERR("Vda_Chn_Open failed, ret=%d\n", ret);
        return ret;
    }

	VMosaic.RectCnt = 2;
	VMosaic.Rect[0].x = 50;
	VMosaic.Rect[0].y = 37;
	VMosaic.Rect[0].w = 200;
	VMosaic.Rect[0].h = 200;

	VMosaic.Rect[1].x = 300;
	VMosaic.Rect[1].y = 74;
	VMosaic.Rect[1].w = 300;
	VMosaic.Rect[1].h = 374;
	
	//Vda_Set_MosaicParam(VMosaic, 0);
	
	return 0;
}

static int Sample_Vosd_GetVdaMdResult()
{
	int ret = 0;
	int i = 0;
	VDA_MD_AREADATA_S MdData = {0};
	int cnt = 0;
#ifdef __DSP_MD_EVENT_SNAP_PIC__
	char *pSnapData = NULL;

	pSnapData = (UINT8*) malloc (256*1024);
	if (NULL == pSnapData)
	{
		VOSD_TRACE_ERR("MD malloc Failed\n");
		return -1;
	}
#endif
	VDA_MD_PARAM_S mdParam = {0};

	ret = Vda_Chn_Get_MdParam(0, 0, &mdParam);
	if (ret)
	{
		VOSD_TRACE_ERR("MD Vda_Chn_Get_MdParam Failed\n");
		return -1;
	}

	while (1)
	{
		if (cnt > 100)
		{
			break;
		}
		cnt++;
		MdData.BlkNum = 0;
		memset(MdData.MArea, 0, MAX_MAREA_NUM);
#ifdef __DSP_MD_EVENT_SNAP_PIC__
		MdData.pSnapData = pSnapData;
		MdData.SnapLen = 0;	
#endif
		ret = Vda_Chn_Get_MdData(0, 0, &MdData);
		if (ret < 0)
		{
			VOSD_TRACE_ERR("MD Vda_Chn_Get_MdData Failed\n");
			return -1;
		}
		VOSD_TRACE_INFO("BlkNum=%d\n", MdData.BlkNum);
		if (MdData.BlkNum)
		{
			for (i = 0; i < mdParam.MbCnt[0] * mdParam.MbCnt[1]; i++)
			{
				if ((i > 0) && (0 == i % mdParam.MbCnt[0]))
				{
					printf("\n");
				}
				printf("%d ",MdData.MArea[i]);
			}
			printf("\n");
		}
#ifdef __DSP_MD_EVENT_SNAP_PIC__
		int fd = -1;
		char command[64] = {0};
		if (MdData.SnapLen && MdData.pSnapData)
		{
			snprintf(command, sizeof(command), "%s/mds-snap-%lld.jpg", STREAM_FILE_SAVE_PATH, get_ms()/1000);
			fd = open(command, O_RDWR | O_CREAT | O_TRUNC, 0644);
			if (MdData.SnapLen != write(fd, MdData.pSnapData, MdData.SnapLen)) 
			{
				printf("MdData write failed\n");
				close(fd);
				return -1;
			}
			close(fd);
			fd = -1;
		}
#endif
		sleep(1);
	}
#ifdef __DSP_MD_EVENT_SNAP_PIC__
	if (NULL == pSnapData)
	{
		free(pSnapData);
		pSnapData = NULL;
	}
#endif
	return 0;
	
}


static int Sample_Vosd_SetVdaMdParam( VDA_MD_PARAM_S *param)
{
	int ret = 0;
	VDA_MD_PARAM_S mdParam = {0};

	ret = Vda_Chn_Get_MdParam(0, 0, &mdParam);
	if (ret)
	{
		VOSD_TRACE_ERR("MD Vda_Chn_Get_MdParam Failed\n");
		return -1;
	}

	mdParam.enable = param->enable;
	mdParam.sen = param->sen;

	ret = Vda_Chn_Set_MdParam(0, 0, mdParam);
	if (ret)
	{
		VOSD_TRACE_ERR("MD Vda_Chn_Set_MdParam Failed\n");
		return -1;
	}

	return 0;
	
}

static int Sample_Vosd_DeInitVdaChn()
{
	int ret = 0;

	ret = Vda_Chn_Close(0, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vda_Chn_Close FAIL(%d)\n",ret);
		return ret;
	}
	
	return 0;
}

static int Sample_Vosd_ReSetMdChn()
{
	int ret = 0;
	VDA_CHN_ATTR_S VdaChnAttr =
    {
        0, 0, 0, 0, 0, 
		{0}, FS_SUB_CHN_WIDTH, FS_SUB_CHN_HEIGHT
    };

	ret = Vda_Chn_Close(0, 0);
	if (ret)
	{
		VOSD_TRACE_ERR("Vda_Chn_Close FAIL(%d)\n",ret);
		return ret;
	}
		
    ret = Vda_Chn_Open(0, VdaChnAttr);
	if (ret < 0)
    {
        VOSD_TRACE_ERR("Vda_Chn_Open failed, ret=%d\n", ret);
        return ret;
    }

	return 0;
	
}


static int Sample_Vosd_DeInitVda()
{
	int ret = 0;

    ret = Vda_Close(0);
	if (ret < 0)
    {
        VOSD_TRACE_ERR("Vda_Close failed, ret=%d\n", ret);
        return ret;
    }
	
	return 0;
}

int Sample_Enc_Yuv_To_Jpeg()
{
    int ret = 0;
	UINT8 *src_buf = NULL;
	UINT8 *dst_buf = NULL;
    int src_width = 1920, src_height = 1080;
	int dst_width = 2560, dst_height = 1440;

/*******************************read NV12 data -> src_buf************************************/
	int frmSize;
	frmSize = src_width * src_height * 3 / 2;
	src_buf = (UINT8*)malloc(frmSize);
	if(src_buf == NULL) {
		VOSD_TRACE_ERR("video_vbm_malloc src_buf failed\n");
		return -1;
	}

    FILE *inFile = NULL;
    inFile = fopen("1920x1080.nv12", "rb");
	if (inFile == NULL) {
		VOSD_TRACE_ERR("fopen src file:%s failed\n", "1920x1080.nv12");
		return -1;
	}

	if(fread(src_buf, frmSize, 1, inFile) != frmSize) {
		fseek(inFile, 0, SEEK_SET);
		fread(src_buf, 1, frmSize, inFile);
	}
	fclose(inFile);

	char path[128];	
	sprintf(path, "snap233.jpeg");
	int bitStreamFileFd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (bitStreamFileFd < 0) {
		VOSD_TRACE_ERR("open bitStreamFileName:%s failed\n", path);
		return -1;
	}

	VENC_YUV_INFO_S YuvInfo;
	YuvInfo.pData = (UINT8*)src_buf;
	YuvInfo.u32Width = 1920;
	YuvInfo.u32Height = 1080;
	YuvInfo.u32Size = 1920*1080*3/2;

	char *jpegBuf = malloc(256*1024);
	int BufSize = 256*1024;
	int JpegSize = 0;
	VRECT_S Rect;
	Rect.x = 100;
	Rect.y = 100;
	Rect.w = 300;
	Rect.h = 300;
	long long startTime = get_ms();
	//Venc_YuvEncToJpeg(0, YuvInfo, jpegBuf, BufSize, &JpegSize);
	Venc_YuvCropEncToJpeg(0, YuvInfo, Rect, jpegBuf, BufSize, &JpegSize, 25);
	long long endTime = get_ms();
	printf("%s encode sucess, jpeg length:%d,cost:%lld\n", path, JpegSize,endTime-startTime);
	if (JpegSize != write(bitStreamFileFd, jpegBuf, JpegSize)) {
		printf("stream write failed\n");
		return -1;
	}

/*******************************************exit******************************************/
	//free(resize_tmp_buf);
	free(jpegBuf);
	free(src_buf);
	//video_vbm_free(dst_buf);
	close(bitStreamFileFd);

	return 0 ;
}

#define AENC_OUT_FILE "./aenc_record.g711"
static int s_AencFileSave = 0;

static SINT32 _Audio_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, char *data, SINT32 size)
{
	static int fd[2] = {-1};
	char filePath[64] = {0};
	int writeRet = 0;
	int ret = 0;
	return 0;
//	AENC_TRACE_INFO("[AUDIO] stype=%d,idx=%d,size=%d!\n", stype, idx, size);
	if (s_AencFileSave)
	{
		if (-1 == fd[idx])
		{
			if (0 == access(AENC_OUT_FILE, F_OK))
			{
				memset(filePath, 0, sizeof(filePath));
				snprintf(filePath, sizeof(filePath), "rm -f %s", AENC_OUT_FILE);
				mysystem(filePath);
			}
		
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
	}
	
	return 0;
}

static int Sample_Vosd_InitAi()
{
	int s32Ret = 0;
	AI_CONF_S stAiCfg = {1, 1, {0}, {0}};
	AUDIO_CONF_S stAiChnConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 640, 35};
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
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
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}

static int Sample_Vosd_InitAenc(AUDIO_TYPE_E audioType)
{
	int s32Ret = 0;
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
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

	s32Ret = Aenc_Chn_Open(0, 0, audioType);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Open(0, 1, audioType);
	if (s32Ret)
	{
		AENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}


static int Sample_Vosd_InitAdec(AUDIO_TYPE_E audioType)
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


static int Sample_Vosd_InitAo()
{
	int s32Ret = 0;
	AUDIO_CONF_S stAoConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 320, 10};
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
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

	//开启喇叭输出
	HalMod_Set_SpeakerStat(0, 1);
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
	return 0;
}

static int Sample_Vosd_DeInitAi()
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


static int Sample_Vosd_DeInitAo()
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


static int Sample_Vosd_DeInitAdec()
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


static int Sample_Vosd_DeInitAenc()
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

static int Sample_Vosd_BindAudio()
{
	int s32Ret = 0;
	AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};
	AENC_BIND_CONF_S stAeBindConf = {0, AUDIO_DEV_AI_TALK, 0, 0};
	AENC_BIND_CONF_S stAeBindConf1 = {0, AUDIO_DEV_AI_TALK, 1, 0};
	
	s32Ret = Ao_Chn_Bind(0, stAoBindConf);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_Bind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Bind(0, stAeBindConf);
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

	return 0;
}

static int Sample_Vosd_UnBindAudio()
{
	int s32Ret = 0;
	AO_BIND_CONF_S stAoBindConf = {0, AUDIO_DEV_ADEC, AO_DEV_CVBS, 0};
	AENC_BIND_CONF_S stAeBindConf = {0, AUDIO_DEV_AI_TALK, 0, 0};
	AENC_BIND_CONF_S stAeBindConf1 = {0, AUDIO_DEV_AI_TALK, 1, 0};
	
	s32Ret = Ao_Chn_UnBind(0, stAoBindConf);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_UnBind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_UnBind(0, stAeBindConf);
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

	return 0;
}

static int Sample_Vosd_InitAudio(AUDIO_TYPE_E audioType)
{
	int s32Ret = 0;

	s32Ret = Sample_Vosd_InitAi();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_InitAi Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_InitAenc(audioType);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_InitAenc Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_InitAo();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_InitAo Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_InitAdec(audioType);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_InitAdec Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_BindAudio();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_BindAudio Failed!\n");
		return -1;
	}

	return 0;
}


static int Sample_Vosd_DeInitAudio()
{
	int s32Ret = 0;

	s32Ret = Sample_Vosd_UnBindAudio();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_UnBindAudio Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_DeInitAi();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_DeInitAi Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_DeInitAenc();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_DeInitAenc Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_DeInitAo();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_DeInitAo Failed!\n");
		return -1;
	}

	s32Ret = Sample_Vosd_DeInitAdec();
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Sample_Vosd_DeInitAdec Failed!\n");
		return -1;
	}

	return 0;
}

int Sample_Vosd_InitModule()
{
	int ret = 0;

	ret = Sample_Vosd_InitVi();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Venc_InitVi FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitVisp();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Venc_InitVisp FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitOsd();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_InitOsd FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitVda();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_InitVda FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitVpss();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_InitVpss FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitVenc();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_InitVenc FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitVencChn();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_InitVencChn FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_InitVdaChn();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_InitVdaChn FAIL(%d)\n",ret);
		return -1;
	}

//	ret = Sample_Vosd_InitAudio(AUDIO_G711U);
//	if (ret)
//	{
//		VOSD_TRACE_ERR("Sample_Vosd_InitAudio FAIL(%d)\n",ret);
//		return -1;
//	}

	return ret;
}

int Sample_Vosd_DeInitModule()
{
	int ret = 0;

//	ret = Sample_Vosd_DeInitAudio();
//	if (ret)
//	{
//		VOSD_TRACE_ERR("Sample_Vosd_DeInitAudio FAIL(%d)\n",ret);
//		return -1;
//	}

	ret = Sample_Vosd_DeInitVdaChn();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVdaChn FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Sample_Vosd_DeInitVda();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVda FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Sample_Vosd_DeInitVencChn();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVencChn FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_DeInitVenc();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVenc FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_DeInitVpss();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVpss FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_DeInitOsd();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitOsd FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_DeInitVisp();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVisp FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_DeInitVi();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitVi FAIL(%d)\n", ret);
		return -1;
	}

	return ret;
}

int Sample_Vosd_StartModule()
{
	int ret = 0;

	ret = Sample_Vosd_StreamOn();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Visp_Init FAIL(%d)\n",ret);
		return -1;
	}
	
//	ret = Sample_Vosd_StartOsd();
//	if (ret)
//	{
//		VOSD_TRACE_ERR("Sample_Venc_StartOsd FAIL(%d)\n",ret);
//		return -1;
//	}

	return ret;
}

int Sample_Vosd_StopModule()
{
	int ret = 0;
	
	ret = Sample_Vosd_StopOsd();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Venc_StopOsd FAIL(%d)\n",ret);
		return -1;
	}
	
	ret = Sample_Vosd_StreamOff();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Venc_StreamOff FAIL(%d)\n",ret);
		return -1;
	}

	return ret;
}

static void _Vosd_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static int _Test_Set_VencAttr()
{
	int ret = 0;
	char sCmd[8] = {0};
	VENC_ENCODEPARAM_S encAttr = {0};
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	
    printf("input chnType:0-main;1-minor\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	ChnType = atoi(sCmd);//(atoi(sCmd) > 1) ? 0 : atoi(sCmd);
	
	printf("input VPayloadType:0-H264;1-H265\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.VPayloadType = atoi(sCmd);//(1 == atoi(sCmd)) ? VPAYLOAD_TYPE_H265 : VPAYLOAD_TYPE_H264;

	printf("input FrameRate:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.FrameRate = atoi(sCmd);
	
	printf("input BitRateType:0-VBR;1-CBR\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.BitRateType = atoi(sCmd);//(atoi(sCmd) > 1) ? 0 : atoi(sCmd);

	printf("input BitRate:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.BitRate = atoi(sCmd);

	printf("input KeyInterval:[0,1000]\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.KeyInterval = atoi(sCmd);

	printf("input bEnbaleQp:0-disable;1-enable\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.stVencQpConf.bEnbaleQp = atoi(sCmd);

	printf("input u32MaxQp:[0,51]\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.stVencQpConf.u32MaxQp = atoi(sCmd);

	printf("input u32MinQp:[0,51]\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	encAttr.stVencQpConf.u32MinQp = atoi(sCmd);

	ret = Sample_Vosd_SetVencChnAttr(ChnType, &encAttr);
	if (ret)
	{
		VENC_TRACE_ERR("Sample_Venc_SetVencChnAttr FAIL(%d)\n",ret);
		return -1;
	}
	
	return 0;
}

static int _Test_Set_VencResolution()
{
	int ret = 0;
	char sCmd[8] = {0};
	int width = 0;
	int height = 0;
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;

    printf("input chnType:0-main;1-minor;3-snap\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	ChnType = atoi(sCmd);//(atoi(sCmd) > 1) ? 0 : atoi(sCmd);
	
//    printf("input width:\n");
//	memset(sCmd, 0, sizeof(sCmd));
//	_Vosd_Get_Cmd(sCmd);
//	width = (atoi(sCmd) > 2880) ? 2880 : atoi(sCmd);

//    printf("input height:\n");
//	memset(sCmd, 0, sizeof(sCmd));
//	_Vosd_Get_Cmd(sCmd);
//	height = (atoi(sCmd) > 1624) ? 1624 : atoi(sCmd);
//	Sample_Vosd_DelFontByString();
//	Sample_Vosd_DelRectAndLine();
	int cnt = 1;
	while(1)
	{
		if (1 == s_mjpegDataCom)
		{
			ret = Sample_Vosd_SetVencResolution(ChnType, width, height);
			VENC_TRACE_ERR("Sample_Vosd_SetVencResolution cnt=%d\n", cnt++);	
		}
		
		sleep(3);
	}
	if (ret)
	{
		VENC_TRACE_ERR("Sample_Venc_SetVencResolution FAIL(%d)\n",ret);
		return -1;
	}
//	Sample_Vosd_DrawFontByString();
//	Sample_Vosd_DrawRectAndLine();
//	while(1)
//	{
//		if (2880 == width)
//		{
//			width = 1920;
//			height = 1080;
//		}
//		else
//		{
//			width = 2880;
//			height = 1624;
//		}

//		ret = Sample_Vosd_SetVencResolution(ChnType, width, height);
//		if (ret)
//		{
//			VENC_TRACE_ERR("Sample_Venc_SetVencResolution FAIL(%d)\n",ret);
//			return -1;
//		}

//		ret = Sample_Vosd_AutoSetVencChnEncType(ChnType);
//		if (ret)
//		{
//			VENC_TRACE_ERR("Sample_Venc_SetVencResolution FAIL(%d)\n",ret);
//			return -1;
//		}

//		sleep(1);
//	}
	return 0;
}

int _Test_Set_OSD()
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;

	while(1)
	{
        printf("osd test:\n"
            "\t0 - draw rect&line\n"
            "\t1 - del rect&line\n"
            "\t2 - draw font by bitmap\n"
            "\t3 - del font by bitmap\n"
            "\t4 - draw font by string\n"
            "\t5 - del font by string\n"
            "\t6 - draw font by platform addr\n"
            "\t7 - del font by platform addr\n"
            "\t8 - draw mosaic\n"
            "\t9 - del mosaic\n"
            "\tq - quit\n");

		_Vosd_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		
		switch (nCmd)
		{
			case 0:
				Sample_Vosd_DrawRectAndLine();
				break;
			case 1:
				Sample_Vosd_DelRectAndLine();
				break;
			case 2:
				Sample_Vosd_DrawFontByBitmap();
				break;
			case 3:
				Sample_Vosd_DelFontByBitmap();
				break;
			case 4:
				Sample_Vosd_DrawFontByString();
				break;
			case 5:
				Sample_Vosd_DelFontByString();
				break;
			case 6:
				Sample_Vosd_DrawFontByPlatAddr();
				break;
			case 7:
				Sample_Vosd_DelFontByPlatAddr();
				break;
			case 8:
				Sample_Vosd_DrawMosaic();
				break;
			case 9:
				Sample_Vosd_DelMosaic();
				break;
			default:
				VOSD_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	return ret;
}

int _Test_Set_MD()
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	VDA_MD_PARAM_S param = {0};

	while(1)
	{
        printf("MD test:\n"
            "\t0 - set MD attr\n"
            "\t1 - get MD result\n"
            "\t2 - reset MD chn\n"
            "\tq - quit\n");

		_Vosd_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		
		switch (nCmd)
		{
			case 0:				
				printf("input enable:0-disable;1-enable\n");
				memset(sCmd, 0, sizeof(sCmd));
				_Vosd_Get_Cmd(sCmd);
				param.enable = atoi(sCmd);

				printf("input sen:[1-5]\n");
				memset(sCmd, 0, sizeof(sCmd));
				_Vosd_Get_Cmd(sCmd);
				param.sen = atoi(sCmd);
				
				Sample_Vosd_SetVdaMdParam(&param);
				break;
			case 1:
				Sample_Vosd_GetVdaMdResult();
				break;
			case 2:
				Sample_Vosd_ReSetMdChn();
				break;
			default:
				VOSD_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}

	return ret;
}

int _Test_DECODE_QR()
{
	int ret = 0;
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	VDA_QR_FRAME_INFO_S qrFrm = {0};
	VDA_QR_RESULT_INFO_S qrResult = {0};

	//get frame
	ret = Vpss_Chn_Get_Frm(0, 2, &VFrmAlg, 1000);
	if (ret)
	{
		VOSD_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
		return -1;
	}
	
	//detect
	qrFrm.Width = VFrmAlg.u32Width;
	qrFrm.Height = VFrmAlg.u32Height;
	qrFrm.Size = VFrmAlg.u32Width * VFrmAlg.u32Height;
	qrFrm.pFrmBuf = (UINT8*)VFrmAlg.u64VirAddr[0];

	memset(&qrResult, 0, sizeof(VDA_QR_RESULT_INFO_S));
	VDA_DECODE_QR_INFO(&qrFrm, &qrResult);
	if (VDA_QR_STATE_DECODE_SUCCESS == qrResult.state)
	{
		VDA_TRACE_INFO("qr state=%d\n", qrResult.state);
		VDA_TRACE_INFO("qr len=%d\n", qrResult.payloadLen);
		VDA_TRACE_INFO("qr data=%s\n", qrResult.payload);
	}
	else
	{
		VOSD_TRACE_ERR("decode fail:state(%d)\n", qrResult.state);
	}

	Vpss_Chn_Rel_Frm(0, 2, &VFrmAlg);

	return ret;
}

int _Test_Set_FrmRate()
{
	int ret = 0;
	char sCmd[8] = {0};
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	SINT32 nFrmRate = 0;
	
    printf("input chnType:0-main;1-minor\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	ChnType = atoi(sCmd);//(atoi(sCmd) > 1) ? 0 : atoi(sCmd);

	printf("input FrameRate:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	nFrmRate = atoi(sCmd);

	ret = Venc_Chn_Set_FrmRate(0, ChnType, 0, nFrmRate);
	if (ret)
	{
		VOSD_TRACE_ERR("Venc_Chn_Set_FrmRate FAIL(%d)\n",ret);
		return -1;
	}
	
	return ret;
}

int _Test_Set_WDREnable()
{
	int ret = 0;
	char sCmd[8] = {0};
	int enable = 0;
	SINT32 nFrmRate = 0;
	
    printf("input chnType:0-disable;1-enable\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	enable = atoi(sCmd);

	ret = Visp_Set_WdrEnable(0, enable);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	
	return ret;
}

int _Test_Set_AF()
{
	int ret = 0;
	char sCmd[8] = {0};
	int enable = 0;
	UINT32 nAFMetrics = 0;
	ISP_AFWeight_S afWeight = {0};

	ret = Visp_Get_AFWeight(0, &afWeight);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}

	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			printf("%d ", afWeight.weight[i][j]);
		}
		printf("\n");
	}

	ret = Visp_Get_AFMetrics(0, &nAFMetrics);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	VOSD_TRACE_ERR("Visp_Get_AFMetrics nAFMetrics(%d)\n", nAFMetrics);
	
	ret = Visp_Set_AFWeight(0, &afWeight);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}

	sleep(1);
	ret = Visp_Get_AFMetrics(0, &nAFMetrics);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	VOSD_TRACE_ERR("Visp_Get_AFMetrics nAFMetrics(%d)\n", nAFMetrics);
	
	return ret;
}

int _Test_Switch_BIN()
{
	int ret = 0;
	char sCmd[8] = {0};
	int enable = 0;
	SINT32 nFrmRate = 0;
	
	while(1)
	{
		ret = Visp_Set_TuneConfigFile(0, "/etc/sensor/sc500ai-t41.bin");
		if (ret)
		{
			VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
			return -1;
		}

		sleep(1);

		ret = Visp_Set_TuneConfigFile(0, "/etc/sensor/sc500ai-t41-wl.bin");
		if (ret)
		{
			VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
			return -1;
		}
	}
	return ret;
}

int _Test_Switch_ViFps()
{
	int ret = 0;
	char sCmd[8] = {0};
	int fps = 0;
	
    printf("input Fps:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	fps = atoi(sCmd);

	Vi_Chn_Set_Attr(0, 0, fps);
	
	return ret;
}

int _Test_Switch_VispFlip()
{
	int ret = 0;
	char sCmd[8] = {0};
	XRS_HVFLIP_E HVFlipMode = XRS_FLIP_NORMAL_MODE;	
	
    printf("input FlipMode:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	HVFlipMode = (XRS_HVFLIP_E)atoi(sCmd);

	__Visp_Set_TuneHVFLIP(0, HVFlipMode);
	
	return ret;
}


int _Test_Get_JZInfo()
{
	ISP_TUNE_ALG_INFOR_S param = {0};
	ISP_TUNE_AE_PARAM_S ae = {0};
	char sCmd[8] = {0};
	FLOAT32 ExposureTime = 0.0;
	FLOAT32 MaxExposureTime = 0.0;
	FLOAT32 MinExposureTime = 0.0;
	ISP_TUNE_AE_MODE_E AeMode; 
	
    printf("input ExposureMode:0-auto;1-manual\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	AeMode = (ISP_TUNE_AE_MODE_E)atoi(sCmd);

	if (ISP_TUNE_AE_MANUAL == AeMode)
	{
	    printf("input ExposureTime:\n");
		memset(sCmd, 0, sizeof(sCmd));
		_Vosd_Get_Cmd(sCmd);
		ExposureTime = atof(sCmd);
	}
	else
	{
	    printf("input MaxExposureTime:\n");
		memset(sCmd, 0, sizeof(sCmd));
		_Vosd_Get_Cmd(sCmd);
		MaxExposureTime = atof(sCmd);
		
	    printf("input MinExposureTime:\n");
		memset(sCmd, 0, sizeof(sCmd));
		_Vosd_Get_Cmd(sCmd);
		MinExposureTime = atof(sCmd);
	}
	Visp_Get_TuneCurrInfo(0, &param);
	printf("stable = %d\n", param.Stable);
	ae.AeMode = AeMode;
	ae.ExposureGain = ExposureTime;
	ae.ExposureTime = ExposureTime;
//	ae.MaxExposureTime = MaxExposureTime;
//	ae.MinExposureTime = MinExposureTime;
	ae.FlickerMode = 1;
	ae.ExposureSpeed = 50;
	ae.ExposureLuma = 50;
	ae.FlickerStrength = ISP_TUNE_FLICKER_STRENGTH_LOW;
	
	Visp_Set_TuneAutoExposure(0, &ae);

	
//	mysystem("cat /tmp/alloc_manager_info;cat /tmp/continuous_mem_info");	
//	mysystem("cat /proc/jz/isp/isp-m0");	
//	mysystem("cat /tmp/mempool-pool0;cat /tmp/mempool-pool1");	
//	mysystem("impdbg --sys;impdbg --fs;impdbg --enc_info");
	
	return 0;
}

int _Test_Set_AEWeight()
{
	int ret = 0;
	ISP_AEWeight_Attr_S aeWeight = {0};
	int i = 0;
	int j = 0;

	ret = Visp_Get_AEWeight(0, &aeWeight);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	printf("get default aeWeight:\n");
	for (i = 0; i < 15; i++)
	{
		for (j = 0; j < 15; j++)
		{
			printf("%d ", aeWeight.aeWeight.weight[i][j]);
		}
		printf("\n");
	}
	
	printf("set aeWeight:\n");
	if (aeWeight.enable)
	{
		aeWeight.enable = 0;
	}
	else
	{
		aeWeight.enable = 1;
	}
	for (i = 0; i < 15; i++)
	{
		aeWeight.aeWeight.weight[i][0] = 7;
	}
	ret = Visp_Set_AEWeight(0, &aeWeight);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}

	memset(&aeWeight, 0, sizeof(ISP_AEWeight_Attr_S));
	ret = Visp_Get_AEWeight(0, &aeWeight);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	printf("get aeWeight:\n");
	for (i = 0; i < 15; i++)
	{
		for (j = 0; j < 15; j++)
		{
			printf("%d ", aeWeight.aeWeight.weight[i][j]);
		}
		printf("\n");
	}

	
	return ret;
}


int _Test_Set_AtList()
{
	int ret = 0;
	ISP_ATLIST_S atList = {0};
	int i = 0;

	ret = Visp_Get_AtList(0, &atList);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	printf("get default AtList:\n");
	for (i = 0; i < 15; i++)
	{
		printf("%lld ", atList.at[i]);
	}
	printf("\n");
	printf("set AtList:\n");
	if (atList.enable)
	{
		atList.enable = 0;
	}
	else
	{
		atList.enable = 1;
	}
	for (i = 0; i < 15; i++)
	{
		atList.at[i] = 10;
	}
	ret = Visp_Set_AtList(0, &atList);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_AtList FAIL(%d)\n",ret);
		return -1;
	}

	memset(&atList, 0, sizeof(ISP_ATLIST_S));
	ret = Visp_Get_AtList(0, &atList);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Get_AtList FAIL(%d)\n",ret);
		return -1;
	}
	printf("get AtList:\n");
	for (i = 0; i < 15; i++)
	{
		printf("%lld ", atList.at[i]);
	}
	printf("\n");
	
	return ret;
}

int _Test_Set_CenterZoomTimes()
{
	int ret = 0;
	char sCmd[32] = {0};
	FLOAT64 widthT = 0.0;
	FLOAT64 heightT = 0.0;

	ret = Visp_Get_CenterZoomTimes(0, &widthT, &heightT);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_WdrEnable FAIL(%d)\n",ret);
		return -1;
	}
	
	VOSD_TRACE_INFO("Visp_Get_CenterZoomTimes:widthT=%f,heightT=%f\n", widthT, heightT);

    VOSD_TRACE_INFO("input widthT:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	widthT = atof(sCmd);

    VOSD_TRACE_INFO("input heightT:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Vosd_Get_Cmd(sCmd);
	heightT = atof(sCmd);
	
	VOSD_TRACE_INFO("usr set:widthT=%f,heightT=%f\n", widthT, heightT);
	ret = Visp_Set_CenterZoomTimes(0, &widthT, &heightT);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Set_CenterZoomTimes FAIL(%d)\n",ret);
		return -1;
	}

	ret = Visp_Get_CenterZoomTimes(0, &widthT, &heightT);
	if (ret)
	{
		VOSD_TRACE_ERR("Visp_Get_CenterZoomTimes FAIL(%d)\n",ret);
		return -1;
	}
	
	VOSD_TRACE_INFO("Visp_Get_CenterZoomTimes:widthT=%f,heightT=%f\n", widthT, heightT);
	return ret;
}


#ifdef _JPEG2NV12_

static void I420ToNV12(unsigned char *I420, int width, int height)
{
	int i = 0;
	int j = 0;
	unsigned int yLen = width * height;
	unsigned char* i420u = I420 + width * height;
	unsigned char* i420v = I420 + yLen*5/4;
	unsigned char* pUV = (unsigned char*)malloc(yLen/2);
	if (NULL == pUV)
	{
		return;
	}

	for( i = 0,j = 0;i < yLen/4; i++,j+=2)
	{	
		pUV[j] = i420u[i];
		pUV[j+1] = i420v[i];
	}

	memcpy(I420+yLen, pUV, yLen/2);
	
	free(pUV);
}

static void I422ToNV12(unsigned char *I422, int width, int height)
{
	int i = 0;
	int j = 0;
	unsigned int yLen = width * height;
	unsigned char* i422u = I422 + width * height;
	unsigned char* i422v = I422 + yLen* 3 / 2;
	unsigned char* pUV = (unsigned char*)malloc(yLen/2);
	if (NULL == pUV)
	{
		return;
	}

	for( i = 0,j = 0;i < yLen / 2; i+=2,j+=2)
	{	
		pUV[j] = i422u[i];
		pUV[j+1] = i422v[i];
	}

	memcpy(I422 + yLen, pUV, yLen/2);
	
	free(pUV);
}


static void I444ToNV12(unsigned char *I444, int w, int h) {
	unsigned char *srcY = NULL; 
	unsigned char *srcU = NULL; 
	unsigned char *srcV = NULL;
	unsigned char* pUV = (unsigned char*)malloc(w*h/2);
	if (NULL == pUV)
	{
		return;
	}
	unsigned char* desUV = pUV;
	srcY = I444;//Y
	srcU = srcY + w * h;//U
	srcV = srcU + w * h;//V

	 //UV
	for (int i = 0; i < h / 2; i++) 
	{
		for (int j = 0; j < w / 2; j++) 
		{
			*desUV = *srcU;
			*(desUV+1) = *srcV;
			desUV += 2;
			srcU += 2;
			srcV += 2;
		}
		srcU = srcU + w;
		srcV = srcV + w;
	}
	
	memcpy(I444 + w*h, pUV, w * h / 2);

	free(pUV);
}


int IMage_TransJpeg2NV12(unsigned char* jpegBuf, int jpegSize, unsigned char* yuvBuf, int yuvMaxLen, int* yuvSize, int* yuvWidh, int *yuvHeight)
{
    tjhandle handle = NULL;
    int width = 0;
	int height = 0;
    int widthStride = 0;
	int heightStride = 0;
	int subsample = 0;
	int colorspace = 0;
    int flags = 0;
    int padding = 1; // 1或4均可，但不能是0
    int ret = 0;
	int imageSize = 0;
	
    handle = tjInitDecompress();
    tjDecompressHeader3(handle, jpegBuf, jpegSize, &width, &height, &subsample, &colorspace);
 
    printf("w: %d h: %d subsample: %d color: %d\n", width, height, subsample, colorspace);
	widthStride = ALIGN(width,2);
	heightStride = ALIGN(height,2);
	
    flags |= 0;
    
    // 注：经测试，指定的yuv采样格式只对YUV缓冲区大小有影响，实际上还是按JPEG本身的YUV格式来转换的
    imageSize = tjBufSizeYUV2(widthStride, padding, heightStride, subsample);
	printf("imageSize %d\n", imageSize);
	if (yuvMaxLen < imageSize)
	{
		printf("jpeg resolution is too larger.jpg:w-%d,h-%d\n", width, height);
		tjDestroy(handle);
		return 1;
	}

	ret = tjDecompressToYUV2(handle, jpegBuf, jpegSize, yuvBuf, widthStride,
		   padding, heightStride, flags);
    if (ret < 0)
    {
        printf("compress to jpeg failed: %s\n", tjGetErrorStr());
		tjDestroy(handle);
		return 2;
    }
	
	if (TJSAMP_444 == subsample)
	{
		I444ToNV12(yuvBuf, widthStride, heightStride);
	}
	else if (TJSAMP_422 == subsample)
	{
		I422ToNV12(yuvBuf, widthStride, heightStride);
	}
	else
	{
		I420ToNV12(yuvBuf, widthStride, heightStride);
	}

	*yuvWidh = widthStride;
	*yuvHeight = heightStride;
	*yuvSize = widthStride * heightStride * 3 / 2;
    printf("*yuvWidh %d,*yuvHeight %d\n", *yuvWidh, *yuvHeight);
    tjDestroy(handle);
	
    return 0;
}



int _Test_Jpg2Yuv()
{
	int ret = 0;
	ISP_ATLIST_S atList = {0};
	int i = 0;
	int inFileLen = 0;
	unsigned char *inFileBuf = NULL;
	int yuvStream = -1;
	unsigned char *yuvBuf = NULL;
	int yuvSize = 0;
	int yuvWidth = 0;
	int yuvHeight = 0;
	
    FILE *inFile = NULL;
    inFile = fopen("/mnt/t41/P5A/pic/test.jpg", "rb");
    if (inFile == NULL) 
	{
        printf("fopen src file:%s failed\n", "/mnt/t41/P5A/pic/test.jpg");
		goto ERR;
    }

    fseek(inFile, 0, SEEK_END);
    inFileLen = ftell(inFile);
    rewind(inFile);

	inFileBuf = (unsigned char*)malloc(inFileLen);
	if (NULL == inFileBuf)
	{
		printf("malloc %d fail\n", inFileLen);
		goto ERR;
	}
	
    if (fread(inFileBuf, inFileLen, 1, inFile) != inFileLen) {
        fseek(inFile, 0, SEEK_SET);
        fread(inFileBuf, 1, inFileLen, inFile);
    }
	
    fclose(inFile);
	inFile = NULL;

	yuvBuf = ( unsigned char*)malloc(1024*1024);

	ret = IMage_TransJpeg2NV12(inFileBuf, inFileLen, yuvBuf, 1024*1024, &yuvSize, &yuvWidth, &yuvHeight);
	if (0 == ret)
	{
		yuvStream = open("/mnt/t41/P5A/pic/yuv1.yuv", O_RDWR | O_CREAT | O_TRUNC, 0644);
		if (yuvSize != write(yuvStream, yuvBuf, yuvSize)) 
		{
			printf("stream write failed\n");
			goto ERR;
		}
		close(yuvStream);
		yuvStream = -1;
	}

ERR:
	if (inFile)
	{
		fclose(inFile);
		inFile = NULL;
	}

	if (yuvStream > 0)
	{
		close(yuvStream);
		yuvStream = -1;
	}
	
	if (yuvBuf)
	{
		free(yuvBuf);
		yuvBuf = NULL;
	}

	if (inFileBuf)
	{
		free(inFileBuf);
		inFileBuf = NULL;
	}
	
	return ret;
}
#endif

int main(int argc, char *argv[])
{
	int ret = 0;
	char sCmd[8] = {0};
	int nCmd = 0;
	int resetCnt = 0;
	
START:
	ret = Sample_Vosd_InitModule();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Venc_InitModule FAIL(%d)\n",ret);
		return -1;
	}

	ret = Sample_Vosd_StartModule();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_StartModule FAIL(%d)\n",ret);
		return -1;
	}

//	while(1)
//	{
//		sleep(1);
//	}
	while(1)
	{
        printf("test:\n"
            "\t0 - get ai vpss frame (path:/tmp/)\n"
            "\t1 - set enc attr(except resolution)\n"
            "\t2 - set enc resolution\n"
            "\t3 - set osd\n"
            "\t4 - set MD\n"
            "\t5 - decode QR\n"
            "\t6 - set enc frmRate\n"
            "\t7 - set wdr enable\n"
            "\t8 - set AF\n"
            "\t9 - switch bin\n"
            "\t10 - switch Vi Fps\n"
            "\t11 - get JZ Sys Info\n"
            "\t12 - set ispFlip\n"
            "\t13 - set aeWeight\n"
            "\t14 - set atlist\n"
            "\t15 - set centerZoomTimes\n"
            "\tq - quit\n");

		_Vosd_Get_Cmd(sCmd);
		
		if ((sCmd[0] == 'q') || (sCmd[0] == 'Q'))
		{
			TRACE_DBG("get cmd to exit \n");
			break;
		}
		nCmd = atoi(sCmd);
		
		switch (nCmd)
		{
			case 0:
				Sample_Vosd_GetVpssFrame(2);
				break;
			case 1:
				_Test_Set_VencAttr();
				break;
			case 2:
				_Test_Set_VencResolution();
				break;
			case 3:
				_Test_Set_OSD();
				break;
			case 4:
				_Test_Set_MD();
				break;
			case 5:
				_Test_DECODE_QR();
				break;
			case 6:
				_Test_Set_FrmRate();
				break;
			case 7:
				_Test_Set_WDREnable();
				break;
			case 8:
				_Test_Set_AF();
				break;
			case 9:
				_Test_Switch_BIN();
				break;
			case 10:
				_Test_Switch_ViFps();
				break;
			case 11:
				_Test_Get_JZInfo();
				break;
			case 12:
				_Test_Switch_VispFlip();
				break;
			case 13:
				_Test_Set_AEWeight();
				break;
			case 14:
				_Test_Set_AtList();
				break;
			case 15:
				_Test_Set_CenterZoomTimes();
				break;
			default:
				VOSD_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
				break;
		}
		
	}
	
	ret = Sample_Vosd_StopModule();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_StopModule FAIL(%d)\n", ret);
		return -1;
	}

	ret = Sample_Vosd_DeInitModule();
	if (ret)
	{
		VOSD_TRACE_ERR("Sample_Vosd_DeInitModule FAIL(%d)\n", ret);
		return -1;
	}

	resetCnt++;
	VOSD_TRACE_INFO("resetCnt(%d)\n", resetCnt);
	goto START;
	
	return 0;
}


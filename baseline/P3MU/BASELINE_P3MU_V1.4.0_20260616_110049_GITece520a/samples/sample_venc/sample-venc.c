
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "mcf.h"
#include "msstd.h"
#include "rtsp.h"
#include "vcomm.h"
#include "vda.h"
#include "venc.h"
#include "vi.h"
#include "visp.h"
#include "vosd.h"
#include "vpss.h"

#include "xrsdef.h"
#include "xtrace.h"

#include "ao.h"
#include "ai.h"
#include "aenc.h"
#include "adec.h"
#include "halmod.h"

#if defined(__SUPPORT_FFMPEG__) || defined(__SUPPORT_OPENH264__)
#include "vdec.h"
#include "lcd.h"
#endif

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

#define STREAM_FILE_SAVE_PATH "/nfsroot"

#if	defined(SNR_JXK308P) || defined(SNR_IMX415) || defined(SNR_OS08E10) || defined(SNR_CS6286)
#define SENSOR_WIDTH				3840
#define SENSOR_HEIGHT				2160
#elif defined(SNR_JXK362P)
#define SENSOR_WIDTH				3200
#define SENSOR_HEIGHT				1800
#elif defined(SNR_GC5623)
#define SENSOR_WIDTH				2880
#define SENSOR_HEIGHT				1620
#elif defined(SNR_OS04D10) || defined(SNR_OS03A10) || defined(SNR_OS04L10) || defined(SNR_JXK06A) || defined(SNR_GC4053)
#define SENSOR_WIDTH				2560
#define SENSOR_HEIGHT				1440
#elif defined(SNR_Q03)
#define SENSOR_WIDTH				2304
#define SENSOR_HEIGHT				1296
#elif defined(SNR_GC2083) || defined(SNR_F38P) || defined(SNR_GC20B3) || defined(SNR_F38PA) || defined(SNR_GC20C3)
#define SENSOR_WIDTH				1920
#define SENSOR_HEIGHT				1080
#elif defined(SNR_IMX335)
#define IMX335_5M_4LANE_LINER//IMX335_2M_60FPS_4LANE //IMX335_5M_4LANE_WDR //IMX335_5M_4LANE_LINER
#ifdef IMX335_2M_60FPS_4LANE
#define SENSOR_WIDTH				1920
#define SENSOR_HEIGHT				1080
#else
#define SENSOR_WIDTH				2592
#define SENSOR_HEIGHT				1944
#endif
#endif

#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
#define MAIN_ENC_WIDTH				3840
#define MAIN_ENC_HEIGHT				2160
#define MAIN_ENC_STREAM_BUF_SIZE 	1536*1024
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_6MP__)
#define MAIN_ENC_WIDTH				3200
#define MAIN_ENC_HEIGHT				1800
#define MAIN_ENC_STREAM_BUF_SIZE 	1024*1024
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_6MP_3_2__)
#define MAIN_ENC_WIDTH				3072
#define MAIN_ENC_HEIGHT				2048
#define MAIN_ENC_STREAM_BUF_SIZE 	1024*1024
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_5MP__)
#define MAIN_ENC_WIDTH				2880
#define MAIN_ENC_HEIGHT				1620
#define MAIN_ENC_STREAM_BUF_SIZE 	768*1024
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_5MP_4_3__)
#ifdef IMX335_2M_60FPS_4LANE
#define MAIN_ENC_WIDTH				1920
#define MAIN_ENC_HEIGHT				1080
#define MAIN_ENC_STREAM_BUF_SIZE 	320*1024
#else
#define MAIN_ENC_WIDTH				2592
#define MAIN_ENC_HEIGHT				1944
#define MAIN_ENC_STREAM_BUF_SIZE 	768*1024
#endif
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_4MP__)
#define MAIN_ENC_WIDTH				2560
#define MAIN_ENC_HEIGHT				1440
#define MAIN_ENC_STREAM_BUF_SIZE 	512*1024
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_3MP__)
#define MAIN_ENC_WIDTH				2304
#define MAIN_ENC_HEIGHT				1296
#define MAIN_ENC_STREAM_BUF_SIZE 	384*1024
#elif defined(__SUPPORT_MAX_MAIN_ENC_RESOLUTION_2MP__)
#define MAIN_ENC_WIDTH				1920
#define MAIN_ENC_HEIGHT				1080
#define MAIN_ENC_STREAM_BUF_SIZE 	320*1024
#else
#define MAIN_ENC_WIDTH				2560
#define MAIN_ENC_HEIGHT				1440
#define MAIN_ENC_STREAM_BUF_SIZE 	512*1024
#endif

#ifdef __SUPPORT_MAX_MINOR_ENC_RESOLUTION_D1__
#define MINOR_ENC_WIDTH				704
#define MINOR_ENC_HEIGHT			576
#define MINOR_ENC_STREAM_BUF_SIZE 	128*1024
#elif defined(__SUPPORT_MAX_MINOR_ENC_RESOLUTION_448P__)
#define MINOR_ENC_WIDTH				800
#define MINOR_ENC_HEIGHT			448
#define MINOR_ENC_STREAM_BUF_SIZE 	128*1024
#elif defined(__SUPPORT_MAX_MINOR_ENC_RESOLUTION_360P__)
#define MINOR_ENC_WIDTH				640
#define MINOR_ENC_HEIGHT			360
#define MINOR_ENC_STREAM_BUF_SIZE 	128*1024
#elif defined(__SUPPORT_MAX_MINOR_ENC_RESOLUTION_720P__)
#define MINOR_ENC_WIDTH				1280
#define MINOR_ENC_HEIGHT			720
#define MINOR_ENC_STREAM_BUF_SIZE 	320*1024
#else
#define MINOR_ENC_WIDTH				800
#define MINOR_ENC_HEIGHT			448
#define MINOR_ENC_STREAM_BUF_SIZE 	128*1024
#endif

#ifdef __SUPPORT_AI_ALGO_INPUT_HS_1920_1080_YVU420__
#define AI_CHN_WIDTH				1920
#define AI_CHN_HEIGHT				1080
#elif defined(__SUPPORT_AI_ALGO_INPUT_HS_640_384_YVU420__)
#define AI_CHN_WIDTH				640
#define AI_CHN_HEIGHT				384
#elif defined(__SUPPORT_AI_ALGO_INPUT_HS_768_448_YUV400__)
#define AI_CHN_WIDTH				768
#define AI_CHN_HEIGHT				448
#elif defined(__SUPPORT_AI_ALGO_INPUT_HS_320_192_YUV400__)
#define AI_CHN_WIDTH				320
#define AI_CHN_HEIGHT				192
#else
#define AI_CHN_WIDTH				1024
#define AI_CHN_HEIGHT				576
#endif

#if defined(BOARD_H82W) || defined(BOARD_G82W)
#define GROUP_NUMBER                2
#define MAX_FRAMERATE               20
#else
#define GROUP_NUMBER                1
#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
#define MAX_FRAMERATE               20
#else
#ifdef IMX335_2M_60FPS_4LANE
#define MAX_FRAMERATE               60
#else
#define MAX_FRAMERATE               25
#endif
#endif
#endif

typedef struct SampleViChnInfo {
    SINT32 ViChn; /* 按物理连接的序号 */
    UINT16 Width;
    UINT16 Height;
    UINT16 CropX;
    UINT16 CropY;
    VISP_SENSOR_CONF_S SnsConf;
} SAMPLE_VI_CHN_INFO_S;

typedef struct SampleViInfo {
    SINT32 ViChnNum;
    SINT32 Framerate;
    SAMPLE_VI_CHN_INFO_S ChnInfo[2];
} SAMPLE_VI_INFO_S;

SAMPLE_VI_INFO_S gViInfo = {
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    .ViChnNum = 2,
    .Framerate = MAX_FRAMERATE,
#else
    .ViChnNum = 1,
    .Framerate = MAX_FRAMERATE,
#endif
    .ChnInfo =
        {
            {
                0,
                SENSOR_WIDTH,
                SENSOR_HEIGHT,
                0,
                0,
                {
                    .BusId = 0,
				#ifdef SNR_OS04D10
                    .Type = VISP_SENSOR_OS04D10_MIPI_4M_30FPS_10BIT,
				#elif SNR_OS03A10
					#if defined(BOARD_H4SVX) || defined(BOARD_H4SCG)
					.Type = VISP_SENSOR_OS03A10_MIPI_4M_30FPS_12BIT_2LANE,
					#else
                    .Type = VISP_SENSOR_OS03A10_MIPI_4M_30FPS_12BIT,
                    #endif
				#elif SNR_GC2083
                    .Type = VISP_SENSOR_GC2083_MIPI_2M_30FPS_10BIT,
				#elif SNR_GC20B3
					.Type = VISP_SENSOR_GC20B3_MIPI_2M_30FPS_10BIT,
				#elif SNR_JXK308P
					.Type = VISP_SENSOR_JXK308P_MIPI_8M_30FPS_10BIT,
				#elif SNR_F38P
					.Type = VISP_SENSOR_JXF38P_MIPI_2M_30FPS_10BIT,
				#elif SNR_F38PA
					.Type = VISP_SENSOR_JXF38PA_MIPI_2M_30FPS_10BIT,
                #elif SNR_Q03
					.Type = VISP_SENSOR_JXQ03_MIPI_3M_30FPS_10BIT,
				#elif SNR_IMX415
                    #if defined(BOARD_H8SXV2)
					.Type = VISP_SENSOR_IMX415_MIPI_8M_30FPS_10BIT_4LANE,
                    #else
					.Type = VISP_SENSOR_IMX415_MIPI_8M_30FPS_10BIT,
                    #endif
                #elif SNR_IMX335
				#ifdef IMX335_2M_60FPS_4LANE
					.Type = VISP_SENSOR_IMX335_MIPI_2M_60FPS_10BIT_4LANE,
				#elif defined(IMX335_5M_4LANE_WDR)
					.Type = VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1_4LANE,
				#elif defined(IMX335_5M_4LANE_LINER)
					.Type = VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT_4LANE,
				#else
					.Type = VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT,
				#endif
				#elif SNR_OS04L10
                    .Type = VISP_SENSOR_OS04L10_MIPI_4M_30FPS_10BIT,
                #elif SNR_OS08E10
                    #if defined(BOARD_H8SVX) 
                    .Type = VISP_SENSOR_OS08E10_MIPI_8M_20FPS_10BIT_2LANE,
                    #else
					.Type = VISP_SENSOR_OS08E10_MIPI_8M_20FPS_10BIT_4LANE,
                    #endif
				#elif SNR_JXK06A
                    .Type = VISP_SENSOR_JXK06A_MIPI_4M_30FPS_10BIT_2LANE,
                #elif SNR_GC20C3
                   .Type = VISP_SENSOR_GC20C3_MIPI_2M_30FPS_10BIT_2LANE,
                #elif SNR_CS6286
                   .Type = VISP_SENSOR_CS6286_MIPI_8M_30FPS_12BIT_DCG_4LANE,
                #elif SNR_GC4053
                    .Type = VISP_SENSOR_GC4053_MIPI_4M_30FPS_10BIT_2LANE,
                #elif SNR_GC5623
                    .Type = VISP_SENSOR_GC5623_MIPI_5M_30FPS_10BIT_4LANE,
				#else
                    .Type = VISP_SENSOR_JXK362P_MIPI_6M_30FPS_10BIT,
				#endif
                    .Mode = VISP_SENSOR_MODE_RGB,
                },
            },
			#if defined(BOARD_H82W) || defined(BOARD_G82W)
            {
                1,
                SENSOR_WIDTH,
                SENSOR_HEIGHT,
                0,
                0,
                {
                    .BusId = 1,
				#ifdef SNR_OS04D10
                    .Type = VISP_SENSOR_OS04D10_MIPI_4M_30FPS_10BIT,
				#elif SNR_OS03A10
                    .Type = VISP_SENSOR_OS03A10_MIPI_4M_30FPS_12BIT,
				#elif SNR_GC2083
                    .Type = VISP_SENSOR_GC2083_MIPI_2M_30FPS_10BIT,
				#elif SNR_GC20B3
					.Type = VISP_SENSOR_GC20B3_MIPI_2M_30FPS_10BIT,
				#elif SNR_JXK308P
					.Type = VISP_SENSOR_JXK308P_MIPI_8M_30FPS_10BIT,
				#elif SNR_OS04L10
					.Type = VISP_SENSOR_OS04L10_MIPI_4M_30FPS_10BIT,
				#else
                    .Type = VISP_SENSOR_JXK362P_MIPI_6M_30FPS_10BIT,
				#endif
                    .Mode = VISP_SENSOR_MODE_RGB,
                },
            },
            #endif
        },
};

#define MCF_CHN_MAX_NUM 1
typedef struct SampleMcfPipeInfo {
    SINT32 Pipe; /* 对应vi */
    SINT32 ViChn;
    UINT16 Width;
    UINT16 Height;
} SAMPLE_MCF_PIPE_INFO_S;

typedef struct SampleMcfChnInfo {
    SINT32 Chn;
    UINT16 Width;
    UINT16 Height;
} SAMPLE_MCF_CHN_INFO_S;

typedef struct SampleMcfInfo {
    SINT32 McfGrp;
    SAMPLE_MCF_PIPE_INFO_S ColorPipe;
    SAMPLE_MCF_PIPE_INFO_S MonoPipe;
    SINT32 ChnNum;
    SAMPLE_MCF_CHN_INFO_S ChnInfo[MCF_CHN_MAX_NUM];
} SAMPLE_MCF_INFO_S;

SAMPLE_MCF_INFO_S gMcfInfo = {
    .McfGrp = 0,
    .ColorPipe = {0, 0, 1920, 1080},
    .MonoPipe = {1, 1, 2560, 1440},
    .ChnNum = 1,
    .ChnInfo = {{0, 2560, 1440}},
};

typedef struct SampleVpssGrpInfo {
    SINT32 Grp;
    UINT16 Width;
    UINT16 Height;
} SAMPLE_VPSS_GRP_INFO_S;

typedef struct SampleVpssChnInfo {
    SINT32 Chn;
    UINT16 Width;
    UINT16 Height;
} SAMPLE_VPSS_CHN_INFO_S;

typedef struct SampleVpssInfo {
    SAMPLE_VPSS_GRP_INFO_S GrpInfo;
    SINT32 ChnNum;
    SAMPLE_VPSS_CHN_INFO_S ChnInfo[4];
} SAMPLE_VPSS_INFO_S;

SAMPLE_VPSS_INFO_S gVpssInfo[GROUP_NUMBER] = {
    {
        {0, SENSOR_WIDTH, SENSOR_HEIGHT},
        3,
        {
            {
                0,
                MAIN_ENC_WIDTH,
                MAIN_ENC_HEIGHT,
            },
            {1, MINOR_ENC_WIDTH, MINOR_ENC_HEIGHT},
            {2, AI_CHN_WIDTH, AI_CHN_HEIGHT},
        },
    },
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    {
        {1, SENSOR_WIDTH, SENSOR_HEIGHT},
        3,
        {
            {
                0,
                MAIN_ENC_WIDTH,
                MAIN_ENC_HEIGHT,
            },
            {1, MINOR_ENC_WIDTH, MINOR_ENC_HEIGHT},
            {2, AI_CHN_WIDTH, AI_CHN_HEIGHT},
        },
    }
#endif
};

typedef struct SampleVencChnInfo {
    SINT32 ChnType;
    UINT16 Width;
    UINT16 Height;
	UINT8  MjpegFollowType;
} SAMPLE_VENC_CHN_INFO_S;

typedef struct SampleVencInfo {
    SINT32 ChnNum;
    SAMPLE_VENC_CHN_INFO_S ChnInfo[7];
} SAMPLE_VENC_INFO_S;

SAMPLE_VENC_INFO_S gVencInfo = {
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    .ChnNum = 6,
#else
    .ChnNum = 3,
#endif
    .ChnInfo =
        {
            {VENC_CHNTYPE_MAIN, MAIN_ENC_WIDTH, MAIN_ENC_HEIGHT},
            {VENC_CHNTYPE_MINOR, MINOR_ENC_WIDTH, MINOR_ENC_HEIGHT},
            {VENC_CHNTYPE_SNAP, MINOR_ENC_WIDTH, MINOR_ENC_HEIGHT},
			#if defined(BOARD_H82W) || defined(BOARD_G82W)
            {VENC_CHNTYPE_MAIN_SEC, MAIN_ENC_WIDTH, MAIN_ENC_HEIGHT},
            {VENC_CHNTYPE_MINOR_SEC, MINOR_ENC_WIDTH, MINOR_ENC_HEIGHT},
            {VENC_CHNTYPE_SNAP_SEC, MINOR_ENC_WIDTH, MINOR_ENC_HEIGHT},
            #endif
        },
};

rtsp_demo_handle demo;
rtsp_session_handle session[6] = {NULL};

static UINT64 get_ms()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (UINT64)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;
}

static SINT32 _Sample_StartVi()
{
    SINT32 Ret = 0, i = 0;
    VIDEO_CHIP_CONF_S ChipConf = {0};
    VI_CONF_S ViConf = {0};

    ViConf.MaxViChn = gViInfo.ViChnNum;
    ViConf.MaxSnsNum = gViInfo.ViChnNum;
    for (i = 0; i < gViInfo.ViChnNum; i++) {
        memcpy(&ViConf.SnsConf[i], &gViInfo.ChnInfo[i].SnsConf, sizeof(VISP_SENSOR_CONF_S));
    }
#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
	ViConf.VSize = (gViInfo.ChnInfo[0].Width == 3840) ? VIDEO_PIC_HD4K : VIDEO_PIC_HD600W;
#endif
    Ret = Vi_Open(ViConf.MaxViChn, ChipConf, ViConf);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Vi_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Vi_Start(0, 0);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Vi_Open failed, ret = %d\n", Ret);
        goto close_vi;
    }

    for (i = 0; i < gViInfo.ViChnNum; i++) {
        VI_CHN_CONF_S ViChnConf = {0};
        ViChnConf.width = gViInfo.ChnInfo[i].Width;
        ViChnConf.height = gViInfo.ChnInfo[i].Height;
        ViChnConf.CropX = gViInfo.ChnInfo[i].CropX;
        ViChnConf.CropY = gViInfo.ChnInfo[i].CropY;
        ViChnConf.MainFrame = gViInfo.Framerate;
        if (Vi_Chn_Open(0, i, ViChnConf) != 0) {
            TRACE_ERR(COMP_VIDEOIN, "Vi_Chn_Open failed, ret = %d\n", Ret);
            goto close_vi_chn;
        }
    }

    return 0;

close_vi_chn:
    for (--i; i >= 0; i--) Vi_Chn_Close(0, i);
    // stop_vi:
    Vi_Stop(0);

close_vi:
    Vi_Close(0);

    return -1;
}

static void _Sample_StopVi(void)
{
    int i = 0;

    for (i = 0; i < gViInfo.ViChnNum; i++) Vi_Chn_Close(0, i);
    Vi_Stop(0);
    Vi_Close(0);
}

static SINT32 _Sample_StartVisp()
{
    SINT32 Ret = 0;
    VIDEO_CHIP_CONF_S ChipConf = {0};
    VISP_CONF_S Conf = {
        .SnsNum = 1,
    };
    VISP_CONTROL_CALLBACK_S ControlCb;

    Ret = Visp_Open(1, ChipConf, ControlCb);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Vi_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Visp_Start(0, Conf);
    if (Ret != 0) {
        Visp_Close(0);
        TRACE_ERR(COMP_VIDEOIN, "Visp_Start failed, ret = %d\n", Ret);
        return -1;
    }

    return 0;
}

static void _Sample_StopVisp(void)
{
    Visp_Stop(0);
    Visp_Close(0);
}
#ifdef __MCF__
static SINT32 _Sample_StartMcf()
{
    SINT32 Ret = 0, i = 0;
    MCF_GRP_ATTR_S McfGrpAttr = {0};

    Ret = Mcf_Open();
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Mcf_Open failed, ret = %d\n", Ret);
        return -1;
    }

    McfGrpAttr.Width = gMcfInfo.MonoPipe.Width;   /* max of all pipe */
    McfGrpAttr.Height = gMcfInfo.MonoPipe.Height; /* max of all pipe */

    McfGrpAttr.ColorPipe.PipeId = gMcfInfo.ColorPipe.Pipe;
    McfGrpAttr.ColorPipe.Width = gMcfInfo.ColorPipe.Width;
    McfGrpAttr.ColorPipe.Height = gMcfInfo.ColorPipe.Height;
    McfGrpAttr.ColorPipe.VpssGrp = 0; /* TODO */
    McfGrpAttr.ColorPipe.Depth = 15;  /* TODO */

    McfGrpAttr.MonoPipe.PipeId = gMcfInfo.MonoPipe.Pipe;
    McfGrpAttr.MonoPipe.Width = gMcfInfo.MonoPipe.Width;
    McfGrpAttr.MonoPipe.Height = gMcfInfo.MonoPipe.Height;
    McfGrpAttr.MonoPipe.VpssGrp = 1; /* TODO */
    McfGrpAttr.MonoPipe.Depth = 15;  /* TODO */

    Ret = Mcf_StartGrp(0, gMcfInfo.McfGrp, &McfGrpAttr);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Mcf_StartGrp failed, ret = %d\n", Ret);
        goto close_mcf;
    }

    for (i = 0; i < gMcfInfo.ChnNum; i++) {
        MCF_CHN_ATTR_S McfChnAttr = {0};
        McfChnAttr.Width = gMcfInfo.ChnInfo[i].Width;
        McfChnAttr.Height = gMcfInfo.ChnInfo[i].Height;

        Ret = Mcf_StartChn(0, gMcfInfo.McfGrp, gMcfInfo.ChnInfo[i].Chn, &McfChnAttr);
        if (Ret != 0) {
            TRACE_ERR(COMP_VIDEOIN, "Mcf_StartGrp failed, ret = %d\n", Ret);
            goto stop_mcf_grp;
        }
    }

    Ret = Mcf_BindVi(gMcfInfo.McfGrp, gMcfInfo.ColorPipe.Pipe, gMcfInfo.ColorPipe.ViChn);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Mcf_BindVi failed, ret = %d\n", Ret);
        goto stop_mcf_chn;
    }

    Ret = Mcf_BindVi(gMcfInfo.McfGrp, gMcfInfo.MonoPipe.Pipe, gMcfInfo.MonoPipe.ViChn);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Mcf_BindVi failed, ret = %d\n", Ret);
        goto unbind_color_vi;
    }

    return 0;

unbind_color_vi:
    Mcf_UnBindVi(gMcfInfo.McfGrp, gMcfInfo.ColorPipe.Pipe, gMcfInfo.ColorPipe.ViChn);
stop_mcf_chn:
    for (--i; i >= 0; i--) Mcf_StopChn(0, gMcfInfo.McfGrp, gMcfInfo.ChnInfo[i].Chn);
stop_mcf_grp:
    Mcf_StopGrp(0, gMcfInfo.McfGrp);

close_mcf:
    Mcf_Close();
    return -1;
}

static void _Sample_StopMcf()
{
    int i = 0;

    Mcf_UnBindVi(gMcfInfo.McfGrp, gMcfInfo.MonoPipe.Pipe, gMcfInfo.MonoPipe.ViChn);
    Mcf_UnBindVi(gMcfInfo.McfGrp, gMcfInfo.ColorPipe.Pipe, gMcfInfo.ColorPipe.ViChn);
    for (i = 0; i < gMcfInfo.ChnNum; i++) Mcf_StopChn(0, gMcfInfo.McfGrp, gMcfInfo.ChnInfo[i].Chn);
    Mcf_StopGrp(0, gMcfInfo.McfGrp);
    Mcf_Close();
}
#endif
static SINT32 _Sample_StartVpss()
{
    SINT32 Ret = 0, i = 0, j = 0;
    VIDEO_CHIP_CONF_S ChipConf = {0};
    VPSS_GRP_ATTR_S VpssGrpAttr = {0};
    VPSS_CHN_ATTR_S VpssChnAttr = {0};

    Ret = Vpss_Open(gVpssInfo[0].ChnNum, ChipConf);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Vpss_Open failed, ret = %d\n", Ret);
        return -1;
    }

#ifdef __MCF__
    Ret = _Sample_StartMcf();
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Vpss_Open failed, ret = %d\n", Ret);
        goto sample_close_vpss;
    }
#endif
    for(j = 0; j < GROUP_NUMBER; j++){
        VpssGrpAttr.Width = gVpssInfo[j].GrpInfo.Width;
        VpssGrpAttr.Height = gVpssInfo[j].GrpInfo.Height;
        VpssGrpAttr.EnMcf = 0;
        VpssGrpAttr.EnScale = 0;
        Ret = Vpss_Grp_Open(0, gVpssInfo[j].GrpInfo.Grp, &VpssGrpAttr);
        if (Ret != 0) {
            TRACE_ERR(COMP_VIDEOIN, "Vpss_Open failed, ret = %d\n", Ret);
            goto sample_close_vpss;
        }
#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
        for (i = gVpssInfo[j].ChnNum - 1; i >= 0; i--) {
#else
		for (i = 0; i < gVpssInfo[j].ChnNum; i++) {
#endif
            memset(&VpssChnAttr, 0, sizeof(VpssChnAttr));
            VpssChnAttr.width = gVpssInfo[j].ChnInfo[i].Width;
            VpssChnAttr.height = gVpssInfo[j].ChnInfo[i].Height;
#if defined(BOARD_H82W) || defined(BOARD_G82W)
            if (i > 1) VpssChnAttr.Depth = 1;
#else
            if (i > 0) VpssChnAttr.Depth = 1;
#endif
            Ret = Vpss_Chn_Open(0, gVpssInfo[j].GrpInfo.Grp, gVpssInfo[j].ChnInfo[i].Chn, VpssChnAttr);
            if (Ret != 0) {
                TRACE_ERR(COMP_VIDEOIN, "Vpss_Chn_Open failed, ret = %d\n", Ret);
                goto sample_close_vpss_chn;
            }
        }
    }

    return 0;
sample_close_vpss_chn:
    for(j; j >= 0; j--){
        for (--i; i >= 0; i--) Vpss_Chn_Close(0, gVpssInfo[j].GrpInfo.Grp, gVpssInfo[j].ChnInfo[i].Chn);
        // sample_close_vpss_grp:
        Vpss_Grp_Close(0, gVpssInfo[j].GrpInfo.Grp);
        i = 2;
    }
    
#ifdef __MCF__
sample_stop_mcf:
    _Sample_StartMcf();
#endif
sample_close_vpss:
    Vpss_Close(0);
    return -1;
}

static void _Sample_StopVpss()
{
    int i = 0, j = 0;
    for(j = 0; j < GROUP_NUMBER; j++){
        for (i = 0; i < gVpssInfo[j].ChnNum; i++) Vpss_Chn_Close(0, gVpssInfo[j].GrpInfo.Grp, gVpssInfo[j].ChnInfo[i].Chn);
        Vpss_Grp_Close(0, gVpssInfo[j].GrpInfo.Grp);
    }

#ifdef __MCF__
    _Sample_StopMcf();
#endif
    Vpss_Close(0);
}

static int _IndexByChnType(VENC_CHNTYPE_E ChnType)
{
    int i = 0;
    for (i = 0; i < gVencInfo.ChnNum; i++) {
        if (ChnType == gVencInfo.ChnInfo[i].ChnType) return i;
    }

    return -1;
}

static SINT32 _Video_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, char *data, SINT32 size)
{
    int ret = 0;
//    static int fd[3] = {-1, -1, -1};
//    char frameFilename[64] = {0};
    MSSTD_S *pMSSTD = (MSSTD_S *)pHead;
//    int writeRet = 0;

    if (MSSTD_ALGORITHM_VIDEO_JPEG_STANDARD == pMSSTD->Algorithm) {
#if 0
        // jpeg图片保存
        VENC_TRACE_ERR("stype=%d,encType=%d,size=%d\n", stype, pMSSTD->Algorithm, size);
        memset(frameFilename, 0, sizeof(frameFilename));
        snprintf(frameFilename, sizeof(frameFilename), "%s/chn%d-seq%d.jpeg", STREAM_FILE_SAVE_PATH, stype,
                 pMSSTD->FrmCnt);
        fd[stype] = open(frameFilename, O_RDWR | O_CREAT, 0x644);
        if (fd[stype] < 0) {
            VENC_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
            return -1;
        }

        writeRet = write(fd[stype], (void *)data, size);
        if (writeRet != size) {
            VENC_TRACE_ERR("write frame failed(%d,%d)\n", writeRet, size);
            return -1;
        }

        close(fd[stype]);
        fd[stype] = -1;
#endif
    } else {
#if 0
        // 视频流保存
        if (-1 == fd[stype]) {
            memset(frameFilename, 0, sizeof(frameFilename));
            if (MSSTD_ALGORITHM_VIDEO_H265_GENERAL == pMSSTD->Algorithm) {
                snprintf(frameFilename, sizeof(frameFilename), "%s/chn%d.h265", STREAM_FILE_SAVE_PATH, stype);
            } else {
                snprintf(frameFilename, sizeof(frameFilename), "%s/chn%d.h264", STREAM_FILE_SAVE_PATH, stype);
            }

            fd[stype] = open(frameFilename, O_RDWR | O_CREAT, 0x644);
            if (fd[stype] < 0) {
                VENC_TRACE_ERR("open %s failed:%s\n", frameFilename, strerror(errno));
                return -1;
            }
        }

        writeRet = write(fd[stype], (void *)data, size);
        if (writeRet != size) {
            VENC_TRACE_ERR("write frame failed(%d,%d)\n", writeRet, size);
            return -1;
        }
#else
        int i = _IndexByChnType(stype);
        if (i >= 0) {
            rtsp_sever_tx_video(demo, session[i], (uint8_t *)data, size,
                                pMSSTD->TimeSec * 1000 * 1000 + pMSSTD->TimeMSec * 1000);
        }
#endif
    }
    return ret;
}

static int Sample_Venc_InitVencChnAttr(SAMPLE_VENC_CHN_INFO_S *pChnInfo, VENC_ENCODEPARAM_S *pEncParam, UINT32 *SrcFrameRate)
{
    if (VENC_CHNTYPE_MAIN == pChnInfo->ChnType) {
        // 一条通路上，venc与fs分辨率需一致
        pEncParam->VPayloadType = VPAYLOAD_TYPE_H265;
        pEncParam->width = pChnInfo->Width;
        pEncParam->height = pChnInfo->Height;
        pEncParam->BitRate = 2048;
        pEncParam->MaxBitRate = pEncParam->BitRate * 2;
        pEncParam->FrameRate = gViInfo.Framerate;
        pEncParam->Quality = 0;
        pEncParam->KeyInterval = pEncParam->FrameRate * 4;
        pEncParam->BitRateType = BITRATE_TYPE_ABR;
        pEncParam->bOffLine = 0;
        pEncParam->stVencQpConf.bEnbaleQp = 1;
        pEncParam->stVencQpConf.u8MaxPictureQuality = 41;
        pEncParam->stVencQpConf.u16MaxPictureSize = 768;
        pEncParam->stVencQpConf.u32MaxQp = 51;
        pEncParam->stVencQpConf.u32MinQp = 28;
        pEncParam->stVencQpConf.u32MaxIQp = 51;
        pEncParam->stVencQpConf.u32MinIQp = 28;
        pEncParam->StreamBufSize = MAIN_ENC_STREAM_BUF_SIZE;
		
		pEncParam->SmartEncEnable = 0;
		pEncParam->BgInterval = 30 * pEncParam->KeyInterval;
    } else if (VENC_CHNTYPE_MINOR == pChnInfo->ChnType) {
        pEncParam->VPayloadType = VPAYLOAD_TYPE_H265;
        pEncParam->width = pChnInfo->Width;
        pEncParam->height = pChnInfo->Height;
        pEncParam->BitRate = 1024;
        pEncParam->MaxBitRate = pEncParam->BitRate * 2;
        pEncParam->FrameRate = gViInfo.Framerate;
        pEncParam->Quality = 0;
        pEncParam->KeyInterval = pEncParam->FrameRate * 4;
        pEncParam->BitRateType = BITRATE_TYPE_VBR;
        pEncParam->bOffLine = 0;
        pEncParam->stVencQpConf.bEnbaleQp = 1;
        pEncParam->stVencQpConf.u8MaxPictureQuality = 40;
        pEncParam->stVencQpConf.u16MaxPictureSize = 256;
        pEncParam->stVencQpConf.u32MaxQp = 46;
        pEncParam->stVencQpConf.u32MinQp = 28;
        pEncParam->stVencQpConf.u32MaxIQp = 51;
        pEncParam->stVencQpConf.u32MinIQp = 28;
        pEncParam->StreamBufSize = MINOR_ENC_STREAM_BUF_SIZE;
		pEncParam->SmartEncEnable = 0;
    } else if (VENC_CHNTYPE_SNAP == pChnInfo->ChnType) {
        pEncParam->MjpegFollowType = VENC_MJPEG_FOLLOW_MINOR;
        pEncParam->VPayloadType = VPAYLOAD_TYPE_MJPEG;
        pEncParam->width = pChnInfo->Width;
        pEncParam->height = pChnInfo->Height;
        pEncParam->BitRate = 2048;
        pEncParam->FrameRate = 5;
        pEncParam->Quality = 25;  // 仅MJPEG流生效，控制jpeg大小
        pEncParam->KeyInterval = pEncParam->FrameRate * 4;
        pEncParam->BitRateType = BITRATE_TYPE_FIXQP;
        pEncParam->bOffLine = 0;
        pEncParam->stVencQpConf.bEnbaleQp = 1;
        pEncParam->stVencQpConf.u32MaxQp = 46;
        pEncParam->stVencQpConf.u32MinQp = 28;
        pEncParam->stVencQpConf.u32MaxIQp = 46;
        pEncParam->stVencQpConf.u32MinIQp = 28;
		if (VENC_MJPEG_FOLLOW_MAIN == pEncParam->MjpegFollowType) {
	        pEncParam->StreamBufSize = MAIN_ENC_STREAM_BUF_SIZE;
	        pEncParam->width = MAIN_ENC_WIDTH;
	        pEncParam->height = MAIN_ENC_HEIGHT;
		} else {
	        pEncParam->StreamBufSize = MINOR_ENC_STREAM_BUF_SIZE;
	        pEncParam->width = MINOR_ENC_WIDTH;
	        pEncParam->height = MINOR_ENC_HEIGHT;
		}
		pChnInfo->MjpegFollowType = pEncParam->MjpegFollowType;
    }else if (VENC_CHNTYPE_MAIN_SEC == pChnInfo->ChnType){
        // 一条通路上，venc与fs分辨率需一致
        pEncParam->VPayloadType = VPAYLOAD_TYPE_H265;
        pEncParam->width = pChnInfo->Width;
        pEncParam->height = pChnInfo->Height;
        pEncParam->BitRate = 2048;
        pEncParam->MaxBitRate = pEncParam->BitRate * 2;
        pEncParam->FrameRate = MAX_FRAMERATE;
        pEncParam->Quality = 0;
        pEncParam->KeyInterval = 100;  // pEncParam->FrameRate * 2;
        pEncParam->BitRateType = BITRATE_TYPE_VBR;
        pEncParam->bOffLine = 0;
        pEncParam->stVencQpConf.bEnbaleQp = 1;
        pEncParam->stVencQpConf.u8MaxPictureQuality = 41;
        pEncParam->stVencQpConf.u16MaxPictureSize = 768;
        pEncParam->stVencQpConf.u32MaxQp = 51;
        pEncParam->stVencQpConf.u32MinQp = 28;
        pEncParam->stVencQpConf.u32MaxIQp = 51;
        pEncParam->stVencQpConf.u32MinIQp = 28;
        pEncParam->StreamBufSize = MAIN_ENC_STREAM_BUF_SIZE;
    }else if (VENC_CHNTYPE_MINOR_SEC == pChnInfo->ChnType){
        pEncParam->VPayloadType = VPAYLOAD_TYPE_H265;
        pEncParam->width = pChnInfo->Width;
        pEncParam->height = pChnInfo->Height;
        pEncParam->BitRate = 1024;
        pEncParam->MaxBitRate = pEncParam->BitRate * 2;
        pEncParam->FrameRate = MAX_FRAMERATE;
        pEncParam->Quality = 0;
        pEncParam->KeyInterval = 100;
        pEncParam->BitRateType = BITRATE_TYPE_VBR;
        pEncParam->bOffLine = 0;
        pEncParam->stVencQpConf.bEnbaleQp = 1;
        pEncParam->stVencQpConf.u8MaxPictureQuality = 40;
        pEncParam->stVencQpConf.u16MaxPictureSize = 256;
        pEncParam->stVencQpConf.u32MaxQp = 46;
        pEncParam->stVencQpConf.u32MinQp = 28;
        pEncParam->stVencQpConf.u32MaxIQp = 51;
        pEncParam->stVencQpConf.u32MinIQp = 28;
        pEncParam->StreamBufSize = MINOR_ENC_STREAM_BUF_SIZE;
    }else if (VENC_CHNTYPE_SNAP_SEC == pChnInfo->ChnType){
        pEncParam->MjpegFollowType = VENC_MJPEG_FOLLOW_MINOR;
        pEncParam->VPayloadType = VPAYLOAD_TYPE_MJPEG;
        pEncParam->width = pChnInfo->Width;
        pEncParam->height = pChnInfo->Height;
        pEncParam->BitRate = 2048;
        pEncParam->FrameRate = 5;
        pEncParam->Quality = 25;  // 仅MJPEG流生效，控制jpeg大小
        pEncParam->KeyInterval = pEncParam->FrameRate * 2;
        pEncParam->BitRateType = BITRATE_TYPE_FIXQP;
        pEncParam->bOffLine = 0;
        pEncParam->stVencQpConf.bEnbaleQp = 1;
        pEncParam->stVencQpConf.u32MaxQp = 46;
        pEncParam->stVencQpConf.u32MinQp = 28;
        pEncParam->stVencQpConf.u32MaxIQp = 46;
        pEncParam->stVencQpConf.u32MinIQp = 28;
        pEncParam->StreamBufSize = MINOR_ENC_STREAM_BUF_SIZE;
    }
	*SrcFrameRate = gViInfo.Framerate;

    return 0;
}
//#define __SUPPORT_SVC__
static int _Sample_InitVencChn()
{
    int ret = 0;
    VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
    VENC_BIND_CONF_S BindConf = {0};
    VENC_ENCODEPARAM_S EncParam = {0};
	UINT32 SrcFrameRate = 0;
    int i = 0;
	VENC_SUPER_FRAME_STRATEGY_S Strategy = {0};

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        Sample_Venc_InitVencChnAttr(&gVencInfo.ChnInfo[i], &EncParam, &SrcFrameRate);
		if (gVencInfo.ChnInfo[i].ChnType == VENC_CHNTYPE_SNAP) {
			if (VENC_MJPEG_FOLLOW_MAIN == EncParam.MjpegFollowType) {
		        EncParam.StreamBufSize = MAIN_ENC_STREAM_BUF_SIZE;
		        EncParam.width = gVencInfo.ChnInfo[0].Width;
		        EncParam.height = gVencInfo.ChnInfo[0].Height;
			} else {
		        EncParam.StreamBufSize = MINOR_ENC_STREAM_BUF_SIZE;
		        EncParam.width = gVencInfo.ChnInfo[1].Width;
		        EncParam.height = gVencInfo.ChnInfo[1].Height;
			}
		}
#ifdef __SUPPORT_SVC__
		if (0 == i) {
			Venc_Chn_SetSvcVersion(0, ChnType, i, VENC_SVC_V2);
		}
#endif
        ChnType = gVencInfo.ChnInfo[i].ChnType;
#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
		if (0 == i) {
			Venc_Chn_SetDuplicateFrameStrategy(0, ChnType, i, VENC_DUPLICATE_FRAME_STRATEGE_COPY);
		}
#endif
        ret = Venc_Chn_Open(0, ChnType, i, EncParam, SrcFrameRate);
        if (ret) {
            VENC_TRACE_ERR("Venc_Chn_Open chn(%d) FAIL(%#x)\n", ChnType, ret);
            return -1;
        }
#if 0//def __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
		if (0 == i) {
			Venc_Chn_SetDuplicateFrameParam(0, ChnType, i, EncParam.KeyInterval, EncParam.KeyInterval * 3 / 2);
		}
#endif
#ifdef __SUPPORT_SVC__
		if (0 == i) {
			VENC_SVC_PARAM Param;
			Param.Enable = XRS_TRUE;
			Param.svc_version = VENC_SVC_V2;
			Param.svc_param_v2.max_ref_num = 3;
			Param.svc_param_v2.qp_delta[0] = -1;
			Param.svc_param_v2.qp_delta[15] = 0;
			Param.svc_param_v2.refresh_interval = 4;
			
			Venc_Chn_SetSvcParam(0, ChnType, i, Param);
		}
#endif

        ret = Venc_Chn_Start(0, ChnType, i, 0);
        if (ret) {
            Venc_Chn_Close(0, ChnType, i);
            VENC_TRACE_ERR("Venc_Chn_Start chn(%d) FAIL(%d)\n", ChnType, ret);
            return -1;
        }
		
		Strategy.super_frame_mode = VENC_SUPER_FRAME_DISCARD;
		Strategy.reencode_priority = VENC_REENCODE_FRAME_BITS_FIRST;
		Strategy.i_frame_bits_threshold = 8*EncParam.StreamBufSize*0.8;
		Strategy.p_frame_bits_threshold = Strategy.i_frame_bits_threshold;
		Strategy.b_frame_bits_threshold = Strategy.p_frame_bits_threshold;

		Venc_Chn_SetSuperFrameStrategy(0, ChnType, i, Strategy);

        BindConf.ChnType = ChnType;
        if( i < gVpssInfo[0].ChnNum) {
            BindConf.VencSrcGrp = gVpssInfo[0].GrpInfo.Grp;
            BindConf.VencSrcChn = i;
			if (VENC_CHNTYPE_SNAP == ChnType) {
				BindConf.VencSrcChn = (VENC_MJPEG_FOLLOW_MAIN == EncParam.MjpegFollowType) ? 0 : 1;
			}
        } else {
            BindConf.VencSrcGrp = gVpssInfo[1].GrpInfo.Grp;
            BindConf.VencSrcChn = (VENC_CHNTYPE_SNAP_SEC == ChnType) ? 1 : i - gVpssInfo[0].ChnNum;
			if (VENC_CHNTYPE_SNAP_SEC == ChnType) {
				BindConf.VencSrcChn = (VENC_MJPEG_FOLLOW_MAIN == EncParam.MjpegFollowType) ? 0 : 1;
			}	
        }

        ret = Venc_Chn_Bind(0, i, BindConf);
        if (ret) {
            Venc_Chn_Stop(0, ChnType, i);
            Venc_Chn_Close(0, ChnType, i);
            VENC_TRACE_ERR("Venc_Chn_Bind chn(%d) FAIL(%d)\n", BindConf.ChnType, ret);
            return -1;
        }

        if (EncParam.VPayloadType == VPAYLOAD_TYPE_H265) {
            rtsp_set_video(session[i], RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
            rtsp_sync_video_ts(session[i], rtsp_get_reltime(), rtsp_get_ntptime());
        } else if (EncParam.VPayloadType == VPAYLOAD_TYPE_H264) {
            rtsp_set_video(session[i], RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
            rtsp_sync_video_ts(session[i], rtsp_get_reltime(), rtsp_get_ntptime());
        }
    }

    return 0;
}

static int Sample_Venc_DeInitVencChn()
{
    int ret = 0;
    VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
    VENC_BIND_CONF_S BindConf = {0};
    int i = 0;

    BindConf.VencSrcType = VENC_SRC_TYPE_VPSS;
    for (i = 0; i < gVencInfo.ChnNum; i++) {
        ChnType = gVencInfo.ChnInfo[i].ChnType;
        BindConf.ChnType = ChnType;

        if( i < gVpssInfo[0].ChnNum){
            BindConf.VencSrcGrp = gVpssInfo[0].GrpInfo.Grp;
            BindConf.VencSrcChn = i;
			if (VENC_CHNTYPE_SNAP == ChnType) {
				BindConf.VencSrcChn = (VENC_MJPEG_FOLLOW_MAIN == gVencInfo.ChnInfo[i].MjpegFollowType) ? 0 : 1;
			}
        }else{
            BindConf.VencSrcGrp = gVpssInfo[1].GrpInfo.Grp;
			if (VENC_CHNTYPE_SNAP_SEC == ChnType) {
				BindConf.VencSrcChn = (VENC_MJPEG_FOLLOW_MAIN == gVencInfo.ChnInfo[i].MjpegFollowType) ? 0 : 1;
			}	
        }

        ret = Venc_Chn_UnBind(0, ChnType, BindConf);
        if (ret) {
            VENC_TRACE_ERR("Venc_Chn_UnBind chn(%d) FAIL(%#x)\n", BindConf.ChnType, ret);
        }

        ret = Venc_Chn_Stop(0, ChnType, i);
        if (ret) {
            VENC_TRACE_ERR("Venc_Chn_Stop chn(%d) FAIL(%#x)\n", ChnType, ret);
        }

        ret = Venc_Chn_Close(0, ChnType, i);
        if (ret) {
            VENC_TRACE_ERR("Venc_Chn_Close chn(%d) FAIL(%#x)\n", ChnType, ret);
        }
    }

    return 0;
}

static SINT32 _Sample_StartVenc()
{
    SINT32 Ret = 0, i = 0;
    VIDEO_CHIP_CONF_S ChipConf = {0};
    VENC_CONF_S VencConf = {0};

    Ret = Venc_Open(gVencInfo.ChnNum, ChipConf, VencConf, NULL, NULL);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Venc_Open failed, ret = %d\n", Ret);
        return -1;
    }

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        Venc_SetChnCallback(gVencInfo.ChnInfo[i].ChnType, (VData_Callback)_Video_Get_DataFunc, NULL);
    }

    Ret = Venc_Start(0, 0);
    if (Ret) {
        VENC_TRACE_ERR("Venc_Start FAIL(%d)\n", Ret);
        goto sample_close_venc;
    }

    Ret = _Sample_InitVencChn();
    if (Ret) {
        VENC_TRACE_ERR("Venc_Start FAIL(%d)\n", Ret);
        goto sample_stop_venc;
    }

    return 0;
sample_stop_venc:
    Venc_Stop(0);
sample_close_venc:
    Venc_Close(0);
    return -1;
}

static void _Sample_StopVenc()
{
    Sample_Venc_DeInitVencChn();
    Venc_Stop(0);
    Venc_Close(0);
}

// 出流后绘制，否则会报错
static int __DrawFontByBitmap(VRECT_S *Rect, char *path, VOSD_CHNTYPE_E type)
{
    char *data = NULL;
    int fontLen = 1344000;
    FILE *fp = NULL;
    VOSD_OVERYLAY_S VosdOverLay = {0};

    fp = fopen(path, "r");
    if (NULL == fp) {
        VOSD_ERR("fopen FAIL(%s)\n", path);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    fontLen = ftell(fp);
    rewind(fp);

    data = (char *)malloc(fontLen);
    if (NULL == data) {
        VOSD_ERR("malloc FAIL(%d)\n", fontLen);
        return -1;
    }

    fread(data, 1, fontLen, fp);

    VosdOverLay.InvEnable = 0;
    VosdOverLay.pData = data;
    VosdOverLay.Rect.x = Rect->x;
    VosdOverLay.Rect.y = Rect->y;
    VosdOverLay.Rect.w = Rect->w;  // 20 * 16 ;
    VosdOverLay.Rect.h = Rect->h;  // 34;

    Vosd_Chn_SetBitMap(0, type, 0, VosdOverLay);

    fclose(fp);
    fp = NULL;
    free(data);
    data = NULL;

    return 0;
}

static int __DrawFontByPlatAddr(VRECT_S *Rect, char *path, VOSD_CHNTYPE_E type)
{
	int ret = 0;
    int fontLen = 0;
    FILE *fp = NULL;
    VOSD_OVERYLAY_S VosdOverLay = {0};

    fp = fopen(path, "r");
    if (NULL == fp) {
        VOSD_ERR("fopen FAIL(%s)\n", path);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    fontLen = ftell(fp);
    rewind(fp);

    VosdOverLay.InvEnable = 0;
    VosdOverLay.Rect.x = Rect->x;
    VosdOverLay.Rect.y = Rect->y;
    VosdOverLay.Rect.w = Rect->w;  // 20 * 16 ;
    VosdOverLay.Rect.h = Rect->h;  // 34;
	ret = Vosd_Chn_SetBitMapAttrToGetAddrFromPlat(0, 0, type, &VosdOverLay);
    if (ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_SetBitMapByText failed, ret = %#x\n", ret);
        return -1;
    }

    fread(VosdOverLay.pData, 1, fontLen, fp);
    fclose(fp);
    fp = NULL;

	ret = Vosd_Chn_UpdateBitMapDataByPlatAddr(0, 0, type, 1);
    if (ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_SetBitMapByText failed, ret = %#x\n", ret);
        return -1;
    }

    return 0;
}


static SINT32 _Sample_StartOsd()
{
    SINT32 Ret = 0;
    VIDEO_CHIP_CONF_S ChipConf = {0};
    VOSD_CONF_S VosdConf = {0};

    Ret = Vosd_Open(1, ChipConf, VosdConf);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOVOSD, "Vosd_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Vosd_Start(0, 0);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOVOSD, "Vosd_Start failed, ret = %d\n", Ret);
        return -1;
    }

    return 0;
}

static void _Sample_StopOsd()
{
    Vosd_Stop(0);
    Vosd_Close(0);
}

static SINT32 _Sample_StartVDA()
{
    SINT32 Ret = 0;
    VIDEO_CHIP_CONF_S ChipConf;
    VDA_CONF_S VdaConf;
    VDA_CHN_ATTR_S ChnAttr = {0};
    VDA_MD_PARAM_S param = {0};

    Ret = Vda_Open(1, ChipConf, VdaConf);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOVOSD, "Vda_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Vda_Start(0, 0, 0);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOVOSD, "Vda_Start failed, ret = %d\n", Ret);
        goto close_vda;
    }
#if !defined(BOARD_H82W) && !defined(BOARD_G82W)
        // 从子码流取帧
        ChnAttr.Width = gVpssInfo[0].ChnInfo[1].Width;
        ChnAttr.Height = gVpssInfo[0].ChnInfo[1].Height;
        Ret = Vda_Chn_Open(0, ChnAttr);
        if (Ret != 0) {
            TRACE_ERR(COMP_VIDEOVOSD, "Vda_Chn_Open failed, ret = %d\n", Ret);
            goto stop_vda;
        }

        param.sen = 5;
        param.enable = 1;
        Ret = Vda_Chn_Set_MdParam(0, 0, param);
        if (Ret != 0) {
            TRACE_ERR(COMP_VIDEOVOSD, "Vda_Chn_Set_MdParam failed, ret = %d\n", Ret);
            goto close_chn;
        }
#endif

    return 0;
close_chn:
    Vda_Chn_Close(0, 0);
stop_vda:
    Vda_Stop(0);
close_vda:
    Vda_Close(0);
    return -1;
}

static void _Sample_StopVDA()
{
#if !defined(BOARD_H82W) && !defined(BOARD_G82W)
    Vda_Chn_Close(0, 0);
#endif
    Vda_Stop(0);
    Vda_Close(0);
}


#if defined(__SUPPORT_FFMPEG__) || defined(__SUPPORT_OPENH264__)
static SINT32 _Sample_StartLcd()
{
    SINT32 Ret = 0;
    LCD_CONF_S LcdConf = {
        .Width=240,
        .Height=320
    };

    Ret = Lcd_Open(0, LcdConf);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOVOSD, "Lcd_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Lcd_Start(0, 60);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOVOSD, "Lcd_Start failed, ret = %d\n", Ret);
        Lcd_Close(0);
        return -1;
    }

    return 0;
}

static void _Sample_StopLcd()
{
    Lcd_Stop(0);
    Lcd_Close(0);
}

static SINT32 _Sample_StartVdec()
{
    SINT32 Ret = 0;
    VDEC_CONF_S VdecConf = {
        .vsize = VIDEO_PIC_QVGA,
    };
    Ret = Vdec_Open(VdecConf, Lcd_SendFrame);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEODEC, "Vdec_Open failed, ret = %d\n", Ret);
        return -1; 
    }

    Ret = Vdec_Start(0, 60);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEODEC, "Vdec_Start failed, ret = %d\n", Ret);
        Vdec_Close(0);
        return -1; 
    }

    return 0;
}

static void _Sample_StopVdec()
{
    Vdec_Stop(0);
    Vdec_Close(0);
}

static SINT32 _Sample_CreateVdecChn(VPAYLOAD_TYPE_E Type)
{
    SINT32 Ret = 0;
    VDEC_CHN_ATTR_S Attr = {
        .Width = 240,
        .Height = 320,
    };

    Ret = Vdec_Chn_Open(0, Type, 0, &Attr);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEODEC, "Vdec_Chn_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Vdec_Chn_Start(0, Type, 0);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEODEC, "Vdec_Chn_Start failed, ret = %d\n", Ret);
        Vdec_Chn_Close(0, Type, 0);
        return -1;
    }

    return 0;
}

static void _Sample_DestoryVdecChn(VPAYLOAD_TYPE_E Type)
{
    Vdec_Chn_Stop(0, Type, 0);
    Vdec_Chn_Close(0, Type, 0);
}

#endif

#define AENC_OUT_FILE "./aenc_record"
static int s_AencFileSave = 0;

static SINT32 _Audio_Get_DataFunc(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, char *data, SINT32 size)
{
	static int fd[2] = {-1, -1};
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
				VENC_TRACE_ERR("open %s failed:%s\n", filePath, strerror(errno));
				return -1;
			}
		}

		writeRet = write(fd[idx], (void *)data, size);
		if (writeRet != size) 
		{
			VENC_TRACE_ERR("write frame failed(%d,%d)\n", writeRet, size);
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

static int audio_ai_init()
{
	int s32Ret = 0;
	AI_CONF_S stAiCfg = {1, 1, {0}, {0}};
	AUDIO_CONF_S stAiChnConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 320, 10};
	
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
#endif	
    s32Ret = Ai_Chn_Open(0, AI_CHTYPE_TALK, 0, stAiChnConf);
	if (s32Ret)
	{
		AI_TRACE_ERR("[AUDIO] Ai_Chn_Open Failed!\n");
		return -1;
	}

	s32Ret = Ai_Set_Volume(0, AUDIO_DEV_AI_TALK, 0, 80);
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
	
	s32Ret = Aenc_Open(2, (VData_Callback)_Audio_Get_DataFunc, NULL);
	if (s32Ret)
	{
		VENC_TRACE_ERR("Aenc_Open failed with %#x\n", s32Ret);
		return -1;
	}

	s32Ret = Aenc_Start(0, 0);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Start Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Open(0, 0, audioType, 320);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Open(0, 1, AUDIO_G711U, 320);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_Open Failed!\n");
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
	AUDIO_CONF_S stAoConf = {XRS_AUDIO_SAMPLE_RATE_8000, XRS_AUDIO_BIT_WIDTH_16, AUDIO_NR_LEVEL_1, 320, 10};
	AI_TRACE_ERR("[AUDIO] %s!\n", __FUNCTION__);
#ifdef __SUPPORT_AI_16K_SAMPLE_RATE_INPUT__
	//ai chn0:AAC编码通道绑定源,采用16K采样率
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

	s32Ret = Ao_Set_Volume(0, AO_DEV_CVBS, 80);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Set_Volume Failed!\n");
		return -1;
	}

	//开启喇叭输出
	HalMod_Set_SpeakerStat(0, 0);
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
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Close(0, 1);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_Close Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Stop(0);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Stop Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Close(0);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Close Failed!\n");
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
	AENC_BIND_CONF_S stAeBindConf1 = {0, AUDIO_DEV_AI_TALK, 1, 0};
	
	s32Ret = Ao_Chn_Bind(0, stAoBindConf);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_Bind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Bind(0, stAeBindConf0);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_Bind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_Bind(0, stAeBindConf1);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_Bind Failed!\n");
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

	s32Ret = Ao_Chn_UnBind(0, stAoBindConf);
	if (s32Ret)
	{
		AO_TRACE_ERR("[AUDIO] Ao_Chn_UnBind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_UnBind(0, stAeBindConf0);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_UnBind Failed!\n");
		return -1;
	}

	s32Ret = Aenc_Chn_UnBind(0, stAeBindConf1);
	if (s32Ret)
	{
		VENC_TRACE_ERR("[AUDIO] Aenc_Chn_UnBind Failed!\n");
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

typedef struct
{
	pthread_t pthreadId;
	int exit;
}AI_CHECK_MANAGER_T;

static AI_CHECK_MANAGER_T s_AiChkMan = {0};

static void *AiCheckThread(void *arg)
{
	int ret = 0;
	int VpssChn = 2;
	AI_CHECK_MANAGER_T *AiChkMan = (AI_CHECK_MANAGER_T *)arg;
		
	while(1)
	{
		if (AiChkMan->exit)
		{
			break;
		}
		
		//get frame
		VIDEO_FRM_ALG_S VFrmAlg = {0};
		ret = Vpss_Chn_Get_Frm(0, 0, VpssChn, &VFrmAlg, 1000);
		if (ret)
		{
			VENC_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
			continue;
		}
#ifdef __SUPPORT_SVC__

		VENC_SVC_RECT_INFO RectInfo = {0};

		RectInfo.rect_num = 1;
		RectInfo.base_resolution.w = MAIN_ENC_WIDTH;
		RectInfo.base_resolution.h = MAIN_ENC_HEIGHT;
		RectInfo.detect_type[0] = VENC_SVC_RECT_TYPE0;
		RectInfo.pts = VFrmAlg.u64PTS;
		RectInfo.rect_attr[0].x = 0;
		RectInfo.rect_attr[0].y = 0;
		RectInfo.rect_attr[0].w = 200;
		RectInfo.rect_attr[0].h = 200;
		Venc_Chn_SetSvcRegion(0, VENC_CHNTYPE_MAIN, 0, RectInfo);

		VENC_SVC_PARAM Param;
		Param.Enable = XRS_TRUE;
		Param.svc_version = VENC_SVC_V2;
		Param.svc_param_v2.max_ref_num = 2;
		Param.svc_param_v2.qp_delta[0] = -1;
		Param.svc_param_v2.qp_delta[15] = 0;
		Param.svc_param_v2.refresh_interval = 2;

		Venc_Chn_SetSvcParam(0, VENC_CHNTYPE_MAIN, 0, Param);
		
#endif
RELEASE_FRAME:
		Vpss_Chn_Rel_Frm(0, 0, VpssChn, &VFrmAlg);
		usleep(10 * 1000);
	}

	return NULL;
}

static int _CreateAiCheckPthread()
{
	int ret = 0;
	
	ret = pthread_create(&s_AiChkMan.pthreadId, NULL, AiCheckThread, (void *)&s_AiChkMan);
	if (ret) 
	{
		AI_TRACE_ERR("pthread_create AiCheckThread failed\n");
		return ERRNO(COMMON_ERROR_THREADSTART, COMP_AUDIOIN);;
	}
		
	s_AiChkMan.exit = 0;
		
	return 0;
}

static int _DestoryAiCheckPthread()
{
	s_AiChkMan.exit = 1;
	pthread_join(s_AiChkMan.pthreadId, NULL);
		
	return 0;
}


static void _Venc_Get_Cmd(char *pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar())) {
        i++;
    }
}

static void _Test_SwitchVencH264CBR_20fps()
{
    VENC_ENCODEPARAM_S param = {0};
    SINT32 SrcFrmRate = 0;
    SINT32 Ret = 0;

    Ret = Venc_Chn_Get_EncodeParam(0, VENC_CHNTYPE_MAIN, 0, &param, &SrcFrmRate);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
        return;
    }

    param.FrameRate = 20;
    param.BitRateType = BITRATE_TYPE_CBR;
    param.KeyInterval = param.FrameRate * 4;
    param.BitRate = param.MaxBitRate = 2048;
    Ret = Venc_Chn_Set_EncodeParam(0, VENC_CHNTYPE_MAIN, 0, param, SrcFrmRate);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Set_EncodeParam failed!\n");
        return;
    }

    memset(&param, 0, sizeof(VENC_ENCODEPARAM_S));
    Ret = Venc_Chn_Get_EncodeParam(0, VENC_CHNTYPE_MINOR, 1, &param, &SrcFrmRate);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
        return;
    }

    param.FrameRate = 20;
    param.BitRateType = BITRATE_TYPE_CBR;
    param.KeyInterval = param.FrameRate * 4;
    param.BitRate = param.MaxBitRate = 1024;
    Ret = Venc_Chn_Set_EncodeParam(0, VENC_CHNTYPE_MINOR, 0, param, SrcFrmRate);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Set_EncodeParam failed!\n");
        return;
    }

	
	int i = 0;

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        if (gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP && gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP_SEC) {
			rtsp_del_session (session[i]);
        }
    }

	rtsp_del_demo(demo);

    demo = rtsp_new_demo(8554);  // rtsp sever socket
    if (NULL == demo) {
        printf("rtsp_new_demo failed\n");
        return ;
    }

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        char path[32] = {0};
        if (gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP && gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP_SEC) {
            snprintf(path, sizeof(path), "/live/%d", i);
            session[i] = rtsp_new_session(demo, path);
            printf("==========> rtsp://127.0.0.1:8554%s <===========\n", path);
        }
    }


    rtsp_set_video(session[0], RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    rtsp_sync_video_ts(session[0], rtsp_get_reltime(), rtsp_get_ntptime());
	
    rtsp_set_video(session[1], RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
    rtsp_sync_video_ts(session[1], rtsp_get_reltime(), rtsp_get_ntptime());
}

static void _Test_SwitchMain_H264_1080P(UINT32 width, UINT32 height)
{
    VENC_ENCODEPARAM_S param = {0};
    SINT32 SrcFrmRate = 0;
    SINT32 Ret = 0;
    VPSS_CHN_ATTR_S ChnAttr;
	SINT32 ChnType;
	VENC_BIND_CONF_S BindConf;

    Ret = Venc_Chn_Get_EncodeParam(0, VENC_CHNTYPE_MAIN, 0, &param, &SrcFrmRate);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
        return;
    }

	Sample_Venc_InitVencChnAttr(&gVencInfo.ChnInfo[0], &param, (UINT32*)&SrcFrmRate);

	ChnType = gVencInfo.ChnInfo[0].ChnType;
	BindConf.ChnType = ChnType;
	BindConf.VencSrcGrp = gVpssInfo[0].GrpInfo.Grp;
	BindConf.VencSrcChn = 0;
	Ret = Venc_Chn_UnBind(0, ChnType, BindConf);
	if (Ret) {
		VENC_TRACE_ERR("Venc_Chn_UnBind chn(%d) FAIL(%#x)\n", BindConf.ChnType, Ret);
	}

    Ret = Venc_Chn_Stop(0, VENC_CHNTYPE_MAIN, 0);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
        return;
    }

    Ret = Venc_Chn_Close(0, VENC_CHNTYPE_MAIN, 0);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
        return;
    }

	Ret = Vpss_Chn_Close(0, gVpssInfo[0].GrpInfo.Grp, gVpssInfo[0].ChnInfo[0].Chn);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vpss_Chn_Close failed!\n");
        return;
    }

	memset(&ChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
	ChnAttr.width = width;
	ChnAttr.height = height;
	Ret = Vpss_Chn_Open(0, gVpssInfo[0].GrpInfo.Grp, gVpssInfo[0].ChnInfo[0].Chn, ChnAttr);
	if (Ret != 0) {
        VENC_TRACE_ERR("Vpss_Chn_Close failed!\n");
        return;
	}

    // 分辨率，编码类型必须要重新创建通道
    param.width = width;
    param.height = height;
    Ret = Venc_Chn_Open(0, VENC_CHNTYPE_MAIN, 0, param, SrcFrmRate);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Open failed!\n");
        return;
    }

    Ret = Venc_Chn_Start(0, VENC_CHNTYPE_MAIN, 0, 0);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Start failed!\n");
        return;
    }

	BindConf.ChnType = ChnType;
	BindConf.VencSrcGrp = gVpssInfo[0].GrpInfo.Grp;
	BindConf.VencSrcChn = 0;
	
	Ret = Venc_Chn_Bind(0, 0, BindConf);
	if (Ret) {
		VENC_TRACE_ERR("Venc_Chn_Bind chn(%d) FAIL(%d)\n", BindConf.ChnType, Ret);
		return;
	}

}

static void _Test_SwitchMinor_H264_448P()
{
    VENC_ENCODEPARAM_S param[3] = {0};
    SINT32 SrcFrmRate[3] = {0};
    SINT32 Ret = 0;
    VPSS_CHN_ATTR_S ChnAttr;
	SINT32 ChnType;
	VENC_BIND_CONF_S BindConf;
	int i = 0;
	
	for (i = 1; i < gVencInfo.ChnNum; i++)
	{
		ChnType = gVencInfo.ChnInfo[i].ChnType;
	    Ret = Venc_Chn_Get_EncodeParam(0, ChnType, i, &param[i], &SrcFrmRate[i]);
	    if (Ret != 0) {
	        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
	        return;
	    }

		Sample_Venc_InitVencChnAttr(&gVencInfo.ChnInfo[i], &param[i], (UINT32*)&SrcFrmRate[i]);

		BindConf.ChnType = ChnType;
		BindConf.VencSrcGrp = gVpssInfo[0].GrpInfo.Grp;
		BindConf.VencSrcChn = 1;
		Ret = Venc_Chn_UnBind(0, ChnType, BindConf);
		if (Ret) {
			VENC_TRACE_ERR("Venc_Chn_UnBind chn(%d) FAIL(%#x)\n", BindConf.ChnType, Ret);
		}

	    Ret = Venc_Chn_Stop(0, ChnType, i);
	    if (Ret != 0) {
	        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
	        return;
	    }

	    Ret = Venc_Chn_Close(0, ChnType, i);
	    if (Ret != 0) {
	        VENC_TRACE_ERR("Venc_Chn_Get_EncodeParam failed!\n");
	        return;
	    }
	}

	Ret = Vpss_Chn_Close(0, gVpssInfo[0].GrpInfo.Grp, gVpssInfo[0].ChnInfo[1].Chn);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vpss_Chn_Close failed!\n");
        return;
    }

	memset(&ChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
	ChnAttr.width = 800;
	ChnAttr.height = 448;
	Ret = Vpss_Chn_Open(0, gVpssInfo[0].GrpInfo.Grp, gVpssInfo[0].ChnInfo[1].Chn, ChnAttr);
	if (Ret != 0) {
        VENC_TRACE_ERR("Vpss_Chn_Close failed!\n");
        return;
	}

	for (i = 1; i < gVencInfo.ChnNum; i++)
	{
		ChnType = gVencInfo.ChnInfo[i].ChnType;
	    // 分辨率，编码类型必须要重新创建通道
	    param[i].width = 800;
	    param[i].height = 448;
	    Ret = Venc_Chn_Open(0, ChnType, i, param[i], SrcFrmRate[i]);
	    if (Ret != 0) {
	        VENC_TRACE_ERR("Venc_Chn_Open failed!\n");
	        return;
	    }

	    Ret = Venc_Chn_Start(0, ChnType, i, 0);
	    if (Ret != 0) {
	        VENC_TRACE_ERR("Venc_Chn_Start failed!\n");
	        return;
	    }

		ChnType = gVencInfo.ChnInfo[i].ChnType;
		BindConf.ChnType = ChnType;
		BindConf.VencSrcGrp = gVpssInfo[0].GrpInfo.Grp;
		BindConf.VencSrcChn = 1;
		Ret = Venc_Chn_Bind(0, i, BindConf);
		if (Ret) {
			VENC_TRACE_ERR("Venc_Chn_Bind chn(%d) FAIL(%d)\n", BindConf.ChnType, Ret);
			return;
		}

	}
}

static void _Test_SwitchFramerate_15fps()
{
    SINT32 Ret = 0;

    Ret = Venc_Chn_Set_FrmRate(0, VENC_CHNTYPE_MAIN, 0, 15);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Set_FrmRate failed!\n");
        return;
    }

    Ret = Venc_Chn_Set_FrmRate(0, VENC_CHNTYPE_MINOR, 1, 15);
    if (Ret != 0) {
        VENC_TRACE_ERR("Venc_Chn_Set_FrmRate failed!\n");
        return;
    }
}

static void _Test_SetMainOSD_x128y128()
{
    VRECT_S Rect = {0};

    Rect.x = 128;
    Rect.y = 128;
    Rect.w = 480;
    Rect.h = 32;
    __DrawFontByBitmap(&Rect, "./151_stream_1_textOSD_xywh_0_0_480_32", VOSD_CHNTYPE_VENC_MAIN);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    __DrawFontByBitmap(&Rect, "./151_stream_1_textOSD_xywh_0_0_480_32", VOSD_CHNTYPE_VENC_MAIN_SEC);
    __DrawFontByBitmap(&Rect, "./151_stream_1_textOSD_xywh_0_0_480_32", VOSD_CHNTYPE_VENC_MINOR_SEC);
#endif
}

static void _Test_SetMainOSD_x128y128_ByPlatAddr()
{
    VRECT_S Rect = {0};

    Rect.x = 128;
    Rect.y = 128;
    Rect.w = 608;
    Rect.h = 64;
	__DrawFontByPlatAddr(&Rect, "./timeosd-608-64", VOSD_CHNTYPE_VENC_MAIN);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    __DrawFontByPlatAddr(&Rect, "./timeosd-608-64", VOSD_CHNTYPE_VENC_MAIN_SEC);
    __DrawFontByPlatAddr(&Rect, "./timeosd-608-64", VOSD_CHNTYPE_VENC_MINOR_SEC);
#endif
}

static void _Test_SetMainOSD_x0y0()
{
    VRECT_S Rect = {0};

    Rect.x = 0;
    Rect.y = 0;
    Rect.w = 608;
    Rect.h = 64;
    __DrawFontByBitmap(&Rect, "./timeosd-608-64", VOSD_CHNTYPE_VENC_MAIN);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    __DrawFontByBitmap(&Rect, "./timeosd-608-64", VOSD_CHNTYPE_VENC_MAIN_SEC);
    __DrawFontByBitmap(&Rect, "./timeosd-608-64", VOSD_CHNTYPE_VENC_MINOR_SEC);
#endif
}

static void _Test_SetMainOSD_x0y0_byText()
{
	VOSD_TEXT_OVERYLAY_S TextOverLay = {0};
	UINT16 text3[512] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x5e,\
						0x41, 0x49, 0x20, 0x49, 0x50, 0x43, 0x20, 0x4b, 0x5e,\
						0x82af, 0x777f, 0x89c6, 0x6d4b, 0x8bd5,0x0,};

	TextOverLay.InvEnable = 0;
	memcpy(TextOverLay.strText, text3, sizeof(UINT16)*512);
	TextOverLay.nX = 0;
	TextOverLay.nY = 0;
	TextOverLay.nPicWidth = gVencInfo.ChnInfo[0].Width;
	TextOverLay.nPicHeight = gVencInfo.ChnInfo[0].Height;
	Vosd_Chn_SetBitMapByText(0, 0, VOSD_CHNTYPE_VENC_MAIN, TextOverLay);
}


static void _Test_DelMainOSD() { 
    Vosd_Chn_DelBitMap(0, VOSD_CHNTYPE_VENC_MAIN, 0); 
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Vosd_Chn_DelBitMap(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0); 
    Vosd_Chn_DelBitMap(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0); 
#endif
}

/**************************************************************************************************************
 * MOSIC
 **************************************************************************************************************/
static void _Test_SetMosaic()
{
    VMOSAIC_REGION_S VMosaic = {
        .BlkSize = 32,
        .Rect =
            {
                .x = 256,
                .y = 256,
                .w = 128,
                .h = 128,
            },
    };

    VMOSAIC_REGION_S VMosaic1 = {
        .BlkSize = 32,
        .Rect =
            {
                .x = 128,
                .y = 128,
                .w = 128,
                .h = 128,
            },
    };


    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN, 0, VMosaic);
    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN, 1, VMosaic1);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0, VMosaic);
    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 1, VMosaic1);
    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0, VMosaic);
    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 1, VMosaic1);
#endif
}

static void _Test_DelMosaic() 
{ 
	Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN, 0); 
    Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN, 1);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0);
    Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 1);
    Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0);
    Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 1);
#endif
}

static void _Test_GetMdResult()
{
    SINT32 Ret = 0, i = 0, j = 0;
    VDA_MD_AREADATA_S MdData;

    Ret = Vda_Chn_Get_MdData(0, 0, &MdData);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vda_Chn_Get_MdData failed!\n");
        return;
    }

    printf("-----------------------------------------------------------------\n");
    for (i = 0; i < MAX_MAREA_ROW_NUM; i++) {
        for (j = 0; j < MAX_MAREA_CLO_NUM; j++) {
            if (MdData.MArea[i * MAX_MAREA_CLO_NUM + j]) {
                printf("1 ");
            } else {
                printf("0 ");
            }
        }
        printf("\n");
    }
    printf("-----------------------------------------------------------------\n");
}

static void _Test_Reset()
{
    SINT32 Ret = 0;
	VDA_MD_PARAM_S param = {0};
	
	Ret = Vda_Chn_Get_MdParam(0, 0, &param);
	if (Ret != 0) {
		TRACE_ERR(COMP_VIDEOVOSD, "Vda_Chn_Set_MdParam failed, ret = %d\n", Ret);
	} else {
		if (param.enable) {
			param.enable = 0;
		} else {
			param.enable = 1;
		}
        VENC_TRACE_ERR("md enable=%d!\n",param.enable);
		Ret = Vda_Chn_Set_MdParam(0, 0, param);
		if (Ret != 0) {
			TRACE_ERR(COMP_VIDEOVOSD, "Vda_Chn_Set_MdParam failed, ret = %d\n", Ret);
		}
	}
}
static void _Test_SetOsdLineAndRect()
{
    SINT32 Ret = 0;
//    VLINE_FRAME_S VLineFrm = {
//        .LineW = 4,
//        .LineCnt = 8,
//        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE},
//        .Line =
//            {
//                {376, 482, 1292, 482},
//                {1292, 482, 1292, 826},
//                {1296, 826, 1064, 826},
//                {1064, 826, 1064, 1170},
//				{1064, 1170, 604, 1170},
//				{604, 1170, 604, 826},
//				{604, 826, 376, 826},
//				{376, 826, 376, 482},
//            },
//    };
//    VLINE_FRAME_S VLineFrm1 = {
//        .LineW = 2,
//        .LineCnt = 8,
//        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE},
//        .Line =
//            {
//                {116, 148, 404, 148},
//                {404, 148, 404, 256},
//                {404, 256, 332, 256},
//                {332, 256, 332, 364},
//				{332, 364, 188, 364},
//				{188, 364, 188, 256},
//				{188, 256, 116, 256},
//				{116, 256, 116, 148},
//            },
//    };
    VLINE_FRAME_S VLineFrm = {
        .LineW = 4,
        .LineCnt = 8,
        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE},
        .Line =
            {
                {820, 374, 1738, 374},
                {1738, 374, 1738, 718},
                {1738, 718, 1508, 718},
                {1508, 718, 1508, 1064},
				{1508, 1064, 1050, 1064},
				{1050, 1064, 1050, 718},
				{1050, 718, 820, 718},
				{820, 718, 820, 374},
            },
    };
    VLINE_FRAME_S VLineFrm1 = {
        .LineW = 2,
        .LineCnt = 8,
        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_BLUE_PURE},
        .Line =
            {
                {256, 116, 542, 116},
                {542, 116, 542, 222},
                {542, 222, 470, 222},
                {470, 222, 470, 330},
				{470, 330, 328, 330},
				{328, 330, 328, 222},
				{328, 222, 256, 222},
				{256, 222, 256, 116},
            },
    };

    VFACE_FRAME_S VRectFrm = {
        .LineW = 4,
        .RectCnt = 4,
        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_RED, VCOLOR_RGBA_BLACK, VCOLOR_RGBA_WHITE, VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_RED, VCOLOR_RGBA_BLACK, VCOLOR_RGBA_WHITE},
        .Rect =
            {
                {0, 0, 600, 400},
                {128, 128, 700, 256},
                {256, 256, 87, 189},
                {192, 192, 123, 357},
                {0, 256, 64, 64},
                {512, 512, 512, 512},
				{256, 256, 64, 64},
				{384, 256, 64, 64},
            },
    };

    Ret = Vosd_Chn_Draw_Line(0, VOSD_CHNTYPE_VENC_MAIN, 0, VLineFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Line(0, VOSD_CHNTYPE_VENC_MINOR, 0, VLineFrm1);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Line(0, VOSD_CHNTYPE_VENC_SNAP, 0, VLineFrm1);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Rect(0, VOSD_CHNTYPE_VENC_MAIN, 0, VRectFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Rect(0, VOSD_CHNTYPE_VENC_MINOR, 0, VRectFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Vosd_Chn_Draw_Line(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0, VLineFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Line(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0, VLineFrm1);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Rect(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0, VRectFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Rect(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0, VRectFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }
#endif
}

static void _Test_DelOsdLineAndRect()
{
    Vosd_Chn_Del_Rect(0, VOSD_CHNTYPE_VENC_MAIN, 0);
    Vosd_Chn_Del_Line(0, VOSD_CHNTYPE_VENC_MAIN, 0);

    Vosd_Chn_Del_Rect(0, VOSD_CHNTYPE_VENC_MINOR, 0);
    Vosd_Chn_Del_Line(0, VOSD_CHNTYPE_VENC_MINOR, 0);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Vosd_Chn_Del_Rect(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0);
    Vosd_Chn_Del_Line(0, VOSD_CHNTYPE_VENC_MAIN_SEC, 0);

    Vosd_Chn_Del_Rect(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0);
    Vosd_Chn_Del_Line(0, VOSD_CHNTYPE_VENC_MINOR_SEC, 0);
#endif
}

static void _DumpVpssFrame(VIDEO_FRM_ALG_S* pVBuf, FILE* pfd)
{
    UINT32 w = 0;
	UINT32 h = 0;
    char* pVBufVirt_Y = NULL;
    char* pVBufVirt_C = NULL;
    char* pMemContent = NULL;
    UINT8 tmpBuff[8192] = {0};
    char* pUserPageAddr[2] = {NULL, NULL};
    UINT64 phyAddr = 0;
    UINT32 uvHeight = pVBuf->u32Height / 2;

    phyAddr = pVBuf->u64PhyAddr[0];

    printf("phy_addr:%llx, size:%d\n", phyAddr, pVBuf->u32FrameSize);
    pUserPageAddr[0] = (char*)(uintptr_t)pVBuf->u64VirAddr[0];//(hi_char*) hi_mpi_sys_mmap(phy_addr, u32Size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }
    printf("stride: %d,%d\n",pVBuf->u32PicStride[0],pVBuf->u32PicStride[1] );

    pVBufVirt_Y = pUserPageAddr[0];
    pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32PicStride[0]) * (pVBuf->u32Height);

    /* save Y ----------------------------------------------------------------*/
    printf("saving......Y......\n");
    for (h = 0; h < pVBuf->u32Height; h++)
    {
        pMemContent = pVBufVirt_Y + h * pVBuf->u32PicStride[0];
        fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
    }

    {
        fflush(pfd);
        /* save U ----------------------------------------------------------------*/
        printf("U......\n");

        for (h = 0; h < uvHeight; h++)
        {
            pMemContent = pVBufVirt_C + h * pVBuf->u32PicStride[1];

            pMemContent += 1;

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                tmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(tmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
        fflush(pfd);

        /* save V ----------------------------------------------------------------*/
        printf("V......\n");
        for (h = 0; h < uvHeight; h++)
        {
            pMemContent = pVBufVirt_C + h * pVBuf->u32PicStride[1];

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                tmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(tmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
    }
    fflush(pfd);

    printf("done %d!\n", pVBuf->u32TimeRef);

    pUserPageAddr[0] = NULL;
}

static int _SaveFrmFile(char *filename, VIDEO_FRM_ALG_S *pFrame)
{
    int ret = -1;
    FILE *p_file = NULL;

    p_file = fopen(filename, "wb");
    if (p_file == NULL)
    {
        printf("open file %s error !!!\n", filename);
        return -1;
    }

	_DumpVpssFrame(pFrame, p_file);

    fflush(p_file);
    fclose(p_file);
    p_file = NULL;

    return ret;
}

#define __Frame_SAVE_FILE__
#define __Frame_CROP_RESIZE__
#define __Frame_ENC_JPEG__
static SINT32 _Test_GetAiFrame( )
{
	int ret = 0;
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	char frameFilename[64] = {0};
	int vpssGrp = 0;
	int vpssChn = 2;
	UINT64 startTime = 0;
	UINT64 endTime  = 0;
	
	ret = Vpss_Chn_Get_Frm(0,vpssGrp, vpssChn, &VFrmAlg, 1000);
	if (ret)
	{
		VENC_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
		return -1;
	}

	VENC_TRACE_ERR("get frame(chn%d %d*%d-%d-%llx)\n", vpssChn, VFrmAlg.u32Width, VFrmAlg.u32Height, VFrmAlg.u32FrameSize, VFrmAlg.u64VirAddr[0]);

#ifdef __Frame_SAVE_FILE__
	//save frame
	_SaveFrmFile("./his_test_demo.yuv", &VFrmAlg);
#endif

#ifdef __Frame_CROP_RESIZE__
	VRECT_S crop = {0, 0, VFrmAlg.u32Width / 2, VFrmAlg.u32Height / 2};
	VIDEO_FRM_ALG_S dstVFrmAlg = {0};

	Venc_MediaMemMalloc((UINT32 *)&dstVFrmAlg.u64PhyAddr[0], (void**)&dstVFrmAlg.u64VirAddr[0], VFrmAlg.u32Width * VFrmAlg.u32Height * 3 / 2);
	dstVFrmAlg.u32Width = VFrmAlg.u32Width;
	dstVFrmAlg.u32Height = VFrmAlg.u32Height;
	dstVFrmAlg.u32PicStride[0] = VFrmAlg.u32Width;
	dstVFrmAlg.u32PicStride[1] = VFrmAlg.u32Width;
	startTime = get_ms();
	Venc_YuvCropAndResize(&VFrmAlg, crop, &dstVFrmAlg);
	endTime = get_ms();
	VENC_TRACE_ERR("Venc_YuvCropAndResize cost %llu ms\n", endTime- startTime);
	Venc_MediaMemFree((UINT32)dstVFrmAlg.u64PhyAddr[0], (void*)(uintptr_t)dstVFrmAlg.u64VirAddr[0]);
#endif

#ifdef __Frame_ENC_JPEG__
	#if 0
	SINT32 bufSize = 1024*1024;
	SINT32 jpegSize = 0;
	UINT8 *pJpegBuf = malloc(bufSize);
	if (pJpegBuf) {
		memset(pJpegBuf, 0, bufSize);
		startTime = get_ms();
		Venc_FrameEncToJpeg(0, &VFrmAlg, pJpegBuf, bufSize, &jpegSize, 40);
		endTime = get_ms();
		VENC_TRACE_ERR("Venc_FrameEncToJpeg cost %llu ms\n", endTime- startTime);
		free(pJpegBuf);
		pJpegBuf = NULL;
	}
	#else 
	SINT32 bufSize = 1024*1024;
	SINT32 jpegSize = 0;
	UINT8 *pJpegBuf = malloc(bufSize);
	if (pJpegBuf) {
		memset(pJpegBuf, 0, bufSize);
		int fontLen = 0;
	    FILE *fp = NULL;
	    VOSD_OVERYLAY_S VosdOverLay = {0};

	    fp = fopen("./timeosd-608-64", "r");
	    if (NULL == fp) {
	        VOSD_ERR("fopen FAIL(%s)\n", "./timeosd-608-64");
	    }

	    fseek(fp, 0, SEEK_END);
	    fontLen = ftell(fp);
	    rewind(fp);

		VosdOverLay.pData = malloc(fontLen);
		if (VosdOverLay.pData) {
		    VosdOverLay.InvEnable = 0;
		    VosdOverLay.Rect.x = 128;
		    VosdOverLay.Rect.y = 128;
		    VosdOverLay.Rect.w = 608;  // 20 * 16 ;
		    VosdOverLay.Rect.h = 64;  // 34;

		    fread(VosdOverLay.pData, 1, fontLen, fp);
			
			startTime = get_ms();
			Venc_FrameEncToJpegWithOsd(0, &VFrmAlg, pJpegBuf, bufSize, &jpegSize, 40, &VosdOverLay);
			endTime = get_ms();
			VENC_TRACE_ERR("Venc_FrameEncToJpegWithOsd cost %llu ms\n", endTime- startTime);
			free(VosdOverLay.pData);
			VosdOverLay.pData = NULL;
		}
		
		fclose(fp);
		fp = NULL;
		free(pJpegBuf);
		pJpegBuf = NULL;
	}
	#endif
#endif
	Vpss_Chn_Rel_Frm(0, vpssGrp, vpssChn, &VFrmAlg);
	VENC_TRACE_ERR("Sample_Vpss_GetVpssFrame end\n");
	return 0;
}

static SINT32 _Test_DECODE_QR()
{
	int ret = 0;
	VIDEO_FRM_ALG_S VFrmAlg = {0};
	VDA_QR_FRAME_INFO_S qrFrm = {0};
	VDA_QR_RESULT_INFO_S qrResult = {0};

	//get frame
	ret = Vpss_Chn_Get_Frm(0, 0, 2, &VFrmAlg, 1000);
	if (ret)
	{
		VENC_TRACE_ERR("Vpss_Chn_Get_Frm FAIL(%d)\n",ret);
		return -1;
	}
	
	//detect
	qrFrm.Width = VFrmAlg.u32Width;
	qrFrm.Height = VFrmAlg.u32Height;
	qrFrm.Size = VFrmAlg.u32Width * VFrmAlg.u32Height;
	qrFrm.pFrmBuf = (UINT8*)(uintptr_t)VFrmAlg.u64VirAddr[0];

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
		VENC_TRACE_ERR("decode fail:state(%d)\n", qrResult.state);
	}

	Vpss_Chn_Rel_Frm(0, 0, 2, &VFrmAlg);

	return ret;
}

int _Test_Set_FrmRate()
{
	int ret = 0;
	char sCmd[8] = {0};
	VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
	SINT32 dstFrmRate = 0;
	
    printf("input chnType:0-main;1-minor\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	ChnType = atoi(sCmd);//(atoi(sCmd) > 1) ? 0 : atoi(sCmd);

	printf("input dstFrmRate:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	dstFrmRate = atoi(sCmd);
	
	ret = Venc_Chn_Set_FrmRate(0, ChnType, 0, dstFrmRate);
	if (ret)
	{
		VENC_TRACE_ERR("Venc_Chn_Set_FrmRate FAIL(%d)\n",ret);
		return -1;
	}
	
	int i = 0;

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        if (gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP && gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP_SEC) {
			rtsp_del_session (session[i]);
        }
    }

	rtsp_del_demo(demo);

    demo = rtsp_new_demo(8554);  // rtsp sever socket
    if (NULL == demo) {
        printf("rtsp_new_demo failed\n");
        return -1;
    }

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        char path[32] = {0};
        if (gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP && gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP_SEC) {
            snprintf(path, sizeof(path), "/live/%d", i);
            session[i] = rtsp_new_session(demo, path);
            printf("==========> rtsp://127.0.0.1:8554%s <===========\n", path);
        }
    }


    rtsp_set_video(session[0], RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    rtsp_sync_video_ts(session[0], rtsp_get_reltime(), rtsp_get_ntptime());

	return ret;
}

static SINT32 _Test_Set_CenterZoomTimes()
{
	int ret = 0;
	char sCmd[32] = {0};
	VPSS_CENTER_ZOOM_ATTR_S attr = {0};

	attr.Enable = 1;
    VENC_TRACE_INFO("input widthT:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	attr.WidthRatio = atof(sCmd);

    VENC_TRACE_INFO("input heightT:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	attr.HeightRatio = atof(sCmd);
	
	VENC_TRACE_INFO("set:usr set:widthT=%f,heightT=%f\n", attr.WidthRatio, attr.HeightRatio);

	ret = Vpss_Grp_Set_CenterZoom(0, 0, attr);;
	if (ret)
	{
		VENC_TRACE_ERR("Visp_Get_CenterZoomTimes FAIL(%d)\n",ret);
		return -1;
	}

	memset(&attr, 0, sizeof(VPSS_CENTER_ZOOM_ATTR_S));
	Vpss_Grp_Get_CenterZoom(0, 0, &attr);
	VENC_TRACE_INFO("get:usr set:widthT=%f,heightT=%f\n", attr.WidthRatio, attr.HeightRatio);
	
	return ret;
}

static SINT32 _Test_Set_Zoom()
{
	int ret = 0;
	char sCmd[32] = {0};
	VPSS_ZOOM_ATTR_S attr = {0};

	attr.Enable = 1;
    VENC_TRACE_INFO("input x:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	attr.Rect.x = atoi(sCmd);

    VENC_TRACE_INFO("input y:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	attr.Rect.y = atoi(sCmd);

    VENC_TRACE_INFO("input width:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	attr.Rect.w = atoi(sCmd);

    VENC_TRACE_INFO("input height:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	attr.Rect.h = atoi(sCmd);
	
	VENC_TRACE_INFO("set:usr set:x=%d,y=%d,w=%d,h=%d\n", attr.Rect.x, attr.Rect.y, attr.Rect.w, attr.Rect.h);

	ret = Vpss_Grp_Set_Zoom(0, 0, attr);;
	if (ret)
	{
		VENC_TRACE_ERR("Visp_Get_CenterZoomTimes FAIL(%d)\n",ret);
		return -1;
	}

	memset(&attr, 0, sizeof(VPSS_ZOOM_ATTR_S));
	Vpss_Grp_Get_Zoom(0, 0, &attr);
	VENC_TRACE_INFO("get:usr set:x=%d,y=%d,w=%d,h=%d\n", attr.Rect.x, attr.Rect.y, attr.Rect.w, attr.Rect.h);
	
	return ret;
}

static SINT32 _Test_Set_Visp_HVFLIP()
{
	int ret = 0;
	char sCmd[32] = {0};
	XRS_HVFLIP_E HVFlipMode;
		
    VENC_TRACE_INFO("input HVFlip mode:0-normal;1-mirror;2-flip;3-mir&flip\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	HVFlipMode = (XRS_HVFLIP_E)atoi(sCmd);
	
	ret = Visp_Set_TuneHVFLIP(0, HVFlipMode);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    ret = Visp_Set_TuneHVFLIP(1, HVFlipMode);
#endif
	if (ret)
	{
		VENC_TRACE_ERR("Visp_Set_TuneHVFLIP FAIL(%d)\n",ret);
		return -1;
	}
	
	return ret;
}

static SINT32 _Sample_SDK_Init()
{
	if (_Sample_StartVi() != 0) {
		TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVi\n");
		return -1;
	}

	if (_Sample_StartVisp() != 0) {
		TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVi\n");
		goto sample_stop_vi;
	}

	if (_Sample_StartVpss()) {
		TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVpss\n");
		goto sample_stop_visp;
	}

	if (_Sample_StartVenc()) {
		TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVenc\n");
		goto sample_stop_vpss;
	}

	if (_Sample_StartOsd()) {
		TRACE_ERR(COMP_VIDEOIN, "_Sample_StartOsd\n");
		goto sample_stop_venc;
	}

	if (_Sample_StartVDA()) {
		TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVDA\n");
		goto sample_stop_osd;
	}

	return 0;

sample_stop_osd:
	_Sample_StopOsd();
sample_stop_venc:
	_Sample_StopVenc();
sample_stop_vpss:
	_Sample_StopVpss();
sample_stop_visp:
	_Sample_StopVisp();
sample_stop_vi:
	_Sample_StopVi();
	return -1;
}

static SINT32 _Sample_SDK_DeInit()
{
	_Sample_StopVDA();
	_Sample_StopOsd();
	_Sample_StopVenc();
	_Sample_StopVpss();
	_Sample_StopVisp();
	_Sample_StopVi();
		
	return 0;
}

static SINT32 _Test_SwitchMIPI_8MPTo6MP()
{
	char sCmd[32] = {0};
	SINT32 switchV = 0;
	
    VENC_TRACE_INFO("input switch to:0-8MP;1-6MP\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	switchV = atoi(sCmd);

	_Sample_SDK_DeInit();

	if (switchV) {
		gViInfo.ChnInfo[0].Width = 3200;
		gViInfo.ChnInfo[0].Height = 1800;
		gViInfo.ChnInfo[0].CropX = 320;
		gViInfo.ChnInfo[0].CropY = 180;
		gViInfo.Framerate = 20;
	
		gVpssInfo[0].GrpInfo.Width = 3200;
		gVpssInfo[0].GrpInfo.Height = 1800;
		gVpssInfo[0].ChnInfo[0].Width = 3200;
		gVpssInfo[0].ChnInfo[0].Height = 1800;
	
		gVencInfo.ChnInfo[0].Width = 3200;
		gVencInfo.ChnInfo[0].Height = 1800;
	
	} else {
		gViInfo.ChnInfo[0].Width = 3840;
		gViInfo.ChnInfo[0].Height = 2160;
		gViInfo.ChnInfo[0].CropX = 0;
		gViInfo.ChnInfo[0].CropY = 0;
		gViInfo.Framerate = 20;
	
		gVpssInfo[0].GrpInfo.Width = 3840;
		gVpssInfo[0].GrpInfo.Height = 2160;
		gVpssInfo[0].ChnInfo[0].Width = 3840;
		gVpssInfo[0].ChnInfo[0].Height = 2160;
		
		gVencInfo.ChnInfo[0].Width = 3840;
		gVencInfo.ChnInfo[0].Height = 2160;
	}

	_Sample_SDK_Init();
	
	return 0;
}

static SINT32 _Test_SwitchSnsMode_8MPTo2MP()
{
	char sCmd[32] = {0};
	SINT32 snsMode = 0;
	
    VENC_TRACE_INFO("input switch mode to:0-8MP Init;1-2MP Init\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	snsMode = atoi(sCmd);

	_Sample_SDK_DeInit();

	if (snsMode) {
		gViInfo.ChnInfo[0].SnsConf.Type = VISP_SENSOR_JXK308P_MIPI_2M_30FPS_10BIT;
		gViInfo.ChnInfo[0].Width = 1920;
		gViInfo.ChnInfo[0].Height = 1080;
		gViInfo.ChnInfo[0].CropX = 0;
		gViInfo.ChnInfo[0].CropY = 0;
		gViInfo.Framerate = 30;
	
		gVpssInfo[0].GrpInfo.Width = 1920;
		gVpssInfo[0].GrpInfo.Height = 1080;
		gVpssInfo[0].ChnInfo[0].Width = 1920;
		gVpssInfo[0].ChnInfo[0].Height = 1080;
	
		gVencInfo.ChnInfo[0].Width = 1920;
		gVencInfo.ChnInfo[0].Height = 1080;
	
	} else {
		gViInfo.ChnInfo[0].SnsConf.Type = VISP_SENSOR_JXK308P_MIPI_8M_30FPS_10BIT;
		gViInfo.ChnInfo[0].Width = 3840;
		gViInfo.ChnInfo[0].Height = 2160;
		gViInfo.ChnInfo[0].CropX = 0;
		gViInfo.ChnInfo[0].CropY = 0;
		gViInfo.Framerate = 20;
	
		gVpssInfo[0].GrpInfo.Width = 3840;
		gVpssInfo[0].GrpInfo.Height = 2160;
		gVpssInfo[0].ChnInfo[0].Width = 3840;
		gVpssInfo[0].ChnInfo[0].Height = 2160;
		
		gVencInfo.ChnInfo[0].Width = 3840;
		gVencInfo.ChnInfo[0].Height = 2160;
	}
	
	_Sample_SDK_Init();
	
	return 0;
}

static SINT32 _Test_SwitchSnsMode_8MPTo6MP()
{
	char sCmd[32] = {0};
	SINT32 snsMode = 0;
	
    VENC_TRACE_INFO("input switch mode to:0-8MP Init;1-6MP Init\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	snsMode = atoi(sCmd);

	_Sample_SDK_DeInit();

	if (snsMode) {
        #if defined(BOARD_H8SVX)
        gViInfo.ChnInfo[0].SnsConf.Type = VISP_SENSOR_OS08E10_MIPI_6M_30FPS_10BIT_2LANE;
        #else
		gViInfo.ChnInfo[0].SnsConf.Type = VISP_SENSOR_OS08E10_MIPI_6M_30FPS_10BIT_4LANE;
        #endif
		gViInfo.ChnInfo[0].Width = 3200;
		gViInfo.ChnInfo[0].Height = 1800;
		gViInfo.ChnInfo[0].CropX = 0;
		gViInfo.ChnInfo[0].CropY = 0;
		gViInfo.Framerate = 30;
	
		gVpssInfo[0].GrpInfo.Width = 3200;
		gVpssInfo[0].GrpInfo.Height = 1800;
		gVpssInfo[0].ChnInfo[0].Width = 3200;
		gVpssInfo[0].ChnInfo[0].Height = 1800;
	
		gVencInfo.ChnInfo[0].Width = 3200;
		gVencInfo.ChnInfo[0].Height = 1800;
	
	} else {
        #if defined(BOARD_H8SVX)
        gViInfo.ChnInfo[0].SnsConf.Type = VISP_SENSOR_OS08E10_MIPI_8M_20FPS_10BIT_2LANE;
        #else
		gViInfo.ChnInfo[0].SnsConf.Type = VISP_SENSOR_OS08E10_MIPI_8M_20FPS_10BIT_4LANE;
        #endif
		gViInfo.ChnInfo[0].Width = 3840;
		gViInfo.ChnInfo[0].Height = 2160;
		gViInfo.ChnInfo[0].CropX = 0;
		gViInfo.ChnInfo[0].CropY = 0;
		gViInfo.Framerate = 20;
	
		gVpssInfo[0].GrpInfo.Width = 3840;
		gVpssInfo[0].GrpInfo.Height = 2160;
		gVpssInfo[0].ChnInfo[0].Width = 3840;
		gVpssInfo[0].ChnInfo[0].Height = 2160;
		
		gVencInfo.ChnInfo[0].Width = 3840;
		gVencInfo.ChnInfo[0].Height = 2160;
	}
	
	_Sample_SDK_Init();
	
	return 0;
}

#ifdef __SUPPORT_BCD__
static SINT32 _bcd_event_callback(void *ctx)
{
    printf("sample_bcd_callback now %llu\n", get_ms());
    return 0;
}

static int _Test_Set_BcdParam(void)
{
	SINT32 ret = 0;
	char sCmd[8] = {0};
	VDA_BCD_PARAM_S bcdParam = {0};

	ret = Vda_Chn_Get_BcdParam(0, 0, &bcdParam);

	printf("input enable:0-dis;1-en\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	bcdParam.enable = atoi(sCmd);

	printf("input alarmSen:[0-100)\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	bcdParam.alarmSen = atoi(sCmd);

	printf("input alarmPeriod:ms\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	bcdParam.alarmPeriod = atoi(sCmd);

	printf("input alarmCountThre:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	bcdParam.alarmCountThre = atoi(sCmd);

	printf("input alarmInterval:ms\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	bcdParam.alarmInterval = atoi(sCmd);

	bcdParam.callback = (fn_bcd_event_callback)_bcd_event_callback;
	ret = Vda_Chn_Set_BcdParam(0, 0, bcdParam);
	if (ret) {
		TRACE_ERR(COMP_VIDEOVDA, "Vda_Chn_Set_BcdParam\n");
		return -1;
	}

	return 0;
}
#endif

static int _Test_Set_ScaleCoefLevel(void)
{
	SINT32 ret = 0;
//	char sCmd[8] = {0};

	VPSS_SCALE_RANGE_S ScaleRange = {VPSS_SCALE_RANGE_6};
	VPSS_SCALE_COEF_LEVEL_S ScaleCoefLevel = {0};
	//set range [0-5] by range-6 cfg
	ret = Vpss_Chn_Get_ScaleCoefInfo(0, ScaleRange, &ScaleCoefLevel);
	if (ret) {
		VPSS_ERR("Vpss_Chn_Get_ScaleCoefInfo failed with %#x\n", ret);
	} else {
		for (int i = VPSS_SCALE_RANGE_0; i < VPSS_SCALE_RANGE_6; i++) {
			ScaleRange.hor = i;
			ScaleRange.ver = i;
			ret = Vpss_Chn_Set_ScaleCoefInfo(0, ScaleRange, ScaleCoefLevel);
			if (ret) {
				VPSS_ERR("Vpss_Chn_Set_ScaleCoefInfo %d failed with %#x\n", i, ret);
			}
		}
	}

	return 0;
}

static int _Test_Set_Venc_DuplicateFrame()
{
#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
    VENC_ENCODEPARAM_S EncParam = {0};
    SINT32 SrcFrmRate = 0;
	SINT32 ret = 0;
	
	Sample_Venc_InitVencChnAttr(&gVencInfo.ChnInfo[0], &EncParam, (UINT32*)&SrcFrmRate);
	ret = Venc_Chn_SetDuplicateFrameParam(0, VENC_CHNTYPE_MAIN, 0, EncParam.KeyInterval, EncParam.KeyInterval * 3 / 2);
	if (ret) {
		VPSS_ERR("Venc_Chn_SetDuplicateFrameParam failed with %#x\n", ret);
	}
#endif
	return 0;
}
// #define TEST_DYNAMIC_FPS 1
#ifdef TEST_DYNAMIC_FPS	

static int random_in_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}
#endif
static int _Test_Set_ViFrmRate()
{
	int ret = 0;
	char sCmd[8] = {0};
	FLOAT32 dstFrmRate = 0;
	
 #ifdef TEST_DYNAMIC_FPS	
    SINT32 nCount = 0;
    
    while(1) {
        nCount++;
        dstFrmRate = (FLOAT32)random_in_range(3, 20);
        VENC_TRACE_ERR("Vi_Chn_Set_Attr times(%d) dstFrmRate(%f)\n", nCount, dstFrmRate);
        ret = Vi_Chn_Set_Attr(0, 0, dstFrmRate);        
        if (ret) {
            VENC_TRACE_ERR("Vi_Chn_Set_Attr FAIL(%d)\n", ret);
            break;
        }
        sleep(2);
    }
#else   
	printf("input viFrmRate:\n");
	memset(sCmd, 0, sizeof(sCmd));
	_Venc_Get_Cmd(sCmd);
	dstFrmRate = atof(sCmd);

	ret = Vi_Chn_Set_Attr(0,  0, dstFrmRate);
	if (ret)
	{
		VENC_TRACE_ERR("Vi_Chn_Set_Attr FAIL(%d)\n",ret);
		return -1;
	}
#endif
	return ret;
}

#if defined(__SUPPORT_FFMPEG__) || defined(__SUPPORT_OPENH264__)

#define LCD_INPUT_FILE "rgb565"
static SINT32 _Test_Lcd_Display()
{
    SINT32 Ret = -1;
    FILE *File = NULL;
    SINT32 BuffSize = 0;
    UINT8 *Buf = NULL;
    SINT32 DataSize = 0;

    File = fopen(LCD_INPUT_FILE, "rb");
    if (!File) {
        VENC_TRACE_ERR("open file %s failed \n", LCD_INPUT_FILE);
        goto FOPEN_ERR;
    }

    fseek(File, 0, SEEK_END);
    BuffSize = ftell(File);
    fseek(File, 0, SEEK_SET);
    Buf = malloc(BuffSize);
    if (!Buf) {
        VENC_TRACE_ERR("open file %s failed \n", LCD_INPUT_FILE);
        Ret = -1;
        goto MALLOC_ERR;
    }

    DataSize = fread(Buf, 1, BuffSize, File);
    if (DataSize != BuffSize) {
        goto FREAD_ERR;    
    }

    Ret = Lcd_SendFrame(0, Buf, BuffSize);
    if (Ret) {
        VENC_TRACE_ERR("Lcd_SendFrame FAIL(%d)\n", Ret);    
    }
    
FREAD_ERR:
    if (Buf) {
        free(Buf);
        Buf = NULL;
    }
MALLOC_ERR:
    if (File) { 
        fclose(File);
        File = NULL;
    }
FOPEN_ERR:
    return Ret;
}
#endif

#ifdef __SUPPORT_FFMPEG__
#define VDEC_INPUT_STREAM_FILE "input.h265"
#define VDEC_INPUT_JPEG_FILE "input.jpeg"
#define STREAM_INBUF_SIZE 240 * 320 / 2
#define RGB565_OUTBUF_SIZE 240 * 320 * 2


static SINT32 _Test_Vdec_Stream()
{
    SINT32 Ret = -1;
    FILE *File = NULL;
    SINT32 DataSize = 0;
    UINT8 *InBuf = NULL;

    File = fopen(VDEC_INPUT_STREAM_FILE, "rb");
    if (!File) {
        VENC_TRACE_ERR("open file %s failed \n", VDEC_INPUT_STREAM_FILE);
        goto FOPEN_ERR;
    }

    InBuf = malloc(STREAM_INBUF_SIZE);
    if (!InBuf) {
        VENC_TRACE_ERR("malloc InBuf failed \n");
        goto MALLOC_ERR;
    }

    Ret = _Sample_CreateVdecChn(VPAYLOAD_TYPE_H265);
    if (Ret) {
        VENC_TRACE_ERR("Vdec_CreateChn FAIL(%d)\n", Ret);
        goto CREATE_CHN_ERR;
    }

    while (1) {
        DataSize = fread(InBuf, 1, STREAM_INBUF_SIZE, File);
        if (!DataSize) {
            break;
        }
           
        Vdec_Chn_SendStream(0, VPAYLOAD_TYPE_H265, 0, InBuf, DataSize);
        if (Ret) {
            VENC_TRACE_ERR("Vdec_Chn_SendStream FAIL(%d)\n", Ret);
        }
		sleep(1);
    }
	VENC_TRACE_ERR("Vdec_Chn_SendStream End\n");

	getchar();
    Vdec_Chn_StopStream(0, VPAYLOAD_TYPE_H265, 0);

    _Sample_DestoryVdecChn(VPAYLOAD_TYPE_H265);

CREATE_CHN_ERR:
    if (InBuf) {
        free(InBuf);
        InBuf = NULL;
    }
MALLOC_ERR:
    if (File) { 
        fclose(File);
        File = NULL;
    }
FOPEN_ERR:
    return Ret;
}

static SINT32 _SaveFile(char *FilePath, UINT8 *Data, SINT32 Size)
{
    FILE *fp = fopen(FilePath, "wb");
    if (fp != NULL) {
        fwrite(Data, Size, 1, fp);
        fclose(fp);
        return 0;
    }
    return -1;
}

static SINT32 _Test_Vdec_JpegToRgb565()
{
    SINT32 Ret = -1;
    FILE *File = NULL;
    SINT32 DataSize = 0;
    UINT8 *InBuf = NULL;
    UINT32 InSize = 0;
    UINT8 *OutBuf = NULL;
    UINT32 OutSize = RGB565_OUTBUF_SIZE;

    File = fopen(VDEC_INPUT_JPEG_FILE, "rb");
    if (!File) {
        VENC_TRACE_ERR("open file %s failed \n", VDEC_INPUT_JPEG_FILE);
        goto FOPEN_ERR;
    }

    fseek(File, 0, SEEK_END);
    InSize = ftell(File);
    fseek(File, 0, SEEK_SET);

    InBuf = malloc(InSize);
    if (!InBuf) {
        VENC_TRACE_ERR("malloc InBuf failed \n");
        goto MALLOC_IN_ERR;
    }

    DataSize = fread(InBuf, 1, InSize, File);
    if (InSize != DataSize) {
        VENC_TRACE_ERR("fread failed \n");
        goto FREAD_ERR;
    }

    OutBuf = malloc(OutSize);
    if (!OutBuf) {
        VENC_TRACE_ERR("malloc OutBuf failed \n");
        goto MALLOC_OUT_ERR;
    }
    UINT64 StartTime = get_ms();       
    Ret = _Sample_CreateVdecChn(VPAYLOAD_TYPE_JPEG);
    if (Ret) {
        VENC_TRACE_ERR("Vdec_CreateChn FAIL(%d)\n", Ret);
        goto CREATE_CHN_ERR;
    }
           
    Ret = Vdec_Chn_SendJpegToRgb565(0, VPAYLOAD_TYPE_JPEG, 0, InBuf, InSize, OutBuf, &OutSize);
    if (Ret) {
        VENC_TRACE_ERR("Vdec_Chn_SendJpegToRgb565 FAIL(%d)\n", Ret);
    }

    _Sample_DestoryVdecChn(VPAYLOAD_TYPE_H265);
	UINT64 EndTime = get_ms();  
	VENC_TRACE_ERR("Vdec_Chn_SendJpegToRgb565 Cost time(%lld)ms\n", EndTime-StartTime);
    _SaveFile("./rgb565", OutBuf, OutSize);

CREATE_CHN_ERR:
    if (OutBuf) {
        free(OutBuf);
        OutBuf = NULL;
    }
MALLOC_OUT_ERR:
FREAD_ERR:
    if (InBuf) {
        free(InBuf);
        InBuf = NULL;
    }
MALLOC_IN_ERR:
    if (File) { 
        fclose(File);
        File = NULL;
    }
FOPEN_ERR:
    return Ret;
}
#endif

#ifdef __SUPPORT_OPENH264__
#define VDEC_INPUT_STREAM_FILE "input.h264"
#define STREAM_INBUF_SIZE 240 * 320 / 2
#define RGB565_OUTBUF_SIZE 240 * 320 * 2

/**
 * NAL 单元读取器 - 从 Annex-B H.264 流中逐个读取 NAL 单元
 * 编译: gcc -o nal_reader nal_reader.c
 * 运行: ./nal_reader input.h264
 */


#define MAX_NAL_SIZE (1024 * 1024)  // 每个 NAL 最大 1MB

/**
 * NAL 读取器上下文
 */
typedef struct {
    FILE* file;              // 输入文件句柄
    uint8_t buffer[MAX_NAL_SIZE];  // 内部缓冲区
    int buffer_size;         // 缓冲区有效数据大小
    int buffer_pos;          // 当前读取位置
    int eof;                 // 文件结束标志
} NALReader;

/**
 * 初始化 NAL 读取器
 */
void nal_reader_init(NALReader* reader, FILE* file) {
    reader->file = file;
    reader->buffer_size = 0;
    reader->buffer_pos = 0;
    reader->eof = 0;
}

/**
 * 从文件中填充内部缓冲区
 * @return 1:成功填充, 0:文件结束, -1:读取错误
 */
int refill_buffer(NALReader* reader) {
    if (reader->eof) return 0;
    
    // 移动未处理的数据到缓冲区开头
    if (reader->buffer_pos > 0) {
        int remaining = reader->buffer_size - reader->buffer_pos;
        if (remaining > 0) {
            memmove(reader->buffer, reader->buffer + reader->buffer_pos, remaining);
        }
        reader->buffer_size = remaining;
        reader->buffer_pos = 0;
    }
    
    // 读取新数据
    int bytes_read = fread(reader->buffer + reader->buffer_size, 
                           1, 
                           MAX_NAL_SIZE - reader->buffer_size, 
                           reader->file);
    
    if (bytes_read == 0) {
        if (feof(reader->file)) {
            reader->eof = 1;
            return 0;
        }
        return -1;
    }
    
    reader->buffer_size += bytes_read;
    return 1;
}

/**
 * 查找起始码（支持 3 字节和 4 字节）
 * @param data      数据缓冲区
 * @param size      数据大小
 * @param start_pos 起始搜索位置
 * @param nal_start 输出: NAL 数据起始位置（起始码之后）
 * @param nal_len   输出: 起始码长度（3 或 4）
 * @return 1:找到, 0:未找到
 */
int find_start_code(uint8_t* data, int size, int start_pos, int* nal_start, int* nal_len) {
    for (int i = start_pos; i < size - 3; i++) {
        // 4字节起始码: 0x00 0x00 0x00 0x01
        if (data[i] == 0x00 && data[i+1] == 0x00 && 
            data[i+2] == 0x00 && data[i+3] == 0x01) {
            *nal_start = i + 4;  // 跳过起始码
            *nal_len = 4;
            return 1;
        }
        // 3字节起始码: 0x00 0x00 0x01
        if (data[i] == 0x00 && data[i+1] == 0x00 && data[i+2] == 0x01) {
            *nal_start = i + 3;  // 跳过起始码
            *nal_len = 3;
            return 1;
        }
    }
    return 0;
}

/**
 * 查找下一个起始码位置（用于确定 NAL 结束）
 * @param data      数据缓冲区
 * @param size      数据大小
 * @param start_pos 起始搜索位置
 * @return 下一个起始码的位置，如果未找到返回 size
 */
int find_next_start_code_pos(uint8_t* data, int size, int start_pos) {
    for (int i = start_pos; i < size - 3; i++) {
        // 3字节起始码
        if (data[i] == 0x00 && data[i+1] == 0x00 && data[i+2] == 0x01) {
            return i;
        }
        // 4字节起始码
        if (i + 3 < size && data[i] == 0x00 && data[i+1] == 0x00 && 
            data[i+2] == 0x00 && data[i+3] == 0x01) {
            return i;
        }
    }
    return size;
}

/**
 * 读取下一个 NAL 单元（不含起始码）
 * @param reader    NAL 读取器
 * @param out_nal   输出缓冲区（调用者分配）
 * @param out_size  输出 NAL 大小
 * @param start_code_len 输出起始码长度（3 或 4）
 * @return 1:成功读取, 0:文件结束, -1:错误
 */
int read_next_nal(NALReader* reader, uint8_t* out_nal, int* out_size, int* start_code_len) {
    int nal_start, nal_len, nal_end;
    
    while (!reader->eof || reader->buffer_pos < reader->buffer_size) {
        // 确保缓冲区有足够数据
        if (reader->buffer_pos + 4 >= reader->buffer_size) {
            if (refill_buffer(reader) <= 0) {
                if (reader->eof && reader->buffer_pos < reader->buffer_size) {
                    // 文件结束，处理剩余数据作为最后一个 NAL
                    int remaining = reader->buffer_size - reader->buffer_pos;
                    if (remaining > 0) {
                        memcpy(out_nal, reader->buffer + reader->buffer_pos, remaining);
                        *out_size = remaining;
                        *start_code_len = 0;  // 最后一个 NAL 可能没有结束码
                        reader->buffer_pos = reader->buffer_size;
                        return 1;
                    }
                }
                return 0;
            }
        }
        
        // 查找起始码
        if (!find_start_code(reader->buffer, reader->buffer_size, 
                             reader->buffer_pos, &nal_start, &nal_len)) {
            // 没找到起始码，可能是数据损坏或需要更多数据
            if (reader->eof) {
                return 0;
            }
            // 移动到缓冲区末尾，尝试重新填充
            reader->buffer_pos = reader->buffer_size;
            continue;
        }
        
        // 记录起始码长度
        *start_code_len = nal_len;
        
        // 查找 NAL 结束位置（下一个起始码）
        nal_end = find_next_start_code_pos(reader->buffer, reader->buffer_size, nal_start);
        
        // 提取 NAL 数据
        int nal_size = nal_end - nal_start + nal_len;
        if (nal_size > 0) {
            memcpy(out_nal, reader->buffer + nal_start - nal_len, nal_size);
            *out_size = nal_size;
        } else {
            *out_size = 0;
        }
        
        // 更新读取位置到当前 NAL 结束
        reader->buffer_pos = nal_end;
        
        return 1;
    }
    
    return 0;
}

/**
 * 获取 NAL 单元类型
 * @param nal_data NAL 数据（第一个字节是 header）
 * @return NAL 类型 (0-31)
 */
int get_nal_type(uint8_t* nal_data) {
    if (!nal_data) return -1;
    return nal_data[0] & 0x1F;
}

/**
 * 获取 NAL 类型名称
 */
const char* get_nal_type_name(int type) {
    static const char* names[] = {
        "Unspecified",          // 0
        "Non-IDR Slice",        // 1
        "Slice Data A",         // 2
        "Slice Data B",         // 3
        "Slice Data C",         // 4
        "IDR Slice",            // 5
        "SEI",                  // 6
        "SPS",                  // 7
        "PPS",                  // 8
        "AUD",                  // 9
        "End of Sequence",      // 10
        "End of Stream",        // 11
        "Filler Data",          // 12
        "SPS Extension",        // 13
        "Prefix NAL",           // 14
        "Subset SPS",           // 15
        "Reserved",             // 16
        "Reserved",             // 17
        "Reserved",             // 18
        "Coded Slice Extension" // 19
    };
    if (type >= 0 && type <= 19) return names[type];
    return "Unknown";
}
/**
 * 移除 H.264 防竞争字节 (0x03)
 */
void remove_emulation_bytes(uint8_t* input, int input_size, uint8_t* output, int* output_size) {
    int i = 0, j = 0;
    
    while (i < input_size) {
        if (i + 2 < input_size && input[i] == 0x00 && input[i+1] == 0x00 && input[i+2] == 0x03) {
            output[j++] = 0x00;
            output[j++] = 0x00;
            i += 3;  // 跳过 0x03
        } else {
            output[j++] = input[i];
            i++;
        }
    }
    
    *output_size = j;
}


/**
 * 主函数 - 示例用法
 */
int _Test_Vdec_Stream_Openh264() {
    FILE* file;
    NALReader reader;
    uint8_t* nal_buffer;
    uint8_t* idr_buf;

    int nal_size, start_code_len, nal_type;
    int idr_size = 0;
    int nal_count = 0;
    
    
    file = fopen(VDEC_INPUT_STREAM_FILE, "rb");
    if (!file) {
        fprintf(stderr, "无法打开文件: %s\n", VDEC_INPUT_STREAM_FILE);
        return -1;
    }
    
    nal_buffer = (uint8_t*)malloc(MAX_NAL_SIZE);
    if (!nal_buffer) {
        fprintf(stderr, "内存分配失败\n");
        fclose(file);
        return -1;
    }
    idr_buf = (uint8_t*)malloc(MAX_NAL_SIZE);
    nal_reader_init(&reader, file);
    
    printf("开始读取 NAL 单元...\n");
    printf("========================================\n");
    
    while (1) {
        int ret = read_next_nal(&reader, nal_buffer, &nal_size, &start_code_len);
        
        if (ret == 0) {
            break;  // 文件结束
        }
        if (ret == -1) {
            fprintf(stderr, "读取错误\n");
            break;
        }
        printf("nal_size [%d]\n",nal_size); 
        // 移除防竞争字节
        // int fixed_size;
        // remove_emulation_bytes(nal_buffer, nal_size, fixed_nal, &fixed_size);
        // printf("fixed_size [%d]\n",fixed_size); 

        nal_count++;
        nal_type = get_nal_type(nal_buffer+start_code_len);
        
        printf("NAL #%d:\n", nal_count);
        printf("  起始码长度: %d 字节\n", start_code_len);
        printf("  类型: %d (%s)\n", nal_type, get_nal_type_name(nal_type));
        printf("  大小: %d 字节\n", nal_size);
        
        // 打印前 16 字节的十六进制（用于调试）
        printf("  前16字节: ");
        for (int i = 0; i < (nal_size > 16 ? 16 : nal_size); i++) {
            printf("%02X ", nal_buffer[i]);
        }
        printf("\n\n");
        //SPS + PPS + IDR组合，再送解码器
        if (nal_type == 7 || nal_type == 8 || nal_type ==6) {
            memcpy(idr_buf + idr_size, nal_buffer, nal_size);
            idr_size  += nal_size;
            // printf("nal_type=%d,idr_size=%d\n", nal_type, idr_size);
        } else if (nal_type == 5) {
            memcpy(idr_buf + idr_size, nal_buffer, nal_size);
            idr_size  += nal_size;
            // printf("nal_type=%d,idr_size=%d\n", nal_type, idr_size);
            ret = Vdec_Chn_SendStream(0, VPAYLOAD_TYPE_H264, 0, idr_buf, idr_size);
            if (ret) {
                VENC_TRACE_ERR("Vdec_Chn_SendStream FAIL(%d)\n", ret);
            }
            idr_size = 0;
        } else if (nal_type == 1) {
            idr_size = 0;
            // printf("nal_type=%d,nal_size=%d,idr_size=%d\n", nal_type, nal_size, idr_size);
            ret = Vdec_Chn_SendStream(0, VPAYLOAD_TYPE_H264, 0, nal_buffer, nal_size);
            if (ret) {
                VENC_TRACE_ERR("Vdec_Chn_SendStream FAIL(%d)\n", ret);
            }
        }

        usleep(50*1000);
    }
    
    printf("========================================\n");
    printf("共读取 %d 个 NAL 单元\n", nal_count);
    
    free(nal_buffer);
    fclose(file);
    
    return 0;
}
#endif

int main(int argc, char *argv[])
{
    char sCmd[8] = {0};
    int nCmd = 0;
    int i = 0;
    int ret = 0;
		
	XTrace_Init(1, 2000, 2000);

    demo = rtsp_new_demo(8554);  // rtsp sever socket
    if (NULL == demo) {
        printf("rtsp_new_demo failed\n");
        return -1;
    }

    for (i = 0; i < gVencInfo.ChnNum; i++) {
        char path[32] = {0};
        if (gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP && gVencInfo.ChnInfo[i].ChnType != VENC_CHNTYPE_SNAP_SEC) {
            snprintf(path, sizeof(path), "/live/%d", i);
            session[i] = rtsp_new_session(demo, path);
            printf("==========> rtsp://127.0.0.1:8554%s <===========\n", path);
        }
    }

    if (_Sample_StartVi() != 0) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVi\n");
        return -1;
    }

    if (_Sample_StartVisp() != 0) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVi\n");
        goto sample_stop_vi;
    }

    if (_Sample_StartVpss()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVpss\n");
        goto sample_stop_visp;
    }
#ifdef __MCF__
    if (Mcf_BindVpss(gMcfInfo.McfGrp, gMcfInfo.McfGrp, gVpssInfo.GrpInfo.Grp, 0)) {
        TRACE_ERR(COMP_VIDEOIN, "Mcf_BindVi failed\n");
        goto sample_stop_vpss;
    }
#endif
    if (_Sample_StartVenc()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVenc\n");
        goto sample_stop_vpss;
    }

    if (_Sample_StartOsd()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartOsd\n");
        goto sample_stop_venc;
    }
	
//	_CreateAiCheckPthread();

	
	if (audio_init(AUDIO_G711U, AUDIO_G711U)){
		TRACE_ERR(COMP_VIDEOIN, "audio_init FAIL\n");
		return -1;
	}

    if (_Sample_StartVDA()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVDA\n");
        goto sample_stop_osd;
    }

#if defined(__SUPPORT_FFMPEG__) || defined(__SUPPORT_OPENH264__)
    if (_Sample_StartLcd()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVDA\n");
        goto sample_stop_vda;
    }

    if (_Sample_StartVdec()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVDA\n");
        goto sample_stop_lcd;
    }
#endif

#ifdef __MCF__
    Mcf_StartFusion(0, gMcfInfo.McfGrp);
#endif
    while (1) {
        printf(
            "Venc Sample:\n"
            "\t1 - Switch Encode H264 20FPS\n"
            "\t2 - Switch Main PicFormat to 1920*1080\n"
            "\t3 - Switch Main PicFormat to 2560*1440\n"
            "\t4 - Switch Minor PicFormat to 800*448\n"
            "\t5 - Switch Framerate to 15fps\n"
            "\t6 - Set Main OSD Pos 0 0\n"
            "\t7 - Set Main OSD Pos 128 128\n"
            "\t8 - Set Main OSD Pos 128 128 By Plat Addr\n"
            "\t9 - Delete Main OSD\n"
            "\t10 - Set Main Mosaic\n"
            "\t11 - Delete Main Mosaic\n"
            "\t12 - Get Md Data\n"
            "\t13 - Draw Line&Rect in Main\n"
            "\t14 - Clear Line&Rect in Main\n"
            "\t15 - Get AI Frm to /tmp\n"
            "\t16 - QR\n"
            "\t17 - Set CenterZoom\n"
            "\t18 - Set mir flip mode\n"
            "\t19 - Switch mipi 8MP to 6MP or 6MP to 8MP\n"
            "\t20 - Set Bcd Param\n"
            "\t21 - Set Vpss scale coef level\n"
            "\t22 - Set Venc DuplicateFrame\n"
            "\t23 - Set Vi FrameRate\n"
            "\t24 - Switch sns mode: 8MP to 2MP or 2MP to 8MP\n"
            "\t25 - Switch sns mode: 8MP to 6MP or 6MP to 8MP\n"
            "\t26 - Set Zoom\n"
            "\t27 - Lcd Display\n"
            "\t28 - Vdec Stream\n"
			"\t29 - Vdec Jpeg\n"
            "\t30 - Vdec H264\n"
            "\tq - quit\n");

        _Venc_Get_Cmd(sCmd);

        if ((sCmd[0] == 'q') || (sCmd[0] == 'Q')) {
            TRACE_DBG("get cmd to exit \n");
            break;
        }

        nCmd = atoi(sCmd);
        switch (nCmd) {
            case 1:
                _Test_SwitchVencH264CBR_20fps();
                break;
            case 2:
                _Test_SwitchMain_H264_1080P(1920, 1080);
                break;
			case 3:
				_Test_SwitchMain_H264_1080P(2560, 1440);
				break;
			case 4:
				_Test_SwitchMinor_H264_448P();
				break;
            case 5:
                //_Test_SwitchFramerate_15fps();
                _Test_Set_FrmRate();
                break;
            case 6:
                _Test_SetMainOSD_x0y0();
                break;
            case 7:
                _Test_SetMainOSD_x128y128();
                break;
			case 8:
				_Test_SetMainOSD_x128y128_ByPlatAddr();
				break;
            case 9:
                _Test_DelMainOSD();
                break;
            case 10:
                _Test_SetMosaic();
                break;
            case 11:
                _Test_DelMosaic();
                break;
            case 12:
                _Test_GetMdResult();
                break;
            case 13:
                _Test_SetOsdLineAndRect();
                break;
            case 14:
                _Test_DelOsdLineAndRect();
                break;
			case 15:
				_Test_GetAiFrame();
				break;
			case 16:
				_Test_DECODE_QR();
				break;
			case 17:
				_Test_Set_CenterZoomTimes();
				break;
			case 18:
				_Test_Set_Visp_HVFLIP();
				break;
			case 19:
#ifdef __SUPPORT_MAX_MAIN_ENC_RESOLUTION_8MP__
				_Test_SwitchMIPI_8MPTo6MP();
#endif
				break;
			case 20:
#ifdef __SUPPORT_BCD__
				_Test_Set_BcdParam();
#endif
				break;
			case 21:
				_Test_Set_ScaleCoefLevel();
				break;
			case 22:
				_Test_Set_Venc_DuplicateFrame();
				break;
			case 23:
				_Test_Set_ViFrmRate();
				break;
			case 24:	
				_Test_SwitchSnsMode_8MPTo2MP();
				break;
			case 25:	
				_Test_SwitchSnsMode_8MPTo6MP();
				break;
			case 26:
				_Test_Set_Zoom();
				break;
#if defined(__SUPPORT_FFMPEG__) || defined(__SUPPORT_OPENH264__)
            case 27:
                _Test_Lcd_Display();
                break;
#ifdef __SUPPORT_FFMPEG__                
            case 28:
                _Test_Vdec_Stream();
                break;
            case 29:
                _Test_Vdec_JpegToRgb565();
                break;
#endif
#ifdef __SUPPORT_OPENH264__
            case 30:
                ret = _Sample_CreateVdecChn(VPAYLOAD_TYPE_H265);
                if (ret) {
                    VENC_TRACE_ERR("Vdec_CreateChn FAIL(%d)\n", ret);
                }
                _Test_Vdec_Stream_Openh264();
                _Sample_DestoryVdecChn(VPAYLOAD_TYPE_H264);
                break;
#endif
#endif
            default:
                VENC_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
                break;
        }
    }
#ifdef __MCF__
    Mcf_StopFusion(0, gMcfInfo.McfGrp);
#endif

	if (audio_unInit()){
		TRACE_ERR(COMP_VIDEOIN, "audio_unInit FAIL\n");
		//return -1;
	}
//	_DestoryAiCheckPthread();
#if defined(__SUPPORT_FFMPEG__) || defined(__SUPPORT_OPENH264__)
    _Sample_StopVdec();
sample_stop_lcd:
    _Sample_StopLcd();
sample_stop_vda:
#endif
    _Sample_StopVDA();
sample_stop_osd:
    _Sample_StopOsd();
sample_stop_venc:
    _Sample_StopVenc();
#ifdef __MCF__
sample_mcf_unbind_vpss:
    Mcf_UnBindVpss(gMcfInfo.McfGrp, 0, gVpssInfo.GrpInfo.Grp, 0);
#endif
sample_stop_vpss:
    _Sample_StopVpss();
sample_stop_visp:
    _Sample_StopVisp();
sample_stop_vi:
    _Sample_StopVi();

	XTrace_Destory();
	
    return 0;
}

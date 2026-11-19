
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "mcf.h"
#include "msstd.h"
#include "rtsp.h"
#include "vda.h"
#include "venc.h"
#include "vi.h"
#include "visp.h"
#include "vosd.h"
#include "vpss.h"
#include "xtrace.h"

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
#define SENSOR_WIDTH				2592
#define SENSOR_HEIGHT				1944
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
#define MAIN_ENC_WIDTH				2592
#define MAIN_ENC_HEIGHT				1944
#define MAIN_ENC_STREAM_BUF_SIZE 	768*1024
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
#define MAX_FRAMERATE               25
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
					.Type = VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT,
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
                    .Type = VISP_SENSOR_CS6286_MIPI_8M_30FPS_12BIT_4LANE,
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

static int _Sample_InitVencChn()
{
    int ret = 0;
    VENC_CHNTYPE_E ChnType = VENC_CHNTYPE_MAIN;
    VENC_BIND_CONF_S BindConf = {0};
    VENC_ENCODEPARAM_S EncParam = {0};
	UINT32 SrcFrameRate = 0;
    int i = 0;

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

#ifdef __SUPPORT_AIISP__
static SINT32 _Sample_StartAiIsp()
{
    VISP_AIISP_ATTR_S AiIspAttr = {0};
    SINT32 Ret = 0;
    
    AiIspAttr.EnAi3DNR = XRS_FALSE;
    AiIspAttr.EnAiBNR = XRS_TRUE;
    AiIspAttr.Width = gViInfo.ChnInfo[0].Width;
    AiIspAttr.Height = gViInfo.ChnInfo[0].Height;

    Ret = Visp_Start_AiIsp(0, 0, &AiIspAttr);
    if (Ret != 0) {
        VISP_ERR("Visp_Start_AiIsp failed!\n");
        return -1;
    }

    return 0;
}

static void _Sample_StopAiIsp()
{
    Visp_Stop_AiIsp(0, 0);
}
#endif
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

static void _Venc_Get_Cmd(char *pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar())) {
        i++;
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
}

static void _Test_SetMainOSD_x0y0()
{
    VRECT_S Rect = {0};

    Rect.x = 0;
    Rect.y = 0;
    Rect.w = 480;
    Rect.h = 32;
    __DrawFontByBitmap(&Rect, "./151_stream_1_textOSD_xywh_0_0_480_32", VOSD_CHNTYPE_VENC_MAIN);
}

static void _Test_DelMainOSD() { Vosd_Chn_DelBitMap(0, VOSD_CHNTYPE_VENC_MAIN, 0); }

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

    Vosd_Chn_Set_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN, 0, VMosaic);
}

static void _Test_DelMosaic() { Vosd_Chn_Del_MosaicParam(0, VOSD_CHNTYPE_VENC_MAIN, 0); }

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

static void _Test_SetOsdLineAndRect()
{
    SINT32 Ret = 0;
    VLINE_FRAME_S VLineFrm = {
        .LineW = 2,
        .LineCnt = 4,
        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_RED, VCOLOR_RGBA_BLACK, VCOLOR_RGBA_WHITE},
        .Line =
            {
                {256, 256, 512, 512},
                {256, 512, 512, 256},
                {256, 256, 512, 256},
                {512, 256, 512, 512},
            },
    };

    VFACE_FRAME_S VRectFrm = {
        .LineW = 2,
        .RectCnt = 4,
        .Color = {VCOLOR_RGBA_BLUE_PURE, VCOLOR_RGBA_RED, VCOLOR_RGBA_BLACK, VCOLOR_RGBA_WHITE},
        .Rect =
            {
                {0, 0, 64, 64},
                {128, 128, 64, 64},
                {0, 128, 64, 64},
                {128, 0, 64, 64},
            },
    };

    Ret = Vosd_Chn_Draw_Line(0, VOSD_CHNTYPE_VENC_MAIN, 0, VLineFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }

    Ret = Vosd_Chn_Draw_Rect(0, VOSD_CHNTYPE_VENC_MAIN, 0, VRectFrm);
    if (Ret != 0) {
        VENC_TRACE_ERR("Vosd_Chn_Draw_Line failed!\n");
        return;
    }
}

static void _Test_DelOsdLineAndRect()
{
    Vosd_Chn_Del_Rect(0, VOSD_CHNTYPE_VENC_MAIN, 0);
    Vosd_Chn_Del_Line(0, VOSD_CHNTYPE_VENC_MAIN, 0);
}

static void _Test_Set3DnrLevelAuto()
{
    ISP_TUNE_NR_PARAM_S NrParam = {0};

    NrParam.Nr2dEnable = 1;
    NrParam.Nr2dLevel = 50;

    NrParam.Nr3dEnable = 1;
    NrParam.Nr3dLevel = 50;

    Visp_Set_TuneNoiseReduction(0, &NrParam);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Visp_Set_TuneNoiseReduction(1, &NrParam);
#endif
}

static void _Test_Set3DnrLevel()
{
    ISP_TUNE_NR_PARAM_S NrParam;

    NrParam.Nr2dEnable = 1;
    NrParam.Nr2dLevel = 75;

    NrParam.Nr3dEnable = 1;
    NrParam.Nr3dLevel = 75;

    Visp_Set_TuneNoiseReduction(0, &NrParam);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Visp_Set_TuneNoiseReduction(1, &NrParam);
#endif
}

static void _Test_SetHDR(int level)
{
    ISP_TUNE_EXT_AE_PARAM_S AeExtParam;

    AeExtParam.ExtAeMode = ISP_TUNE_EXT_AE_HDR;
    AeExtParam.Level = level;

    Visp_Set_TuneExtendAutoExposure(0, &AeExtParam);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Visp_Set_TuneExtendAutoExposure(1, &AeExtParam);
#endif
}

static void _Test_SetBLC(int level)
{
    ISP_TUNE_EXT_AE_PARAM_S AeExtParam;

    AeExtParam.ExtAeMode = ISP_TUNE_EXT_AE_BLC;
    AeExtParam.Level = level;

    Visp_Set_TuneExtendAutoExposure(0, &AeExtParam);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Visp_Set_TuneExtendAutoExposure(1, &AeExtParam);
#endif
}

static void _Test_SetHLC(int level)
{
    ISP_TUNE_EXT_AE_PARAM_S AeExtParam;

    AeExtParam.ExtAeMode = ISP_TUNE_EXT_AE_HLC;
    AeExtParam.Level = level;

    Visp_Set_TuneExtendAutoExposure(0, &AeExtParam);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Visp_Set_TuneExtendAutoExposure(1, &AeExtParam);
#endif
}

static void _Test_SetImageStyle(int strength)
{
    SINT32 Ret = 0;
    ISP_TUNE_IMAGE_STYLE_PARAM_S param;

    param.Sharpen = strength;
    param.Contrast = strength;
    param.Hue = strength;
    param.Luma = strength;
    param.Saturation = strength;
    Ret = Visp_Set_TuneImageStyle(0, &param);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneImageStyle fail\n");
    }
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Visp_Set_TuneImageStyle(1, &param);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneImageStyle fail\n");
    }
#endif
}

static void _Test_SetDehaze(int strength)
{
    SINT32 Ret = 0;
    ISP_TUNE_DEHAZE_PARAM_S param;

    param.DehazeMode = ISP_TUNE_DEHAZE_ON;
    param.DehazeLevel = strength;
    Ret = Visp_Set_TuneDehaze(0, &param);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneDehaze fail\n");
    }
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Visp_Set_TuneDehaze(1, &param);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneDehaze fail\n");
    }
#endif
}

static void _Test_SetAeWeight()
{
    SINT32 Ret = 0, i, j;
    ISP_AE_WEIGHT_ATTR_S weight;

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 17; j++) {
            weight.aeWeight.weight[i][j] = 1;
        }
    }

    weight.enable = 1;
    weight.aeWeight.weight[0][0] = 15;

    Ret = Visp_Set_AEWeight(0, &weight);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_AEWeight fail\n");
    }
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Visp_Set_AEWeight(1, &weight);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_AEWeight fail\n");
    }
#endif
}

static void _Test_SetAtList()
{
    SINT32 Ret = 0, i, j;
    ISP_ATLIST_S atList;

    atList.enable = 1;
    atList.compensation = 45;

    Ret = Visp_Set_AtList(0, &atList);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_AtList fail\n");
    }
}

static void _Test_SetAeAuto()
{
    ISP_TUNE_AE_PARAM_S AeParam = {0};
    SINT32 Ret = 0;

    AeParam.FlickerMode = 1;
    AeParam.AeMode = 1;
    AeParam.ExposureSpeed = 50;
    AeParam.ExposureLuma = 50;
    AeParam.MaxExposureTime = 0.030;
    AeParam.MinExposureTime = 0.001;
    AeParam.ExposureRect.StartX = 0;
    AeParam.ExposureRect.StartY = 0;
    AeParam.ExposureRect.Width = 10000;
    AeParam.ExposureRect.Height = 10000;

    Ret = Visp_Set_TuneAutoExposure(0, &AeParam);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneAutoExposure fail\n");
    }
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Visp_Set_TuneAutoExposure(1, &AeParam);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneAutoExposure fail\n");
    }
#endif
}

static void _Test_SetAeManual()
{
    ISP_TUNE_AE_PARAM_S AeParam = {0};
    SINT32 Ret = 0;
    static int times = 0;

    AeParam.AeMode = ISP_TUNE_AE_MANUAL;
    AeParam.ExposureTime = 0.23;
    if (times >= 10) {
        times = 0;
    } else {
        times++;
    }
    AeParam.ExposureGain = 10 * times;

    Ret = Visp_Set_TuneAutoExposure(0, &AeParam);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneAutoExposure fail\n");
    }
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Visp_Set_TuneAutoExposure(1, &AeParam);
    if (Ret != 0) {
        VISP_ERR("Visp_Set_TuneAutoExposure fail\n");
    }
#endif
}

static void _Test_GetSceneFps()
{
    ISP_SCENE_FPS_S fps = {0};
    SINT32 Ret = 0;

    Ret = Visp_Get_SceneFps(0, &fps);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Visp_Get_SceneFps\n");
        return;
    }

    printf("fps: %d, gop: %d, ae_time: %f\n", fps.Fps, fps.Gop, fps.AeTime);
#if defined(BOARD_H82W) || defined(BOARD_G82W)
    Ret = Visp_Get_SceneFps(0, &fps);
    if (Ret != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Visp_Get_SceneFps\n");
        return;
    }

    printf("fps: %d, gop: %d, ae_time: %d\n", fps.Fps, fps.Gop, fps.AeTime);
#endif
}

ISP_AF_WEIGHT_S afWeight = {{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
}};
static void _Test_GetAfData()
{
    UINT32 AFMetrics;
    SINT32 Ret = 0;
    static int first = 1;

    if (first) {
        Visp_Set_AFWeight(0, &afWeight);
        first = 0;
    }
    
    Ret = Visp_Get_AFMetrics(0, &AFMetrics);
    if (Ret != 0) {
        VISP_ERR("Visp_Get_AFMetrics fail!\n");
        return;
    }

    VISP_INFO("AF value = %d\n", AFMetrics);
}

int main(int argc, char *argv[])
{
    char sCmd[8] = {0};
    int nCmd = 0;
    int i = 0;

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

    XTrace_Init(1, 1000, 2000);

    if (_Sample_StartVi() != 0) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVi\n");
        return -1;
    }

    if (_Sample_StartVisp() != 0) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVi\n");
        goto sample_stop_vi;
    }

    if (_Sample_StartVpss() != 0) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVpss\n");
        goto sample_stop_visp;
    }
#ifdef __MCF__
    if (Mcf_BindVpss(gMcfInfo.McfGrp, gMcfInfo.McfGrp, gVpssInfo.GrpInfo.Grp, 0)) {
        TRACE_ERR(COMP_VIDEOIN, "Mcf_BindVi failed\n");
        goto sample_stop_vpss;
    }
#endif
    if (_Sample_StartVenc() != 0) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVenc\n");
        goto sample_stop_vpss;
    }
#ifdef __SUPPORT_AIISP__
    if (_Sample_StartAiIsp()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartAiIsp\n");
        goto sample_stop_venc;
    }
#endif
    if (_Sample_StartOsd()) {
        TRACE_ERR(COMP_VIDEOIN, "_Sample_StartOsd\n");
        goto sample_stop_aiisp;
    }

    // if (_Sample_StartVDA()) {
    //     TRACE_ERR(COMP_VIDEOIN, "_Sample_StartVDA\n");
    //     goto sample_stop_osd;
    // }

    if (Visp_Start_SceneAuto(0, "./scene/", 0) != 0) {
        TRACE_ERR(COMP_VIDEOIN, "Visp_Start failed\n");
        goto sample_stop_vda;
    }

#ifdef __MCF__
    Mcf_StartFusion(0, gMcfInfo.McfGrp);
#endif

    while (1) {
        printf(
            "Venc Sample:\n"
            "\t1 - set 3dnr level 0\n"
            "\t2 - set 3dnr level 1\n"
            "\t3 - set hdr auto\n"
            "\t4 - set hdr normal\n"
            "\t5 - set image style 50\n"
            "\t6 - set image style 100\n"
            "\t7 - set dehaze 50\n"
            "\t8 - set dehaze 100\n"
            "\t9 - set blc 50\n"
            "\t10 - set hlc 50\n"
            "\t11 - start pqtool serve\n"
            "\t12 - stop pqtool serve\n"
            "\t13 - pause scene auto\n"
            "\t14 - continue scene auto\n"
            "\t15 - switch scene mode normal\n"
            "\t16 - switch scene mode ir\n"
            "\t17 - set ae weight\n"
            "\t18 - set ae auto\n"
            "\t19 - set ae manual\n"
            "\t20 - get scene fps\n"
            "\t21 - get AF\n"
            "\t22 - set AtList\n"
            "\tq - quit\n");

        _Venc_Get_Cmd(sCmd);

        if ((sCmd[0] == 'q') || (sCmd[0] == 'Q')) {
            TRACE_DBG("get cmd to exit \n");
            break;
        }

        nCmd = atoi(sCmd);
        switch (nCmd) {
            case 1:
                _Test_Set3DnrLevelAuto();
                break;
            case 2:
                _Test_Set3DnrLevel();
                break;
            case 3:
                _Test_SetHDR(50);
                break;
            case 4:
                _Test_SetHDR(100);
                break;
            case 5:
                _Test_SetImageStyle(50);
                break;
            case 6:
                _Test_SetImageStyle(75);
                break;
            case 7:
                _Test_SetDehaze(50);
                break;
            case 8:
                _Test_SetDehaze(100);
                break;
            case 9:
                _Test_SetBLC(50);
                break;
            case 10:
                _Test_SetHLC(50);
                break;
            case 11:
                Visp_Start_TuneServe(0);
                break;
            case 12:
                Visp_Stop_TuneServe(0);
                break;
            case 13:
                Visp_Pause_SceneAuto(0, 1);
                break;
            case 14:
                Visp_Pause_SceneAuto(0, 0);
                break;
            case 15:
                Visp_Switch_SceneMode(0, 0);
                break;
            case 16:
                Visp_Switch_SceneMode(0, 1);
                break;
            case 17:
                _Test_SetAeWeight();
                break;
            case 18:
                _Test_SetAeAuto();
                break;
            case 19:
                _Test_SetAeManual();
                break;
            case 20:
                _Test_GetSceneFps();
                break;
            case 21:
                _Test_GetAfData();
                break;
            case 22:
                _Test_SetAtList();
                break;
            default:
                VENC_TRACE_ERR("unsupported nCmd, %d\n", nCmd);
                break;
        }
    }
#ifdef __MCF__
    Mcf_StopFusion(0, gMcfInfo.McfGrp);
#endif
    Visp_Stop_SceneAuto(0);
sample_stop_vda:
    // _Sample_StopVDA();
sample_stop_osd:
    _Sample_StopOsd();
sample_stop_aiisp:
#ifdef __SUPPORT_AIISP__
    _Sample_StopAiIsp();
#endif
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

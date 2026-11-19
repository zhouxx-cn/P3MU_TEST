#if defined(__IS_SUPPORT_LCD_MANAGER__)

#include "Xrs_Lcd.h"
#include "lcd.h"
#include "vdec.h"
#include "halmod.h"
#include "halmod_types.h"
#include "Common.h"

#define USE_VDEC (1)
#define USE_OPENH264 (0)    //使用开源openh264，与USE_VDEC互斥

#define XRS_LCD_WIDTH (240)
#define XRS_LCD_HEIGHT (320)

#if USE_OPENH264
#include <limits.h>   // 提供 UCHAR_MAX
#include "codec_api.h"
#include "codec_def.h"

static ISVCDecoder* g_decoder = NULL;
static SBufferInfo g_sDstBufInfo = {0};
static pthread_mutex_t g_decoder_mutex = PTHREAD_MUTEX_INITIALIZER;

// --- RGB565 转换函数 (整数运算版, 约1KB) ---
// 将YUV420P的一帧数据转换为RGB565格式
// 参数: yuv[3] 指向Y, U, V平面的指针
//       width, height: 图像尺寸
//       stride[3] 各平面的 stride(每行实际字节数)
//       rgb565_out: 输出缓冲区，大小为 width * height * 2 字节
static void yuv420p_to_rgb565_stride(const char* yuv[3], int width, int height, int stride[3], char* rgb565_out)
{
    int i, j;
    const char* y_plane = yuv[0];
    const char* u_plane = yuv[1];
    const char* v_plane = yuv[2];
    
    int y_stride = stride[0];
    int uv_stride = stride[1];  // U 和 V 的 stride 通常相同

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // 使用 stride 计算实际内存偏移
            int y_idx = i * y_stride + j;
            int y = y_plane[y_idx];
            
            int uv_i = i / 2;
            int uv_j = j / 2;
            int uv_idx = uv_i * uv_stride + uv_j;
            int u = u_plane[uv_idx] - 128;
            int v = v_plane[uv_idx] - 128;

            int r = y + ((359 * v) >> 8);
            int g = y - ((88 * u + 183 * v) >> 8);
            int b = y + ((454 * u) >> 8);

            if (r < 0) {r = 0;} if (r > 255) {r = 255;}
            if (g < 0) {g = 0;} if (g > 255) {g = 255;}
            if (b < 0) {b = 0;} if (b > 255) {b = 255;}

            unsigned short rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
            int out_idx = (i * width + j) * 2;  // RGB565 输出没有 stride，紧凑排列
            rgb565_out[out_idx] = rgb565 & 0xFF;
            rgb565_out[out_idx + 1] = (rgb565 >> 8);
        }
    }
}
#endif

SINT32 Xrs_Lcd_StartLCD(void *hChannelHandle)
{
    SINT32 Ret = 0;
    LCD_CONF_S LcdConf = {
        .Width = XRS_LCD_WIDTH,
        .Height = XRS_LCD_HEIGHT
    };

    Ret = Lcd_Open(0, LcdConf);
    if (Ret != 0) {
        ZW_ERROR("Lcd_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Lcd_Start(0, 60);
    if (Ret != 0) {
        ZW_ERROR("Lcd_Start failed, ret = %d\n", Ret);
        Lcd_Close(0);
        return -1;
    }

	ZW_DEBUG("Hi_MPP_StartOSD Success\n");
	return 0;
}

SINT32 Xrs_Lcd_StopLCD(void)
{
	int s32Ret = 0;

    s32Ret = Lcd_Stop(0);
	if (s32Ret < 0)
	{
		ZW_ERROR("[LCD] Lcd_Stop failed with %#x\n", s32Ret);
	}

    s32Ret = Lcd_Close(0);
	if (s32Ret < 0)
	{
		ZW_ERROR("[LCD] Lcd_Close failed with %#x\n", s32Ret);
	}

    return 0;
}

SINT32 Xrs_Lcd_RefreshFrame(char *pData, UINT32 DataSize)
{
	int s32Ret = 0;

    s32Ret = Lcd_SendFrame(0, (UINT8 *)pData, DataSize);
	if (s32Ret < 0)
	{
		ZW_ERROR("[LCD] Lcd_SendFrame failed with %#x\n", s32Ret);
	}

    return s32Ret;
}

SINT32 Xrs_Lcd_SetBackLightLevel(int level)
{
	int s32Ret = 0;    
    HALMOD_LIGHT_ATTR_S LightAttr = { 0 };

    LightAttr.Type = LIGHT_TYPE_BACK;
    LightAttr.Action = level ? HALMOD_LIGHT_ACTION_ON : HALMOD_LIGHT_ACTION_OFF;
    LightAttr.Data = level;
    LightAttr.FrqKHZ = 20;

    s32Ret = HalMod_Set_LightAttr(0, LightAttr);
    if (s32Ret < 0)
    {
        ZW_ERROR("[LCD] HalMod_Set_LightAttr failed!\n");
    }

    return s32Ret;
}

SINT32 Xrs_Lcd_StartVdec(void *hChannelHandle)
{
#if USE_VDEC
    SINT32 Ret = 0;
    VDEC_CONF_S VdecConf = {
        .vsize = VIDEO_PIC_QVGA,
    };

    Ret = Vdec_Open(VdecConf, Lcd_SendFrame);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Open failed, ret = %d\n", Ret);
        return -1; 
    }

    Ret = Vdec_Start(0, 60);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Start failed, ret = %d\n", Ret);
        Vdec_Close(0);
        return -1; 
    }
#endif
    return 0;
}

SINT32 Xrs_Lcd_StopVdec(void)
{
#if USE_VDEC
	int s32Ret = 0;

    s32Ret = Vdec_Stop(0);
	if (s32Ret < 0)
	{
		ZW_ERROR("[LCD] Vdec_Stop failed with %#x\n", s32Ret);
	}

    s32Ret = Vdec_Close(0);
	if (s32Ret < 0)
	{
		ZW_ERROR("[LCD] Vdec_Close failed with %#x\n", s32Ret);
	}
#endif
    return 0;
}

SINT32 Xrs_Lcd_CreateVdecChn(int nType)
{
#if USE_VDEC
    SINT32 Ret = 0;
    VPAYLOAD_TYPE_E Type = VPAYLOAD_TYPE_H264;
    VDEC_CHN_ATTR_S Attr = {
        .Width = XRS_LCD_WIDTH,
        .Height = XRS_LCD_HEIGHT,
    };

    Ret = Vdec_Chn_Open(0, Type, 0, &Attr);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Chn_Open failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Vdec_Chn_Start(0, Type, 0);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Chn_Start failed, ret = %d\n", Ret);
        Vdec_Chn_Close(0, Type, 0);
        return -1;
    }
#elif USE_OPENH264
    pthread_mutex_lock(&g_decoder_mutex);
    // 初始化OpenH264解码器
    if (g_decoder == NULL)
    {
        if (WelsCreateDecoder(&g_decoder) != 0) {
            ZW_ERROR("WelsCreateDecoder failed\n");            
            pthread_mutex_unlock(&g_decoder_mutex);
            return -1;
        }

        SDecodingParam decParam = {0};
        decParam.uiTargetDqLayer = UCHAR_MAX;       // 解码所有层
        decParam.eEcActiveIdc = ERROR_CON_SLICE_COPY; // 错误隐藏策略
        decParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC; // H.264裸流类型

        if ((*g_decoder)->Initialize(g_decoder, &decParam) != 0) {
            ZW_ERROR("Decoder Initialize failed\n");
            (*g_decoder)->Uninitialize(g_decoder);
            WelsDestroyDecoder(g_decoder);
            g_decoder = NULL;            
            pthread_mutex_unlock(&g_decoder_mutex);
            return -1;
        }
    }
    pthread_mutex_unlock(&g_decoder_mutex);
#endif

    ZW_INFO("[LCD] CreateVdecChn Success! \n");
    return 0;
}

SINT32 Xrs_Lcd_SendStreamToChn(int nType, char *pData, UINT32 DataSize)
{
#if USE_VDEC
    SINT32 Ret = 0;
    VPAYLOAD_TYPE_E Type = VPAYLOAD_TYPE_H264;

    Ret = Vdec_Chn_SendStream(0, Type, 0, (UINT8 *)pData, DataSize);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Chn_SendStream failed, ret = %d\n", Ret);
        return -1;
    }
#elif USE_OPENH264
    pthread_mutex_lock(&g_decoder_mutex);
    if (g_decoder == NULL) {
        ZW_ERROR("[LCD] SendStreamToCh Failed! NULL\n");        
        pthread_mutex_unlock(&g_decoder_mutex);
        return -1;
    }

    int ret = 0;
    char* p_Openh264Data[3] = {NULL, NULL, NULL};

    ret = (*g_decoder)->DecodeFrame2(g_decoder, (const unsigned char*)pData, DataSize, (unsigned char**)p_Openh264Data, &g_sDstBufInfo);
    if (ret != 0) {
        ZW_ERROR("DecodeFrame2 error: %d\n", ret);        
        pthread_mutex_unlock(&g_decoder_mutex);
        return -1;
    }

    // 检查是否有新的帧解码完成
    if (g_sDstBufInfo.iBufferStatus == 1) {
        // 获取图像尺寸信息
        int width = g_sDstBufInfo.UsrData.sSystemBuffer.iWidth;
        int height = g_sDstBufInfo.UsrData.sSystemBuffer.iHeight;

        if (width == XRS_LCD_WIDTH && height == XRS_LCD_HEIGHT)
        {
            // 将YUV420P转换为RGB565
            char* rgb565_data = (char*)malloc(width * height * 2);
            if (rgb565_data) {
                // 准备YUV平面指针
                const char* yuv_planes[3] = {p_Openh264Data[0], p_Openh264Data[1], p_Openh264Data[2]};
            
                // 获取 stride（每行实际字节数）
                int stride[3] = {
                    g_sDstBufInfo.UsrData.sSystemBuffer.iStride[0],  // Y stride
                    g_sDstBufInfo.UsrData.sSystemBuffer.iStride[1],  // U stride
                    g_sDstBufInfo.UsrData.sSystemBuffer.iStride[1]   // V stride
                };

                // YUV420->RGB565
                yuv420p_to_rgb565_stride(yuv_planes, width, height, stride, rgb565_data);

                // 处理RGB565帧
                Xrs_Lcd_RefreshFrame(rgb565_data, width * height * 2);

                free(rgb565_data);
            }
        }
    }
    pthread_mutex_unlock(&g_decoder_mutex);
#endif

    //ZW_INFO("[LCD] SendStreamToCh Success! \n");
    return 0;
}

SINT32 Xrs_Lcd_DestoryVdecChn(int nType)
{
#if USE_VDEC
    SINT32 Ret = 0;
    VPAYLOAD_TYPE_E Type = VPAYLOAD_TYPE_H264;

    Ret = Vdec_Chn_Stop(0, Type, 0);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Chn_Stop failed, ret = %d\n", Ret);
        return -1;
    }

    Ret = Vdec_Chn_Close(0, Type, 0);
    if (Ret != 0) {
        ZW_ERROR("Vdec_Chn_Close failed, ret = %d\n", Ret);
        return -1;
    }
#elif USE_OPENH264
    pthread_mutex_lock(&g_decoder_mutex);
    if (g_decoder != NULL) {
        (*g_decoder)->Uninitialize(g_decoder);
        WelsDestroyDecoder(g_decoder);
        g_decoder = NULL;
    }

    // 清零解码器状态，确保下次创建时干净
    memset(&g_sDstBufInfo, 0, sizeof(SBufferInfo));
    pthread_mutex_unlock(&g_decoder_mutex);
#endif

    ZW_INFO("[LCD] DestoryVdecChn Success! \n");
    return 0;
}

#endif

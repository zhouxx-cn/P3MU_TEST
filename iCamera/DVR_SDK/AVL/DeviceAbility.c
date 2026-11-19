#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#include "zwsdk.h"

#include "mxml.h"
// #include "sys_banned.h"
#include "global_gather.h"



//#define TEST

#define ERR_LOG(format, args...)  do{printf("\033[1;31m[DeviceInfo][%s:%d]\033[m" format, __FUNCTION__, __LINE__, ##args);}while(0)

/* 高级编码参数枚举 */
typedef enum 
{
	eVideoCodeType_H264  = 0x0,
	eVideoCodeType_H265  = 0x1,
	eVideoCodeType_MJPEG = 0x2,
	eVideoCodeType_H265Plus  = 0x3,
	eVideoCodeType_H264Plus  = 0x4,
	eVideoCodeType_num	 = 0x5,
}eVideoCodeType;

/* 分辨率枚举 */
typedef enum
{
	eResolutionType_CIF     = 0,
	eResolutionType_D1      = 1,
	eResolutionType_SD      = 2,
	eResolutionType_QuadVGA = 3,
	eResolutionType_SXGA    = 4,
	eResolutionType_UXGA    = 5,
	eResolutionType_720P    = 6,
	eResolutionType_1080P   = 7,
	eResolutionType_VGA     = 8,
	eResolutionType_QXGA    = 9,
	eResolutionType_640x360 = 10,
	eResolutionType_QVGA    = 11,
    eResolutionType_UWXGA   = 12,   //UWXGA(2592×1520)
    eResolutionType_USXGA   = 13,   //USXGA(2592×1944)
    eResolutionType_320X176 = 14,
    eResolutionType_480X256 = 15,
	eResolutionType_800X448 = 16,
	eResolutionType_1600X900 = 17,
	eResolutionType_1408X792 = 18,
    eResolutionType_num     = 32,
}eResolutionType;

/* 码率类型枚举 */
typedef enum
{
    eBitrateType_CBR      = 0x0,
    eBitrateType_VBR      = 0x1,
    eBitrateType_SmartAVC = 0x2,
    eBitrateType_num      = 0x3,
}eBitrateType;

/* 码流类型枚举 */
typedef enum
{
    eStreamType_VIDEO       = 0x0,
    eStreamType_VIDEO_AUDIO = 0x1,
    eStreamType_num         = 0x2,
}eStreamType;


typedef enum
{
	ePicFunc_DeFrog			=0x0,		//支持去雾
	ePicFunc_ShutterMode 		=0x1,		//支持快门模式
	ePicFunc_LDC				=0x2,		//支持镜头矫正
	ePicFunc_num				=0x3,
	
}ePicFunc;

/* 单个码流的编码信息 */
typedef struct tagStreamInfo
{
    char sVideoCoder[4][MAX_STRING_CNT];                   /* 高级编码参数:H264/H265/MJPEG */
    char sResolution[MAX_RESOLUTION_CNT][MAX_STRING_CNT];  /* 码流分辨率支持的枚举 */
    char sBitrateType[4][MAX_STRING_CNT];                  /* 码流码率类型支持的枚举 */
    char sStreamType[4][MAX_STRING_CNT];                   /* 码流类型支持的枚举 */
	char sPicFunc[MAX_PICFUNC_CNT][MAX_STRING_CNT];			/* 当前码流支持的图像功能枚举*/
	int  nMaxFramerate;                                    /* 码流支持的最大帧率 */
    int  nMaxBitrate;                                      /* 码流支持的最大码率 */
	int  nSupportVideoCoderCount;                          /* 当前码流支持的高级编码参数个数 */
	int  nSupportResolutionCount;                          /* 当前码流支持的分辨率个数 */
	int  nSupportBitrateTypeCount;                         /* 当前码流支持的码率类型个数 */
	int  nSupportStreamTypeCount;                          /* 当前码流支持流种类(视频流或者复合流)个数 */
	int  nSupportPicFuncCount;							 /*  当前码流支持的图片功能个数*/
} stuStreamInfo, *pStreamInfo;

/* 设备信息 */
typedef struct tagDeviceInfo
{
    stuStreamInfo stream_info[MAX_STREAM_NUM]; /* 16个通道码流的编码信息 */ 
	int nSupportStreamCount;                   /* 当前设备支持的码流个数 */
}stuDeviceInfo, *pDeviceInfo;

/* 设备能力单位映射表 */
typedef struct tagDeviceAbility
{
    char sDeviceString[MAX_STRING_CNT];
    int  eDeviceEnum;
}stuDeviceAbilityMap, *pDeviceAbilityMap;

/* 设备能力映射表 */
typedef struct tagDeviceAbilityMapArray
{
    stuDeviceAbilityMap video_code_map[eVideoCodeType_num];
    stuDeviceAbilityMap resolution_map[eResolutionType_num];
    stuDeviceAbilityMap bitrate_type_map[eBitrateType_num];
    stuDeviceAbilityMap stream_type_map[eStreamType_num];
	stuDeviceAbilityMap pic_func_map[ePicFunc_num];
}stuDeviceAbilityMapArray, *pDeviceAbilityMapArray;

/* 操作的设备类型 */
typedef enum 
{
    eOperateType_VIDEO_CODE   = 0,//编码类型
    eOperateType_RESOLUTION   = 1,//分辨率
    eOperateType_BITRATE_TYPE = 2,//码率类型
    eOperateType_STREAM_TYPE  = 3,//流类型
    eOperateType_PicFunc		 = 4,//图像功能
    eOperateType_num           = 5,
}eOperateType;

static stuDeviceInfo gDeviceInfo;

static int gbXmlModelInitOk = 0;

static stuDeviceAbilityMapArray gDeviceAbilityArrayMap = {
    .video_code_map = {
        {"H264",                0},
        {"H265",                1},
        {"MJPEG",               2},
		{"H265+",				3},
		{"H264+",				4},
    },
    .resolution_map = {
        {"352X288",             0},
        {"704X576",             1},
        {"1280X960",            3},
        {"1280X1024",           4},
        {"1600X1200",           5},
        {"1280X720",            6},
        {"1920X1080",           7},
        {"640X480",             8},
        {"2048X1536",           9},
        {"640X360",             10},
        {"320X240",             11},
        {"2592X1520",           12},
        {"2592X1944",           13},
        {"320X176",             14},
        {"480X256",             15},
        {"1408X792",            16},
        {"1600X900",            17},
		{"640X512", 			100},
        {"800X448",             100},
        {"2304X1296",           100},
        {"2560X1920",           100},
        {"2880X1624",           100},
        {"2880X1620",           100},
        {"2880X1616",           100},
        {"2560X1440",           100},
        {"3840X2160",			100},
        {"704X288",				100},
        {"3200X1800",			100},
    },
    .bitrate_type_map = {
        {"VBR",                 0},
        {"CBR",                 1},
        {"SmartAVC",            2},
    },
    .stream_type_map = {
        {"Video",               0},
        {"VideoAudio",          1},
    },
    .pic_func_map = {

		{"SupportDeFrog",		0},
		{"SupportShutterMode",	1},
		{"SupportLDC",			2},
	},
};

static int DeviceAbilityMapFunc(eOperateType ops_type, const char *device_string)
{
    if (ops_type<eOperateType_VIDEO_CODE || ops_type>eOperateType_num || !device_string)
    {
        ERR_LOG("Param error!\n");
        return -1;
    }

    int iItemIndex = 0;

    switch(ops_type)
    {
        case  eOperateType_VIDEO_CODE:   
        {
            for (iItemIndex=0; iItemIndex<eVideoCodeType_num; iItemIndex++)
            {
                if (0 == strcmp(device_string, gDeviceAbilityArrayMap.video_code_map[iItemIndex].sDeviceString))
                {
                    return (int)gDeviceAbilityArrayMap.video_code_map[iItemIndex].eDeviceEnum;
                }
            }
            break;
        }
        case  eOperateType_RESOLUTION:
        {
            for (iItemIndex=0; iItemIndex<eResolutionType_num; iItemIndex++)
            {
                if (0 == strcmp(device_string, gDeviceAbilityArrayMap.resolution_map[iItemIndex].sDeviceString))
                {
                    return (int)gDeviceAbilityArrayMap.resolution_map[iItemIndex].eDeviceEnum;
                }
            }
            break;
        }
        case  eOperateType_BITRATE_TYPE:
        {
            for (iItemIndex=0; iItemIndex<eBitrateType_num; iItemIndex++)
            {
                if (0 == strcmp(device_string, gDeviceAbilityArrayMap.bitrate_type_map[iItemIndex].sDeviceString))
                {
                    return (int)gDeviceAbilityArrayMap.bitrate_type_map[iItemIndex].eDeviceEnum;
                }
            }
            break;
        }
        case  eOperateType_STREAM_TYPE:
        {
            for (iItemIndex=0; iItemIndex<eStreamType_num; iItemIndex++)
            {
                if (0 == strcmp(device_string, gDeviceAbilityArrayMap.stream_type_map[iItemIndex].sDeviceString))
                {
                    return (int)gDeviceAbilityArrayMap.stream_type_map[iItemIndex].eDeviceEnum;
                }
            }
            break;
        }
		case eOperateType_PicFunc:
		{
			for (iItemIndex=0; iItemIndex<ePicFunc_num; iItemIndex++)
            {
                if (0 == strcmp(device_string, gDeviceAbilityArrayMap.pic_func_map[iItemIndex].sDeviceString))
                {
                    return (int)gDeviceAbilityArrayMap.pic_func_map[iItemIndex].eDeviceEnum;
                }
            }
            break;

		}
        default: ERR_LOG("Not support %d ops\n", ops_type); return -1; 
    }

    ERR_LOG("ops_type=%d, device_string=%s error!\n", ops_type, device_string);
    
    return -1;
}

static int GetDeviceInfoFromXml(const char *xml_file, pDeviceInfo pInfo)
{
    if (NULL==xml_file || NULL==pInfo)
    {
        ERR_LOG("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    mxml_node_t *tree = NULL;
    mxml_node_t *node = NULL;
    mxml_node_t *StreamConfig = NULL;
    mxml_node_t *StreamID = NULL;
    mxml_node_t *tmp_node = NULL;
    mxml_node_t *Type = NULL;

    int  i=0,j=0,k=0,t=0,l=0;
    int  stream_id = 0;

    FILE *fp = fopen(xml_file, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "open the %s error: %s!\n", xml_file, strerror(errno));
        return -1;
    }
    
    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    fclose(fp);
    if(tree == NULL)
    {
        ERR_LOG("Load file %s error!\n", xml_file);
        return -1;
    }
    
    node = mxmlFindElement(tree, tree, "Device", NULL, NULL, MXML_DESCEND);
    if(node == NULL)
    {
        ERR_LOG("can not find element Device!\n");
        mxmlDelete(tree);
        return -1;
    }
    
    node = mxmlFindElement(node, tree, "StreamList", NULL, NULL, MXML_DESCEND);
    if(node == NULL)
    {
        ERR_LOG("can not find element StreamList!\n");
        mxmlDelete(tree);
        return -1;
    }
 
    for(StreamConfig = mxmlFindElement(node, tree, "StreamConfig", NULL, NULL, MXML_DESCEND);
        StreamConfig != NULL;
        StreamConfig = mxmlFindElement(StreamConfig, tree, "StreamConfig", NULL, NULL, MXML_DESCEND))
    {
        StreamID = mxmlFindElement(StreamConfig, tree, "StreamID", NULL, NULL, MXML_DESCEND);
        stream_id = atoi(mxmlGetText(StreamID, NULL));//字符串 转 整数
//      printf("StreamID:%d\n", stream_id);

        /* 高级编码类型 */        
        i = 0;
        tmp_node = mxmlFindElement(StreamConfig, tree, "VideoCodecType", NULL, NULL, MXML_DESCEND);
        Type = mxmlFindElement(tmp_node, tree, "Type", NULL, NULL, MXML_DESCEND);
        if (NULL != mxmlGetText(Type, NULL))
        {
            memset(pInfo->stream_info[stream_id].sVideoCoder[i], 0, MAX_STRING_CNT);
            strncpy(pInfo->stream_info[stream_id].sVideoCoder[i], mxmlGetText(Type, NULL),sizeof(pInfo->stream_info[stream_id].sVideoCoder[i]) - 1);            
//          printf("VideoCodecType:%s\n", mxmlGetText(Type, NULL));
            /* 循环查找同名节点 */
            while(NULL!=(Type=mxmlGetNextSibling(Type)))
            {
                if (mxmlGetType(Type)==0)
                {
                    i ++;
                    memset(pInfo->stream_info[stream_id].sVideoCoder[i], 0, MAX_STRING_CNT);
                    strncpy(pInfo->stream_info[stream_id].sVideoCoder[i], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sVideoCoder[i]) - 1);
//                  printf("VideoCodecType:%s\n", pInfo->stream_info[stream_id].sVideoCoder[i]);
                }
            }
        }
		pInfo->stream_info[stream_id].nSupportVideoCoderCount = i + 1;

        /* 分辨率类型 */        
        j = 0;
        tmp_node = mxmlFindElement(StreamConfig, tree, "ResolutionType", NULL, NULL, MXML_DESCEND);
        Type = mxmlFindElement(tmp_node, tree, "Type", NULL, NULL, MXML_DESCEND);
        if (NULL != mxmlGetText(Type, NULL))
        {
            memset(pInfo->stream_info[stream_id].sResolution[j], 0, MAX_STRING_CNT);
            strncpy(pInfo->stream_info[stream_id].sResolution[j], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sResolution[j]) - 1);
//          printf("ResolutionType:%s\n", mxmlGetText(Type, NULL));
            /* 循环查找同名节点 */
            while(NULL!=(Type=mxmlGetNextSibling(Type)))
            {
                if (mxmlGetType(Type)==0)
                {
                    j ++;
                    memset(pInfo->stream_info[stream_id].sResolution[j], 0, MAX_STRING_CNT);
                    strncpy(pInfo->stream_info[stream_id].sResolution[j], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sResolution[j]) - 1);
//                  printf("ResolutionType:%s\n", pInfo->stream_info[stream_id].sResolution[j]);                
                }
            }
        }
		pInfo->stream_info[stream_id].nSupportResolutionCount = j + 1;

        /* 码率类型 */
        k = 0;
        tmp_node = mxmlFindElement(StreamConfig, tree, "BitrateType", NULL, NULL, MXML_DESCEND);
        Type = mxmlFindElement(tmp_node, tree, "Type", NULL, NULL, MXML_DESCEND);
        if (NULL != mxmlGetText(Type, NULL))
        {
            memset(pInfo->stream_info[stream_id].sBitrateType[k], 0, MAX_STRING_CNT);
            strncpy(pInfo->stream_info[stream_id].sBitrateType[k], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sBitrateType[k]) - 1);
//          printf("BitrateType:%s\n", mxmlGetText(Type, NULL));
            /* 循环查找同名节点 */
            while(NULL!=(Type=mxmlGetNextSibling(Type)))
            {
                if (mxmlGetType(Type)==0)
                {
                    k ++;
                    memset(pInfo->stream_info[stream_id].sBitrateType[k], 0, MAX_STRING_CNT);
                    strncpy(pInfo->stream_info[stream_id].sBitrateType[k], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sBitrateType[k]) - 1);
//                  printf("BitrateType:%s\n", pInfo->stream_info[stream_id].sBitrateType[k]);
                }
            }
        }
		pInfo->stream_info[stream_id].nSupportBitrateTypeCount= k + 1;

        /* 流类型 */
        t = 0;
        tmp_node = mxmlFindElement(StreamConfig, tree, "StreamType", NULL, NULL, MXML_DESCEND);
        Type = mxmlFindElement(tmp_node, tree, "Type", NULL, NULL, MXML_DESCEND);
        if (NULL != mxmlGetText(Type, NULL))
        {
            memset(pInfo->stream_info[stream_id].sStreamType[t], 0, MAX_STRING_CNT);
            strncpy(pInfo->stream_info[stream_id].sStreamType[t], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sStreamType[t]) - 1);
//          printf("StreamType:%s\n", mxmlGetText(Type, NULL));
            /* 循环查找同名节点 */
            while(NULL!=(Type=mxmlGetNextSibling(Type)))
            {
                if (mxmlGetType(Type)==0)
                {
                    t ++;
                    memset(pInfo->stream_info[stream_id].sStreamType[t], 0, MAX_STRING_CNT);
                    strncpy(pInfo->stream_info[stream_id].sStreamType[t], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sStreamType[t]) - 1);
//                  printf("StreamType:%s\n", pInfo->stream_info[stream_id].sBitrateType[k]);
                }
            }
        }
        pInfo->stream_info[stream_id].nSupportStreamTypeCount = t + 1;

		/*图像功能*/
		printf("\n********load pic func.\n\n");
		l=0;
		tmp_node = mxmlFindElement(StreamConfig, tree, "PicType", NULL, NULL, MXML_DESCEND);
		Type = mxmlFindElement(tmp_node, tree, "Type", NULL, NULL, MXML_DESCEND);
		if (NULL != mxmlGetText(Type, NULL))
        {
            memset(pInfo->stream_info[stream_id].sPicFunc[l], 0, MAX_STRING_CNT);
            strncpy(pInfo->stream_info[stream_id].sPicFunc[l], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sPicFunc[l]) - 1);
          	printf("pInfo->stream_info[%d].sPicFunc[%d]:%s\n",
				stream_id,l, pInfo->stream_info[stream_id].sPicFunc[l]);
            /* 循环查找同名节点 */
            while(NULL!=(Type=mxmlGetNextSibling(Type)))
            {
                if (mxmlGetType(Type)==0)
                {
                    l ++;
                    memset(pInfo->stream_info[stream_id].sPicFunc[l], 0, MAX_STRING_CNT);
                    strncpy(pInfo->stream_info[stream_id].sPicFunc[l], mxmlGetText(Type, NULL), sizeof(pInfo->stream_info[stream_id].sPicFunc[l]) - 1);
                    printf("pInfo->stream_info[%d].sPicFunc[%d]:%s\n",
				stream_id,l, pInfo->stream_info[stream_id].sPicFunc[l]);
                }
            }
        }
		pInfo->stream_info[stream_id].nSupportPicFuncCount = l +1;
		printf("\n");

        /* 最大帧率 */
        tmp_node = mxmlFindElement(StreamConfig, tree, "MaxFrameRate", NULL, NULL, MXML_DESCEND);
        if (NULL != mxmlGetText(tmp_node, NULL))
        {
            pInfo->stream_info[stream_id].nMaxFramerate = atoi(mxmlGetText(tmp_node, NULL));
//          printf("MaxFrameRate:%d\n", pInfo->stream_info[stream_id].nMaxFramerate);
        }

        /* 最大码率 */
        tmp_node = mxmlFindElement(StreamConfig, tree, "MaxBitrate", NULL, NULL, MXML_DESCEND);
        if (NULL != mxmlGetText(tmp_node, NULL))
        {
            pInfo->stream_info[stream_id].nMaxBitrate = atoi(mxmlGetText(tmp_node, NULL));
//          printf("MaxBitrate:%d\n", pInfo->stream_info[stream_id].nMaxBitrate);
        }
    }

    mxmlDelete(tree);

    pInfo->nSupportStreamCount = stream_id+1;

    return 0;
}

static int XmlModelInit(void)
{
    int nRet = 0;
    int nBoardId = 0;
    char xml_cfg_name[128] = {0};
    
    gbXmlModelInitOk = 0;

    memset(&gDeviceInfo, 0, sizeof(gDeviceInfo));

    //1 1.根据版型获取对应配置文件
    ZWSys_GetBoardInfo(&nBoardId);
//    nBoardId = MP4Sys_GetBoardInfo();
    snprintf(xml_cfg_name,sizeof(xml_cfg_name), "%s/IPC_%#X_CFG.xml", XML_CFG_PATH, nBoardId);

    printf("xml_cfg_name is %s\n", xml_cfg_name);

    //1 2.解析配置文件并将信息存放到全局结构体里面
    nRet = GetDeviceInfoFromXml(xml_cfg_name, &gDeviceInfo);
    if (nRet)
    {
        ERR_LOG("Fail to call GetDeviceInfoFromXml!\n");
        return -1;
    }

    gbXmlModelInitOk = 1;

    return 0;    
}

/*@ preif 检查该流对应的设备是否支持该能力
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ param [in] nOperateType 要操作的对象 [0:高级编码参数 1:分辨率 2:码率类型 3:码流类型 4:图像功能]
 *@ param [in] nCompareParam 要比较的参数对象
 *@ return -1 不支持或者参数错误
 *@        >0 对应的参数对象
 */
int IsSupportDeviceAbility(int nStreamId, int nOperateType, int nCompareParam, char *pResolution)
{
    int nRet = 0;
    if (0 == gbXmlModelInitOk)
    {
        if (XmlModelInit())
        {
            ERR_LOG("Xml model init fail!\n");
            return -1;
        }
        gbXmlModelInitOk = 1;
    }
    
    if (nOperateType<(int)eOperateType_VIDEO_CODE || nOperateType>(int)eOperateType_num || nStreamId<0 || nStreamId>gDeviceInfo.nSupportStreamCount-1)
    {
        ERR_LOG("Param error!\n");
        return -1;
    }

    int i = 0;

    switch(nOperateType)
    {
        case 0://高级编码参数
        {
            for (i=0; i<gDeviceInfo.stream_info[nStreamId].nSupportVideoCoderCount; i++)
            {
                if (nCompareParam == DeviceAbilityMapFunc(eOperateType_VIDEO_CODE, gDeviceInfo.stream_info[nStreamId].sVideoCoder[i]))
                {
                    return nCompareParam;
                }
            }
            break;
        }
        
        case 1://分辨率
        {
            for (i=0; i<gDeviceInfo.stream_info[nStreamId].nSupportResolutionCount; i++)
            {
                nRet = DeviceAbilityMapFunc(eOperateType_RESOLUTION, gDeviceInfo.stream_info[nStreamId].sResolution[i]);
                if (nRet >= 0 && pResolution != NULL)
                {                    
                    fprintf(stderr, "[%s][%d] ********zxr sResolution=[%s] pResolution=[%s]*****\r\n",__FUNCTION__,__LINE__, gDeviceInfo.stream_info[nStreamId].sResolution[i], pResolution);
                    if (strcasecmp(pResolution, gDeviceInfo.stream_info[nStreamId].sResolution[i]) == 0)
                    {
                        fprintf(stderr, "[%s][%d] ********now matched resolution, value=[%s]*****\r\n",__FUNCTION__,__LINE__, pResolution);
                        return 100;
                    }
                }

                fprintf(stderr, "[%s][%d] ********can not support this solution, value=[%s]*****\r\n",__FUNCTION__,__LINE__, pResolution);  
            }
            break;
        }
        
        case 2:
        {
            for (i=0; i<gDeviceInfo.stream_info[nStreamId].nSupportBitrateTypeCount; i++)
            {
                if (nCompareParam == DeviceAbilityMapFunc(eOperateType_BITRATE_TYPE, gDeviceInfo.stream_info[nStreamId].sBitrateType[i]))
                {
                    return nCompareParam;
                }
            }
            break;
        }
        
        case 3:
        {
            for (i=0; i<gDeviceInfo.stream_info[nStreamId].nSupportStreamTypeCount; i++)
            {
                if (nCompareParam == DeviceAbilityMapFunc(eOperateType_STREAM_TYPE, gDeviceInfo.stream_info[nStreamId].sStreamType[i]))
                {
                    return nCompareParam;
                }
            }
            break;
        }

		case 4:
		{
			for (i=0; i<gDeviceInfo.stream_info[nStreamId].nSupportPicFuncCount; i++)
            {
                if (nCompareParam == DeviceAbilityMapFunc(eOperateType_PicFunc, gDeviceInfo.stream_info[nStreamId].sPicFunc[i]))
                {
                    return nCompareParam;
                }
            }
            break;
		}
        default: ERR_LOG("Not support %d ops\n", nOperateType); return -1; 
    }

    ERR_LOG("nStreamId=%d, nOperateType=%d, nCompareParam=%d not support!\n", nStreamId, nOperateType, nCompareParam);

    return -1;
}

/*@ preif 获取设备默认支持的能力
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ param [in] nOperateType 要操作的对象 [0:高级编码参数 1:分辨率 2:码率类型 3:码流类型]
 *@ param [out] pDefaultAbility 默认支持能力级
 *@ return -1 不支持或者参数错误
 *@        =0 成功
 */
int GetDeviceDefaultAbility(int nStreamId, int nOperateType, int *pDefaultAbility, int *nWidth, int *nHeight)
{
    if (0 == gbXmlModelInitOk)
    {
        if (XmlModelInit())
        {
            ERR_LOG("Xml model init fail!\n");
            return -1;
        }
        gbXmlModelInitOk = 1;
    }
    
    if (nOperateType<(int)eOperateType_VIDEO_CODE || nOperateType>(int)eOperateType_num || nStreamId<0 || nStreamId>gDeviceInfo.nSupportStreamCount-1 || !pDefaultAbility)
    {
        ERR_LOG("Param error!\n");
        return -1;
    }

    switch(nOperateType)
    {
        case 0:
        {
            *pDefaultAbility = DeviceAbilityMapFunc(eOperateType_VIDEO_CODE, gDeviceInfo.stream_info[nStreamId].sVideoCoder[0]);
            break;
        }
        
        case 1:
        {
            *pDefaultAbility = DeviceAbilityMapFunc(eOperateType_RESOLUTION, gDeviceInfo.stream_info[nStreamId].sResolution[0]);
            if (nWidth != NULL && nHeight != NULL)
            {
                //这里需要解析 gDeviceInfo.stream_info[nStreamId].sResolution[0]分别对应的长和宽，目前为了简化，先直接指定最小的分辨率
                *nWidth = 352;
                *nHeight = 288;
            }
            break;
        }
        
        case 2:
        {
            *pDefaultAbility = DeviceAbilityMapFunc(eOperateType_BITRATE_TYPE, gDeviceInfo.stream_info[nStreamId].sBitrateType[0]);
            break;
        }
        
        case 3:
        {
            *pDefaultAbility = DeviceAbilityMapFunc(eOperateType_STREAM_TYPE, gDeviceInfo.stream_info[nStreamId].sStreamType[0]);
            break;
        }
        
        default: ERR_LOG("Not support %d ops\n", nOperateType); return -1; 
    }

    if (-1 == *pDefaultAbility)
    {
        ERR_LOG("Fail to called DeviceAbilityMapFunc(stream:%d,nOperateType:%d)\n", nStreamId, nOperateType);
        return -1;
    }

    return 0;
}

/*@ preif 获取该流对应的设备支持的最大帧率
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ return -1 错误
 *@        >0 支持的最大帧率
 */
int GetSupportMaxFramerate(int nStreamId)
{
    if (0 == gbXmlModelInitOk)
    {
        if (XmlModelInit())
        {
            ERR_LOG("Xml model init fail!\n");
            return -1;
        }
        gbXmlModelInitOk = 1;
    }

    if (nStreamId<0 || nStreamId>gDeviceInfo.nSupportStreamCount-1)
    {
        ERR_LOG("Param error!\n");
        return -1;
    }

    return gDeviceInfo.stream_info[nStreamId].nMaxFramerate;
}

/*@ preif 获取该流对应的设备支持的最大码率
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ return -1 错误
 *@        >0 支持的最大码率
 */
int GetSupportMaxBitrate(int nStreamId)
{
    if (0 == gbXmlModelInitOk)
    {
        if (XmlModelInit())
        {
            ERR_LOG("Xml model init fail!\n");
            return -1;
        }
        gbXmlModelInitOk = 1;
    }

    if (nStreamId<0 || nStreamId>gDeviceInfo.nSupportStreamCount-1)
    {
        ERR_LOG("Param error!\n");
        return -1;
    }
    
    return gDeviceInfo.stream_info[nStreamId].nMaxBitrate;
}

#ifdef TEST

int main(int argc, const char *argv[])
{
    int i, j, k, t, m,n;
    int nDeviceAbilityEnum = 0;

    if (XmlModelInit())
    {
        ERR_LOG("Fail to called XmlModelInit\n");
        return 0;
    }
    
    for (i=0; i<gDeviceInfo.nSupportStreamCount; i++)
    {
        for (j=0; j<gDeviceInfo.stream_info[i].nSupportVideoCoderCount; j++)
        {
            printf("gDeviceInfo.stream_info[%d].sVideoCoder[%d]=%s\n", i, j, gDeviceInfo.stream_info[i].sVideoCoder[j]);
            nDeviceAbilityEnum = DeviceAbilityMapFunc(eOperateType_VIDEO_CODE, gDeviceInfo.stream_info[i].sVideoCoder[j]);
            printf("VideoCode, i=%d, j=%d, nDeviceAbilityEnum=%d\n", i, j, nDeviceAbilityEnum);
        }
        
        for (k=0; k<gDeviceInfo.stream_info[i].nSupportResolutionCount; k++)
        {
            printf("gDeviceInfo.stream_info[%d].sResolution[%d]=%s\n", i, k, gDeviceInfo.stream_info[i].sResolution[k]);
            nDeviceAbilityEnum = DeviceAbilityMapFunc(eOperateType_RESOLUTION, gDeviceInfo.stream_info[i].sResolution[k]);
            printf("Resolution, i=%d, k=%d, nDeviceAbilityEnum=%d\n", i, k, nDeviceAbilityEnum);
        }

        for (t=0; t<gDeviceInfo.stream_info[i].nSupportBitrateTypeCount; t++)
        {
            printf("gDeviceInfo.stream_info[%d].sBitrateType[%d]=%s\n", i, t, gDeviceInfo.stream_info[i].sBitrateType[t]);
            nDeviceAbilityEnum = DeviceAbilityMapFunc(eOperateType_BITRATE_TYPE, gDeviceInfo.stream_info[i].sBitrateType[t]);
            printf("BitrateType, i=%d, t=%d, nDeviceAbilityEnum=%d\n", i, t, nDeviceAbilityEnum);
        }
        for (n=0; n<gDeviceInfo.stream_info[i].nSupportStreamTypeCount; n++)
        {
            printf("gDeviceInfo.stream_info[%d].sBitrateType[%d]=%s\n", i, n, gDeviceInfo.stream_info[i].sStreamType[n]);
            nDeviceAbilityEnum = DeviceAbilityMapFunc(eOperateType_STREAM_TYPE, gDeviceInfo.stream_info[i].sStreamType[n]);
            printf("BitrateType, i=%d, n=%d, nDeviceAbilityEnum=%d\n", i, n, nDeviceAbilityEnum);
        }
		for(m=0;m<gDeviceInfo.stream_info[i].nSupportPicFuncCount;m++)
		{
			printf("gDeviceInfo.stream_info[%d].sPicFunc[%d]=%s\n", i, n, gDeviceInfo.stream_info[i].sPicFunc[m]);
            nDeviceAbilityEnum = DeviceAbilityMapFunc(eOperateType_PicFunc, gDeviceInfo.stream_info[i].sPicFunc[m]);
            printf("PicFunc, i=%d, m=%d, nDeviceAbilityEnum=%d\n", i, m, nDeviceAbilityEnum);
		}
		
        
		printf("nMaxBitrate=%d; nMaxFramerate=%d\n", gDeviceInfo.stream_info[i].nMaxBitrate, gDeviceInfo.stream_info[i].nMaxFramerate);

        printf("\n");
    }
    
    /* 测试第一码流情况 */
    if (-1 == IsSupportDeviceAbility(0, 0, 0)) // 1
        ERR_LOG("[stream:0; OPS:0; CMP:0] Not Support!\n");
    else
        printf("[stream:0; OPS:0; CMP:0] Ok! Support!\n");
        
    if (-1 == IsSupportDeviceAbility(0, 0, 2)) // 0
        ERR_LOG("[stream:0; OPS:0; CMP:2] Not Support!\n");
    else
        printf("[stream:0; OPS:0; CMP:2] Ok! Support!\n");

printf("----------------------Video Code Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(0, 1, 7)) // 1
        ERR_LOG("[stream:0; OPS:1; CMP:7] Not Support!\n");
    else
        printf("[stream:0; OPS:1; CMP:7] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(0, 1, 8)) // 0
        ERR_LOG("[stream:0; OPS:1; CMP:8] Not Support!\n");
    else
        printf("[stream:0; OPS:1; CMP:8] Ok! Support!\n");

printf("----------------------Resolution Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(0, 2, 2))
        ERR_LOG("[stream:0; OPS:2; CMP:2] Not Support!\n");
    else
        printf("[stream:0; OPS:2; CMP:2] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(0, 2, 1))
        ERR_LOG("[stream:0; OPS:2; CMP:1] Not Support!\n");
    else
        printf("[stream:0; OPS:2; CMP:1] Ok! Support!\n");
        
printf("----------------------BitrateType Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(0, 3, 0))
        ERR_LOG("[stream:0; OPS:3; CMP:0] Not Support!\n");
    else
        printf("[stream:0; OPS:3; CMP:0] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(0, 3, 1))
        ERR_LOG("[stream:0; OPS:3; CMP:1] Not Support!\n");
    else
        printf("[stream:0; OPS:3; CMP:1] Ok! Support!\n");

printf("----------------------StreamType Test Over----------------------\n");
	if (-1 == IsSupportDeviceAbility(0, 4, 0))
	{
		ERR_LOG("[stream:0; OPS:4; CMP:0] Not Support!\n");
	}
	else
	{
		printf("[stream:0; OPS:4; CMP:0] Ok! Support!\n");
	}

	if (-1 == IsSupportDeviceAbility(0, 4, 1))
	{
		ERR_LOG("[stream:0; OPS:4; CMP:0] Not Support!\n");
	}
	else
	{
		printf("[stream:0; OPS:4; CMP:1] Ok! Support!\n");
	}

	if (-1 == IsSupportDeviceAbility(0, 4, 2))
	{
		ERR_LOG("[stream:0; OPS:4; CMP:0] Not Support!\n");
	}
	else
	{
		printf("[stream:0; OPS:4; CMP:2] Ok! Support!\n");
	}
printf("----------------------PicFunc Test Over----------------------\n");
    printf("MaxFrameRate = %d\n", GetSupportMaxFramerate(0));
    printf("MaxBitrate = %d\n", GetSupportMaxBitrate(0));
            
printf("\n----------------------First Stream Test Over----------------------\n\n");



	/* 测试第二码流情况 */
    if (-1 == IsSupportDeviceAbility(1, 0, 0)) // 1
        ERR_LOG("[stream:1; OPS:0; CMP:0] Not Support!\n");
    else
        printf("[stream:1; OPS:0; CMP:0] Ok! Support!\n");
        
    if (-1 == IsSupportDeviceAbility(1, 0, 2)) // 0
        ERR_LOG("[stream:1; OPS:0; CMP:2] Not Support!\n");
    else
        printf("[stream:1; OPS:0; CMP:2] Ok! Support!\n");

printf("----------------------Video Code Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(1, 1, 10)) // 1
        ERR_LOG("[stream:1; OPS:1; CMP:10] Not Support!\n");
    else
        printf("[stream:1; OPS:1; CMP:10] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(1, 1, 12)) // 0
        ERR_LOG("[stream:1; OPS:1; CMP:12] Not Support!\n");
    else
        printf("[stream:1; OPS:1; CMP:12] Ok! Support!\n");

printf("----------------------Resolution Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(1, 2, 2))
        ERR_LOG("[stream:1; OPS:2; CMP:2] Not Support!\n");
    else
        printf("[stream:1; OPS:2; CMP:2] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(1, 2, 1))
        ERR_LOG("[stream:1; OPS:2; CMP:4] Not Support!\n");
    else
        printf("[stream:1; OPS:2; CMP:4] Ok! Support!\n");
        
printf("----------------------BitrateType Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(1, 3, 0))
        ERR_LOG("[stream:1; OPS:3; CMP:0] Not Support!\n");
    else
        printf("[stream:1; OPS:3; CMP:0] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(1, 3, 1))
        ERR_LOG("[stream:1; OPS:3; CMP:1] Not Support!\n");
    else
        printf("[stream:1; OPS:3; CMP:1] Ok! Support!\n");

    printf("MaxFrameRate = %d\n", GetSupportMaxFramerate(1));
    printf("MaxBitrate = %d\n", GetSupportMaxBitrate(1));


printf("----------------------StreamType Test Over----------------------\n");

    /* 测试第三码流情况 */
    if (-1 == IsSupportDeviceAbility(2, 0, 0)) // 1
        ERR_LOG("[stream:2; OPS:0; CMP:0] Not Support!\n");
    else
        printf("[stream:2; OPS:0; CMP:0] Ok! Support!\n");
        
    if (-1 == IsSupportDeviceAbility(2, 0, 2)) // 0
        ERR_LOG("[stream:2; OPS:0; CMP:2] Not Support!\n");
    else
        printf("[stream:2; OPS:0; CMP:2] Ok! Support!\n");

printf("----------------------Video Code Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(2, 1, 7)) // 1
        ERR_LOG("[stream:2; OPS:1; CMP:7] Not Support!\n");
    else
        printf("[stream:2; OPS:1; CMP:7] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(2, 1, 0)) // 0
        ERR_LOG("[stream:2; OPS:1; CMP:0] Not Support!\n");
    else
        printf("[stream:2; OPS:1; CMP:0] Ok! Support!\n");

printf("----------------------Resolution Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(2, 2, 2))
        ERR_LOG("[stream:2; OPS:1; CMP:7] Not Support!\n");
    else
        printf("[stream:2; OPS:1; CMP:7] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(2, 2, 1))
        ERR_LOG("[stream:2; OPS:1; CMP:4] Not Support!\n");
    else
        printf("[stream:2; OPS:1; CMP:4] Ok! Support!\n");
        
printf("----------------------BitrateType Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(2, 3, 0))
        ERR_LOG("[stream:2; OPS:3; CMP:7] Not Support!\n");
    else
        printf("[stream:2; OPS:3; CMP:7] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(2, 3, 1))
        ERR_LOG("[stream:2; OPS:3; CMP:4] Not Support!\n");
    else
        printf("[stream:2; OPS:3; CMP:4] Ok! Support!\n");

    printf("MaxFrameRate = %d\n", GetSupportMaxFramerate(2));
    printf("MaxBitrate = %d\n", GetSupportMaxBitrate(2));

printf("----------------------StreamType Test Over----------------------\n");

    /* 测试第四码流情况 */
    if (-1 == IsSupportDeviceAbility(3, 0, 0))
        ERR_LOG("[stream:3; OPS:0; CMP:0] Not Support!\n");
    else
        printf("[stream:3; OPS:0; CMP:0] Ok! Support!\n");
        
    if (-1 == IsSupportDeviceAbility(3, 0, 2))
        ERR_LOG("[stream:3; OPS:0; CMP:2] Not Support!\n");
    else
        printf("[stream:3; OPS:0; CMP:2] Ok! Support!\n");

printf("----------------------Video Code Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(3, 1, 7))
        ERR_LOG("[stream:3; OPS:1; CMP:7] Not Support!\n");
    else
        printf("[stream:3; OPS:1; CMP:7] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(3, 1, 0)) 
        ERR_LOG("[stream:3; OPS:1; CMP:0] Not Support!\n");
    else
        printf("[stream:3; OPS:1; CMP:0] Ok! Support!\n");

printf("----------------------Resolution Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(3, 2, 0))
        ERR_LOG("[stream:3; OPS:2; CMP:0] Not Support!\n");
    else
        printf("[stream:3; OPS:2; CMP:0] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(3, 2, 1))
        ERR_LOG("[stream:3; OPS:2; CMP:1] Not Support!\n");
    else
        printf("[stream:3; OPS:2; CMP:1] Ok! Support!\n");
        
printf("----------------------BitrateType Test Over----------------------\n");

    if (-1 == IsSupportDeviceAbility(3, 3, 0))
        ERR_LOG("[stream:3; OPS:3; CMP:0] Not Support!\n");
    else
        printf("[stream:3; OPS:3; CMP:0] Ok! Support!\n");

    if (-1 == IsSupportDeviceAbility(3, 3, 1))
        ERR_LOG("[stream:3; OPS:3; CMP:1] Not Support!\n");
    else
        printf("[stream:3; OPS:3; CMP:1] Ok! Support!\n");

    printf("MaxFrameRate = %d\n", GetSupportMaxFramerate(3));
    printf("MaxBitrate = %d\n", GetSupportMaxBitrate(3));

printf("----------------------StreamType Test Over----------------------\n");
    
    return 0;
}

#endif


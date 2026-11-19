#ifndef __DEVICE_INFO_H__
#define __DEVICE_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
typedef enum
{
    MainStreamId   = 0,
    AuxStreamId    = 1,
    HdAuxStreamId  = 2,
    FourthStreamId = 3,
}eStreamId;

typedef enum
{
    AdvVideoCodeOps  = 0,
    ResolutionOps    = 1,
    BitrateTypeOps   = 2,
    StreamTypeOps    = 3,
    PicFuncOps		 = 4,
}eOperateType;
#endif

/*@ preif 检查该流对应的设备是否支持该能力
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ param [in] nOperateType 要操作的对象 [0:高级编码参数 1:分辨率 2:码率类型 3:码流类型]
 *@ param [in] nCompareParam 要比较的参数对象
 *@ return -1 不支持或者参数错误
 *@        >0 对应的参数对象
 */
int IsSupportDeviceAbility(int nStreamId, int nOperateType, int nCompareParam, char *pResolution);

/*@ preif 获取设备默认支持的能力
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ param [in] nOperateType 要操作的对象 [0:高级编码参数 1:分辨率 2:码率类型 3:码流类型]
 *@ param [out] pDefaultAbility 默认支持能力级
 *@ return -1 参数错误
 *@        =0 成功
 */
int GetDeviceDefaultAbility(int nStreamId, int nOperateType, int *pDefaultAbility, int *nWidth, int *nHeight);

/*@ preif 获取该流对应的设备支持的最大帧率
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ return -1 错误
 *@        >0 支持的最大帧率
 */
int GetSupportMaxFramerate(int nStreamId);

/*@ preif 获取该流对应的设备支持的最大码率
 *@ param [in] nStreamId 流ID号 [0:主码流 1:子码流 2:第二子码流 3:第三子码流， ... 依次类推]
 *@ return -1 错误
 *@        >0 支持的最大码率
 */
int GetSupportMaxBitrate(int nStreamId);


#ifdef __cplusplus
}
#endif

#endif // end of __DEVICE_INFO_H__


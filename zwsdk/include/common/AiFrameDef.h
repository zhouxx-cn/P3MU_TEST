#ifndef  _AI_FRAME_DEFINE_H_
#define  _AI_FRAME_DEFINE_H_
/*******************************************
 武汉卓威视讯
fjz
20190221
 本文件用于定义智能事件相关
 帧头结构、宏定义等，
 供服务端及客户端共同使用
*******************************************/
#include <stdint.h>

typedef unsigned short U16;
typedef unsigned int U32;
 
#define MAX_HUMAN_SHAPE_RESULT_NUM 16
#define MAX_FIRE_RESULT_NUM 16
//特征值最大长度定义
#define  MAX_FEATURE_LEN (320)

typedef struct stRectPeople
{
	U16 x;
	U16 y;
	U16 w;
	U16 h;
} RECT_PEOPLE;

typedef struct _mpp_people_frame_head
{
	U32 u32AIType;      // 固定为0x1
	U32 u32Size;        // 人形数据长度，头部长度+人形图片长度
	U32 u32Time_sec;    // 绝对时间戳，秒数部分
	U32 u32Time_usec;   // 绝对时间戳，毫秒数部分
	U32 u32Num;			// 人形数目
    U32 u32BgPicOffset; //背景图片相对于帧起始位置的偏移,当u32BgPicSize=0此值无意义
    U32 u32BgPicSize;   //背景图片大小，0表示此帧数据不包含背景图
    RECT_PEOPLE rcPeople[MAX_HUMAN_SHAPE_RESULT_NUM];     //人形位置，x,y,w,h 均为相对值,范围为0-10000,0表示左/上,10000表示右/下
    //人形图片数据紧跟其后
} __attribute__((packed)) PEOPLE_FRAME_HEAD;
	
typedef struct tagBackgroundImg
{
	uint32_t uImageLen; 			// 背景图jpg长度 0则没有
	char     *pImageData; 			// 背景图jpg数据
    uint32_t uImageWidth;          	// 背景图 宽
    uint32_t uImageHeight;          // 背景图 高
} BackgroundImg;

typedef struct _mpp_ElectronicFence_frame_head
{
	unsigned int		uDataPacketStartCode; 	//0xB7010000
	unsigned char		byFrameType;			//帧类型
    unsigned long		u32RealTimeSec;			//时间戳-秒
	int 				needAlarm;				//是否需要报警
	BackgroundImg		stBackgroundImg;		//背景图
} __attribute__((packed)) ELECTRONIC_FENCE_FRAME_HEAD;

typedef struct _mpp_CrossBorderDetect_frame_head
{
	unsigned int		uDataPacketStartCode; 	//0xB7010000
	unsigned char		byFrameType;			//帧类型
    unsigned long		u32RealTimeSec;			//时间戳-秒
	int 				needAlarm;				//是否需要报警
	BackgroundImg		stBackgroundImg;		//背景图
} __attribute__((packed)) CROSS_BORDER_DETECT_FRAME_HEAD;

typedef struct _mpp_OffDutyDetect_frame_head
{
	unsigned int		uDataPacketStartCode; 	//0xB7010000
    unsigned char		byFrameType;			//帧类型
    unsigned long		u32RealTimeSec;			//时间戳-秒
	int 				needAlarm;				//是否需要报警
	unsigned short      frequency;      		//离岗次数
    unsigned short      totalTimes;    		 	//总共离岗时间
	BackgroundImg		stBackgroundImg;		//背景图
} __attribute__((packed)) OFF_DUTY_DETECT_FRAME_HEAD;

typedef struct _mpp_PassengerFlow_frame_head
{
    unsigned int        uDataPacketStartCode; 	//0xB7010000
    unsigned char		byFrameType;			//帧类型
    unsigned long		u32RealTimeSec;			//时间戳-秒
    unsigned short      inFlow;         		//进入人数
    unsigned short      outFlow;        		//离开人数
	BackgroundImg		stBackgroundImg;		//背景图
} __attribute__((packed)) PASSENGER_FLOW_FRAME_HEAD;

typedef struct _mpp_PassengerFlowHuman_frame_head
{
	unsigned int		uDataPacketStartCode; 	//0xB7010000
    unsigned char		byFrameType;			//帧类型
    unsigned long		u32RealTimeSec;			//时间戳-秒
	int 				needAlarm;				//是否需要报警
	int 				inOutStatus;			//出入状态:1-进入; 2-离开
	BackgroundImg		stBackgroundImg;		//背景图
} __attribute__((packed)) PASSENGER_FLOW_HUMAN_FRAME_HEAD;

typedef struct _mpp_FireFun_frame_head
{
    unsigned int        uDataPacketStartCode; 	//0xB7010000
    unsigned char		byFrameType;			//帧类型
    unsigned long		u32RealTimeSec;			//时间戳-秒
    int                 needAlarm;      		//是否需要报警
    unsigned int        u32Num;         		//火焰数量
    RECT_PEOPLE         rcFire[MAX_FIRE_RESULT_NUM];
	BackgroundImg		stBackgroundImg;		//背景图
} __attribute__((packed)) FIRE_FUN_FRAME_HEAD;

typedef enum 
{
    EVENT_FACECAP,      //人脸抓拍
    EVENT_FACEDETECT,   //人脸检测
    EVENT_FACERECO,     //人脸比对
	EVENT_MASKDETECT,   //口罩检测
}E_EVENT_TYPE;

typedef struct tagRECT_ST
{
    uint32_t    left;
    uint32_t    top;
    uint32_t    right;
    uint32_t    bottom;
}ST_RECT;

//人脸抓拍事件帧 
typedef struct TagFaceCapEvent
{
	uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint64_t faceId;        // 人脸id
    ST_RECT   stRect;        // 人脸区域
    float    confidence;    // 评分 
    uint32_t feature_data_size;                 // 特征值数据长度   0则没有
    char     feature_data[MAX_FEATURE_LEN];     // 特征值数据
    int      face_pic_len;       // 人脸图jpg长度   0则没有
    char*    face_pic_data;      // 人脸图jpg数据 [内部alloc]
    int      bg_pic_len;         // 背景图jpg长度   0则没有
    char*    bg_pic_data;        // 背景图jpg数据 [内部alloc]
    uint32_t nRuleID;            // 规则ID
    char acOrganizationID[64];   // 组织id
    char acOrganizationName[64]; // 组织名称
}FaceCapEvent;

//人脸比对事件帧 
typedef struct TagFaceRecoEvent
{
	uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint64_t faceId;        // 人脸id
    ST_RECT   stRect;        // 人脸区域
    float    confidence;    // 评分 
    uint32_t feature_data_size;                 // 特征值数据长度   0则没有
    uint8_t  feature_data[MAX_FEATURE_LEN];     // 特征值数据
    int      face_pic_len;       // 人脸图jpg长度   0则没有
    char*    face_pic_data;      // 人脸图jpg数据 [内部alloc]
    int      bg_pic_len;         // 背景图jpg长度   0则没有
    char*    bg_pic_data;        // 背景图jpg数据 [内部alloc]
    // uint32_t nRuleID;            // 规则ID
    //比对结果及人脸数据库信息
    uint32_t uRecoNum;           //匹配数量， 0：没有  1：有  >1：有多个（无效值 目前不支持）
    uint64_t uRecoGroupId;       //匹配人脸组id
    uint64_t uRecoFaceId;        //匹配人脸id
    float    fRecoScores;        //匹配度  [0-1.0] 
    char     uuidGroup[64];/*人脸组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    char     uuidSubgroup[64];/*人脸子组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    char     uuidFace[64];/*人脸信息唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    char     people_name[32];/*人脸信息名称，32个字节的字符串*/
    // int gender;/*性别，0-男，1-女，2-未知*/
    // char phone_number[20];/*手机号*/
    // int certificates_type;/*证件类型，0-身份证，1-护照，2-其他*/
    // char certificates_id[20];/*证件号*/
    
    int      face_pic_w;         // 人脸图 宽
    int      face_pic_h;         // 人脸图 高
    char	 groupName[32];		//脸库名称
    char 	 subGroupName[64]; // 组织名称
    int      bg_pic_w;           // 背景图 宽
    int      bg_pic_h;           // 背景图 高
}FaceRecoEvent;

//口罩事件帧 
typedef struct TagMaskDetectEvent
{
	uint32_t nFrameId; 			// 帧ID
	uint32_t nSrcW; 			// 原始帧宽
	uint32_t nSrcH; 			// 原始帧高
	uint32_t u32TimeSec; 		// 发生时间s
	uint32_t u32TimeUSec; 		// 发生时间us
	uint64_t faceId; 			// 人脸id
	ST_RECT stRect; 			// 人脸区域
	uint32_t face_pic_len; 		// 人脸图jpg长度 0则没有
	char* face_pic_data; 		// 人脸图jpg数据 [内部alloc]
    uint32_t face_pic_w;        // 人脸图 宽
    uint32_t face_pic_h;        // 人脸图 高
	uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
	uint32_t maskStatus;		//口罩状态:0-无口罩；1-有口罩
}MaskDetectEvent;

//人脸抓拍（比对）事件帧
typedef struct tagFaceEventPacketHeader
{
	unsigned int        	uDataPacketStartCode; 	//0xB7010000
	unsigned char       	byFrameType;			// 帧类型
	E_EVENT_TYPE      		eEventType;				// 事件类型
    unsigned int        	uFrameNo;

	union FaceEvent
	{
		FaceCapEvent		stFaceCap;				// 人脸抓拍事件结构体
		FaceRecoEvent		stFaceReco;				// 人脸比对事件结构体
		MaskDetectEvent		stMaskDetect;			// 口罩事件结构体
	}stFaceEvent;
}__attribute__((packed)) FaceEventPacketHeader, *PFaceEventPacketHeader;
#define	FACE_EVENT_PACKET_LEN	sizeof(FaceEventPacketHeader)

#endif  //_AI_FRAME_DEF_H_


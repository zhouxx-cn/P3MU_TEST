
//片段类型
typedef enum eFragmentType
{
	//分析结果
	eFragmentPeopleDetect		= 0,	//人体检测
	eFragmentFaceDetect			= 1,	//人脸检测
	eFragmentFaceRecognition 	= 2,	//人脸识别
	eFragmentMotionTrack 		= 3,	//移动轨迹
	eFragmentCarDetect 			= 4,	//移动轨迹
	eFragmentFireDetect 	    = 5,	//火焰侦测
	//规则配置
	eFragmentDetectRegion 		= 10,	//检测区域
	eFragmentTraversePlane		= 11,	//绊线位置
} eFragmentType;

//叠加类型
typedef enum eOverlayType{
	eOverlayRect				= 0,	//四边形
	eOverlayLine				= 1,	//线段集
	eOverlayPolygon				= 2,	//多边形
	eOverlayText				= 3,	//文本叠加
	eOverlayTrack				= 4,	//轨迹
} eOverlayType;

//坐标
typedef struct tagOverlayPoint
{
	unsigned short	m_X;		//位置
	unsigned short	m_Y;
}OverlayPoint, *POverlayPoint;

//四边形
typedef struct tagOverlayRectangle
{
	unsigned long	m_Color;	//32位RGB值
	unsigned short	m_Left;		//坐标
	unsigned short	m_Right;
	unsigned short	m_Top;
	unsigned short	m_Bottom;
}OverlayRectangle, *POverlayRectangle;

//多边形
#ifdef _WIN32
#pragma pack(1)
typedef struct tagOverlayPolygon
#else
typedef struct tagOverlayPolygon
#endif
{
	unsigned long	m_Color;	//32位RGB值
	unsigned short	m_Number;	//边缘数量
	//OverlayPoint	m_Position[n];//位置
#ifdef _WIN32
}OverlayPolygon, *POverlayPolygon;
#pragma pack()
#else
}__attribute__((__packed__)) OverlayPolygon, *POverlayPolygon;
#endif

//线条
typedef struct tagOverlayLine
{
	unsigned long	m_Color;	//32位RGB值
	OverlayPoint	m_Begin;	//起始点
	OverlayPoint	m_End;		//结束点
}OverlayLine, *POverlayLine;

//文本
#ifdef _WIN32
#pragma pack(1)
typedef struct tagOverlayText
#else
typedef struct tagOverlayText
#endif
{
	unsigned long	m_Color;	//32位RGB值
	unsigned short	m_Length;	//文本长度
	OverlayPoint	m_Position;	//位置
//	char			m_String[m_Length];//文本内容
#ifdef _WIN32
}OverlayText, *POverlayText;
#pragma pack()
#else
}__attribute__((__packed__)) OverlayText, *POverlayText;
#endif

// 轨迹
typedef struct tagOverlayTrack
{
	unsigned long	m_Color;	//32位RGB值
	unsigned long	m_ID;		//目标ID
	OverlayPoint	m_Center;	//中心点
}OverlayTrack, *POverlayTrack;

//智能帧片段头V1
#ifdef _WIN32
#pragma pack(1)
typedef struct tagAIFragmentV1
#else
typedef struct tagAIFragmentV1
#endif
{
	unsigned short	m_Type;		//分片类型（见eFragmentType定义）
	unsigned char	m_Version;	//版本号（默认为1）
	unsigned char	m_Overlay;	//叠加方式（见eOverlayType定义）
	unsigned short	m_Length;	//负载长度
#ifdef _WIN32
}AIFragmentV1, *PAIFragmentV1;
#pragma pack()
#else
}__attribute__((__packed__)) AIFragmentV1, *PAIFragmentV1;
#endif
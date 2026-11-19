/*


*/
#ifndef __AISDK_API_H__
#define __AISDK_API_H__

#include "AISDK_struct.h"
#include "AISDK_ErrorCode.h"



#ifdef __cplusplus
extern "C" {
#endif    //@__cplusplus








/*************************
 * define *
*/
typedef int (*AISDK_Func_EventCallback)(int ch, E_AI_EVENT_TYPE eEventType, void* pData, void* userData);

typedef int (*AISDK_Func_ReleaseDetectYUV)(int ch, unsigned long uFrameId, void* userData);// @ch [暂无效，固定为0]




/*************************
 * API *
*/
//目前只支持AICFG_TYPE_STRUCT 对应 AISDK_DEF_INIT_PARAM 
AISDK_API int AISDK_Init(E_AISDK_CFG_TYPE eCfgType, const char* pCfgBuf, unsigned int iCfgBufLen);
AISDK_API int AISDK_Uninit(void);

AISDK_API int AISDK_AiPkgInstall(const char* pkgtar_pathname);
AISDK_API int AISDK_AiPkgUnstall(void);
AISDK_API int AISDK_AiPkgCheckInstall(const char* pInstallName);
/**
 * @brief 授权 - 获取指纹文件
 * 
 * @param pathname [in] 指定写入的pathname
 * @return AISDK_API int  <0 error ;  other is filesize
 */
AISDK_API int AISDK_AuthGetFingerFile(const char* pathname, const char* uuid, unsigned int uuid_len);
AISDK_API int AISDK_AuthImport(const char* pathname);
AISDK_API int AISDK_AuthRemove(const char* pathname);
AISDK_API int AISDK_AuthStatus(ST_AISDK_DEF_AUTH_INFO* pstAuthInfo);

/**
 * @brief 初始化
 * 
 * @param scene 功能场景   AI场景枚举 参考 E_AI_SCENE    按位使能，eg. AI_SCENE_FACERECO|AI_SCENE_HD|AI_SCENE_MD|AI_SCENE_FACEEVENT 
 * @param eCfgType 未使用
 * @param pCfgBuf 未使用
 * @return int 0：成功   -116 宠物授权无或到期导致失败   -117 宠物授权无或到期导致失败   -n 其他错误
 */
AISDK_API int AISDK_Start(int scene, E_AISDK_CFG_TYPE eCfgType, const char* pCfgBuf, unsigned int iCfgBufLen);

/**
 * @brief 反初始化
 * 
 * @return int 0:成功
 */
AISDK_API int AISDK_Stop(void);

// /**
//  * @brief 获取算法版本  算法厂商_算法类型_版本号x.y
//  *          大版本x，不同表示特征值不兼容； 小版本y，不同表示特征值兼容的升级。
//  * @param bufVer 填充buf，  输入缓存区地址；输出信息填充buf。
//  * @return int   0：成功 
//  */
// AISDK_API int AISDK_GetVerInfo(char* bufVer);

/**
 * @brief 功能配置
 * 
 * @param scene 需要配置的功能
 * @param iDataType 配置的数据类型  目前只支持 AICFG_TYPE_JSON 
 * @param cfgBuf 配置数据buf      参考：___AISDK_JSON_CFG_FACECAP___ 等，之下的说明
 * @param iCfgBufLen 暂未使用
 * @return int 0：成功 
 */
AISDK_API int AISDK_Cfg(int ch, unsigned int scene, E_AISDK_CFG_TYPE eDataType, const char* cfgBuf, unsigned int iCfgBufLen);

/**
 * @brief 发送图像（yuv格式）流到AI检测
 * 
 * @param ch 通道号
 * @param pYuvData yuv数据
 * @return int 成功返回0,  >0 使用了图像但是错误(101 算法过期(目前主要指烟火算法))   <0 未使用图像就错误返回 
 */
AISDK_API int AISDK_SendDetectYUV(int ch, YUV_FRAME_INFO_S *pYuvData);

/**
 * @brief 注册释放AI检测yuv图像的方法到内部，内部在合适的时机释放送检帧。
 * 
 * @param func 方法指针
 * @param userData 用户数据，回调时原样返还
 * @return int 0：成功 
 */
AISDK_API int AISDK_RegisterFuncReleaseDetectYUV(AISDK_Func_ReleaseDetectYUV func, void* userData);

/**
 * @brief 注册事件回调
 * 
 * @param func 输入，回调函数
 * @param userData 输入，用户数据，回调时原样返还
 * @return int 0：成功 
 */
AISDK_API int AISDK_RegisterEventCallback(AISDK_Func_EventCallback func, void* userData);

/**
 * @brief 注册功能回调
 * 
 * @param type 
 * @param func 
 * @param userData 
 * @return int 0：成功
 */
int AISDK_RegisterFuncCallback(AISDK_DEF_E_FUNC_CB_TYPE type, const AISDK_DEF_FUNC_CB* func, void* userData);




#ifdef  __SUP_FACE_DB_IF
// 脸库管理相关

/**
 * @brief 初始化人脸数据库
 *          如果需要使用的话需要AISDK_Start前初始化
 * @param pFileDBPath   eg. /storage/db 或 /storage/db/
 * @return int 0 成功
 */
AISDK_API int AISDK_FaceManager_FaceDBInit(const char* pFileDBPath);

/**
 * @brief 
 * 
 * @return E_AISDK_ERR_FACEDB 
 */
AISDK_API int AISDK_FaceManager_FaceDBUnInit(void);

// //初始化
// AISDK_API int AISDK_FaceManager_Init();
// //反初始化
// AISDK_API int AISDK_FaceManager_Uninit();

/**
 * -------------------鉴权在应用做 
 * -----------------
//比对功能绑定（脸库所有者）
AISDK_API int AISDK_FaceManager_AdminBind(char* pUuid);
//比对功能绑定者查询
AISDK_API int AISDK_FaceManager_AdminInfo(char* pUuid);
//比对功能解绑（脸库所有者）
AISDK_API int AISDK_FaceManager_AdminUnbind(void);
////////注：所有接口入参调用者，修改接口都验证和所有者一致才可操作； 不绑定的话验证时放行
*/

//功能：添加人脸组。
//参数：const AISDK_ST_FACERECO_GROUP *pDBFaceCroup 传入参数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceGroupAdd(const AISDK_ST_FACERECO_GROUP *pDBFaceGroup);

//删除脸库
//功能：删除人脸组。
//参数：const AISDK_ST_FACERECO_GROUP *pDBFaceCroup 传入参数，仅uuid有效
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceGroupDel(const AISDK_ST_FACERECO_GROUP *pDBFaceGroup);

//功能：删除所有人脸组。
//参数：const DBFaceGroup_t *pDBFaceCroup 传入参数，仅uuid有效
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceGroupDelALL();

//修改脸库信息
//功能：修改人脸组。
//参数：const AISDK_ST_FACERECO_GROUP *pDBFaceCroup 传入参数，根据uuid匹配；
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceGroupAlter(const AISDK_ST_FACERECO_GROUP *pDBFaceGroup);

//功能：查询所有人脸组个数。
//参数：int *pCount 传出参数，存放实际的有效个数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceGroupCountQuery(int *pCount);

//功能：查询所有人脸组。
//参数：DBFaceGroup_t *pDBFaceCroup 传出参数 int *pCount 传入参数：pDBFaceCroup的最大个数，传出参数：存入查询的实际个数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceGroupQueryALL(AISDK_ST_FACERECO_GROUP *pDBFaceGroup, int *pCount);

//功能：查询人脸组信息
//参数：DBFaceInfo_t *pDBFaceInfo 传入参数，根据uuid匹配，传出参数
//返回值：成功：0，失败：错误码
int AISDK_FaceManager_FaceDBGroupQueryByUUID(AISDK_ST_FACERECO_GROUPINFO *pDBGroupInfo);



//组织架构配置
//  影响规则筛选业务
//功能：配置人脸子组。
//参数：const AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup 传入参数，人脸组id是必须的
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceSubGroupCfg(const AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup);


//功能：删除人脸子组。
//参数：const DBFaceSubGroup_t *pDBFaceSubGroup 传入参数，仅uuid有效
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceDBSubGroupDel(const AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup);

//功能：修改人脸子组。
//参数：const DBFaceSubGroup_t *pDBFaceSubGroup 传入参数，根据匹配uuid
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceDBSubGroupAlter(const AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup);

//组织架构查询
//功能：查询所有人脸子组。
//参数：DBFaceSubGroup_t **pDBFaceSubGroup 传入参数，传出参数，内部申请空间
        //int *pCount 传出参数，存放实际的有效个数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceSubGroupQueryAll(AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup, int *pCount);

//功能：查询人脸子组的资源释放。
//参数：DBFaceSubGroup_t *pDBFaceSubGroup 传入参数，释放空间
//返回值：成功：0，失败：错误码
//int AISDK_FaceManager_FaceSubGroupQueryRelease(AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup);

//功能：查询某人脸组的子组。
//参数：
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceSubGroupQuery(char* pGroupUuid, AISDK_ST_FACERECO_GROUP_SUB *pDBFaceSubGroup,int *pCount);


//功能：查询人脸子组的所有个数。
//参数：int *pCount 传出参数，存放实际的有效个数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceSubGroupCountQuery(int *pCount);

//功能：查询子组信息
//参数：pDBFaceSubGroup 传入参数，根据uuid匹配，传出参数
//返回值：成功：0，失败：错误码
int AISDK_FaceManager_FaceDBSubGroupQueryByUUID(AISDK_ST_FACERECO_SUBGROUPINFO *pDBFaceSubGroup);

//人脸添加-有特征值
//人脸添加-无特征值带底图    ：抽特征值后 再添加 
//功能：添加人脸信息。
//参数：const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo 传入参数
// 暂不支持pFacePicPathname添加 置NULL
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceInfoAdd(const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo, const char* pFacePicPathname);

//人脸添加-无特征值有jpg图片 o
//功能：添加人脸信息。
//参数： pFacePicPathname 人脸图片路径
//参数： AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo 传入参数  [特征值和uuid会被jpg解析出的信息填充]
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceInfoAdd_ByJpg(const char* pFacePicPathname, AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo);

//功能：获取人脸jpg算法特征值及抠图人脸jpg。
//输入参数：pFacePicPathname 人脸图片路径
//输出参数：pFeature 人脸特征值数据
//			featureLen 人脸特征值长度
//			pCropJpgBuf 人脸抠图数据; 可为NULL 则不输出
//			pCropJpgLen 人脸抠图长度; 可为NULL 则不输出
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_GetAlgoFaceAttrByJpg(const char* pFacePicPathname, AISDK_ST_FACERECO_JPG_RET* pRet);

//删除人脸
//功能：删除人脸信息。
//参数：const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo 传入参数，仅uuid有效
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceInfoDel(const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo);

//修改人脸信息
//功能：修改人脸信息。
//参数：const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo 传入参数，根据uuid匹配
// 		pFacePicPathname 人脸图片路径名， 有特征值数据优先使用现成的特征值修改，无特征值时可以根据图片抽特征并完成修改
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceInfoAlter(const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo, const char* pFacePicPathname);


//功能：按人脸组ID查询人脸信息（因数据量大，无法全部输出，因此采用limit和offset的方式）
//参数：int nStatus 传入参数
	//int *plimit传入参数，pDBFaceInfo指向结构体的个数，传出参数，实际查询的个数
	//int nOffset 传入参数，偏移个数
	//DBFaceInfo_t *pDBFaceInfo 传出参数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceDBInfoQueryByGroup(uint64_t GroupId,int *pLimit,  int nOffset, AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo);

//人脸查询 （条件）   Z
//  包含版本信息
//功能：按所属分组查询人脸信息（因数据量大，无法全部输出，因此采用limit和offset的方式）
//参数：//AISDK_ST_FACERECO_FACEINFO *pINDBFaceInfo传入参数
		//int *plimit传入参数，pDBFaceInfo指向结构体的个数，传出参数，实际查询的个数
		//int nOffset 传入参数，偏移个数
		//DBFaceInfo_t *pDBFaceInfo 传出参数
		// eOrderMode  暂无效
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceInfoQueryBySubGroup(AISDK_ST_FACERECO_FACEINFO *pINDBFaceInfo, E_AISDK_FACE_INFO_QUERY_ORDER_MODE eOrderMode, int *pLimit,  int nOffset, AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo);


//功能：查询子组所有人脸信息个数
//参数：int *pCount 传出参数，存放实际的有效个数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceDBInfoCountQueryBySubGroup(const AISDK_ST_FACERECO_FACEINFO *pSubGroupUUID,int *pCount);

//功能：查询单个人脸信息
//参数：DBFaceInfo_t *pDBFaceInfo 传入参数，根据人脸id匹配，传出参数
//返回值：E_AISDK_ERR_FACEDB
int AISDK_FaceManager_FaceDBInfoQueryByFaceID(AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo);


//功能：从jpg人脸图中识别人脸信息。
//  如果有人脸信息则是已入库人员
//  有特征值则检测并抽特征成功
//参数：const AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo 传出参数。
//参数：pFacePicPathname  人脸jpg图片的路径名。 
//返回值：成功：0，失败：错误码
int AISDK_FaceManager_DetectFaceInfoByJpg(AISDK_ST_FACERECO_FACEINFO *pDBFaceInfo, const char* pFacePicPathname);



//组织架构修改了，人员老旧的组织信息哪里更新？ 数据库层自己更新？  被动更新人脸时更新 
//根据有效参数匹配，这样是否好？
//是需要按人脸id（非uuid，算法入库id）查询的
//组织架构依赖人脸组(脸库)
// 人脸修改时，组织改变和所属脸库的冲突是否需要检测和处理？


#endif

//功能：设置抓拍配置
//参数：pSnapshotCfg 输入参数，抓拍配置结构
//返回值：成功：0，失败：错误码
int AISDK_SetSnapshotCfg(AISDK_DEF_S_SNAPSHOT_CFG *pSnapshotCfg);

//功能：获取抓拍配置
//参数：pSnapshotCfg 输出参数，抓拍配置结构
//返回值：成功：0，失败：错误码
int AISDK_GetSnapshotCfg(AISDK_DEF_S_SNAPSHOT_CFG *pSnapshotCfg);

//函数功能: 获取移动侦测参数(VDA_MD_PARAM_S)
//输出参数: pMdParam: 移动侦测的配置参数指针
//返回值  : 0-成功,<0-错误代码
int AISDK_MotionDetect_Get_MdParam(AISDK_VDA_MD_PARAM_S *pMdParam);

//函数功能: 获取移动侦测数据(PVDA_MD_AREADATA_S)
//输入参数: 无
//输出参数: pMdData: 移动侦测数据结构指针
//返回值  : 0-成功,<0-错误码
int AISDK_MotionDetect_Get_MdData(AISDK_VDA_MD_AREADATA_S *pMdData);

/**
 * @description: 打开 VDA 模块
 * @param {void} nMaxViChn:最大通道数,stChipConf:芯片参数 stVdaConf:vda配置
 * @return {SINT32} 正常返回0，异常返回<0
 */
int AISDK_InitVda(int nMaxViChn,AISDK_VIDEO_CHIP_CONF_S stChipConf,AISDK_VDA_CONF_S stVdaConf);

/**
 * @description: 关闭 VDA 模块
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
int AISDK_DeInitVda(void);

/**
 * @description: 创建移动检测和遮挡检测的通道
 * @param  stVdaChnAttr vda通道参数
 * @return {SINT32} 正常返回0，异常返回<0
 */
int AISDK_InitVdaChn(AISDK_VDA_CHN_ATTR_S stVdaChnAttr);

/**
 * @description: 销毁移动侦测和遮挡检测的通道
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
int AISDK_DeInitVdaChn(void);

/*
 * 函数介绍: 设置马赛克参数，供移动侦测使用排除马赛克区域使用
 * 输入参数: VMosaic: 马赛克区域
 * 			 bDestory: 销毁标志 0:不销毁马赛克区域，对应的移动侦测区域无效，1：销毁马赛克区域，对应的移动侦测区域有效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 */
int AISDK_Set_MosaicParam(AISDK_VMOSAIC_REGION_S VMosaic, int bDestory);



#ifdef __cplusplus
}
#endif    //@__cplusplus

#endif   // __AISDK_API_H__

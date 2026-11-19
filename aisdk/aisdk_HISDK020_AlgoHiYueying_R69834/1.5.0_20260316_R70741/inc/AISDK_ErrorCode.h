/*


*/
#ifndef __AISDK_ERROR_CODE_H__
#define __AISDK_ERROR_CODE_H__




#ifdef __cplusplus
extern "C" {
#endif    //@__cplusplus


#define AISDK_ERRORCODE_OFFSET_COMMON			(-0x20000)
#define AISDK_ERRORCODE_OFFSET_FACE_DB_OPT		(-0x1000)	//0x00100000

#define AISDK_RETCODE_SUCCESS					(0)



enum E_AISDK_ERR_COMMON
{
	AISDK_ERR_Success = AISDK_RETCODE_SUCCESS,
	AISDK_ERR_ParamNull = AISDK_ERRORCODE_OFFSET_COMMON - 0x1,
	AISDK_ERR_ParamError = AISDK_ERRORCODE_OFFSET_COMMON - 0x2,
	AISDK_ERR_ExceedCapMax = AISDK_ERRORCODE_OFFSET_COMMON - 0x3,
	AISDK_ERR_ExceedCapMin = AISDK_ERRORCODE_OFFSET_COMMON - 0x4,
	AISDK_ERR_NotFind = AISDK_ERRORCODE_OFFSET_COMMON - 0x5,
	AISDK_ERR_NotInitSys = AISDK_ERRORCODE_OFFSET_COMMON - 0x6,
	AISDK_ERR_NotInitMod = AISDK_ERRORCODE_OFFSET_COMMON - 0x7,
	AISDK_ERR_NotInitFunc = AISDK_ERRORCODE_OFFSET_COMMON - 0x8,
	AISDK_ERR_NotEnabled = AISDK_ERRORCODE_OFFSET_COMMON - 0x9,
	AISDK_ERR_NotSupport = AISDK_ERRORCODE_OFFSET_COMMON - 0xA,
	AISDK_ERR_NotRunning = AISDK_ERRORCODE_OFFSET_COMMON - 0xB,
	AISDK_ERR_ExecFaild = AISDK_ERRORCODE_OFFSET_COMMON - 0xC,
	AISDK_ERR_Other = AISDK_ERRORCODE_OFFSET_COMMON - 0x800,
};

enum E_AISDK_ERR_FACEDB
{
	AISDK_ERR_FACEDB_Success = AISDK_RETCODE_SUCCESS,
	AISDK_ERR_FACEDB_PicFormatWrong = 			AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x1, 	//图片格式有误
	AISDK_ERR_FACEDB_PicFaceNotExist = 			AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x2, 	//图片中没有人脸
	AISDK_ERR_FACEDB_PicFaceDetectFaild = 		AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x3, 	//图片人脸检测失败
	AISDK_ERR_FACEDB_PicFaceTooMany = 			AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x4, 	//图片中人脸太多
	AISDK_ERR_FACEDB_PicFaceFeatureFaild = 		AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x5, 	//图片人脸特征提取失败
	AISDK_ERR_FACEDB_PicToYuvFaild = 			AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x9, 	//图片转yuv失败
	AISDK_ERR_FACEDB_PicSizeTooLarge = 			AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0xa, 	//图片尺寸太大
	AISDK_ERR_FACEDB_DbOpened = 				AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x80, 	//数据库已打开
	AISDK_ERR_FACEDB_DbOpenFaild,															//数据库打开失败
	AISDK_ERR_FACEDB_DbBuildTableFaild,														//数据库建表失败
	AISDK_ERR_FACEDB_DbNotInit,																//数据库未初始化
	AISDK_ERR_FACEDB_ParamNull,																//参数为空
	AISDK_ERR_FACEDB_ParamError,															//参数错误
	AISDK_ERR_FACEDB_OutOfMem,																//内存不足
	AISDK_ERR_FACEDB_AddGroupDbFaild,														//添加组-数据库操作失败
	AISDK_ERR_FACEDB_DelGroupDbFaild,														//删除组-数据库操作失败
	AISDK_ERR_FACEDB_GroupDbQueryFaild,														//组查询-数据库操作失败
	AISDK_ERR_FACEDB_GroupAlgoCreateFaild,													//组-算法创建失败
	AISDK_ERR_FACEDB_GroupFull,																//组已满
	AISDK_ERR_FACEDB_GroupNotExist,															//组不存在
	AISDK_ERR_FACEDB_GroupDbNotExist,														//组数据库不存在
	AISDK_ERR_FACEDB_GroupDbOptFaild,														//组数据库操作失败
	AISDK_ERR_FACEDB_GroupDbReQueryFaild,													//组数据库重新查询失败
	AISDK_ERR_FACEDB_SubGrpDbOptFaild,														//子组数据库操作失败
	AISDK_ERR_FACEDB_FaceDbOptFaild,														//人脸信息数据库操作失败
	AISDK_ERR_FACEDB_FaceDbQueryFaild,														//人脸信息查询-数据库操作失败
	AISDK_ERR_FACEDB_FaceDbReQueryFaild,													//人脸信息数据库重新查询失败
	AISDK_ERR_FACEDB_FaceNameExist,															//人脸名字已存在
	AISDK_ERR_FACEDB_FaceOptFaild,															//人脸信息操作失败
	AISDK_ERR_FACEDB_FileError,																//文件错误
	AISDK_ERR_FACEDB_FaceFull,																//人脸库已满了
	AISDK_ERR_FACEDB_Other = 					AISDK_ERRORCODE_OFFSET_FACE_DB_OPT - 0x100,	//其他错误
};


#ifdef __cplusplus
}
#endif    //@__cplusplus

#endif   // __AISDK_ERROR_CODE_H__

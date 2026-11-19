#ifndef __TAG_ADVANCEENCCFG_H_
#define __TAG_ADVANCEENCCFG_H_


//遮挡区域(子结构)
typedef struct tagVIDEO_SHELTER
{
	unsigned short wHideAreaTopLeftX;				/* 遮挡区域的x坐标 */
	unsigned short wHideAreaTopLeftY;				/* 遮挡区域的y坐标 */
	unsigned short wHideAreaWidth;				/* 遮挡区域的宽 */
	unsigned short wHideAreaHeight;				/*遮挡区域的高*/
}VIDEO_SHELTER, *LPVIDEO_SHELTER;

typedef struct tagAdvanceEncCfg
{
	int nVideoCodecModeNormal[DVR_MAX_VIDEOIN_NUM];				/*!< 当前普通录像编码类型, 数组成员值: 0,表示H264, 1表示H265 */
	int nVideoCodecModeEvent[DVR_MAX_VIDEOIN_NUM];				/*!< 当前事件录像编码类型, 数组成员值: 0,表示H264, 1表示H265 */
	int nVideoCodecModeVGANet[DVR_MAX_VIDEOIN_NUM];				/*!< 当前第二子码流编码类型, 数组成员值: 0,表示H264, 1表示H265 */
	int nVideoCodecModeNet[DVR_MAX_VIDEOIN_NUM];				/*!< 当前子码流编码类型, 数组成员值: 0,表示H264, 1表示H265 */

	VIDEO_SHELTER struShelter[DVR_MAX_VIDEOIN_NUM];		        /*!< 所有通道的视频遮挡配置  */
}AdvanceEncCfg, *lpAdvanceEncCfg;

#endif


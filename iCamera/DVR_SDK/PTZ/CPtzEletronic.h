#ifndef __PTZ_ELETRONIC_H_

class CPtzEletronic
{
  private:
	int m_nChannel;

	int m_nDevType;

	/*!< 数字变倍 (0-关 1-开)                                   */
	/*!< 低照度 (0-关 1-开)                                     */
	/*!< 背光补偿 (0-关 1-开)                                   */
	/*!< 自动快门 (0-手动 1-自动)                               */
	/*!< 白平衡 (0-手动 1-自动 2-室内 3-室外 4-ATW 5-ONE PUSH)  */
	/*!< 红增益 (0~255)                                         */
	/*!< 蓝增益 (0~255)                                         */
	/*!< 锐度(0~15)                                             */
	/*!< 曝光模式0自动 1手动 2 快门优先 3光圈优先               */
	/*!< 快门速度 (0~21)                                        */
	/*!< 光圈 (0-17）                                           */
	/*!< 宽动态 (0-关 1-开)                                     */
	/*!< 3D降噪 (0-关 1-开)                                     */
	/*!< 镜像  (0-关 1-开)                                      */
	/*!< 翻转  (0-关 1-开)                                      */
	/*!< 高清视频模式  (长宽比，分辨率，制式频率) */
	int m_nEletronicProp[100];

	static const char *m_aszEletronicPropName[];
	static int m_nSonyDefValue[];
  public:
	  CPtzEletronic(int nIndex);
	 ~CPtzEletronic();
	int Init(int nDevType);
	int GetType();
	int GetEletronicProp(int nIndex);
	int SetEletronicProp(int nIndex, int nValue);
	int ResetAllEletronicProp();
};

#endif /*__PTZ_ELETRONIC_H_*/

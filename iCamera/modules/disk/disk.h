#ifndef DISK_H
#define DISK_H


#ifdef __cplusplus
extern "C"
{
#endif

#define TRUE 						1
#define FALSE 					0
#define READONLY    				0
#define READWRITE   				1
#define MAX_DISK_PARTITIONSEX	14

#define MOUNTPOINT "/tmp/app/disk/"

#ifndef BOOL
#define BOOL	int
#endif


	typedef struct _PARTITONDEVICEINFO
	{
		int Type;				//分区类型0为系统保留分区，1为流存储分区
		int Partition_has_mounted;
		unsigned long Total_partitions_space;
		unsigned long Available_partitions_space;
		char DevName[50];
		char Mount_point[50];
		unsigned long dwStartCHS;	// 分区起始扇区
		unsigned long dwEndtCHS;	// 分区大小

	} PARTITONDEVICEINFO;

	typedef struct _DISKDEVICEINFOEx
	{
		char DeviceName[30];	//model
		char SerialNo[30];		//序列号
		char Dev[30];			//设备节点
		unsigned long Total_disk_space;
		int IsWritting;
		int RW_Status;
		int PartitionsNum;
		struct _PARTITONDEVICEINFO Partitions[MAX_DISK_PARTITIONSEX];
	} DiskDeviceInfoEx;

/* call-back funtion definition */
	typedef void (*HD_EventCallback) (const char *dev);

	typedef enum _DISKACTTYPE
	{
		DISKACT_NONE,
		DISKACT_SATA = 1,		/* hotplug-events drive SATA devices */
		DISKACT_USB = 2,		/* hotplug-evnets drive USB devices */
		DISKACT_CDROM = 4,		/* hotplug-evnets drive CDROM devices */
		DISKACT_SDCARD = 8,		/* hotplug-evnets drive CDROM devices */
		DISKACT_ALL = 255,		/* hotplug-events drive all devices */

	} DiskActType;


//!\brief  获取系统内硬盘总数
//!\return 成功硬盘个数
//!\note   异常卡会阻塞
	int EDVR_HD_GetTotalDiskCount(void);

    int EDVR_HD_GetSdCardCount(void);

//!\brief       获取指定硬盘的总线编号
//!\param   int index       硬盘编号(从0开始，到硬盘总数-1)
//!\return  成功返回硬盘总线编号,失败返回-1
	int EDVR_HD_GetBusNumber(int index);

//!\brief       获取指定硬盘的总线编号
//!\param   const char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  成功返回硬盘总线编号,失败返回-1
	int EDVR_HD_GetBusNumberEx(const char *dev);

//!\brief       有新硬盘加入时的回调.对硬盘各分区作mount操作
//!\param   char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  添加成功返回该硬盘被成功挂载的分区个数，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_AddDiskCallBack(const char *dev);

//!\brief      移除硬盘的回调.对硬盘各分区作umount操作
//!\param   char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  移除成功返回0，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_RemoveDiskCallBack(const char *dev);

//!\brief       检测到挂载点被置为RO，将之重新挂载后，通知EDVR将之添加
//!\param   MountpointAdded 处理挂载点添加的函数指针.
//!\return  无
//!\note   异常卡会阻塞
	void EDVR_HD_SetMountPointAdd(void (*MountpointAdded) (char *MountPoing));

//!\brief       检测到挂载点被置为RO，通知EDVR将之移除
//!\param   MountpointRemoved   处理挂载点被移除的函数指针.
//!\return  无
//!\note   异常卡会阻塞
	void EDVR_HD_SetMountPointRemove(void (*MountpointRemoved) (char *MountPoing));

//!\brief       获取指定硬盘信息.
//!\param   int index       硬盘编号(从0开始，到硬盘总数-1)
//!\param   DiskDeviceInfoEx * diskinfo 保存磁盘信息的结构体的指针.
//!\return  成功返回0，失败返回-1
//!\note   异常卡会阻塞
	int EDVR_HD_GetDiskInfoEx(int index, DiskDeviceInfoEx * diskinfo);

//!\brief       获取指定硬盘信息.
//!\param   const char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\param   DiskDeviceInfoEx * diskinfo 保存磁盘信息的结构体的指针.
//!\return  成功返回0，失败返回-1
//!\note   异常卡会阻塞
	int EDVR_HD_GetDiskInfoExFromDev(const char *dev, DiskDeviceInfoEx * diskinfo);

//!\brief       判断给定硬盘是否为新硬盘(SD卡设备的判断标准为至少有一个FAT32分区)
//!\param   const char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  是新硬盘返回1,否则返回0,错误返回-1
//!\note   异常卡会阻塞
	BOOL EDVR_HD_IsNewDiskEx(const char *dev);

//!\brief       初始化硬盘(SD卡设备会初始化为1个FAT32主分区，包含所有存储空间)
//!\param   const char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  是新硬盘返回1,否则返回0,错误返回-1
//!\note   异常卡会阻塞
	int EDVR_HD_Init(const char *dev);

//!\brief       创建一个分区
//!\param   int index               硬盘编号(从0开始，到硬盘总数-1)
//!\param   unsigned long nSize 分区大小，单位为MB
//!\return  0为成功，1硬盘不存在，2为超过最大分区数量，3其他错误
//!\note   异常卡会阻塞
	int EDVR_HD_MakePartitionFromSize(int index, unsigned long nSize);

//!\brief       删除一个分区
//!\param   int index               硬盘编号(从0开始，到硬盘总数-1)
//!\param   int nPartitionIndex 分区编号(从0开始，到有效分区总数-1)
//!\return  成功返回0, 失败返回错误码

/*
说明:
	硬盘只有所有分区被卸载以后，才能删除分区。
	可以调用EDVR_HD_RemoveDiskCallBack来卸载所有分区。
	卸载时，会自动先关闭该硬盘上所有打卡的文件。
	
错误码:	
	1	没有找到指定的硬盘
	2	超过最大允许的分区数
	-1	没有找到指定的分区
	-2	该硬盘有分区被挂载
	-3	该分区是系统保留分区
	-4	该硬盘有文件打开
*/
//!\note   异常卡会阻塞
	int EDVR_HD_DelPartition(int index, int nPartitionIndex);

//!\brief       挂载指定分区
//!\param   char *dev       硬盘分区设备节点名，如/dev/sda1(SD卡设备为/dev/mmcblk0p1)
//!\return  成功返回0，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_MountPartition(const char *dev);

//!\brief   挂载指定分区(不进行dosfsck)
//!\param   const char *dev       硬盘分区设备节点名，如/dev/sda1(SD卡设备为/dev/mmcblk0p1)
//!\return  成功返回0，失败返回-1.
    int DiskMountPartitionNoDosfsck(const char *dev);

//!\brief       卸载指定分区
//!\param   char *mountpoint    分区挂载点，如/dat/sda1
//!\return  成功返回0，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_UMountPartition(const char *mountpoint);

//!\brief   重建分区表
//!\param   const char *dev  硬盘分区设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  成功返回0，失败返回-1.
    int EDVR_HD_ReBuildPartition(const char *dev, int part);

//!\brief       格式化分区(分区必须卸载后才能格式化)
//!\param   const char *dev       硬盘分区设备节点名，如/dev/sda1(SD卡设备为/dev/mmcblk0p1)
//!\return  成功返回0，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_FormatPartition(const char *dev);

//!\brief       检测是否有挂载点被置为RO (只读)
//!\return  无
//!\note   异常卡会阻塞
	void EDVR_HD_ReMountRo();

//!\brief       判断给定设备节点是否有效
//!\param   const char *dev 设备节点，如/dev/sda
//!\return  是有效磁盘返回1，否则返回0.
	int EDVR_HD_IsValidDisk(const char *dev);

//!\brief       判断给定设备节点的磁盘类型
//!\param   const char *dev 设备节点，如/dev/sda
//!\return  DiskActType
	DiskActType EDVR_HD_GetDiskType(const char *dev);

//!\brief       根据磁盘索引获取设备节点名
//!\param   int index       硬盘编号(从0开始，到硬盘总数-1)
//!\param   char *dev       返回磁盘设备节点名
//!\param   int len     缓冲区大小
//!\return  成功返回0，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_Index2DevNode(int index, char *dev, int len);

//!\brief       根据设备节点名获取挂载点
//!\param   const char *pszDevNode      磁盘或分区的设备节点名
//!\param   int nPartitionIndex         分区号，从1开始
//!\param   char *pszMountPoint         返回挂载点
//!\param   int nBufLen                 缓冲区大小
//!\return  成功返回0，失败返回-1.
	int EDVR_HD_GetMountPoint(const char *pszDevNode, char *pszMountPoint, int nBufLen);

//!\brief       根据磁盘设备节点名获取挂载点
//!\param   const char *pszDiskDevNode  磁盘设备节点名
//!\param   int nPartitionIndex         分区号，从1开始
//!\param   char *pszMountPoint         返回挂载点
//!\param   int nBufLen                 缓冲区大小
//!\return  成功返回0，失败返回-1.
	int EDVR_HD_GetPartitionMountPoint(const char *pszDiskDevNode, int nPartitionIndex, char *pszMountPoint, int nBufLen);

//!\brief       根据分区挂载点，返回磁盘设备节点名
//!\param   const char *pszMountPoint       分区挂载点
//!\param   char *pszDiskDevNode                返回磁盘设备节点名
//!\param   int nBufLen                 缓冲区大小
//!\return  成功返回0，失败返回-1.
	int EDVR_HD_GetDiskDevNode(const char *pszMountPoint, char *pszDiskDevNode, int nBufLen);

//!\brief       获取分区挂载点空间大小
//!\param   const char *pszMountPoint       分区挂载点
//!\param   int *pbMounted              返回是否挂载
//!\param   unsigned long  *pdwTotalSize        返回总的空间大小
//!\param   unsigned long  *pdwAvailableSize    返回剩余空间大小
//!\return  成功返回0，失败返回-1.
//!\note   异常卡会阻塞
	int EDVR_HD_GetMountPointSpace(const char *pszMountPoint, int *pbMounted, unsigned long *pdwTotalSize, unsigned long *pdwAvailableSize);

/**
 *  \brief		Open File and return the Handler
 *
 *  \param	char *pszDestFileName	File name
 *
 *  \return	Success:  return File Description Handle
 *			Failure:  return -1
 */
 //!\note   异常卡会阻塞
	int EDVR_HD_BufferTransferStart(char *pszDestFileName);

/**
 *  \brief		copy the data of buffer to the file in usb device
 *
 *  \param	int nTranferHandle		file handle
 *		 	char *pBuffer           	pointing to the address filled with user's data
 *			unsigned long	nSize	the size of buffer
 *
 *  \return	Success: return the size of written bytes
 *			Failure: return -1
 */
  //!\note   异常卡会阻塞
	int EDVR_HD_BufferTransfer(int nTransferHandle, char *pBuffer, unsigned long nSize);

/**
 *  \brief		Close the openned file
 *
 *  \param	int nTransferHandle		Openned file handle
 *
 *  \return	Success: return 0
 *			Failure: return -1
 */
   //!\note   异常卡会阻塞
	int EDVR_HD_BufferTransferStop(int nTransferHandle);

/**
 *  \brief  setting callback functions handling the kind of hotplug-events
 *
 *  \param  DeviceActType devtype
 *			ACT_ALL     sending hotplug-events to all removable devices
 *			ACT_SATA    sending hotplug-events to removable sata devices
 *			ACT_USB	    sending hotplug-events to removable usb devices
 *			ACT_CDROM	sending hotplug-events to removable cdrom devices
 *			ACT_SDCARD	sending hotplug-events to removable sdcard devices
 *			ACT_NONE	not handle hotplug-events
 *
 *  \return void
 */
	void EDVR_HD_SetEventType(DiskActType devtype);

/**
 *  \brief handle usb device remove event, and invoke call-back functions
 *
 *  \param	HD_EventCallback pRemoveCallback
 *			call-back functions for usb device unplugged
 *
 *  \return	0	successed
 *			1	event has already been installed
 *			2      error while installed signal
 */
   //!\note   异常卡会阻塞
	int EDVR_HD_DeviceRemoveEvent(HD_EventCallback pRemoveCallback);

/**
 *  \brief handle usb device add event, and invoke call-back functions
 *
 *  \param	HD_EventCallback pAddCallback
 *			call-back functions for usb device plugged
 *
 *  \return	0	successed
 *			1	event has already been installed
 *			2	error while installed signal
 */
   //!\note   异常卡会阻塞
	int EDVR_HD_DeviceAddEvent(HD_EventCallback pAddCallback);

#ifdef __cplusplus
}
#endif

#endif

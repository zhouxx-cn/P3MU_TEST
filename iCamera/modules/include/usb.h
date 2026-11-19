#ifndef USB_H
#define USB_H

/**
 *  macro definition
 */

#ifdef __cplusplus
extern "C"
{
#endif

/* usb device  support max partition */
#define MAX_PARTITIONS			10

/* usb transfer optimized buffer size */
#define USB_BUF_SIZ				(1024 * 512)

/* usb max multifile-writing numbers */
#define MAX_USB_FILEHANDLE		10

/**
 *  struct/union definition
 */
	typedef struct _DEVICEINFO
	{
		char DeviceName[30];	/* usb device model */
		char DeviceType[15];	/* usb device type: DISK or CD-ROM */
		char DeviceVendor[30];	/* usb device vendor */
		int Partitions;			/* usb device partitions number */
		unsigned long Total_partitions_space[MAX_PARTITIONS];	/* total space */
		unsigned long Available_partitions_space[MAX_PARTITIONS];	/* total available space */
		char Mount_point[MAX_PARTITIONS][60];	/* per-mounted partitions space */
		char DevName[MAX_PARTITIONS][60];	/* per-mounted node name */
	} DeviceInfo;

	typedef struct _HOTPLUGINFO
	{
		char DeviceName[30];	/* device name */
		unsigned short Partitions;	/* device partitions information */
		int DevType;			/* device type */
		void *Reserved;			/* reserved ? */
	} HotplugInfo;

	typedef enum _DEVICEACTTYPE
	{
		ACT_ALL,				/* hotplug-events drive all devices */
		ACT_SATA,				/* hotplug-events drive SATA devices */
		ACT_USB,				/* hotplug-evnets drive USB devices */
		ACT_CDROM,				/* hotplug-evnets drive CDROM devices */

		ACT_NONE
	} DeviceActType;


/* call-back funtion definition */
	typedef void (*USB_EventCallBack) (HotplugInfo hpInfo);

/**
 *  external functions
 */

	int EDVR_USB_IsConnect(char *devname);

/**
 *  \brief no used
 *
 *  \param  
 *
 *  \return 
 */
	void EDVR_USB_MountDevice();

/**
 *  \brief no used
 *
 *  \param  
 *
 *  \return 
 */
	void EDVR_USB_UMountDevice();

/**
 *  \brief  get device information from index number
 *
 *  \param  int nIndex  	 index number
 *			DeviceInfo *info deviceinfo
 *
 *  \return 0  success
 *			-1 error
 */
	int EDVR_USB_GetDeviceInfo(int nIndex, DeviceInfo * device_info);

/**
 *  \brief  get total usb/ide device counts
 *
 *  \param  void
 * 
 *  \return if error return -1
 *			if success return total disk counts
 */
	int EDVR_USB_GetTotalDiskCount();

	int EDVR_USB_CDRecord(int index, char *FileNames[], int FileNumber, void (*CDRecordCallBack) (int ret_status, int MB_Written));

	int EDVR_USB_CDErase(int index);

/**
 *  \brief		Open File and return the Handler
 *
 *  \param		char *pDestFileName
 *				File name
 *
 *  \return		Success:  return File Description Handle
 *				Failure:  return -1
 */
	int EDVR_USB_BufferTransferStart(char *pDestFileName);

/**
 *  \brief   copy the data of buffer to the file in usb device
 *
 *  \param   int nTranferHandle  	 file handle
 *		 	 char *pBuffer           pointing to the address filled with
 *									 user's data
 *			 unsigned long			 nSize
 *									 the size of buffer
 *
 *  \return  Success: return the size of written bytes
 *			 Failure: return -1
 */
	int EDVR_USB_BufferTransfer(int nTransferHandle, char *pBuffer, unsigned long nSize);

/**
 *  \brief  Close the openned file
 *
 *  \param  int nTransferHandle
 *			Openned file handle
 *
 *  \return Success: return 0
 *			Failure: return -1
 */
	int EDVR_USB_BufferTransferStop(int nTransferHandle);

/**
 *  \brief handle usb device remove event, and invoke call-back functions
 *
 *  \param  USB_EventCallBack USB_RevmoveCallBack
 *			call-back functions for usb device unplugged
 *
 *  \return  0		successed
 *			 1		event has already been installed
 *			 2      error while installed signal
 */
	int EDVR_USB_DeviceRemoveEvent(USB_EventCallBack USB_RemoveCallBack);

/**
 *  \brief handle usb device add event, and invoke call-back functions
 *
 *  \param  USB_EventCallBack USB_AddCallBack
 *			call-back functions for usb device plugged
 *
 *  \return  0		successed
 *			 1		event has already been installed
 *			 2      error while installed signal
 */
	int EDVR_USB_DeviceAddEvent(USB_EventCallBack USB_AddCallBack);

/**
 *  \brief  clear usb handled events
 *
 *  \param  void
 *
 *  \return void
 */
	void EDVR_USB_DeviceClearEvent(void);

/**
 *  \brief  setting callback functions handling the kind of hotplug-events
 *
 *  \param  DeviceActType devtype
 *			ACT_ALL     sending hotplug-events to all removable devices
 *			ACT_SATA    sending hotplug-events to removable sata devices
 *			ACT_USB	    sending hotplug-events to removable usb devices
 *			ACT_CDROM	sending hotplug-events to removable cdrom devices
 *			ACT_NONE	not handle hotplug-events
 *
 *  \return void
 */
	void EDVR_USB_SetEventType(DeviceActType devtype);

/**
 *  \brief  get sata device inode name from the bus number of the sata device
 *
 *  \param  busno
 *		    the bus number of the sata device
 *
 *		    devname
 *		    the device inode name of the sata device, such as sda,sdb...
 *
 *  \return  0		successed
 *			 -1		failured
 */
	int EDVR_SATA_Index2Name(int busno, char *devname);

/**
 *  \brief  get sata device bus number from the device name
 *
 *  \param  busno
 *		    the bus number of the sata device
 *
 *		    devname
 *		    the device inode name of the sata device, such as sda,sdb...
 *
 *  \return  0		successed
 *			 -1		failured
 */
	int EDVR_SATA_Name2Index(char *devname, int *busno);

/**
 *  \brief  remove sata device by hand
 *
 *  \param  index
 *			sata device bus number
 *
 *  \return  0		successed
 *			 -1		failured
 */
	int EDVR_SATA_DeviceRemove(int index);


/**
 *  \brief Get USB LIB Version Information 
 *
 *  \param  void
 *			
 *  \return USB Version Strings 
 */
	char *EDVR_USB_GetVerionString(void);

#ifdef __cplusplus
}
#endif
#endif

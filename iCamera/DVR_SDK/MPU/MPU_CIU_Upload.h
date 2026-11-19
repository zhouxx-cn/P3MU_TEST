#ifndef _INC_MPU_CIU_UPLOAD_H_
#define _INC_MPU_CIU_UPLOAD_H_

typedef int (*Upload_Callback) (unsigned long dwUserID, int iFileType, unsigned long ulTransSize, unsigned long ulTotalSize, const char *pcIpAddr);	//

#endif							/* #ifndef _INC_MPU_CIU_UPLOAD_H_ */

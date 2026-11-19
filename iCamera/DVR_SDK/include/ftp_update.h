#ifndef _FTP_LIB_H_
#define _FTP_LIB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ftp filelist struct */
#define MAX_FILE	256
#define MAX_PATH 	1024

#define DWORD unsigned long

	typedef struct ftp_file_t
	{
		int attr;				/* file attribute */
		DWORD size;				/* file size */
		char filename[MAX_PATH];	/* file name */
	} ftp_file_t;

	typedef int (*Progress) (int bytes);

#define ATTR_DIR	0x01
#define ATTR_FILE	0x02

	int EDVR_FTP_SetProcessCB(Progress cbGetFileProcess);

	int EDVR_FTP_SetServerIP(const char *ftp_server_ip, int nPort);

	int EDVR_FTP_SetUserAccount(const char *ftp_usr_name, const char *ftp_usr_passwd);

	int EDVR_FTP_SetWorkDir(const char *remote_dir, const char *local_dir);

	int EDVR_FTP_Start();

	int EDVR_FTP_Get_Filelist(const char *_path, ftp_file_t * _filelist);

	int EDVR_FTP_Get_File(const char *sRemoteFile, const char *sLocalFile);

	int EDVR_FTP_Quit(void);


#ifdef __cplusplus
}
#endif							/* __cplusplus */

#endif

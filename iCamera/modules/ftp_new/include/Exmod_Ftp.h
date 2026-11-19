#ifndef __EXMOD_FTP_H__
#define __EXOOD_FTP__

/*@prief ftp_upload 通过FTP进行文件上传
 *@param[IN] ServerName 服务器名称
 *@param[IN] ServerPort 服务器端口号
 *@param[IN] FolderPath 远程文件夹名称
 *@param[IN] UserName 服务器用户名
 *@param[IN] Password 服务器密码
 *@param[IN] FilePath 本地文件名(全路径)
 *@param[IN] FileName 远程文件名
 *@return 0 成功 -1失败
 */
int ftp_upload(const char *ServerName, int ServerPort, const char *FolderPath, const char *UserName, const char *Password, const char *FilePath, const char *FileName);

#endif

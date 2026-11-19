
/*
*\brief ADSL configration and connect functions
*
* (C) 2005-2008 WuHan HighEasy Tecnology
*
* \author pczhang pczhang@higheasy.com
*
*\file adsl.c
*\remark
*
* ADSL configaration & Connect & Disconnect Share libarys for HighEasy EDVR Platform
*
* Create Date : 2006.02.20
*
*This Share Libarary should be use with the Following Files
* 	"/usr/sbin/pppoe"
* 	"/usr/sbin/pppd"
* 	"/etc/ppp/pppoe.conf"
* 	"/etc/ppp/pap-secrets"
*       "/etc/ppp/chap-secrets"
* 	"/etc/resolv.conf"
* 	"/usr/sbin/pppoe-start"
* 	"/usr/sbin/pppoe-stop"
* 	"/usr/sbin/pppoe-status"
* History
*	(1) pczhang 2006-02-21
*		Remove sed command
*       (2) pczhang 2007-4-27
*               Modify EDVR_ADSL_Connect Function to fix EDVR_ADSL_Connect return failed while PPPoE-Link is OK !
*               Add Function EDVR_ADSL_GetADSLStatus
*       (3) pczhang 2007-4-30
*               Add restore ConfigFile when Some ConfigFile is destroyed!
*       (4) pczhang 2007-8-27
*               Add More Strong Backup mechinism for Important Configuration File such as pppoe.conf
*                Change "system with mv" to rename
*       (5) pczhang 2007-11-26
*               Change "system" to "mysystem"
*       (6) pczhang 2008-3-17
*               Add CHAP config file
*       (7) pczhang 2009-8-10
*               Fix /etc/resolv.conf larged
*
*/
#include <stdio.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "adsl_api.h"
#include "ip.h"
#include "mysystem.h"
#include "sys_banned.h"
/*!Define standard RETURN VALUES for FALSE*/
#define FALSE 0

/*!Define standard RETURN VALUES for TRUE*/
#define TRUE   1

/*!Define PPPOE configuration file */
#define PPPOE_CONF "/mnt/mtd/etc/ppp/pppoe.conf"

/*!Define PAP-SECRETS file*/
#define PAP_CONF "/mnt/mtd/etc/ppp/pap-secrets"


/*!Define CHAP-SECRETS file*/
#define CHAP_CONF "/mnt/mtd/etc/ppp/chap-secrets"

/*!Define */
#define RESOLV_CONF "/mnt/mtd/etc/resolv.conf"

/*!Define Configuration File Start Flag*/
#define CONF_BEGIN_FLAG   "#$[HIGHEASY_ADSL_CONF_BEGIN]$#"

/*!Define Configuration File End Flag*/
#define CONF_END_FLAG   "#$[HIGHEASY_ADSL_CONF_END]$#"

#define MAX_ITEMS   50

/*!Global ADSL Ip*/
static char g_szADSLIp[16] = { 0 };

static char DefaultConfigFile_PPPOE_CONF[][255] = {
	"#$[HIGHEASY_ADSL_CONF_BEGIN]$#",
	"#################################################",
	"#       HighEasy PPPoE Configuration File       #",
	"#                 Rebuild Version               #",
	"#################################################",
	"ETH='eth0'",
	"USER='higheasy'",
	"DEMAND=no",
	"DNSTYPE=SPECIFY",
	"PEERDNS=no",
	"DNS1=204.204.204.204",
	"DNS2=204.204.204.204",
	"DEFAULTROUTE=yes",
	"CONNECT_TIMEOUT=30",
	"CONNECT_POLL=2",
	"ACNAME=",
	"SERVICENAME=",
	"PING=\".\"",
	"CF_BASE=`basename $CONFIG`",
	"PIDFILE=\"/var/run/pppoe_adsl.pid\"",
	"SYNCHRONOUS=no",
	"CLAMPMSS=1412",
	"LCP_INTERVAL=20",
	"LCP_FAILURE=3",
	"PPPOE_TIMEOUT=80",
	"FIREWALL=NONE",
	"LINUX_PLUGIN=",
	"PPPOE_EXTRA=\"\"",
	"PPPD_EXTRA=\"\"",
	"#$[HIGHEASY_ADSL_CONF_END]$#"
};

static void RestoreConfigFile(char *FileName);

static int EDVR_ADSL_SetUserName(char *sUserName);

static int EDVR_ADSL_SetUserPassword(char *sUserName, char *sPwd);

static int EDVR_ADSL_SetDNS_2_PPPOE_CONF(char *pDNS, char *sDNS);

static int EDVR_ADSL_SetDNS_2_RESOLV_CONF(char *pDNS, char *sDNS);

int EDVR_Replace_Section_In_File(char *FileName, char *SectionPattern, char *NewValue);

static int EDVR_ADSL_Conf_File_Valid(const char *FileName);

static int copyfile(const char *szSrcFile, const char *szDstFile);

static int BuildConfigFile(const char *FileName);

/*!
 * \brief
 *   Check the Configuration File is OK or not
 * \param sFileName
 *   the configuration file
 * \return 
 *   0 means ok, otherwise means errors
 */
static int EDVR_ADSL_Conf_File_Valid(const char *sFileName)
{
	/*
	 * the configuration file should look as
	 * #$[HIGHEASY_ADSL_CONF_BEGIN]$#
	 *       .........
	 * #$[HIGHEASY_ADSL_CONF_END]$#
	 */
	FILE *fpConfCheck = NULL;

	char szBufTmp[128];

	/*Step-1 : File Exist or not */
	fpConfCheck = fopen(sFileName, "r");
	if (NULL == fpConfCheck)
		return -1;

	/*Step-2: the content of file is valid or not */
	memset(szBufTmp, 0, 128);
	fgets(szBufTmp, 128, fpConfCheck);
	if (strstr(szBufTmp, CONF_BEGIN_FLAG) == NULL)
	{
		fclose(fpConfCheck);
		return -1;
	}

	//fseek(fpConfCheck, -strlen(CONF_END_FLAG), SEEK_END); 
	while (!feof(fpConfCheck))
	{
		memset(szBufTmp, 0, 128);
		fgets(szBufTmp, 128, fpConfCheck);
		if (strstr(szBufTmp, CONF_END_FLAG) != NULL)
		{
			fclose(fpConfCheck);
			return 0;
		}
	}

	fclose(fpConfCheck);
	return -1;
}

/*!
 * \brief 
 * 	Set the ADSL Login UserName & Password
 *
 *\param sUserName
 *	Contain "Log-in" user name with '\0'
 *\param sPwd
 *	Contain "Log-in" password with '\0'
 *
 *\return
 *     0 on success , non-zero else
 *
 */
int EDVR_ADSL_SetLoginInfo(char *sUserName, char *sPwd)
{
	int nRet;

	nRet = EDVR_ADSL_SetUserName(sUserName);
	if (nRet < 0)
		return nRet;

	nRet = EDVR_ADSL_SetUserPassword(sUserName, sPwd);
	if (nRet < 0)
		return nRet;

	return 0;
}

/*!
 * \brief 
 * 	Replace the original-value aftr the SectionPattern with NewValue
 * \param  FileName
 *      The FileName that contain the SectionPattern
 * \param SectionPattern
 *	SectionPattern such as DNS1=
 * \param NewValue
 *	NewValue that will be replace the old value 
 * \return
 *	zeor on success, else non-zero
 *
 */

int EDVR_Replace_Section_In_File(char *FileName, char *SectionPattern, char *NewValue)
{
	FILE *fp_new;

	FILE *fp_bak;

	char bakFileName[255];

	char line_buffer[512];

	char *sRetValue;

	/*Step-1 : rename FileName to FileName-bak */
	snprintf(bakFileName, sizeof(bakFileName), "%s-bak", FileName);
	rename(FileName, bakFileName);
	/*Step-2 : Lookup SectionPattern in FileName and Do "Replace" */
	/*Step-2-1 : Create New File */
	fp_new = NULL;
	fp_new = fopen(FileName, "w+");
	if (fp_new == NULL)
	{
		printf("Create File %s Failed...\n", FileName);
		return -1;
	}

	fp_bak = NULL;
	fp_bak = fopen(bakFileName, "r");
	if (fp_bak == NULL)
	{
		fclose(fp_new);
		printf("Can't open File %s\n", bakFileName);
		return -1;
	}


	memset(line_buffer, 0, 512);
	sRetValue = fgets(line_buffer, 512, fp_bak);
	while ((!feof(fp_bak)) && (NULL != sRetValue))
	{
		if (strstr(line_buffer, "#") != NULL || (strstr(line_buffer, SectionPattern) == NULL))
		{						//Just Copy to new file
			fwrite(line_buffer, strlen(line_buffer), 1, fp_new);
		}
		else
		{
			//First Replace and then write to new file
			snprintf(line_buffer,sizeof(line_buffer), "%s%s\n", SectionPattern, NewValue);
			fwrite(line_buffer, strlen(line_buffer), 1, fp_new);
		}
		//when encounter CONF_END_FLAG, just break
		if (strstr(line_buffer, CONF_END_FLAG) != NULL)
			break;

		memset(line_buffer, 0, 512);

		sRetValue = fgets(line_buffer, 512, fp_bak);
	}

	fclose(fp_bak);
	fclose(fp_new);
	return 0;
}

/*!
 * \brief
 *     Set The ADSL Connect UserName provided by ISP
 *
 * \param sUserName
 *	Contain "Log-in" user name with '\0'
 * \return 
 * 	0 on success , non zero else
 *
 */
static int EDVR_ADSL_SetUserName(char *sUserName)
{
	char USER_Buffer[255];

	int bValidStatus = EDVR_ADSL_Conf_File_Valid(PPPOE_CONF);

	if (bValidStatus != 0)
	{
		/*!试图从备份文件恢复PPPOE_CONF */
		RestoreConfigFile(PPPOE_CONF);
		bValidStatus = EDVR_ADSL_Conf_File_Valid(PPPOE_CONF);
		if (bValidStatus != 0)
		{
			/*!如果配置文件恢复也失败， 返回错误 */
			printf("[libadsl.so]Can't find /etc/ppp/pppoe.conf, Now rebuild it!\n");
			BuildConfigFile(PPPOE_CONF);
			//return ERR_WITHOUT_PPPOE_CONF_FILE;
		}
	}

	if (sUserName == NULL)
		return ERR_INVALID_USERNAME;


	snprintf(USER_Buffer, sizeof(USER_Buffer), "'%s'", sUserName);
	EDVR_Replace_Section_In_File(PPPOE_CONF, "USER=", USER_Buffer);

	return 0;
}

/*!
 * \brief
 *     Set The ADSL Connect Username & Password  provided by ISP to PAP_CONF
 *
 * \param sUserName
 *	Contain "Log-in" user name with '\0'
 * \param sPwd
 *	Contain "Log-in" password with '\0'
 * \return 
 * 	0 on success , non zero else
 *
 */
static int EDVR_ADSL_SetUserPassword(char *sUserName, char *sPwd)
{
	FILE *fp_pap_conf;

	FILE *fp_chap_conf;

	if ((sPwd) == NULL || (sUserName == NULL))
	{
		printf("[libadsl.so]UserName or Password is NULL!\n");
		return ERR_INVALID_PWD;
	}
	/*Step-2 : renew PAP password */
	fp_pap_conf = fopen(PAP_CONF, "w+");
	if (NULL == fp_pap_conf)
		return 0;

	fprintf(fp_pap_conf, "\"%s\"    *    \"%s\"\n", sUserName, sPwd);
	fclose(fp_pap_conf);

	/*Step-3 : renew CHAP password */
	fp_chap_conf = fopen(CHAP_CONF, "w+");
	if (NULL == fp_chap_conf)
		return 0;

	fprintf(fp_chap_conf, "\"%s\"    *    \"%s\"\n", sUserName, sPwd);
	fclose(fp_chap_conf);
	return 0;
}

/*!
 * \brief int EDVR_ADSL_SetDNS(char * PrimaryDNS, char * SecondaryDNS) 
 *	Update DNS information in PPPOE_CONF & RESOLV_CONF
 * \param PrimaryDNS
 *	DNS1 information
 * \param SecondaryDNS
 *	DNS2 information
 * \see
 *	EDVR_ADSL_SetDNS_2_RESOLV_CONF 
 *	EDVR_ADSL_SetDNS_2_PPPOE_CONF
 * \return
 *	zero on success, else non-zero
 *
 */
int EDVR_ADSL_SetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	int nRet;

	nRet = EDVR_ADSL_SetDNS_2_RESOLV_CONF(PrimaryDNS, SecondaryDNS);
	if (nRet < 0)
		return nRet;

	nRet = EDVR_ADSL_SetDNS_2_PPPOE_CONF(PrimaryDNS, SecondaryDNS);
	if (nRet < 0)
		return nRet;

	return 0;
}

/*!
 * \brief
 *	Start ADSL Connect
 * \param NULL
 *
 * \return
 * 	zero on success , else non-zero
 */
int EDVR_ADSL_Connect()
{
	int ret = 0;

	char system_cmd[255] = {0};

	char buffer[100] = {0};

	char pppoe_status_file[] = "/mnt/mtd/etc/ppp/pppoe.status";

	FILE *fp_status = NULL;

	snprintf(system_cmd,sizeof(system_cmd), "pppoe-start > /mnt/mtd/etc/ppp/pppoe.status");

	mysystem(system_cmd);

	memset(g_szADSLIp, 0, 16);

	printf("\npppoe-start done.....\n");
	fp_status = fopen(pppoe_status_file, "r");
	if (NULL == fp_status)
	{
		EDVR_ADSL_Disconnect();
		return ERR_WITHOUT_STATUS_FILE;
	}

	fgets(buffer, 100, fp_status);

	fclose(fp_status);

	if (strstr(buffer, "Connected") != NULL)	/*Connect Success */
	{
		ret = mysystem("route del default ppp0");	//先删除后加路由
		ret = mysystem("route add default ppp0");
		printf("\nEDVR_ADSL_Connect : ADSL_Connect Success, route return %d\n", ret);
		if (0 != ret)
		{
			printf("添加路由失败，需要重新拔号方向正常工作\n");
			EDVR_ADSL_Disconnect();
			return ERR_CONNECT_FAILURE;
		}
		return 0;
	}

	EDVR_ADSL_get_if_address("ppp0", g_szADSLIp, GET_ADDRESS);
	if (strcmp(g_szADSLIp, "") != 0)	/*Connct Success too */
	{
		ret = mysystem("route del default ppp0");
		ret = mysystem("route add default ppp0");
		printf("\nEDVR_ADSL_Connect : ADSL_Connect Success, route return %d\n", ret);
		if (0 != ret)
		{
			printf("添加路由失败，需要重新拔号方向正常工作\n");
			EDVR_ADSL_Disconnect();
			return ERR_CONNECT_FAILURE;
		}
		return 0;
	}


	printf("\nEDVR_ADSL_Connect : ADSL_Connect Failure\n");
	return ERR_CONNECT_FAILURE;
}

/*!
 * \brief 
 *	Disconnect ADSL connect
 * \param NULL
 */
int EDVR_ADSL_Disconnect()
{

	mysystem("pppoe-stop > /dev/null");
	return 0;
}


/*!
 * \brief
 *     Get ADSL Status
 * \param NULL
 *     0 means Status is OK ; Other means dis-connect!
  */
int EDVR_ADSL_GetADSLStatus(char *sz_adsl_ip)
{
	char szIp[16];

	memset(szIp, 0, 16);

	EDVR_ADSL_get_if_address("ppp0", szIp, GET_ADDRESS);

	if (strcmp(szIp, "") == 0)
	{
		memset(g_szADSLIp, 0, 16);
		return ERR_CONNECT_FAILURE;
	}

	if (strcmp(szIp, g_szADSLIp) != 0)
	{
		//printf ("\nRoute Table Updated [Old_ADSL_Ip = %s , New_ADSL_Ip = %s]!\n", g_szADSLIp, szIp);
		strncpy(g_szADSLIp, szIp, sizeof(g_szADSLIp) - 1);
		mysystem("route add default ppp0");
	}

	memcpy(sz_adsl_ip, g_szADSLIp, 16);

	return 0;
}


/*!
 * \brief
 *	update DNS1 & DNS2 information in PPPOE_CONF
 * \param pDNS
 *     DNS1 value
 * \param sDNS
 *     DNS2 value
 * \return 
 *     zero on success , else non-zeor	
 *
 */
static int EDVR_ADSL_SetDNS_2_PPPOE_CONF(char *pDNS, char *sDNS)
{
	int bValidStatus = -1;

	bValidStatus = EDVR_ADSL_Conf_File_Valid(PPPOE_CONF);
	if (bValidStatus != 0)
	{
		/*!试图从备份文件恢复PPPOE_CONF */
		RestoreConfigFile(PPPOE_CONF);
		bValidStatus = EDVR_ADSL_Conf_File_Valid(PPPOE_CONF);
		if (bValidStatus != 0)
		{
			/*!如果配置文件恢复也失败， 返回错误 */
			printf("[libadsl.so]Can't find /etc/ppp/pppoe.conf, Now rebuild it!\n");
			BuildConfigFile(PPPOE_CONF);
			//return ERR_WITHOUT_PPPOE_CONF_FILE;
		}
	}
	if (pDNS == NULL)
	{
		printf("[libadsl.so]Primary DNS must be non-null\n");
		return ERR_INVALID_PRIMARY_DNS;
	}

	EDVR_Replace_Section_In_File(PPPOE_CONF, "DNS1=", pDNS);

	/*Step-3 :  Replace the DNS2 Section with the sDNS */
	if (NULL != sDNS)
		EDVR_Replace_Section_In_File(PPPOE_CONF, "DNS2=", sDNS);

	return 0;

}

/*!
 * \brief
 *	Update nameserver DNS1 & DNS2 information in RESOLV_CONF file
 * \param pDNS
 *	DNS1 value
 * \param sDNS
 *      DNS2 value
 * \return 
 *	zero on success, else non-zero
 *
 *
 */
static int EDVR_ADSL_SetDNS_2_RESOLV_CONF(char *pDNS, char *sDNS)
{
	char system_cmd[255];

	if (pDNS == NULL)
		return ERR_INVALID_PRIMARY_DNS;
	/*Step-1 : copy resolv.conf to  resolv.conf-bak */
	/*Step-2 :  Write RP-PPPOE IFO */
	snprintf(system_cmd,sizeof(system_cmd), "echo '#MADE-BY-RP-PPPOE' > %s", RESOLV_CONF);
	printf("\n-1-software[dvrsdk]-%s--%s--\n",__FUNCTION__,system_cmd);
	mysystem(system_cmd);
	/*Step-3 :  Replace the DNS Section with the PrimaryDNS */
	snprintf(system_cmd,sizeof(system_cmd), "echo 'nameserver  %s' >> %s", pDNS, RESOLV_CONF);
	printf("\n-2-software[dvrsdk]-%s--%s--\n",__FUNCTION__,system_cmd);
	mysystem(system_cmd);
	
	/*Step-4 :  Replace the DNS Section with the PrimaryDNS */
	if (NULL != sDNS)
	{
		snprintf(system_cmd,sizeof(system_cmd), "echo 'nameserver  %s' >> %s", sDNS, RESOLV_CONF);
		printf("\n-3-software[dvrsdk]-%s--%s--\n",__FUNCTION__,system_cmd);
		mysystem(system_cmd);
	}

	return 0;
}

/*!
 * \brief
 *    copy a srcFile to dstFile
 * \param szSrcFile
 *    the src File, must be exist!
 * \param szDstFile
 *    the dst File, create if not exist!
 * \return
 *    0 means success, otherwise failed
 */
static int copyfile(const char *szSrcFile, const char *szDstFile)
{
	FILE *fpSrc, *fpDst;

	char TmpBuf[1024];

	size_t nReadBytes;

	fpSrc = fpDst = NULL;
	fpSrc = fopen(szSrcFile, "r");
	if (NULL == fpSrc)
		return -1;

	fpDst = fopen(szDstFile, "w+");
	if (NULL == fpDst)
	{
		fclose(fpSrc);
		return -1;
	}

	while (!feof(fpSrc))
	{
		nReadBytes = fread(TmpBuf, 1, 1024, fpSrc);
		if (nReadBytes > 0)
			fwrite(TmpBuf, 1, nReadBytes, fpDst);
	}

	fclose(fpSrc);
	fclose(fpDst);

	return 0;
}

/*!
 * \brief
 *   Copy *-bak file to * file
 * \param FileName
 *   Original File to restore from bakup file
 * \return
 *   NULL
 */
static void RestoreConfigFile(char *FileName)
{
	char bakFileName[255];

	/*Step-1 : copy FileName-bak to FileName */
	memset(bakFileName, 0, 255);
	snprintf(bakFileName,sizeof(bakFileName), "%s-bak", FileName);
	if (0 != copyfile(bakFileName, FileName))
		printf("\n[libadsl.so] RestoreConfigFile occur FATAL ERROR!!!!!\n");
}

/*!
 * \brief
 *  Build pppoe.conf file incores
 * \param szFileName
 *  ConfigFile Name
 * \return 
 *  0 means success, otherwise means failed(Not Enough space or etc.)
 */
static int BuildConfigFile(const char *szFileName)
{
	FILE *fpRebuild;

	int nrItems;

	int i;

	fpRebuild = NULL;

	if (strcmp(szFileName, PPPOE_CONF) != 0)
	{
		printf("[BuildConfigFile]Not Support ConfigFile type!\n");
		return -1;
	}

	fpRebuild = fopen(szFileName, "w+");
	if (NULL == fpRebuild)
		return -1;

	nrItems = sizeof (DefaultConfigFile_PPPOE_CONF) / 255;

	for (i = 0; i < nrItems; i++)
		fprintf(fpRebuild, "%s\n", DefaultConfigFile_PPPOE_CONF[i]);

	fclose(fpRebuild);
	return 0;
}

#if 0
static int mysystem(const char *cmdstring)
{
	pid_t pid;

	int status;

	if (cmdstring == NULL)
		return 1;

	if ((pid = vfork()) < 0)
		status = -1;
	else if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char *) 0);
		_exit(127);
	}
	else
	{
		while (waitpid(pid, &status, 0) < 0)
			if (errno != EINTR)
			{
				status = -1;
				break;
			}
	}
	return status;
}
#endif

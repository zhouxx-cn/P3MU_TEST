#ifndef ADSL_H
#define ADSL_H

/*!Define return Error-Value*/
#define ERR_WITHOUT_PPPOE_CONF_FILE             -1
#define ERR_WITHOUT_PAP_CONF                    -2
#define ERR_INVALID_USERNAME                    -3
#define ERR_WITHOUT_RESOLV_CONF_FILE            -4
#define ERR_INVALID_PRIMARY_DNS                 -5
#define ERR_INVALID_PWD                         -6
#define ERR_WITHOUT_STATUS_FILE                 -7
#define ERR_CONNECT_FAILURE                     -8


#ifdef __cplusplus
extern "C"
{
#endif

	int EDVR_ADSL_SetLoginInfo(char *sUserName, char *sPwd);
	int EDVR_ADSL_SetDNS(char *PrimaryDNS, char *SecondaryDNS);
	int EDVR_ADSL_Connect();
	int EDVR_ADSL_Disconnect();

/*!
 * \brief
 *     Get ADSL Status
 * \param NULL
 *     0 means Status is OK ; Other means dis-connect!
  */
	int EDVR_ADSL_GetADSLStatus(char *sz_adsl_ip);

#ifdef __cplusplus
}
#endif

#endif

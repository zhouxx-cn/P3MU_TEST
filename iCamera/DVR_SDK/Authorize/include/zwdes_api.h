
#include "global_gather.h"

#if !defined(AFX_GLOBALFUNCS_H__1B76B93F_7D5F_496C_86EB_B8113C8C0385__INCLUDED_)
#define AFX_GLOBALFUNCS_H__1B76B93F_7D5F_496C_86EB_B8113C8C0385__INCLUDED_


enum    {ENCRYPT1,DECRYPT};
typedef enum    {ZW_TRUE,ZW_FALSE} ZW_BOOL;


#define ERROR_INVALID_LICENSECODE   -1
#define ERROR_INVLIAD_ARGRUMENT     -2
#define ERROR_ALLOC_MEM             -3
#define ERROR_INVALID_LICENSELEN    -4
#define ERROR_INVALID_CUSTOMCODE	-5
#define ERROR_INVALID_CLOUDCODE		-6
#define	ERROR_INVALID_SN			-7

#define LICENSE_SOURCE_MAX_LEN      (256)
#define MAX_SN_LEN		            (32)
#define MAC_LEN			            (6)
#define MAX_DID_LENS		        (63)

typedef struct tagalpha
{
	unsigned char	type;
	unsigned char	len;
	char			did[MAX_DID_LENS];
}cloud;
typedef struct tagPI31415926
{
	unsigned short	customID;
	unsigned long	boardID;
	char			sn[MAX_SN_LEN];
	unsigned long	atime;
	unsigned char	mac[MAC_LEN];
	unsigned long	funcion;
	cloud			cloudID;
}license;


#if defined(__cplusplus)
extern "C" {
#endif

void test_PBOC_3DES(unsigned char *Ekey,long *ttext,long *ctext0,long *ctext1);

unsigned int HY_GetDESEncrypt( unsigned char *m_Random , unsigned int m_inLen , unsigned char *m_EncKey , unsigned char *m_product);

                                                                                                                                                                                                                                                                                                                    
unsigned int HY_Get3DESEncrypt( unsigned char *m_Random , unsigned int m_inLen , unsigned char *m_EncKey , unsigned char *m_product);

unsigned int HY_GetDESDecrypt( unsigned char *m_inBuff , unsigned int m_inLen , unsigned char *m_DecKey , unsigned char *m_product);

unsigned int HY_GetDeviceAuth( unsigned char *m_Random , unsigned int m_inLen , unsigned int Enc , unsigned char *m_product);


ZW_BOOL Des_Gok(char *Out, char *In, long datalen, const char *Key, int keylen, unsigned int Type);

int PI31415926(char* szActiveCode, unsigned int dwSN, unsigned int dwBoardID, unsigned int* dwCapability);

int testProbe(char* strLicenseCode, license *plic);

#if defined(__cplusplus)
}
#endif

#endif

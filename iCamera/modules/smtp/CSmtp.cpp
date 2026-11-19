////////////////////////////////////////////////////////////////////////////////
// Original class CFastSmtp written by 
// christopher w. backen <immortal@cox.net>
// More details at: http://www.codeproject.com/KB/IP/zsmtp.aspx
// 
// Modifications introduced by Jakub Piwowarczyk:
// 1. name of the class and functions
// 2. new functions added: SendData,ReceiveData and more
// 3. authentication added
// 4. attachments added
// 5 .comments added
// 6. DELAY_IN_MS removed (no delay during sending the message)
// 7. non-blocking mode
// More details at: http://www.codeproject.com/KB/mcpp/CSmtp.aspx
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// SSL/TLS support added by John Tang by making use of OpenSSL: http://www.openssl.org/ 
// More details at: http://www.codeproject.com/KB/IP/smtp_ssl.aspx
//
// PLAIN, CRAM-MD5 and DIGESTMD5 authentication added by David Johns
//
// Revision History:
// - Version 2.4: Updated with fixes reported as of 22 Oct 2015
//   > Fixed issues with files being left opened and buffer not being deleted if an error occurs as discussed here: http://www.codeproject.com/Messages/4651730/Re-File-attachment.aspx
//       - Thanks to Josep Sol?
//     > Fixed issue with opening attachments as discussed here: http://www.codeproject.com/Messages/4640325/File-path-mistakenly-ommitted-from-file-name-when-.aspx
//       - Thanks to Graham
//     > Fixed potential memory leak as discussed here: http://www.codeproject.com/Messages/5010012/Memory-leaks.aspx
//       - Thanks to LahPo
//     > Made total message size limit larger as recommended here: http://stackoverflow.com/questions/22426686/csmtp-wont-send-an-e-mail-attachment-but-without-the-attachment-it-works-fine/28333737#28333737
//       - Thanks to Stanislav
//     > Fixed an issue with incomplete attachment file paths as discussed here: http://www.codeproject.com/Messages/5127588/Re-Attachment-does-not-come.aspx
//       - Thanks to Member 11508846 and Member 11887128
// - Version 2.3: Updated with fixes reported as of 17 Aug 2013
//     > Removed Bcc header so that recipients don't see who it was Bcc'd to as discussed here: http://www.codeproject.com/Messages/4633562/Bcc-and-mail-header.aspx
//       - Thanks to o15s19
//     > Fixed problem with attaching files that have unicode or reserved character filenames as discussed here: http://www.codeproject.com/Messages/4610174/Re-About-snprintf-FileName-255-Attachments-FileId-.aspx
//       - Thanks to uni_gauldoth
//     > Improved the method used for checking attachment file sizes as discussed here: http://www.codeproject.com/Messages/4562481/retreiving-file-size-for-attachments.aspx
//       - Thanks to GKarRacer
//     > Added #include <unistd.h> for linux compiles, which was required for gethostname as discussed here: http://www.codeproject.com/Messages/4551908/Works-on-Linux-CSmtp-cpp-needed-sharpinclude-unist.aspx
//       - Thanks to jim fred
// - Version 2.2: Updated with fixes reported as of 6 May 2013
//     > Fixed check on MsgBody.size() as discussed here: http://www.codeproject.com/Messages/4555663/Incorrect-range-check.aspx
//       - Thanks to GKarRacer
//     > Moved memory allocation and checking if attachments could be opened to before the MAIL command is
//       issued to avoid throwing errors in a place where you can't terminate the connection gracefully without
//       the email being sent corrupted
//     > Changed all sprintf calls to snprintf to add greater security. #define'd snprintf to sprintf_s for
//       MSVC.  Also changed all strcpy to snprintf since that is the only way to use a secure function that
//       is portable between standard C and MSVC since MS re-ordered the arguments between strcpy and
//       strcpy_s
//     > Fixed issue with SayQuit that could lead to infinite loop discussed here: http://www.codeproject.com/Messages/4451901/exception-in-SayQuit-could-lead-to-infinite-loop.aspx
//       - Thanks to jcyangzh!
//     > Fixed issue with AUTH PLAIN implementation discussed here: http://www.codeproject.com/Messages/4433069/looks-like-a-bug-in-plain-auth.aspx
//       - Thanks to sbrytskyy!
// - Version 2.1: Updated with fixes reported as of 26 Mar 2012
//     > Fixed issue in main.cpp with referring to USE_TLS in the wrong scope discussed here: http://www.codeproject.com/Messages/4151405/Re-USE_SSL-no-member-of-CSmtp.aspx
//       - Thanks to Alan P Brown!
//     > Added modifications to allow it to compile in Debian Linux discussed here: http://www.codeproject.com/Messages/4132697/linux-port-patch.aspx
//       - Thanks to Oleg Dolgov!
//     > Added ability to change the character set, inspired by this post: http://www.codeproject.com/Messages/4238701/Re-The-subject-contains-the-Chinese-letters-could-.aspx
//       - Thanks to LeonHuang0726 and John TWC for the suggestion!
//     > Added ability to request a read receipt by calling SetReadReceipt as proposed here: http://www.codeproject.com/Messages/3938944/Disposition-Notification-To.aspx
//       - Thanks to Gospa for the suggestion!
//     > Added check for Linux when adding paths of attachments in the MIME header as suggested here: http://www.codeproject.com/Messages/4357144/portability-bug-w-attachment-name.aspx
//       - Thanks to Spike!
//     > Switched method of setting private std::string variables to use the = operator as suggested here: http://www.codeproject.com/Messages/4356937/portability-bugs-w-std-string-and-exceptions.aspx
//       - Thanks to Spike!
//     > Added SetLocalHostName function proposed here: http://www.codeproject.com/Messages/4092347/bug-fixes-GetLocalHostName-Send.aspx
//       - Thanks to jerko!
//     > Added the modifications to allow it to compile in Linux described here: http://www.codeproject.com/Messages/3878620/My-vote-of-5.aspx
//       - Thanks to korisk!
//     > Added the fix that corrects behavior when m_sNameFrom is empty described here: http://www.codeproject.com/Messages/4196071/Bug-Mail-sent-by-mail-domain-com.aspx
//       - Thanks to agenua.grupoi68!
// - Version 2.0: Updated to all fixes reported as of 23 Jun 2011:
//     > Added the m_bAuthenticate member variable to be able to disable authentication
//       even though it may be supported by the server. It defaults to true so if it is
//       not set the library will act as it would have before the addition.
//     > Added the ability to pass the security type, m_type, the new m_Authenticate flag,
//       the login and password into the ConnectRemoteServer function. If these new arguments
//       are not included in the call the function will work as it did before.
//     > Added the ability to pass the new m_Authenticate flag into the SetSMTPServer function.
//       If not provided, the function will act as it would before the addition.
//     > Added fix described here: http://www.codeproject.com/Messages/3681792/Bug-when-reading-answer.aspx
//       - Thanks to Martin Kjallman!
//     > Added fixes described here: http://www.codeproject.com/Messages/3707662/Mistakes.aspx
//       - Thanks to Karpov Andrey!
//     > Added fixes described here: http://www.codeproject.com/Messages/3587166/Re-Possible-Solution-To-Misc-EHLO-Errors.aspx
//       - Thanks to Jakub Piwowarczyk!
// - Version 1.9: Started with Revion 6 in code project http://www.codeproject.com/script/Articles/ListVersions.aspx?aid=98355
////////////////////////////////////////////////////////////////////////////////

#include "CSmtp.h"
#include "base64.h"
#include <cassert>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define CSMTP_DEBUG_INFO 1
#ifdef CSMTP_DEBUG_INFO
#define CSMTP_DEBUG  printf("[%s %s %d]:",__FILE__,__FUNCTION__,__LINE__);printf
#else
#define CSMTP_DEBUG(x...) 
#endif

#ifndef LINUX
//Add "openssl-0.9.8l\out32" to Additional Library Directories
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libeay32.lib")
#endif

#define MAXWORDLEN	     (1024)
#define ISOCTAL(c)       ((c) >= '0' && (c) < '8')



/* Allow SSL_write(..., n) to return r with 0 < r < n (i.e. report success
 * when just a single record has been written): */
/* Make it possible to retry SSL_write() with changed buffer location
 * (buffer contents must stay the same!); this is not the default to avoid
 * the misconception that non-blocking SSL_write() behaves like
 * non-blocking write(): */
/* Never bother the application with retries if the transport
 * is blocking: */
/* Don't attempt to automatically build certificate chain */

#if 0
Command_Entry command_list[] = 
{
	{command_INIT,          0,     5*60,  220, ECSmtp::SERVER_NOT_RESPONDING},
	{command_EHLO,          5*60,  5*60,  250, ECSmtp::COMMAND_EHLO},
	{command_AUTHPLAIN,     5*60,  5*60,  235, ECSmtp::COMMAND_AUTH_PLAIN},
	{command_AUTHLOGIN,     5*60,  5*60,  334, ECSmtp::COMMAND_AUTH_LOGIN},
	{command_AUTHCRAMMD5,   5*60,  5*60,  334, ECSmtp::COMMAND_AUTH_CRAMMD5},
	{command_AUTHDIGESTMD5, 5*60,  5*60,  334, ECSmtp::COMMAND_AUTH_DIGESTMD5},
	{command_DIGESTMD5,     5*60,  5*60,  335, ECSmtp::COMMAND_DIGESTMD5},
	{command_USER,          5*60,  5*60,  334, ECSmtp::UNDEF_XYZ_RESPONSE},
	{command_PASSWORD,      5*60,  5*60,  235, ECSmtp::BAD_LOGIN_PASS},
	{command_MAILFROM,      5*60,  5*60,  250, ECSmtp::COMMAND_MAIL_FROM},
	{command_RCPTTO,        5*60,  5*60,  250, ECSmtp::COMMAND_RCPT_TO},
	{command_DATA,          5*60,  2*60,  354, ECSmtp::COMMAND_DATA},
	{command_DATABLOCK,     3*60,  0,     0,   ECSmtp::COMMAND_DATABLOCK},	// Here the valid_reply_code is set to zero because there are no replies when sending data blocks
	{command_DATAEND,       3*60,  10*60, 250, ECSmtp::MSG_BODY_ERROR},
	{command_QUIT,          5*60,  5*60,  221, ECSmtp::COMMAND_QUIT},
	{command_STARTTLS,      5*60,  5*60,  220, ECSmtp::COMMAND_EHLO_STARTTLS}
};
#else
Command_Entry command_list[] = 
{
	{command_INIT,          0,    	10,  220, ECSmtp::SERVER_NOT_RESPONDING},
	{command_EHLO,          10,  10,  250, ECSmtp::COMMAND_EHLO},
	{command_AUTHPLAIN,     10,  10,  235, ECSmtp::COMMAND_AUTH_PLAIN},
	{command_AUTHLOGIN,     10,  10,  334, ECSmtp::COMMAND_AUTH_LOGIN},
	{command_AUTHCRAMMD5,   10,  10,  334, ECSmtp::COMMAND_AUTH_CRAMMD5},
	{command_AUTHDIGESTMD5, 10,  10,  334, ECSmtp::COMMAND_AUTH_DIGESTMD5},
	{command_DIGESTMD5,     10,  10,  335, ECSmtp::COMMAND_DIGESTMD5},
	{command_USER,          10,  10,  334, ECSmtp::UNDEF_XYZ_RESPONSE},
	{command_PASSWORD,      10,  10,  235, ECSmtp::BAD_LOGIN_PASS},
	{command_MAILFROM,      10,  10,  250, ECSmtp::COMMAND_MAIL_FROM},
	{command_RCPTTO,        10,  10,  250, ECSmtp::COMMAND_RCPT_TO},
	{command_DATA,          10,  10,  354, ECSmtp::COMMAND_DATA},
	{command_DATABLOCK,     10,  0,   0,   ECSmtp::COMMAND_DATABLOCK},	// Here the valid_reply_code is set to zero because there are no replies when sending data blocks
	{command_DATAEND,       20,  20, 250, ECSmtp::MSG_BODY_ERROR},
	{command_QUIT,          10,  10,  221, ECSmtp::COMMAND_QUIT},
	{command_STARTTLS,      10,  10,  220, ECSmtp::COMMAND_EHLO_STARTTLS}
};
#endif
Command_Entry* FindCommandEntry(SMTP_COMMAND command)
{
	Command_Entry* pEntry = NULL;
	for(size_t i = 0; i < sizeof(command_list)/sizeof(command_list[0]); ++i)
	{
		if(command_list[i].command == command)
		{
			pEntry = &command_list[i];
			break;
		}
	}
	assert(pEntry != NULL);
	return pEntry;
}

#define SMTP_HOST_ADDR_NUM (4)
static int parse_host_to_ip(const char *host, char pcServerAddr[SMTP_HOST_ADDR_NUM][256], int *hostnum) 
{
    struct addrinfo hints;
    struct addrinfo *result, *result_pointer;
    int ret;
	char hostname[256] = "";
	int	iRetVal = -1;
	
    /* obtaining address matching host */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;  /* any protocol */

    ret = getaddrinfo(host, NULL, &hints, &result);
    if (ret != 0)
	{
        fprintf(stderr, "getaddrinfo: %s host=[%s]\n", gai_strerror(ret), host);
        return -1;
    }

	*hostnum = 0;
    /* traverse the returned list and output the ip addresses */
    for (result_pointer = result; result_pointer != NULL; result_pointer = result_pointer->ai_next) 
	{
        ret = getnameinfo(result_pointer->ai_addr, result_pointer->ai_addrlen, hostname, sizeof(hostname), NULL, 0, NI_NUMERICHOST);
        if (ret != 0) 
		{
            fprintf(stderr, "error in getnameinfo: %s \n", gai_strerror(ret));
            continue;
        } 
		else 
        {
            printf("******smtp IP: %s \n", hostname);
			iRetVal = 0;

			snprintf(pcServerAddr[*hostnum], strlen(hostname)+1, "%s", hostname);
			(*hostnum)++;
			
			if (*hostnum == SMTP_HOST_ADDR_NUM)
			{
				break;
			}
        }
    }
	if (NULL != result)
	{
    	freeaddrinfo(result);
	}
	
    return iRetVal;
}

// A simple string match
bool IsKeywordSupported(const char* response, const char* keyword)
{
	assert(response != NULL && keyword != NULL);
	if(response == NULL || keyword == NULL)
		return false;
	int res_len = strlen(response);
	int key_len = strlen(keyword);
	if(res_len < key_len)
		return false;
	int pos = 0;
	for(; pos < res_len - key_len + 1; ++pos)
	{
		if(_strnicmp(keyword, response+pos, key_len) == 0)
		{
			if(pos > 0 &&
				(response[pos - 1] == '-' ||
				 response[pos - 1] == ' ' ||
				 response[pos - 1] == '='))
			{
				if(pos+key_len < res_len)
				{
					if(response[pos+key_len] == ' ' ||
					   response[pos+key_len] == '=')
					{
						return true;
					}
					else if(pos+key_len+1 < res_len)
					{
						if(response[pos+key_len] == '\r' &&
						   response[pos+key_len+1] == '\n')
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

unsigned char* CharToUnsignedChar(const char *strIn)
{
	unsigned char *strOut;

	unsigned long length,
		          i;


	length = strlen(strIn);

	strOut = new unsigned char[length+1];
	if(!strOut) return NULL;

	for(i=0; i<length; i++) strOut[i] = (unsigned char) strIn[i];
	strOut[length]='\0';

	return strOut;
}

/* Get one line info from text file */
static ssize_t ReadLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    /* # of bytes fetched by last read()*/
    size_t  totRead;                    /* Total bytes read so far*/
    char   *buf;
    char    ch;
 
    if (n <= 0 || buffer == NULL)
    {
        errno = EINVAL;
        return -1;
    }
 
    buf = (char *)buffer;
    totRead = 0;
    for(;;)
    {
        numRead = read(fd, &ch, 1);
        if (numRead == -1)
        {
            if (errno == EINTR)         /*Interrupted ----> restart read()*/
                continue;
            else
                return -1;              /* some other error*/
        }
        else if (numRead == 0)
        {                               /* EOF*/
            if (totRead == 0)           /* No bytes read so far, return 0*/
                return 0;
            else                        /* Some bytes read so far, add '\0'*/
                break;
        }
        else
        {                       
            if (totRead < n-1)
            {                           /* Discard > (n-1) bytes */
                totRead++;
                if (totRead == n-2)     /* 让readLine最多能读取n-1个字符，不包括null字符，并且不丢弃字节*/
                    break;
                *buf++ = ch;
            }
 
            if (ch == '\n')
            {
                break;
            }
        }
    }
 
    *buf = '\0';
    
    return totRead;
}


CSmtp *CSmtp::m_pCSmtp = NULL;

/* 获取对象实例化 */
CSmtp *CSmtp::GetInstance()
{
    if (NULL == m_pCSmtp)
    {
        m_pCSmtp = new CSmtp;
        if
 (NULL == m_pCSmtp)
        {
            printf("Get CSmtp Instance Fail!\n");
            return NULL;
        }
    }
    return (CSmtp *)m_pCSmtp;
}

/* 释放实例化指针 */
int CSmtp::ReleaseInstance()
{
    if (NULL != m_pCSmtp)
    {
        delete m_pCSmtp;
        m_pCSmtp = NULL;
    }

	return (int) 0;
}


////////////////////////////////////////////////////////////////////////////////
//        NAME: CSmtp
// DESCRIPTION: 构造函数
//   ARGUMENTS: none
// USES GLOBAL: none
// MODIFIES GL: m_iXPriority, m_iSMTPSrvPort, RecvBuf, SendBuf
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////

CSmtp::CSmtp()
{
	hSocket = INVALID_SOCKET;
	m_bConnected = false;
	m_iXPriority = XPRIORITY_NORMAL;
	m_iSMTPSrvPort = 0;
	m_bAuthenticate = true;
	
	try
	{

#ifndef LINUX
		// Initialize WinSock
		WSADATA wsaData;
		WORD wVer = MAKEWORD(2,2);    
		if (WSAStartup(wVer,&wsaData) != NO_ERROR)
			throw ECSmtp(ECSmtp::WSA_STARTUP);
		if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
		{
			WSACleanup();
			throw ECSmtp(ECSmtp::WSA_VER);
		}
#endif

		char hostname[255];
		if(gethostname((char *) &hostname, 255) == SOCKET_ERROR) throw ECSmtp(ECSmtp::WSA_HOSTNAME);
		m_sLocalHostName = hostname;

		if((RecvBuf = new char[BUFFER_SIZE]) == NULL)
			throw ECSmtp(ECSmtp::LACK_OF_MEMORY);

		if((SendBuf = new char[BUFFER_SIZE]) == NULL)
			throw ECSmtp(ECSmtp::LACK_OF_MEMORY);

		m_type = NO_SECURITY;
		m_ctx = NULL;
		m_ssl = NULL;
		m_bHTML = false;
		m_bReadReceipt = false;

		m_sCharSet = "US-ASCII";

	}
	catch(const ECSmtp &e)
	{
		CSMTP_DEBUG("new CSMTP failed! errorNum: %d \n",(int)e.GetErrorNum());
	}
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CSmtp
// DESCRIPTION: 析构函数
//   ARGUMENTS: none
// USES GLOBAL: RecvBuf, SendBuf
// MODIFIES GL: RecvBuf, SendBuf
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
CSmtp::~CSmtp()
{
	if(m_bConnected) DisconnectRemoteServer();

	if(SendBuf)
	{
		delete[] SendBuf;
		SendBuf = NULL;
	}
	if(RecvBuf)
	{
		delete[] RecvBuf;
		RecvBuf = NULL;
	}

	CleanupOpenSSL();

#ifndef LINUX
	WSACleanup();
#endif

    ReleaseInstance();
    
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: AddAttachment
// DESCRIPTION: 添加一个新的附件
//   ARGUMENTS: const char *Path - name of attachment added
// USES GLOBAL: Attachments
// MODIFIES GL: Attachments
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::AddAttachment(const char *Path)
{
	assert(Path);
	Attachments.insert(Attachments.end(), Path);
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: AddRecipient
// DESCRIPTION: 添加一个新的收件人
//   ARGUMENTS: const char *email - mail of the recipient
//              const char *name - name of the recipient
// USES GLOBAL: Recipients
// MODIFIES GL: Recipients
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
int CSmtp::AddRecipient(const char *email, const char *name)
{	
	/*
		if(!email)
			throw ECSmtp(ECSmtp::UNDEF_RECIPIENT_MAIL);
	*/
	if(!email)
	{
		return (int)ECSmtp::UNDEF_RECIPIENT_MAIL ;
	}
	Recipient recipient;
	recipient.Mail = email;
	if(name!=NULL) recipient.Name = name;
	else recipient.Name.empty();

	Recipients.insert(Recipients.end(), recipient);   
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: AddCCRecipient
// DESCRIPTION: 添加一个新的抄送收件人
//   ARGUMENTS: const char *email - mail of the cc-recipient
//              const char *name - name of the ccc-recipient
// USES GLOBAL: CCRecipients
// MODIFIES GL: CCRecipients
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
int CSmtp::AddCCRecipient(const char *email, const char *name)
{
/*
	if(!email)
		throw ECSmtp(ECSmtp::UNDEF_RECIPIENT_MAIL);
*/
	if(!email)
	{
		return (int)ECSmtp::UNDEF_RECIPIENT_MAIL;
	}
	Recipient recipient;
	recipient.Mail = email;
	if(name!=NULL) recipient.Name = name;
	else recipient.Name.empty();

	CCRecipients.insert(CCRecipients.end(), recipient);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: AddBCCRecipient
// DESCRIPTION: 添加一个新的密送收件人
//   ARGUMENTS: const char *email - mail of the bcc-recipient
//              const char *name - name of the bccc-recipient
// USES GLOBAL: BCCRecipients
// MODIFIES GL: BCCRecipients
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
int CSmtp::AddBCCRecipient(const char *email, const char *name)
{	
/*
	if(!email)
		throw ECSmtp(ECSmtp::UNDEF_RECIPIENT_MAIL);
*/
	if(!email)
	{
		return (int)ECSmtp::UNDEF_RECIPIENT_MAIL;
	}
	Recipient recipient;
	recipient.Mail = email;
	if(name!=NULL) recipient.Name = name;
	else recipient.Name.empty();

	BCCRecipients.insert(BCCRecipients.end(), recipient);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: AddMsgLine
// DESCRIPTION: 在消息中添加新的行。
//   ARGUMENTS: const char *Text - text of the new line
// USES GLOBAL: MsgBody
// MODIFIES GL: MsgBody
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::AddMsgLine(const char* Text)
{
	MsgBody.insert(MsgBody.end(), Text);
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DelMsgLine
// DESCRIPTION: 在文本消息中删除指定的行
//   ARGUMENTS: unsigned int Line - line to be delete
// USES GLOBAL: MsgBody
// MODIFIES GL: MsgBody
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
int CSmtp::DelMsgLine(unsigned int Line)
{
/*
	if(Line >= MsgBody.size())
		throw ECSmtp(ECSmtp::OUT_OF_MSG_RANGE);
*/
	if(Line >= MsgBody.size())
	{
		return (int)ECSmtp::OUT_OF_MSG_RANGE;
	}

	MsgBody.erase(MsgBody.begin()+Line);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: AddMsgFile
// DESCRIPTION: 将文本信息里面的内容发送到邮件正文里面
//   ARGUMENTS: const char *TextFilename - 存放正文的文件
// USES GLOBAL: MsgBody
// MODIFIES GL: MsgBody
//     RETURNS: void
//      AUTHOR: fancong
// AUTHOR/DATE: fancong 2016-9-22
////////////////////////////////////////////////////////////////////////////////
int CSmtp::AddMsgFile(const char *TextFilename)
{    
        int fd = open(TextFilename, O_RDONLY);
		/*
        if (-1 == fd)
        {
            throw ECSmtp(ECSmtp::TXT_FILE_NOT_EXIST);
        }*/
		if (-1 == fd)
		{
			return (int)ECSmtp::TXT_FILE_NOT_EXIST;
		}
    
        char LineMsg[128] = {0};
        int  ret = 0;

        while(1)
        {
            memset(LineMsg, 0, sizeof(LineMsg));
            ret = ReadLine(fd, LineMsg, sizeof(LineMsg));
            if (-1==ret || 0==ret) break;
            LineMsg[strlen(LineMsg)-1] = '\0';
            AddMsgLine(LineMsg);
        }
        
        close(fd);
        fd = -1;
		return 0;
	
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DelRecipients
// DESCRIPTION: 删除所有收件人
//   ARGUMENTS: void
// USES GLOBAL: Recipients
// MODIFIES GL: Recipients
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::DelRecipients()
{
	Recipients.clear();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DelBCCRecipients
// DESCRIPTION: 删除所有密送收件人
//   ARGUMENTS: void
// USES GLOBAL: BCCRecipients
// MODIFIES GL: BCCRecipients
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::DelBCCRecipients()
{
	BCCRecipients.clear();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DelCCRecipients
// DESCRIPTION: 删除所有抄送收件人
//   ARGUMENTS: void
// USES GLOBAL: CCRecipients
// MODIFIES GL: CCRecipients
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::DelCCRecipients()
{
	CCRecipients.clear();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DelMsgLines
// DESCRIPTION: 删除消息文本
//   ARGUMENTS: void
// USES GLOBAL: MsgBody
// MODIFIES GL: MsgBody
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::DelMsgLines()
{
	MsgBody.clear();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DelAttachments
// DESCRIPTION: 删除所有附件
//   ARGUMENTS: void
// USES GLOBAL: Attchments
// MODIFIES GL: Attachments
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::DelAttachments()
{
	Attachments.clear();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: ModMsgLine
// DESCRIPTION: 修改消息正文的特定行
//   ARGUMENTS: unsigned int Line - the line number to modify
//              const char* Text - the new contents of the line
// USES GLOBAL: MsgBody
// MODIFIES GL: MsgBody
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
int  CSmtp::ModMsgLine(unsigned int Line,const char* Text)
{
	if(Text)
	{
		/*if(Line >= MsgBody.size())
			throw ECSmtp(ECSmtp::OUT_OF_MSG_RANGE);
			*/
		if(Line >= MsgBody.size())
			return (int)ECSmtp::OUT_OF_MSG_RANGE;
		MsgBody.at(Line) = std::string(Text);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: ClearMessage
// DESCRIPTION: 清除收件人和消息
//   ARGUMENTS: none
//     RETURNS: none
//      AUTHOR: David Johns
// AUTHOR/DATE: DRJ 2013-05-20
////////////////////////////////////////////////////////////////////////////////
void CSmtp::ClearMessage()
{
	DelRecipients();
	DelBCCRecipients();
	DelCCRecipients();
	DelAttachments();
	DelMsgLines();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Send
// DESCRIPTION: 发送Email
//   ARGUMENTS: none
// USES GLOBAL: m_sSMTPSrvName, m_iSMTPSrvPort, SendBuf, RecvBuf, m_sLogin,
//              m_sPassword, m_sMailFrom, Recipients, CCRecipients,
//              BCCRecipients, m_sMsgBody, Attachments, 
// MODIFIES GL: SendBuf 
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
int CSmtp::Send()
{
	unsigned int i,rcpt_count,res,FileId;
	int erro =0;
	char *FileBuf = NULL;
	FILE* hFile = NULL;

	
	unsigned long int FileSize,TotalSize,MsgPart;
	string FileName,EncodedFileName;
	string::size_type pos;

	// ***** CONNECTING TO SMTP SERVER *****

	CSMTP_DEBUG("m_sSMTPSrvName.c_str() %s , m_iSMTPSrvPort %d, m_type %d, m_bAuthenticate %d m_sPassword %s\n",m_sSMTPSrvName.c_str(), m_iSMTPSrvPort, m_type, m_bAuthenticate,m_sPassword.c_str());

	//参数有修改，需要重新建立连接
   if(hSocket != INVALID_SOCKET)
	{
		/*
		if((erro=DisconnectRemoteServer()))
		{
			return erro;
		}
		*/
		//不考虑disconect 错误，避免上一次错误导致本次disconect出错
		DisconnectRemoteServer();
	}
	if ((erro=ConnectRemoteServer()))
	{	
		return erro;
   	}

	try{
		//Allocate memory
		if((FileBuf = new char[55]) == NULL)
			throw ECSmtp(ECSmtp::LACK_OF_MEMORY);

		//Check that any attachments specified can be opened
		TotalSize = 0;
		for(FileId=0;FileId<Attachments.size();FileId++)
		{
			// opening the file:
			hFile = fopen(Attachments[FileId].c_str(), "rb");
			if(hFile == NULL)
			{
				throw ECSmtp(ECSmtp::FILE_NOT_EXIST);
			}
			// checking file size:
			fseek(hFile, 0, SEEK_END);
			FileSize = ftell(hFile);
			TotalSize += FileSize;

			// sending the file:
			if(TotalSize/1024 > MSG_SIZE_IN_MB*1024)
				throw ECSmtp(ECSmtp::MSG_TOO_BIG);

			fclose(hFile);
			hFile=NULL;
		}

		// ***** SENDING E-MAIL *****
		
		// MAIL <SP> FROM:<reverse-path> <CRLF>
		if(!m_sMailFrom.size())
			throw ECSmtp(ECSmtp::UNDEF_MAIL_FROM);
		Command_Entry* pEntry = FindCommandEntry(command_MAILFROM);
		snprintf(SendBuf, BUFFER_SIZE, "MAIL FROM:<%s>\r\n", m_sMailFrom.c_str());
		SendData(pEntry);
		ReceiveResponse(pEntry);

		// RCPT <SP> TO:<forward-path> <CRLF>
		if(!(rcpt_count = Recipients.size()))
			throw ECSmtp(ECSmtp::UNDEF_RECIPIENTS);
		pEntry = FindCommandEntry(command_RCPTTO);
		for(i=0;i<Recipients.size();i++)
		{
			snprintf(SendBuf, BUFFER_SIZE, "RCPT TO:<%s>\r\n", (Recipients.at(i).Mail).c_str());
			SendData(pEntry);
			ReceiveResponse(pEntry);
		}

		for(i=0;i<CCRecipients.size();i++)
		{
			snprintf(SendBuf, BUFFER_SIZE, "RCPT TO:<%s>\r\n", (CCRecipients.at(i).Mail).c_str());
			SendData(pEntry);
			ReceiveResponse(pEntry);
		}

		for(i=0;i<BCCRecipients.size();i++)
		{
			snprintf(SendBuf, BUFFER_SIZE, "RCPT TO:<%s>\r\n", (BCCRecipients.at(i).Mail).c_str());
			SendData(pEntry);
			ReceiveResponse(pEntry);
		}
		
		pEntry = FindCommandEntry(command_DATA);
		// DATA <CRLF>
		snprintf(SendBuf, BUFFER_SIZE, "DATA\r\n");
		SendData(pEntry);
		ReceiveResponse(pEntry);
		
		pEntry = FindCommandEntry(command_DATABLOCK);
		// send header(s)
		FormatHeader(SendBuf);
		SendData(pEntry);

		// send text message
		if(GetMsgLines())
		{
			for(i=0;i<GetMsgLines();i++)
			{
				snprintf(SendBuf, BUFFER_SIZE, "%s\r\n",GetMsgLineText(i));
				SendData(pEntry);
			}
		}
		else
		{
			snprintf(SendBuf, BUFFER_SIZE, "%s\r\n"," ");
			SendData(pEntry);
		}

		// next goes attachments (if they are)
		for(FileId=0;FileId<Attachments.size();FileId++)
		{
#ifndef LINUX
			pos = Attachments[FileId].find_last_of("\\");
#else
			pos = Attachments[FileId].find_last_of("/");
#endif
			if(pos == string::npos) FileName = Attachments[FileId];
			else FileName = Attachments[FileId].substr(pos+1);

            //RFC 2047 - Use UTF-8 charset,base64 encode.
            EncodedFileName = "=?UTF-8?B?";
            EncodedFileName += base64_encode((unsigned char *) FileName.c_str(), FileName.size());
            EncodedFileName += "?=";

			snprintf(SendBuf, BUFFER_SIZE, "--%s\r\n", BOUNDARY_TEXT);
			strcat(SendBuf, "Content-Type: application/x-msdownload; name=\"");
			strcat(SendBuf, EncodedFileName.c_str());
			strcat(SendBuf, "\"\r\n");
			strcat(SendBuf, "Content-Transfer-Encoding: base64\r\n");
			strcat(SendBuf, "Content-Disposition: attachment; filename=\"");
			strcat(SendBuf, EncodedFileName.c_str());
			strcat(SendBuf, "\"\r\n");
			strcat(SendBuf, "\r\n");

			SendData(pEntry);

			// opening the file:
			hFile = fopen(Attachments[FileId].c_str(), "rb");
			if(hFile == NULL)
				throw ECSmtp(ECSmtp::FILE_NOT_EXIST);
			
			// get file size:
			fseek(hFile, 0, SEEK_END);
			FileSize = ftell(hFile);
			fseek (hFile,0,SEEK_SET);

			MsgPart = 0;
			for(i=0;i<FileSize/54+1;i++)
			{
				res = fread(FileBuf,sizeof(char),54,hFile);
				MsgPart ? strcat(SendBuf,base64_encode(reinterpret_cast<const unsigned char*>(FileBuf),res).c_str())
							: strcpy(SendBuf,base64_encode(reinterpret_cast<const unsigned char*>(FileBuf),res).c_str());
				strcat(SendBuf,"\r\n");
				MsgPart += res + 2;
				if(MsgPart >= BUFFER_SIZE/2)
				{ // sending part of the message
					MsgPart = 0;
					SendData(pEntry); // FileBuf, FileName, fclose(hFile);
				}
			}
			if(MsgPart)
			{
				SendData(pEntry); // FileBuf, FileName, fclose(hFile);
			}
			fclose(hFile);
			hFile=NULL;
		}
		delete[] FileBuf;
		FileBuf=NULL;
		
		// sending last message block (if there is one or more attachments)
		if(Attachments.size())
		{
			snprintf(SendBuf, BUFFER_SIZE, "\r\n--%s--\r\n",BOUNDARY_TEXT);
			SendData(pEntry);
		}
		
		pEntry = FindCommandEntry(command_DATAEND);
		// <CRLF> . <CRLF>
		snprintf(SendBuf, BUFFER_SIZE, "\r\n.\r\n");
		SendData(pEntry);
		ReceiveResponse(pEntry);
		return 0;
	}
	catch(const ECSmtp& e)
	{
		if(hFile) fclose(hFile);
		if (FileBuf) 
		{
			delete[] FileBuf;
			FileBuf = NULL;
		}
		DisconnectRemoteServer();
		return (int)e.GetErrorNum();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: ConnectRemoteServer
// DESCRIPTION: 连接到远程服务器. 
//   ARGUMENTS: const char *server - service name
//              const unsigned short port - service port
// USES GLOBAL: m_pcSMTPSrvName, m_iSMTPSrvPort, SendBuf, RecvBuf, m_pcLogin,
//              m_pcPassword, m_pcMailFrom, Recipients, CCRecipients,
//              BCCRecipients, m_pcMsgBody, Attachments, 
// MODIFIES GL: m_oError 
//     RETURNS: socket of the remote service
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
int CSmtp::ConnectRemoteServer()
{
	CSMTP_DEBUG("szServer %s,nPort %d,securityType %d,authenticate %d,login %s,password %s\n",m_sSMTPSrvName.c_str(),m_iSMTPSrvPort, m_type,m_bAuthenticate,m_sLogin.c_str(),m_sPassword.c_str());
	unsigned short nPort = 0;
	LPSERVENT lpServEnt;
	SOCKADDR_IN sockAddr;
	char		acServerAddr[SMTP_HOST_ADDR_NUM][256];
	unsigned long ul = 1;
	fd_set fdwrite,fdexcept;
	timeval timeout;
	int res = 0;
	Command_Entry* pEntry  = NULL;
	int host_count = 0;
	int try_count = 0;
	char str[32];
	try
	{
		if (m_iSMTPSrvPort != 0)
		{
			nPort = htons(m_iSMTPSrvPort);
		}
		else
		{
			lpServEnt = getservbyname("mail", 0);
			if (NULL == lpServEnt)
			{
				nPort = htons(25);
			}
			else
			{
				nPort = lpServEnt->s_port;
			}
		}
		

		res = parse_host_to_ip(m_sSMTPSrvName.c_str(), acServerAddr, &host_count);
		if (res < 0)
		{		
			throw ECSmtp(ECSmtp::WSA_INVALID_SOCKET);
			return 0;
		}
		try_count = 0;

try_again:
		timeout.tv_sec = 5+(try_count%3)*10;
		timeout.tv_usec = 0;

		hSocket = INVALID_SOCKET;
		if ((hSocket = socket(PF_INET, SOCK_STREAM,0)) == INVALID_SOCKET)
		{
			throw ECSmtp(ECSmtp::WSA_INVALID_SOCKET);
		}
		
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = nPort;
		sockAddr.sin_addr.s_addr = inet_addr(acServerAddr[try_count/3]);   
		printf("line %d address:%s\n", __LINE__, inet_ntop(AF_INET, &sockAddr.sin_addr.s_addr, str, sizeof(str)));

		// start non-blocking mode for socket:
#ifdef LINUX
		if (SOCKET_ERROR == ioctl(hSocket,FIONBIO, (unsigned long*)&ul))
#else
		if (SOCKET_ERROR == ioctlsocket(hSocket,FIONBIO, (unsigned long*)&ul))
#endif
		{
#ifdef LINUX
			close(hSocket);
#else
			closesocket(hSocket);
#endif
			throw ECSmtp(ECSmtp::WSA_IOCTLSOCKET);
		}

		if (SOCKET_ERROR == connect(hSocket,(LPSOCKADDR)&sockAddr,sizeof(sockAddr)))
		{
#ifdef LINUX
			if (errno != EINPROGRESS)
#else
			if (WSAGetLastError() != WSAEWOULDBLOCK)
#endif
			{
#ifdef LINUX
				close(hSocket);
#else
				closesocket(hSocket);
#endif
				throw ECSmtp(ECSmtp::WSA_CONNECT);
			}
		}
		else
		{
			return true;
		}

		while (true)
		{
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcept);

			FD_SET(hSocket,&fdwrite);
			FD_SET(hSocket,&fdexcept);

			if(SOCKET_ERROR == (res = select(hSocket+1, NULL, &fdwrite, &fdexcept, &timeout)))
			{
#ifdef LINUX
				close(hSocket);
#else
				closesocket(hSocket);
#endif
				throw ECSmtp(ECSmtp::WSA_SELECT);
			}
			
			if (!res)
			{
#ifdef LINUX
				close(hSocket);
#else
				closesocket(hSocket);
#endif
				try_count++;
				if ((try_count/3) < host_count)
				{
					goto try_again;
				}
				throw ECSmtp(ECSmtp::SELECT_TIMEOUT);
			}
			if (res && FD_ISSET(hSocket,&fdwrite))
			{
				break;
			}
			if (res && FD_ISSET(hSocket,&fdexcept))
			{
#ifdef LINUX
				close(hSocket);
#else
				closesocket(hSocket);
#endif
				throw ECSmtp(ECSmtp::WSA_SELECT);
			}
		} // while
		
		FD_CLR(hSocket,&fdwrite);
		FD_CLR(hSocket,&fdexcept);
		//是否使用安全连接判断		
		//SetSecurityType(USE_TLS_IFDO);
		if (USE_TLS == GetSecurityType() || USE_SSL == GetSecurityType() 
			|| USE_TLS_IFDO == GetSecurityType())
		{
			InitOpenSSL();
			if (USE_SSL == GetSecurityType())
			{
				OpenSSLConnect();
				pEntry = FindCommandEntry(command_INIT);
				ReceiveResponse(pEntry);
				SayHello();
			}
			else if (USE_TLS == GetSecurityType() || USE_TLS_IFDO == GetSecurityType())
			{
				pEntry = FindCommandEntry(command_INIT);
				ReceiveResponse(pEntry);
				SayHello();//抛出异常
				if (true == IsSupportedTls())//&& 0)
				{
					StartTls();
					SayHello();
				}
				else if (USE_TLS == GetSecurityType())
				{
					throw ECSmtp(ECSmtp::STARTTLS_NOT_SUPPORTED);
				}
				else
				{
					CleanupOpenSSL();
				}
			}
		}
		else
		{
			CleanupOpenSSL();
			pEntry = FindCommandEntry(command_INIT);
			ReceiveResponse(pEntry);
			SayHello();
		}
	
		if (m_bAuthenticate && true == IsKeywordSupported(RecvBuf, "AUTH"))
		{
			CSMTP_DEBUG("NEED AUTH\n");
			if (!m_sLogin.size())
			{
				throw ECSmtp(ECSmtp::UNDEF_LOGIN);
			}
			
			if (!m_sPassword.size())
			{
				throw ECSmtp(ECSmtp::UNDEF_PASSWORD);
			}
			
			if (true == IsKeywordSupported(RecvBuf, "LOGIN"))
			{
				CSMTP_DEBUG("NEED LOGIN\n");
				pEntry = FindCommandEntry(command_AUTHLOGIN);
				snprintf(SendBuf, BUFFER_SIZE, "AUTH LOGIN\r\n");
				SendData(pEntry);
				ReceiveResponse(pEntry);

				// send login:
				std::string encoded_login = base64_encode(reinterpret_cast<const unsigned char*>(m_sLogin.c_str()),m_sLogin.size());
				pEntry = FindCommandEntry(command_USER);
				snprintf(SendBuf, BUFFER_SIZE, "%s\r\n",encoded_login.c_str());
				SendData(pEntry);
				ReceiveResponse(pEntry);
				CSMTP_DEBUG("m_sLogin %s \n",m_sLogin.c_str());
				
				// send password:
				std::string encoded_password = base64_encode(reinterpret_cast<const unsigned char*>(m_sPassword.c_str()),m_sPassword.size());
				pEntry = FindCommandEntry(command_PASSWORD);
				snprintf(SendBuf, BUFFER_SIZE, "%s\r\n",encoded_password.c_str());
				SendData(pEntry);
				ReceiveResponse(pEntry);
				CSMTP_DEBUG("m_sPassword %s \n",m_sPassword.c_str());
			}
			else if (true == IsKeywordSupported(RecvBuf, "PLAIN"))
			{
				pEntry = FindCommandEntry(command_AUTHPLAIN);
				snprintf(SendBuf, BUFFER_SIZE, "%s^%s^%s", m_sLogin.c_str(), m_sLogin.c_str(), m_sPassword.c_str());
				unsigned int length = strlen(SendBuf);
				unsigned char *ustrLogin = CharToUnsignedChar(SendBuf);
				for (unsigned int i=0; i<length; i++)
				{
					if(ustrLogin[i]==94) ustrLogin[i]=0;
				}
				std::string encoded_login = base64_encode(ustrLogin, length);
				delete[] ustrLogin;
				ustrLogin = NULL;
				snprintf(SendBuf, BUFFER_SIZE, "AUTH PLAIN %s\r\n", encoded_login.c_str());
				SendData(pEntry);
				ReceiveResponse(pEntry);
			}
			else if (true == IsKeywordSupported(RecvBuf, "CRAM-MD5"))
			{
				pEntry = FindCommandEntry(command_AUTHCRAMMD5);
				snprintf(SendBuf, BUFFER_SIZE, "AUTH CRAM-MD5\r\n");
				SendData(pEntry);
				ReceiveResponse(pEntry);

				std::string encoded_challenge = RecvBuf;
				encoded_challenge = encoded_challenge.substr(4);
				std::string decoded_challenge = base64_decode(encoded_challenge);
				
				/////////////////////////////////////////////////////////////////////
				//test data from RFC 2195
				//decoded_challenge = "<1896.697170952@postoffice.reston.mci.net>";
				//m_sLogin = "tim";
				//m_sPassword = "tanstaaftanstaaf";
				//MD5 should produce b913a602c7eda7a495b4e6e7334d3890
				//should encode as dGltIGI5MTNhNjAyYzdlZGE3YTQ5NWI0ZTZlNzMzNGQzODkw
				/////////////////////////////////////////////////////////////////////

				unsigned char *ustrChallenge = CharToUnsignedChar(decoded_challenge.c_str());
				unsigned char *ustrPassword = CharToUnsignedChar(m_sPassword.c_str());
				if (!ustrChallenge || !ustrPassword)
				{
					throw ECSmtp(ECSmtp::BAD_LOGIN_PASSWORD);
				}
				// if ustrPassword is longer than 64 bytes reset it to ustrPassword=MD5(ustrPassword)
				int passwordLength = m_sPassword.size();
				if (passwordLength > 64)
				{
					MD5 md5password;
					md5password.update(ustrPassword, passwordLength);
					md5password.finalize();
					ustrPassword = md5password.raw_digest();
					passwordLength = 16;
				}

				//Storing ustrPassword in pads
				unsigned char ipad[65] = {0};
				unsigned char opad[65] = {0};
				memset(ipad, 0, 64);
				memset(opad, 0, 64);
				memcpy(ipad, ustrPassword, passwordLength);
				memcpy(opad, ustrPassword, passwordLength);

				// XOR ustrPassword with ipad and opad values
				for (int i = 0; i < 64; i++)
				{
					ipad[i] ^= 0x36;
					opad[i] ^= 0x5c;
				}

				//perform inner MD5
				MD5 md5pass1;
				md5pass1.update(ipad, 64);
				md5pass1.update(ustrChallenge, decoded_challenge.size());
				md5pass1.finalize();
				unsigned char *ustrResult = md5pass1.raw_digest();

				//perform outer MD5
				MD5 md5pass2;
				md5pass2.update(opad, 64);
				md5pass2.update(ustrResult, 16);
				md5pass2.finalize();
				decoded_challenge = md5pass2.hex_digest();

				delete[] ustrChallenge;
				delete[] ustrPassword;
				delete[] ustrResult;
				ustrChallenge = NULL;
				ustrPassword = NULL;
				ustrResult = NULL;

				decoded_challenge = m_sLogin + " " + decoded_challenge;
				encoded_challenge = base64_encode(reinterpret_cast<const unsigned char*>(decoded_challenge.c_str()),decoded_challenge.size());

				snprintf(SendBuf, BUFFER_SIZE, "%s\r\n", encoded_challenge.c_str());
				pEntry = FindCommandEntry(command_PASSWORD);
				SendData(pEntry);
				ReceiveResponse(pEntry);
			}
			else if (true == IsKeywordSupported(RecvBuf, "DIGEST-MD5"))
			{
				pEntry = FindCommandEntry(command_DIGESTMD5);
				snprintf(SendBuf, BUFFER_SIZE, "AUTH DIGEST-MD5\r\n");
				SendData(pEntry);
				ReceiveResponse(pEntry);

				std::string encoded_challenge = RecvBuf;
				encoded_challenge = encoded_challenge.substr(4);
				std::string decoded_challenge = base64_decode(encoded_challenge);

				/////////////////////////////////////////////////////////////////////
				//Test data from RFC 2831
				//To test jump into authenticate and read this line and the ones down to next test data section
				//decoded_challenge = "realm=\"elwood.innosoft.com\",nonce=\"OA6MG9tEQGm2hh\",qop=\"auth\",algorithm=md5-sess,charset=utf-8";
				/////////////////////////////////////////////////////////////////////
				
				//Get the nonce (manditory)
				int find = decoded_challenge.find("nonce");
				if (find < 0)
				{
					throw ECSmtp(ECSmtp::BAD_DIGEST_RESPONSE);
				}
				std::string nonce = decoded_challenge.substr(find+7);
				find = nonce.find("\"");
				if (find < 0)
				{
					throw ECSmtp(ECSmtp::BAD_DIGEST_RESPONSE);
				}
				nonce = nonce.substr(0, find);

				//Get the realm (optional)
				std::string realm;
				find = decoded_challenge.find("realm");
				if (find >= 0)
				{
					realm = decoded_challenge.substr(find+7);
					find = realm.find("\"");
					if (find < 0)
					{
						throw ECSmtp(ECSmtp::BAD_DIGEST_RESPONSE);
					}
					realm = realm.substr(0, find);
				}

				//Create a cnonce
				char cnonce[17] = {0};
				char nc[9] = {0};
				snprintf(cnonce, 17, "%x", (unsigned int) time(NULL));

				//Set nonce count
				snprintf(nc, 9, "%08d", 1);

				//Set QOP
				std::string qop = "auth";

				//Get server address and set uri
				//Skip this step during test
#ifdef __linux__
				socklen_t len = 0;
#else
				int len = 0;
#endif
				struct sockaddr_storage addr = {0};
				len = sizeof addr;
				if (!getpeername(hSocket, (struct sockaddr*)&addr, &len))
				{
					throw ECSmtp(ECSmtp::BAD_SERVER_NAME);
				}
				struct sockaddr_in *s = (struct sockaddr_in *)&addr;
				std::string uri = inet_ntoa(s->sin_addr);
				uri = "smtp/" + uri;

				/////////////////////////////////////////////////////////////////////
				//test data from RFC 2831
				//m_sLogin = "chris";
				//m_sPassword = "secret";
				//snprintf(cnonce, 17, "OA6MHXh6VqTrRk");
				//uri = "imap/elwood.innosoft.com";
				//Should form the response:
				//    charset=utf-8,username="chris",
				//    realm="elwood.innosoft.com",nonce="OA6MG9tEQGm2hh",nc=00000001,
				//    cnonce="OA6MHXh6VqTrRk",digest-uri="imap/elwood.innosoft.com",
				//    response=d388dad90d4bbd760a152321f2143af7,qop=auth
				//This encodes to:
				//    Y2hhcnNldD11dGYtOCx1c2VybmFtZT0iY2hyaXMiLHJlYWxtPSJlbHdvb2
				//    QuaW5ub3NvZnQuY29tIixub25jZT0iT0E2TUc5dEVRR20yaGgiLG5jPTAw
				//    MDAwMDAxLGNub25jZT0iT0E2TUhYaDZWcVRyUmsiLGRpZ2VzdC11cmk9Im
				//    ltYXAvZWx3b29kLmlubm9zb2Z0LmNvbSIscmVzcG9uc2U9ZDM4OGRhZDkw
				//    ZDRiYmQ3NjBhMTUyMzIxZjIxNDNhZjcscW9wPWF1dGg=
				/////////////////////////////////////////////////////////////////////

				//Calculate digest response
				unsigned char *ustrRealm = CharToUnsignedChar(realm.c_str());
				unsigned char *ustrUsername = CharToUnsignedChar(m_sLogin.c_str());
				unsigned char *ustrPassword = CharToUnsignedChar(m_sPassword.c_str());
				unsigned char *ustrNonce = CharToUnsignedChar(nonce.c_str());
				unsigned char *ustrCNonce = CharToUnsignedChar(cnonce);
				unsigned char *ustrUri = CharToUnsignedChar(uri.c_str());
				unsigned char *ustrNc = CharToUnsignedChar(nc);
				unsigned char *ustrQop = CharToUnsignedChar(qop.c_str());
				if (!ustrRealm || !ustrUsername || !ustrPassword 
					|| !ustrNonce || !ustrCNonce || !ustrUri || !ustrNc || !ustrQop)
				{
					throw ECSmtp(ECSmtp::BAD_LOGIN_PASSWORD);
				}
				
				MD5 md5a1a;
				md5a1a.update(ustrUsername, m_sLogin.size());
				md5a1a.update((unsigned char*)":", 1);
				md5a1a.update(ustrRealm, realm.size());
				md5a1a.update((unsigned char*)":", 1);
				md5a1a.update(ustrPassword, m_sPassword.size());
				md5a1a.finalize();
				unsigned char *ua1 = md5a1a.raw_digest();

				MD5 md5a1b;
				md5a1b.update(ua1, 16);
				md5a1b.update((unsigned char*)":", 1);
				md5a1b.update(ustrNonce, nonce.size());
				md5a1b.update((unsigned char*)":", 1);
				md5a1b.update(ustrCNonce, strlen(cnonce));
				//authzid could be added here
				md5a1b.finalize();
				char *a1 = md5a1b.hex_digest();
				
				MD5 md5a2;
				md5a2.update((unsigned char*) "AUTHENTICATE:", 13);
				md5a2.update(ustrUri, uri.size());
				//authint and authconf add an additional line here	
				md5a2.finalize();
				char *a2 = md5a2.hex_digest();

				delete[] ua1;
				ua1 = CharToUnsignedChar(a1);
				unsigned char *ua2 = CharToUnsignedChar(a2);
				
				//compute KD
				MD5 md5;
				md5.update(ua1, 32);
				md5.update((unsigned char*)":", 1);
				md5.update(ustrNonce, nonce.size());
				md5.update((unsigned char*)":", 1);
				md5.update(ustrNc, strlen(nc));
				md5.update((unsigned char*)":", 1);
				md5.update(ustrCNonce, strlen(cnonce));
				md5.update((unsigned char*)":", 1);
				md5.update(ustrQop, qop.size());
				md5.update((unsigned char*)":", 1);
				md5.update(ua2, 32);
				md5.finalize();
				decoded_challenge = md5.hex_digest();

				delete[] ustrRealm;
				delete[] ustrUsername;
				delete[] ustrPassword;
				delete[] ustrNonce;
				delete[] ustrCNonce;
				delete[] ustrUri;
				delete[] ustrNc;
				delete[] ustrQop;
				delete[] ua1;
				delete[] ua2;
				delete[] a1;
				delete[] a2;
				ustrRealm = NULL;
				ustrUsername = NULL;
				ustrPassword = NULL;
				ustrNonce = NULL;
				ustrCNonce = NULL;
				ustrUri = NULL;
				ustrQop = NULL;
				ua1 = NULL;
				ua2 = NULL;
				a1 = NULL;
				a2 = NULL;

				//send the response
				if (strstr(RecvBuf, "charset") >= 0)
				{
					snprintf(SendBuf, BUFFER_SIZE, "charset=utf-8,username=\"%s\"", m_sLogin.c_str());
				}
				else
				{
					snprintf(SendBuf, BUFFER_SIZE, "username=\"%s\"", m_sLogin.c_str());
				}
				if (!realm.empty())
				{
					snprintf(RecvBuf, BUFFER_SIZE, ",realm=\"%s\"", realm.c_str());
					strcat(SendBuf, RecvBuf);
				}
				snprintf(RecvBuf, BUFFER_SIZE, ",nonce=\"%s\"", nonce.c_str());
				strcat(SendBuf, RecvBuf);
				snprintf(RecvBuf, BUFFER_SIZE, ",nc=%s", nc);
				strcat(SendBuf, RecvBuf);
				snprintf(RecvBuf, BUFFER_SIZE, ",cnonce=\"%s\"", cnonce);
				strcat(SendBuf, RecvBuf);
				snprintf(RecvBuf, BUFFER_SIZE, ",digest-uri=\"%s\"", uri.c_str());
				strcat(SendBuf, RecvBuf);
				snprintf(RecvBuf, BUFFER_SIZE, ",response=%s", decoded_challenge.c_str());
				strcat(SendBuf, RecvBuf);
				snprintf(RecvBuf, BUFFER_SIZE, ",qop=%s", qop.c_str());
				strcat(SendBuf, RecvBuf);
				unsigned char *ustrDigest = CharToUnsignedChar(SendBuf);
				encoded_challenge = base64_encode(ustrDigest, strlen(SendBuf));
				delete[] ustrDigest;
				ustrDigest = NULL;
				snprintf(SendBuf, BUFFER_SIZE, "%s\r\n", encoded_challenge.c_str());
				pEntry = FindCommandEntry(command_DIGESTMD5);
				SendData(pEntry);
				ReceiveResponse(pEntry);

				//Send completion carraige return
				snprintf(SendBuf, BUFFER_SIZE, "\r\n");				
				pEntry = FindCommandEntry(command_PASSWORD);
				SendData(pEntry);
				ReceiveResponse(pEntry);
			}
			else
			{
				throw ECSmtp(ECSmtp::LOGIN_NOT_SUPPORTED);
			}
		}
	}
	
	catch(const ECSmtp &e)
	{
		if ('5' == RecvBuf[0] && '3' == RecvBuf[1] && '0' == RecvBuf[2])
		{
			m_bConnected = false;
		}
		CSMTP_DEBUG("connectRemoteServer Failed !\n");
		DisconnectRemoteServer();
		//继续往外层抛出异常可能导致aborted,故在此返回错误码
		//throw;
		return (int)e.GetErrorNum();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: DisconnectRemoteServer
// DESCRIPTION: 从SMTP服务器断开连接并关闭socket
//   ARGUMENTS: none
// USES GLOBAL: none
// MODIFIES GL: none
//     RETURNS: void
//      AUTHOR: David Johns
// AUTHOR/DATE: DRJ 2010-08-14
////////////////////////////////////////////////////////////////////////////////
int CSmtp::DisconnectRemoteServer()
{
	int ret =0;
	try
	{
		if(m_bConnected) SayQuit();
	}
	catch(const ECSmtp &e)
	{
		ret = (int)e.GetErrorNum();
	}
	if(hSocket)
	{
		CSMTP_DEBUG("disconnect\n");
		//反向初始化OpenSSL
		CleanupOpenSSL();
#ifdef LINUX
		close(hSocket);
#else
		closesocket(hSocket);
#endif
	}
	hSocket = INVALID_SOCKET;
	
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SmtpXYZdigits
// DESCRIPTION: Converts three letters from RecvBuf to the number.
//   ARGUMENTS: none
// USES GLOBAL: RecvBuf
// MODIFIES GL: none
//     RETURNS: integer number
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
int CSmtp::SmtpXYZdigits()
{
	assert(RecvBuf);
	if(RecvBuf == NULL)
		return 0;
	return (RecvBuf[0]-'0')*100 + (RecvBuf[1]-'0')*10 + RecvBuf[2]-'0';
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: FormatHeader
// DESCRIPTION: 准备信息头
//   ARGUMENTS: char* header - formated header string
// USES GLOBAL: Recipients, CCRecipients, BCCRecipients
// MODIFIES GL: none
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
void CSmtp::FormatHeader(char* header)
{
	char month[][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	size_t i;
	std::string to;
	std::string cc;
	std::string bcc;
	time_t rawtime;
	struct tm* timeinfo;

	// date/time check
	if(time(&rawtime) > 0)
		timeinfo = localtime(&rawtime);
	else
		throw ECSmtp(ECSmtp::TIME_ERROR);

	// check for at least one recipient
	if(Recipients.size())
	{
		for (i=0;i<Recipients.size();i++)
		{
			if(i > 0)
				to.append(",");
			to += Recipients[i].Name;
			to.append("<");
			to += Recipients[i].Mail;
			to.append(">");
		}
	}
	else
		throw ECSmtp(ECSmtp::UNDEF_RECIPIENTS);

	if(CCRecipients.size())
	{
		for (i=0;i<CCRecipients.size();i++)
		{
			if(i > 0)
				cc. append(",");
			cc += CCRecipients[i].Name;
			cc.append("<");
			cc += CCRecipients[i].Mail;
			cc.append(">");
		}
	}
	
	// Date: <SP> <dd> <SP> <mon> <SP> <yy> <SP> <hh> ":" <mm> ":" <ss> <SP> <zone> <CRLF>
	snprintf(header, BUFFER_SIZE, "Date: %d %s %d %d:%d:%d\r\n", timeinfo->tm_mday,
	         month[timeinfo->tm_mon], timeinfo->tm_year+1900, timeinfo->tm_hour,
             timeinfo->tm_min, timeinfo->tm_sec); 
	
	// From: <SP> <sender>  <SP> "<" <sender-email> ">" <CRLF>
	if(!m_sMailFrom.size()) throw ECSmtp(ECSmtp::UNDEF_MAIL_FROM);
	 
	strcat(header,"From: ");
	if(m_sNameFrom.size()) strcat(header, m_sNameFrom.c_str());
	 
	strcat(header," <");
	strcat(header,m_sMailFrom.c_str());
	strcat(header, ">\r\n");

	// X-Mailer: <SP> <xmailer-app> <CRLF>
	if(m_sXMailer.size())
	{
		strcat(header,"X-Mailer: ");
		strcat(header, m_sXMailer.c_str());
		strcat(header, "\r\n");
	}

	// Reply-To: <SP> <reverse-path> <CRLF>
	if(m_sReplyTo.size())
	{
		strcat(header, "Reply-To: ");
		strcat(header, m_sReplyTo.c_str());
		strcat(header, "\r\n");
	}

	// Disposition-Notification-To: <SP> <reverse-path or sender-email> <CRLF>
	if(m_bReadReceipt)
	{
		strcat(header, "Disposition-Notification-To: ");
		if(m_sReplyTo.size()) strcat(header, m_sReplyTo.c_str());
		else strcat(header, m_sNameFrom.c_str());
		strcat(header, "\r\n");
	}

	// X-Priority: <SP> <number> <CRLF>
	switch(m_iXPriority)
	{
		case XPRIORITY_HIGH:
			strcat(header,"X-Priority: 2 (High)\r\n");
			break;
		case XPRIORITY_NORMAL:
			strcat(header,"X-Priority: 3 (Normal)\r\n");
			break;
		case XPRIORITY_LOW:
			strcat(header,"X-Priority: 4 (Low)\r\n");
			break;
		default:
			strcat(header,"X-Priority: 3 (Normal)\r\n");
	}

	// To: <SP> <remote-user-mail> <CRLF>
	strcat(header,"To: ");
	strcat(header, to.c_str());
	strcat(header, "\r\n");

	// Cc: <SP> <remote-user-mail> <CRLF>
	if(CCRecipients.size())
	{
		strcat(header,"Cc: ");
		strcat(header, cc.c_str());
		strcat(header, "\r\n");
	}

	if(BCCRecipients.size())
	{
		strcat(header,"Bcc: ");
		strcat(header, bcc.c_str());
		strcat(header, "\r\n");
	}

	// Subject: <SP> <subject-text> <CRLF>
	if(!m_sSubject.size()) 
		strcat(header, "Subject:  ");
	else
	{
	  strcat(header, "Subject: ");
	  strcat(header, m_sSubject.c_str());
	}
	strcat(header, "\r\n");
	
	// MIME-Version: <SP> 1.0 <CRLF>
	strcat(header,"MIME-Version: 1.0\r\n");
	if(!Attachments.size())
	{ // no attachments
		if(m_bHTML) strcat(header, "Content-Type: text/html; charset=\"");
		else strcat(header, "Content-type: text/plain; charset=\"");
		strcat(header, m_sCharSet.c_str());
		strcat(header, "\"\r\n");
		strcat(header,"Content-Transfer-Encoding: 7bit\r\n");
		strcat(SendBuf,"\r\n");
	}
	else
	{ // there is one or more attachments
		strcat(header,"Content-Type: multipart/mixed; boundary=\"");
		strcat(header,BOUNDARY_TEXT);
		strcat(header,"\"\r\n");
		strcat(header,"\r\n");
		// first goes text message
		strcat(SendBuf,"--");
		strcat(SendBuf,BOUNDARY_TEXT);
		strcat(SendBuf,"\r\n");
		if(m_bHTML) strcat(SendBuf,"Content-type: text/html; charset=");
		else strcat(SendBuf,"Content-type: text/plain; charset=");
		strcat(header, m_sCharSet.c_str());
		strcat(header, "\r\n");
		strcat(SendBuf,"Content-Transfer-Encoding: 7bit\r\n");
		strcat(SendBuf,"\r\n");
	}

	// done
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: ReceiveData
// DESCRIPTION: 接收行终止 '\n'.
//   ARGUMENTS: none
// USES GLOBAL: RecvBuf
// MODIFIES GL: RecvBuf
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-07
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// MODIFICATION: Receives data as much as possible. Another function ReceiveResponse
//               will ensure the received data contains '\n'
// AUTHOR/DATE:  John Tang 2010-08-01
////////////////////////////////////////////////////////////////////////////////
void CSmtp::ReceiveData(Command_Entry* pEntry)
{
	if(m_ssl != NULL)
	{
		ReceiveData_SSL(m_ssl, pEntry);
		return;
	}
	int res = 0;
	fd_set fdread;
	timeval time;

	time.tv_sec = pEntry->recv_timeout;
	time.tv_usec = 0;

	assert(RecvBuf);

	if(RecvBuf == NULL)
		throw ECSmtp(ECSmtp::RECVBUF_IS_EMPTY);

	FD_ZERO(&fdread);

	FD_SET(hSocket,&fdread);

	if((res = select(hSocket+1, &fdread, NULL, NULL, &time)) == SOCKET_ERROR)
	{
		FD_CLR(hSocket,&fdread);
		throw ECSmtp(ECSmtp::WSA_SELECT);
	}

	if(!res)
	{
		//timeout
		FD_CLR(hSocket,&fdread);
		throw ECSmtp(ECSmtp::SERVER_NOT_RESPONDING);
	}

	if(FD_ISSET(hSocket,&fdread))
	{
		res = recv(hSocket,RecvBuf,BUFFER_SIZE,0);
		if(res == SOCKET_ERROR)
		{
			FD_CLR(hSocket,&fdread);
			throw ECSmtp(ECSmtp::WSA_RECV);
		}
	}

	FD_CLR(hSocket,&fdread);
	RecvBuf[res] = 0;
	if(res == 0)
	{
		throw ECSmtp(ECSmtp::CONNECTION_CLOSED);
	}
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SendData
// DESCRIPTION: 从发送缓冲区发送数据
//   ARGUMENTS: none
// USES GLOBAL: SendBuf
// MODIFIES GL: none
//     RETURNS: void
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SendData(Command_Entry* pEntry)
{
	if(m_ssl != NULL)
	{
		SendData_SSL(m_ssl, pEntry);
		return;
	}
	int idx = 0,res,nLeft = strlen(SendBuf);
	fd_set fdwrite;
	timeval time;

	time.tv_sec = pEntry->send_timeout;
	time.tv_usec = 0;

	assert(SendBuf);

	if(SendBuf == NULL)
		throw ECSmtp(ECSmtp::SENDBUF_IS_EMPTY);

	while(nLeft > 0)
	{
		FD_ZERO(&fdwrite);

		FD_SET(hSocket,&fdwrite);

		if((res = select(hSocket+1,NULL,&fdwrite,NULL,&time)) == SOCKET_ERROR)
		{
			FD_CLR(hSocket,&fdwrite);
			throw ECSmtp(ECSmtp::WSA_SELECT);
		}

		if(!res)
		{
			//timeout
			FD_CLR(hSocket,&fdwrite);
			throw ECSmtp(ECSmtp::SERVER_NOT_RESPONDING);
		}

		if(res && FD_ISSET(hSocket,&fdwrite))
		{
			res = send(hSocket,&SendBuf[idx],nLeft,0);
			if(res == SOCKET_ERROR || res == 0)
			{
				FD_CLR(hSocket,&fdwrite);
				throw ECSmtp(ECSmtp::WSA_SEND);
			}
			nLeft -= res;
			idx += res;
		}
	}

	OutputDebugStringA(SendBuf);
	FD_CLR(hSocket,&fdwrite);
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetLocalHostName
// DESCRIPTION: 获取本地主机名
//   ARGUMENTS: none
// USES GLOBAL: m_pcLocalHostName
// MODIFIES GL: m_oError, m_pcLocalHostName 
//     RETURNS: socket of the remote service
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
const char* CSmtp::GetLocalHostName()
{
	return m_sLocalHostName.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetRecipientCount
// DESCRIPTION: 获取收件人数目
//   ARGUMENTS: none
// USES GLOBAL: Recipients
// MODIFIES GL: none 
//     RETURNS: number of recipents
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
unsigned int CSmtp::GetRecipientCount() const
{
	return Recipients.size();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetBCCRecipientCount
// DESCRIPTION: 获取密送人数目
//   ARGUMENTS: none
// USES GLOBAL: BCCRecipients
// MODIFIES GL: none 
//     RETURNS: number of bcc-recipents
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
unsigned int CSmtp::GetBCCRecipientCount() const
{
	return BCCRecipients.size();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetCCRecipientCount
// DESCRIPTION: 获取抄送人数目
//   ARGUMENTS: none
// USES GLOBAL: CCRecipients
// MODIFIES GL: none 
//     RETURNS: number of cc-recipents
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
unsigned int CSmtp::GetCCRecipientCount() const
{
	return CCRecipients.size();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetReplyTo
// DESCRIPTION: 获取一个返回地址的信息
//   ARGUMENTS: none
// USES GLOBAL: m_sReplyTo
// MODIFIES GL: none 
//     RETURNS: m_sReplyTo string
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
const char* CSmtp::GetReplyTo() const
{
	return m_sReplyTo.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetMailFrom
// DESCRIPTION: 获取一个From信息
//   ARGUMENTS: none
// USES GLOBAL: m_sMailFrom
// MODIFIES GL: none 
//     RETURNS: m_sMailFrom string
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
const char* CSmtp::GetMailFrom() const
{
	return m_sMailFrom.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetSenderName
// DESCRIPTION: 获取发送者的名字
//   ARGUMENTS: none
// USES GLOBAL: m_sNameFrom
// MODIFIES GL: none 
//     RETURNS: m_sNameFrom string
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
const char* CSmtp::GetSenderName() const
{
	return m_sNameFrom.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetSubject
// DESCRIPTION: 获取邮件主题
//   ARGUMENTS: none
// USES GLOBAL: m_sSubject
// MODIFIES GL: none 
//     RETURNS: m_sSubject string
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
const char* CSmtp::GetSubject() const
{
	return m_sSubject.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetXMailer
// DESCRIPTION: 获取信件是从哪个客户端发送出来的
//   ARGUMENTS: none
// USES GLOBAL: m_pcXMailer
// MODIFIES GL: none 
//     RETURNS: m_pcXMailer string
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
const char* CSmtp::GetXMailer() const
{
	return m_sXMailer.c_str();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetXPriority
// DESCRIPTION: Returns m_iXPriority string.
//   ARGUMENTS: none
// USES GLOBAL: m_iXPriority
// MODIFIES GL: none 
//     RETURNS: CSmptXPriority m_pcXMailer
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
CSmptXPriority CSmtp::GetXPriority() const
{
	return m_iXPriority;
}

const char* CSmtp::GetMsgLineText(unsigned int Line) const
{
	if(Line >= MsgBody.size())
		throw ECSmtp(ECSmtp::OUT_OF_MSG_RANGE);
	return MsgBody.at(Line).c_str();
}

unsigned int CSmtp::GetMsgLines() const
{
	return MsgBody.size();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetCharSet
// DESCRIPTION: 修改默认额字符集(US-ASCII)
//   ARGUMENTS: const char *sCharSet 
// USES GLOBAL: m_sCharSet
// MODIFIES GL: m_sCharSet
//     RETURNS: none
//      AUTHOR: David Johns
// AUTHOR/DATE: DJ 2012-11-03
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetCharSet(const char *sCharSet)
{
    m_sCharSet = sCharSet;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetLocalHostName
// DESCRIPTION: 修改本地主机名 
//   ARGUMENTS: const char *sLocalHostName 
// USES GLOBAL: m_sLocalHostName
// MODIFIES GL: m_sLocalHostName
//     RETURNS: none
//      AUTHOR: jerko
// AUTHOR/DATE: J 2011-12-01
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetLocalHostName(const char *sLocalHostName)
{
    m_sLocalHostName = sLocalHostName;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetXPriority
// DESCRIPTION: 修改消息优先级
//   ARGUMENTS: CSmptXPriority priority - priority of the message (	XPRIORITY_HIGH,
//              XPRIORITY_NORMAL, XPRIORITY_LOW)
// USES GLOBAL: none
// MODIFIES GL: m_iXPriority 
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetXPriority(CSmptXPriority priority)
{
	m_iXPriority = priority;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetReplyTo
// DESCRIPTION: 设置返回的地址
//   ARGUMENTS: const char *ReplyTo - return address
// USES GLOBAL: m_sReplyTo
// MODIFIES GL: m_sReplyTo
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetReplyTo(const char *ReplyTo)
{
	m_sReplyTo = ReplyTo;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetReadReceipt
// DESCRIPTION: 设置是否请求读收据
//   ARGUMENTS: bool requestReceipt - whether or not to request a read receipt
// USES GLOBAL: m_bReadReceipt
// MODIFIES GL: m_bReadReceipt
//     RETURNS: none
//      AUTHOR: David Johns
// AUTHOR/DATE: DRJ 2012-11-03
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetReadReceipt(bool requestReceipt/*=true*/)
{
	m_bReadReceipt = requestReceipt;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetSenderMail
// DESCRIPTION: 设置发送方的Email
//   ARGUMENTS: const char *EMail - sender's e-mail
// USES GLOBAL: m_sMailFrom
// MODIFIES GL: m_sMailFrom
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetSenderMail(const char *EMail)
{
	m_sMailFrom = EMail;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetSenderName
// DESCRIPTION: 设置发送方的名字
//   ARGUMENTS: const char *Name - sender's name
// USES GLOBAL: m_sNameFrom
// MODIFIES GL: m_sNameFrom
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetSenderName(const char *Name)
{
	m_sNameFrom = Name;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetSubject
// DESCRIPTION: 设置邮件的主题
//   ARGUMENTS: const char *Subject - subject of the message
// USES GLOBAL: m_sSubject
// MODIFIES GL: m_sSubject
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetSubject(const char *Subject)
{
	m_sSubject = Subject;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetXMailer
// DESCRIPTION: 设置发送邮件的名称
//   ARGUMENTS: const char *XMailer - programe name
// USES GLOBAL: m_sXMailer
// MODIFIES GL: m_sXMailer
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetXMailer(const char *XMailer)
{
	m_sXMailer = XMailer;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetLogin
// DESCRIPTION: 设置登录SMTP帐户.
//   ARGUMENTS: const char *Login - login of SMTP account's owner
// USES GLOBAL: m_sLogin
// MODIFIES GL: m_sLogin
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetLogin(const char *Login)
{
	m_sLogin = Login;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetPassword
// DESCRIPTION: 设置登录SMTP密码.
//   ARGUMENTS: const char *Password - password of SMTP account's owner
// USES GLOBAL: m_sPassword
// MODIFIES GL: m_sPassword
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JP 2010-07-08
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetPassword(const char *Password)
{
	m_sPassword = Password;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: SetSMTPServer
// DESCRIPTION: 设置登录SMTP的服务器和端口号
//   ARGUMENTS: const char* SrvName - SMTP service name
//              const unsigned short SrvPort - SMTO service port
// USES GLOBAL: m_sSMTPSrvName
// MODIFIES GL: m_sSMTPSrvName 
//     RETURNS: none
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
//							JO 2010-0708
////////////////////////////////////////////////////////////////////////////////
void CSmtp::SetSMTPServer(const char* SrvName, const unsigned short SrvPort, bool authenticate)
{
	m_iSMTPSrvPort = SrvPort;
	m_sSMTPSrvName = SrvName;
	m_bAuthenticate = authenticate;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: GetErrorText (friend function)
// DESCRIPTION: 返回错误码对应的字符串
//   ARGUMENTS: CSmtpError ErrorId - error code
// USES GLOBAL: none
// MODIFIES GL: none 
//     RETURNS: error string
//      AUTHOR: Jakub Piwowarczyk
// AUTHOR/DATE: JP 2010-01-28
////////////////////////////////////////////////////////////////////////////////
std::string ECSmtp::GetErrorText() const
{
	switch(ErrorCode)
	{
		case ECSmtp::CSMTP_NO_ERROR:
			return "";
		case ECSmtp::WSA_STARTUP:
			return "Unable to initialise winsock2";
		case ECSmtp::WSA_VER:
			return "Wrong version of the winsock2";
		case ECSmtp::WSA_SEND:
			return "Function send() failed";
		case ECSmtp::WSA_RECV:
			return "Function recv() failed";
		case ECSmtp::WSA_CONNECT:
			return "Function connect failed";
		case ECSmtp::WSA_GETHOSTBY_NAME_ADDR:
			return "Unable to determine remote server";
		case ECSmtp::WSA_INVALID_SOCKET:
			return "Invalid winsock2 socket";
		case ECSmtp::WSA_HOSTNAME:
			return "Function hostname() failed";
		case ECSmtp::WSA_IOCTLSOCKET:
			return "Function ioctlsocket() failed";
		case ECSmtp::BAD_IPV4_ADDR:
			return "Improper IPv4 address";
		case ECSmtp::UNDEF_MSG_HEADER:
			return "Undefined message header";
		case ECSmtp::UNDEF_MAIL_FROM:
			return "Undefined mail sender";
		case ECSmtp::UNDEF_SUBJECT:
			return "Undefined message subject";
		case ECSmtp::UNDEF_RECIPIENTS:
			return "Undefined at least one reciepent";
		case ECSmtp::UNDEF_RECIPIENT_MAIL:
			return "Undefined recipent mail";
		case ECSmtp::UNDEF_LOGIN:
			return "Undefined user login";
		case ECSmtp::UNDEF_PASSWORD:
			return "Undefined user password";
		case ECSmtp::BAD_LOGIN_PASSWORD:
			return "Invalid user login or password";
		case ECSmtp::BAD_DIGEST_RESPONSE:
			return "Server returned a bad digest MD5 response";
		case ECSmtp::BAD_SERVER_NAME:
			return "Unable to determine server name for digest MD5 response";
		case ECSmtp::COMMAND_MAIL_FROM:
			return "Server returned error after sending MAIL FROM";
		case ECSmtp::COMMAND_EHLO:
			return "Server returned error after sending EHLO";
		case ECSmtp::COMMAND_AUTH_PLAIN:
			return "Server returned error after sending AUTH PLAIN";
		case ECSmtp::COMMAND_AUTH_LOGIN:
			return "Server returned error after sending AUTH LOGIN";
		case ECSmtp::COMMAND_AUTH_CRAMMD5:
			return "Server returned error after sending AUTH CRAM-MD5";
		case ECSmtp::COMMAND_AUTH_DIGESTMD5:
			return "Server returned error after sending AUTH DIGEST-MD5";
		case ECSmtp::COMMAND_DIGESTMD5:
			return "Server returned error after sending MD5 DIGEST";
		case ECSmtp::COMMAND_DATA:
			return "Server returned error after sending DATA";
		case ECSmtp::COMMAND_QUIT:
			return "Server returned error after sending QUIT";
		case ECSmtp::COMMAND_RCPT_TO:
			return "Server returned error after sending RCPT TO";
		case ECSmtp::MSG_BODY_ERROR:
			return "Error in message body";
		case ECSmtp::CONNECTION_CLOSED:
			return "Server has closed the connection";
		case ECSmtp::SERVER_NOT_READY:
			return "Server is not ready";
		case ECSmtp::SERVER_NOT_RESPONDING:
			return "Server not responding";
		case ECSmtp::FILE_NOT_EXIST:
			return "Attachment file does not exist";
		case ECSmtp::MSG_TOO_BIG:
			return "Message is too big";
		case ECSmtp::BAD_LOGIN_PASS:
			return "Bad login or password";
		case ECSmtp::UNDEF_XYZ_RESPONSE:
			return "Undefined xyz SMTP response";
		case ECSmtp::LACK_OF_MEMORY:
			return "Lack of memory";
		case ECSmtp::TIME_ERROR:
			return "time() error";
		case ECSmtp::RECVBUF_IS_EMPTY:
			return "RecvBuf is empty";
		case ECSmtp::SENDBUF_IS_EMPTY:
			return "SendBuf is empty";
		case ECSmtp::OUT_OF_MSG_RANGE:
			return "Specified line number is out of message size";
		case ECSmtp::COMMAND_EHLO_STARTTLS:
			return "Server returned error after sending STARTTLS";
		case ECSmtp::SSL_PROBLEM:
			return "SSL problem";
		case ECSmtp::COMMAND_DATABLOCK:
			return "Failed to send data block";
		case ECSmtp::STARTTLS_NOT_SUPPORTED:
			return "The STARTTLS command is not supported by the server";
		case ECSmtp::LOGIN_NOT_SUPPORTED:
			return "AUTH LOGIN is not supported by the server";
        case ECSmtp::TXT_FILE_NOT_EXIST:
            return "The txt file send message from it not exist";
		default:
			return "Undefined error id";
	}
}

void CSmtp::SayHello()
{
	Command_Entry* pEntry = FindCommandEntry(command_EHLO);
	//这里GetLocalHostName获得的名字如果带有括号，如(zview)
	//将导致此步失败，返回501 5.5.4错误
	//因此这里固定使用domain
	snprintf(SendBuf, BUFFER_SIZE, "EHLO %s\r\n", 
		/*GetLocalHostName()!=NULL ? m_sLocalHostName.c_str() : "domain"*/"domain");
	SendData(pEntry);
	ReceiveResponse(pEntry);
	m_bConnected = true;
}

void CSmtp::SayQuit()
{
	// ***** CLOSING CONNECTION *****
	
	Command_Entry* pEntry = FindCommandEntry(command_QUIT);
	// QUIT <CRLF>
	snprintf(SendBuf, BUFFER_SIZE, "QUIT\r\n");
	m_bConnected=false;
	SendData(pEntry);
	ReceiveResponse(pEntry);
}

bool CSmtp::IsSupportedTls()
{
	return IsKeywordSupported(RecvBuf, "STARTTLS");
}

void CSmtp::StartTls()
{
	Command_Entry* pEntry = FindCommandEntry(command_STARTTLS);
	snprintf(SendBuf, BUFFER_SIZE, "STARTTLS\r\n");
	SendData(pEntry);
	ReceiveResponse(pEntry);

	OpenSSLConnect();
}

void CSmtp::ReceiveData_SSL(WOLFSSL* ssl, Command_Entry* pEntry)
{
	int res = 0;
	int offset = 0;
	fd_set fdread;
	fd_set fdwrite;
	timeval time;

	int read_blocked_on_write = 0;

	time.tv_sec = pEntry->recv_timeout;
	time.tv_usec = 0;

	assert(RecvBuf);

	if(RecvBuf == NULL)
		throw ECSmtp(ECSmtp::RECVBUF_IS_EMPTY);

	bool bFinish = false;

	while(!bFinish)
	{
		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);

		FD_SET(hSocket,&fdread);

		if(read_blocked_on_write)
		{
			FD_SET(hSocket, &fdwrite);
		}

		if((res = select(hSocket+1, &fdread, &fdwrite, NULL, &time)) == SOCKET_ERROR)
		{
			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			throw ECSmtp(ECSmtp::WSA_SELECT);
		}

		if(!res)
		{
			//timeout
			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			throw ECSmtp(ECSmtp::SERVER_NOT_RESPONDING);
		}

		if(FD_ISSET(hSocket,&fdread) || (read_blocked_on_write && FD_ISSET(hSocket,&fdwrite)) )
		{
			while(1)
			{
				read_blocked_on_write=0;

				const int buff_len = 1024;
				char buff[buff_len];
				
				res = wolfSSL_read(ssl, buff, buff_len);

				int ssl_err = wolfSSL_get_error(ssl, res);
				if(ssl_err == SSL_ERROR_NONE)
				{
					if(offset + res > BUFFER_SIZE - 1)
					{
						FD_ZERO(&fdread);
						FD_ZERO(&fdwrite);
						throw ECSmtp(ECSmtp::LACK_OF_MEMORY);
					}
					memcpy(RecvBuf + offset, buff, res);
					offset += res;
					if(wolfSSL_pending(ssl))
					{
						continue;
					}
					else
					{
						bFinish = true;
						break;
					}
				}
				else if(ssl_err == SSL_ERROR_ZERO_RETURN)
				{
					bFinish = true;
					break;
				}
				else if(ssl_err == SSL_ERROR_WANT_READ)
				{
					break;
				}
				else if(ssl_err == SSL_ERROR_WANT_WRITE)
				{
					/* We get a WANT_WRITE if we're
					trying to rehandshake and we block on
					a write during that rehandshake.

					We need to wait on the socket to be 
					writeable but reinitiate the read
					when it is */
					read_blocked_on_write=1;
					break;
				}
				else
				{
					FD_ZERO(&fdread);
					FD_ZERO(&fdwrite);
					CSMTP_DEBUG("ReceiveData_SSL Failed!\n");
					throw ECSmtp(ECSmtp::SSL_PROBLEM);
				}
			}
		}
	}

	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	RecvBuf[offset] = 0;
	if(offset == 0)
	{
		throw ECSmtp(ECSmtp::CONNECTION_CLOSED);
	}
}

#define USE_STATIC_STD_STRING 1

#if  USE_STATIC_STD_STRING
#define USE_STD_STRING 1
#elif USE_LOCAL_STD_STRING
#define USE_STD_STRING 1
#endif

void CSmtp::ReceiveResponse(Command_Entry* pEntry)
{
#if USE_STATIC_STD_STRING
	static std::string line;
	line.clear();
#elif USE_STD_STRING
	std::string line;
#else
	char line[BUFFER_SIZE];
	memset(line, 0, BUFFER_SIZE);
#endif

	int reply_code = 0;
	bool bFinish = false;
	while(!bFinish)
	{
		ReceiveData(pEntry);
#if USE_STD_STRING
		line.append(RecvBuf);
		size_t len = line.length();
#else
		strncat(line, RecvBuf, BUFFER_SIZE-strlen(line)-1);
		size_t len = strlen(line);
#endif
		size_t begin = 0;
		size_t offset = 0;

		while (1) // loop for all lines
		{
			while(offset + 1 < len)
			{
				if ('\r' == line[offset] && '\n' == line[offset+1])
				{
					break;
				}
				++offset;
			}
			if (offset + 1 < len) // we found a line
			{
				// see if this is the last line
				// the last line must match the pattern: XYZ<SP>*<CRLF> or XYZ<CRLF> where XYZ is a string of 3 digits 
				offset += 2; // skip <CRLF>
				if (offset - begin >= 5)
				{
					if (isdigit(line[begin]) && isdigit(line[begin+1]) 
						&& isdigit(line[begin+2]))
					{
						// this is the last line
						if (5 == (offset - begin) || ' ' == line[begin+3])
						{
							reply_code = (line[begin]-'0')*100 + (line[begin+1]-'0')*10 + line[begin+2]-'0';
							bFinish = true;
							break;
						}
					}
				}
				begin = offset;	// try to find next line
			}
			else // we haven't received the last line, so we need to receive more data 
			{
				break;
			}
		}
	}
#if USE_STD_STRING
	snprintf(RecvBuf, BUFFER_SIZE, line.c_str());
#else
	snprintf(RecvBuf, BUFFER_SIZE, line);
#endif
	OutputDebugStringA(RecvBuf);
	if(reply_code != pEntry->valid_reply_code)
	{
		throw ECSmtp(pEntry->error);
	}
}

void CSmtp::SendData_SSL(WOLFSSL* ssl, Command_Entry* pEntry)
{
	int offset = 0,res,nLeft = strlen(SendBuf);
	fd_set fdwrite;
	fd_set fdread;
	timeval time;

	int write_blocked_on_read = 0;

	time.tv_sec = pEntry->send_timeout;
	time.tv_usec = 0;

	assert(SendBuf);

	if(SendBuf == NULL)
		throw ECSmtp(ECSmtp::SENDBUF_IS_EMPTY);

	while(nLeft > 0)
	{
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdread);

		FD_SET(hSocket,&fdwrite);

		if(write_blocked_on_read)
		{
			FD_SET(hSocket, &fdread);
		}

		if((res = select(hSocket+1,&fdread,&fdwrite,NULL,&time)) == SOCKET_ERROR)
		{
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
			throw ECSmtp(ECSmtp::WSA_SELECT);
		}

		if(!res)
		{
			//timeout
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
			throw ECSmtp(ECSmtp::SERVER_NOT_RESPONDING);
		}

		if(FD_ISSET(hSocket,&fdwrite) || (write_blocked_on_read && FD_ISSET(hSocket, &fdread)) )
		{
			write_blocked_on_read=0;

			/* Try to write */
			res = wolfSSL_write(ssl, SendBuf+offset, nLeft);
	          
			switch(wolfSSL_get_error(ssl,res))
			{
			  /* We wrote something*/
			  case SSL_ERROR_NONE:
				nLeft -= res;
				offset += res;
				break;
	              
				/* We would have blocked */
			  case SSL_ERROR_WANT_WRITE:
				break;

				/* We get a WANT_READ if we're
				   trying to rehandshake and we block on
				   write during the current connection.
	               
				   We need to wait on the socket to be readable
				   but reinitiate our write when it is */
			  case SSL_ERROR_WANT_READ:
				write_blocked_on_read=1;
				break;
	              
				  /* Some other error */
			  default:	      
				FD_ZERO(&fdread);
				FD_ZERO(&fdwrite);
				CSMTP_DEBUG("SendData_SSL Failed \n");
				throw ECSmtp(ECSmtp::SSL_PROBLEM);
			}

		}
	}

	OutputDebugStringA(SendBuf);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fdread);
}

void CSmtp::InitOpenSSL()
{
	wolfSSL_library_init();
	wolfSSL_load_error_strings();
	if (GetSecurityType() == USE_SSL)
	{
		m_ctx = wolfSSL_CTX_new (wolfSSLv23_client_method());
	}
	else
	{
		m_ctx = wolfSSL_CTX_new (wolfTLSv1_2_client_method());
	}
	if(m_ctx == NULL)
		throw ECSmtp(ECSmtp::SSL_PROBLEM);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP160K1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP160R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP160R2);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP192K1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP192R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP224K1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP224R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP256K1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP256R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP384R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_SECP521R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_BRAINPOOLP256R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_BRAINPOOLP384R1);
	wolfSSL_CTX_UseSupportedCurve(m_ctx,WOLFSSL_ECC_BRAINPOOLP512R1);
}

void CSmtp::OpenSSLConnect()
{
	//wolfSSL_Debugging_ON();
	if(m_ctx == NULL)
	{
		CSMTP_DEBUG("OpenSSLConnect Failed\n");
		throw ECSmtp(ECSmtp::SSL_PROBLEM);
	}
	if(m_ssl == NULL)
	{	
		wolfSSL_CTX_set_verify(m_ctx, SSL_VERIFY_NONE, 0);
		m_ssl = wolfSSL_new (m_ctx); 
	}
	if(m_ssl == NULL)
	{
		CSMTP_DEBUG("OpenSSLConnect Failed\n");
		throw ECSmtp(ECSmtp::SSL_PROBLEM);
	}
	wolfSSL_set_fd (m_ssl, (int)hSocket);
	
	int res = 0;
	fd_set fdwrite;
	fd_set fdread;
	int write_blocked = 0;
	int read_blocked = 0;

	timeval time;
	time.tv_sec = TIME_IN_SEC;
	time.tv_usec = 0;

	while(1)
	{
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdread);

		if(write_blocked)
			FD_SET(hSocket, &fdwrite);
		if(read_blocked)
			FD_SET(hSocket, &fdread);

		if(write_blocked || read_blocked)
		{
			write_blocked = 0;
			read_blocked = 0;
			if((res = select(hSocket+1,&fdread,&fdwrite,NULL,&time)) == SOCKET_ERROR)
			{
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdread);
				throw ECSmtp(ECSmtp::WSA_SELECT);
			}
			if(!res)
			{
				//timeout
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdread);
				throw ECSmtp(ECSmtp::SERVER_NOT_RESPONDING);
			}
		}
		res = wolfSSL_connect(m_ssl);
		switch((wolfSSL_get_error(m_ssl, res)))
		{
		  case  SSL_SUCCESS:
		  case SSL_ERROR_NONE:
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
			return;
			break;
              
		  case SSL_ERROR_WANT_WRITE:
			write_blocked = 1;
			break;

		  case SSL_ERROR_WANT_READ:
			read_blocked = 1;
			break;
              
		  default:	      
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdread);
			CSMTP_DEBUG("OpenSSLConnect Failed\n");
			throw ECSmtp(ECSmtp::SSL_PROBLEM);
		}
	}
}

void CSmtp::CleanupOpenSSL()
{
	if(m_ssl != NULL)
	{
		wolfSSL_shutdown (m_ssl);  /* send SSL/TLS close_notify */
		wolfSSL_free (m_ssl);
		m_ssl = NULL;
	}
	if(m_ctx != NULL)
	{
		wolfSSL_CTX_free (m_ctx);	
		m_ctx = NULL;
		wolfSSL_ERR_free_strings();
		wolfSSL_EVP_cleanup();
		wolfSSL_cleanup_all_ex_data();
	}
}

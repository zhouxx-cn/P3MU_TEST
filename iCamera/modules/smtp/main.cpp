#include "CSmtp.h"
#include <iostream>

int main()
{
	bool bError = false;

	try
	{
		CSmtp mail;

#define test_163_tls

#if defined(test_163_tls)
		mail.SetSMTPServer("smtp.163.com",25);
		mail.SetSecurityType(USE_SSL);
		//mail.SetSecurityType(USE_SSL);
		mail.SetLogin("13687223393@163.com");
		mail.SetPassword("z123456");
		mail.SetSenderName("User");
		mail.SetSenderMail("13687223393@163.com");
		mail.SetReplyTo("13687223393@163.com");
		mail.SetSubject("The message");
		mail.AddRecipient("13687223393@163.com");
#elif defined(test_qq_tls)
		mail.SetSMTPServer("smtp.qq.com",587);
		mail.SetSecurityType(USE_TLS);
		//mail.SetSecurityType(USE_SSL);
		mail.SetLogin("3333333@qq.com");
		mail.SetPassword("3333333");//验证码
		//mail.SetPassword("wood19831216");
		mail.SetSenderName("User");
		mail.SetSenderMail("3333333@qq.com");
		mail.SetReplyTo("3333333@qq.com");
		mail.SetSubject("The message");
		mail.AddRecipient("3333333@qq.com");
#elif defined(test_gmail_tls)
		mail.SetSMTPServer("smtp-mail.outlook.com",587);
		mail.SetSecurityType(USE_TLS);
		mail.SetLogin("zzz@hotmail.com");
		//mail.SetPassword("zzz");
		mail.SetPassword("zzzz");
		mail.SetSenderName("User");
		mail.SetSenderMail("zzz@hotmail.com");
		mail.SetReplyTo("zzz@hotmail.com");
		mail.SetSubject("The message");
		mail.AddRecipient("ehust@hotmail.com");
#elif defined(test_gmail_ssl)
		mail.SetSMTPServer("smtp.gmail.com",465);
		mail.SetSecurityType(USE_SSL);
#elif defined(test_hotmail_TLS)
		mail.SetSMTPServer("smtp.live.com",25);
		mail.SetSecurityType(USE_TLS);
#elif defined(test_aol_tls)
		mail.SetSMTPServer("smtp.aol.com",587);
		mail.SetSecurityType(USE_TLS);
#elif defined(test_yahoo_ssl)
		mail.SetSMTPServer("plus.smtp.mail.yahoo.com",465);
		mail.SetSecurityType(USE_SSL);
#endif
  		mail.SetXPriority(XPRIORITY_NORMAL);
  		mail.SetXMailer("The Bat! (v3.02) Professional");
  		mail.AddMsgLine("Hello,");
		mail.AddMsgLine("");
		mail.AddMsgLine("...");
		mail.AddMsgLine("How are you today?");
		mail.AddMsgLine("");
		mail.AddMsgLine("Regards");
		mail.ModMsgLine(5,"regards");
		mail.DelMsgLine(2);
		mail.AddMsgLine("User");

  		//mail.AddAttachment("../test1.jpg");
  		//mail.AddAttachment("c:\\test2.exe");
		//mail.AddAttachment("c:\\test3.txt");
		mail.Send();
	}
	catch(ECSmtp e)
	{
		std::cout << "Error: " << e.GetErrorText().c_str() << ".\n";
		bError = true;
	}
	if(!bError)
		std::cout << "Mail was send successfully.\n";
	return 0;
}

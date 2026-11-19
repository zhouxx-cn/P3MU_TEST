//!\file tagSerialPort.h
//!\brief 定义DVR目前支持的串口类型
//!\author yuk
//!\date 2009-09-21

#ifndef __TAG_SERIALPORT_H_
#define __TAG_SERIALPORT_H_

//!串口类型
//##ModelId=49B863C803C8
enum SERIAL_TYPE
{
	//!RS232串口
	//##ModelId=49B863DB0167
	TTY232,

	//!RS485串口
	//##ModelId=49B863E902BF
	TTY485,

	//!RS422串口
	//##ModelId=49B863F200DA
	TTY422
};

//!\brief 串口类型数,支持三种串口类型
const unsigned int MAX_RS_TYPE = 3;

#endif

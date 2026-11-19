#ifndef _IP_H
#define _IP_H
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Enumarators for the address operations that hides
   the IOCTL types */

	typedef enum addr_get_ops_enum
	{
		GET_NETMASK = SIOCGIFNETMASK,
		GET_BROADCAST = SIOCGIFBRDADDR,
		GET_ADDRESS = SIOCGIFADDR,
		GET_GATEWAY = 0x11,
		GET_MAC = SIOCGIFHWADDR
	} ADDR_GET_OPS;

	typedef enum addr_set_ops_enum
	{
	//ioctl请求的request 参数
		SET_NETMASK = SIOCSIFNETMASK,//获取子网掩码
		SET_BROADCAST = SIOCSIFBRDADDR,//接口操作 获取广播地址
		SET_ADDRESS = SIOCSIFADDR,//接口操作 获取接口地址
		SET_GATEWAY = 0x12
	} ADDR_SET_OPS;

	int Replace_Section_In_File(char *FileName, char *ifname, char *SectionPattern, char *NewValue);

/* A helper function to set one of the interfcace addresses,
   be it the IP, netmask or broadcast. It also sets the out_address
   to the dotted quad form of the address. out_address must be a least 
   of IPQUADSIZ length. */

	int set_if_address(char *if_name, char *in_address, ADDR_SET_OPS operation);

/* A helper function to get one of the interfcace addresses,
   be it the IP, netmask or broadcast in dotted quad notation.
   address must point to a buffer of at least DQUADSIZ size */

	int get_if_address(char *if_name, char *address, int addressSize, ADDR_GET_OPS operation);
#ifdef __cplusplus
}
#endif


#endif

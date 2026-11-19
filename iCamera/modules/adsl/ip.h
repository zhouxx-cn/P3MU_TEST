#ifndef _IP_H
#define _IP_H
#include <sys/ioctl.h>

/* Enumarators for the address operations that hides
   the IOCTL types */

typedef enum addr_get_ops_enum
{
	GET_NETMASK = SIOCGIFNETMASK,
	GET_BROADCAST = SIOCGIFBRDADDR,
	GET_ADDRESS = SIOCGIFADDR,
	GET_GATEWAY = 0x11
} ADDR_GET_OPS;

typedef enum addr_set_ops_enum
{
	SET_NETMASK = SIOCSIFNETMASK,
	SET_BROADCAST = SIOCSIFBRDADDR,
	SET_ADDRESS = SIOCSIFADDR,
	SET_GATEWAY = 0x12
} ADDR_SET_OPS;


/* A helper function to set one of the interfcace addresses,
   be it the IP, netmask or broadcast. It also sets the out_address
   to the dotted quad form of the address. out_address must be a least 
   of IPQUADSIZ length. */

int EDVR_ADSL_get_if_address(char *if_name, char *in_address, ADDR_GET_OPS operation);

/* A helper function to get one of the interfcace addresses,
   be it the IP, netmask or broadcast in dotted quad notation.
   address must point to a buffer of at least DQUADSIZ size */
#endif

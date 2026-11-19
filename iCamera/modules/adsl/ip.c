#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "./ip.h"

#define IPQUADSIZ (16)
#define IFNAME "eth0"
#define SYSTEM_NETWORK_CONF "/mnt/mtd/etc/network/interfaces"

/*
 *      Display an IP address in readable format.
 */

#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

/* A helper function to get one of the interfcace addresses,
   be it the IP, netmask or broadcast in dotted quad notation.
   address must point to a buffer of at least DQUADSIZ size */

int EDVR_ADSL_get_if_address(char *if_name, char *address, ADDR_GET_OPS operation)
{

	struct ifreq ifr;

//  struct sockaddr_in sockaddr;
	int sock, ret;

	if ((!if_name) || (!address))
		return EFAULT;



	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return errno;

	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

	if (ioctl(sock, operation, &ifr) >= 0)
	{

		struct sockaddr_in *sadr = (struct sockaddr_in *) &ifr.ifr_addr;

		ret = 0;
		snprintf(address, IPQUADSIZ, "%d.%d.%d.%d", NIPQUAD(sadr->sin_addr));

	}
	else
	{

		ret = errno;
	}

	close(sock);


	return ret;
}

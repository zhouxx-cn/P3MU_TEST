#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s ipaddress netmask\n", argv[0]);
        return -1;
    }

	unsigned int ip = 0;
    int ip1=0, ip2=0, ip3=0, ip4=0;
    int netmask1=0, netmask2=0, netmask3=0, netmask4;

	ip = inet_addr(argv[1]);
	if ((unsigned int) (-1) == ip || 0 == ip)
	{
		return 0;
	}

	ip = inet_addr(argv[2]);
	if ((unsigned int) (-1) == ip || 0 == ip)
	{
		return 0;
	}

    sscanf(argv[1], "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    sscanf(argv[2], "%d.%d.%d.%d", &netmask1, &netmask2, &netmask3, &netmask4);
    
    char new_ip[20] = {0};

    snprintf(new_ip, sizeof(new_ip), "%d.%d.%d.%d", 
             ip1&netmask1, ip2&netmask2, ip3&netmask3, ip4&netmask4);

    printf("%s", new_ip);

    return 0;
}

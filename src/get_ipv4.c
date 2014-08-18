#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "polshare.h"

int
get_ipv4 (void)
{
	
	struct ifaddrs *iflist, *iface;
	char host[NI_MAXHOST];
	int family, s, n;

	if (getifaddrs (&iflist) < 0)
	{
		fprintf (stderr, "getifaddrs");
		return -1;
	}

	for (iface = iflist, n = 0; iface != NULL; iface = iface->ifa_next, n++)
	{		
		if (iface->ifa_addr == NULL)
			continue;

		family = iface->ifa_addr->sa_family;
		
		if (family == AF_INET)
		{
			memset (host, 0, sizeof (host));
			s = getnameinfo (iface->ifa_addr,
					sizeof (struct sockaddr_in),
					host, NI_MAXHOST,
					NULL, 0, NI_NUMERICHOST);
			if (s != 0)
			{
				fprintf (stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
				return -1;
			}
			
			if ((strcmp (host, "127.0.0.1") == 0) ||
				(strcmp (host, "127.0.1.1") == 0) ||
				(strcmp (host, "127.0.0.0") == 0) ||
				(strcmp (host, "::1") == 0) ||
				(strcmp (host, "::0") == 0))
				continue;
			
			else printf("Server IPv4: %s\n", host);
		}

	}

	freeifaddrs (iflist);
	
	return 0;
}

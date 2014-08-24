#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <inttypes.h>
#include <stdint.h>
#include <errno.h>
#include "polshare.h"
 

static uint16_t cksum (void *, uint32_t);
static int ping_ip (const char *, const char *);


int 
get_connected_ip (const char *src)
{
	if (getuid () != 0)
	{
		fprintf (stderr, "[!] ERROR: ping must be executed as root\n");
		return -1;
	}
	
	int i;
	
	char *ip, *src_ip, *ip_slice;
	
	ip = malloc(16);
	src_ip = malloc(16);
	ip_slice = malloc(16);
	
	strcpy(src_ip, src);

	char *p1 = strtok(src_ip, ".");
	strcpy(ip_slice, p1); strcat(ip_slice, ".");

	p1 = strtok(NULL, ".");
	strcat(ip_slice, p1); strcat(ip_slice, ".");

	p1 = strtok(NULL, ".");
	strcat(ip_slice, p1); strcat(ip_slice, ".");

	ip_slice[strlen (ip_slice)] = '\0';

	/* Il ciclo 'for' è da migliorare in base alle classi (o meglio devo tener conto della sottorete e fare i relativi calcoli):
	 * Classe A: 10.0.0.0 - 10.255.255.255
	 * Classe B: 172.16.0.0 - 172.31.255.255
	 * Classe C: 192.168.0.0 - 192.168.255.255
	 */	
	for (i = 2; i < 255; i++)
	{
		memset (ip, 0, strlen (ip));
		snprintf (ip, 16, "%s%d", ip_slice, i);
		ping_ip (src, ip);
	}

	free (ip);
	free (src_ip);
	free (ip_slice);
	
	return 0;
}


static int
ping_ip (	const char *src_addr,
		const char *dst_addr)
{
	struct iphdr *ip;
	struct icmphdr *icmp;
	struct sockaddr_in connection;

	char *packet, *buffer;
	int optval, sockfd;
	ssize_t ret_bytes;
	socklen_t addrlen;
     
	packet = malloc (sizeof (struct iphdr) + sizeof (struct icmphdr));
	buffer = malloc (sizeof (struct iphdr) + sizeof (struct icmphdr));
     
	//ip = (struct iphdr *) packet;
	//icmp = (struct icmphdr *)(packet + sizeof (struct iphdr));
	ip = malloc (sizeof (struct iphdr) + sizeof (struct icmphdr));
	icmp = malloc (sizeof (struct iphdr) + sizeof (struct icmphdr));

	ip->ihl		= 5;
	ip->version	= 4;
	ip->tos		= 0;
	ip->tot_len	= (sizeof (struct iphdr) + sizeof (struct icmphdr));
	ip->id		= htons (0);
	ip->frag_off	= 0;
	ip->ttl		= 64;
	ip->protocol	= IPPROTO_ICMP;
	ip->saddr	= inet_addr (src_addr);
	ip->daddr	= inet_addr (dst_addr);
	ip->check	= cksum (ip, sizeof (struct iphdr));
     
	if ((sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
	{
		fprintf (stderr, "%s\n", strerror (errno));
		return -1;
	}
     
	/* 
	 * IP_HDRINCL must be set on the socket so that
	 * the kernel does not attempt to automatically add
	 * a default ip header to the packet
	 * 
	 */
	setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof (int));
     
	icmp->type		= ICMP_ECHO;
	icmp->code		= 0;
	icmp->un.echo.id	= htons (getpid() & 0xffff);
	icmp->un.echo.sequence	= 0;
	icmp->checksum		= cksum (icmp, sizeof (struct icmphdr));
     
     
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = inet_addr (dst_addr);
          
	sendto (sockfd, packet, ip->tot_len, 0, (struct sockaddr *)&connection, sizeof (struct sockaddr));
     
	addrlen = sizeof (connection);
	
	struct timeval timeout;      
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof (timeout)) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
		return -1;
	}
	
	if ((ret_bytes = recvfrom (sockfd, buffer, sizeof (struct iphdr) + sizeof (struct icmphdr), 0, (struct sockaddr *)&connection, &addrlen)) == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			printf ("%s is offline\n", dst_addr);
		else
		{
			fprintf (stderr, "%s\n", strerror (errno));
			return -1;
		}
	}
	else
		printf ("%s is online\n", dst_addr);
 
	free (packet);
	free (buffer);
	close (sockfd);
	
	return 0;
}


static uint16_t
cksum (	void *tmp,
	uint32_t len)
{
	uint16_t *ip = tmp;
	uint32_t sum = 0;

        while (len > 1)
        {
		sum += *ip++;
		len -= 2;
	}

	if (len)       /* take care of left over byte */
		sum += (uint16_t) *(unsigned char *)ip;
          
	return ~sum;
}

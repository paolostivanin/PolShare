#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gcrypt.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include "polshare.h"

int
main (	int argc,
	char **argv)

{
	
	static struct option long_options[] = 
	{
		{"help",	no_argument,		NULL, 'h'},
		{"version",	no_argument, 		NULL, 'v'},
		{"send",	required_argument, 	NULL, 's'},
		{"recv",	required_argument, 	NULL, 'r'},
		{"list-ip",	required_argument, 	NULL, 'i'},
		{NULL, 0, NULL, 0}
	};
	
	int ch;

	while ((ch = getopt_long (argc, argv, "vhs:r:i:", long_options, NULL)) != -1)
	{
		switch (ch)
		{
			case 'v':
				printf ("polshare v%s developed by Paolo Stivanin <info@paolostivanin.com>\n", VERSION);
				break;
			
			case 'h':
				printf ("Available options are:\n");
				printf ("--send <ip>\t::= to send a file to the specified ip address\n");
				printf ("--recv [y|n]\t::= to auto accept all incoming files OR to confirm every file before download\n");
				printf ("--list-ip <ip>\t::= list all the ip addresses that are connected to your lan\n");
				printf ("--version\t::= display version and developer info\n");
				printf ("--help\t\t::= display this simple help :)\n");
				break;

			case 's':
				do_action (1, optarg);
				break;

			case 'r':
				do_action (2, optarg);
				break;
			
			case 'i':
				do_action (3, optarg);
				
			case '?':
				break;
		}
	}
	
	return 0;
}


int
do_action (	int req,
		const char *opt)
{
	int ret = -1, count = 0, ch = -1;
	char buf[4];
	unsigned int i;
	if (req == 1)
	{
		if (strlen (opt) < 7 || strlen (opt) > 15)
		{
			printf ("Error: ip address too short or long\n");
			return -1;
		}
		
		for (i = 0; i < strlen (opt); i++)
		{
			if ( !(isdigit (opt[i])) && opt[i] != '.')
			{
				printf ("Bad ip address\n");
				return -1;
			}
			if (opt[i] == '.')
				count++;
		}
		
		if (count != 3)
		{
			printf ("Bad ip address\n");
			return -1;
		}
		
		while (1)
		{
			printf ("Write:\n1 to send a file\n2 to quit\nChoice: ");
			fgets (buf, 3, stdin);
			buf[1] = '\0';
			if (strcmp(buf, "1") == 0)
				ch =1;
			else
				ch = 2;
			
			printf ("\n");
			
			switch (ch)
			{
				case 1:
					do_send (opt);
					break;
				case 2:
					return 0;
			}
		}
	}
	else if (req == 2)
	{
		printf ("Type CTRL+C to exit\n");
		while (1)
			do_recv(opt);
	}
	else if (req == 3)
	{
		count = 0;
		
		if (strlen(opt) < 7 || strlen(opt) > 15)
		{
			printf ("Error: ip address too short or long\n");
			return -1;
		}
		
		for (i = 0; i < strlen (opt); i++)
		{
			if ( !(isdigit (opt[i])) && opt[i] != '.')
			{
				printf ("Bad ip address\n");
				return -1;
			}
			if (opt[i] == '.')
				count++;
		}
		
		if (count != 3)
		{
			printf ("Bad ip address\n");
			return -1;
		}
		printf ("IPv4 that are connected to your LAN:\n");
		ret = get_connected_ip (opt);
  		if (ret < 0)
  		{
   			printf ("Error during lan scan. You have to manually search to the active hosts into your LAN\n");
   			return -1;
  		}
  		printf ("\n");
	}
	return 0;
}	

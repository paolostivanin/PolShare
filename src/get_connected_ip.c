#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "polshare.h"

int
get_connected_ip (const char *ip)
{
	char *cmd = NULL;
	const char *part_of_cmd = "nmap -n -sP -oG - | grep \": Up\" | cut -d' ' -f2";
	size_t total_cmd_len = strlen(ip) + strlen(part_of_cmd) + 3;

	cmd = malloc (total_cmd_len);
	snprintf (cmd, total_cmd_len, "nmap -n -sP -oG - %s | grep \": Up\" | cut -d' ' -f2", ip);
	
  	if (system (cmd) < 0)
  	{
		fprintf(stderr, "[!] ERROR: Please install nmap.\n");
		free (cmd);
		return -1;
  	}
  	
  	free (cmd);
  	
  	return 0;
}

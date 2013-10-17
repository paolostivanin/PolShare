#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "landnd.h"

int get_connected_ip(const char *ip){
	char *cmd = NULL;
	const char *part_of_cmd = "nmap -n -sP -oG - | grep \": Up\" | cut -d' ' -f2";

	cmd = malloc(strlen(ip) + strlen(part_of_cmd) + 3);
	sprintf(cmd, "nmap -n -sP -oG - %s | grep \": Up\" | cut -d' ' -f2", ip);
	
  	if(system(cmd) < 0){
    	printf("Error on system function. Probably 'nmap' is not installed\n");
    	free(cmd);
    	return -1;
  	}
  	free(cmd);
  	return 0;
}

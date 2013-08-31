#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "landnd.h"

int get_connected_ip(void){
	char *input_ip = NULL, *cmd = NULL;
	const char *part_of_cmd = "nmap -n -sP -oG - | grep \": Up\" | cut -d' ' -f2";

	printf("READ THIS HELP:\n      EX 1: if your LAN IPv4 address is 192.168.1.14 then you have to write 192.168.1.0/24\n");
	printf("      EX 2: if your LAN IPv4 is 172.16.1.15 then you have to write 172.16.1.0/24\n");
	printf("      EX 3: if your LAN IPv4 is 192.168.0.27 then you have to write 192.168.0.0/24\n");
	printf("      So you have ONLY to replace YOUR LAST IPv4 NUMBER WITH A ZERO (0) AND ADD /24\n");

	printf("Write your LAN IPv4: ");
	if(scanf("%m[^\n]%*c", &input_ip) == EOF){
		printf("\nscanf error, exiting...\n");
		return -1;
	}
	cmd = malloc(strlen(input_ip) + strlen(part_of_cmd) + 3);
	sprintf(cmd, "nmap -n -sP -oG - %s | grep \": Up\" | cut -d' ' -f2", input_ip);
	
  	if(system(cmd) < 0){
    	printf("Error on system function");
    	free(input_ip);
    	free(cmd);
    	return -1;
  	}
  	free(input_ip);
  	free(cmd);
  	return 0;
}
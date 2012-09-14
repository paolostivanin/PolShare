#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "landnd.h"

int get_connected_ip(void){
	const char *cmd = "nmap -n -sP -oG - 192.168.1.0/24 | grep \": Up\" | cut -d' ' -f2";
  	if(system(cmd) < 0){
    	printf("Error on system function");
    	return -1;
  	}
  	return 0;
}
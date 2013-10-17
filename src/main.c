#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include "landnd.h"

int main(int argc, char **argv){
	static struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"send", required_argument, NULL, 's'},
		{"recv", required_argument, NULL, 'r'},
		{"list-ip", required_argument, NULL, 'i'},
		{NULL, 0, NULL, 0}
	};
	int ch;
	while ((ch = getopt_long(argc, argv, "v", long_options, NULL)) != -1){
			switch(ch){
			case 'v':
				printf("lanDND v%s developed by Paolo Stivanin <info@paolostivanin.com>\n", VERSION);
				return 0;
			
			case 'h':
				printf("Available options are:\n");
				printf("--send <ip>\t::= to send a file to the specified ip address\n");
				printf("--recv [y|n]\t::= to auto accept all incoming files OR to confirm every file before download\n");
				printf("--list-ip <ip>\t::= list all the ip addresses that are connected to your lan\n");
				printf("--version\t::= display version and developer info\n");
				printf("--help\t\t::= display this simple help :)\n");
				return 0;
			
			case '?':
				return -1;
			}
	}
	optind = 1;
	while ((ch = getopt_long(argc, argv, "s:r:i:", long_options, NULL)) != -1){
		switch (ch){
			case 's':
				do_action(1, optarg);
				return 0;

			case 'r':
				do_action(2, optarg);
				return 0;
			
			case 'i':
				do_action(3, optarg);
		}
	}
	return 0;
}

int do_action(int req, const char *opt){
	int ret_val = -1;
	if(req == 1){
		ret_val = do_send(opt);
		if(ret_val < 0){
			if(ret_val == -2) return 0;
			printf("Something went wrong, exiting...\n");
			return -1;
		}
	}
	else if(req == 2){
		ret_val = do_recv(opt);
		if(ret_val < 0){
			if(ret_val == -2) return 0;
			printf("Something went wrong, exiting...\n");
			return -1;
		}
	}
	else if(req == 3){
		printf("IPv4 that are connected in your LAN:\n");
		ret_val = get_connected_ip(opt);
  		if(ret_val < 0){
   			printf("Error during lan scan. You have to manually search to the active hosts into your LAN\n");
   			return -1;
  		}
  		printf("\n");
	}
	return 0;
}

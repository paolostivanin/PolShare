/* Descrizione: Share your files with PCs that are in your LAN
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.0.0-alpha3
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/LanDND>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "landnd.h"

int main(int argc, char **argv){
	if(argc > 2){
		printf("Usage: %s [-s]\n", argv[0]);
		return EXIT_FAILURE;
	}

  	int ret_val = -1, choice;

  	if(argc == 2){
  		if(strcmp(argv[1], "-s") == 0){
  			printf("IPv4 that are connected in your LAN:\n");
    		ret_val = get_connected_ip();
    		if(ret_val < 0){
      			printf("Error during lan scan, you have to manually input the server ip address\n");
    		}
    		printf("\n");
  		}
  	}

	printf("Type 1 for send or 2 for receive: ");
	if(scanf("%1d", &choice) != 1){
		printf("scanf ERROR, exiting...\n");
		return EXIT_FAILURE;
	}
	getchar(); /* necessario altrimenti scanf mi impalla il buffer di input */
	if(choice == 1){
		ret_val = do_send();
		if(ret_val < 0){
			if(ret_val == -2) return EXIT_SUCCESS;
			printf("Something went wrong, exiting...\n");
			return EXIT_FAILURE;
		}
	}
	else{
		ret_val = do_recv();
		if(ret_val < 0){
			if(ret_val == -2) return EXIT_SUCCESS;
			printf("Something went wrong, exiting...\n");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
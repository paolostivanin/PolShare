/* Descrizione: Share your files with PCs that are in your LAN
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.1-exp
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/LanDND>
 */

#define _GNU_SOURCE
#define VERSION "1.0-beta3"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "landnd.h"

int main(int argc, char **argv){

	if(argc > 3 || argc < 2){
		printf("Usage: %s <number> [-s] OR [-ya] OR [-sya] OR [-v] OR [-h]\n", argv[0]);
		return EXIT_FAILURE;
	}
	if(atoi(argv[1]) != 1){
		if(atoi(argv[1]) != 2){
			printf("Error: the <number> MUST be 1 (send) or 2 (receive)\n");
			return EXIT_FAILURE;
		}
	}

  	int ret_val = -1, choice = atoi(argv[1]), is_ya = 0;

  	if(argc == 3){
		if((strcmp(argv[2], "-s") == 0) || (strcmp(argv[2], "-sya") == 0)){
			printf("IPv4 that are connected in your LAN:\n");
  			ret_val = get_connected_ip();
  			if(ret_val < 0){
   				printf("Error during lan scan. You have to manually search to the active hosts into your LAN\n");
  			}
  			printf("\n");
  			if(strcmp(argv[2], "-sya") == 0) is_ya = 1;
		}
	}

	if(argc == 3){
		if(strcmp(argv[2], "-ya") == 0) is_ya = 1;
	}

	if(argc == 3){
		if(strcmp(argv[2], "-v") == 0){
  			printf("LanDND v%s developed by Paolo Stivanin <http://www.polslinux.it>\n", VERSION);
    		return EXIT_SUCCESS;
  		}
  	}

  	if(argc == 3){
  		if(strcmp(argv[2], "-h") == 0){
  			printf("--> HELP:\n");
  			printf("-s\t-> search to all the active hosts into your LAN and print their IPv4\n");
  			printf("-ya\t-> auto accept all the files you're going to receive\n");
  			printf("-sya\t-> -s option merged with -ya option so you could use\n\t   both of them in one time\n");
  			printf("-v\t-> display version info and developer info\n");
  			printf("-h\t-> display this simple help :)\n");
    		return EXIT_SUCCESS;
  		}
  	}

	if(choice == 1){
		ret_val = do_send();
		if(ret_val < 0){
			if(ret_val == -2) return EXIT_SUCCESS;
			printf("Something went wrong, exiting...\n");
			return EXIT_FAILURE;
		}
	}
	else{
		ret_val = do_recv(is_ya);
		if(ret_val < 0){
			if(ret_val == -2) return EXIT_SUCCESS;
			printf("Something went wrong, exiting...\n");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

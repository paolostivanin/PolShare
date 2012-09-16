/* Descrizione: Share your files with PCs that are in your LAN
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/LanDND>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "landnd.h"

char *get_ip_from_file(char *compare){
	char buf[BUFSIZ];
	char *copy = NULL, *token = NULL, *ip = NULL, *ip_file = NULL, *err = "ERR";
	FILE *fp;
	printf("Write the path to the file: ");
	if(scanf("%m[^\n]%*c", &ip_file) == EOF){
		printf("\nscanf ERROR, exiting...\n");
		return err;
	}
	fp = fopen(ip_file, "r");
	if(fp == NULL){
		printf("Error on opening file for read\n");
		return err;
	}
	while(fgets(buf, sizeof(buf), fp) != NULL){
		copy = strdup(buf);
		token = strtok(copy, " ");
		if(strcmp(token, compare) == 0){
			token = strtok(NULL, "\n");
			break;
		}
		else continue;
	}
	ip = (char *)malloc(strlen(token)+1);
	if(ip == NULL){
		printf("Error during memory allocation\n");
		return err;
	}
	strcpy(ip, token);
	free(ip_file);
	free(copy);
	fclose(fp);
	return err;
}
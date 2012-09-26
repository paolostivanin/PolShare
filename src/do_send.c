/* Descrizione: Share your files with PCs that are in your LAN
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/LanDND>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include "landnd.h"

int do_send(void){
	struct stat fileStat;
	int fd, sockd = -1, is_set = 0;
	uint32_t fsize = 0, size_to_send = 0, num_of_file = 0;
	off_t offset = 0;
	ssize_t rc, tx = 0;
	static char buffer[2], tmp_buf[BUFSIZ];
	char *ipv4_server = NULL, *tmp_input = NULL, *input_file = NULL, *dup_input = NULL;
	char *token = NULL, *tmp_token = NULL, *file = NULL, *is_gnome = "'";
	static struct sockaddr_in remote_server_addr;
	static struct hostent *hp;
	printf("Write the IPv4 server address: "); /* SE SCRIVE TIPO CUSTOM=HOME ALLORA CERCARE SUL FILE */
	if(fgets(tmp_buf, BUFSIZ, stdin) == NULL){
		printf("fgets error\n");
		return -1;
	} /* HO DOPPIO INSERIMENTO DI IP CON FGETS E SCANF...VEDERE COME SISTEMARE */
	token = strtok(tmp_buf, "=");
	if(strcmp(token, "CUSTOM") == 0){
		token = strtok(NULL, "\n");
		ipv4_server = get_ip_from_file(token);
		if(strcmp(ipv4_server, "ERR") == 0){
			printf("Cannot get the ip from the file, exiting...\n");
			return -1;
		}
		//goto new_start;
	}
	else{
		token = strtok(tmp_buf, "\n");
		ipv4_server = (char *)malloc(strlen(token)+1);
		strcpy(ipv4_server, token);
	}
	/*if(scanf("%m[^\n]%*c", &ipv4_server) == EOF){
		printf("\nscanf ERROR, exiting...\n");
		return -1;
	}
	new_start:*/
	/* Info sul server remoto */
	hp = gethostbyname(ipv4_server);
	remote_server_addr.sin_family = AF_INET;
	remote_server_addr.sin_port = 15000;
	remote_server_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr;

	if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error during socket creation\n");
		free(ipv4_server);
		return -1;
  	}

  	if(connect(sockd, (struct sockaddr *) &remote_server_addr, sizeof(remote_server_addr)) < 0){
  		printf("Connection error\n");
   		free(ipv4_server);
   		close(sockd);
   		return -1;
  	}

	printf("Drag (or write) the file(s) to send: ");
	if(scanf("%m[^\n]%*c", &tmp_input) == EOF){
		printf("\nscanf ERROR, exiting...\n");
   		free(ipv4_server);
   		close(sockd);
		return -1;
	}
	if(*tmp_input == *is_gnome) remove_char(tmp_input, '\''); /* se il primo carattere è ' allora lo tolgo */
	else{ is_set = 1; input_file = strdup(tmp_input); } /* altrimenti copio il tutto com'è */
	token = NULL;
	dup_input = strdup(input_file);
	token = strtok(dup_input, " ");
	while(token != NULL){
		token = strtok(NULL, " ");
		num_of_file++;
	}
	free(dup_input);
	token = NULL;
	if(send(sockd, &num_of_file, sizeof(num_of_file), 0) < 0){
		printf("Failed to send the number of files\n");
		if(is_set == 1) free(input_file);
		free(tmp_input);
		free(ipv4_server);
		free(input_file);
		close(sockd);
		return -1;
	}
	tmp_token = strtok(input_file, " ");
	while(tmp_token != NULL){
		rc = offset = 0;
		if(stat(tmp_token, &fileStat) < 0){
			printf("stat ERROR, exiting...\n");
			if(is_set == 1) free(input_file);
			free(tmp_input);
    		free(ipv4_server);
			free(input_file);
			close(sockd);
			return -1;
		}
		if(S_ISDIR(fileStat.st_mode)){
			printf("ERROR: This is a directory not file, exiting...\n");
			free(tmp_input);
			if(is_set == 1) free(input_file);
    		free(ipv4_server);
    		close(sockd);
			return -1;
		}
		file = basename(tmp_token); /* basename potrebbe modificare input_file così ne passo una copia e sono al sicuro */
		fsize = strlen(file)+1;
		if(send(sockd, &fsize, sizeof(fsize), 0) < 0){
			printf("Error on sending the file name length\n");
    		free(ipv4_server);
    		if(is_set == 1) free(input_file);
    		free(tmp_input);
			close(sockd);
			return -1;
		}
		if(send(sockd, file, fsize, 0) < 0){
			printf("Error on sending the file name\n");
    		free(ipv4_server);
	    	if(is_set == 1) free(input_file);
    		free(tmp_input);
			close(sockd);
			return -1;
		}
		fsize = fileStat.st_size;
		if(send(sockd, &fsize, sizeof(fsize), 0) < 0){
			printf("Error on sending the file size\n");
    		free(ipv4_server);
    		if(is_set == 1) free(input_file);
    		free(tmp_input);	
			close(sockd);
			return -1;
		}
		printf("Waiting for server confirmation...\n");
		if(recv(sockd, buffer, 2, 0) < 0){
			printf("Error on receiving Y or N\n");
    		free(ipv4_server);
    		if(is_set == 1) free(input_file);
    		free(tmp_input);	
			close(sockd);
			return -1;		
		}
		if(strcmp(buffer, "N") == 0){
			printf("The server has aborted the file transfer\n");
    		free(ipv4_server);
    		if(is_set == 1) free(input_file);
    		free(tmp_input);	
			close(sockd);
			return -2;
		}
  		fd = open(tmp_token, O_RDONLY);
  		if(fd < 0){
	   		printf("Error on opening file\n");
   			free(ipv4_server);
   			if(is_set == 1) free(input_file);
   			free(tmp_input);	  
	  		close(sockd);
   			return -1;
  		}
		for(size_to_send = fsize; size_to_send > 0; ){
	    	rc = sendfile(sockd, fd, &offset, size_to_send);
    		if(rc <= 0){
	    		printf("Error on sendfile");
      			close(fd);
  	  			if(is_set == 1) free(input_file);
      			free(ipv4_server);      	
      			free(tmp_input);
      			close(sockd);
      			return -1;
    		}
    	    tx += rc;
   			printf("\r%d%%", (tx * 100 / fsize));
    		fflush(NULL);
    		size_to_send -= rc;
  		}
  		close(fd);
  		tmp_token = strtok(NULL, " ");
  	}
  	free(tmp_input);
 	if(is_set == 1) free(input_file);
	free(ipv4_server);
  	printf("--> File successfully sent\n");
	close(sockd);
	return 0;
}
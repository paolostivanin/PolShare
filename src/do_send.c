#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
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

int do_send(const char *ip){
	struct stat fileStat;
	int fd, sockd = -1;
	uint32_t fsize = 0, size_to_send = 0;
	off_t offset = 0;
	ssize_t rc, tx = 0;
	static char buffer[2], hash[33];
	char *tmp_input = NULL, *input_file = NULL;
	char *file = NULL, *is_gnome = "'";
	static struct sockaddr_in remote_server_addr;
	static struct hostent *hp;
	
	/* Info sul server remoto */
	hp = gethostbyname(ip);
	remote_server_addr.sin_family = AF_INET;
	remote_server_addr.sin_port = 15000;
	remote_server_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr;

	if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error during socket creation\n");
		return -1;
  	}

  	if(connect(sockd, (struct sockaddr *) &remote_server_addr, sizeof(remote_server_addr)) < 0){
  		printf("Connection error\n");
   		close(sockd);
   		return -1;
  	}

	printf("Drag (or write) the file to send: ");
	if(scanf("%m[^\n]%*c", &tmp_input) == EOF){
		printf("\nscanf ERROR, exiting...\n");
   		close(sockd);
		return -1;
	}
	if(*tmp_input == *is_gnome) remove_char(tmp_input, '\''); /* se il primo carattere è ' allora lo tolgo */
	input_file = strdup(tmp_input);

	rc = offset = 0;
	if(tmp_input[0] == '.' && tmp_input[1] == '.'){
		printf("You cannot use relative path\n");
		free(tmp_input);
		free(input_file);
		close(sockd);
		return -1;
	}
	if(strlen(tmp_input) > 5){
		char *start = NULL;
		start = malloc(6);
		if(start == NULL){
			printf("memory allocation error\n");
			free(tmp_input);
			free(input_file);
			close(sockd);
			return -1;
		}
		memcpy(start, tmp_input, 5);
		start[6] = '\0';
		if(strcmp(start, "/home") != 0){
			printf("You cannot send file from outside your home directory\n");
			free(tmp_input);
			free(start);
			free(input_file);
			close(sockd);
			return -1;
		}
		free(start);
	}
	if(stat(tmp_input, &fileStat) < 0){
		printf("stat ERROR, exiting...\n");
		free(input_file);
		free(tmp_input);
		free(input_file);
		close(sockd);
		return -1;
	}
	if(S_ISDIR(fileStat.st_mode)){
		printf("ERROR: This is a directory not file, exiting...\n");
		free(tmp_input);
		free(input_file);
    		close(sockd);
		return -1;
	}
	file = basename(input_file); /* basename potrebbe modificare input_file così ne passo una copia e sono al sicuro */
	fsize = strlen(file)+1;
	if(send(sockd, &fsize, sizeof(fsize), 0) < 0){
		free(input_file);
    		free(tmp_input);
		close(sockd);
		return -1;
	}
	if(send(sockd, file, fsize, 0) < 0){
		printf("Error on sending the file name\n");
	    	free(input_file);
    		free(tmp_input);
		close(sockd);
		return -1;
	}
	fsize = fileStat.st_size;
	if(send(sockd, &fsize, sizeof(fsize), 0) < 0){
		printf("Error on sending the file size\n");
    		 free(input_file);
    		free(tmp_input);	
		close(sockd);
		return -1;
	}
	printf("Waiting for server confirmation...\n");
	if(recv(sockd, buffer, 2, 0) < 0){
		printf("Error on receiving Y or N\n");
    		free(input_file);
    		free(tmp_input);	
		close(sockd);
		return -1;		
	}
	if(strcmp(buffer, "N") == 0){
		printf("The server has aborted the file transfer\n");
    		free(input_file);
    		free(tmp_input);	
		close(sockd);
		return -2;
	}
  	fd = open(tmp_input, O_RDONLY | O_NOFOLLOW);
  	if(fd < 0){
		printf("Error on opening file\n");
   		free(input_file);
   		free(tmp_input);	  
		close(sockd);
   		return -1;
  	}

  	tx = 0;
	for(size_to_send = fsize; size_to_send > 0; ){
		rc = sendfile(sockd, fd, &offset, size_to_send);
    		if(rc <= 0){
	    		printf("Error on sendfile");
      			close(fd);
  	  		free(input_file);
      			free(tmp_input);
      			close(sockd);
      			return -1;
    		}
    	    	tx += rc;
   		printf("\r%zd%%", (tx * 100 / fsize));
    		fflush(NULL);
    		size_to_send -= rc;
  	}
  	memset(buffer, 0, sizeof(buffer));
  	char *file_md5 = check_md5(tmp_input);
  	strcpy(hash, file_md5);
  	hash[32] = '\0';
  	if(send(sockd, hash, 33, 0) < 0){
		printf("Error on sending file hash\n");
    		free(input_file);
    		free(tmp_input);
    		free(file_md5);	
		close(sockd);
		return -1;
	}
  	if(recv(sockd, buffer, 2, 0) < 0){
		printf("Error on receiving file hash confirmation\n");
    		free(input_file);
    		free(tmp_input);
    		free(file_md5);	
		close(sockd);
		return -1;
	}
	if(strcmp(buffer, "Y") == 0){
		printf("\n--> md5sum matches\n");
		printf("--> File successfully transferred\n");
	}
	else{
		printf("\n--> md5sum DOESN'T matches. File transfer FAILED\n");
	}
  	free(file_md5);
  	close(fd);
  	free(tmp_input);
 	free(input_file);
	close(sockd);
	return 0;
}

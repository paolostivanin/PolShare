#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <signal.h>
#include "landnd.h"

static void signal_handler(int);
static void cleanup(void);
static void init_signals(void);
static void panic(const char *, ...);

struct sigaction sigact;

struct _socketInfo{
	int fd, sockd, newsockd;
} socketStruct;

int do_recv(const char *acceptOrNot){
	if(get_ipv4() < 0){
		printf("ERROR on getting IPv4 address, exiting...\n");
		return -1;
	}
	uint32_t fsize = 0, total_bytes_read = 0, fsize_tmp;
	ssize_t nread = 0, tx = 0;
	size_t socket_len = 0;
	void *filebuffer = NULL;
	char *filename = NULL;
	static char yORn[2], hash[33];
	static struct sockaddr_in local_server_addr, cli_addr;
	/* Info sul server locale */
	local_server_addr.sin_family = AF_INET;
	local_server_addr.sin_port = 15000;
	local_server_addr.sin_addr.s_addr = INADDR_ANY;
	
	atexit(cleanup);
	init_signals();

	if((socketStruct.sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error on socket creation\n");
		return -1;
	}

	if(bind(socketStruct.sockd, (struct sockaddr *) &local_server_addr, sizeof(local_server_addr)) < 0){
		printf("Bind error\n");
		return -1;
	}
	
	if(listen(socketStruct.sockd, 20) < 0){
		printf("Error on listen");
		return -1;
	}
	new_conn:
	socket_len = sizeof(cli_addr);
	if((socketStruct.newsockd = accept(socketStruct.sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
    	perror("Connection error (accept)\n");
    	return -1;
  	}
	
	fsize = total_bytes_read = nread = 0;
    memset(yORn, 0, sizeof(yORn));
    if(recv(socketStruct.newsockd, &fsize, sizeof(fsize), 0) < 0){
    	printf("Error on receiving the file name length\n");
   		return -1;
    }
    filename = (char *)malloc(fsize);
    if(filename == NULL){
		printf("Error during filename memory allocation\n");
     	return -1;
    }
    if(recv(socketStruct.newsockd, filename, fsize, 0) < 0){
   		printf("Error on receiving the file name\n");
   		free(filename);
   		return -1;
    }
    fsize = 0;
    if(recv(socketStruct.newsockd, &fsize, sizeof(fsize), 0) < 0){
		printf("Error on receiving the file size\n");
   		free(filename);
		return -1;
    }
    
	if(strcmp(acceptOrNot, "y") == 0){
      	strcpy(yORn, "Y");
      	goto auto_accept;
    }

    printf("Do you want to receive the file '%s' which size is '%"PRIu32"' bytes? (Y or N)\n", filename, fsize);
    another_yorn:
    if(scanf("%1s", yORn) == EOF){
	   	printf("Scanf error\n");
    	free(filename);
    	return -1;
    }
    yORn[0] = toupper(yORn[0]);
    if(strcmp(yORn, "Y") != 0){
	   	if(strcmp(yORn, "N") != 0){
      		printf("You have to write Y or N, try again: ");
      		memset(yORn, 0, sizeof(yORn));
      		goto another_yorn;
       	}
    }
    auto_accept:
    if(strcmp(yORn, "N") == 0){
	   	printf("Transfer aborted\n");
    	if(send(socketStruct.newsockd, yORn, 2, 0) < 0){
	     	printf("Error on sending N\n");
      		free(filename);
      		return -1;   
		}
      	close(socketStruct.newsockd);
      	free(filename);
      	goto new_conn;
    }
    else{
		if(send(socketStruct.newsockd, yORn, 2, 0) < 0){
			printf("Error on sending Y\n");
			free(filename);
			return -1;
		}
	}
    fsize_tmp = fsize;
    filebuffer = malloc(fsize);
    if(filebuffer == NULL){
		printf("Error during filebuffer memory allocation\n");
      	free(filename);
      	return -1;
    }
	socketStruct.fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if(socketStruct.fd  < 0) {
		printf("Error during file opening\n");
      	free(filename);
      	free(filebuffer);
   	  	return -1;
    }
    tx = 0;
    while((total_bytes_read != fsize) && ((nread = read(socketStruct.newsockd, filebuffer, fsize_tmp)) > 0)){
		tx += nread;
      	printf("\r%zd%%", (tx * 100 / fsize));
   	  	if(write(socketStruct.fd, filebuffer, nread) != nread){
	   	    printf("Write error\n");
     	  	free(filename);
  		  	free(filebuffer);
     	  	return -1;
   		}
   	  	total_bytes_read += nread;
   	  	fsize_tmp -= nread;
    }
    char *file_md5 = check_md5(filename);
    if(recv(socketStruct.newsockd, hash, 33, 0) < 0){
	    printf("Error on receiving file md5\n");
      	free(filename);
      	free(file_md5);
      	return -1;
    }
    if(strcmp(file_md5, hash) == 0){
	    memset(yORn, 0, sizeof(yORn));
		strcpy(yORn, "Y");
      	if(send(socketStruct.newsockd, yORn, 2, 0) < 0){
        	free(filename);
        	free(file_md5);
        	return -1;
      	}
      	printf("\n--> File successfully received\n");
    }
    else{
	  	memset(yORn, 0, sizeof(yORn));
      	strcpy(yORn, "N");
      	if(send(socketStruct.newsockd, yORn, 2, 0) < 0){
	        close(socketStruct.newsockd);
        	close(socketStruct.fd);
        	free(filename);
        	free(file_md5);
			goto new_conn;
      	}
      	printf("\n--> File transfer FAILED, md5sum doesn't match\n");
    }
    free(file_md5);
    close(socketStruct.fd);
    free(filename);
    free(filebuffer);
    close(socketStruct.newsockd);
    goto new_conn;
}

void init_signals(void){
	sigact.sa_handler = signal_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
   	sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
}

static void signal_handler(int sig){
    if(sig == SIGINT) panic("\nCtrl-C caught, exiting...\n");
}

void panic(const char *fmt, ...){
    char buf[50];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buf, fmt, argptr);
    va_end(argptr);
    fprintf(stderr, "%s\n", buf);
    exit(EXIT_SUCCESS);
}

void cleanup(void){
    sigemptyset(&sigact.sa_mask);
    if(socketStruct.fd) close(socketStruct.fd);
    if(socketStruct.newsockd) close(socketStruct.newsockd);
    if(socketStruct.sockd) close(socketStruct.sockd);
}




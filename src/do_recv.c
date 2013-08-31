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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include "landnd.h"

int do_recv(const int is_ya_set){
	if(get_ipv4() < 0){
		printf("ERROR on getting IPv4 address, exiting...\n");
		return -1;
	}
	uint32_t fsize = 0, total_bytes_read = 0, fsize_tmp, client_counter, counter = 0;
	int fd, sockd = -1, newsockd = -1;
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

	if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error on socket creation\n");
		return -1;
	}

	if(bind(sockd, (struct sockaddr *) &local_server_addr, sizeof(local_server_addr)) < 0){
		printf("Bind error\n");
		close(sockd);
		return -1;
	}

	if(listen(sockd, 20) < 0){
		printf("Error on listen");
		close(sockd);
		return -1;
	}
	
  socket_len = sizeof(cli_addr);
	if((newsockd = accept(sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
    perror("Connection error (accept)\n");
    close(sockd);
    return -1;
  }
  if(recv(newsockd, &client_counter, sizeof(client_counter), 0) < 0){
    printf("Error on the number of files\n");
    close(newsockd);
    close(sockd);
    return -1;
  }
  while(counter < client_counter){
    fsize = total_bytes_read = nread = 0;
    memset(yORn, 0, sizeof(yORn));
    if(recv(newsockd, &fsize, sizeof(fsize), 0) < 0){
      printf("Error on receiving the file name length\n");
   	  close(newsockd);
   	  close(sockd);
   	  return -1;
    }
    filename = (char *)malloc(fsize);
    if(filename == NULL){
      printf("Error during filename memory allocation\n");
      close(newsockd);
      close(sockd);
    }
    if(recv(newsockd, filename, fsize, 0) < 0){
   	  printf("Error on receiving the file name\n");
   	  close(newsockd);
   	  close(sockd);
   	  free(filename);
   	  return -1;
    }
    fsize = 0;
    if(recv(newsockd, &fsize, sizeof(fsize), 0) < 0){
   	  printf("Error on receiving the file size\n");
   	  close(newsockd);
   	  close(sockd);
   	  free(filename);
   	  return -1;
    }
    
    if(is_ya_set == 1){
      strcpy(yORn, "Y");
      goto auto_accept;
    }

    printf("Do you want to receive the file '%s' which size is '%"PRIu32"' bytes? (Y or N)\n", filename, fsize);
    another_yorn:
    if(scanf("%1s", yORn) == EOF){
      printf("Scanf error\n");
      close(newsockd);
      close(sockd);
      free(filename);
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
      if(send(newsockd, yORn, 2, 0) < 0){
        printf("Error on sending N\n");
        close(newsockd);
        close(sockd);
        free(filename);    
      }
      close(newsockd);
      close(sockd);
      free(filename);
      return -2;
    }
    else{
      if(send(newsockd, yORn, 2, 0) < 0){
        printf("Error on sending Y\n");
        close(newsockd);
        close(sockd);
        free(filename);    
      }
    }
    fsize_tmp = fsize;
    filebuffer = malloc(fsize);
    if(filebuffer == NULL){
      printf("Error during filebuffer memory allocation\n");
      close(newsockd);
      close(sockd);
      free(filename);
    }
    fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if(fd  < 0) {
   	  printf("Error during file opening\n");
   	  close(newsockd);
   	  close(sockd);
      free(filename);
      free(filebuffer);
   	  return -1;
    }
    tx = 0;
    while((total_bytes_read != fsize) && ((nread = read(newsockd, filebuffer, fsize_tmp)) > 0)){
      tx += nread;
      printf("\r%zd%%", (tx * 100 / fsize));
   	  if(write(fd, filebuffer, nread) != nread){
   	    printf("Write error\n");
  		  close(newsockd);
   	    close(sockd);
        close(fd);
     	  free(filename);
  		  free(filebuffer);
     	  return -1;
   	  }
   	  total_bytes_read += nread;
   	  fsize_tmp -= nread;
    }
    char *file_md5 = check_md5(filename);
    if(recv(newsockd, hash, 33, 0) < 0){
      printf("Error on receiving file md5\n");
      close(newsockd);
      close(sockd);
      close(fd);
      free(filename);
      free(file_md5);
      return -1;
    }
    if(strcmp(file_md5, hash) == 0){
      memset(yORn, 0, sizeof(yORn));
      strcpy(yORn, "Y");
      if(send(newsockd, yORn, 2, 0) < 0){
        close(newsockd);
        close(sockd);
        close(fd);
        free(filename);
        free(file_md5);
        return -1;
      }
      printf("\n--> md5sum matches\n");
      printf("--> File successfully transferred\n");
    }
    else{
      memset(yORn, 0, sizeof(yORn));
      strcpy(yORn, "N");
      if(send(newsockd, yORn, 2, 0) < 0){
        close(newsockd);
        close(sockd);
        close(fd);
        free(filename);
        free(file_md5);
        return -1;
      }
      printf("--> md5sum NOT matches\n");
      printf("\n--> File transfer FAILED\n");
    }
    free(file_md5);
    close(fd);
    free(filename);
    free(filebuffer);
    //printf("\n");
    counter++;
  }
  close(newsockd);
  close(sockd);
  return 0;
}
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

int do_send(void);
int do_recv(void);
int get_ipv4(void);

int main(int argc, char **argv){
	if(argc != 1){
		printf("Usage: %s\n", argv[0]);
		return EXIT_FAILURE;
	}
		
	int ret_val = -1, choice;

	printf("Type 1 for send or 2 for receive: ");
	if(scanf("%1d", &choice) != 1){
		printf("scanf ERROR, exiting...\n");
		return EXIT_FAILURE;
	}
	getchar(); /* necessario altrimenti scanf mi impalla il buffer di input */
	if(choice == 1){
		ret_val = do_send();
		if(ret_val < 0){
			printf("Something went wrong, exiting...\n");
			return EXIT_FAILURE;
		}
	}
	else{
		ret_val = do_recv();
		if(ret_val < 0){
			printf("Something went wrong, exiting...\n");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int do_send(void){
	struct stat fileStat;
	int fd, sockd = -1, is_set = 0;
	uint32_t fsize = 0, size_to_send = 0;
	off_t offset = 0;
	ssize_t rc;
	char *ipv4_server = NULL, *tmp_input = NULL, *input_file = NULL, *token = NULL, *file = NULL, *is_gnome = "'";
	static struct sockaddr_in remote_server_addr;
	static struct hostent *hp;
	printf("Write the IPv4 server address: ");
	if(scanf("%m[^\n]%*c", &ipv4_server) == EOF){
		printf("\nscanf ERROR, exiting...\n");
		return -1;
	}
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

	printf("Drag here the file to send: ");
	if(scanf("%m[^\n]%*c", &tmp_input) == EOF){
		printf("\nscanf ERROR, exiting...\n");
    	free(ipv4_server);
    	close(sockd);
		return -1;
	}
	if(*tmp_input == *is_gnome) input_file = strtok(tmp_input, "'"); /* se il primo carattere è ' allora lo tolgo */
	else{ is_set = 1; input_file = strdup(tmp_input); } /* altrimenti copio il tutto com'è */
	if(stat(input_file, &fileStat) < 0){
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
	token = strdup(input_file);
	file = basename(token); /* basename potrebbe modificare input_file così ne passo una copia e sono al sicuro */
	fsize = strlen(file)+1;
	if(send(sockd, &fsize, sizeof(fsize), 0) < 0){
		printf("Error on sending the file name length\n");
    	free(ipv4_server);
    	if(is_set == 1) free(input_file);
    	free(tmp_input);
		free(token);
		close(sockd);
		return -1;
	}
	if(send(sockd, file, fsize, 0) < 0){
		printf("Error on sending the file name length\n");
    	free(ipv4_server);
    	if(is_set == 1) free(input_file);
    	free(tmp_input);
		free(token);
		close(sockd);
		return -1;
	}
	fsize = fileStat.st_size;
	if(send(sockd, &fsize, sizeof(fsize), 0) < 0){
		printf("Error on sending the file size\n");
    	free(ipv4_server);
    	if(is_set == 1) free(input_file);
    	free(tmp_input);
		free(token);
		close(sockd);
		return -1;
	}
  	fd = open(input_file, O_RDONLY);
  	if(fd < 0){
    	printf("Error on opening file\n");
    	free(ipv4_server);
    	if(is_set == 1) free(input_file);
    	free(tmp_input);
		free(token);
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
			free(token);
			free(tmp_input);
			close(sockd);
      		return -1;
    	}
    	size_to_send -= rc;
  	}
  	close(fd);
  	free(tmp_input);
  	if(is_set == 1) free(input_file);
	free(ipv4_server);
	free(token);
	close(sockd);
	return 0;
}

int do_recv(void){
	if(get_ipv4() < 0){
		printf("ERROR on getting IPv4 address, exiting...\n");
		return -1;
	}
	uint32_t fsize = 0, total_bytes_read = 0, fsize_tmp;
	int fd, sockd = -1, newsockd = -1, nread = 0;
	size_t socket_len = 0;
	void *filebuffer = NULL;
	char *filename = NULL;
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
    	printf("Error on receiving the file size\n");
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
    fsize_tmp = fsize;
    filebuffer = malloc(fsize);
    if(filebuffer == NULL){
    	printf("Error during filebuffer memory allocation\n");
    	close(newsockd);
    	close(sockd);
    	free(filename);
    }
  	fd = open(filename, O_CREAT | O_WRONLY, 0644);
  	if (fd  < 0) {
    	printf("Error during file opening\n");
    	close(newsockd);
    	close(sockd);
      	free(filename);
  		free(filebuffer);
    	return -1;
  	}
  	while((total_bytes_read != fsize) && ((nread = read(newsockd, filebuffer, fsize_tmp)) > 0)){
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
  	close(fd);
  	free(filename);
  	free(filebuffer);
    close(newsockd);
    close(sockd);
    return 0;
}

int get_ipv4(void){
    struct ifaddrs *iflist, *iface;

    if (getifaddrs(&iflist) < 0) {
        perror("getifaddrs");
        return -1;
    }

    for(iface = iflist; iface; iface = iface->ifa_next){
    	int af = iface->ifa_addr->sa_family;
        const void *addr;
        char addrp[INET_ADDRSTRLEN];

        switch(af){
            case AF_INET:
                addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
                break;
            default:
                addr = NULL;
        }

        if(addr){
            if(inet_ntop(af, addr, addrp, sizeof addrp) == NULL){
                perror("inet_ntop");
                return -1;
            }

            if((strcmp(iface->ifa_name, "lo") == 0) || (strcmp(addrp, "127.0.0.1") == 0) || (strcmp(addrp, "127.0.1.1") == 0) || (strcmp(addrp, "127.0.0.0") == 0) || (strcmp(addrp, "::1") == 0) || (strcmp(addrp, "::0") == 0)) continue;
            else printf("The server LAN IPv4 is: %s\n", addrp);
        }
    }

    freeifaddrs(iflist);
    return 0;
}
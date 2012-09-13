#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

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
	char *ipv4_server = NULL, *input_file = NULL, *token = NULL, *file = NULL;
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

	printf("Drag here the file to send: ");
	if(scanf("%m[^\n]%*c", &input_file) == EOF){
		printf("\nscanf ERROR, exiting...\n");
		return -1;
	}
	if(stat(input_file, &fileStat) < 0){
		printf("stat ERROR, exiting...\n");
		free(input_file);
		return -1;
	}
	if(S_ISDIR(fileStat.st_mode)){
		printf("ERROR: This is a directory not file, exiting...\n");
		free(input_file);
		return -1;
	}
	token = strdup(input_file);
	file = basename(token); /* basename potrebbe modificare input_file così ne passo una copia e sono al sicuro */
	free(ipv4_server);
	free(input_file);
	free(token);
	return 0;
}

int do_recv(void){
	if(get_ipv4() < 0){
		printf("ERROR on getting IPv4 address, exiting...\n");
		return -1;
	}
	static struct sockaddr_in local_server_addr, cli_addr;
	/* Info sul server locale */
	local_server_addr.sin_family = AF_INET;
	local_server_addr.sin_port = 15000;
	local_server_addr.sin_addr.s_addr = INADDR_ANY;

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
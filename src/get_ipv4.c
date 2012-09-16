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
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "landnd.h"

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
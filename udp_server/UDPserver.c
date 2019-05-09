//
// Created by Max Pokidaylo on 2019-05-09.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "FileData.h"
#define BUFSIZE 1024


int main() {
    int sockfd; /* socket */
    int portno = 48772; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    int pid;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return EXIT_FAILURE;
    }

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* allow any IP address to connect */
    serveraddr.sin_port = htons((unsigned short) portno);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("ERROR on binding");
        return EXIT_FAILURE;
    }
    printf("server is listening on %d\n", portno);
    /*
     * main loop: wait for a datagram, then echo it
     */
    clientlen = sizeof(clientaddr);
    char buffer[1024];
    FILE *readME;
    readME = fopen("TCPMainManu.txt", "r");
    if (readME) {
        while (fgets(buffer, 1024, readME) != NULL) {
            strcat(buffer, "\0");
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
        }
    }

    sendto(sockfd, "Welcome\n", strlen("Welcome\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
    while (1) {
        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(buf, BUFSIZE);
        n = (int) recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen);
        if (n < 0) {
            perror("ERROR in recvfrom");
            return EXIT_FAILURE;
        }
        if (strcmp(buf,"exit\n") == 0) {
            sendto(sockfd, "Goodbye!\n", strlen("Goodbye!\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
            break;
        } else {
            option2(sockfd,buf,clientaddr,clientlen);
        }
    }
}

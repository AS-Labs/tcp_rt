#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

int main(int argc, char **argv) {
    
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *item1;
    char *item2;
    char *delimiter = ":";

    u_short port;                /* user specified port number */
    char *addr;                  /* will be a pointer to the address */
    struct sockaddr_in address;  /* the libc network address data structure */
    short int sock = -1;         /* file descriptor for the network socket */
    fd_set fdset;
    struct timeval tv;

    if (argc != 2) {
        fprintf(stderr, "Usage %s filename\n", argv[0]);
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    printf("time(unix),server ip,port,latency(ms)\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        item1 = strtok(line, delimiter);
        item2 = strtok(NULL, delimiter);
        connection_lat(item1, item2);
    }


    port = atoi(argv[1]);
    addr = argv[2];

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(addr); /* assign the address */
    address.sin_port = htons(port);            /* translate int2port num */

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    connect(sock, (struct sockaddr *)&address, sizeof(address));

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = 2;             /* 2 second timeout */
    tv.tv_usec = 0;

    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            printf("%s:%d is open\n", addr, port);
        }
    }

    close(sock);
    return 0;
}


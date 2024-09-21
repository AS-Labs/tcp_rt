#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>





int connection_lat(char *ip_var, char *port_var) {
    // first arg is the target servers IP
    const char *server_ip = ip_var;
    // second arg is the target port, converting it from string to int
    int PORT = atoi(port_var);

    // Creating a socket, and sock is where we place the fd from the socket syscall.
    // The arguments of socket(int domain, int type, int protocol)
    // domain = AF_INET i.e IPv4
    // type = SOCK_STREAM i.e TCP socket?
    // protocol = 0 i.e the system will choose this by default but 0 is usually used for TCP
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);

    // in defining the sockaddr_in as SERVERIP
    // we get access to the struct of sockaddr_in
    // which gives us access to the below:
    // short            sin_family;
    // unsigned short   sin_port;   
    // struct in_addr   sin_addr;
    // char             sin_zero[8];
    //

    fd_set fdset;
    struct timeval tv;

    struct sockaddr_in SERVERIP;
    // specify the SERVERIP as IPv4
    SERVERIP.sin_family = AF_INET;
    // convert the unsigned short int from host byte order to network byte order
    SERVERIP.sin_port = htons(PORT);
    // convert IPv4 from text to binary as a pointer to the character string for IPv4
    inet_pton(AF_INET, server_ip, &(SERVERIP.sin_addr));

    struct timeval start_time, end_time;

    

    // timeval gives the time_t tv_sec in seconds and assigned to a pointer start_time var
    // timezone is given as null, as that is not required?
    gettimeofday(&start_time, NULL);



    // initiate a connection on a socket with the connect() syscall.
    // if addr length is -1 then close the socket fd and return 1.
    // handling this for if the connection is refused or does not exist.
    if (connect(sock, (struct sockaddr *)&SERVERIP, sizeof(SERVERIP)) == -1) {
        perror("connect");
        close(sock);
        return 1;
    }

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = 2;
    tv.tv_sec = 0;

    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if(so_error == 0) {
            printf("%s:%d open\n", server_ip, PORT);
        }
    }

    // getting the end time after doing the connection to measure response
    gettimeofday(&end_time, NULL);

    // close the socket to release the fd
    close(sock);

    // converting the times to ms
    long STR_MS = start_time.tv_sec * 1000 + start_time.tv_usec / 1000;
    long END_MS = end_time.tv_sec * 1000 + end_time.tv_usec / 1000;
    long response_time = END_MS - STR_MS;

    // output
    printf("%lu,%s,%d,%ld\n",(unsigned long)time(NULL) ,server_ip, PORT, response_time);

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Example: %s filename\n", argv[0]);
        return 1;
    }

    // Create file read loop where server IP and port are parsed and sent through a function that takes the two as arguments

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *item1;
    char *item2;
    char *delimiter = ":";

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    printf("time(unix),server ip,port,latency(ms)\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        item1 = strtok(line, delimiter);
        item2 = strtok(NULL, delimiter);
        connection_lat(item1, item2);
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
    return 0;
}

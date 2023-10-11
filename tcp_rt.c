#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Example: %s SERVERIP PORT\n", argv[0]);
        return 1;
    }

    // first arg is the target servers IP
    const char *server_ip = argv[1];
    // second arg is the target port, converting it from string to int
    int port = atoi(argv[2]);

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

    // in defining the sockaddr_in as server_addr
    // we get access to the struct of sockaddr_in
    // which gives us access to the below:
    // short            sin_family;
    // unsigned short   sin_port;   
    // struct in_addr   sin_addr;
    // char             sin_zero[8];
    struct sockaddr_in server_addr;
    // specify the server_addr as IPv4
    server_addr.sin_family = AF_INET;
    // convert the unsigned short int from host byte order to network byte order
    server_addr.sin_port = htons(port);
    // convert IPv4 from text to binary as a pointer to the character string for IPv4
    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));

    struct timeval start_time, end_time;

    // timeval gives the time_t tv_sec in seconds and assigned to a pointer start_time var
    // timezone is given as null, as that is not required?
    gettimeofday(&start_time, NULL);

    // initiate a connection on a socket with the connect() syscall
    // arg1 = int fd;
    // arg2 = struct sockaddr 
    // arg3 = addr length
    // if addr length is -1 then close the socket fd and return 1
    // handling this for if the connection is refused or does not exist.
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock);
        return 1;
    }

    // getting the end time after doing the connection to measure response
    gettimeofday(&end_time, NULL);

    // close the socket to release the fd
    close(sock);

    // converting the times to ms
    long start_ms = start_time.tv_sec * 1000 + start_time.tv_usec / 1000;
    long end_ms = end_time.tv_sec * 1000 + end_time.tv_usec / 1000;
    long response_time = end_ms - start_ms;

    // our output
    printf("%s,%d,%ld ms\n", server_ip, port, response_time);

    return 0;
}

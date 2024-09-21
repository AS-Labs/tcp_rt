#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

void print_usage(const char *program_name) {
    printf("Usage: %s <filename>\n", program_name);
}

long get_unix_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

double calculate_latency(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}

int connect_to_server(const char *ip, int port, int timeout_seconds) {
    struct sockaddr_in server_address;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0) {
        close(sockfd);
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    char line[256];

    printf("Time,ip,port,latency\n");

    while (fgets(line, sizeof(line), file)) {
        char *ip = strtok(line, ":");
        char *port_str = strtok(NULL, ":");
        int port = atoi(port_str);

        struct timeval start, end;
        gettimeofday(&start, NULL);

        int sockfd = connect_to_server(ip, port, 1);
        if (sockfd < 0) {
            long current_time = get_unix_time();
            printf("%ld,%s,%d,timeout\n", current_time, ip, port);
            continue;
        }

        gettimeofday(&end, NULL);
        double latency = calculate_latency(start, end);

        long current_time = get_unix_time();
        printf("%ld,%s,%d,%.2f\n", current_time, ip, port, latency);

        close(sockfd);
    }

    fclose(file);
    return 0;
}


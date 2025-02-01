#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

#define ACK_BUFFER_SIZE 64
#define TIMEOUT_SEC 2
#define TIMEOUT_USEC 0

void communicate(int sock, struct sockaddr_in *server_addr, FILE *file) {
    socklen_t addr_len = sizeof(*server_addr);

    /* Get file size and determine number of packages */
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    long total_packages = ceil((double)fileSize / (BUFFER_SIZE - 6));
    fseek(file, 0, SEEK_SET);

    /* Send file to server in multiples packages */
    size_t bytes_read;
    long actual_package = 0;
    char buffer[BUFFER_SIZE], data[BUFFER_DATA_SIZE], ack_buffer[ACK_BUFFER_SIZE];
    while ((bytes_read = fread(data, sizeof(char), BUFFER_DATA_SIZE, file)) > 0) {
        /* Encode package data */
        encode(buffer, actual_package, total_packages, data);

        /* Send package */
        while (1) {
            if (sendto(sock, buffer, bytes_read + BUFFER_HEADER_SIZE, 0, (struct sockaddr *)server_addr, addr_len) < 0) {
                error("Failed to send package to server");
            }

            /* Configure ACK timeout */
            struct timeval tv;
            tv.tv_sec = TIMEOUT_SEC;
            tv.tv_usec = TIMEOUT_USEC;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            /* Wait for the ACK */
            socklen_t addr_len = sizeof(server_addr);
            ssize_t ack_bytes = recvfrom(sock, ack_buffer, ACK_BUFFER_SIZE, 0, (struct sockaddr *)server_addr, &addr_len);
            if (random_error() || ack_bytes < 0) {
                warn("Timeout ACK, trying again");
                continue;
            }

            actual_package++;
            break;
        }
    }
}

int main(int argc, char **argv) {
    /* Randomness */
    srand(time(NULL));

    /* Network connection variables */
    char *ip, *filename;
    int port;
    get_args(argc, argv, &ip, &port, &filename);

    /* Get source file */
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        error("Failed to read file");
    }
    success("File opened");

    /* Create UDP socket */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        error("Failed to create UDP client socket");
    }
    success("UDP client socket created");

    /* Server adress info */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Send file to server */
    success("Start sending file");
    communicate(sock, &server_addr, file);
    success("Finish sending file");

    /* Close the socket */
    fclose(file);
    close(sock);
    success("Disconnected");

    return 0;
}
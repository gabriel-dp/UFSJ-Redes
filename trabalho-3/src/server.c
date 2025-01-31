#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

#define BUFFER_SIZE 1024

/* Properly handle server stop */
int server_sock = -1;
void handle_server_stop(int sig) {
    if (server_sock >= 0) {
        close(server_sock);
        success("Server socket closed");
    }
    error("Server stopped");
}

void communicate(int sock, struct sockaddr_in *client_addr, FILE *file) {
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(*client_addr);

    unsigned long int total_packages = -1, last_package = -1, actual_package = -1;
    do {
        /* Receive package */
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)client_addr, &addr_len);
        if (bytes_received < 0) {
            error("Failed to receive ");
        }
        warn("Received package");

        /* Validate content of the package */
        // Checksum?
        if (total_packages == -1) {
            total_packages = ((unsigned char)buffer[3] << 16) | ((unsigned char)buffer[4] << 8) | (unsigned char)buffer[5];
        }
        actual_package = ((unsigned char)buffer[0] << 16) | ((unsigned char)buffer[1] << 8) | (unsigned char)buffer[2];
        printf("%ld | %ld | %ld | (%ld)\n\n", last_package, actual_package, total_packages, bytes_received);
        if (last_package != actual_package) {
            fwrite(buffer, sizeof(char), bytes_received, file);  // NÃO TA ESCREVENDO ESSA CACETA NO ARQUIVO
            last_package = actual_package;
        }

        /* Send ACK to client*/
        // Different ACKs for each package?
        if (sendto(sock, "ACK", 3, 0, (struct sockaddr *)client_addr, addr_len) < 0) {
            error("Failed to send ACK");
        }
    } while (actual_package != total_packages - 1);
}

int main(int argc, char **argv) {
    /* Register signal handlers */
    signal(SIGINT, handle_server_stop);
    signal(SIGTERM, handle_server_stop);

    /* Network connection variables */
    char *ip, *filename;
    int port;
    get_args(argc, argv, &ip, &port, &filename);

    /* Socket variables */
    struct sockaddr_in server_addr, client_addr;

    /* Create UDP socket */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        error("Failed to create UDP server socket");
    }
    success("UDP server socket created");

    /* Set server address info */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Bind socket */
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Failed to bind to port");
    }
    success("Binded to port");

    /* Infinite waiting */
    while (1) {
        success("Waiting");

        FILE *file = fopen(filename, "wb");
        if (file == NULL) {
            error("Failed to create output file");
        }

        communicate(sock, &client_addr, file);

        fclose(file);
    }

    return 0;
}
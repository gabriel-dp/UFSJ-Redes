#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

/* Properly handle server stop */
int sock = -1;
void handle_server_stop(int sig) {
    if (sock >= 0) {
        close(sock);
        success("Server socket closed");
        exit(0);
    }
    error("Server stopped");
}

void communicate(int sock, struct sockaddr_in *client_addr, char *output_filename) {
    socklen_t addr_len = sizeof(*client_addr);

    FILE *output_file = NULL;
    char buffer[BUFFER_SIZE], data[BUFFER_DATA_SIZE];
    long total_packages = -1, last_package = -1, actual_package = -1;

    do {
        /* Receive package */
        ssize_t bytes_received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)client_addr, &addr_len);
        if (bytes_received <= 0) {
            warn("Failed to receive data");
            break;
        }

        /* Create output file to write data */
        if (output_file == NULL) {
            output_file = fopen(output_filename, "wb");
            if (output_file == NULL) {
                error("Failed to create output file");
            }
            success("Start receiving file");
        }

        /* Decode package content */
        decode(buffer, &actual_package, &total_packages, data);
        if (last_package != actual_package) {
            fwrite(data, sizeof(char), bytes_received - BUFFER_HEADER_SIZE, output_file);
            last_package = actual_package;
        } else {
            warn("Duplicated package, discarded");
        }

        /* Send ACK to client*/
        if (sendto(sock, "ACK", 3, 0, (struct sockaddr *)client_addr, addr_len) < 0) {
            warn("Failed to send ACK");
            break;
        }
    } while (actual_package != total_packages - 1);

    /* Properly closes output file */
    if (output_file != NULL) {
        success("Finish receiving file");
        fclose(output_file);
    }
}

int main(int argc, char **argv) {
    /* Register signal handlers */
    signal(SIGINT, handle_server_stop);
    signal(SIGTERM, handle_server_stop);

    /* Network connection variables */
    char *ip, *output_filename;
    int port;
    get_args(argc, argv, &ip, &port, &output_filename);

    /* Socket variables */
    struct sockaddr_in server_addr, client_addr;

    /* Create UDP socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
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
        communicate(sock, &client_addr, output_filename);
    }

    return 0;
}
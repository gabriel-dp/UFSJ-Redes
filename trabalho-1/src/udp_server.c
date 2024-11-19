#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void communicate(int server_sock, struct sockaddr_in *client_addr) {
    /* Create data buffer */
    char buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE - 6];
    memset(buffer, 0, sizeof(buffer));
    memset(aux_buffer, 0, sizeof(aux_buffer));
    socklen_t addr_len = sizeof(*client_addr);

    /* Receive file name from client and try to get respective file */
    recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)client_addr, &addr_len);
    printf("\nReceived from client: %s\n\n", buffer);
    FILE *file = get_file(SERVER_DIRECTORY, buffer, "rb");
    if (file == NULL) {
        /* Send error message */
        strcpy(buffer, "ERROR - FILE NOT FOUND");
        sendto(server_sock, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, addr_len);
        return;
    }

    /* Get file size and determine number of packages */
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    long total_packages = ceil((double)fileSize / (BUFFER_SIZE - 6));
    fseek(file, 0, SEEK_SET);

    /* Read file and send to the client */
    long sent_packages = 0;
    size_t bytes_read;
    memset(buffer, 0, sizeof(buffer));
    memset(aux_buffer, 0, sizeof(aux_buffer));
    while ((bytes_read = fread(aux_buffer, sizeof(char), BUFFER_SIZE - 6, file)) > 0) {
        sent_packages++;

        /* Encode package id and total packages using 3 bytes */
        buffer[0] = (sent_packages >> 16) & 0xFF;
        buffer[1] = (sent_packages >> 8) & 0xFF;
        buffer[2] = sent_packages & 0xFF;
        buffer[3] = (total_packages >> 16) & 0xFF;
        buffer[4] = (total_packages >> 8) & 0xFF;
        buffer[5] = total_packages & 0xFF;

        memcpy(buffer + 6, aux_buffer, bytes_read);
        sendto(server_sock, buffer, bytes_read + 6, 0, (struct sockaddr *)client_addr, addr_len);

        memset(buffer, 0, sizeof(buffer));
        memset(aux_buffer, 0, sizeof(aux_buffer));
    }

    /* Close the file */
    fclose(file);
}

int main(int argc, char **argv) {
    /* Network connection variables */
    char *ip;
    int port;
    get_args(argc, argv, &ip, &port, NULL);

    /* Socket variables */
    struct sockaddr_in server_addr, client_addr;

    /* Create UDP socket */
    int server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0) {
        error("Failed to create UDP server socket");
    }
    success("UDP server socket created");

    /* Set server address info */
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Bind socket */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Failed to bind to port");
    }
    success("Binded to port");

    /* Infinite waiting */
    while (1) {
        success("Waiting for client...");
        communicate(server_sock, &client_addr);
    }

    return 0;
}
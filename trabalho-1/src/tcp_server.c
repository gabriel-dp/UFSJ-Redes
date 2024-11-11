#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void communicate(int client_sock) {
    /* Create data buffer */
    char buffer[BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));

    /* Read message and try to get respective file */
    read(client_sock, buffer, sizeof(buffer));
    printf("\nReceived from client: %s\n\n", buffer);
    FILE *file = get_file(SERVER_DIRECTORY, buffer, "r");

    /* Read file and send it to client */
    size_t bytes_read;
    bzero(buffer, sizeof(buffer));
    if (file != NULL) {
        while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE - 1, file)) > 0) {
            buffer[bytes_read] = '\0';  // Fix end of buffer
            // printf("\nSending to client: %s\n\n", buffer);
            write(client_sock, buffer, strlen(buffer));
            bzero(buffer, sizeof(buffer));
        }
        fclose(file);
    } else {
        strcpy(buffer, "ERROR - FILE NOT FOUND");
        write(client_sock, buffer, strlen(buffer));
    }
}

int main() {
    /* Basic network connection variables */
    char *ip = "127.0.0.1";
    int port = 8001;

    /* Socket variables */
    struct sockaddr_in server_addr, client_addr;

    /* Create socket */
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        error("Failed to create TCP server socket");
    }
    success("TCP server socket created");

    /* Set address info */
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Bind socket */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Failed to bind to port");
    }
    success("Bind to port");

    /* Listen for connections */
    if (listen(server_sock, 5) < 0) {
        error("Failed to listen for connections");
    }

    /* Infinite waiting */
    while (1) {
        /* Accepting connections */
        success("Listening...");
        socklen_t addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0) {
            warn("Accept error");
            continue;
        }
        success("Client connected");

        /* Receive data from the client */
        communicate(client_sock);

        /* Close the client socket */
        close(client_sock);
        success("Client disconnected");
    }

    /* Close socket before finish */
    success("Closed server socket before finish");
    close(server_sock);
    return 0;
}

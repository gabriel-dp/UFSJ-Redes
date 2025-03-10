#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void communicate(int client_sock) {
    /* Create data buffer */
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    /* Read file name from client and try to get respective file */
    read(client_sock, buffer, sizeof(buffer));
    printf("\nReceived from client: %s\n\n", buffer);
    FILE *file = get_file(SERVER_DIRECTORY, buffer, "rb");
    if (file == NULL) {
        /* Send error message */
        strcpy(buffer, "ERROR - FILE NOT FOUND");
        write(client_sock, buffer, strlen(buffer));
        return;
    }

    /* Read file and send it to the client */
    size_t bytes_read;
    memset(buffer, 0, sizeof(buffer));
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        write(client_sock, buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer));
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
    success("Binded to port");

    /* Listen for connections */
    if (listen(server_sock, 5) < 0) {
        error("Failed to listen for connections");
    }

    /* Infinite waiting */
    while (1) {
        /* Accepting connections */
        success("Waiting for client...");
        socklen_t addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0) {
            warn("Accept error");
            continue;
        }
        success("Client connected");

        /* Communicate with client */
        communicate(client_sock);

        /* Close the client socket */
        close(client_sock);
        success("Client disconnected");
    }

    return 0;
}

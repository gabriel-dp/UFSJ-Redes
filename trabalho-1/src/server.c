#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    /* Basic network connection variables */
    char *ip = "127.0.0.1";
    int port = 8000;

    /* Socket variables */
    struct sockaddr_in server_addr, client_addr;  // Socket addresses
    char buffer[1024];                            // Buffer to hold data received

    /* Create socket */
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-] Socket error\n");
        exit(1);
    }
    printf("[+] TCP server socket created\n");

    /* Set address info */
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Bind socket */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-] Bind error");
        exit(1);
    }
    printf("[+] Bind to port number: %d\n", port);

    /* Listen for connections */
    if (listen(server_sock, 5) < 0) {
        perror("[-] Listen error");
        exit(1);
    }
    printf("[+] Listening...\n");

    /* Infinite waiting */
    while (1) {
        /* Accepting connections */
        socklen_t addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("[-] Accept error");
            continue;
        }
        printf("[+] Client connected\n");

        /* Receive data from the client */
        int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            perror("[-] Receive error");
        } else {
            buffer[bytes_received] = '\0';  // Null-terminate the received data
            printf("Received: %s\n", buffer);
        }

        /* Close the client socket */
        close(client_sock);
        printf("[+] Client disconnected\n");
    }

    return 0;
}
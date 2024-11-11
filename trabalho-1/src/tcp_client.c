#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void communicate(int sock) {
    /* Create data buffer*/
    char buffer[BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));

    /* Read user input */
    printf("\nSending to server: ");
    scanf("%1023s", buffer);
    printf("\n");

    /* Send data to server */
    ssize_t bytes_sent = write(sock, buffer, strlen(buffer));
    if (bytes_sent < 0) {
        close(sock);
        error("Failed to write data");
    }

    /* Read server data */
    FILE *file = get_file(CLIENT_DIRECTORY, buffer, "w");
    if (file == NULL) {
        return;
    }

    bzero(buffer, sizeof(buffer));
    ssize_t total_bytes = 0, read_bytes;
    time start = get_time();
    while ((read_bytes = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        // printf("Received from server: %s\n\n", buffer);
        fputs(buffer, file);
        bzero(buffer, sizeof(buffer));
        total_bytes += read_bytes;
    }
    time end = get_time();

    print_statistics_download(start, end, total_bytes);

    fclose(file);
}

int main() {
    /* Basic network connection variables */
    char *ip = "127.0.0.1";
    int port = 8001;

    /* Socket variables*/
    struct sockaddr_in addr;

    /* Create socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("Failed to create TCP client socket");
    }
    success("TCP client socket created");

    /* Set address info */
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    /* Connect to server */
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        error("Failed to open a connection");
    }
    success("Connected to the server");

    /* Send and receive data */
    communicate(sock);

    /* Close the socket */
    close(sock);
    success("Disconnected");
    return 0;
}

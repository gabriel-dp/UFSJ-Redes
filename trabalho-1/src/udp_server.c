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
    socklen_t addr_len = sizeof(*client_addr);
    bzero(buffer, sizeof(buffer));

    /* Receive file name from client */
    ssize_t bytes_received = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)client_addr, &addr_len);
    if (bytes_received < 0) {
        warn("Falha ao receber dados do cliente");
        return;
    }
    printf("\nRecebido do cliente: %s\n\n", buffer);
    FILE *file = get_file(SERVER_DIRECTORY, buffer, "rb");

    /* Get file size and determine number of packages */
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    long total_packages = ceil((double)fileSize / (BUFFER_SIZE - 6));
    fseek(file, 0, SEEK_SET);

    if (file != NULL) {
        /* Read file and send it to client */
        long sent_packages = 0;
        size_t bytes_read;
        bzero(buffer, sizeof(buffer));
        bzero(aux_buffer, sizeof(aux_buffer));
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
            ssize_t bytes_sent = sendto(server_sock, buffer, bytes_read + 6, 0, (struct sockaddr *)client_addr, addr_len);
            if (bytes_sent < 0) {
                warn("Falha ao enviar dados para o cliente");
                break;
            }

            bzero(buffer, sizeof(buffer));
            bzero(aux_buffer, sizeof(aux_buffer));
        }
    } else {
        /* Send error message */
        strcpy(buffer, "ERROR - FILE NOT FOUND");
        sendto(server_sock, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, addr_len);
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
        error("Falha ao criar o socket UDP do servidor");
    }
    success("Socket UDP do servidor criado");

    /* Set server address info */
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Bind socket */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Falha ao associar o socket a porta");
    }
    success("Socket associado a porta");

    /* Loop infinito para aguardar conexoes */
    while (1) {
        success("Aguardando cliente...");
        communicate(server_sock, &client_addr);
    }

    return 0;
}
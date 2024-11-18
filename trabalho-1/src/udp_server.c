#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void communicate(int server_sock, struct sockaddr_in *client_addr) {
    /* Create data buffer */
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(*client_addr);
    bzero(buffer, sizeof(buffer));

    /* Receive file name from client */
    ssize_t bytes_received = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)client_addr, &addr_len);
    if (bytes_received < 0) {
        warn("Falha ao receber dados do cliente");
        return;
    }
    printf("\nRecebido do cliente: %s\n\n", buffer);

    /* Open requested file for client */
    FILE *file = get_file(SERVER_DIRECTORY, buffer, "rb");

    /* Read file and send it to client */
    size_t bytes_read;
    unsigned long int total_packages = 0;
    bzero(buffer, sizeof(buffer));
    if (file == NULL) {
        /* Send error message */
        strcpy(buffer, "ERRO - ARQUIVO NAO ENCONTRADO");
        sendto(server_sock, buffer, strlen(buffer), 0,
               (struct sockaddr *)client_addr, addr_len);
        return;
    }
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        ssize_t bytes_sent = sendto(server_sock, buffer, bytes_read, 0, (struct sockaddr *)client_addr, addr_len);
        if (bytes_sent < 0) {
            warn("Falha ao enviar dados para o cliente");
            break;
        }
        bzero(buffer, sizeof(buffer));
        total_packages++;
    }

    /* Exibe as estatisticas no servidor */
    printf("Total de pacotes enviados: %lu\n", total_packages);

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
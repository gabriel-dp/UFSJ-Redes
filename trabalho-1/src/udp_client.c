#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void communicate(int sock, struct sockaddr_in *server_addr, char *filename) {
    /* Create data buffer */
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(*server_addr);
    bzero(buffer, sizeof(buffer));

    /* Get file name */
    if (filename == NULL) {
        /* Read user input */
        printf("\nEnviando para o servidor: ");
        scanf("%1023s", buffer);
    } else {
        /* Use command line file */
        strcpy(buffer, filename);
    }
    printf("\n");

    /* Send file name to server */
    ssize_t bytes_sent = sendto(sock, buffer, strlen(buffer), 0, 
                                (struct sockaddr *)server_addr, addr_len);
    if (bytes_sent < 0) {
        close(sock);
        error("Falha ao enviar dados para o servidor");
    }

    /* Create response file */
    FILE *file = get_file(CLIENT_DIRECTORY, buffer, "wb");
    if (file == NULL) {
        return;
    }

    /* Receive file data */
    bzero(buffer, sizeof(buffer));
    ssize_t total_bytes = 0, read_bytes;
    unsigned long int total_packages = 0, received_packages = 0;
    time start = get_time();
    while ((read_bytes = recvfrom(sock, buffer, sizeof(buffer), 0, 
                                  (struct sockaddr *)server_addr, &addr_len)) > 0) {
        fwrite(buffer, sizeof(char), read_bytes, file);
        bzero(buffer, sizeof(buffer));
        total_bytes += read_bytes;
        received_packages++;
    }
    time end = get_time();

    /* Imprime as estatisticas */
    total_packages = total_bytes / BUFFER_SIZE;  // Aproxima o numero total de pacotes enviados
    print_statistics_download(start, end, total_bytes);
    print_statistics_packages(total_packages, received_packages);

    /* Fecha o arquivo */
    fclose(file);
}

int main(){
    
}
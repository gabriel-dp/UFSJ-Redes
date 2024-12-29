#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "communicate.h"
#include "hangman.h"
#include "utils.h"

int main(int argc, char **argv) {
    /* Base input constants */
    char *ip;
    int port;
    get_args(argc, argv, &ip, &port, NULL);

    /* Socket variables*/
    struct sockaddr_in addr;

    /* Create socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("Failed to create TCP client socket");
    }
    success("TCP client socket created");

    /* Set address adress info */
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

    /* Communicate */
    communicate_with_server(sock);

    /* Close socket */
    close(sock);
    success("Disconnected");

    return 0;
}
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hangman.h"
#include "utils.h"

void communicate(int sock) {
    char request[WORD_MAX_SIZE + 1] = "\0\0";
    char response[RESPONSE_SIZE] = {0};

    do {
        /* Send attempt to server */
        ssize_t bytes_sent = write(sock, request, WORD_MAX_SIZE);
        if (bytes_sent < 0) {
            close(sock);
            error("Failed to write data");
        }
        success("Sent data");

        /* Read game state from server */
        read(sock, response, sizeof(response));
        game_t game;
        word_t mystery_word;
        decode(response, &game, &mystery_word);

        print_game(&game, &mystery_word);
        if (game.state != PLAYING) {
            if (game.state == WON) {
                printf("YOU WON! CONGRATS!\n\n");
            } else {
                printf("YOU LOSE! DUMBASS!\n\n");
            }
            break;
        }

        /* User input */
        printf("Your turn: ");
        fgets(request, WORD_MAX_SIZE, stdin);
        request[strcspn(request, "\n")] = '\0';
        uppercase(request);
    } while (strcmp(request, "0") != 0);

    write(sock, "\0", 1);
}

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

    /* Communicate with server */
    communicate(sock);

    /* Close socket */
    close(sock);
    success("Disconnected");

    return 0;
}
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "communicate.h"
#include "hangman.h"
#include "utils.h"

#define MAX_CLIENTS 100

/* Properly handle server stop */
int server_sock = -1;
void handle_server_stop(int sig) {
    if (server_sock >= 0) {
        close(server_sock);
        success("Server socket closed");
    }
    error("Server stopped");
}

int main(int argc, char **argv) {
    /* Register signal handlers */
    signal(SIGINT, handle_server_stop);
    signal(SIGTERM, handle_server_stop);

    /* Set random seed only once */
    srand(time(NULL));

    /* Base input constants */
    char *ip, *file;
    int port;
    get_args(argc, argv, &ip, &port, &file);

    /* Socket variables */
    struct sockaddr_in server_addr, client_addr;
    int client_sockets[MAX_CLIENTS] = {0};  // Array to store client sockets
    fd_set readfds;                         // File descriptors to use in select

    /* Create socket */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
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

    /* Loop to generate infinity games */
    while (1) {
        word_t correct_word = get_random_word(file);
        game_t game;
        word_t mystery_word;
        init_game(&game, &mystery_word, &correct_word);

        do {
            /* Clear and set file descriptors */
            FD_ZERO(&readfds);
            FD_SET(server_sock, &readfds);
            int max_sd = server_sock, sd;

            /* Add active client sockets to descriptors */
            for (int i = 0; i < MAX_CLIENTS; i++) {
                sd = client_sockets[i];
                if (sd > 0) {
                    FD_SET(sd, &readfds);
                }
                if (sd > max_sd) {
                    max_sd = sd;
                }
            }

            /* Wait for activity from any client */
            int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
            if (activity < 0 && errno != EINTR) {
                error("Select error");
            }

            /* Check if there is a new connection to accept */
            if (FD_ISSET(server_sock, &readfds)) {
                socklen_t addr_size = sizeof(client_addr);
                int new_socket = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
                if (new_socket < 0) {
                    error("Erro no accept");
                }

                /* Add new connection in the first empty slot */
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] == 0) {
                        client_sockets[i] = new_socket;
                        success("New client added");
                        break;
                    }
                }
            }

            /* Check activities of every client */
            for (int i = 0; i < MAX_CLIENTS; i++) {
                sd = client_sockets[i];
                if (FD_ISSET(sd, &readfds)) {
                    /* Communicate */
                    int keep_alive = communicate_with_client(sd, &game, &mystery_word, &correct_word);
                    if (!keep_alive) {
                        close(sd);
                        client_sockets[i] = 0;
                    }
                }
            }
        } while (game.state == PLAYING);
    }

    return 0;
}

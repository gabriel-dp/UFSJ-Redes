#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "communicate.h"
#include "hangman.h"
#include "utils.h"

/* Properly handle server stop */
int server_sock = -1;
void handle_server_stop(int sig) {
    if (server_sock >= 0) {
        close(server_sock);
        success("Server socket closed");
    }
    error("Server stopped");
}

int main(int argc, char** argv) {
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
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
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
            /* Accepting connections */
            success("Waiting for client...");
            socklen_t addr_size = sizeof(client_addr);
            int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
            if (client_sock < 0) {
                warn("Accept error");
                continue;
            }
            success("Client connected");

            /* Communicate */
            int keep_connection = 0;
            do {
                keep_connection = communicate_with_client(client_sock, &game, &mystery_word, &correct_word);
            } while (keep_connection);

            /* Close client socket */
            close(client_sock);
            success("Client disconnected");
        } while (game.state == PLAYING);
    }

    return 0;
}
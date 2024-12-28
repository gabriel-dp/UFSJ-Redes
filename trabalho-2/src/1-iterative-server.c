#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hangman.h"
#include "utils.h"

int communicate(int client_sock, game_t* game, word_t* mystery_word, word_t* correct_word) {
    /* Create data buffers */
    char request[WORD_MAX_SIZE + 1] = {0};
    char response[RESPONSE_SIZE] = {0};

    /* Receive player request and process it */
    size_t bytes_received = read(client_sock, request, sizeof(request));
    success("Received attempt");
    printf("|%s|\n", request);

    if (strlen(request) == 1) {
        char letter = request[0];
        try_letter(letter, game, mystery_word, correct_word);
    } else if (strlen(request) > 1) {
        word_t word;
        word.chars = malloc(strlen(request) + 1);
        word.size = strlen(request);
        memset(word.chars, 0, strlen(request) + 1);
        strncpy(word.chars, request, strlen(request));
        try_word(&word, game, mystery_word, correct_word);
        free(word.chars);
    }

    // "\0\0" Keep playing
    // "" or "\0" ends connection
    if (bytes_received <= 1) {
        warn("Ending connection");
        return 0;
    }

    /* Send encoded game to client */
    encode(response, game, mystery_word);
    write(client_sock, response, RESPONSE_SIZE);
    success("Finish attempt");

    return game->state == PLAYING;
}

int main(int argc, char** argv) {
    /* Base input constants */
    char *ip, *file;
    int port;
    get_args(argc, argv, &ip, &port, &file);

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
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error("Failed to bind to port");
    }
    success("Binded to port");

    /* Listen for connections */
    if (listen(server_sock, 5) < 0) {
        error("Failed to listen for connections");
    }

    /* Infinity games */
    while (1) {
        /* Single game logic */
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

            /* Communicate with client */
            int keep_connection = 0;
            do {
                keep_connection = communicate(client_sock, &game, &mystery_word, &correct_word);
            } while (keep_connection);

            /* Close client socket */
            close(client_sock);
            success("Client disconnected");
        } while (game.state == PLAYING);

        free(mystery_word.chars);
        free(correct_word.chars);
    }

    /* Close server socket */
    close(server_sock);

    return 0;
}
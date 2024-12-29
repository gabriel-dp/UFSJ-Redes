#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hangman.h"
#include "utils.h"

void communicate_with_server(int socket) {
    char request[WORD_MAX_SIZE + 1] = "\0";
    char response[RESPONSE_SIZE] = {0};

    /* Loop to keep connection open */
    do {
        /* Send attempt to server */
        ssize_t bytes_sent = write(socket, request, WORD_MAX_SIZE);
        if (bytes_sent < 0) {
            close(socket);
            error("Failed to write data");
        }
        success("Sent data");

        /* Read game state from server */
        read(socket, response, sizeof(response));
        game_t game;
        word_t mystery_word;
        decode(response, &game, &mystery_word);

        /* Display game on user terminal */
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
        printf("Your turn (0 to leave): ");
        fgets(request, WORD_MAX_SIZE, stdin);
        request[strcspn(request, "\n")] = '\0';
        uppercase(request);
    } while (strcmp(request, "0") != 0);
}

void communicate_with_client(int client_socket, game_t* game, word_t* mystery_word, word_t* correct_word) {
    char request[WORD_MAX_SIZE + 1] = {0};
    char response[RESPONSE_SIZE] = {0};

    /* Loop to keep connection open */
    do {
        /* Receive player request */
        size_t bytes_received = read(client_socket, request, sizeof(request));
        warn("Received attempt");
        printf("|%s|\n", request);

        /* Handle user disconnection */
        if (bytes_received <= 0) {
            warn("Connection ended");
            break;
        }

        /* Process player attempt */
        if (strlen(request) == 1) {
            char letter = request[0];
            try_letter(letter, game, mystery_word, correct_word);
        } else if (strlen(request) > 1) {
            word_t word;
            strcpy(word.chars, request);
            try_word(&word, game, mystery_word, correct_word);
        }

        /* Send encoded game to client */
        encode(response, game, mystery_word);
        write(client_socket, response, RESPONSE_SIZE);
        warn("Finish attempt");
    } while (game->state == PLAYING);
}

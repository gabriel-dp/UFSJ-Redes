#include "communicate.h"

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

void send_file(int client_sock) {
    char buffer[BUFFER_SIZE];

    FILE* file = fopen("tests/01mb.bin", "rb");
    if (file == NULL) {
        /* Send error message */
        strcpy(buffer, "ERROR - FILE NOT FOUND");
        write(client_sock, buffer, strlen(buffer));
        return;
    }

    /* Read file and send it to the client */
    size_t bytes_read;
    memset(buffer, 0, sizeof(buffer));
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        write(client_sock, buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer));
    }

    /* Close the file */
    fclose(file);
}

int communicate_with_client(int client_socket, game_t* game, word_t* mystery_word, word_t* correct_word) {
    const char* endpoint_attempt = "/game/try/";
    char endpoint[20] = {0};

    char request[REQUEST_SIZE] = {0};
    char response[1024];
    char game_state[RESPONSE_SIZE] = {0};
    char attempt[WORD_MAX_SIZE + 1] = {0};

    /* Receive player request */
    size_t bytes_received = read(client_socket, request, sizeof(request));

    /* Handle user disconnection */
    if (bytes_received <= 0) {
        warn("Connection ended");
        return 0;
    }

    /* Extract endpoint (method must always be GET) */
    char* line = strtok(request, "\r\n");
    sscanf(line, "GET %s", endpoint);

    /* Attempt endpoint makes the attempt */
    if (strncmp(endpoint, endpoint_attempt, strlen(endpoint_attempt)) == 0) {
        strcpy(attempt, endpoint + strlen(endpoint_attempt));
        warn("Received attempt");
        printf("|%s|\n", attempt);

        /* Process player attempt */
        if (strlen(attempt) == 1) {
            char letter = attempt[0];
            try_letter(letter, game, mystery_word, correct_word);
        } else if (strlen(attempt) > 1) {
            word_t word;
            strcpy(word.chars, attempt);
            try_word(&word, game, mystery_word, correct_word);
        }
    }

    /* Send encoded game to client */
    encode(game_state, game, mystery_word);

    /* HTTP encoding stuff */
    const char* status_line = "HTTP/1.1 200 OK\r\n";
    const char* content_type = "Content-Type: text/html\r\n";
    char content_length[64];
    snprintf(content_length, sizeof(content_length), "Content-Length: %lu\r\n", strlen(game_state) + 1024 * 1024);
    const char* connection = "Connection: keep-alive\r\n";
    const char* header_end = "\r\n";
    snprintf(response, sizeof(response), "%s%s%s%s%s", status_line, content_type, content_length, connection, header_end);  // Response header

    /* HTTP response sending*/
    strncat(response, game_state, RESPONSE_SIZE);
    strcat(response, "|");
    write(client_socket, response, strlen(response));
    send_file(client_socket);

    warn("Finish attempt");

    return 1;
}

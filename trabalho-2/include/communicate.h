#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "hangman.h"

#define REQUEST_SIZE 1024
#define BUFFER_SIZE 4096

void communicate_with_server(int socket);
int communicate_with_client(int client_sock, game_t* game, word_t* mystery_word, word_t* correct_word);

#endif
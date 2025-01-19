#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "communicate.h"
#include "hangman.h"
#include "utils.h"

typedef struct {
    int client_sock;
    game_t* game;
    word_t *mystery_word, *correct_word;
} thread_data_t;

void* handle_client(void* arg) {
    /* Extract data and free pointer */
    thread_data_t* data = (thread_data_t*)arg;
    int client_sock = data->client_sock;
    game_t* game = data->game;
    word_t* mystery_word = data->mystery_word;
    word_t* correct_word = data->correct_word;
    free(data);

    /* Communicate */
    int keep_alive;
    do {
        keep_alive = communicate_with_client(client_sock, game, mystery_word, correct_word);
    } while (game->state == PLAYING && keep_alive);

    /* Close client socket and thread */
    close(client_sock);
    success("Client disconnected");
    pthread_exit(NULL);
    success("Thread exit");
}

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

    /* Thread management */
    pthread_t tid;

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

            /* Create thread to handle client */
            thread_data_t* data = malloc(sizeof(thread_data_t));
            data->client_sock = client_sock;
            data->game = &game;
            data->correct_word = &correct_word;
            data->mystery_word = &mystery_word;
            if (pthread_create(&tid, NULL, handle_client, data) != 0) {
                error("Thread creation error");
                close(data->client_sock);
                free(data);
                continue;
            }
            pthread_detach(tid);
            success("Thread created");
        } while (game.state == PLAYING);
    }

    return 0;
}
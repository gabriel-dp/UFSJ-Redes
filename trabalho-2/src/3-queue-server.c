#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "communicate.h"
#include "hangman.h"
#include "utils.h"

/* Queue constants */
#define QUEUE_SIZE 10
#define WORKER_THREADS 4

/* Thread data struct */
typedef struct {
    int client_sock;
    game_t *game;
    word_t *mystery_word, *correct_word;
} thread_data_t;

/* Queue data struct */
typedef struct {
    thread_data_t data[QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty, not_full;
} client_queue_t;

/* Init queue attributes */
void queue_init(client_queue_t *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

/* Insert item at the end of the queue */
void queue_push(client_queue_t *queue, thread_data_t data) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }
    queue->data[queue->rear] = data;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->count++;
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

/* Take first item on queue and remove it */
thread_data_t queue_pop(client_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }
    thread_data_t data = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    return data;
}

/* Function to process clients */
void *worker_thread(void *arg) {
    /* Wait for infinite clients */
    while (1) {
        /* Receive client */
        thread_data_t data = queue_pop((client_queue_t *)arg);
        success("Thread received client to communicate");

        /* Communicate */
        int keep_alive;
        do {
            keep_alive = communicate_with_client(data.client_sock, data.game, data.mystery_word, data.correct_word);
        } while (data.game->state == PLAYING && keep_alive);

        /* Close client socket */
        close(data.client_sock);
        success("Client disconnected");
    }

    return NULL;
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

int main(int argc, char **argv) {
    /* Register signal handlers */
    signal(SIGINT, handle_server_stop);
    signal(SIGTERM, handle_server_stop);

    /* Set random seed only once */
    srand(time(NULL));

    /* Queue used to handle requests */
    client_queue_t client_queue;
    queue_init(&client_queue);

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
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Failed to bind to port");
    }
    success("Binded to port");

    /* Listen for connections */
    if (listen(server_sock, 5) < 0) {
        error("Failed to listen for connections");
    }

    /* Create worker threads */
    pthread_t workers[WORKER_THREADS];
    for (int i = 0; i < WORKER_THREADS; i++) {
        if (pthread_create(&workers[i], NULL, worker_thread, &client_queue) != 0) {
            error("Error creating threads");
            close(server_sock);
            exit(EXIT_FAILURE);
        }
    }
    success("Worker threads ready");

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
            int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
            if (client_sock < 0) {
                warn("Accept error");
                continue;
            }
            success("Client connected");

            /* Create thread to handle client */
            thread_data_t data;
            data.client_sock = client_sock;
            data.game = &game;
            data.correct_word = &correct_word;
            data.mystery_word = &mystery_word;
            queue_push(&client_queue, data);
        } while (game.state == PLAYING);
    }

    return 0;
}
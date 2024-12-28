#include "hangman.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void uppercase(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

word_t get_random_word(char* file_path) {
    int total_lines, random_line;
    char buffer[WORD_MAX_SIZE + 1];

    FILE* file = fopen(file_path, "r");
    fscanf(file, "%d", &total_lines);

    srand(time(NULL));
    random_line = rand() % total_lines;

    for (int i = 0; i <= random_line; i++) {
        fscanf(file, "%s", buffer);  // Clear previous \n
        fscanf(file, "%s", buffer);
    }

    word_t word;
    word.chars = (char*)malloc(strlen(buffer) + 1);
    word.size = strlen(buffer);
    strcpy(word.chars, buffer);
    uppercase(word.chars);

    return word;
}

void print_game(game_t* game, word_t* word) {
    printf("\n");
    printf("_____     ");
    printf("%s", word->chars);
    printf("          ");
    for (int i = 0; i < 26; i++) {
        if (game->alphabet[i] == MISS) printf("%c", i + 65);
    }
    printf("\n");

    printf("|   |\n");

    printf("|   ");
    printf((game->lifes < 6) ? "O" : " ");
    printf("\n");

    printf("|  ");
    printf((game->lifes < 4) ? "/" : " ");
    printf((game->lifes < 5) ? "|" : " ");
    printf((game->lifes < 3) ? "\\" : " ");
    printf("\n");

    printf("|  ");
    printf((game->lifes < 2) ? "/" : " ");
    printf(" ");
    printf((game->lifes < 1) ? "\\" : " ");
    printf("\n");

    printf("|\n\n");
}

void init_game(game_t* game, word_t* mystery_word, word_t* correct_word) {
    game->state = PLAYING;
    game->lifes = 6;
    memset(game->alphabet, UNKNOWN, 26 * sizeof(int));

    mystery_word->size = correct_word->size;
    mystery_word->chars = (char*)malloc(mystery_word->size + 1);
    memset(mystery_word->chars, 0, mystery_word->size + 1);
    for (size_t i = 0; i < correct_word->size; i++) {
        mystery_word->chars[i] = '-';
    }
}

int try_letter(char letter, game_t* game, word_t* mystery_word, word_t* correct_word) {
    int exists = 0;

    for (int i = 0; i < correct_word->size; i++) {
        if (correct_word->chars[i] == letter) {
            mystery_word->chars[i] = letter;
            exists = 1;
        }
    }

    game->alphabet[letter - 65] = exists ? HIT : MISS;
    if (!exists) {
        game->lifes--;
        if (game->lifes == 0) {
            game->state = LOSE;
        }
    } else if (strcmp(mystery_word->chars, correct_word->chars) == 0) {
        game->state = WON;
    }

    return exists;
}

int try_word(word_t* word, game_t* game, word_t* mystery_word, word_t* correct_word) {
    if (word->size == correct_word->size && strcmp(word->chars, correct_word->chars) == 0) {
        strcpy(mystery_word->chars, word->chars);
        game->state = WON;
        return 1;
    }

    game->state = LOSE;
    game->lifes = 0;
    return 0;
}

void encode(char* message, game_t* game, word_t* mystery_word) {
    memset(message, 0, RESPONSE_SIZE);

    message[0] = game->state;
    message[1] = game->lifes;
    for (int i = 0; i < 26; i++) {
        message[i + 2] = game->alphabet[i];
    }

    message[28] = mystery_word->size;
    strcat(message + 29, mystery_word->chars);
}

void decode(char* message, game_t* game, word_t* mystery_word) {
    game->state = message[0];
    game->lifes = message[1];
    for (int i = 0; i < 26; i++) {
        game->alphabet[i] = message[i + 2];
    }

    mystery_word->size = message[28];
    mystery_word->chars = (char*)malloc(mystery_word->size + 1);
    memset(mystery_word->chars, 0, mystery_word->size + 1);
    strncpy(mystery_word->chars, message + 29, mystery_word->size);
}

#ifndef HANGMAN_H
#define HANGMAN_H

#include <stdlib.h>

#define WORD_MAX_SIZE 64

enum game_state {
    PLAYING,
    WON,
    LOSE
};

enum alphabet_state {
    UNKNOWN,
    HIT,
    MISS
};

typedef int alphabet_state_t[26];

typedef struct {
    char* chars;
    size_t size;
} word_t;

typedef struct {
    int state;
    int lifes;
    alphabet_state_t alphabet;
} game_t;

void uppercase(char* str);
word_t get_random_word(char* file_path);
void print_game(game_t* game, word_t* word);
void init_game(game_t* game, word_t* word, word_t* correct_word);

int try_letter(char letter, game_t* game, word_t* mystery_word, word_t* correct_word);
int try_word(word_t* word, game_t* game, word_t* mystery_word, word_t* correct_word);

#endif
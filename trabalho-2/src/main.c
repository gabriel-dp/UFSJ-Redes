#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hangman.h"

int main(int argc, char** argv) {
    word_t correct_word = get_random_word(argv[1]);
    game_t game;
    word_t mystery_word;
    init_game(&game, &mystery_word, &correct_word);
    print_game(&game, &mystery_word);

    char buffer[64 + 1];
    do {
        memset(buffer, 0, 64 + 1);
        printf("\nYour turn: ");
        scanf("%s", buffer);
        if (strlen(buffer) == 1) {
            char letter = buffer[0];
            try_letter(letter, &game, &mystery_word, &correct_word);
        } else {
            word_t word;
            word.chars = malloc(strlen(buffer) + 1);
            word.size = strlen(buffer);
            memset(word.chars, 0, strlen(buffer) + 1);
            strncpy(word.chars, buffer, strlen(buffer));
            try_word(&word, &game, &mystery_word, &correct_word);
        }
        print_game(&game, &mystery_word);
        fgets(buffer, 64, stdin);
    } while (game.state == PLAYING);

    if (game.state == WON) {
        printf("\nCONGRATS!\n");
    } else {
        printf("\nBRO, HORRIBLE\n");
    }

    return 0;
}
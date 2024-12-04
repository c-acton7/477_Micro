/*
 * game_engine.h
 *
 *  Created on: Sep 16, 2024
 *      Author: colby
 */

#ifndef INC_GAME_ENGINE_H_
#define INC_GAME_ENGINE_H_

#define SCREEN_HEIGHT 480
#define CENTER_Y (SCREEN_HEIGHT / 2)
#define SCREEN_WIDTH 800
#define CENTER_X (SCREEN_WIDTH / 2)

#define ROWS 4
#define COLS 4
#define NUM_WORDS (ROWS*COLS)+1
#define MARGIN 20
#define SPACE 10
#define HEADER (SCREEN_HEIGHT*0.2)

#define SQWIDTH ((SCREEN_WIDTH - 2 * MARGIN - (COLS - 1) * SPACE) / COLS)
#define SQHEIGHT ((SCREEN_HEIGHT*0.8 - 2 * MARGIN - (ROWS - 1) * SPACE) / ROWS)

#include "fatfs_sd_card.h"

typedef enum {
    GAME_MODE_PLAY,
    GAME_MODE_MENU,
    GAME_MODE_END,
    GAME_MODE_SETTINGS,
    GAME_MODE_TITLE
} GameMode;

typedef struct {
    WordVector word_matrix[NUM_WORDS + 1]; // Array for words and clue
    GameMode mode;                         // Current game mode
    int set_b;                             // Set brightness flag
    int current_b;                         // Current brightness level
    uint8_t guess_remain;                  // Guesses remaining
    int score;                             // Current score
    int hi_score;                          // High score
    int picked;                            // Currently picked index
    int index0;                            // Tracked index 0
    int index1;                            // Tracked index 1
    int board_num;                         // Current board number
    int round_num;                         // Current round number
    volatile uint8_t game_timer;           // Remaining game timer
} Game_HandleTypeDef;

//extern GameMode mode;

void title_screen(Game_HandleTypeDef *hgame);
void init_game(Game_HandleTypeDef *hgame);
void show_menu(Game_HandleTypeDef *hgame);
void generate_random_letters(char* buffer, size_t length);
void shuffle_array(uint8_t *array, size_t size, int *index0, int *index1);
void game_matrix(Game_HandleTypeDef *hgame);
void process_input(Game_HandleTypeDef *hgame, int key);
void end_game(Game_HandleTypeDef *hgame);
void settings(Game_HandleTypeDef *hgame);
void calc_score(Game_HandleTypeDef *hgame, int* guesses, int* truths);

#endif /* INC_GAME_ENGINE_H_ */

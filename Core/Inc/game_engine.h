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
#define NUM_WORDS (ROWS*COLS)
#define MARGIN 20
#define SPACE 10
#define HEADER (SCREEN_HEIGHT*0.2)

#define SQWIDTH ((SCREEN_WIDTH - 2 * MARGIN - (COLS - 1) * SPACE) / COLS)
#define SQHEIGHT ((SCREEN_HEIGHT*0.8 - 2 * MARGIN - (ROWS - 1) * SPACE) / ROWS)

typedef enum {
    GAME_MODE_PLAY,
    GAME_MODE_MENU,
    GAME_MODE_END,
    GAME_MODE_SETTINGS,
    GAME_MODE_TITLE
} GameMode;

extern GameMode mode;

void title_screen(void);
void show_menu(void);
void generate_random_letters(char* buffer, size_t length);
void game_matrix(void);
void process_input(int key);
void end_game(void);
void settings(void);
void calc_score(int* guesses, int* truths);

#endif /* INC_GAME_ENGINE_H_ */

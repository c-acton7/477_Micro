/*
 * game_engine.c
 *
 *  Created on: Sep 16, 2024
 *      Author: colby
 */

#include "RA8875.h"
#include "game_engine.h"
#include "keyboard_io.h"
#include "fatfs_sd_card.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>

//TODO: make this a struct instead of global vars
WordVector word_matrix[NUM_WORDS];
GameMode mode;
int set_b = 0;
int current_b = 255;
int guess_remain = 2;
int score = 0;
int hi_score = 0;
int picked = -1;

void generate_random_letters(char* buffer, size_t length) {
	srand(HAL_GetTick());

	for (size_t i = 0; i < length; i++) {
		buffer[i] = (rand() % 26) + 65;
	}
	buffer[length] = '\0';
}

void title_screen(void) {
	mode = GAME_MODE_TITLE;

	//background
	RA8875_pwm1_out(255);
	RA8875_fill_screen(RA8875_BLACK);
	//corner text
	RA8875_text_mode();
	RA8875_text_color (RA8875_BLUE, RA8875_BLACK);
	RA8875_text_scale(1);
	RA8875_text_cursor_position(80, 0);
	RA8875_text_write("Beat", 5);
	RA8875_text_cursor_position(80, 55);
	RA8875_text_write("The", 4);
	RA8875_text_cursor_position(80, 110);
	RA8875_text_write("Clock", 6);
	RA8875_text_cursor_position(SCREEN_WIDTH-150, SCREEN_HEIGHT-150);
	RA8875_text_write("Crack", 6);
	RA8875_text_cursor_position(SCREEN_WIDTH-150, SCREEN_HEIGHT-95);
	RA8875_text_write("The", 4);
	RA8875_text_cursor_position(SCREEN_WIDTH-150, SCREEN_HEIGHT-40);
	RA8875_text_write("Code", 5);
	RA8875_graphic_mode();
	//center emblem
	RA8875_draw_fill_circle(CENTER_X, CENTER_Y, 250, 0x8C4F);	// blue
	RA8875_draw_fill_circle(CENTER_X, CENTER_Y, 225, 0x94b3);	// light blue
	RA8875_draw_fill_rect(CENTER_X - 170, CENTER_Y - 90, 340, 160, RA8875_YELLOW);
	RA8875_draw_hexagon(CENTER_X, CENTER_Y, 160, RA8875_YELLOW);
	RA8875_draw_hexagon(CENTER_X, CENTER_Y, 150, RA8875_BLACK);
	RA8875_draw_fill_rect(CENTER_X - 160, CENTER_Y - 80, 320, 140, RA8875_BLACK);
	//side graphics
	RA8875_draw_fill_rect(50, 0, 10, SCREEN_HEIGHT, RA8875_WHITE);
	RA8875_draw_fill_rect(SCREEN_WIDTH-50, 0, 10, SCREEN_HEIGHT, RA8875_WHITE);

	RA8875_text_mode();
	//center text
	RA8875_text_cursor_position(CENTER_X-144, CENTER_Y-75);
	RA8875_text_color (RA8875_YELLOW, RA8875_BLACK);
	RA8875_text_scale(3);
	RA8875_text_write("Conundrum", 9);
	RA8875_text_cursor_position(CENTER_X-75, CENTER_Y-5);
	RA8875_text_write("Crack", 6);
	//side text
	char letters[16];
	generate_random_letters(letters, 16);
	for(size_t i = 0; i < 16; i++) {
		if(i<8) {
			RA8875_text_cursor_position(10, i*60);
			RA8875_text_write(&letters[i], 1);
		}
		else {
			RA8875_text_cursor_position(SCREEN_WIDTH-35, (i-8)*60);
			RA8875_text_write(&letters[i], 1);
		}
	}
	RA8875_graphic_mode();

	//cracks
	RA8875_draw_fill_rect(200, 0, 5, 160, RA8875_YELLOW);
	RA8875_draw_fill_rect(150, 80, 150, 5, RA8875_YELLOW);
	RA8875_draw_fill_rect(130, SCREEN_HEIGHT-220, 5, 220, RA8875_YELLOW);
	RA8875_draw_fill_rect(SCREEN_WIDTH-300, 0, 5, 100, RA8875_YELLOW);
	RA8875_draw_fill_rect(SCREEN_WIDTH-300, 100, 200, 5, RA8875_YELLOW);
	RA8875_draw_fill_rect(SCREEN_WIDTH-100, 0, 5, 280, RA8875_YELLOW);
	RA8875_draw_fill_rect(SCREEN_WIDTH-200, SCREEN_HEIGHT-200, 100, 5, RA8875_YELLOW);
	RA8875_draw_fill_rect(SCREEN_WIDTH-180, SCREEN_HEIGHT-250, 5, 250, RA8875_YELLOW);
	//intersects
	RA8875_draw_fill_circle(202, 82, 7, RA8875_YELLOW);
	RA8875_draw_fill_circle(202, 82, 5, RA8875_BLUE);
	RA8875_draw_fill_circle(202, 82, 3, RA8875_YELLOW);
	RA8875_draw_fill_circle(132, SCREEN_HEIGHT-218, 7, RA8875_YELLOW);
	RA8875_draw_fill_circle(132, SCREEN_HEIGHT-218, 5, RA8875_BLUE);
	RA8875_draw_fill_circle(132, SCREEN_HEIGHT-218, 3, RA8875_YELLOW);
	RA8875_draw_fill_circle(SCREEN_WIDTH-98, 100, 7, RA8875_YELLOW);
	RA8875_draw_fill_circle(SCREEN_WIDTH-98, 100, 5, RA8875_BLUE);
	RA8875_draw_fill_circle(SCREEN_WIDTH-98, 100, 3, RA8875_YELLOW);
	RA8875_draw_fill_circle(SCREEN_WIDTH-100, SCREEN_HEIGHT-200, 7, RA8875_YELLOW);
	RA8875_draw_fill_circle(SCREEN_WIDTH-100, SCREEN_HEIGHT-200, 5, RA8875_BLUE);
	RA8875_draw_fill_circle(SCREEN_WIDTH-100, SCREEN_HEIGHT-200, 3, RA8875_YELLOW);
	RA8875_draw_fill_circle(SCREEN_WIDTH-178, SCREEN_HEIGHT-198, 7, RA8875_YELLOW);
	RA8875_draw_fill_circle(SCREEN_WIDTH-178, SCREEN_HEIGHT-198, 5, RA8875_BLUE);
	RA8875_draw_fill_circle(SCREEN_WIDTH-178, SCREEN_HEIGHT-198, 3, RA8875_YELLOW);
	//hour glass
	RA8875_draw_fill_triangle(CENTER_X-40, CENTER_Y+100, CENTER_X+40, CENTER_Y+100, CENTER_X, CENTER_Y+160, RA8875_WHITE);
	RA8875_draw_fill_triangle(CENTER_X-40, CENTER_Y+210, CENTER_X+40, CENTER_Y+210, CENTER_X, CENTER_Y+140, RA8875_WHITE);
	RA8875_draw_fill_round_rect(CENTER_X-40, CENTER_Y+90, 80, 20, 10, 0xbc20);
	RA8875_draw_fill_round_rect(CENTER_X-40, CENTER_Y+200, 80, 20, 10, 0xbc20);	//orange
	RA8875_draw_fill_rect(CENTER_X-34, CENTER_Y+100, 7, 110, 0xbc20);
	RA8875_draw_fill_rect(CENTER_X+26, CENTER_Y+100, 7, 110, 0xbc20);

	HAL_Delay(5000);
	for(int p = 200; p >= 0; p -= 20) {
		RA8875_pwm1_out(p);
		HAL_Delay(100);
	}
	RA8875_fill_screen(RA8875_BLACK);

	return;
}

void show_menu(void) {
	mode = GAME_MODE_MENU;

	//background
	RA8875_pwm1_out(current_b);
	RA8875_fill_screen(RA8875_BLUE);
	//banner
	RA8875_draw_fill_rect(0, 90, SCREEN_WIDTH, 10, RA8875_YELLOW);
	RA8875_draw_fill_rect(0, 100, SCREEN_WIDTH, 10, RA8875_BLACK);
	//arrows
	RA8875_draw_fill_triangle(700, 180, 750, 290, 700, 400, 0x8C4F);
	RA8875_draw_fill_triangle(680, 180, 730, 290, 680, 400, RA8875_BLUE);
	RA8875_draw_fill_triangle(650, 180, 700, 290, 650, 400, RA8875_YELLOW);
	RA8875_draw_fill_triangle(630, 180, 680, 290, 630, 400, RA8875_BLUE);
	RA8875_draw_fill_triangle(70, 180, 20, 290, 70, 400, 0x8C4F);
	RA8875_draw_fill_triangle(90, 180, 40, 290, 90, 400, RA8875_BLUE);
	RA8875_draw_fill_triangle(120, 180, 70, 290, 120, 400, RA8875_YELLOW);
	RA8875_draw_fill_triangle(140, 180, 90, 290, 140, 400, RA8875_BLUE);
	//bubble
	RA8875_draw_fill_ellipse(CENTER_X, CENTER_Y+42, SCREEN_WIDTH/4, SCREEN_HEIGHT/4, 0x94b3);
	RA8875_draw_fill_triangle(590, 250, 590, 330, 638, CENTER_Y+50, 0x94b3);//0deg
	RA8875_draw_fill_triangle(212, 250, 212, 330, 174, CENTER_Y+20, 0x94b3);//180deg
	RA8875_draw_fill_triangle(400, 175, 510, 192, 475, 130, 0x94b3);//74deg
	RA8875_draw_fill_triangle(400, 175, 290, 192, 325, 130, 0x94b3);//106deg
	RA8875_draw_fill_triangle(522, 380, 400, 398, 470, 420, 0x94b3);//-74deg
	RA8875_draw_fill_triangle(282, 380, 400, 398, 330, 420, 0x94b3);//-106deg
	RA8875_draw_fill_triangle(592, 250, 510, 192, 580, 190, 0x94b3);//32deg
	RA8875_draw_fill_triangle(212, 250, 290, 192, 220, 190, 0x94b3);//122deg
	RA8875_draw_fill_triangle(592, 310, 490, 368, 580, 400, 0x94b3);//-32deg
	RA8875_draw_fill_triangle(220, 400, 212, 310, 310, 368, 0x94b3);//-122deg
	RA8875_draw_fill_triangle(202, 270, 232, 330, 124, 420, 0x94b3);//stem
	//buttons
	RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y-35, 300, 70, 10, RA8875_WHITE);
	RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y+55, 300, 70, 10, RA8875_WHITE);
	RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y-25, 280, 50, 7, RA8875_BLACK);
	RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y+65, 280, 50, 7, RA8875_BLACK);

	RA8875_text_mode();
	//button text
	RA8875_text_color (RA8875_YELLOW, RA8875_BLACK);
	RA8875_text_cursor_position(CENTER_X-45, CENTER_Y-25);
	RA8875_text_scale(2);
	RA8875_text_write("Play", 5);
	RA8875_text_cursor_position(CENTER_X-95, CENTER_Y+65);
	RA8875_text_write("Settings", 9);
	//main text
	RA8875_text_color (RA8875_YELLOW, RA8875_BLUE);
	RA8875_text_cursor_position(CENTER_X-53, 20);
	RA8875_text_scale(3);
	RA8875_text_write("MENU", 5);
	//corner text
	RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
	RA8875_text_cursor_position(10, 0);
	RA8875_text_scale(1);
	RA8875_text_write("Conundrum", 9);
	RA8875_text_cursor_position(SCREEN_WIDTH-90, 0);
	RA8875_text_write("Crack", 5);
	RA8875_text_cursor_position(10, 55);
	RA8875_text_write("ECE", 3);
	RA8875_text_cursor_position(SCREEN_WIDTH-60, 55);
	RA8875_text_write("477", 3);
	RA8875_graphic_mode();

	//lightning
	RA8875_draw_fill_triangle(CENTER_X-79, 10, CENTER_X-72, 50, CENTER_X-87, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X-72, 85, CENTER_X-65, 50, CENTER_X-80, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X+99, 10, CENTER_X+92, 50, CENTER_X+107, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X+92, 85, CENTER_X+85, 50, CENTER_X+100, 50, RA8875_YELLOW);

	return;
}

void game_matrix(void) {
	mode = GAME_MODE_PLAY;
	get_word_matrix(word_matrix);

	//background
	RA8875_fill_screen(RA8875_BLUE);
	//4x4 square matrix with words
	for (int row = 0; row < ROWS; row++) {
	  for (int col = 0; col < COLS; col++) {
		  int x = MARGIN + col * (SQWIDTH + SPACE);
		  int y = HEADER + MARGIN + row * (SQHEIGHT + SPACE);

		  RA8875_draw_fill_rect(x, y, SQWIDTH, SQHEIGHT, RA8875_WHITE);

		  int textX = x + SQWIDTH / 2 - 40;
		  int textY = y + SQHEIGHT / 2 - 15;

		  RA8875_text_mode();
		  RA8875_text_cursor_position(textX, textY);
		  RA8875_text_color (RA8875_BLACK, RA8875_WHITE);
		  RA8875_text_scale(1);

		  RA8875_text_write(word_matrix[4*row + col].word, 6);
		  RA8875_graphic_mode();
	  }
	}
	//timer
	RA8875_text_mode();
	RA8875_text_cursor_position(CENTER_X-200, 50);
	RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
	RA8875_text_scale(1);
	RA8875_text_write("10", 3);
	//score
	RA8875_text_cursor_position(CENTER_X+190, 50);
	RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
	RA8875_text_scale(1);
	RA8875_text_write("Score: 0", 9);
	//clue
	RA8875_text_cursor_position(CENTER_X, 50);
	RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
	RA8875_text_scale(1);
	RA8875_text_write("kitchen", 8);
	RA8875_graphic_mode();

	HAL_TIM_Base_Start_IT(&htim6);

	return;
}

void process_input(int key) {

	switch(mode) {
		case GAME_MODE_TITLE:
			break;
		case GAME_MODE_MENU:
			if(key == 0) {
				RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y-35, 300, 70, 10, RA8875_BLACK);
				RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y-25, 280, 50, 7, RA8875_GREEN);
				RA8875_text_mode();
				RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
				RA8875_text_cursor_position(CENTER_X-45, CENTER_Y-25);
				RA8875_text_scale(2);
				RA8875_text_write("Play", 5);
				RA8875_graphic_mode();

				HAL_Delay(250);
				game_matrix();
			}
			else if(key == 4) {
				RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y+55, 300, 70, 10, RA8875_BLACK);
				RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y+65, 280, 50, 7, RA8875_GREEN);
				RA8875_text_mode();
				RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
				RA8875_text_cursor_position(CENTER_X-95, CENTER_Y+65);
				RA8875_text_scale(2);
				RA8875_text_write("Settings", 9);
				RA8875_graphic_mode();

				HAL_Delay(250);
				settings();
			}
			break;
		case GAME_MODE_PLAY:
			if(0 <= key && key < 16 && key != picked) {
				guess_remain--;
				int x = MARGIN + (key % 4) * (SQWIDTH + SPACE);
				int y = HEADER + MARGIN + (key / 4) * (SQHEIGHT + SPACE);
			    int textX = x + SQWIDTH / 2 - 40;
			    int textY = y + SQHEIGHT / 2 - 15;
				RA8875_draw_fill_rect(x, y, SQWIDTH, SQHEIGHT, RA8875_GREEN);
				RA8875_text_mode();
				RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
				RA8875_text_cursor_position(textX, textY);
				RA8875_text_scale(1);
				RA8875_text_write(word_matrix[key].word, 6);
				RA8875_graphic_mode();
				if(guess_remain) {
					picked = key;
				}
				else {
					int guess_list[2] = {picked, key};
					int gt[2] = {0, 1};
					calc_score(guess_list, gt);
					char buffer[9];
					sprintf(buffer, "Score: %d", score);
					RA8875_text_mode();
					RA8875_text_cursor_position(CENTER_X+190, 50);
					RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
					RA8875_text_scale(1);
					RA8875_text_write(buffer, 9);
					RA8875_graphic_mode();
					picked = -1;
					guess_remain = 2;
				}
			}
			break;
		case GAME_MODE_END:
			if(key == 0) {
				RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y-35, 300, 70, 10, RA8875_BLACK);
				RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y-25, 280, 50, 7, RA8875_GREEN);
				RA8875_text_mode();
				RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
				RA8875_text_cursor_position(CENTER_X-120, CENTER_Y-23);
				RA8875_text_scale(2);
				RA8875_text_write("Play Again", 10);
				RA8875_graphic_mode();

				HAL_Delay(250);
				game_matrix();
			}
			else if(key == 4) {
				RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y+55, 300, 70, 10, RA8875_BLACK);
				RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y+65, 280, 50, 7, RA8875_GREEN);
				RA8875_text_mode();
				RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
				RA8875_text_cursor_position(CENTER_X-45, CENTER_Y+65);
				RA8875_text_scale(2);
				RA8875_text_write("Menu", 5);
				RA8875_graphic_mode();

				HAL_Delay(250);
				show_menu();
			}
			break;
		case GAME_MODE_SETTINGS:
			if(key == 0) {
				RA8875_draw_fill_rect(10, CENTER_Y-40, 280, 50, RA8875_GREEN);
				RA8875_text_mode();
				RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
				RA8875_text_cursor_position(30, CENTER_Y-40);
				RA8875_text_scale(2);
				RA8875_text_write("Brightness", 10);
				RA8875_graphic_mode();

				set_b = 1;
				RA8875_draw_fill_rect(CENTER_X+50, CENTER_Y-28, 256, 25, RA8875_YELLOW);
				RA8875_draw_fill_rect(CENTER_X+50+current_b, CENTER_Y-34, 10, 37, RA8875_BLACK);

				HAL_Delay(250);
			}
			else if(key == 4) {
				if(set_b) {
					set_b = 0;
					RA8875_draw_fill_rect(CENTER_X+40, CENTER_Y-35, 280, 40, RA8875_BLUE);
					RA8875_draw_fill_rect(10, CENTER_Y-40, 280, 150, RA8875_BLACK);

					RA8875_text_mode();
					RA8875_text_color (RA8875_YELLOW, RA8875_BLACK);
					RA8875_text_cursor_position(30, CENTER_Y-40);
					RA8875_text_scale(2);
					RA8875_text_write("Brightness", 10);
					RA8875_text_cursor_position(80, CENTER_Y+10);
					RA8875_text_write("Volume", 6);
					RA8875_text_cursor_position(30, CENTER_Y+60);
					RA8875_text_write("Difficulty", 10);
					RA8875_graphic_mode();
				}
				else {
					RA8875_draw_fill_round_rect(SCREEN_WIDTH-210, SCREEN_HEIGHT-90, 180, 70, 10, RA8875_BLACK);
					RA8875_draw_fill_round_rect(SCREEN_WIDTH-200, SCREEN_HEIGHT-80, 160, 50, 7, RA8875_GREEN);
					RA8875_text_mode();
					RA8875_text_color (RA8875_BLACK, RA8875_GREEN);
					RA8875_text_cursor_position(SCREEN_WIDTH-165, SCREEN_HEIGHT-80);
					RA8875_text_scale(2);
					RA8875_text_write("Back", 5);
					RA8875_graphic_mode();

					HAL_Delay(250);
					show_menu();
				}
			}
			else if((key == 1) && set_b) {
				current_b += 8;
				if (current_b > 255) {
					current_b = 255;
				}
				RA8875_pwm1_out(current_b);
				RA8875_draw_fill_rect(CENTER_X+40, CENTER_Y-35, 280, 40, RA8875_BLUE);
				RA8875_draw_fill_rect(CENTER_X+50, CENTER_Y-28, 256, 25, RA8875_YELLOW);
				RA8875_draw_fill_rect(CENTER_X+45+current_b, CENTER_Y-34, 10, 37, RA8875_BLACK);
			}
			else if((key == 5) && set_b) {
				current_b -= 8;
				if (current_b < 10) {
					current_b = 10;
				}
				RA8875_pwm1_out(current_b);
				RA8875_draw_fill_rect(CENTER_X+40, CENTER_Y-35, 280, 40, RA8875_BLUE);
				RA8875_draw_fill_rect(CENTER_X+50, CENTER_Y-28, 256, 25, RA8875_YELLOW);
				RA8875_draw_fill_rect(CENTER_X+45+current_b, CENTER_Y-34, 10, 37, RA8875_BLACK);
			}
			break;
	}
}

void end_game(void) {
	mode = GAME_MODE_END;

	HAL_Delay(500);
	//background
	RA8875_fill_screen(RA8875_BLUE);
	//banner
	RA8875_draw_fill_rect(0, 90, SCREEN_WIDTH, 10, RA8875_YELLOW);
	RA8875_draw_fill_rect(0, 100, SCREEN_WIDTH, 10, RA8875_BLACK);
	//arrows
	RA8875_draw_fill_triangle(700, 180, 750, 290, 700, 400, 0x8C4F);
	RA8875_draw_fill_triangle(680, 180, 730, 290, 680, 400, RA8875_BLUE);
	RA8875_draw_fill_triangle(650, 180, 700, 290, 650, 400, RA8875_YELLOW);
	RA8875_draw_fill_triangle(630, 180, 680, 290, 630, 400, RA8875_BLUE);
	RA8875_draw_fill_triangle(70, 180, 20, 290, 70, 400, 0x8C4F);
	RA8875_draw_fill_triangle(90, 180, 40, 290, 90, 400, RA8875_BLUE);
	RA8875_draw_fill_triangle(120, 180, 70, 290, 120, 400, RA8875_YELLOW);
	RA8875_draw_fill_triangle(140, 180, 90, 290, 140, 400, RA8875_BLUE);
	//bubble
	RA8875_draw_fill_ellipse(CENTER_X, CENTER_Y+42, SCREEN_WIDTH/4, SCREEN_HEIGHT/4, 0x94b3);
	RA8875_draw_fill_triangle(590, 250, 590, 330, 638, CENTER_Y+50, 0x94b3);//0deg
	RA8875_draw_fill_triangle(212, 250, 212, 330, 174, CENTER_Y+20, 0x94b3);//180deg
	RA8875_draw_fill_triangle(400, 175, 510, 192, 475, 130, 0x94b3);//74deg
	RA8875_draw_fill_triangle(400, 175, 290, 192, 325, 130, 0x94b3);//106deg
	RA8875_draw_fill_triangle(522, 380, 400, 398, 470, 420, 0x94b3);//-74deg
	RA8875_draw_fill_triangle(282, 380, 400, 398, 330, 420, 0x94b3);//-106deg
	RA8875_draw_fill_triangle(592, 250, 510, 192, 580, 190, 0x94b3);//32deg
	RA8875_draw_fill_triangle(212, 250, 290, 192, 220, 190, 0x94b3);//122deg
	RA8875_draw_fill_triangle(592, 310, 490, 368, 580, 400, 0x94b3);//-32deg
	RA8875_draw_fill_triangle(220, 400, 212, 310, 310, 368, 0x94b3);//-122deg
	RA8875_draw_fill_triangle(202, 270, 232, 330, 124, 420, 0x94b3);//stem
	//buttons
	RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y-35, 300, 70, 10, RA8875_WHITE);
	RA8875_draw_fill_round_rect(CENTER_X-150, CENTER_Y+55, 300, 70, 10, RA8875_WHITE);
	RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y-25, 280, 50, 7, RA8875_BLACK);
	RA8875_draw_fill_round_rect(CENTER_X-140, CENTER_Y+65, 280, 50, 7, RA8875_BLACK);

	RA8875_text_mode();
	//button text
	RA8875_text_color (RA8875_YELLOW, RA8875_BLACK);
	RA8875_text_cursor_position(CENTER_X-120, CENTER_Y-25);
	RA8875_text_scale(2);
	RA8875_text_write("Play Again", 10);
	RA8875_text_cursor_position(CENTER_X-45, CENTER_Y+65);
	RA8875_text_write("Menu", 4);
	//main text
	RA8875_text_color (RA8875_YELLOW, RA8875_BLUE);
	RA8875_text_cursor_position(CENTER_X-143, 20);
	RA8875_text_scale(3);
	RA8875_text_write("GAME", 4);
	RA8875_text_cursor_position(CENTER_X+5, 20);
	RA8875_text_write("OVER", 4);
	//corner text
	RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
	RA8875_text_cursor_position(10, 0);
	RA8875_text_scale(1);
	RA8875_text_write("Score:", 6);
	RA8875_text_cursor_position(SCREEN_WIDTH-180, 0);
	RA8875_text_write("High Score:", 11);
	RA8875_text_cursor_position(10, 55);
	char buffer[3];
	sprintf(buffer, "%d", score);
	RA8875_text_write(buffer, 3);
	RA8875_text_cursor_position(SCREEN_WIDTH-60, 55);
	if(score > hi_score) hi_score = score;
	sprintf(buffer, "%d", hi_score);
	RA8875_text_write(buffer, 3);
	RA8875_graphic_mode();

	//lightning
	RA8875_draw_fill_triangle(CENTER_X-169, 10, CENTER_X-162, 50, CENTER_X-177, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X-162, 85, CENTER_X-155, 50, CENTER_X-170, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X+159, 10, CENTER_X+152, 50, CENTER_X+167, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X+152, 85, CENTER_X+145, 50, CENTER_X+160, 50, RA8875_YELLOW);

	score = 0;

	return;
}

void settings(void) {
	mode = GAME_MODE_SETTINGS;

	HAL_Delay(500);
	//background
	RA8875_fill_screen(RA8875_BLUE);
	//banner
	RA8875_draw_fill_rect(0, 90, SCREEN_WIDTH, 10, RA8875_YELLOW);
	RA8875_draw_fill_rect(0, 100, SCREEN_WIDTH, 10, RA8875_BLACK);
	//buttons
	RA8875_draw_fill_rect(0, CENTER_Y-50, 300, 170, RA8875_WHITE);
	RA8875_draw_fill_rect(10, CENTER_Y-40, 280, 50, RA8875_BLACK);
	RA8875_draw_fill_rect(10, CENTER_Y+10, 280, 50, RA8875_BLACK);
	RA8875_draw_fill_rect(10, CENTER_Y+60, 280, 50, RA8875_BLACK);
	RA8875_draw_fill_round_rect(SCREEN_WIDTH-210, SCREEN_HEIGHT-90, 180, 70, 10, RA8875_WHITE);
	RA8875_draw_fill_round_rect(SCREEN_WIDTH-200, SCREEN_HEIGHT-80, 160, 50, 7, RA8875_BLACK);
	RA8875_draw_hexagon(CENTER_X+200, CENTER_Y+10, 120, 0x94b3);
	RA8875_draw_hexagon(CENTER_X+100, CENTER_Y+10, 120, 0x94b3);
	//trapezoid
	RA8875_draw_fill_triangle(0, SCREEN_HEIGHT-100, 310, SCREEN_HEIGHT-95, 0, SCREEN_HEIGHT-30, RA8875_YELLOW);
	RA8875_draw_fill_triangle(0, SCREEN_HEIGHT-100, 290, SCREEN_HEIGHT-100, 0, SCREEN_HEIGHT-50, RA8875_BLUE);
	RA8875_draw_fill_triangle(0, CENTER_Y-70, 310, CENTER_Y-75, 0, CENTER_Y-140, RA8875_YELLOW);
	RA8875_draw_fill_triangle(0, CENTER_Y-70, 290, CENTER_Y-70, 0, CENTER_Y-120, RA8875_BLUE);
	RA8875_draw_fill_rect(300, CENTER_Y-75, 12, 220, RA8875_YELLOW);

	//button text
	RA8875_text_mode();
	RA8875_text_color (RA8875_YELLOW, RA8875_BLACK);
	RA8875_text_cursor_position(30, CENTER_Y-40);
	RA8875_text_scale(2);
	RA8875_text_write("Brightness", 10);
	RA8875_text_cursor_position(80, CENTER_Y+10);
	RA8875_text_write("Volume", 6);
	RA8875_text_cursor_position(30, CENTER_Y+60);
	RA8875_text_write("Difficulty", 10);
	RA8875_text_cursor_position(SCREEN_WIDTH-165, SCREEN_HEIGHT-80);
	RA8875_text_write("Back", 4);
	//main text
	RA8875_text_color (RA8875_YELLOW, RA8875_BLUE);
	RA8875_text_cursor_position(CENTER_X-128, 20);
	RA8875_text_scale(3);
	RA8875_text_write("SETTINGS", 8);
	//corner text
	RA8875_text_color (RA8875_WHITE, RA8875_BLUE);
	RA8875_text_cursor_position(10, 0);
	RA8875_text_scale(1);
	RA8875_text_write("Brightness:", 11);
	RA8875_text_cursor_position(SCREEN_WIDTH-120, 0);
	RA8875_text_write("Volume:", 7);
	RA8875_text_cursor_position(10, 55);
//	char buffer[3];
//	sprintf(buffer, "%d", score);
//	RA8875_text_write(buffer, 3);
	RA8875_text_write("254", 3);
	RA8875_text_cursor_position(SCREEN_WIDTH-60, 55);
//	sprintf(buffer, "%d", hi_score);
//	RA8875_text_write(buffer, 3);
	RA8875_text_write("100", 3);
	RA8875_graphic_mode();

	//lightning
	RA8875_draw_fill_triangle(CENTER_X-159, 10, CENTER_X-152, 50, CENTER_X-167, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X-152, 85, CENTER_X-145, 50, CENTER_X-160, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X+159, 10, CENTER_X+152, 50, CENTER_X+167, 50, RA8875_YELLOW);
	RA8875_draw_fill_triangle(CENTER_X+152, 85, CENTER_X+145, 50, CENTER_X+160, 50, RA8875_YELLOW);

	return;
}

void calc_score(int* guesses, int* truths) {
	int round_score = -1;

	if((guesses[0] == truths[0] || guesses[0] == truths[1]) && (guesses[1] == truths[0] || guesses[1] == truths[1])) {
		round_score = 3;
	}
	else if((guesses[0] == truths[0] || guesses[0] == truths[1]) || (guesses[1] == truths[0] || guesses[1] == truths[1])) {
		round_score = 1;
	}

	score += round_score;
	if(score < 0) {
		score = 0;
	}

	return;
}

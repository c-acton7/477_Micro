/*
 * keyboard_io.c
 *
 *  Created on: Sep 11, 2024
 *      Author: colby
 */

#include "main.h"
#include "keyboard_io.h"

int read_keypad(void) {

	for(int i = 0; i < 4; i++) { // Set current column as output and low
		if(i == 0) {
			HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL4_GPIO_Port, COL4_Pin, GPIO_PIN_SET);
		}
		else if(i == 1) {
			HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL4_GPIO_Port, COL4_Pin, GPIO_PIN_SET);
		}
		else if(i == 2) {
			HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(COL4_GPIO_Port, COL4_Pin, GPIO_PIN_SET);
		}
		else if(i == 3) {
			HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(COL4_GPIO_Port, COL4_Pin, GPIO_PIN_RESET);
		}

		// Read current rows
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET) return i*5 + 0;
		if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET) return i*5 + 1;
		if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET) return i*5 + 2;
		if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET) return i*5 + 3;
		if(HAL_GPIO_ReadPin(ROW5_GPIO_Port, ROW5_Pin) == GPIO_PIN_RESET) return i*5 + 4;
	}

	return -1; // No key pressed
}

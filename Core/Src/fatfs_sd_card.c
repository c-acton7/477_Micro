/*
 * fatfs_sd_card.c
 *
 *  Created on: Sep 23, 2024
 *      Author: colby
 */

#define TRUE  1
#define FALSE 0
#define bool BYTE

#include "stm32h7xx_hal.h"

#include "diskio.h"
#include "fatfs_sd_card.h"
#include "game_engine.h"
#include "fatfs.h"

#include <string.h>

void get_word_matrix(WordVector* word_matrix)
{
	HAL_Delay(1000);
    FATFS fs;
    FIL file;
    FRESULT fr;

    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {		//1 means now
    	return;
    }

    fr = f_open(&file, "floats.bin", FA_READ);
    if (fr != FR_OK) {
    	return;
    }

//<<<<<<< HEAD
//    //read and discard the first line (header and BOM line)
//    f_gets(line, sizeof(line), &file);
//
//    // Read lines from the file and parse them
//    for(int i = 0; i < NUM_WORDS; i++) {
//    	f_gets(line, sizeof(line), &file);
//        parse_word_data(line, &word_matrix[i]);
//    }

    float buffer[128];
    UINT bytesRead;
    FRESULT res;
    long totalFloats = 0;
    while (1) {
    	bytesRead = 0;
        res = f_read(&file, buffer, 512, &bytesRead);
        if (res != FR_OK || totalFloats >= 2500000) {
            break;
        }
        if(bytesRead != 512){
        	break;
        }
        totalFloats += bytesRead; // Keep track of the total floats processed
    }


    //close the file
    f_close(&file);

    //unmount the SD card
    f_mount(NULL, "", 0);

    return;
}

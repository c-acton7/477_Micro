/*
 * fatfs_sd_card.h
 *
 *  Created on: Sep 23, 2024
 *      Author: colby
 */

#ifndef __FATFS_SD_H
#define __FATFS_SD_H

#include "diskio.h"

/* Definitions for MMC/SDC command */
#define CMD0     (0x40+0)       /* GO_IDLE_STATE */
#define CMD1     (0x40+1)       /* SEND_OP_COND */
#define CMD8     (0x40+8)       /* SEND_IF_COND */
#define CMD9     (0x40+9)       /* SEND_CSD */
#define CMD10    (0x40+10)      /* SEND_CID */
#define CMD12    (0x40+12)      /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)      /* SET_BLOCKLEN */
#define CMD17    (0x40+17)      /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)      /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)      /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)      /* WRITE_BLOCK */
#define CMD25    (0x40+25)      /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)      /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)      /* APP_CMD */
#define CMD58    (0x40+58)      /* READ_OCR */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC    0x01    /* MMC ver 3 */
#define CT_SD1    0x02    /* SD ver 1 */
#define CT_SD2    0x04    /* SD ver 2 */
#define CT_SDC    0x06    /* SD */
#define CT_BLOCK  0x08    /* Block addressing */

//game-specific parsing
#define MAX_LINE_LENGTH 13
#define MAX_WORD_LENGTH 13  // 11 characters + null terminator
#define VECTOR_SIZE 5

typedef struct {
    char word[MAX_WORD_LENGTH];
    size_t length;
    float vector[VECTOR_SIZE];
} WordVector;

/* Functions */
DSTATUS SD_disk_initialize (BYTE pdrv);
DSTATUS SD_disk_status (BYTE pdrv);
DRESULT SD_disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT SD_disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT SD_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

void get_word_matrix(WordVector* word_matrix, int board_num, int round_num);
void parse_word_data(char* line, WordVector* word_vector);

#define SPI_TIMEOUT 100

#endif

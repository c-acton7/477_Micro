/*
 * audio_out.c
 *
 *  Created on: Dec 03, 2024
 *      Author: colby
 */

#include "main.h"
#include "audio_out.h"

void AudioPlay(uint16_t* audio_data, uint32_t size)
{
    if (HAL_I2S_Transmit_DMA(&hi2s3, audio_data, size) != HAL_OK)
    {
        // Transmission Error
        Error_Handler();
    }
}

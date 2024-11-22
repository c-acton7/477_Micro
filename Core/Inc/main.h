/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim6;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define SD_SPI_HANDLE hspi2
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define LCD_WAIT_Pin GPIO_PIN_4
#define LCD_WAIT_GPIO_Port GPIOC
#define LCD_RESET_Pin GPIO_PIN_5
#define LCD_RESET_GPIO_Port GPIOC
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define ROW5_Pin GPIO_PIN_0
#define ROW5_GPIO_Port GPIOD
#define ROW1_Pin GPIO_PIN_1
#define ROW1_GPIO_Port GPIOD
#define ROW2_Pin GPIO_PIN_2
#define ROW2_GPIO_Port GPIOD
#define ROW3_Pin GPIO_PIN_3
#define ROW3_GPIO_Port GPIOD
#define ROW4_Pin GPIO_PIN_4
#define ROW4_GPIO_Port GPIOD
#define COL1_Pin GPIO_PIN_5
#define COL1_GPIO_Port GPIOD
#define COL2_Pin GPIO_PIN_6
#define COL2_GPIO_Port GPIOD
#define COL3_Pin GPIO_PIN_7
#define COL3_GPIO_Port GPIOD
#define COL4_Pin GPIO_PIN_9
#define COL4_GPIO_Port GPIOB
#define LEDY_Pin GPIO_PIN_0
#define LEDY_GPIO_Port GPIOE
#define LEDR_Pin GPIO_PIN_1
#define LEDR_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32l4xx_hal.h"
/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* Private defines -----------------------------------------------------------*/
//#define DEBUG
#define _3S                        15000
#define OFF_CURRENT                   0 
#define ADD_CURRENT                  50
#define CurrentON_Pin GPIO_PIN_2
#define CurrentON_GPIO_Port GPIOA
#define STB_Pin GPIO_PIN_4
#define STB_GPIO_Port GPIOC 
#define PWM_Pin GPIO_PIN_6
#define PWM_GPIO_Port GPIOC
#define Valve_Pin GPIO_PIN_7
#define Valve_GPIO_Port GPIOC
#define OC_Pin GPIO_PIN_8
#define OC_GPIO_Port GPIOC
#define Fan_Pin GPIO_PIN_9
#define Fan_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_8
#define Buzzer_GPIO_Port GPIOA
#define Trigger_Pin GPIO_PIN_11
#define Trigger_GPIO_Port GPIOA
#define HF_Pin GPIO_PIN_12
#define HF_GPIO_Port GPIOA
#define Welding_helmet_Pin GPIO_PIN_15
#define Welding_helmet_GPIO_Port GPIOA
#define SELECT_MODE_Pin GPIO_PIN_5
#define SELECT_MODE_GPIO_Port GPIOC
/* Exported types ------------------------------------------------------------*/
typedef enum{
  OFF,
	ON
} SwitchTypeDef;
typedef enum{
  PRESS,
	RELEASE
} StateInputTypeDef;
typedef enum{
  WELDING_CURRENT,
	POST_GAS,
	TAKT_TIME,
	WELDING_TIME
} displayTypeDef;
typedef enum{
	UNACTIVE,
	ACTIVE
} stateTypeDef;
typedef enum{
	CTIG,
	TIG
} ModeTypeDef;
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

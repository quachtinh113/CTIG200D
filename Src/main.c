/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "clock.h"
#include "uart.h"
#include "adc.h"
#include "dac.h"
#include "timer.h"
#include "gpio.h"
#include "error.h"
#include "display.h"
#include "process.h"
#include "manage_task.h"
/* Private variables --------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
DAC_HandleTypeDef hdac1;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim7;
UART_HandleTypeDef huart1;
/* Definitions for Tasks */
osThreadId_t TaskHandle_feedback,TaskHandle_gpio,TaskHandle_welding;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 3
};
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void){
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_DAC1_Init();
  MX_TIM2_Init();
	MX_TIM7_Init();
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);  
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*OFF_CURRENT);
	#ifdef DEBUG
	  MX_USART1_UART_Init();
		send_message("Initialize device\r\n");  
  #endif
	MX_GPIO_Init();
  TM1620_Init();
  /* Init scheduler */
  osKernelInitialize();
  /* creation of defaultTask */
  TaskHandle_feedback = osThreadNew(feedback_signals, NULL, &defaultTask_attributes);
	TaskHandle_gpio = osThreadNew(process_gpio, NULL, &defaultTask_attributes);
	TaskHandle_welding = osThreadNew(process_welding, NULL, &defaultTask_attributes);
	/*Start timer interrupt*/
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim7);
  /* Start scheduler */
  osKernelStart();
	/*Start DAC*/
  while (1)
	{
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

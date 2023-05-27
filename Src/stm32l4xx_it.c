/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32l4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dac.h"
#include "gpio.h"
#include "process.h"
#include <stdbool.h>
/* Private variables --------------------------------------------------------*/
uint32_t count_3s=0;
uint32_t countTime=0;
bool flag_welding_helmet=false;
bool turningOffHelmet=false;
uint32_t timeout_hf;
bool enable_timeout_hf;
/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim7;
extern bool start;
extern displayTypeDef show;
extern uint16_t time_pregas;
extern uint32_t time_post_gas;
extern bool Time_Out;
extern bool EnableWaitTimeOut;
extern bool Failed_Start;
extern uint16_t counter_wait_feedback_current;
extern uint16_t welding_current;
extern uint16_t takt_time; 
extern uint16_t welding_time; 
extern uint32_t cycle;
extern uint16_t welding_current;
extern StateInputsDef switchSelectMode;
extern bool welding_ctig;
extern bool enableUpdateDisplayWeldingCurrent;
/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void){
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void){
  while (1){ 
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void){
  while (1){
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void){
  while (1){
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void){
  while (1){
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void){
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void){
  HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void){
  HAL_DMA_IRQHandler(&hdma_adc1);
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void){
  HAL_TIM_IRQHandler(&htim2);
	if(switchSelectMode.Select_mode==CTIG && welding_ctig==true){
		countTime+=1;
		if(countTime<=cycle){
			if(countTime>welding_time || check_trigger()==RELEASE){
				if(check_current_on()==RELEASE && check_trigger()==PRESS){
					hf_on();
				}
				/*Turn off pwm*/
				pwm_off();  
				/*Turn on welding helmet*/
		    Welding_helmet_on();
			}
		}
		else {
			countTime=0;
			welding_ctig=false;
			/*Turn off welding helmet*/
			Welding_helmet_off();
			/*Turn on hf*/
			hf_on();
			timeout_hf=_3ms;
			enable_timeout_hf=true;
		}
	}
	
	if(show!=WELDING_CURRENT){
		count_3s+=1;
		if(count_3s>=_3S){
			show=WELDING_CURRENT;
			enableUpdateDisplayWeldingCurrent=true;
		}
	}
	if(time_pregas>0){
		time_pregas-=1;
	}
	
	if(time_post_gas>0){
		time_post_gas-=1;
	}
	
	if(EnableWaitTimeOut==true){
		counter_wait_feedback_current+=1;   
		if(counter_wait_feedback_current>=_3S){
			counter_wait_feedback_current=0;
			EnableWaitTimeOut=false;
			Time_Out=true;
		}
	}
}
/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void){
  HAL_TIM_IRQHandler(&htim7);
  /*Detect active current_on*/
	if(check_current_on()==PRESS){
		/*Turn off HF*/
		hf_off();
	}
	
	if(enable_timeout_hf==true && timeout_hf>0){
		timeout_hf-=1;
		if(timeout_hf==0){
			/*Turn on pwm*/
			pwm_on();
			enable_timeout_hf=false;
		}
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

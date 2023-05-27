#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include "error.h"
#include "gpio.h"
#include "dac.h"
#include "uart.h"
/*Private variables ---------------------------------------------------------*/
StateInputsDef switchSelectMode;
StateInputsDef previousState;
SwitchTypeDef currentON=OFF;
/*External variables ---------------------------------------------------------*/
extern DAC_HandleTypeDef hdac1;
extern SwitchTypeDef led_oc;
extern bool enableChangeMode;
extern uint16_t welding_current;
extern bool EnableWaitTimeOut;
/**@fn MX_GPIO_Init(void).
 * @param None.
 * @description GPIO Initialization Function
 * @retval None.
 */
void MX_GPIO_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STB_GPIO_Port, STB_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Fan_GPIO_Port, Fan_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : STB_Pin */
  GPIO_InitStruct.Pin = STB_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(STB_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin : Fan_Pin */
  GPIO_InitStruct.Pin = Fan_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(Fan_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin :  Welding_helmet_Pin */
  GPIO_InitStruct.Pin =  Welding_helmet_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(Welding_helmet_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin :  HF_Pin */
  GPIO_InitStruct.Pin =  HF_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(HF_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin :  Valve_Pin */
  GPIO_InitStruct.Pin =  Valve_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Valve_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin :  PWM_Pin */
  GPIO_InitStruct.Pin =  PWM_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PWM_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin : SELECT_MODE_Pin */
  GPIO_InitStruct.Pin = SELECT_MODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SELECT_MODE_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin : CurrentON_Pin */
  GPIO_InitStruct.Pin = CurrentON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CurrentON_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin : OC_Pin */
  GPIO_InitStruct.Pin = OC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OC_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin : Trigger_Pin */
  GPIO_InitStruct.Pin = Trigger_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Trigger_GPIO_Port, &GPIO_InitStruct);	
	/*Turn off fan, pwm, valve, buzzer, hf and welding helmet*/
	fan_off();
	pwm_off();
	valve_off(); 
	buzzer_off();
	hf_off();  
	/*Start state of inputs are release*/
	switchSelectMode.Select_mode=check_select_mode();
	previousState.Select_mode=switchSelectMode.Select_mode;
	switchSelectMode.Oc=RELEASE;
	switchSelectMode.Trigger=RELEASE;
	switchSelectMode.CurrentOn=RELEASE;
	previousState.Oc=RELEASE;
	previousState.CurrentOn=RELEASE;
	Welding_helmet_on();
}
/** @fn ModeTypeDef check_select_mode(void).
 * @description 
 * Check state switch select mode.
 * @retval PRESS or RELEASE.
 */
ModeTypeDef check_select_mode(void){
	if(HAL_GPIO_ReadPin(SELECT_MODE_GPIO_Port,SELECT_MODE_Pin)==GPIO_PIN_RESET){
		return CTIG;
	}		
	else{
		return TIG;
	}			
}
/** @fn StateInputTypeDef check_oc(void).
 * @description 
 * Check state OC.
 * @retval PRESS or RELEASE.
 */
static StateInputTypeDef check_oc(void){
	if(HAL_GPIO_ReadPin(OC_GPIO_Port,OC_Pin)==GPIO_PIN_SET){
		return PRESS;
	}		
	else{
		return RELEASE;
	}			
}
/** @fn StateInputTypeDef check_trigger(void).
 * @description 
 * Check state trigger.
 * @retval PRESS or RELEASE.
 */
StateInputTypeDef check_trigger(void){
	if(HAL_GPIO_ReadPin(Trigger_GPIO_Port,Trigger_Pin)==GPIO_PIN_RESET){
		return PRESS;
	}		
	else{
		return RELEASE;
	}			
}
/**@fn StateInputTypeDef check_current_on(void).
 * @description 
 * Check state currentON.
 * @retval PRESS or RELEASE.
 */
StateInputTypeDef check_current_on(void){
	if(HAL_GPIO_ReadPin(CurrentON_GPIO_Port,CurrentON_Pin)==GPIO_PIN_RESET){
		return PRESS;
	}		
	else{
		return RELEASE;
	}			
}
/**@fn soft_reset(void).
 * @description
 * Soft reset device.
 * @retval Node.
 */
static void soft_reset(void){
	__set_PRIMASK(1);
	SCB->AIRCR =  ((uint32_t)0x05FA0000) |  ((uint32_t)0x01);
	while(1);
}
/**@fn process_inputs(void)
 * @description
 * Call functions of correspond inputs.
 * @retval Node.
 */
void process_inputs(void){
	
	/*Process input select mode*/
	if(check_select_mode()==CTIG && previousState.Select_mode==TIG && enableChangeMode==true && check_trigger()==RELEASE){
		previousState.Select_mode=CTIG;
		switchSelectMode.Select_mode=CTIG;
		/*Turn off timeout counter*/
		EnableWaitTimeOut=false;
		#ifdef DEBUG
			send_message("Selected cool tig\r\n");  
		#endif
		soft_reset();
	}
	else if(check_select_mode()==TIG && previousState.Select_mode==CTIG && enableChangeMode==true && check_trigger()==RELEASE){
		previousState.Select_mode=TIG;
		switchSelectMode.Select_mode=TIG;
		/*Turn off timeout counter*/
		EnableWaitTimeOut=false;
		#ifdef DEBUG
			send_message("Selected tig\r\n");  
		#endif
		soft_reset();
	}
	/*Process OC signal*/
	if(check_oc()==PRESS && previousState.Oc==RELEASE){																																																										
		previousState.Oc=PRESS;
		switchSelectMode.Oc=PRESS;
		#ifdef DEBUG
			send_message("Non OC\r\n");  
		#endif
		/*Turn off led oc*/
		led_oc=OFF;
	}
	else if(check_oc()==RELEASE && previousState.Oc==PRESS){
		previousState.Oc=RELEASE;
		switchSelectMode.Oc=RELEASE;
		#ifdef DEBUG
			send_message("OC\r\n");  
		#endif
		/*Turn on led oc*/
		led_oc=ON;
	}	
	
	/*Process currentON signal*/
	if(check_current_on()==PRESS && previousState.CurrentOn==RELEASE){
		previousState.CurrentOn=PRESS;
		switchSelectMode.CurrentOn=PRESS;
		currentON=ON;
	}
	else if(check_current_on()==RELEASE && previousState.CurrentOn==PRESS){
		previousState.CurrentOn=RELEASE;
		switchSelectMode.CurrentOn=RELEASE;
		currentON=OFF;
	}
	
}





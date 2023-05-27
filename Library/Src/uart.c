#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "error.h"
extern UART_HandleTypeDef huart1;
/**@fn MX_USART1_UART_Init(void).
 * @param None.
 * @description USART1 Initialization Function
 * @retval None.
 */
void MX_USART1_UART_Init(void){
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}
/**@fn send_message(char * message).
 * @param *message : string message.
 * @description send message uart
 * @retval None.
 */
void send_message(char * message)
	{
	HAL_UART_Transmit(&huart1,(uint8_t*)message, strlen(message),10);
}
/*Function: Send a byte to monitor*/
void send_byte(uint8_t *data){
	HAL_UART_Transmit(&huart1,(uint8_t*)data,1,10);
}
/*Function: Send a number to monitor with number in range [0,9999]*/
void send_numbers(uint32_t number){
	uint8_t vNumber[4];
	vNumber[0]=0x30+number/1000;
	number%=1000;
	vNumber[1]=0x30+number/100;
	number%=100;
	vNumber[2]=0x30+number/10;
	vNumber[3]=0x30+number%10;
	send_message("Number:");
	HAL_UART_Transmit(&huart1,(uint8_t*)vNumber,4,10);
	send_message("\r\n");	
}

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
#define STB_LOW             HAL_GPIO_WritePin(STB_GPIO_Port, STB_Pin, GPIO_PIN_RESET)
#define STB_HIGH            HAL_GPIO_WritePin(STB_GPIO_Port, STB_Pin, GPIO_PIN_SET)
/* Private variables --------------------------------------------------------*/
uint8_t fontCode[]={	0x3F,0x06,0x5B,0x4F,0x66,	    //0-4
											0x6D,0x7D,0x07,0x7F,0x6F,    	//5-9
											0xBF,0x86,0xDB,0xCF,0xE6,	    //.0-.4
											0xED,0xFD,0x87,0xFF,0xEF,	    //.5-.9
	                    0x00,0x04,0x01,0x40,0x79,0x40 //OFF-LED, LED-OC, LED-OT, OT, E, -
};
char DisplayArray[MAX_TUBE_NUM];
extern SPI_HandleTypeDef hspi1;
/**@fn MX_SPI1_Init(void).
 * @param None.
 * @description SPI1 Initialization Function.
 * @retval None.
 */	
static void MX_SPI1_Init(void){
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}
/**@fn TM1620_Init(void).
 * @param None.
 * @description TM1620 Initialization Function.
 * @retval None.
 */	
int TM1620_Init(void){
	MX_SPI1_Init();
	HAL_Delay(5);
	return 0;
} 
/**@fn TM1620_Write_8bit(uint8_t data).
 * @param data : a byte data.
 * @description Write 8 bit to TM1620
 * @retval HAL_OK or HAL_ERROR.
 */
int TM1620_Write_8bit(uint8_t data){
		uint8_t buffer[4];
		buffer[0]= data;
		if(HAL_SPI_Transmit(&hspi1,buffer,1,1)!=HAL_OK){
		return HAL_ERROR;
	}
	else {
		return HAL_OK;
	}
}
/**@fn TM1620_Write(TM1620_CMD cmd,uint8_t data).
 * @param data : data to TM1620.
 * @description Write data to TM1620.
 * @retval None.
 */
void TM1620_Write(TM1620_CMD cmd,uint8_t data){
	switch(cmd){
		case TM1620_DISP_MODE:
			STB_LOW;
			TM1620_Write_8bit(data&0x03);
		    STB_HIGH;
			break;
		case TM1620_DATA:
			STB_LOW;
			TM1620_Write_8bit(0x40|(data&0x7));
			STB_HIGH;
			break;
		case TM1620_BRIGHT:
			STB_LOW;
			TM1620_Write_8bit(0x88|(0x7&data));
			STB_HIGH;
			break;
		case TM1620_ADDR:
			STB_LOW;
			TM1620_Write_8bit(0xC0|(0xF&data));
			break;
	}
}
/**@fn u16ToDisplayArray(uint16_t uin).
 * @param uin : number need display on LEDs.
 * @description process and save number to a array.
 * @retval None.
 */
void u16ToDisplayArray(uint16_t uin)
	{
		uint16_t temp=uin;
		for(int i=(MAX_TUBE_NUM-1);i>=0;i--){
		DisplayArray[i]=temp%10+'0';
		temp=temp/10;
	}
}
/**@fn display_leds(uint16_t uin, bool divide, SwitchTypeDef led_ot, bool  more_90_oC,SwitchTypeDef led_oc, ModeTypeDef mode, bool initial_start).
* @param uin : number need display on LEDs, divide : divide of uin, led_ot : status of led OT, mode : TIG or CTIG, more_90_oC : less or more 90oC,led_oc : status of led OC, initial_start : show 'E-3'.
 * @description Display number to LEDs.
 * @retval None.
 */
void display_leds(uint16_t uin, bool divide, SwitchTypeDef led_ot, bool  more_90_oC, SwitchTypeDef led_oc, ModeTypeDef mode, bool initial_start)
{
	
	uint8_t fcode[MAX_TUBE_NUM];
	if(initial_start==true){
		fcode[0]=fontCode[24];
  	fcode[1]=fontCode[25];
	  fcode[2]=fontCode[3];
		fcode[4]=0;
		if(led_ot==ON){
			fcode[4]=fontCode[20];
		}
		if(led_oc==ON){
			fcode[4]|=fontCode[20];
		}
	}
	else {
		fcode[4]=0;
		if(led_oc==ON){
			fcode[4]|=fontCode[21];
			fcode[0]=fontCode[24];
			fcode[1]=fontCode[25];
			fcode[2]=fontCode[1];
		}
		else if(led_oc==OFF && led_ot==ON && more_90_oC==true) {
			fcode[4]=fontCode[21];
			fcode[0]=fontCode[24];
			fcode[1]=fontCode[25];
			fcode[2]=fontCode[2];
		}
		else if((led_oc==OFF && led_ot==OFF) || (led_oc==OFF && led_ot==ON && more_90_oC==false)){
			if(divide==true){
				if(uin<10){
					fcode[0]=fontCode[20];
					fcode[1]=fontCode[10];
					fcode[2]=fontCode[uin];
				}
				else if(uin>=10 && uin<100){
					fcode[0]=fontCode[20];
					fcode[1]=fontCode[uin/10+10];
					fcode[2]=fontCode[uin%10];
				}
				else if(uin>=100 && uin<1000){
					fcode[0]=fontCode[uin/100];
					fcode[1]=fontCode[(uin%100)/10+10];
					fcode[2]=fontCode[(uin%100)%10];
				}
			}
			else {
				if(uin<10){
					fcode[0]=fontCode[20];
					fcode[1]=fontCode[20];
					fcode[2]=fontCode[uin];
				}
				else if(uin>=10 && uin<100){
					fcode[0]=fontCode[20];
					fcode[1]=fontCode[uin/10];
					fcode[2]=fontCode[uin%10];
				}
				else if(uin>=100 && uin<1000){
					fcode[0]=fontCode[uin/100];
					fcode[1]=fontCode[(uin%100)/10];
					fcode[2]=fontCode[(uin%100)%10];
				}
			}
		}	
		if(mode==CTIG){
			fcode[4]|=fontCode[22];
		}
	}
	
	TM1620_Write(TM1620_DISP_MODE,0x02);
	TM1620_Write(TM1620_DATA,AUTO_ADDR_MODE);		
	TM1620_Write(TM1620_ADDR,0x00);
	for(int i=0;i<MAX_TUBE_NUM;i++){
		TM1620_Write_8bit(fcode[i]);
		TM1620_Write_8bit(0x00);
	}
	STB_HIGH;
	TM1620_Write(TM1620_BRIGHT,MAX_TUBE_BRIGHTNESS);			
}

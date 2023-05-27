#include "main.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include "string.h"
#include "uart.h"
#include "adc.h"
#include "gpio.h"
#include "dac.h"
#include "error.h"
#include "kalman.h"
/* Private variables --------------------------------------------------------*/
uint16_t buffer[NUMBER_CHANNEL_ADC]; 
uint32_t temp_buffer[NUMBER_CHANNEL_ADC]; 
uint16_t welding_current;
uint16_t post_gas;
uint16_t takt_time; 
uint16_t welding_time; 
uint16_t inData;
volatile uint16_t previous;
uint8_t count_sample=0;
uint16_t var_adc_3=0;
uint16_t var_adc_2=0;
uint16_t var_adc_1=0;
uint16_t var_adc_0=0;
bool OT=false;
bool more_90_oC=false;
uint32_t cycle;
uint8_t times=0;
uint16_t kalman_adc;
//uint16_t tam=0;
uint16_t count_times=0;
bool enableUpdateDisplayWeldingTime;
bool enableUpdateDisplayWeldingCurrent;
/* External variables --------------------------------------------------------*/
extern DAC_HandleTypeDef hdac1;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern displayTypeDef show;
extern bool start;
extern uint32_t count_3s;
extern bool enableChangeMode;
extern StateInputsDef switchSelectMode;
/**@fn MX_ADC1_Init(void).
 * @param None.
 * @description ADC Initialization Function
 * @retval None.
 */	
void MX_ADC1_Init(void)
	{
  ADC_ChannelConfTypeDef sConfig = {0};
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 5;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}
/**@fn MX_DMA_Init(void).
 * @param None.
 * @description DMA Initialization Function
 * @retval None.
 */
void MX_DMA_Init(void) {
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}
/**@fn process_feedback_signals(void).
 * @description
 * Convert ADC feedback signals 
 * @retval Node.
 */
void process_feedback_signals(void){ 
	/*Start convert ADC*/
		HAL_ADC_Start_DMA(&hadc1,(uint32_t*)buffer,NUMBER_CHANNEL_ADC);
	for(uint8_t i=0;i<NUMBER_SAMPLE;i++){
		temp_buffer[i]+=buffer[i];
	}
	/*Check OT signal*/
	if(buffer[4]>3456){
		OT=false;
		more_90_oC=false; 
	}
	else if(buffer[4]<=3456 && buffer[4]>1180){
		OT=true;
		more_90_oC=false; 
	}
	else if(buffer[4]<=1180){
		OT=true;
		more_90_oC=true;  
	}
					count_sample+=1;
			if(count_sample>=NUMBER_SAMPLE){
				if(times<TIMES){
					times+=1;
		}
		/*Calculation average ADC of buffer*/
			//previous=welding_current;
		for(uint8_t i=0;i<NUMBER_SAMPLE;i++){
			temp_buffer[i]/=NUMBER_SAMPLE;
				//kalman=temp_buffer[i];
		}
		/*Use kalman to filter out noise*/
			kalman_filter(previous);
			
		/*Calculation adjust welding current from buffer ADC*/
		kalman_adc=previous;
		previous=welding_current;
		var_adc_0=temp_buffer[0];
		welding_current=10+(uint16_t)(((double)temp_buffer[0])/16);
		if(welding_current>250){
			welding_current=250;
		}
		if(welding_current!= previous && (previous>var_adc_0+3|| previous<var_adc_0-3)&& welding_current%2==0){
			enableUpdateDisplayWeldingCurrent=true;
			out_preset((ModeTypeDef)switchSelectMode.Select_mode);
			show=WELDING_CURRENT;
		}
		
		/*Calculation adjust post gas from buffer ADC*/
		previous = var_adc_1;
		var_adc_1 = (double)temp_buffer[1];
		post_gas=20+(uint16_t)(((double)temp_buffer[1])/40.3);
			if(post_gas>100){
				post_gas=100;
		}
		if((post_gas!=previous )&&(previous >var_adc_1+3||previous <var_adc_1-3)){ 
			if(times>=TIMES){
				show=POST_GAS;
				count_3s=0;
				#ifdef DEBUG
					send_message("Post gas\r\n");  
				#endif
			}		
		}
		/*Calculation adjust takt_time from buffer ADC*/
		previous=var_adc_2;
		var_adc_2=(double)temp_buffer[2];
		takt_time=1+(uint16_t)(((double)temp_buffer[2])/40.49); 
		if(takt_time>100){
			takt_time=100;
		}
		if((takt_time!=previous)&&((previous>var_adc_2+3)||(previous<var_adc_2-3))){
			if(times>=TIMES){
				show=TAKT_TIME;
			  count_3s=0;
				#ifdef DEBUG
					send_message("Takt time\r\n");  
				#endif
			}
		}
		/*Calculation adjust welding time from buffer ADC*/
		previous = var_adc_3;
		var_adc_3=temp_buffer[3];		
//		welding_time=10+(uint16_t)(((double)temp_buffer[3])/4.3); 	
//		if((var_adc_3>=0)&&(var_adc_3 <523)){welding_time=1+((var_adc_3)/52.2);}
//		if(var_adc_3>=523&&var_adc_3<1805){welding_time=((var_adc_3)/33.9);}
//		if(var_adc_3>=1805&&var_adc_3<3164){welding_time=((var_adc_3)/33.91);}
		if((var_adc_3 < 1800){welding_time=(10+((var_adc_3)/7.49)));}
		if(var_adc_3>=1800&&var_adc_3<=4020){
			welding_time=100+((var_adc_3-1800)/2.44);
			if((welding_time!=previous)&&(previous>=(var_adc_3+3)||previous<=(var_adc_3-3)) && welding_time%10==0){
			if(welding_time>=100&&welding_time<=199)
			{welding_time=100;}
			if(welding_time>=200&&welding_time<=299)
			{welding_time=200;}
			if(welding_time>=300&&welding_time<=399)
			{welding_time=300;}
			if(welding_time>=400&&welding_time<=499)
			{welding_time=400;}
			if(welding_time>=500&&welding_time<=599)
			{welding_time=500;}
			if(welding_time>=600&&welding_time<=699)
			{welding_time=600;}
			if(welding_time>=700&&welding_time<=799)
			{welding_time=700;}
			if(welding_time>=800&&welding_time<=899)
			{welding_time=800;}
			if(welding_time>=900)
				{welding_time=900;}
				}
		}
			uint16_t step;
//			if(welding_time<10){step=1;}
//			if(welding_time>=10&&welding_time<50){step=5;}
			if(welding_time<100){step=5;}
			if(welding_time>=100){step=100;}
			if(welding_time!=previous&&welding_time%step==0){
			if(times>=TIMES)
				{
				enableUpdateDisplayWeldingTime=true;
				show=WELDING_TIME;
			  count_3s=0;
				#ifdef DEBUG
					send_numbers(welding_time);
				#endif
		  	}
			}
		/*Calculation a cycle time*/
		cycle= welding_time+100*takt_time;
		/*Reset temp_buffer*/
		for(uint8_t i=0;i<NUMBER_SAMPLE;i++)
				{
					temp_buffer[i]=0;
				}
		count_sample=0;
	}
}





 

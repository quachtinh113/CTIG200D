#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "dac.h"
#include "error.h"
extern DAC_HandleTypeDef hdac1;
extern uint16_t welding_current;
/**@fn MX_DAC1_Init(void).
 * @param None.
 * @description DAC Initialization Function
 * @retval None.
 */
void MX_DAC1_Init(void){
  DAC_ChannelConfTypeDef sConfig = {0};
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
}
/**@fn out_preset(ModeTypeDef mode).
 * @param mode : TIG or CTIG 
 * @description out preset
 * @retval None.
 */
void out_preset(ModeTypeDef mode){
	if(mode==TIG){
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*welding_current);
	}
	else if(mode==CTIG){
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*(welding_current+ADD_CURRENT));
	}
}

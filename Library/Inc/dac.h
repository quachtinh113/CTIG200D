#include "main.h"
/* Private defines -----------------------------------------------------------*/
#define COEFFICIENT_DAC         12.41      //(4096/3.3V)*0.01 
#define OFF_CURRENT              0
extern void MX_DAC1_Init(void);
extern void out_preset(ModeTypeDef mode);


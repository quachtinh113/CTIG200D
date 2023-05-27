#include "main.h"
#include <stdbool.h>
#define MAX_TUBE_NUM					5			
#define MAX_TUBE_BRIGHTNESS		0x02	
#define AUTO_ADDR_MODE		0x00	
#define FIXED_ADDR_MODE		0x04
typedef enum {	TM1620_DISP_MODE	=	0x0,	
								TM1620_DATA				=	0x1,	
								TM1620_BRIGHT			= 0x2, 
								TM1620_ADDR				=	0x3 	
}TM1620_CMD;
static void MX_SPI1_Init(void);
extern void TM1620_Write(TM1620_CMD cmd,uint8_t data);
extern int TM1620_Init(void);
extern void display_leds(uint16_t uin, bool divide, SwitchTypeDef led_ot, bool  more_90_oC,SwitchTypeDef led_oc, ModeTypeDef mode, bool initial_start);

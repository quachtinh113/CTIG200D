#include "main.h"
#include "cmsis_os.h"
#include <stdbool.h>
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
displayTypeDef show=WELDING_CURRENT;
bool runFan=false;
bool start=false;
SwitchTypeDef led_ot=OFF, led_oc=OFF;
/* Extern variables --------------------------------------------------------*/
extern DAC_HandleTypeDef hdac1;
extern uint16_t welding_current;
extern uint16_t post_gas;
extern uint16_t takt_time; 
extern uint16_t welding_time;
extern StateInputsDef switchSelectMode;
extern bool OT;
extern bool Failed_Start;
extern bool more_90_oC;
extern bool enableUpdateDisplayWeldingTime;
extern bool enableUpdateDisplayWeldingCurrent;
/**@fn feedback_signals(void *argument)
 * @description
 * Process task feedback signals 
 * @retval Node.
 */
void feedback_signals(void *argument){  
	for(;;){
		process_feedback_signals();
		switch(show){
			case WELDING_CURRENT: {
				if(enableUpdateDisplayWeldingCurrent==true){
					enableUpdateDisplayWeldingCurrent=false;
					display_leds(welding_current, false, led_ot, more_90_oC,led_oc, (ModeTypeDef)switchSelectMode.Select_mode, Failed_Start);
				}
			}break;
			case POST_GAS: display_leds(post_gas, true, led_ot, more_90_oC, led_oc, (ModeTypeDef)switchSelectMode.Select_mode, Failed_Start);
			break;
			case TAKT_TIME: {
				if(switchSelectMode.Select_mode==CTIG){
					display_leds(takt_time, true, led_ot, more_90_oC, led_oc, (ModeTypeDef)switchSelectMode.Select_mode, Failed_Start);
				}
			}break;
			case WELDING_TIME: {
				if(switchSelectMode.Select_mode==CTIG && enableUpdateDisplayWeldingTime==true){
					enableUpdateDisplayWeldingTime=false;
					display_leds(welding_time, false, led_ot, more_90_oC, led_oc, (ModeTypeDef)switchSelectMode.Select_mode,Failed_Start); 
				}
			}break;
			default: break;
		}
  }
}
/**@fn process_gpio(void *argument)
 * @description
 * Process task gpio 
 * @retval Node.
 */
void process_gpio(void *argument){  
	for(;;){
		process_inputs();
		if(OT==true){
			if(runFan==true){
				runFan=false;
				/*Turn on fan*/
				fan_on();
				/*Turn on led ot*/
				led_ot=ON;
			}	
		}
		else {
			if(runFan==false){
				runFan=true;
				/*Turn off fan*/
				fan_off();
				/*Turn off led ot*/
				led_ot=OFF;
			}
		}
    osDelay(10);
  }
}
/**@fn process_welding(void *argument)
 * @description
 * Process task welding 
 * @retval Node.
 */
void process_welding(void *argument){  
	for(;;){
		if(switchSelectMode.Select_mode==CTIG){
			process_ctig();
		}
    else {
			process_tig();
		}
  }
}

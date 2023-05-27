#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include "string.h"
#include "clock.h"
#include "uart.h"
#include "adc.h"
#include "dac.h"
#include "timer.h"
#include "gpio.h"
#include "process.h"
/* Private variables --------------------------------------------------------*/
uint16_t time_pregas;
uint32_t time_post_gas;
bool Time_Out=false;
bool EnableWaitTimeOut=false;
bool Failed_Start=false;
bool InitialFail=false;;
uint16_t counter_wait_feedback_current=0;
bool enableChangeMode=true;
bool welding_ctig=false;
bool check;
bool enablePregas=false;
/* Extenal variables --------------------------------------------------------*/
extern DAC_HandleTypeDef hdac1;
extern uint16_t welding_current;
extern uint16_t post_gas;
extern uint16_t takt_time; 
extern uint16_t welding_time;
extern bool OT;
extern bool more_90_oC;
extern StateInputsDef switchSelectMode;
extern uint32_t countTime;

/**@fn wait_current_on(void).
 * @param None.
 * @description 
 * Wait current on if after 2 seconds not current on will message error.
 * @retval false or true.
 */
static bool wait_current_on(void){
	/*Toggle hot start current*/
	#ifdef DEBUG
		send_message("----hot start current");
	#endif
	/*Set hotstart*/
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*50);
	/*Turn off welding helmet*/
	Welding_helmet_off();
	#ifdef DEBUG
		send_message("----delay 40ms");
  #endif
	/*Turn on HF and PWM */
	hf_on(); 
	pwm_on();   
	EnableWaitTimeOut=true;
	counter_wait_feedback_current=0;
	Time_Out=false;
	while(check_current_on()==RELEASE){
		/*Only into this block when more than 3 seconds current_on still off */
		if(Time_Out==true){
			Time_Out=false;
			/*Turn off gas valve, PWM and hf*/
			valve_off();  
			hf_off();   
			pwm_off();  
			/*Turn on welding helmet*/
	    Welding_helmet_on();
			Failed_Start=true;
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*welding_current);
			while(check_trigger()==PRESS){
        CHECK:				
				HAL_Delay(10);	
				if(check_trigger()==RELEASE){
					Failed_Start=false;
					enableChangeMode=true;
					HAL_Delay(1000);
					#ifdef DEBUG
						send_message("Failed start\r\n");
					#endif
					return false;
				}
			}
			goto CHECK;
		}
		/*Only into this block when less than 2 seconds but release trigger*/
		else if(Time_Out==false && check_trigger()==RELEASE){
			enableChangeMode=true;
			/*Turn off PWM*/
			pwm_off(); 
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*welding_current);
			#ifdef DEBUG
			  send_message("\r\nReleased trigger\r\n");
      #endif
			/*Turn off HF*/
			hf_off(); 
			/*Turn on welding helmet*/
	    Welding_helmet_on();
			return false;
		}
	}	
	/*Turn off HF*/
	hf_off();   
	/*Delay 30ms*/
	HAL_Delay(150);
	
	return true;
}
/**@fn process_tig(void).
 * @param None.
 * @description 
 * Function process tig.
 * @retval None.
 */
void process_tig(void){
	START:    
	if(check_trigger()==PRESS && (more_90_oC==false && switchSelectMode.Oc==PRESS))
		{
		out_preset((ModeTypeDef)switchSelectMode.Select_mode);
    enableChangeMode=false;
		/*Turn on gas valve*/ 
		valve_on(); 
		
		/*Check enable pregas*/
		if(enablePregas==true){
			/*Pregas 300ms*/
			time_pregas=_300ms;
			#ifdef DEBUG
				send_message("Pregas gas----");
			#endif
			enablePregas=false;
			while(time_pregas>0){
				if(check_trigger()==RELEASE){
					goto POST_GAS;
				}
			}
		}
		
		/*Yet active current_on then wait active current_on in 2s*/
		if(check_current_on()!=PRESS){
			check=wait_current_on();
			if(check==false){
				InitialFail=true;
				goto POST_GAS;
			}
		}
		/*Process welding*/
		#ifdef DEBUG
			send_message("welding----");
    #endif
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,COEFFICIENT_DAC*(welding_current));
		EnableWaitTimeOut=false;
		while(check_trigger()==PRESS){
			if(check_trigger()==RELEASE){
				goto POST_GAS;
			}
			if(more_90_oC==true || switchSelectMode.Oc==RELEASE ){
				pwm_off(); 
				enableChangeMode=true;
				/*Turn on welding helmet*/
	      Welding_helmet_on();
				goto END;
			}
		}
		POST_GAS:
		pwm_off(); 
		/*Turn on welding helmet*/
	  Welding_helmet_on();
		enableChangeMode=true;
		time_post_gas=100*post_gas;
		#ifdef DEBUG
			send_message("post gas----");
    #endif  
		while(time_post_gas>0){
			if(check_trigger()==PRESS){
				time_post_gas=0;
				goto START;
			}
			/*Check have OT or OC*/
			if(more_90_oC==true || switchSelectMode.Oc==RELEASE ){
				time_post_gas=0;
				goto END;
			}
		}
		END:
		#ifdef DEBUG
			send_message("turn off gas valve\r\n");
    #endif
		/*Turn off valve*/
		valve_off(); 
		/*Enable pregas flag */
		enablePregas=true;
		InitialFail=false;
	}
}
/**@fn process_ctig(void).                      
 * @param None.
 * @description 
 * Function process ctig.
 * @retval None.
 */
void process_ctig(void){
	START:  
	if(check_trigger()==PRESS && more_90_oC==false && switchSelectMode.Oc==PRESS){
		#ifdef DEBUG
			send_message("Start process----");
		#endif
		out_preset((ModeTypeDef)switchSelectMode.Select_mode);
		enableChangeMode=false;
		/*Turn on gas valve*/ 
		valve_on(); 
		/*Check enable pregas*/
		if(enablePregas==true){
			/*Pregas 300ms*/
			time_pregas=_300ms;
			#ifdef DEBUG
				send_message("Pregas gas----");
			#endif
			enablePregas=false;
			while(time_pregas>0){
				if(check_trigger()==RELEASE){
					goto POST_GAS;
				}
			}
		}
		#ifdef DEBUG
			send_message("welding----");
    #endif
		/*Turn off welding helmet*/
		Welding_helmet_off();
		/*Deley 40ms*/
	  HAL_Delay(200);
		/*Turn on pwm and hf*/
		pwm_on();
		hf_on();
		countTime=0;
		do{			 
			RECHECK:
			while(check_current_on()==RELEASE){
				if(check_trigger()==RELEASE){
					goto POST_GAS;
				}
				else {
					goto RECHECK;
				}
			}
			welding_ctig=true;	
			/*Check have OT or OC*/
			if(more_90_oC==true || switchSelectMode.Oc==RELEASE){
				welding_ctig=false;
				enableChangeMode=true;
				/*Turn on welding helmet*/
		    Welding_helmet_on();
				goto END;
			}
		}while(check_trigger()==PRESS);
		POST_GAS:
		/*Turn on welding helmet*/
		Welding_helmet_on();
		welding_ctig=false;
		pwm_off(); 
		hf_off(); 
		enableChangeMode=true;
		time_post_gas=100*post_gas;
		#ifdef DEBUG
			send_message("post gas----");
    #endif
		while(time_post_gas>0){
			if(check_trigger()==PRESS){
				time_post_gas=0;
				goto START;
			}
			if(more_90_oC==true || switchSelectMode.Oc==RELEASE){
				time_post_gas=0;
				goto END;
			}
		}
		END:
		#ifdef DEBUG
			send_message("turn off gas valve\r\n");
    #endif
		/*Turn off valve*/
		valve_off(); 
		/*Enable pregas flag */
		enablePregas=true;
	}
}

#include "main.h"
#include "cmsis_os.h"
/* Private defines -----------------------------------------------------------*/
#define TIMES                    5
/* Private defines -----------------------------------------------------------*/
#define fan_off()                HAL_GPIO_WritePin(Fan_GPIO_Port, Fan_Pin, GPIO_PIN_SET)
#define fan_on()                 HAL_GPIO_WritePin(Fan_GPIO_Port, Fan_Pin, GPIO_PIN_RESET)
#define pwm_off()                HAL_GPIO_WritePin(PWM_GPIO_Port, PWM_Pin, GPIO_PIN_SET)
#define pwm_on()                 HAL_GPIO_WritePin(PWM_GPIO_Port, PWM_Pin, GPIO_PIN_RESET)
#define valve_off()              HAL_GPIO_WritePin(Valve_GPIO_Port, Valve_Pin, GPIO_PIN_SET)
#define valve_on()               HAL_GPIO_WritePin(Valve_GPIO_Port, Valve_Pin, GPIO_PIN_RESET)
#define buzzer_off()             HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET)
#define buzzer_on()              HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET)
#define hf_off()                 HAL_GPIO_WritePin(HF_GPIO_Port, HF_Pin, GPIO_PIN_RESET)
#define hf_on()                  HAL_GPIO_WritePin(HF_GPIO_Port, HF_Pin, GPIO_PIN_SET)
#define Welding_helmet_off()     HAL_GPIO_WritePin(Welding_helmet_GPIO_Port, Welding_helmet_Pin, GPIO_PIN_SET)
#define Welding_helmet_on()      HAL_GPIO_WritePin(Welding_helmet_GPIO_Port, Welding_helmet_Pin, GPIO_PIN_RESET)
/* Private typedef -----------------------------------------------------------*/
typedef enum{
  WAIT = 0,
  SELECT_MODE
} InputTypeDef;
typedef struct {
	uint8_t Select_mode;
	uint8_t Oc;
	uint8_t Trigger;
	uint8_t CurrentOn;
}StateInputsDef;
extern void MX_GPIO_Init(void);
extern ModeTypeDef check_select_mode(void);
static StateInputTypeDef check_oc(void);
extern StateInputTypeDef check_current_on(void);
extern StateInputTypeDef check_trigger(void);
extern void process_inputs(void);
static void soft_reset(void);

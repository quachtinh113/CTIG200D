#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
/* Private defines -----------------------------------------------------------*/
#define _300ms                         300
#define _10ms                           10
#define _3ms                            15
static bool wait_current_on(void);
extern void process_tig(void);
extern void process_ctig(void);
extern void reset_soft(void);

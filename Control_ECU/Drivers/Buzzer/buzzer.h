#ifndef BUZZER_H_
#define BUZZER_H_
#include <stdint.h>

//functions declarations
void init_Buzzer(void);
void Timer0A_Handler(void);
void toggle_Buzzer(void);
void Buzzer_Start(void);


#endif

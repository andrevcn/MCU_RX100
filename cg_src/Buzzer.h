#ifndef __BUZZER_H
#define __BUZZER_H

/* Inclusoes de arquivos de cabecalho ----------------------------------------*/
#include "r_cg_main.h"
#include "r_cg_userdefine.h"

void BUZ_PinInit(void);
void BUZ_Play(uint8_t times, uint16_t time_on, uint16_t time_off);
void BUZ_Stop(void);
void BUZ_ISR_Task(void);

#endif /*__BUZZER_H */



#ifndef __PIT_H__
#define __PIT_H__

#include <isr.h>

void pit_init();
void isr_timerTick_c();
void sleep(int);

extern void isr_timerTick();

#endif

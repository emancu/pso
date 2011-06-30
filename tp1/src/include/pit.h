#ifndef __PIT_H__
#define __PIT_H__

#include <isr.h>
#include <vga.h>
#include <i386.h>
#include <debug.h>  // We need in_panic
#include <loader.h> // We need cur_pid

void pit_init();
void isr_timerTick_c();
void sleep(int);

extern void isr_timerTick();

#endif

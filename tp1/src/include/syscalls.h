#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <tipos.h>

#define SYS_INT 0x30

uint_32 syscall_list[128];
extern void isr_syscall();

//todo alemata PARA QUE ESTA LA DEFINICION ACA... YA ESTA EN mm.h
extern void* palloc();

#ifdef __KERNEL__

// Sólo se compila en modo "kernel"

#else
// __TAREA___

// Sólo se compila en modo "tarea"

// Declarar los "wrapers" para los syscalls que incluyen las tareas.

#endif

#endif

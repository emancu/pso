#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <tipos.h>

#define SYS_INT 0x30

uint_32 syscall_list[128];
//esta es la syscal para hacer un llamado a una funcion del kernel.
extern void isr_syscall();

//todo alemata PARA QUE ESTA LA DEFINICION ACA... YA ESTA EN mm.h
extern void* palloc();
extern uint_32 getpid();

extern int read(int fd, void* buf, uint_32 size);
extern int write(int fd, const void* buf, uint_32 size);
extern int seek(int fd, uint_32 size);
extern int close(int fd);

extern int open(const char* filename, uint_32 flags);



#ifdef __KERNEL__

// Sólo se compila en modo "kernel"

#else
// __TAREA___

// Sólo se compila en modo "tarea"

// Declarar los "wrapers" para los syscalls que incluyen las tareas.

#endif


#endif

#ifndef __LOADER_H__
#define __LOADER_H__

#include <pso_file.h>
#include <tss.h>
#include <syscalls.h>

#define PID_IDLE_TASK 0
#define MAX_PID 32

#ifdef __KERNEL__

typedef struct str_task {
	int next, prev;
	// Informaci'on
} task;

extern task task_table[];
extern uint_32 cur_pid;

void loader_init(void);
pid loader_load(pso_file* f, int pl);

void loader_enqueue(int* cola);
void loader_unqueue(int* cola);

void loader_exit(void);

void loader_switchto(pid pd);

#endif

/* Syscalls */
// pid getpid(void);
// void exit(pid pd);

#endif

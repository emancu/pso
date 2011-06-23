#ifndef __LOADER_H__
#define __LOADER_H__

#include <pso_file.h>
#include <tss.h>
#include <syscalls.h>
#include <device.h>

#define PID_IDLE_TASK 0
#define MAX_PID 32

#define STACK_0_VIRTUAL 0xFFFFE000
#define STACK_3_VIRTUAL 0xFFFFD000

#ifdef __KERNEL__

typedef struct str_task {
  int next;
  int prev;
  uint_32 cr3;
  uint_32 esp0;
} task;

/* Variables */
extern task task_table[];
extern uint_32 cur_pid;

/* Funciones */
void loader_init(void);

pid loader_load(pso_file* f, int pl);
void loader_enqueue(int* cola);
void loader_unqueue(int* cola);
extern void loader_switchto(pid pd);
extern void task_ret(void);
extern void fork_ret(void);
void loader_tick();
void loader_exit(void);

uint_32 get_new_pid(void);
void free_pid(uint_32);

/* Syscalls */
uint_32 getpid(void);
uint_32 sys_getpid(void);

void sys_exit(void);

uint_32 sys_fork(uint_32 org_eip, uint_32 org_esp);

#endif
#endif

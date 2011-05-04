#ifndef __LOADER_H__
#define __LOADER_H__

#include <pso_file.h>
#include <tss.h>
#include <syscalls.h>

#define PID_IDLE_TASK 0
#define MAX_PID 32

#ifdef __KERNEL__

typedef struct str_task {
  int next;
  int prev;
  struct task_tss *context;
} task;

typedef struct str_task_tss {
  uint_32 edi;
  uint_32 esi;
  uint_32 ebp;
  uint_32 esp;
  uint_32 ebx;
  uint_32 edx;
  uint_32 ecx;
  uint_32 eax;
  uint_32 eflags;
  uint_32 eip;
  uint_32 cr3;
  uint_32 esp0;
  uint_16 ss0;
  // Informaci'on
} task_tss;

extern task task_table[];
extern uint_32 cur_pid;

void loader_init(void);
pid loader_load(pso_file* f, int pl);

void loader_enqueue(int* cola);
void loader_unqueue(int* cola);

void loader_exit(void);

void loader_switchto(pid pd);


uint_32 get_pid(void);
void free_pids(uint_32);

#endif

/* Syscalls */
// pid getpid(void);
// void exit(pid pd);

#endif

#ifndef __SCHED_H__
#define __SCHED_H__

#include <tss.h>
#include <tipos.h>
#include <syscalls.h>


int debugEnabled;

#ifdef __KERNEL__



void sched_init(void);

void sched_load(pid);
void sched_unblock(pid);

int sched_exit();
int sched_block();
int sched_tick();


// Auxiliar
void configure_task(pid, int, pid, pid);

void show_task_structure(pid pd);
void showTasks();


// Testing

bool sched_test_size(int);
bool sched_test_last(pid);
bool sched_test_current(pid);
bool sched_test_status(pid, int);
bool sched_test_node(pid , pid , pid );


#endif

#endif /* SCHED_H_ */

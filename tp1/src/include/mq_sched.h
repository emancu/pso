#ifndef __MQ_SCHED_H__
#define __MQ_SCHED_H__

#include <idt.h>
#include <vga.h>
#include <tss.h>
#include <tipos.h>
#include <loader.h>
#include <syscalls.h>

#ifdef __KERNEL__

void sched_init(void);

void sched_load(pid);
void sched_unblock(pid);

int sched_exit();
int sched_block();
int sched_tick();

// Private
void configure_task(pid, int, int, pid, pid);
void enqueue(pid, int);
pid dequeue(int);
void update_current_pid();


// Show
void show_task_structure(pid pd);
void showTasks();


// Testing
bool sched_test_size(int);
bool sched_test_last(pid);
bool sched_test_current(pid);
bool sched_test_status(pid, int);
bool sched_test_node(pid , pid , pid );

#endif
#endif /* MQ_SCHED_H_ */

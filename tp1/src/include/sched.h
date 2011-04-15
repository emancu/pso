#ifndef __SCHED_H__
#define __SCHED_H__

#include <tss.h>
#include <tipos.h>
#include <syscalls.h>

#ifdef __KERNEL__

void sched_init(void);

void sched_load(pid);
void sched_unblock(pid);

int sched_exit();
int sched_block();
int sched_tick();

#endif

#endif /* SCHED_H_ */

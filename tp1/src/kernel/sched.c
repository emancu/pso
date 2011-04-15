#include <idt.h>
#include <sched.h>
#include <loader.h>


typedef struct str_sched_task {
	uint_32 state;
	uint_32 quantum;
	// ...etc... (se puede modificar todo este struct)
} sched_task;
sched_task tasks[MAX_PID];


void sched_init(void) {
}

void sched_load(pid pd) {
}

void sched_unblock(pid pd) {
}

int sched_exit() {
	return 0;
}
int sched_block() {
	return 0;
}
int sched_tick() {
	return 0;
}

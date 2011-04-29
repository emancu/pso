#include <idt.h>
#include <sched.h>
#include <loader.h>


#define STATE_RUNNING 0
#define STATE_BLOCKED 1

typedef struct str_sched_task {
  uint_32 state;
  uint_32 next;
  uint_32 prev;
} sched_task;


sched_task tasks[MAX_PID];
static uint_32 quantum;
static uint_32 current_pid;
static uint_32 last_task_pid;


void sched_init(void) {
  current_pid = 0; // IDLE task
  //no hacemos nada, el quantum se setea en el tick o block
}

void sched_load(pid pd) {
  // last esta en 0 significa que la queue esta vacia,

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
  // si es el IDLE task devolvemos inmediatamente la que sigue
  if (current_pid == 0){
    quantum = 10;
    if (last_task_pid != 0)
      return last_task_pid;
    return 0; // IDLE Task
  }

  // Si no es IDLE Task
  if (--quantum == 0){
    quantum = 10;
    return tasks[current_pid].next;
  }
  return current_pid;
}

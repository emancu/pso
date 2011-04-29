#include <idt.h>
#include <sched.h>
#include <loader.h>


#define STATE_RUNNING 0
#define STATE_BLOCKED 1
#define STATE_FINISHED 2

typedef struct str_sched_task {
  int state;
  pid next;
  pid prev;
} sched_task;


sched_task tasks[MAX_PID];
static uint_32 quantum;
static pid current_pid;
static pid last;


void sched_init(void) {
  last = 0; // Cola vacia
  current_pid = 0; // IDLE task
}

//Agregamos despues de
void sched_load(pid pd) {
  // last esta en 0 significa que la queue esta vacia,
  if( current_pid == 0){
    tasks[pd].next = pd;
    tasks[pd].prev = pd;
  }else{
    tasks[pd].prev = last;
    tasks[pd].next = tasks[last].next;
    tasks[tasks[last].next].prev = pd;
    tasks[last].next = pd;
  }
  last = pd;
  tasks[pd].state = STATE_RUNNING;
}

void sched_unblock(pid pd) {
  tasks[pd].state = STATE_RUNNING;
  if( current_pid == 0){
    tasks[pd].next = pd;
    tasks[pd].prev = pd;
  }else{
    tasks[pd].prev = current_pid;
    tasks[pd].next = tasks[current_pid].next;
    tasks[tasks[current_pid].next].prev = pd;
    tasks[current_pid].next = pd;
  }
  /* No modificamos el last, por que si lo modificamos, las nuevas tareas aparecen detras
     y se cambiaria el orden de ejecucion. */
}

int sched_exit() {
  quantum = 10;
  if (tasks[current_pid].next == current_pid){
    // Es el ultimo elemento en la cola
    last = 0;
    current_pid = 0;
  }else{
    int next = tasks[current_pid].next;
    int prev = tasks[current_pid].prev;
    tasks[next].prev = prev;
    tasks[prev].next = next;
    last = prev;
    current_pid = next;
  }
  configure_task(current_pid, STATE_FINISHED, -1, -1);
  return current_pid;
}

int sched_block() {
  quantum = 10;
  if (tasks[current_pid].next == current_pid){
    // Es el ultimo elemento en la cola
    last = 0;
    current_pid = 0;
  }else{
    int next = tasks[current_pid].next;
    int prev = tasks[current_pid].prev;
    tasks[next].prev = prev;
    tasks[prev].next = next;
    last = prev;
    current_pid = next;
  }
  configure_task(current_pid, STATE_BLOCKED, -1, -1);

  return current_pid;
}

int sched_tick() {
  // si es el IDLE task devolvemos inmediatamente la que sigue
  if (current_pid == 0){
    if (last != 0){
      quantum = 10;
      return last;
    }
    return 0; // IDLE Task
  }

  // Si no es IDLE Task
  if (--quantum == 0){
    quantum = 10;
    return tasks[current_pid].next;
  }
  return current_pid;
}

void configure_task(pid pd, int state, pid next, pid prev){
  tasks[pd].state = state;
  tasks[pd].next = next;
  tasks[pd].prev = prev;
}

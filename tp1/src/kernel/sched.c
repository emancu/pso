
#include <sched.h>

#define STATE_RUNNING 0
#define STATE_BLOCKED 1
#define STATE_FINISHED 2

#define QUANTUM 2


char* states[] = {"running", "blocked", "finished"};

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
    tasks[pd].prev = current_pid;
    tasks[pd].next = tasks[current_pid].next;
    tasks[tasks[current_pid].next].prev = pd;
    tasks[current_pid].next = pd;
  }
  last = pd;
  tasks[pd].state = STATE_RUNNING;
}

void sched_unblock(pid pd) {
  tasks[pd].state = STATE_RUNNING;
  if( current_pid == 0){
    tasks[pd].next = pd;
    tasks[pd].prev = pd;
    //si pasa esto salto inmediatamente a la nueva tarea
    current_pid = pd;
    last = current_pid;
    loader_switchto(pd);
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
  quantum = QUANTUM;
  if (tasks[current_pid].next == current_pid){
    // Es el ultimo elemento en la cola
    last = 0;
    configure_task(current_pid, STATE_FINISHED, -1, -1);
    current_pid = 0;
  }else{
    int next = tasks[current_pid].next;
    int prev = tasks[current_pid].prev;
    tasks[next].prev = prev;
    tasks[prev].next = next;
    last = prev;
    configure_task(current_pid, STATE_FINISHED, -1, -1);
    current_pid = next;
  }
  return current_pid;
}

int sched_block() {
  quantum = QUANTUM;
  if (tasks[current_pid].next == current_pid){
    // Es el ultimo elemento en la cola
    last = 0;
    configure_task(current_pid, STATE_BLOCKED, -1, -1);
    current_pid = 0;
  }else{
    int next = tasks[current_pid].next;
    int prev = tasks[current_pid].prev;
    tasks[next].prev = prev;
    tasks[prev].next = next;

    if(current_pid == last)
      last = prev;
    configure_task(current_pid, STATE_BLOCKED, -1, -1);
    current_pid = next;
  }

  return current_pid;
}

int sched_tick() {
  // si es el IDLE task devolvemos inmediatamente la que sigue
  if (current_pid == 0){
    if (last != 0){
      quantum = QUANTUM;
      current_pid = last;
    }
    return current_pid;
  }

  // Si no es IDLE Task
  if (--quantum == 0){
    quantum = QUANTUM;
    current_pid = tasks[current_pid].next;
  }
  return current_pid;
}

void configure_task(pid pd, int state, pid next, pid prev){
  tasks[pd].state = state;
  tasks[pd].next = next;
  tasks[pd].prev = prev;
}


// Show
void showTasks(){
  int i = 0;
  for(i = 0 ; i <= 4 ; i++){
    show_task_structure(i);
  }
  printf("current pid: %d", current_pid);
  printf("last: %d", last);
}


void show_task_structure(pid pd){
  printf("----------");
  printf("mi pid: %d estado: %s next: %d prev: %d " , pd, states[tasks[pd].state], tasks[pd].next, tasks[pd].prev);
}


// Testing

bool sched_test_size(int expected_size){
  int size;
  if (last == 0){
    size =0;
  }else{
    pid aux = last;
    size = 1;
    while(tasks[aux].next != last){
      size++;
      aux = tasks[aux].next;
    }
  }

  if( expected_size != size){
    printf("Size %d, expected %d",size, expected_size);
    return 0;
  }
  return 1;
}

bool sched_test_last(pid expected_pid){
  if( last != expected_pid){
    printf("Last = %d, expected %d",last, expected_pid);
    return 0;
  }
  return 1;
}

bool sched_test_current(pid expected_current_pid){
  if( current_pid != expected_current_pid){
    printf("Current pid = %d, expected %d",current_pid, expected_current_pid);
    return 0;
  }
  return 1;
}

bool sched_test_status(pid task_pid, int expected_state){
  if (tasks[task_pid].state != expected_state){
    printf("State of %d is %d, expected %d",task_pid, tasks[task_pid].state, expected_state);
    return 0;
  }
  return 1;
}

bool sched_test_node(pid node, pid expected_next, pid expected_prev){
  if((tasks[node].next != expected_next)||(tasks[node].prev != expected_prev)){
    printf("Node %d [next]=%d, [prev]=%d, expected [next]=%d, expected [prev]=%d",node, tasks[node].next, tasks[node].prev, expected_next, expected_prev);
    return 0;
  }
  return 1;
}

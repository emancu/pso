#include <mq_sched.h>

#define STATE_RUNNING   0
#define STATE_BLOCKED   1
#define STATE_FINISHED  2

#define TYPE_NONE     -1
#define TYPE_REALTIME  0
#define TYPE_LOW       1

#define QUANTUM 2

char* states[] = {"running", "blocked", "finished"};

typedef struct str_sched_task {
  int state;
  int type;
  pid next;
  pid prev;
} sched_task;


sched_task tasks[MAX_PID];

static uint_32 quantum;
static pid current_pid;
static pid next_pid[2];


void sched_init(void) {
  next_pid[TYPE_REALTIME], next_pid[TYPE_LOW] = 0; // Cola vacia
  current_pid = 0; // IDLE task
}

// Todas las tareas comienzan por ser de baja prioridad
void sched_load(pid pd) {
  int next = pd;
  int prev = pd;

  if (next_pid[TYPE_LOW] != 0) {
    next = next_pid[TYPE_LOW];
    prev = tasks[next].prev;
  }

  enqueue(pd, TYPE_LOW, next, prev);
  // Sigue apuntando al next, a menos que este vacia. En ese caso apunta a si misma
  next_pid[TYPE_LOW] = tasks[pd].next;
}

// Si una tarea se desbloquea, pasa a tener alta prioridad
void sched_unblock(pid pd) {
  int next = pd;
  int prev = pd;

  if(next_pid[TYPE_REALTIME] != 0){
    next = next_pid[TYPE_REALTIME];
    prev = tasks[next].prev;
  }

  enqueue(pd, TYPE_REALTIME, next, prev);
  // Sigue apuntando al next, a menos que este vacia. En ese caso apunta a si misma
  next_pid[TYPE_REALTIME] = tasks[pd].next;

  // Si esta IDLE salto inmediatamente a la nueva tarea
  // VER SI SE LO AGREGO AL SCHED_LOAD
  if( current_pid == 0){
    current_pid = pd;
    loader_switchto(pd);
  }
}

int sched_exit() {
  return dequeue(STATE_FINISHED);
}

int sched_block() {
  return dequeue(STATE_BLOCKED);
}

int sched_tick() {
  // si es el IDLE task devolvemos inmediatamente la que sigue
  if (current_pid == 0) {
    quantum = QUANTUM;

    if (next_pid[TYPE_REALTIME] != 0)
      current_pid = next_pid[TYPE_REALTIME];
    else
      current_pid = next_pid[TYPE_LOW];

  } else {  // Si no es IDLE Task
    if (--quantum == 0){
      pid tmp_pid = current_pid;

      quantum = QUANTUM;
      dequeue(STATE_RUNNING); // VER QUE ESTADO LE CORRESPONDE
      // Se excedio en tiempo de ejecucion, la bajamos.
      int next = tmp_pid;
      int prev = tmp_pid;

      if (next_pid[TYPE_LOW] != 0) {
        next = next_pid[TYPE_LOW];
        prev = tasks[next].prev;
      }

      enqueue(tmp_pid, TYPE_LOW, next, prev);
      // Sigue apuntando al next, a menos que este vacia. En ese caso apunta a si misma
      next_pid[TYPE_LOW] = tasks[pd].next;

      current_pid = next_pid[TYPE_REALTIME];
      // SALTEAR NEXT.next para la prox ejecucion
      if (current_pid == 0)
        current_pid = next_pid[TYPE_LOW];
    }
  }
  return current_pid;
}


/*
 * Private
 */

void configure_task(pid pd, int type, int state, pid next, pid prev){
  tasks[pd].type  = type;
  tasks[pd].state = state;
  tasks[pd].next  = next;
  tasks[pd].prev  = prev;
}

void enqueue(pid pd, int type, pid next, pid prev) {
  configure_task(pd, type, STATE_RUNNING, next, prev);

  tasks[next].prev = pd;
  tasks[prev].next = pd;
}

// Makes the transition to a specified state.
// BLOCKED or FINISHED
pid dequeue(int state) {
  quantum = QUANTUM;

  // Es el ultimo elemento en la cola
  if (tasks[current_pid].next == current_pid) {
    last = 0;
    // Como se desencola, no pertenece a ninguna cola
    configure_task(current_pid, TYPE_NONE, state, -1, -1);
    current_pid = 0;
  }else{
    int next = tasks[current_pid].next;
    int prev = tasks[current_pid].prev;
    tasks[next].prev = prev;
    tasks[prev].next = next;
    last = prev;
    // Como se desencola, no pertenece a ninguna cola
    configure_task(current_pid, TYPE_NONE, state, -1, -1);
    current_pid = next;
  }
  return current_pid;
}


/*
 * END Private
 */

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


/*
 * Testing
 */

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

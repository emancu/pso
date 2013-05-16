#define STATE_RUNNING   0
#define STATE_BLOCKED   1
#define STATE_FINISHED  2

#define TYPE_NONE      -1
#define TYPE_REALTIME   0
#define TYPE_LOW        1

void test_scheduler(){
  int i=0;
  printf("Testing Scheduler");
  sched_load(1);
  sched_load(2);
  sched_load(3);

  sched_test_size(TYPE_REALTIME, 0);
  sched_test_size(TYPE_LOW,3);
  sched_test_next_pid(TYPE_REALTIME, 0);
  sched_test_next_pid(TYPE_LOW, 1);
  sched_test_current(0);
  sched_test_status(1, STATE_RUNNING);
  sched_test_status(2, STATE_RUNNING);
  sched_test_status(3, STATE_RUNNING);
  sched_test_node(1,2,3);
  sched_test_node(2,3,1);
  sched_test_node(3,1,2);
  printf("Load Pass.");

  // De 0 a la last
  printf("Start Tick test.");
  sched_tick();

  sched_test_size(TYPE_REALTIME, 0);
  sched_test_size(TYPE_LOW,3);
  sched_test_next_pid(TYPE_REALTIME, 0);
  sched_test_next_pid(TYPE_LOW, 2);
  sched_test_current(1); // Debe cambiar el current
  sched_test_status(1, STATE_RUNNING);
  sched_test_status(2, STATE_RUNNING);
  sched_test_status(3, STATE_RUNNING);
  sched_test_node(1,2,3);
  sched_test_node(2,3,1);
  sched_test_node(3,1,2);
  printf("Tick Pass.");

  // Pasamos varios ticks para cambiar de tarea
  for(i=0; i < 21; i++){
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Hice tick %d", i);
    sched_tick();
  }

  sched_test_current(2);
  printf("Tick with task switch pass.");

  sched_block();

  sched_test_size(TYPE_REALTIME, 0);
  sched_test_size(TYPE_LOW,2);
  sched_test_next_pid(TYPE_REALTIME, 0);
  sched_test_next_pid(TYPE_LOW, 1);
  sched_test_current(3); // Debe cambiar el current
  sched_test_status(1,STATE_RUNNING);
  sched_test_status(2,STATE_BLOCKED);
  sched_test_status(3,STATE_RUNNING);
  sched_test_type(2,TYPE_NONE);
  sched_test_node(1,3,3);
  sched_test_node(2,-1,-1);
  sched_test_node(3,1,1);
  printf("Block Pass.");

  // Termina la ejecucion de la tarea actual
  sched_exit();

  sched_test_size(TYPE_REALTIME, 0);
  sched_test_size(TYPE_LOW,1);
  sched_test_next_pid(TYPE_REALTIME, 0);
  sched_test_next_pid(TYPE_LOW, 1);
  sched_test_current(1); // Debe cambiar el current
  sched_test_status(1,STATE_RUNNING);
  sched_test_status(2,STATE_BLOCKED);
  sched_test_status(3,STATE_FINISHED); // Debe cambiar el state
  sched_test_node(1,1,1);
  sched_test_node(2,-1,-1);
  sched_test_node(3,-1,-1);
  printf("Exit Pass.");

  // Desbloquear la 2 y volverla a la cola de ejecucion
  sched_unblock(2);

  sched_test_size(TYPE_REALTIME, 1);
  sched_test_size(TYPE_LOW,1);
  sched_test_next_pid(TYPE_REALTIME, 2);
  sched_test_next_pid(TYPE_LOW, 1);
  sched_test_current(1); // FIXME: Creo q cambia a la 2
  sched_test_status(1,STATE_RUNNING);
  sched_test_status(2,STATE_RUNNING); //Debe cambiar el state
  sched_test_status(3,STATE_FINISHED);
  sched_test_type(1,TYPE_LOW);
  sched_test_type(2,TYPE_REALTIME);
  sched_test_node(1,1,1);
  sched_test_node(2,2,2);
  sched_test_node(3,-1,-1);
  printf("Unblock Pass.");

  printf("Finish Scheduler test");
}

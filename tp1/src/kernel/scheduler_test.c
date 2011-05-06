void test_scheduler(){
  int i=0;
  printf("Testing Scheduler");
  sched_load(1);
  sched_load(2);
  sched_load(3);

  sched_test_size(3);
  sched_test_last(3);
  sched_test_current(0);
  sched_test_status(1,0);
  sched_test_status(2,0);
  sched_test_status(3,0);
  sched_test_node(1,2,3);
  sched_test_node(2,3,1);
  sched_test_node(3,1,2);
  printf("Load Pass.");

  // De 0 a la last
  sched_tick();

  sched_test_size(3);
  sched_test_last(3);
  sched_test_current(3); // Debe cambiar el current
  sched_test_status(1,0);
  sched_test_status(2,0);
  sched_test_status(3,0);
  sched_test_node(1,2,3);
  sched_test_node(2,3,1);
  sched_test_node(3,1,2);
  printf("Tick Pass.");

  // Pasamos varios ticks para cambiar de tarea
  for(i=0; i< 25; i++)
    sched_tick();

  sched_test_current(2);
  printf("Tick with task switch pass.");

  sched_block();

  sched_test_size(2); //Debe cambiar el size
  sched_test_last(3);
  sched_test_current(3); // Debe cambiar el current
  sched_test_status(1,0);
  sched_test_status(2,1);
  sched_test_status(3,0);
  sched_test_node(1,3,3);
  sched_test_node(2,-1,-1);
  sched_test_node(3,1,1);
  printf("Block Pass.");

  // Termina la ejecucion de la tarea actual
  sched_exit();

  sched_test_size(1); //Debe cambiar el size
  sched_test_last(1); //Debe cambiar el last
  sched_test_current(1); // Debe cambiar el current
  sched_test_status(1,0);
  sched_test_status(2,1);
  sched_test_status(3,2); // Debe cambiar el state
  sched_test_node(1,1,1);
  sched_test_node(2,-1,-1);
  sched_test_node(3,-1,-1);
  printf("Exit Pass.");

  // Desbloquear la 2 y volverla a la cola de ejecucion
  sched_unblock(2);

  sched_test_size(2); //Debe cambiar el size
  sched_test_last(1);
  sched_test_current(1);
  sched_test_status(1,0);
  sched_test_status(2,0); //Debe cambiar el state
  sched_test_status(3,2);
  sched_test_node(1,2,2); //Debe cambiar el next y prev
  sched_test_node(2,1,1); //Debe cambiar el next y prev
  sched_test_node(3,-1,-1);
  printf("Unblock Pass.");

  printf("Finish Scheduler test");
}

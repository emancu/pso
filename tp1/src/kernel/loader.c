#include <loader.h>
#include <vga.h>
#include <mm.h>
#include <sched.h>
#include <i386.h>

task task_table[MAX_PID];
uint_32 cur_pid;

uint_32 tasks_running;
uint_32 tasks_blocked;

static int free_pids[MAX_PID];
static int next_free;

void loader_init(void) {
  int i;
  // Inicializamos la cola de peeds libres
  free_pids[0] = -1; // La IDLE ocupa la 0, nadie deberia apuntar a la 0
  for (i = 1; i < MAX_PID - 1; i++)
    free_pids[i] = i + 1;
  free_pids[MAX_PID - 1] = -1; // es la ultima

  next_free = 1;
  cur_pid = 0;

  //NOTE: Deberiamos contar la IDLE Task?
  tasks_running = tasks_blocked = 0;

  //hay que generar la tarea actual.. que dps se convierte en idle
  task_table[0].cr3 = rcr3();

  //registro las syscalls (UNIFICAR UN LUGAR O CADA UNA EN SU MODULO)
  syscall_list[0x31] = (uint_32) &sys_getpid;
  syscall_list[0x32] = (uint_32) &sys_exit;
  syscall_list[0x38] = (uint_32) &sys_fork;
}

pid loader_load(pso_file* f, int pl) {
  //me guardo el cr3 viejo.
  uint_32 old_cr3 = rcr3();

  //pido un directorio para la nueva tarea
  void* task_dir = mm_dir_new();

  //TODO VER CUANTA MEMORIA NECESITA REALMENTE
  void* puntero_page_tarea = mm_mem_alloc();

  //stacks de anillo 3 y 0 para la tarea
  void* task_stack3 = mm_mem_alloc();
  void* task_stack0 = mm_mem_alloc();

  //ver esto donde van mapeados los stacks
  mm_page_map(STACK_3_VIRTUAL, task_dir, (uint_32) task_stack3, 0, USR_STD_ATTR);
  mm_page_map(STACK_0_VIRTUAL, task_dir, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

  //TODO ver estas direcciones temporales donde ponerlas
  mm_page_map(KERNEL_TEMP_PAGE,(mm_page *) old_cr3, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

  //inicializamos la pila de nivel 0 para que tenga el contexto para
  //poder volver del switchto
  uint_32* stack0 = (uint_32*) (KERNEL_TEMP_PAGE + 0xffC);
  *stack0-- = 0x23;
  *stack0-- = STACK_3_VIRTUAL + 0x1000;
  *stack0-- = 0x202;
  *stack0-- = 0x1B;
  *stack0-- = (uint_32) f->_main;
  *stack0-- = (uint_32) &task_ret;
  *stack0-- = resp();
  *stack0-- = 0x0;
  *stack0-- = 0x0;
  *stack0-- = 0x0;

  mm_page_map((uint_32) f->mem_start, task_dir, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);

  //mapeo la direccion virtual temporal para copiar en la pagina que recien se me asigno.
  mm_page_map((uint_32) KERNEL_TEMP_PAGE,(mm_page *) old_cr3, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);
  tlbflush();

  //copio la tarea desde donde esta a la pagina que acabo de mapear.
  uint_8* addr_to_copy = (uint_8*) KERNEL_TEMP_PAGE;
  uint_8* task_to_copy = (uint_8*) f;
  uint_32 cant_to_copy = f->mem_end_disk - f->mem_start;
  int i;

  for (i = 0; i < cant_to_copy; i++) {
    *addr_to_copy++ = *task_to_copy++;
  }

  //tengo que armar la estreuctura
  uint_32 requested_pid = get_new_pid();
  task_table[requested_pid].cr3 = (uint_32) task_dir;
  task_table[requested_pid].esp0 = STACK_0_VIRTUAL + 0xFD8;

  mm_page_free(KERNEL_TEMP_PAGE,(mm_page *) old_cr3);

  tlbflush();

  sched_load(requested_pid);

  tasks_running++;
  return requested_pid;
}

void loader_tick() {
  int new_current_pid = sched_tick();
  if (new_current_pid != cur_pid) {
    loader_switchto(new_current_pid);
  }
}

void loader_enqueue(int* cola) {
  if (*cola == -1) {
    // Esta vacia, hay que agregar el primer elemento
    task_table[cur_pid].next = cur_pid;
    task_table[cur_pid].prev = cur_pid;
    *cola = cur_pid;
  } else {
    int prev_pid = task_table[*cola].prev;
    // Lo agregamos atras para evitar starvation
    task_table[prev_pid].next = cur_pid;
    task_table[*cola].prev = cur_pid;
    task_table[cur_pid].next = *cola;
    task_table[cur_pid].prev = prev_pid;
  }

  int i = sched_block();
  tasks_blocked++;
  tasks_running--;
  loader_switchto(i);
}

void loader_unqueue(int* cola) {
  int old_cola = *cola;

  if (*cola != -1) {
    int next_node = task_table[*cola].next;
    int prev_node = task_table[*cola].prev;

    task_table[next_node].prev = prev_node;
    task_table[prev_node].next = next_node;
    task_table[*cola].next = -1;
    task_table[*cola].prev = -1;
    *cola = ((next_node == *cola) ? -1 : next_node);
    sched_unblock(old_cola);
    tasks_blocked--;
    tasks_running++;
  }
}

void loader_exit(void) {
  device_release_devices(cur_pid);
  mm_dir_free((mm_page*) task_table[cur_pid].cr3);
  loader_switchto(sched_exit());
  free_pid(cur_pid);
  tasks_running--;
}

uint_32 get_new_pid(void) {
  int ret = next_free;
  next_free = free_pids[next_free];
  return ret;
}

void free_pid(uint_32 pid) {
  free_pids[pid] = next_free;
  next_free = pid;
}

/*
 * Syscalls
 */

uint_32 sys_getpid(void) {
  return cur_pid;
}


uint_32 sys_fork(uint_32 org_eip, uint_32 org_esp){
  //me guardo el cr3 viejo.
  uint_32 old_cr3 = rcr3();

  //pido un directorio para la nueva tarea
  void* new_cr3 = mm_dir_fork((mm_page*)old_cr3);
  if (new_cr3 == NULL) //No pudo hacerse fork de la estrucutra de paginación
    return -1;

   //stacks de anillo 3 y 0 para la tarea
  void* task_stack3 = mm_mem_alloc();
  void* task_stack0 = mm_mem_alloc();

  //ver esto donde van mapeados los stacks
  mm_page_map(STACK_3_VIRTUAL, new_cr3, (uint_32) task_stack3, 0, USR_STD_ATTR);
  mm_page_map(STACK_0_VIRTUAL, new_cr3, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

  //TODO ver estas direcciones temporales donde ponerlas
  mm_page_map(KERNEL_TEMP_PAGE,(mm_page *) old_cr3, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

  //inicializamos la pila de nivel 0 para que tenga el contexto para
  //poder volver del switchto
  uint_32* stack0 = (uint_32*) (KERNEL_TEMP_PAGE + 0xffC);
  *stack0-- = 0x23;
  *stack0-- = org_esp;
  *stack0-- = 0x202;
  *stack0-- = 0x1B;
  *stack0-- = org_eip;
  *stack0-- = (uint_32) &fork_ret;
  *stack0-- = resp();
  *stack0-- = 0x0;
  *stack0-- = 0x0;
  *stack0-- = 0x0;

  // mm_page_map((uint_32) f->mem_start, task_dir, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);
  //Copio la pila de usuario como está
  mm_copy_vf((uint_32*)STACK_3_VIRTUAL, (uint_32)task_stack3, PAGE_SIZE);

  mm_page_free(KERNEL_TEMP_PAGE, (mm_page*) old_cr3);
  tlbflush();

  //tengo que armar la estructura de proceso
  uint_32 requested_pid = get_new_pid();
  task_table[requested_pid].cr3 = (uint_32) new_cr3;
  task_table[requested_pid].esp0 = STACK_0_VIRTUAL + 0xFD8;

  //Duplico los file descriptor actualizando referencias
  device_fork_descriptors(cur_pid, requested_pid);

  // esto esta mal.. tiene q decidir q numero devolver creo q necesitamos un semaforo!
  sched_load(requested_pid);

  return requested_pid;
}

void sys_exit(void) {
  loader_exit();
}


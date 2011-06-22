#include <loader.h>
#include <vga.h>
#include <mm.h>
#include <sched.h>
#include <i386.h>

task task_table[MAX_PID];
uint_32 cur_pid;

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

	//hay que generar la tarea actual.. que dps se convierte en idle
	task_table[0].cr3 = rcr3();

	//registro las syscalls (UNIFICAR UN LUGAR O CADA UNA EN SU MODULO)
	syscall_list[0x31] = (uint_32) &sys_getpid;
	syscall_list[0x32] = (uint_32) &sys_exit;
}

pid loader_load(pso_file* f, int pl) {
	//me guardo el cr3 viejo.
	uint_32 old_cr3 = rcr3();

	//pido un directorio para la nueva tarea
	void* task_dir = mm_dir_new();

	//TODO VER CUANTA MEMORIA NESECITA REALMENTE
	void* puntero_page_tarea = mm_mem_alloc();

	//stacks de anillo 3 y 0 para la tarea
	void* task_stack3 = mm_mem_alloc();
	void* task_stack0 = mm_mem_alloc();

	//ver esto donde van mapeados los stacks
	mm_page_map(0x00401000, task_dir, (uint_32) task_stack3, 0, USR_STD_ATTR);
	mm_page_map(0xFFFFF000, task_dir, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

	//TODO ver estas direcciones temporales donde ponerlas
	mm_page_map(0x55555000,(mm_page *) old_cr3, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

	//inicializamos la pila de nivel 0 para que tenga el contexto para
	//poder volver del switchto
	uint_32* stack0 = (uint_32*) 0x55555ffC;
	*stack0-- = 0x23;
	*stack0-- = 0x00402000;
	*stack0-- = 0x202;
	*stack0-- = 0x1B;
	*stack0-- = (uint_32) f->_main;
	*stack0-- = (uint_32) &task_ret;
	*stack0-- = resp();
	*stack0-- = 0x0;
	*stack0-- = 0x0;
	*stack0-- = 0x0;

	//mapeo la direccion virtual 0x00400000 en la pagina que recien se me asigno.
	mm_page_map((uint_32) f->mem_start, task_dir, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);

	//mapeo la direccion virtual temporal para copiar en la pagina que recien se me asigno.
	mm_page_map((uint_32) 0x00700000,(mm_page *) old_cr3, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);

	//copio la tarea desde donde esta a la pagina que acabo de mapear.
	uint_8* addr_to_copy = (uint_8*) 0x00700000;
	uint_8* task_to_copy = (uint_8*) f;
	uint_32 cant_to_copy = f->mem_end_disk - f->mem_start;
	int i;

	for (i = 0; i < cant_to_copy; i++) {
		*addr_to_copy++ = *task_to_copy++;
	}

	//tengo que armar la estreuctura
	uint_32 requested_pid = get_new_pid();
	task_table[requested_pid].cr3 = (uint_32) task_dir;
	task_table[requested_pid].esp0 = 0xFFFFFFD8;

	mm_page_free(0x00700000,(mm_page *) old_cr3);
	mm_page_free(0x55555000,(mm_page *) old_cr3);

	tlbflush();

	sched_load(requested_pid);

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
	}
}

void loader_exit(void) {
	mm_dir_free((mm_page*) task_table[cur_pid].cr3);
	loader_switchto(sched_exit());
	free_pid(cur_pid);
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

/*
uint_32 sys_fork(void){
  //me guardo el cr3 viejo.
  uint_32 old_cr3 = rcr3();

  //pido un directorio para la nueva tarea
  void* new_cr3 = mm_dir_new();
  //cargo el cr3 con el nuevo directorio.
  // lcr3((uint_32) task_dir);

  // Copio el CR3
  for(i=0; i < 1024; i++){
    *new_cr3++ = *aux_old_cr3 +i;
  }

  mm_page_copy((uint_32) f->mem_start, new_cr3, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);


  void* puntero_page_tarea = mm_mem_alloc();

  //ver esto donde van mapeados los stacks
  // mm_page_map(0x00401000, task_dir, (uint_32) task_stack3, 0, USR_STD_ATTR);
  // mm_page_map(0xFFFFF000, task_dir, (uint_32) task_stack0, 0, MM_ATTR_RW | MM_ATTR_US_S);

  //mapeo la direccion virtual 0x00400000 en la pagina que recien se me asigno.
  mm_page_map((uint_32) f->mem_start, task_dir, (uint_32) puntero_page_tarea, 0, USR_STD_ATTR);

  //copio la tarea desde donde esta a la pagina que acabo de mapear.
  // uint_8* addr_to_copy = (uint_8*) f->mem_start;
  // uint_8* task_to_copy = (uint_8*) f;
  // uint_32 cant_to_copy = f->mem_end_disk - f->mem_start;
  // int i;
  // for (i = 0; i < cant_to_copy; i++) {
    // *addr_to_copy++ = *task_to_copy++;
  // }

  //tengo que armar la estreuctura de proceso
  uint_32 requested_pid = get_new_pid();
  task_table[requested_pid].cr3 = (uint_32) new_cr3;
  task_table[requested_pid].esp0 = 0xFFFFFFD8;

  sched_load(requested_pid);

  //vuelvo al directorio de la tarea actual
  lcr3((uint_32) old_cr3);

  // esto esta mal.. tiene q decidir q numero devolver creo q necesitamos un semaforo!
  return requested_pid;
}
*/
void sys_exit(void) {
  loader_exit();
}


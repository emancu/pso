#include <loader.h>
#include <vga.h>
#include <mm.h>
#include <sched.h>


task task_table[MAX_PID];

/* pid "actual" */
uint_32 cur_pid = 0;

void loader_init(void) {
    int i;
    task_tss* ptr_context;
    for(i = 1 ; i < MAX_PID ; i++){
        ptr_context = (task_tss*) task_table[i].context;
        ptr_context->cr3 = 0;
    }
}

uint_32 get_pid(void){
    uint_32 i;
    task_tss* ptr_context;
    for(i = 1 ; i < MAX_PID ; i++){
        ptr_context = (task_tss*) task_table[i].context;
        if(ptr_context->cr3 == 0)
            return i;

    }
    //si no hay ninguno libre
    return -1;
}

pid loader_load(pso_file* f, int pl) {
    //ver cuanta memoria nesecita
    uint_32 old_cr3 = rcr3();
    printf("old cr3: %x", old_cr3);

    //solo para testiar le cambie el signature
    if(pl == 1){
        f->signature[0] = 'A';
        f->signature[1] = 'L';
        f->signature[2] = 'E';
    }
    printf("f apunta a: %x" , f);
    printf("header: %s", f->signature);
    printf("header: %x", f->mem_start);
    printf("header: %x", f->mem_end_disk);
    printf("header: %x", f->mem_end);
    printf("header: %x", f->_main);

    //pido un directorio para la nueva tarea
    void* task_dir = mm_dir_new();
    //cargo el cr3 con el nuevo directorio. TENGO QUE GUARDAR EL VIEJO!!
    lcr3((uint_32) task_dir);
    //pido una pagina de 4k para la tarea (hay que sacar cuanto ocupa)
    void* puntero_page_tarea = mm_mem_alloc();

    void* task_stack3 = mm_mem_alloc();
    void* task_stack0 = mm_mem_alloc();

    //ver esto donde van mapeados los stacks
    mm_page_map(0x00401000, task_dir, (uint_32)task_stack3, 0, USR_STD_ATTR);
    mm_page_map(0x00402000, task_dir, (uint_32)task_stack0, 0, 0x2);

    //mapeo la direccion virtual 0x00400000 en la pagina que recien se me asigno.
    mm_page_map(0x00400000, task_dir, (uint_32)puntero_page_tarea, 0, USR_STD_ATTR);

    //copio la tarea desde donde esta a la pagina que acabo de mapear.
    uint_8* addr_to_copy = (uint_8*) 0x00400000;
    uint_8* task_to_copy = (uint_8*) f;
    uint_32 cant_to_copy = f->mem_end_disk - f->mem_start;
    int i;
    for(i = 0 ; i < cant_to_copy ; i++){
        *addr_to_copy++ = *task_to_copy++;
    }


    //tengo que armar la estreuctura
    uint_32 requested_pid = get_pid();
    printf("pid: %d" , requested_pid);
    task_tss* ptr_context =(task_tss*) task_table[i].context;
    ptr_context->cr3 = (uint_32) task_dir;
    //lo apunto al final de la pila
    ptr_context->esp = 0x00402000;
    ptr_context->esp0 = 0x00403000;

    ptr_context->ss0 = rss();
    ptr_context->eip = (uint_32) f->_main;

    sched_load(requested_pid);

    //vuelvo al directorio de la tarea actual
    lcr3((uint_32) old_cr3);

	return 0;

}


void loader_tick(){
    int new_current_pid = sched_tick();
    if(new_current_pid != cur_pid){}
        loader_switchto(new_current_pid);
}

void loader_enqueue(int* cola) {
}

void loader_unqueue(int* cola) {
}

void loader_exit(void) {
}

void loader_switchto(pid pd) {
}

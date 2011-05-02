#include <loader.h>
#include <vga.h>


task task_table[MAX_PID];

/* pid "actual" */
uint_32 cur_pid = 0;

void loader_init(void) {

}

pid loader_load(pso_file* f, int pl) {
    //printf("header: %s", *f);
    printf("header: %s", f->signature);
	return 0;
}

void loader_enqueue(int* cola) {
}

void loader_unqueue(int* cola) {
}

void loader_exit(void) {
}

void loader_switchto(pid pd) {
}

#include <pic.h>
#include <idt.h>
#include <vga.h>
#include <mm.h>
#include <sched.h>
#include <gdt.h>
#include <debug.h>
#include <loader.h>
#include <syscalls.h>
#include <i386.h>
#include <device.h>
#include <con.h>
#include <proc.h>
#include <fs.h>
#include <fdd.h>
#include <pit.h>
#include <serial.h>
#include <fat12.h>

#include "klib_machine.h"
#include "scheduler_test.c"

extern void* _end;
extern pso_file task_task1_pso;
extern pso_file task_task2_pso;

/* Entry-point del modo protegido luego de cargar los registros de
 * segmento y armar un stack */
void kernel_init(void) {
	gdt_init();
	clear_screen();

	// breakpoint();
	//init all modules
	idt_init();
	debug_init();
	pit_init();
	mm_init();
	loader_init();
	device_init();
	con_init();
	// serial_init();
	sti();
	fdd_init();
	fat12_init();
	fs_init();

	//load tasks
	//	loader_load(&task_task1_pso, 0);
	chardev_file* ale = (chardev_file*) fs_open("/disk/TASK2.PSO", 0x3);
	char * dir = ((char *) ale) + 0x200;
	loader_load((pso_file *) dir, 0);

	//	int i = 0;
	//	int j;
	//	for (i = 0; i < 18000000; i++) {
	//		j++;
	//	}
	//	loader_load(&task_task2_pso, 0);

	return;
}

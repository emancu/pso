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

#include "klib_machine.h"
#include "scheduler_test.c"

extern void* _end;
extern pso_file task_task1_pso;
extern pso_file task_task2_pso;
extern char* kernel_sym;

/* Entry-point del modo protegido luego de cargar los registros de
 * segmento y armar un stack */
void kernel_init(void) {
  int i;
  gdt_init();
  clear_screen();

  char* kernel_sym_r = (char*)&kernel_sym;
  printf("Kernel_sym: %x\n", &kernel_sym);
  for (i = 0; i < 100; i++)
    vga_printf(3+(i/vga_cols), i%vga_cols, "%c", VGA_BC_BLACK | VGA_FC_WHITE, kernel_sym_r[i]);

  breakpoint();
  //init all modules
  idt_init();
  debug_init();
  mm_init();
  loader_init();
  fs_init();
  device_init();
  con_init();
  fdd_init();
  breakpoint();

  //load tasks
  loader_load(&task_task1_pso,0);
  loader_load(&task_task1_pso,1);


  sti();
  return;
}

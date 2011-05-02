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

#include "klib_machine.h"
#include "scheduler_test.c"

extern void* _end;
extern pso_file task_task1_pso;

/* Entry-point del modo protegido luego de cargar los registros de
 * segmento y armar un stack */
void kernel_init(void) {
  gdt_init();
  clear_screen();
  //vga_write(vga_rows-1, vga_cols/4-1, " OS by: E. Mancuso, A. Mataloni, M. Miguel ", VGA_FC_BLUE | VGA_FC_LIGHT | VGA_BC_WHITE);
  //print_machine_stat();
  idt_init();
  debug_init();
  mm_init();
  sti();


  //printf("task_init: %x" , &task_task1_pso);
  loader_load(&task_task1_pso,0);

  return;
}

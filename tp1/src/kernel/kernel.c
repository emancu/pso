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

extern void* _end;
/* Entry-point del modo protegido luego de cargar los registros de
 * segmento y armar un stack */
void kernel_init(void) {
  gdt_init();
  clear_screen();
  vga_write(vga_rows-1, vga_cols/4-1, " OS by: E. Marcusso, A. Mataloni, M. Miguel ", VGA_FC_BLUE | VGA_FC_LIGHT | VGA_BC_WHITE);
  print_machine_stat();
  idt_init();
  sti();
  return;
}

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

#include "klib_print.h"
#include "klib_machine.h"

extern void* _end;
/* Entry-point del modo protegido luego de cargar los registros de
 * segmento y armar un stack */
void kernel_init(void) {
  gdt_init();
  clear_screen();
  printk_machine_stat();
  pic_reset(0x20,0x2A);
  pic_enable();
  idt_init();
  breakpoint();
  return;
}

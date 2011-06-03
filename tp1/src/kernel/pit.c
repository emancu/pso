#include <pit.h>
#include <isr.h>
#include <vga.h>
#include <i386.h>
#include <debug.h>  // We need in_panic
#include <loader.h> // We need cur_pid

int tick;

void pit_init() {
  tick = 0;
  idt_register(32, &isr_timerTick, 0);
}

void sleep(mils){
  // TODO: Hacer el sleep !!!
  printf("IMPLEMENTAR SLEEP !!!");
  hlt();
}

void isr_timerTick_c() {
  outb(0x20,0x20);
  char clock[] = {'\\', '-', '/', '|'};
  if (!in_panic) {
    vga_printf(vga_rows-1, vga_cols-1, "%c", VGA_BC_BLACK | VGA_FC_GREEN | VGA_FC_LIGHT ,clock[tick++%4]);
    loader_tick();
  } else {
    vga_printf(vga_rows-1, vga_cols-2, "!H", VGA_FC_BLACK | VGA_BC_RED);
  }

  outb(0x20,0x20);
}

#include <debug.h>
#include <isr.h>
#include <vga.h>
#include <mm.h>
#include <sched.h>
#include <i386.h>

const char* exp_name[] = {
  "Divide Error",
  "Debug Interrupt",
  "NMI Interrupt",
  "Breakpoint",
  "Interrupt on overflow",
  "BOUND range exceeded",
  "Invalid Opcode",
  "Device not available",
  "Double fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment not present",
  "Stack exception",
  "General protection fault",
  "Page fault",
  "Reserved",
  "Floating point exception",
  "Alignment check"
};

bool in_panic = FALSE;
void debug_kernelpanic(const uint_32* stack, const exp_state* expst) {
  /* No permite panics anidados */
  if (in_panic) while(1) hlt();
  in_panic = TRUE;
  breakpoint();
  printf("stack: %x, expst: %x", stack, expst);
  // Completar
}

int ale = 0;

void debug_init(void) {
  /* Registra todas las excepciones para sí */
  idt_register(33, &debug_kernel_panic, 0);
  idt_register(32, &timerTick, 0);
}

void isr_timer_tick() {
    //printf("Tick! %d \n", ale++);
    outb(0x20,0x20);
}

void isr_keyboard() {
    char tecla=0;
    printf("Tecladooo!!! \n");
    __asm__ __volatile__("inb $0x60, %%al" : "=a" (tecla));
    outb(0x20,0x20);

}

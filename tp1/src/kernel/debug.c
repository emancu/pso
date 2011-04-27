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

void print_expst(const exp_state* expst) {
  printf("exp_state @ %x\n", expst);
  printf(">gs: %x, fs: %x, es: %x, ds: %x, ss: %x\n", expst->gs, expst->fs, expst->es, expst->ds, expst->ss);
  printf(">edi: %x, esi: %x, ebp: %x, esp: %x\n", expst->edi, expst->esi, expst->ebp, expst->esp);
  printf(">ebx: %x, edx: %x, ecx: %x, eax: %x\n", expst->ebx, expst->edx, expst->ecx, expst->eax);
  printf(">errorcode: %x, org_eip: %x, org_cs: %x, eflags: %x, org_esp: %x, org_ss: %x\n",\
      expst->errcode, expst->org_eip, expst->org_cs, expst->eflags, expst->org_esp, expst->org_ss);
}

bool in_panic = FALSE;
void debug_kernelpanic(const uint_32* stack, const exp_state* expst) {
  /* No permite panics anidados */
  if (in_panic) while(1) hlt();
  in_panic = TRUE;
  print_expst(expst);
  // breakpoint();
  // printf("stack: %x, expst: %x", stack, expst);
  // Completar
}

int ale = 0;

void debug_init(void) {
  /* Registra todas las excepciones para sí */
  idt_register(33, &isr_keyboard, 0);
  idt_register(32, &isr_timerTick, 0);
}

void isr_timerTick_c() {
    // printf("Tick! %d \n", ale++);
    char clock[] = {'\\', '-', '/', '|'};
    if (!in_panic) {
      vga_printf(vga_rows-1, vga_cols-1, "%c", VGA_BC_BLACK | VGA_FC_GREEN | VGA_FC_LIGHT ,clock[ale++%4]);
      outb(0x20,0x20);
    } else
      vga_printf(vga_rows-1, vga_cols-2, "!H", VGA_FC_BLACK | VGA_BC_RED);
}

void isr_keyboard_c() {
    char tecla=0;
    printf("Tecladooo!!! \n");
    __asm__ __volatile__("inb $0x60, %%al" : "=a" (tecla));
    outb(0x20,0x20);
}

#include <debug.h>
#include <isr.h>
#include <vga.h>
#include <mm.h>
#include <sched.h>
#include <i386.h>
#include <loader.h>
#include <con.h>
#include <serial.h>

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
  "Alignment check",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Timer Tick",
  "Keyboard"
};

uint_32 error_num = -1;

void print_expst(const exp_state* expst) {
  printf("exp_state @ %x\n", expst);
  printf(">gs: %x, fs: %x, es: %x, ds: %x, ss: %x\n", expst->gs, expst->fs, expst->es, expst->ds, expst->ss);
  printf(">edi: %x, esi: %x, ebp: %x, esp: %x\n", expst->edi, expst->esi, expst->ebp, expst->esp);
  printf(">ebx: %x, edx: %x, ecx: %x, eax: %x\n", expst->ebx, expst->edx, expst->ecx, expst->eax);
  printf(">errorcode: %x, org_eip: %x, org_cs: %x, eflags: %x, org_esp: %x, org_ss: %x\n",\
      expst->errcode, expst->org_eip, expst->org_cs, expst->eflags, expst->org_esp, expst->org_ss);
}

void print_stack(uint_32 f, uint_32 c, uint_32 dwords, uint_32 cols, const uint_32* stack) {
  uint_32 col_count = 0;
  int i = 0, j = 0;

  vga_write(f, c, "Stack:", VGA_BC_GREEN | VGA_FC_WHITE | VGA_FC_LIGHT);
  vga_printf(f, c+6, " %x", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, (uint_32)stack);

  col_count = c;
  for (i = 0; i < dwords; i++) {
    if (i%cols == 0) {
      f++;
      col_count = 1+ c + (VGA_HEX_MAX_WIDTH+1)*cols;
      for (j = i; j < i+cols*8; j++) //Imprimo en formato caracter
        vga_printf(f, col_count++, "%c", VGA_BC_BLACK | VGA_FC_BLUE | VGA_FC_LIGHT, ((char*) stack)[i*8+j]);
      col_count = c;
    }
    vga_printf(f, col_count, "%x", VGA_FC_WHITE | VGA_BC_BLACK | VGA_FC_LIGHT, stack[i]); //Imprimo de a ints en hexa
    col_count += VGA_HEX_MAX_WIDTH+1;
  }
}

bool in_panic = FALSE;
void debug_kernelpanic(const uint_32* stack, const exp_state* expst) {
  /* No permite panics anidados */
  if (in_panic) while(1) hlt();
  in_panic = TRUE;
  // print_expst(expst);
  // Completar
  clear_screen();
  //Imprimo el cartel "Kernel Panic!"
  vga_write(0,0, "Kernel Panic!", VGA_FC_WHITE | VGA_BC_RED | VGA_FC_LIGHT);
  //Imprimo los registros generales)
  vga_printf(PANIC_GEN_ROW, PANIC_GEN_COL, "EAX %x, EBX %x, ECX %x, EDX %x, CS %x, DS %x, SS %x",\
      VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, expst->eax, expst->ebx, expst->ecx, expst->edx, expst->org_cs, expst->ds, expst->ss);
  vga_printf(PANIC_GEN_ROW+1, PANIC_GEN_COL, "ESI %x, EDI %x, ESP %x, EIP %x, FLAGS %x", VGA_FC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT,\
      expst->esi, expst->edi, expst->esp, expst->org_eip, expst->eflags);
  vga_printf(PANIC_GEN_ROW+2, PANIC_GEN_COL, "ERRCODE %x, ORG_CS %x, ORG_SS %x, ORG_ESP %x", VGA_FC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT,\
      expst->errcode, expst->org_cs, expst->org_ss, expst->org_esp);
  vga_printf(PANIC_GEN_ROW+3, PANIC_GEN_COL, "TR %x, CR2 %x, CR3 %x, PID %x", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT,\
      rtr(), rcr2(), rcr3(), 0xF);

  //Imprimo el stack
  print_stack(PANIC_STACK_ROW, PANIC_STACK_COL, 12*4, 4, stack);

  //Imprimo texto de error
  if (error_num > 0 && error_num < sizeof(exp_name)) {
    vga_printf(0, vga_cols-strlen(exp_name[error_num])-13, " Exception: %s ", VGA_BC_MAGENTA | VGA_FC_WHITE | VGA_FC_LIGHT, exp_name[error_num]);
  } else {
    vga_write(0, vga_cols-strlen(" Exception Undefined "), " Undefined Exception ", VGA_BC_MAGENTA | VGA_FC_WHITE | VGA_FC_LIGHT);
  }
  error_num = -1;
}

int tick = 0;

void debug_init(void) {
  /* Registra todas las excepciones para sí */
  idt_register(36, &isr_serial, 0);
  idt_register(33, &isr_keyboard, 0);
  idt_register(32, &isr_timerTick, 0);
  idt_register(0, &isr_0_DE, 0);
  idt_register(1, &isr_1_DB, 0);
  idt_register(2, &isr_2_NMI, 0);
  idt_register(3, &isr_3_BP, 0);
  idt_register(4, &isr_4_OF, 0);
  idt_register(5, &isr_5_BR, 0);
  idt_register(6, &isr_6_UD, 0);
  idt_register(7, &isr_7_NM, 0);
  idt_register(8, &isr_8_DF, 0);
  idt_register(9, &isr_9_CSO, 0);
  idt_register(10, &isr_A_TS, 0);
  idt_register(11, &isr_B_NP, 0);
  idt_register(12, &isr_C_SS, 0);
  idt_register(13, &isr_D_GP, 0);
  idt_register(14, &isr_E_PF, 0);
  idt_register(16, &isr_10_MF, 0);
  idt_register(17, &isr_11_AC, 0);
  idt_register(18, &isr_12_MC, 0);
  idt_register(19, &isr_13_XM, 0);
}

int i = 0;

void isr_timerTick_c() {
    //breakpoint();
    outb(0x20,0x20);
    // printf("Tick! %d \n", tick++);
    char clock[] = {'\\', '-', '/', '|'};
    if (!in_panic) {
      vga_printf(vga_rows-1, vga_cols-1, "%c", VGA_BC_BLACK | VGA_FC_GREEN | VGA_FC_LIGHT ,clock[tick++%4]);
      i++;
      i = 0;
      loader_tick();
    } else
      vga_printf(vga_rows-1, vga_cols-2, "!H", VGA_FC_BLACK | VGA_BC_RED);

    outb(0x20,0x20);

}

void isr_keyboard_c() {
    sint_16 tecla=0;
    // printf("Tecladooo!!! \n");
    __asm__ __volatile__("inb $0x60, %%al" : "=a" (tecla));
    console_keyPressed(tecla);
    //printf("tecla recibida: %x", tecla);
    outb(0x20,0x20);
}

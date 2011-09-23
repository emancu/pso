#include <debug.h>

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
  "Undefined 18",
  "Undefined 19",
  "Undefined 20",
  "Undefined 21",
  "Undefined 22",
  "Undefined 23",
  "Undefined 24",
  "Undefined 25",
  "Undefined 26",
  "Undefined 27",
  "Undefined 28",
  "Undefined 29",
  "Undefined 30",
  "Undefined 31",
  "Timer Tick",
  "Keyboard"
};

uint_32 error_num = -1;
bool in_panic = FALSE;
chardev_serial* log_chardev;

uint_32* obtain_prev_func(uint_32* ebp) {
  uint_32* old_ebp_dir = (uint_32*)(*ebp);
  uint_32* old_eip_dir = (uint_32*)(*ebp + 4);
  *ebp = *old_ebp_dir;
  return (uint_32*)*old_eip_dir;
}

char* find_function_name(uint_32 eip) {
  char* kernel_sym = (char*)&kernel_syme;
  char* kernel_sym_end = (char*)&kernel_sym_ende;
  char* closest_name = kernel_sym;
  uint_32 closest_dir = 0x7FFFFFFF, ret;
  unsigned int dir;
  //Busco el máximo número de función que no sea más grande que la pasada
  while (kernel_sym < kernel_sym_end) {
    ret = str_into_hex(kernel_sym, 0, &dir);
    // printf("dir = %x | kernel_sym = %x | closest_name = %x", dir, kernel_sym, closest_name);

    if (eip - dir < 0) continue; //Si la dirección leída del archivo es mayor que el parámetro no sirve
    // if (eip - dir < eip - closest_dir) { //Sino actualizo las variables
      // closest_dir = dir;
    // }
    if (ret < 0) break;
    kernel_sym += ret;
    //Luego avanzo más allá de los caracteres buscando un espacio o \n
    while (isDigit(*kernel_sym)) {
      kernel_sym++;
    }
    kernel_sym++;
    //Luego paso los espacios o \n que haya hasta llegar a caracteres
    while (isSpace(*kernel_sym)) {
      kernel_sym++;
    }
    if (eip - dir < eip - closest_dir) { //Sino actualizo las variables
      closest_dir = dir;
      closest_name = kernel_sym; //Actualizo el nombre de la función
    }
    //Como estos caracteres son el nombre de la función, también los salteo hasta llegar a un número
    while(!isSpace(*kernel_sym)) kernel_sym++;
    while (!isDigit(*kernel_sym)) kernel_sym++;
  }

  // printf("*ffn: %x, %x, %x  ", kernel_sym, closest_name, closest_dir);
  kernel_sym = closest_name; //NOTE: Trucho, muy trucho
  while (isCharacter(*kernel_sym) || *kernel_sym == '_') kernel_sym++;
  *kernel_sym = '\0';
  return closest_name;
}

void print_backtrace(uint_32 f, uint_32 c, uint_32 level, uint_32 params, const uint_32 ebp, const uint_32 last_eip){
  uint_32 ebp_hold = ebp;
  uint_32 i, j, func_name_len;
  char* func_name;
  uint_32 eip;
  //Imprimo el título
  vga_write(f++, c, "Backtrace:", VGA_BC_CYAN | VGA_FC_WHITE | VGA_FC_LIGHT);
  //Imprimo la primera función
  func_name = find_function_name(last_eip);
  vga_printf(f, c, " (%x) %s( ", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, last_eip, func_name);
  func_name_len = strlen(func_name);
  for (j = 0; j < params; j++) {
     vga_printf(f, c+func_name_len+10+10*j, "%x", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, (ebp_hold+8+(j*4)));
  }
  vga_write(f, c+func_name_len+10+10*j, ")", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT);

  //Imprimo el resto de las funciones
  f++;
  for (i = 0; i < level; i++) {
    eip = (uint_32)obtain_prev_func(&ebp_hold);
    func_name = find_function_name(eip);
    func_name_len = strlen(func_name);
    vga_printf(f+i, c, " (%x) %s( ", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, eip, func_name);
    for (j = 0; j < params; j++) {
      vga_printf(f+i, c+func_name_len+10+10*j, "%x", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, (ebp_hold+8+(j*4)));
    }
    vga_write(f+i, c+func_name_len+10+10*j, ")", VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT);
  }
}

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

void debug_kernelpanic(const uint_32* stack, const exp_state* expst) {
  breakpoint();
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
      rtr(), rcr2(), rcr3(), cur_pid);

  //Imprimo el stack
  print_stack(PANIC_STACK_ROW, PANIC_STACK_COL, 12*4, 4, stack);

  //Imprimo texto de error
  if (error_num > 0 && error_num < sizeof(exp_name)) {
    vga_printf(0, vga_cols-strlen(exp_name[error_num])-13, " Exception: %s ", \
        VGA_BC_MAGENTA | VGA_FC_WHITE | VGA_FC_LIGHT, exp_name[error_num]);
  } else {
    vga_printf(0, vga_cols-strlen(" Exception Undefined  ")-dec_into_string_len(error_num), \
        " Undefined Exception %d", VGA_BC_MAGENTA | VGA_FC_WHITE | VGA_FC_LIGHT, error_num);
  }
  error_num = -1;

  //Imprimo el backtrace
  print_backtrace(PANIC_BT_ROW, PANIC_BT_COL, 4, 5, expst->ebp, expst->org_eip);
}

// TODO: El que las registra es el debug ? o deberia ser otro ?!
void debug_init(void) {
  /* Registra todas las excepciones para sí */
  idt_register(33, &isr_keyboard, 0);
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

void logger_init(){
  log_chardev = (chardev_serial *) serial_open(3);
}

// Log with format into serial 3
void logf(const char* format, ...) {
  va_list argp;
  va_start(argp, format);
  uint_32 size = printf_len(format,argp);
  char buff[size + 1];
  va_start(argp, format);

  sprintf_in(buff, format, argp);

  logs(buff, size);
}

// Log an string into serial 3
void logs(const char* buff, uint_32 size){
  if (log_chardev != NULL){
    log_chardev->dev.write((chardev*)log_chardev, buff, size);
    log_chardev->dev.write((chardev*)log_chardev, "\n\r", 2);
  }
}

void isr_keyboard_c() {
  sint_16 tecla=0;
  __asm__ __volatile__("inb $0x60, %%al" : "=a" (tecla));
  console_keyPressed(tecla);
  outb(0x20,0x20);
}


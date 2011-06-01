#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <isr.h>
#include <vga.h>
#include <i386.h>

extern uint_32 error_num;
extern bool in_panic;

void debug_init(void);

void debug_kernelpanic(const uint_32* stack, const exp_state* expst);

/* Prints the stack content in the screen starting at row 'f' and column 'c', writing a total
 * of 'dwords' dwords in 'cols' columns starting from memmory position 'stack' and going
 * upwards. */
void print_stack(uint_32 f, uint_32 c, uint_32 dwords, uint_32 cols, const uint_32* stack);

/* Prints the backtrace of 'level' number of cunctions using a starting frame given by 'ebp'.
 * With each call backtraced the function it prints the first 'params' dwords in the stack of
 * that function call frame. */
void print_backtrace(uint_32 f, uint_32 c, uint_32 level, uint_32 params, const uint_32 ebp);

void isr_keyboard_c();

extern void debug_kernel_panic();
extern void isr_keyboard();
extern void isr_0_DE();
extern void isr_1_DB();
extern void isr_2_NMI();
extern void isr_3_BP();
extern void isr_4_OF();
extern void isr_5_BR();
extern void isr_6_UD();
extern void isr_7_NM();
extern void isr_8_DF();
extern void isr_9_CSO();
extern void isr_A_TS();
extern void isr_B_NP();
extern void isr_C_SS();
extern void isr_D_GP();
extern void isr_E_PF();
extern void isr_10_MF();
extern void isr_11_AC();
extern void isr_12_MC();
extern void isr_13_XM();

extern char exp_msg[];
extern uint_32 exp_num;



#ifndef NDEBUG
#define __mac_xstr(s) #s
#define __mac_str(s) __mac_xstr(s)
#define kassert(EXP) { if (!(EXP)) { cli(); vga_write(0, 0, "Assertion failed at " \
  __mac_str(__FILE__)":"__mac_str(__LINE__)": "#EXP, \
  VGA_BC_RED | VGA_FC_WHITE | VGA_FC_LIGHT); hlt(); while(1); } }
#else
#define kassert(EXP) {}
#endif

#endif

#define PANIC_GEN_COL 0
#define PANIC_GEN_ROW 2
#define PANIC_STACK_COL 0
#define PANIC_STACK_ROW 7

#ifndef __IDT_H__
#define __IDT_H__

/* Para inicializar la IDT del sistema. */
void idt_init(void);

/* Para registrar una ISR */
void idt_register(int intr, void (*isr)(void), int pl);

/* Números de interrupciones de la IDT */
#define ISR_DIVIDE     0		// divide error
#define ISR_DEBUG      1		// debug exception
#define ISR_NMI        2		// non-maskable interrupt
#define ISR_BRKPT      3		// breakpoint
#define ISR_OFLOW      4		// overflow
#define ISR_BOUND      5		// bounds check
#define ISR_ILLOP      6		// illegal opcode
#define ISR_DEVICE     7		// device not available 
#define ISR_DBLFLT     8		// double fault
#define ISR_COPRO      9		// coprocesor (WTF!!)
#define ISR_TSS       10		// invalid task switch segment
#define ISR_SEGNP     11		// segment not present
#define ISR_STACK     12		// stack exception
#define ISR_GPFLT     13		// genernal protection fault
#define ISR_PGFLT     14		// page fault
/* #define ISR_RES    15 */	// reserved
#define ISR_FPERR     16		// floating point error
#define ISR_ALIGN     17		// aligment check
#define ISR_MCHK      18		// machine check
#define ISR_SIMDERR   19		// SIMD floating point error

/* Número de interrupciones para las PIC */
#define ISR_IRQ0    0x20		// Timer tick
#define ISR_IRQ1    0x21		// Teclado
#define ISR_IRQ2    0x22		// Cascada de la IRQ9
#define ISR_IRQ3    0x23		// Default COM2 y COM4
#define ISR_IRQ4    0x24		// Default COM1 y COM3
#define ISR_IRQ5    0x25		// LPT2
#define ISR_IRQ6    0x26		// Floppy Drive Controller
#define ISR_IRQ7    0x27		// LPT1

#define ISR_IRQ8    0x28		// Real Time Clock
#define ISR_IRQ9    0x29		// 
#define ISR_IRQ10   0x2A		// Reserved
#define ISR_IRQ11   0x2B		// Reserved
#define ISR_IRQ12   0x2C		// PS/2 mouse
#define ISR_IRQ13   0x2D		// Math coprocessor
#define ISR_IRQ14   0x2E		// Primary IDE
#define ISR_IRQ15   0x2F		// Secondary IDE


/* Struct de descriptor de IDT */
typedef struct str_idt_descriptor {
	unsigned short idt_length;
	unsigned int idt_addr;
} __attribute__((__packed__)) idt_descriptor;

/* Struct de una entrada de la IDT */
typedef union str_idt_entry {
	struct str_idt_entry_vl {
		unsigned int vl0, vl1;
	} vl;
	struct str_idt_entry_fld {
		unsigned short offset_0;
		unsigned short segsel;
		unsigned short attr;
		unsigned short offset_1;
	} fld;
} __attribute__((__packed__, aligned (8))) idt_entry;

extern idt_entry idt[];
extern idt_descriptor IDT_DESC;

/** Constantes de los atributos **/
/* Masks */
#define IDT_ATTR_P    0x8000
#define IDT_ATTR_DPL  0x6000
#define IDT_ATTR_DPL0 0x0000
#define IDT_ATTR_DPL1 0x2000
#define IDT_ATTR_DPL2 0x4000
#define IDT_ATTR_DPL3 0x6000
#define IDT_ATTR_S    0x1000

#define IDT_ATTR_D    0x0800
#define IDT_ATTR_TYPE 0x0700


/* S Field */
#define IDT_ATTR_S_ON  0x0000
#define IDT_ATTR_S_OFF 0x1000

/* D Field */
#define IDT_ATTR_D_16  0x0000
#define IDT_ATTR_D_32  0x0800

/* TYPE Field */
#define IDT_ATTR_TYPE_INT 0x0600
#define IDT_ATTR_TYPE_EXP 0x0700

#endif

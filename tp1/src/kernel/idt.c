#include <tipos.h>
#include <i386.h>
#include <gdt.h>
#include <isr.h>
#include <idt.h>

static const uint_32 IDT_ATTR_DPL_[4] = { IDT_ATTR_DPL0, IDT_ATTR_DPL1, IDT_ATTR_DPL2, IDT_ATTR_DPL3 };

/* Macro para crear una entrada de la IDT dando offset(32), selector(16) y attr(16). */
#define make_idt_entry(offset, select, attr) \
	(idt_entry){{((uint_32)(offset) & 0xFFFF) | ((uint_32)(select) << 16), \
	((uint_32)(attr) & 0xFFFF) | ((uint_32)(offset) & 0xFFFF0000) }}

#define idt_entry_null make_idt_entry(0,0,0)

#define IDT_INT IDT_ATTR_P | IDT_ATTR_S_ON | IDT_ATTR_D_32 | IDT_ATTR_TYPE_INT
#define IDT_EXP IDT_ATTR_P | IDT_ATTR_S_ON | IDT_ATTR_D_32 | IDT_ATTR_TYPE_EXP

idt_entry idt[128] = {};

idt_descriptor IDT_DESC = {sizeof(idt)-1, (uint_32)&idt};

void idt_init(void) {
	//Rellenar la IDT
	// Carga el IDTR
	return;
}

void idt_register(int intr, void (*isr)(void), int pl ) {
	//Agregar la intr a la IDT
}

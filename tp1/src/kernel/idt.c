#include <tipos.h>
#include <i386.h>
#include <gdt.h>
#include <isr.h>
#include <idt.h>
#include <pic.h>
#include <klib_print.h>

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

int ale = 0;

void idt_init(void) {
    //Rellenar la IDT
    int i;
    for(i = 0; i < 128; i++){
        idt[i] = idt_entry_null;
    }
    // Carga el IDTR
    lidt(&IDT_DESC);

    pic_reset(0x20,0x28);
    pic_enable();

    for(i = 0; i < 128; i++){
        idt_register(32, &timerTick, 0);
    }
            idt_register(33, &keyboard, 0);

    //    idt_register(33, &isr_keyboard, 0);
	return;
}

void idt_register(int intr, void (*isr)(void), int pl ) {
    int dpl = (pl == 3)? IDT_ATTR_DPL3 : IDT_ATTR_DPL0;
    int sel = (pl == 3)? (3 << 3) : (1 << 3);
    idt[intr] = make_idt_entry(isr, sel, IDT_INT | dpl);
}

void isr_timer_tick() {
    printk("Tick! %d \n", ale++);
    outb(0x20,0x20);
}

void isr_keyboard() {
    char tecla=0;
    printk("Tecladooo!!! \n");
    __asm__ __volatile__("inb $0x60, %%al" : "=a" (tecla));
    outb(0x20,0x20);

}




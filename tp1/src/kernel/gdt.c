#include <tipos.h>
#include <gdt.h>
#include <tss.h>
#include <i386.h>


/* Macro para crear una entrada de la GDT dando base(32), limit(20) y attr(12). */
#define make_gdt_entry(base, limit, attr) \
  (gdt_entry){{(uint_32)(limit) & 0xFFFF, (uint_32)(base) & 0xFFFF, \
  ((uint_32)(base) >> 16) & 0xFF, (uint_16)(attr) & 0xF, \
  ((uint_16)(attr) >> 4) & 0x1, ((uint_16)(attr) >> 5) & 0x3, ((uint_16)(attr) >> 7) & 0x1, \
  ((uint_32)(limit) >> 16) & 0xF, ((uint_16)(attr) >> 8) & 0x1, \
  ((uint_16)(attr) >> 9) & 0x1, ((uint_16)(attr) >> 10) & 0x1, ((uint_16)(attr) >> 11) & 0x1, \
  ((uint_32)(base) >> 24) & 0xFF }}

#define GDT_ATTR_SEG GDT_ATTR_G | GDT_ATTR_S_OFF | GDT_ATTR_DB | GDT_ATTR_P

// Segmento de código es E=1, no conforming (C=0) y leíble (R=1), (no accedido A=0)
#define GDT_ATTR_SEG_CODE GDT_ATTR_E | GDT_ATTR_R

// Segmento de datos es E=0, expansion direction = 0, Writable (W=1), (no accedido A=0)
#define GDT_ATTR_SEG_DATA GDT_ATTR_W

gdt_entry gdt[GDT_COUNT] = {
  make_gdt_entry(0, 0, 0),
  make_gdt_entry(0, 0xFFFFF, GDT_ATTR_SEG | GDT_ATTR_SEG_CODE | GDT_ATTR_DPL0), // SEG_CODE_0
  make_gdt_entry(0, 0xFFFFF, GDT_ATTR_SEG | GDT_ATTR_SEG_DATA | GDT_ATTR_DPL0), // SEG_DATA_0
  make_gdt_entry(0, 0xFFFFF, GDT_ATTR_SEG | GDT_ATTR_SEG_CODE | GDT_ATTR_DPL3), // SEG_CODE_3
  make_gdt_entry(0, 0xFFFFF, GDT_ATTR_SEG | GDT_ATTR_SEG_DATA | GDT_ATTR_DPL3), // SEG_DATA_3
  // El resto no está inicializado ;)
};

gdt_descriptor GDT_DESC = {sizeof(gdt)-1, (uint_32)&gdt};

void gdt_init(void) {
  tss_entry.esp0 = 0xFFFFF000;
  tss_entry.ss0 = 0x18;
  gdt[5] = make_gdt_entry(&tss_entry, sizeof(tss), GDT_ATTR_P | GDT_ATTR_DPL0 | GDT_ATTR_TYPE_TSS); // TSS

  //todo ALEMATA hay que cargar esto
  ltr(0x28);
}

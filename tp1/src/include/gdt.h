#ifndef __GDT_H__
#define __GDT_H__

#include <tipos.h>

typedef struct str_gdt_descriptor {
  unsigned short gdt_length;
  unsigned int gdt_addr;
} __attribute__((__packed__)) gdt_descriptor;


typedef union str_gdt_entry {
  struct str_gdt_entry_fld {
    unsigned short limit_0;
    unsigned short base_01;
    unsigned char base_2;
    unsigned char type:4;
    unsigned char s:1;
    unsigned char dpl:2;
    unsigned char p:1;
    unsigned char limit_1:4;
    unsigned char avl:1;
    unsigned char l:1;
    unsigned char db:1;
    unsigned char g:1;
    unsigned char base_3;
  } fld;
  struct str_gdt_entry_vl {
    unsigned int vl0, vl1;
  } vl;
} __attribute__((__packed__, aligned (8))) gdt_entry;

/** Tabla GDT **/
extern gdt_entry gdt[];
#define GDT_COUNT 8

void gdt_init(void);

/** Constantes de los atributos **/

/* Masks */
#define GDT_ATTR_A    0x001
#define GDT_ATTR_W    0x002
#define GDT_ATTR_ED   0x004
#define GDT_ATTR_R    0x002
#define GDT_ATTR_C    0x004
#define GDT_ATTR_E    0x008
#define GDT_ATTR_S    0x010
#define GDT_ATTR_DPL  0x060
#define GDT_ATTR_P    0x080
#define GDT_ATTR_AVL  0x100
#define GDT_ATTR_DB   0x400
#define GDT_ATTR_G    0x800

/* DPL Field */
#define GDT_ATTR_DPL0 0x000
#define GDT_ATTR_DPL1 0x020
#define GDT_ATTR_DPL2 0x040
#define GDT_ATTR_DPL3 0x060

/* S Field */
#define GDT_ATTR_S_ON  0x000
#define GDT_ATTR_S_OFF 0x010

/* TYPE Field */
#define GDT_ATTR_TYPE_TSS 0x009


#endif

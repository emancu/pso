#ifndef __VGA_H__
#define __VGA_H__

#include <tipos.h>

extern uint_8* vga_addr;
extern const uint_16 vga_cols;
extern const uint_16 vga_rows;

void vga_init(void);

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr);
void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) __attribute__ ((format (printf, 3, 5)));

/* Paleta de 16 colores */
#define VGA_FC_BLACK   0x00
#define VGA_FC_BLUE    0x01
#define VGA_FC_GREEN   0x02
#define VGA_FC_RED     0x03
#define VGA_FC_CYAN    0x04
#define VGA_FC_MAGENTA 0x05
#define VGA_FC_BROWN   0x06
#define VGA_FC_WHITE   0x07

#define VGA_FC_LIGHT   0x08
#define VGA_FC_BLINK   0x80

#define VGA_BC_BLACK   0x00
#define VGA_BC_BLUE    0x10
#define VGA_BC_GREEN   0x20
#define VGA_BC_CYAN    0x30
#define VGA_BC_RED     0x40
#define VGA_BC_MAGENTA 0x50
#define VGA_BC_BROWN   0x60
#define VGA_BC_WHITE   0x70

#endif


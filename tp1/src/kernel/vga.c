#include <i386.h>
#include <tipos.h>
#include "vga.h"

uint_16 vga_port = 0x3D0;

uint_8* vga_addr = (uint_8*)0xB8000;
const uint_16 vga_cols = 80;
const uint_16 vga_rows = 25;

void vga_init(void) {
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
}

void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) {
}

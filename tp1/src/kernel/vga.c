#include <i386.h>
#include <tipos.h>
#include "vga.h"

uint_16 vga_port = 0x3D0;

uint_8* vga_addr = (uint_8*)0xB8000;
const uint_16 vga_cols = 80;
const uint_16 vga_rows = 25;
uint_8* vga_limit = (uint_8*) 0xB8FA0;

void vga_init(void) {
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  const char* str = msg;
  while(*str != '\0' && video < vga_addr) {
    if (*str == '\n') { //Avanzo una línea el puntero
		video = (uint_8*)(video + (vga_cols * 2 - (video - vga_addr)));
    } else { //Escribo en pantalla
      *video++ = *str;
      *video++ = attr;
    }
    str++;
  }
}

void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) {
  va_list argp;
  int amount = 8;
  va_start(argp, attr);
  while(printf_resolver(f, c, amount, attr, argp) < 0) {
    va_start(argp, attr);
    amount *= 2;
  }
}

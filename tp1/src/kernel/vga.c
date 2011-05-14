#ifndef __VGA_C__
#define __VGA_C__

#include <vga.h>

uint_16 vga_port = 0x3D0;

uint_8* vga_addr = (uint_8*)0xB8000;
const uint_16 vga_cols = 80;
const uint_16 vga_rows = 25;
uint_8* vga_limit = (uint_8*) 0xB8FA0;

uint_16 fila = 0;

void vga_init(void) {
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    if (msg[str] == '\n') { //Avanzo una línea el puntero
    video = (uint_8*)(video + (vga_cols * 2));
    } else { //Escribo en pantalla
      *video++ = msg[str];
      *video++ = attr;
    }
    str++;
  }
}

int printf_resolver_dinamic(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
	char buff[amount];
	return printf_resolver(f, c, attr, amount, (char*)buff, fmt, argp);
}

void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) {
  va_list argp;
  int amount = 256;
  va_start(argp, attr);
  while(printf_resolver_dinamic(f, c, attr, amount, format, argp) < 0) {
    va_start(argp, attr);
    amount *= 2;
  }
}

void move_scr_up() {
  uint_8* video = vga_addr;
  while (video < (vga_limit - (vga_cols*2))) {
    *video = *(video+(vga_cols*2));
    video++;
  }
  char bkg = *(video-1);
  while (video < vga_addr + vga_rows * vga_cols * 2) {
    *video++ = 0x00;
    *video++ = bkg;
  }
}

void printf(const char* fmt, ...) {
	va_list argp;
	int amount = 256;
	va_start(argp, fmt);
	if (fila == vga_rows) {
    fila--;
		move_scr_up();
  }

	while(printf_resolver_dinamic(fila, 0, VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, amount, fmt, argp) < 0) {
		va_start(argp, fmt);
		amount *= 2;
	}

	if (fila < vga_rows) fila++;
}



void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    *video++ = 0x00;
    *video++ = fmt;
  }
}

void fill_screen_with_memory(uint_8* initial_position) {
  uint_8* video = vga_addr;
  uint_8* memory_video = initial_position;
  while(video < vga_addr + vga_cols*2*vga_rows) {
    *video++ = *memory_video++;
    *video++ = *memory_video++;
  }
}

void copy_screen_to_memory(uint_8* initial_position) {
    uint_8* video = vga_addr;
  uint_8* memory_video = initial_position;
  while(video < vga_addr + vga_cols*2*vga_rows) {
    *memory_video++ = *video++;
    *memory_video++ = *video++;
  }
}



void clear_screen() {
  fill_screen(0,0);
}


int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, char* buff, const char* fmt, va_list argp) {
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
        case 'c':
          ch = (char)va_arg(argp, int);
          if (char_into_string(buff, &count, amount, ch) < 0)
            return -1;
          arg++;
          break;
        case 'd':
        case 'i':
          x = va_arg(argp, int);
          if (dec_into_string(buff, &count, amount, x) < 0)
            return -1;
          arg++;
          break;
        case 'x':
          x = va_arg(argp, int);
          if (hex_into_string(buff, &count, amount, x) < 0)
            return -1;
          arg++;
          break;

          break;
        case 's':
          str = va_arg(argp, char*);
          if (str_into_string(buff, &count, amount, str) < 0)
            return -1;
          arg++;
          break;
      }
      i++;
    } else { //No special action is taken, char is copied
      if (count >= amount) return -1;
      buff[count] = fmt[i];
      i++;
      count++;
    }
  }
  buff[count] = '\0';
  vga_write(f, c, buff, attr);
  va_end(argp);
  return 0;
}

uint_8 make_format(char blink, char front, char back, char bright) {
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
  fmt |= ((back & 0x7) << 4);
  fmt |= ((blink > 0) << 7);
  return fmt;
}

#endif

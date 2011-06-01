#ifndef __VGA_C__
#define __VGA_C__

#include <vga.h>

uint_16 vga_port = 0x3D0;

uint_8* vga_addr = (uint_8*) 0xB8000;
const uint_16 vga_cols = 80;
const uint_16 vga_rows = 25;
uint_8* vga_limit = (uint_8*) 0xB8FA0;

uint_16 fila = 0;

void vga_init(void) {
}

//!todo video es siempre donde empieza la memoria. dps se hace la cuenta con fila columna
void vga_write_in_memory(char* video, uint_8* fila, uint_8* col, const char* msg, uint_8 attr, uint_8 cant) {
	video = (char*) (video + vga_cols * 2 * *fila + *col * 2);
	int str = 0;
	char* limit = (char*) (video + 4000);
	while (((cant == 0 && msg[str] != '\0') || (cant != 0 && str < cant)) && video < limit) {
		if (msg[str] == '\n') { //Avanzo una línea el puntero
			(*fila)++;
			(*col) = 0;
			video = video + *col * 2 * *fila;
		} else if (msg[str] == 0x08) { //Backspace
			if (*col == 0) {
				*col = vga_cols - 1;
				(*fila)--;
			} else {
				(*col)--;
			}

			*(video - 2) = ' ';
			video = video + *col * 2 * *fila;
		} else { //Escribo en pantalla
			*video++ = msg[str];
			*video++ = attr;

			if (*col + 1 == vga_cols)
				(*fila)++;
			*col = (*col + 1) % vga_cols;
		}
		str++;
	}
}


void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
	uint_8 fila = f, col = c;
	vga_write_in_memory((char*) vga_addr, &fila, &col, msg, attr, 0);
}

void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) {
	va_list argp;
	va_start(argp, attr);
	int amount = printf_len(format, argp);
	char buff[amount + 1];
	va_start(argp, attr);
	if (sprintf_in(buff, format, argp) < 0) {
	}
	//Manejar error
	//Ahora no tiene razón de fallar
	vga_write(f, c, buff, attr);
}


void move_scr_up(uint_8* video_addr) {
	uint_8* video = video_addr;
	uint_8* screen_limit = (uint_8*) (video_addr + 4000);
	while (video < (screen_limit - (vga_cols * 2))) {
		*video = *(video + (vga_cols * 2));
		video++;
	}
	char bkg = *(video - 1);
	while (video < video_addr + vga_rows * vga_cols * 2) {
		*video++ = 0x00;
		*video++ = bkg;
	}
}

void printf(const char* fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	int amount = printf_len(fmt, argp);
	char buff[amount + 1];
	va_start(argp, fmt);

	if (fila == vga_rows) {
		fila--;
		move_scr_up(vga_addr);
	}

	if (sprintf_in(buff, fmt, argp) < 0) {
	}
	vga_write(fila, 0, buff, VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT);

	if (fila < vga_rows)
		fila++;
}

void fill_screen(char color, char bright) {
	uint_8* video = vga_addr;
	char fmt = make_format(0x0, 0x0, color, bright);
	while (video < vga_addr + vga_cols * 2 * vga_rows) {
		*video++ = 0x00;
		*video++ = fmt;
	}
}

void copy_memory_to_screen(uint_8* initial_position) {
	uint_8* video = vga_addr;
	uint_8* memory_video = initial_position;
	while (video < vga_addr + vga_cols * 2 * vga_rows) {
		*video++ = *memory_video++;
		*video++ = *memory_video++;
	}
}

void copy_screen_to_memory(uint_8* initial_position) {
	uint_8* video = vga_addr;
	uint_8* memory_video = initial_position;
	while (video < vga_addr + vga_cols * 2 * vga_rows) {
		*memory_video++ = *video++;
		*memory_video++ = *video++;
	}
}

void clear_screen() {
	fill_screen(0, 0);
}

uint_8 make_format(char blink, char front, char back, char bright) {
	uint_8 fmt = (uint_8) 0;
	fmt |= ((front & 0x7) << 0);
	fmt |= ((bright > 0) << 3);
	fmt |= ((back & 0x7) << 4);
	fmt |= ((blink > 0) << 7);
	return fmt;
}

#endif

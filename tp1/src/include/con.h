#ifndef __CON_H__
#define __CON_H__

#include <tipos.h>
#include <device.h>
#include <sem.h>
#include <vga.h>
#include <mm.h>

#define CON_BUFF_SIZE 30
#define CON_ERROR_READTOOLARGE -5
#define CON_STYLE VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT

typedef struct str_chardev_console {
	chardev dev;
	uint_16 fila;
	uint_16 columna;
	//Buffer con las teclas presionadas mientras la consola estaba activa
	char buff[CON_BUFF_SIZE];
	uint_8 buff_index_start; //Última posición sin leer
	uint_8 buff_index_end; //Próxima posición de buffer vacía
	uint_8 buff_cant; //Cantidad de entradas para leer
	uint_8 busy;
	uint_8 read_expected;
	sem_t sem;
	char console_screen[4000];
} __attribute__((packed)) chardev_console;



void con_init();

chardev* con_open(void);

/*
 * Keyboard useful functions
 */
uint_8 getc(uint_16 scan_code);

void console_keyPressed(sint_16 tecla);

void move_to_right_console();

void move_to_left_console();


#endif

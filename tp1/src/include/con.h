#ifndef __CON_H__
#define __CON_H__

#include <tipos.h>
#include <device.h>

#define BUFF_SIZE 30


typedef struct str_chardev_console {
	chardev dev;
	uint_16 fila;
	uint_16 columna;
	char buff[BUFF_SIZE];
	uint_8 buff_index;
	uint_16 busy;
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

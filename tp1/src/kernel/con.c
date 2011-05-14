#include <con.h>

chardev_console* current_console;

sint_32 con_read(chardev* this, void* buf, uint_32 size) {
	printf("Read a este chardev: %x, con este size: %d", this, size);
	int i;
	// Verificamos que no se esté tratando de leer más de lo que permite el buffer
	if (size > CON_BUFF_SIZE)
		return CON_ERROR_READTOOLARGE;
	size = (size > CON_BUFF_SIZE) ? CON_BUFF_SIZE : size;

	chardev_console* this_chardev_console = (chardev_console *) this;
	char* char_buf = (char*) buf;
	//Chequeamos si hay suficientes cosas para leer en el buffer
	if (this_chardev_console->buff_cant < size) {
		//No hay suficiente para leer, lo bloqueamos en su semáforo
		this_chardev_console->busy = 1;
		this_chardev_console->read_expected = size;
		loader_enqueue((sint_32*) &(this_chardev_console->sem.q));
	} //Si me desbloquean entonces hay para leer.
	for (i = 0; i < size; i++) {
		char_buf[i] = this_chardev_console->buff[this_chardev_console->buff_index_start];
		this_chardev_console->buff_index_start = (this_chardev_console->buff_index_start + 1) % CON_BUFF_SIZE;
	}
	this_chardev_console->buff_cant -= size;
	this_chardev_console->read_expected = 0;
	return 0;
}

sint_32 con_write(chardev* this, const void* buf, uint_32 size) {
	//!TODO: Chequear no pasarse de la pantalla y el uso de \n.
	chardev_console* this_chardev_console = (chardev_console *) this;
	char* char_buf = (char*)buf;

	uint_8* video = (uint_8*) (this_chardev_console->console_screen + vga_cols * 2 * this_chardev_console->fila + this_chardev_console->columna * 2);
	int str = 0;
	while (str < size && video < (uint_8*) (this_chardev_console->console_screen + 4000)) {
		if (char_buf[str] == '\n') { //Avanzo una línea el puntero
			this_chardev_console->fila++;
			this_chardev_console->columna = 0;
			video = (uint_8*) (this_chardev_console->console_screen + vga_cols * 2 * this_chardev_console->fila + this_chardev_console->columna * 2);
		} else { //Escribo en pantalla
			*video++ = char_buf[str];
			*video++ = CON_STYLE;
			if (this_chardev_console->columna + 1 == vga_cols)
				this_chardev_console->fila++;
			this_chardev_console->columna = (this_chardev_console->columna+1) % vga_cols;
		}
		str++;
	}
	if (current_console == this_chardev_console)
		fill_screen_with_memory((uint_8*)current_console->console_screen);
	return str;
}

uint_32 con_flush(chardev* this) {
	return 0;
}

chardev* con_open(void) {
	//pido una pagina nueva del kernel para la consola
	chardev_console* new_chardev_console = (chardev_console *) mm_mem_kalloc();
	new_chardev_console->fila = 0;
	new_chardev_console->columna = 0;
	new_chardev_console->buff_index_start = 0;
	new_chardev_console->buff_index_end = 0;
	new_chardev_console->buff_cant = 0;
	new_chardev_console->sem = SEM_NEW(1);
	new_chardev_console->dev.read = &con_read;
	new_chardev_console->dev.write = &con_write;
	//init structure;
	current_console = new_chardev_console;
	fill_screen_with_memory((uint_8*)current_console->console_screen);
	return (chardev*) new_chardev_console;
	//!TODO: Manejar errores
	return NULL;
}

void move_to_right_console() {
	//set curren
}

void move_to_left_console() {
	//set current_console
}

void con_init() {
}

/*
 * Keyboard useful function
 */
int altPressed = 0;
int shiftPressed = 0;

void console_keyPressed(sint_16 tecla) {
	if (altPressed == 1 && shiftPressed == 1) {
		if (tecla == 0x4d) {
			move_to_right_console();
		} else if (tecla == 0x4b) {
			move_to_left_console();
		}
	}

	//simepre se escribe en la consola actual
	char incoming_char = getc(tecla);
	if (0 != getc(tecla) && current_console->buff_cant < CON_BUFF_SIZE) {
		uint_8 index = current_console->buff_index_end;
		current_console->buff[index] = incoming_char;
		current_console->buff_index_end = (++index) % CON_BUFF_SIZE;
		current_console->buff_cant++;

		printf("Recibi tecla");

		if (current_console->buff_cant >= current_console->read_expected) {
			//despertar a la tarea que estaba esperando.
			printf("Despierto a la tarea que estaba esperando.");
			current_console->busy = 0;
			loader_unqueue((sint_32*) &(current_console->sem.q));
		}
	}
}

uint_8 getc(uint_16 scan_code) {
	switch (scan_code) {
	//case 0x2a: screen_write("SHIFT", BLACK, WHITE);
	//return 0;
	case 0x1e:
		return 'a';
	case 0x30:
		return 'b';
	case 0x2e:
		return 'c';
	case 0x20:
		return 'd';
	case 0x12:
		return 'e';
	case 0x21:
		return 'f';
	case 0x22:
		return 'g';
	case 0x23:
		return 'h';
	case 0x17:
		return 'i';
	case 0x24:
		return 'j';
	case 0x25:
		return 'k';
	case 0x26:
		return 'l';
	case 0x32:
		return 'm';
	case 0x31:
		return 'n';
	case 0x18:
		return 'o';
	case 0x19:
		return 'p';
	case 0x10:
		return 'q';
	case 0x13:
		return 'r';
	case 0x1f:
		return 's';
	case 0x14:
		return 't';
	case 0x16:
		return 'u';
	case 0x2f:
		return 'v';
	case 0x11:
		return 'w';
	case 0x2d:
		return 'x';
	case 0x15:
		return 'y';
	case 0x2c:
		return 'z';
	case 0x39:
		return ' ';
		//Enter
	case 0x1c:
		return '\n';
		//tab
	case 0x0f:
		return '\t';
		//Backspace
	case 0x0e:
		return 0x08;
		//F1
	case 0x3b:
		return '_';
		//F2
	case 0x3c:
		return '_';
		//F3
	case 0x3d:
		return '_';
		//F4
	case 0x3e:
		return '_';
		//F5
	case 0x3f:
		return '_';
		//F6
	case 0x40:
		return '_';
		//F7
	case 0x41:
		return '_';
		//F8
	case 0x42:
		return '_';
		//F9
	case 0x43:
		return '_';
		//F10
	case 0x44:
		return '_';
		//F11
	case 0x57:
		return '_';
		//F12
	case 0x58:
		return '_';

	case 0x38:
		altPressed = 1;
		return 0;
	case 0x2a:
		shiftPressed = 1;
		return 0;
	case 0x38 + 0x80:
		altPressed = 0;
		return 0;
	case 0x2a + 0x80:
		shiftPressed = 0;
		return 0;

	default:
		return 0;
	}
}

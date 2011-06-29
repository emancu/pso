#include <con.h>
#include <lib_str.h>
#include <sched.h>

#define CONSOLE_STYLE_DEFAULT VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT

/*
 * Private functions
 */

static uint_8* calculate_video_position(const chardev_console* this_chardev_console) {
	return (uint_8*) (this_chardev_console->console_screen + vga_cols * 2 * this_chardev_console->fila + this_chardev_console->columna * 2);
}

/*
 * End Private functions
 */

chardev_console* current_console;
chardev_console* empty_console;

char block[3] = { 219, '\0', ' ' };

#define BLOCK_NO_BLINK VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT | VGA_FC_WHITE
#define BLOCK_BLINK VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT | VGA_FC_BLINK

//  *video++ = 219; // ASCII for a block (full-filled '_')
//  *video++ = VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT | VGA_FC_BLINK;

void con_init() {
	// NOTE: Borrar linea al pedo
	// current_console = 0x0;
	empty_console = (chardev_console*) con_open();
	set_console_style(empty_console, VGA_BC_RED | VGA_FC_GREEN | VGA_FC_LIGHT);
	current_console = 0x0;
	vga_write(12, 35, "EMPTY CONSOLE", VGA_BC_RED | VGA_FC_GREEN | VGA_FC_LIGHT);
	copy_screen_to_memory((uint_8*) empty_console->console_screen);

	syscall_list[0x39] = (uint_32) &sys_run;

	//saco el cursor
	outb(0x3D4, 14); // Tell the VGA board we are setting the high cursor byte.
	outb(0x3D5, 0xFFFF >> 8); // Send the high cursor byte.
	outb(0x3D4, 15); // Tell the VGA board we are setting the low cursor byte.
	outb(0x3D5, (unsigned char) 0xFFFF); // Send the low cursor byte.
}

sint_32 con_read(chardev* this, void* buf, uint_32 size) {
	// Verificamos que no se esté tratando de leer más de lo que permite el buffer
	if (size > CON_BUFF_SIZE)
		return CON_ERROR_READTOOLARGE;

	chardev_console* this_chardev_console = (chardev_console *) this;
	char* char_buf = (char*) buf;
	//Chequeamos si hay suficientes cosas para leer en el buffer
	if (this_chardev_console->buff_cant < size) {
		//No hay suficiente para leer, lo bloqueamos en su semáforo
		this_chardev_console->busy = 1;
		this_chardev_console->read_expected = size;
		sem_wait(&this_chardev_console->sem);
	}

	//Si me desbloquean entonces hay para leer.
	int i;
	for (i = 0; i < size; i++) {
		char_buf[i] = this_chardev_console->buff[this_chardev_console->buff_index_start];
		this_chardev_console->buff_index_start = (this_chardev_console->buff_index_start + 1) % CON_BUFF_SIZE;
	}

	//  !todo chequear el race condition
	this_chardev_console->buff_cant -= size;
	this_chardev_console->read_expected = 0;
	return 0;
}

sint_32 con_write(chardev* this, const void* buf, uint_32 size) {
	char* char_buf = (char*) buf;
	chardev_console* this_chardev_console = (chardev_console *) this;

	int str = 0;
	uint_8* video = calculate_video_position(this_chardev_console);
	uint_8* screen_limit = (uint_8*) (this_chardev_console->console_screen + 4000);

	while (str < size && video < screen_limit) {
		//en realidad este \n no va a venir en la consola porque se va a mandar a ejecutar...
		if (char_buf[str] == '\n') { //Avanzo una línea el puntero
			write_in_console(this_chardev_console, &block[2], BLOCK_NO_BLINK, 1);
			this_chardev_console->fila++;
			this_chardev_console->columna = 0;
		} else if (char_buf[str] == 0x08) { //Backspace
			if (this_chardev_console->columna >= 14) {//depende de la longitud del ps1.
				if (this_chardev_console->columna == 0) {
					this_chardev_console->columna = vga_cols - 1;
					this_chardev_console->fila--;
				} else {
					this_chardev_console->columna--;
				}
				//escribo en la consola el block (cursor)
				write_in_console(this_chardev_console, &block[0], BLOCK_NO_BLINK, 2);
			}

		} else {
			//escribo en la pantalla
			write_in_console(this_chardev_console, &char_buf[str], this_chardev_console->style, 1);

			//actualizo fila y columna
			if (this_chardev_console->columna + 1 == vga_cols)
				this_chardev_console->fila++;
			this_chardev_console->columna = (this_chardev_console->columna + 1) % vga_cols;

			write_in_console(this_chardev_console, &block[0], BLOCK_NO_BLINK, 1);
		}

		//efecto consola
		if (this_chardev_console->fila == vga_rows - 1) {
			this_chardev_console->fila--;
			move_scr_up((uint_8*) &this_chardev_console->console_screen);
			move_scr_up((uint_8*) 0xB8000);
		}
		str++;
	}

	return str;
}

void write_in_console(chardev_console* this_console, const char* msg, uint_8 style, uint_8 cant) {
	char * addr = (current_console == this_console) ? (char *) vga_addr : (char *) &this_console->console_screen;
	vga_write_in_memory(addr, &this_console->fila, &this_console->columna, msg, style, cant);
}

uint_32 con_flush(chardev* this) {
	if (this->refcount != 0) {
		this->refcount--;
		return 0;
	}
	chardev_console* console_to_close = (chardev_console*) this;

	if (console_to_close->next != console_to_close) {
		console_to_close->next->prev = console_to_close->prev;
		console_to_close->prev->next = console_to_close->next;
	}

	if (current_console == ((chardev_console*) this)) {
		if (console_to_close->next == console_to_close)
			move_to_empty_console();
		else
			move_to_right_console();
	}

	mm_mem_free((uint_32*) this);
	return 0;
}

chardev* con_open(void) {
	// Pedimos pagina nueva del kernel para la consola y la inicializamos
	chardev_console* new_chardev_console = (chardev_console *) mm_mem_kalloc();
	new_chardev_console->dev.clase = DEV_ID_CHAR_CON;
	new_chardev_console->fila = 0;
	new_chardev_console->columna = 0;
	new_chardev_console->last_ps1_fila = 0;
	new_chardev_console->ps1_width = 12; //!todo alemata esto esta cableado. ver cuando se puede setear
	new_chardev_console->buff_index_start = 0;
	new_chardev_console->buff_index_end = 0;
	new_chardev_console->buff_cant = 0;
	new_chardev_console->sem = SEM_NEW(0);
	new_chardev_console->dev.read = &con_read;
	new_chardev_console->dev.write = &con_write;
	new_chardev_console->dev.flush = &con_flush;
	new_chardev_console->style = CONSOLE_STYLE_DEFAULT;

	// Inicializamos Screen
	int i;
	for (i = 0; i < 4000; i += 2) {
		new_chardev_console->console_screen[i] = 0x0;
		new_chardev_console->console_screen[i + 1] = CONSOLE_STYLE_DEFAULT;
	}

	//actualizo para swichear
	if (current_console == 0x0) {
		new_chardev_console->next = new_chardev_console;
		new_chardev_console->prev = new_chardev_console;
	} else {
		new_chardev_console->next = current_console->next;
		new_chardev_console->prev = current_console;
		(current_console->next)->prev = new_chardev_console;
		current_console->next = new_chardev_console;
	}

	copy_screen_to_memory((uint_8*) current_console->console_screen);
	current_console = new_chardev_console;
	copy_memory_to_screen((uint_8*) current_console->console_screen);
	return (chardev*) new_chardev_console;

	//TODO: Manejar errores
	return NULL;
}

extern pso_file task_task1_pso;

sint_32 sys_run(const char* archivo) {
	int j = 0;
	for (j = 0; j < CON_BUFF_SIZE; j++) {
		current_console->buff[j] = '\0';
	}
	int i = 6;
	char command[50] = "run ";
	char fileFullPath[50] = "/disk/";
	if (!strncmp(command, archivo, 4)) {
		str_into_string(fileFullPath, &i, archivo + 4);
		str_convert_to_mayus(fileFullPath, 6, strlen(fileFullPath));
		chardev_file* file_char_dev = (chardev_file*) fs_open(fileFullPath, 0x3);
		debugEnabled = 1;
		if (file_char_dev != NULL) {
			char * dir = ((char *) file_char_dev) + 0x200;
			return loader_load((pso_file *) dir, 0);
		}
	}
	return -1;
	//en archivo viene todo el comando que mando el usuario. ej: run task1.pso

}

void move_to_right_console() {
	if (current_console != 0x0) {
		copy_screen_to_memory((uint_8*) current_console->console_screen);
		copy_memory_to_screen((uint_8*) current_console->next->console_screen);
		current_console = current_console->next;
	}
}

void move_to_left_console() {
	copy_screen_to_memory((uint_8*) current_console->console_screen);
	if (current_console != 0x0) {
		copy_memory_to_screen((uint_8*) current_console->prev->console_screen);
		current_console = current_console->prev;
	}
}

void move_to_empty_console() {
	copy_memory_to_screen((uint_8*) empty_console->console_screen);
	current_console = empty_console;
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

	// Siempre se escribe en la consola actual
	char incoming_char = getc(tecla);
	if (incoming_char != 0 && current_console->buff_cant < CON_BUFF_SIZE) {
		uint_8 index = current_console->buff_index_end;
		current_console->buff[index] = incoming_char;
		current_console->buff_index_end = (++index) % CON_BUFF_SIZE;
		current_console->buff_cant++;

		if (current_console->buff_cant >= current_console->read_expected && current_console->busy != 0) {
			// Despertar a la tarea que estaba esperando.
			current_console->busy = 0;
			sem_signaln(&current_console->sem);
		}
	}
}

void set_console_style(chardev_console* console, uint_8 style) {
	console->style = style;
	int i;
	for (i = 1; i < 4000; i += 2)
		console->console_screen[i] = style;

	if (current_console == console)
		copy_memory_to_screen((uint_8 *) console->console_screen);
}

uint_8 getc(uint_16 scan_code) {
	switch (scan_code) {
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
	case 0x02:
		return '1';
	case 0x03:
		return '2';
	case 0x04:
		return '3';
	case 0x05:
		return '4';
	case 0x06:
		return '5';
	case 0x07:
		return '6';
	case 0x08:
		return '7';
	case 0x09:
		return '8';
	case 0x0a:
		return '9';
	case 0x0b:
		return '0';
	case 0x34:
		return '.';
	case 0x39:
		return ' '; //Enter
	case 0x1c:
		return '\n'; //tab
	case 0x0f:
		return '\t'; //Backspace
	case 0x0e:
		return 0x08;
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

	default: // F1-F12 or unknown
		return ((scan_code >= 0x3b && scan_code <= 0x44) || (scan_code >= 0x57 && scan_code <= 0x58)) ? '_' : 0;
	}
}


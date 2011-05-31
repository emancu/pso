#include <con.h>

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

char block[1] = { 219 };

//	*video++ = 219; // ASCII for a block (full-filled '_')
//	*video++ = VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT | VGA_FC_BLINK;

void con_init() {
	// NOTE: Borrar linea al pedo
	// current_console = 0x0;
	empty_console = (chardev_console*) con_open();
	set_console_style(empty_console, VGA_BC_RED | VGA_FC_GREEN | VGA_FC_LIGHT);
	current_console = 0x0;
	vga_write(12, 35, "EMPTY CONSOLE", VGA_BC_RED | VGA_FC_GREEN | VGA_FC_LIGHT);
	copy_screen_to_memory((uint_8*) empty_console->console_screen);

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

	this_chardev_console->buff_cant -= size;
	this_chardev_console->read_expected = 0;
	return 0;
}

sint_32 con_write(chardev* this, const void* buf, uint_32 size) {
	//!TODO: Chequear no pasarse de la pantalla y el uso de \n.
	char* char_buf = (char*) buf;
	chardev_console* this_chardev_console = (chardev_console *) this;

	int str = 0;
	uint_8* video = calculate_video_position(this_chardev_console);
	uint_8* screen_limit = (uint_8*) (this_chardev_console->console_screen + 4000);

	while (str < size && video < screen_limit) {
		//!todo no se si este manejo va aca o si lo tiene que tener el vga_write??
		if (current_console == this_chardev_console) {
			vga_write_cant(&this_chardev_console->fila, &this_chardev_console->columna, &char_buf[str], this_chardev_console->style, 1);
		} else {
			vga_write_in_memory((char *) &this_chardev_console->console_screen, &this_chardev_console->fila, &this_chardev_console->columna, &char_buf[str],
					this_chardev_console->style, 1);
		}
		str++;
	}

	return str;
}

uint_32 con_flush(chardev* this) {
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
	new_chardev_console->fila = 0;
	new_chardev_console->columna = 0;
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

		if (current_console->buff_cant >= current_console->read_expected) {
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

#include <con.h>
#include <tipos.h>
#include <vga.h>
#include <mm.h>

uint_32 current_console;


sint_32 con_read(chardev* this, void* buf, uint_32 size) {
	return 0;
}

sint_32 con_write(chardev* this, const void* buf, uint_32 size) {
	return 0;
}

uint_32 con_flush(chardev* this) {
	return 0;
}


chardev* con_open(void) {
    //pido una pagina nueva del kernel para la consola
    chardev_console* new_chardev_console = (chardev_console *) mm_mem_kalloc();
    new_chardev_console->fila = 0;
    new_chardev_console->columna = 0;
    //init structure;
    return (chardev*)new_chardev_console;
	return NULL;
}

void move_to_right_console(){
}

void move_to_left_console(){
}



void con_init() {
}




/*
 * Keyboard useful function
 */
int altPressed = 0;
int shiftPressed = 0;


void console_keyPressed(sint_16 tecla){
    if(altPressed == 1 && shiftPressed == 1){
        if(tecla == 0x4d){
            move_to_right_console();
        }else if (tecla == 0x4b){
            move_to_left_console();
        }
    }

    if(0 != getc(tecla))
        printf("%c", getc(tecla));
}


uint_8 getc(uint_16 scan_code) {
    switch (scan_code) {
    //case 0x2a: screen_write("SHIFT", BLACK, WHITE);
    //return 0;
    case 0x1e: return 'a';
    case 0x30: return 'b';
    case 0x2e: return 'c';
    case 0x20: return 'd';
    case 0x12: return 'e';
    case 0x21: return 'f';
    case 0x22: return 'g';
    case 0x23: return 'h';
    case 0x17: return 'i';
    case 0x24: return 'j';
    case 0x25: return 'k';
    case 0x26: return 'l';
    case 0x32: return 'm';
    case 0x31: return 'n';
    case 0x18: return 'o';
    case 0x19: return 'p';
    case 0x10: return 'q';
    case 0x13: return 'r';
    case 0x1f: return 's';
    case 0x14: return 't';
    case 0x16: return 'u';
    case 0x2f: return 'v';
    case 0x11: return 'w';
    case 0x2d: return 'x';
    case 0x15: return 'y';
    case 0x2c: return 'z';
    case 0x39: return ' ';
    //Enter
    case 0x1c: return '\n';
    //tab
    case 0x0f: return '\t';
    //Backspace
    case 0x0e: return 0x08;
    //F1
    case 0x3b: return '_';
    //F2
    case 0x3c: return '_';
    //F3
    case 0x3d: return '_';
    //F4
    case 0x3e: return '_';
    //F5
    case 0x3f: return '_';
    //F6
    case 0x40: return '_';
    //F7
    case 0x41: return '_';
    //F8
    case 0x42: return '_';
    //F9
    case 0x43: return '_';
    //F10
    case 0x44: return '_';
    //F11
    case 0x57: return '_';
    //F12
    case 0x58: return '_';

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

    default: return 0;
    }
}

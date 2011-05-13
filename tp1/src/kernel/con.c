#include <con.h>


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

	return NULL;
}



void con_init() {

}

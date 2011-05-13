#include <hdd.h>

sint_32 hdd_block_write(blockdev* this, uint_32 pos, const void* buf, uint_32 size) {
	return -1; //Opcional
}

sint_32 hdd_block_read(blockdev* this, uint_32 pos, void* buf, uint_32 size) {
	return 0;
}

blockdev* hdd_open(int nro) {
	return 0;
}

void hdd_init(void) {
	
}



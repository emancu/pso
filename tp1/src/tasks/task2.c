#include <syscalls.h>


int main(void) {
	open("/console", 0x3);
	open("/console", 0x3);
	while (1){
	}
	// void* p = palloc();
	return 0;
}

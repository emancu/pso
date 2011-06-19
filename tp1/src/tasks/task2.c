#include <syscalls.h>
#include <i386.h>
#include <vga.h>

int main(void) {
//	char buff[10];

	char* msg = "Hola run";
//	char* msg2 = "222";
	int fd = open("/console", 0x3);
//	write(fd, msg, 8);

	while(1){

	}
	//  int fd2 = open("/disk/KERNEL.BIN", 0x3);
	//  char buff[512];
	//  breakpoint();
	//  seek(fd2, 512);
	//  read(fd2, buff, 512);
	//  breakpoint();
	//
	//	while (1) {
	//	}
	// void* p = palloc();
	return 0;
}

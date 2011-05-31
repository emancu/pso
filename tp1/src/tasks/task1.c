#include <syscalls.h>
#include <tipos.h>
// #include <lib_str.h>


int main(void) {
	// char buff[10];
	char* ps1 = "kernel@pso: ";
	char* enter = "enterr";
	char msg[3];
	msg[1] = (char) 219;
	msg[2] = '\0';
	// char* msg2 = "Hola 2!";
	int fd = open("/console", 0x3);
	//int fd2 = open("/console", 0x3);
	//!todo preguntar de strlen
	write(fd, ps1, 12);
	while (1) {
		read(fd, msg, 1);
		if (msg[0] == '\n') {
			write(fd, enter, 6);
		} else {
			write(fd, msg, 1);
		}
	}

	//write(fd2, msg2, 7);
	//  int i, j = 0;
	//  for (i = 0; i < 6000000; i++) {
	//    j = 0;
	//  }
	//close(fd2);
	//  for (i = 0; i < 6000000; i++) {
	//    j = 0;
	//  }
	//  close(fd);


	// void* p = palloc();
	return 0;
}

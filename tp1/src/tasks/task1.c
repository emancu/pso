#include <syscalls.h>
#include <tipos.h>

int main(void) {
	char buff[10];
	char* msg = "Hola Mundo! ";
	char* msg2 = "Hola 2!";
	int fd = open("/console", 0x3);
	//int fd2 = open("/console", 0x3);
	while(1){
		read(fd,msg,1);
		write(fd, msg, 1);
	}

	//write(fd2, msg2, 7);
//	int i, j = 0;
//	for (i = 0; i < 6000000; i++) {
//		j = 0;
//	}
	//close(fd2);
//	for (i = 0; i < 6000000; i++) {
//		j = 0;
//	}
//	close(fd);


	// void* p = palloc();
	return 0;
}

#include <syscalls.h>
#include <tipos.h>


int main(void) {
	char buff[10];
	char* msg = "Hola Mundo!\n";
	int fd = open("/console", 0x3);
	write(fd,msg,12);
	read(fd, buff, 5);
	write(fd,buff,5);
	while (1){
	}
	// void* p = palloc();
	return 0;
}

#include <syscalls.h>
#include <tipos.h>


int main(void) {
	char buff[10];
	char* msg = "Hola Mundo! ";
	char* msg2 = "Hola 22222! ";
	int fd = open("/console", 0x3);
	int fd2 = open("/console", 0x3);
	int i , j = 0;
	for(i = 0 ; i < 6000000 ; i++){
		j = 0;
	}
	write(fd,msg,12);
	write(fd2,msg2,12);
	read(fd, buff, 5);
	write(fd,buff,5);
	while (1){
	}
	// void* p = palloc();
	return 0;
}

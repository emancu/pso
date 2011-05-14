#include <syscalls.h>
#include <tipos.h>


int main(void) {
	char buff[10];
	int fd = open("/console", 0x3);
	write(fd,buff,4);
	while (1){
	}
	// void* p = palloc();
	return 0;
}

#include <syscalls.h>

int main(void) {
	//	char buff[10];
	char* msg = "3143339a";
	char* msg2 = "222";
	int fd = open("/console", 0x3);
	int fd2 = open("/console", 0x3);

	write(fd2, msg2, 3);
	write(fd, msg, 8);


	while (1) {
	}
	// void* p = palloc();
	return 0;
}

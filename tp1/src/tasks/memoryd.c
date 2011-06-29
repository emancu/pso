#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>


int main(void) {
	// char buff[10];
	int tmp = 0;
	for (tmp = 0; tmp < 1027; tmp++) {
		int* p = (int *) palloc();
		*p = 5;
	}

	breakpoint();
	while(1){

	}


	return 0;
}

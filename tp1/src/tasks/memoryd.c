#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>


int main(void) {
	// char buff[10];
	int tmp = 1026;
	for (tmp = 1026; tmp < 1027; tmp++) {
		int* p = (int *) palloc();
		*p = 5;
		share_page(p);
	}

	breakpoint();
	while(1){

	}


	return 0;
}

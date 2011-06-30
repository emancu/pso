#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>


int main(void) {
	// char buff[10];
	int tmp = 1026;
	int* p;
	for (tmp = 1026; tmp < 1027; tmp++) {
		 p = (int *) palloc();
		*p = 0;
		share_page(p);
	}
	int pid = fork();

	if(pid == 0){
		while(1){
			breakpoint();
			*p = *p % 10;
		}
	}else{
		while(tmp > 0){
			*p += 32;
			tmp = 0;
		}
		breakpoint();
	}

	while(1){
	}

	return 0;
}

#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>
// #include <lib_str.h>

int idx;

int main(void) {
	char command[14] = "run task1.pso";
	run(command);
	// void* p = palloc();
	return 0;
}

#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>
// #include <lib_str.h>

//para guardar el comando que voy escribiendo
char command[100];
//indice para moverse en command
int idx;

int main(void) {
	// char buff[10];
	int i,runResult = 0;
	char* ps1 = "console1@pso: ";
	char* invalidCommand = "command not found";
	char* enter = "\n";
	idx = 0;
	char msg[3];
	int fd = open("/console", 0x3);
	write(fd, ps1, 14);

	while (1) {
		read(fd, msg, 1);
		if (msg[0] == '\n') {
			runResult = run(command);
			if (runResult){
				write(fd, enter, 1);
				write(fd, invalidCommand,17);
			}
			write(fd, enter, 1);
			write(fd, ps1, 14);
			for (i = 0; i < 100; i++) {
				command[i] = '\0';
			}
			idx = 0;

		} else {
			write(fd, msg, 1);
			//			 si no se apreto el Backspace.
			if (msg[0] != 0x08) {
				command[idx++] = msg[0];
			} else {
				if (idx != 0){
					command[--idx] = '\0';
					idx--;
				}


		}
	}
}

// void* p = palloc();
return 0;
}

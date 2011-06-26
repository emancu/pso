#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>
#include <lib_str.h>

//para guardar el comando que voy escribiendo
char command[100];
//indice para moverse en command
int idx;

int main(void) {
	// char buff[10];
  
  char* error = "Could not open file";
  char buf[7];
  int i=0; 

	int fd = open("/console", 0x3);

  int file = open("/disk/lyric.txt", 0x3);
  if (file < 0)
    write(fd, error, 19);

  // fork();
  int pid = fork();
  while (i < 100) {
    if (pid) {
      read(file, buf, 5);
      write(fd, buf, 5);
    } else {
      read(file, buf+1, 5);
      buf[0] = '*';
      buf[6] = '*';
      write(fd, buf, 7);
    }
    i += 5;
  }

// void* p = palloc();
return 0;
}

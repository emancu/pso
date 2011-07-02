#include <i386.h>
#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <pipe.h>

int main(void) {
  int pipes[2] = {15, 16};
  int con = open("/console", 0x3);
  char* texto = "Puto el que lee...";
  int i;

  //breakpoint();
  pipe(pipes);
  int pid = fork();
  if (pid) { //Soy el padre (escribo)
    close(pipes[0]); //Cierro lectura
    for (i = 0; i < 18; i+=4) {
       write(pipes[1], &(texto[i]), 4); 
    }
  } else { //Soy el hijo (leo)
    close(pipes[1]); //Ciero escritura
    char* buf[3];
    for (i = 0; i < 18; i++) {
      pid = read(pipes[0], buf, 5);
      write(con, buf, pid);
      i -= (5 - pid);
    }
  }
  //breakpoint();

  while(1);
  exit();
  return 0;
}

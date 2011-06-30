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
    for (i = 0; i < 18; i++) {
       write(pipes[1], &(texto[i]), 1); 
    }
  } else { //Soy el hijo (leo)
    close(pipes[1]); //Ciero escritura
    char* buf[3];
    for (i = 0; i < 18; i++) {
      read(pipes[0], buf, 1);
      write(con, buf, 1);
    }
  }
  //breakpoint();

  exit();
  return 0;
}

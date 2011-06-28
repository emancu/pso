#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>

//para guardar el comando que voy escribiendo
char command[100];
//indice para moverse en command
int idx;

void run_command(const char*, int, const char*);

int main(void) {
  // char buff[10];
  idx = 0;
  char msg[3];
  char* ps1 = "console1@pso: ";

  int fd = open("/console", 0x3);
  write(fd, ps1, 14);

  while (1) {
    read(fd, msg, 1);
    if (msg[0] == '\n') {
      run_command(command, fd, ps1);
    } else {
      write(fd, msg, 1);

      // si no se apreto el Backspace.
      if (msg[0] != 0x08) {
        command[idx++] = msg[0];
      } else {
        if (idx != 0)
          command[--idx] = '\0';
      }
    }
  }

  return 0;
}

void run_command(const char* command, int fd, const char* ps1){
  char* invalidCommand = "command not found";
  char* enter = "\n";

  idx = 0;
  if (run(command) < 0) {
    write(fd, enter, 1);
    write(fd, invalidCommand, 17);
  }
  write(fd, enter, 1);
  write(fd, ps1, 14);
}

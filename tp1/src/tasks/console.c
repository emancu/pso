#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>

void run_command(const char*, int, const char*);

int main(void) {
  char command[100], msg;
  char* ps1 = "console@pso: ";
  int idx = 0;

  int fd = open("/console", 0x3);
  write(fd, ps1, 13);

  while (1) {
    read(fd, &msg, 1);
    if (msg == '\n') {
      run_command(command, fd, ps1);
      idx = 0;
    } else {
      write(fd, &msg, 1);

      // si no se apreto el Backspace.
      if (msg != 0x08) {
        command[idx++] = msg;
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

  if (run(command) < 0) {
    write(fd, "\n", 1);
    write(fd, invalidCommand, 17);
  }
  write(fd, "\n", 1);
  write(fd, ps1, 13);
}

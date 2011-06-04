#include <syscalls.h>
#include <tipos.h>
// #include <lib_str.h>


int main(void) {
  // char buff[10];
  // char* ps1 = "kernel@pso: ";
  char msg[14];
  msg[1] = (char)219;
  msg[2] = '\0';
  // char* msg2 = "Hola 2!";
  int fd = open("/serial0", 0x3);
  //int fd2 = open("/console", 0x3);
  //!todo preguntar de strlen
  while(1)
    read(fd,msg,14);

  return 0;
}

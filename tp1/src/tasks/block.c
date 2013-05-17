#include <i386.h>
#include <syscalls.h>

int main(void) {
  char buf[5];
  unsigned int i;
  int con = open("/console", 0x3);
  // write(con, "Presiona una tecla para continuar!", 34);
  while(1) {
    i = 15000000;
    write(con, "Presiona una tecla para procesar!\n", 35);
    read(con, buf, 1);
    while( i > 0){
      i--;
    }
  }
  exit();
  return 0;
}

#include <i386.h>
#include <syscalls.h>


int main(void) {
  // breakpoint();
  char buf[5];

  int con = open("/console", 0x3);
  write(con, "Hola mundo!", 11);
  write(con, "\n", 1);
  write(con, "\n", 1);
  write(con, "\n", 1);
  write(con, "\n", 1);
  write(con, "Me muero...", 11);
  read(con, buf, 1);
  exit();
  return 0;
}

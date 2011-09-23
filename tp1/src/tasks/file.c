#include <syscalls.h>

int main(void) {
  int con = open("/console", 0x3);
  int file = open("/disk/LYRIC.TXT", 0x3);
  char buf[20];
  read(file, buf, 20);
  write(con, buf, 20);
  read(con, buf, 1);
  exit();
}

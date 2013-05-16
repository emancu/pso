#include <syscalls.h>

int main(void) {
  int con = open("/console", 0x3);
  int file = open("/disk/LYRIC.TXT", 0x3);
  char buf[64];

  while(1){
    read(file, buf, 64);
    write(con, buf, 64);
    read(con, buf, 1);
  }
  exit();
}

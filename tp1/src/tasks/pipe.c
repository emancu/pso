#include <i386.h>
#include <syscalls.h>
#include <tipos.h>
#include <fs.h>

int main(void) {
  int pipes[2] = {15, 16};
  
  breakpoint();
  pipe(&pipes);
  breakpoint();
}

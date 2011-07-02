#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>


int global = 31;

int main(void) {
  int* p;
  p = (int *) 0x400f00;
//  *p=10;
    breakpoint();
  int pid = fork();

  if(pid == 0){
    global = 12;
    *p = 4;
    breakpoint();
  }else{
    global = 5;
    *p= 9;
    breakpoint();
  }

  breakpoint();

  exit();
  return 0;
}

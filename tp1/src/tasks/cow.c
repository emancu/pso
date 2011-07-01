#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>


int global = 31;

int main(void) {
  int* p;
  p = (int *) palloc();
  int pid = fork();

  if(pid == 0){
    breakpoint();
    global = 12;
    breakpoint();
  }else{
    breakpoint();
    global = 5;
    breakpoint();
  }

  exit();
  return 0;
}

#ifndef __KLIB_MACHINE_C__
#define __KLIB_MACHINE_C__

#include "klib_machine.h"

void print_machine_stat() {
  printf("Machine status\n");
  printf("    cr0: %x | cr2: %x | cr3: %x | cr4: %x \n", rcr0, rcr2, rcr3, rcr4);
  //printf("    gdtr: %x\n", rgdt);
  //printf("    ldtr: %x\n", rldt);
  //printf("    idtr: %x\n", ridt);
  printf("    tr: %x\n", rtr());
  printf("\n");
  printf("    cs: %x | ds: %x | es: %x | fs: %x | gs: %x | ss: %x \n", rcs, rds, res, rfs, rgs, rss);
}

#endif

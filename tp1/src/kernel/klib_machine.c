#ifndef __KLIB_MACHINE_C__
#define __KLIB_MACHINE_C__

#include "klib_machine.h"

void printk_machine_stat() {
  printk("Machine status\n");
  printk("    cr0: %x | cr2: %x | cr3: %x | cr4: %x \n", rcr0, rcr2, rcr3, rcr4);
  /*printk("    gdtr: %x\n", rgdt);
  printk("    ldtr: %x\n", rldt);
  printk("    idtr: %x\n", ridt);*/
  printk("    tr: %x\n", rtr());
  printk("\n");
  printk("    cs: %x | ds: %x | es: %x | fs: %x | gs: %x | ss: %x \n", rcs, rds, res, rfs, rgs, rss);
}

#endif

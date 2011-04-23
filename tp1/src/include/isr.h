#ifndef __ISR_H__
#define __ISR_H__


#include <gdt.h>
#include <idt.h>

typedef struct str_exp_state {
  uint_32 gs;
  uint_32 fs;
  uint_32 es;
  uint_32 ds;
  uint_32 ss;
  uint_32 edi;
  uint_32 esi;
  uint_32 ebp;
  uint_32 esp;
  uint_32 ebx;
  uint_32 edx;
  uint_32 ecx;
  uint_32 eax;
  uint_32 errcode;
  uint_32 org_eip;
  uint_32 org_cs;
  uint_32 eflags;
  uint_32 org_esp;
  uint_32 org_ss;
} __attribute__((__packed__)) exp_state;

#endif

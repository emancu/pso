/*
 * tss.h
 *
 *  Created on: Oct 21, 2008
 *      Author: slashack
 */

#ifndef TASK_STATE_SEGMENT__H
#define TASK_STATE_SEGMENT__H

/*
 *  Includes
 */
#include "tipos.h"


typedef struct str_tss {
  uint_16 ptl;
  uint_16 unused0;
  uint_32 esp0;
  uint_16 ss0;
  uint_16 unused1;
  uint_32 esp1;
  uint_16 ss1;
  uint_16 unused2;
  uint_32 esp2;
  uint_16 ss2;
  uint_16 unused3;
  uint_32 cr3;
  uint_32 eip;
  uint_32 eflags;
  uint_32 eax;
  uint_32 ecx;
  uint_32 edx;
  uint_32 ebx;
  uint_32 esp;
  uint_32 ebp;
  uint_32 esi;
  uint_32 edi;
  uint_16 es;
  uint_16 unused4;
  uint_16 cs;
  uint_16 unused5;
  uint_16 ss;
  uint_16 unused6;
  uint_16 ds;
  uint_16 unused7;
  uint_16 fs;
  uint_16 unused8;
  uint_16 gs;
  uint_16 unused9;
  uint_16 ldt;
  uint_16 unused10;
  uint_16 dtrap;
  uint_16 iomap;
} __attribute__((__packed__, aligned (8))) tss;

extern tss tss_entry;

#endif


#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <tipos.h>
#include <device.h>
#include <tipos.h>
#include <i386.h>
#include <isr.h>
#include <pic.h>
#include <debug.h>
#include <mm.h>

#ifdef __KERNEL__

#define SERIAL_BUFF_SIZE 64
#define SERIAL_ERROR_READTOOLARGE -1

typedef struct str_chardev_serial {
  chardev dev;                //dejar siempre primero
  uint_32 port;
}__attribute__((packed)) chardev_serial;

void serial_init();

void reset_serial_port(int serial_port);
void configure_serial_port(int serial_port);

extern void isr_serial_c();

chardev* serial_open(int nro);

#endif

#endif

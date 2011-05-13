#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <tipos.h>
#include <device.h>

#ifdef __KERNEL__

void serial_init();

chardev* serial_open(int nro);

#endif

#endif

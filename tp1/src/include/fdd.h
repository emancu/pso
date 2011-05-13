#ifndef __FDD_H__
#define __FDD_H__

#ifdef __KERNEL__
#include <tipos.h>
#include <device.h>


blockdev* fdd_open(int nro);

void fdd_init(void);
#endif

#endif

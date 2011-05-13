#ifndef __HDD_H__
#define __HDD_H__


#ifdef __KERNEL__
#include <tipos.h>
#include <device.h>

blockdev* hdd_open(int nro);

void hdd_init(void);

#endif

#endif


#ifndef __PROC_H__
#define __PROC_H__

#include <device.h>
#define CPUID_BUFF_LEN 100

typedef struct str_proc_cpuid_dev {
	chardev dev;
	uint_16 fp;
	char* buff;
	uint_16 busy;
} __attribute__((packed)) proc_cpuid_dev;



sint_32 proc_cpuid_read(chardev* this, void* buf, uint_32 size);
sint_32 proc_cpuid_write(chardev* this, const void* buf, uint_32 size);


uint_32 proc_cpuid_flush(chardev* this);


chardev* proc_cpuid_open(void);

#endif

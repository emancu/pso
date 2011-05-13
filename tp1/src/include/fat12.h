#ifndef __FAT12_H__
#define __FAT12_H__

#include <tipos.h>
#include <device.h>

/* Class fat12 */
typedef struct str_fat12 {
	blockdev* dev;
	/* ... completar ...*/
} fat12;


void fat12_init(void);

chardev* fat12_open(fat12* this, const char* filename, uint_32 flags);

void fat12_create(fat12* this, blockdev* dev);

#endif

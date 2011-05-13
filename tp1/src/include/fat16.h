#ifndef __FAT16_H__
#define __FAT16_H__

#include <tipos.h>
#include <device.h>

/* Class fat16 */
typedef struct str_fat16 {
	blockdev* dev;
	/* ... completar ...*/
} fat16;

void fat16_init(void);

chardev* fat16_open(fat16* this, const char* filename, uint_32 flags);

void fat16_create(fat16* this, blockdev* dev);

#endif

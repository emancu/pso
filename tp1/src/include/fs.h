#ifndef __FS_H__
#define __FS_H__

#include <tipos.h>
#include <device.h>
#include <syscalls.h>

#define FS_OPEN_RDONLY   0x0001
#define FS_OPEN_WRONLY   0x0002
#define FS_OPEN_RDWR     0x0003

// Syscalls
// int open(const char*, uint_32)

#ifdef __KERNEL__

void fs_init(void);

chardev* fs_open(const char* filename, uint_32 flags);

#endif

#endif

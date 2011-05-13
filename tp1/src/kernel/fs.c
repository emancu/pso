#include <fs.h>
#include <device.h>
#include <debug.h>

#include <sched.h>
#include <errors.h>

#include <device.h>


// Discos
#include <fdd.h>
#include <hdd.h>

// Sistemas de archivos
#include <fat12.h>
#include <fat16.h>

// Dispositivos
#include <serial.h>
#include <con.h>

#include <proc.h>


static sint_32 strcmp(const char* p, const char* q) {
	for (; *p && *q && *p==*q; p++,q++);
	return *p?(*q?(sint_32)*p-(sint_32)*q:1):*q?-1:0;
}

static sint_32 strncmp(const char* p, const char* q, uint_32 n) {
    for (; n && *p && *q && *p==*q; p++,q++,n--);
    return n?(*p?(*q?(sint_32)*p-(sint_32)*q:1):*q?-1:0):0;
}

/*
 * Disco 1;
 */
fat12 disk;

/*
 * Se pueden agregar más discos así:
 */
 //~ fat16 disk2;
 //~ fat32 disk3;
 //~ ext2 disk4;
 //~ resiserfs disk5;
 //~ ntfs disk6;

void fs_init(void) {
	/* Inicializar los dispositivos (ojo con las llamadas bloqueantes) */
	// Ejemplo: fat12_create(&disk, fdd_open(0));
	// Ejemplo: fat16_create(&disk2, hdd_open(0));
	//registro syscall open
	syscall_list[0x37] = (uint_32) &sys_open;
}


int sys_open(const char* file_name, uint_32 flags){
    int new_descriptor;
    chardev* new_char_dev = fs_open(file_name, flags);
    new_descriptor = device_descriptor(new_char_dev);
    printf("new descriptor: %d", new_descriptor);
    return new_descriptor;
}


chardev* fs_open(const char* filename, uint_32 flags) {
	/* Checkea el pedido de apertura como lectura o escritura */
	if ((flags & FS_OPEN_RDWR) == 0) return NULL; /* Pedido frutero */
	if (!strcmp(filename, "/serial0")) return serial_open(0);
	if (!strcmp(filename, "/serial1")) return serial_open(1);
	if (!strcmp(filename, "/console")) return con_open();
    if (!strcmp(filename, "/proc/cpuid")) return proc_cpuid_open();
 	/*
	 * Pedido para el disco 1: Usamos fat12 para abrirlo
	 */
	if (!strncmp(filename, "/disk/",6)) return fat12_open(&disk, filename+5, flags);

	return NULL;
}



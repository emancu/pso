#ifndef __MM_H__
#define __MM_H__

#include <tipos.h>
#include <vga.h>

#define MM_ATTR_P     0x001 // Present
#define MM_ATTR_RW    0x002 // Read/Write
#define MM_ATTR_RW_R  0x000 //
#define MM_ATTR_RW_W  0x002 //
#define MM_ATTR_US    0x004 // User/Supervisor
#define MM_ATTR_US_U  0x004 //
#define MM_ATTR_US_S  0x000 //
#define MM_ATTR_WT    0x008 // Wrtie-Through
#define MM_ATTR_CD    0x010 // Cache Disabled
#define MM_ATTR_A     0x020 // Accessed
#define MM_ATTR_D     0x040 // Dirty (for Pages)
#define MM_ATTR_AVL   0x040 // Available (for Directory)
#define MM_ATTR_PAT   0x080 // Page Table Attribute Index (for Pages)
#define MM_ATTR_SZ_4K 0x000 // Page Size (for Directory)
#define MM_ATTR_SZ_4M 0x080 // Page Size (for Directory)
#define MM_ATTR_G     0x100 // Global (ignored for Directory)

#define MM_ATTR_USR   0xE00 // bits for kernel

/* Control Register flags */
#define CR0_PE		0x00000001	// Protection Enable
#define CR0_MP		0x00000002	// Monitor coProcessor
#define CR0_EM		0x00000004	// Emulation
#define CR0_TS		0x00000008	// Task Switched
#define CR0_ET		0x00000010	// Extension Type
#define CR0_NE		0x00000020	// Numeric Errror
#define CR0_WP		0x00010000	// Write Protect
#define CR0_AM		0x00040000	// Alignment Mask
#define CR0_NW		0x20000000	// Not Writethrough
#define CR0_CD		0x40000000	// Cache Disable
#define CR0_PG		0x80000000	// Paging

#define CR4_PCE		0x00000100	// Performance counter enable
#define CR4_MCE		0x00000040	// Machine Check Enable
#define CR4_PSE		0x00000010	// Page Size Extensions
#define CR4_DE		0x00000008	// Debugging Extensions
#define CR4_TSD		0x00000004	// Time Stamp Disable
#define CR4_PVI		0x00000002	// Protected-Mode Virtual Interrupts
#define CR4_VME		0x00000001	// V86 Mode Extensions

#define PAGE_SIZE 4096
typedef struct str_mm_page {
	uint_32 attr:12;
	uint_32 base:20;
}  __attribute__((__packed__, aligned (4))) mm_page;

//Este tipo es el conjunto de bits usados para saber si un page_frame
//está ocupado o no.
typedef uint_32 page_frame_info;

#define make_mm_entry(base, attr) (mm_page){(uint_32)(attr), (uint_32)(base)}
#define make_mm_entry_addr(addr, attr) (mm_page){(uint_32)(attr), (uint_32)(addr) >> 12}

#define PAGE_SIZE 4096
#define MAGIC_NUMBER 0x4D324432
#define USR_MEM_START 4194304
#define KRN_MEM_START 1048576
#define PFI_OCCUPIED 0xFFFFFFFF

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var, pos) ((var) |= (1<<(pos)))
#define UNSET_BIT(var, pos) ((var) &= (~(1<<(pos))))

void mm_init(void);
void* mm_mem_alloc();
void* mm_mem_kalloc();
void mm_mem_free(void* page);

/* Manejador de directorios de página */
mm_page* mm_dir_new(void);
void mm_dir_free(mm_page* d);

/* Syscalls */
void* palloc(void);


/* Funciones de inicio de memoria */

// Esta función devuelve el puntero de la última posición de memoria válida contigua.
// Para ello, empezando desde 'start', escribe de a saltos 'jump' y verifica que se mantuvo
// lo escrito. Si es así se considera que el intervalo es válido y salta otro intervalo
// 'jump'. Sino se considera que la memoria es inválida y se devuelve el último valor escrito válido.
uint_32* memory_detect(uint_32* start, const uint_32 jump);

/* Funciones de tablas y directorios de páginas */


#endif


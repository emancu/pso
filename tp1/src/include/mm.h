#ifndef __MM_H__
#define __MM_H__

#include <tipos.h>
#include <vga.h>
#include <syscalls.h>
#include <loader.h>
#include <sem.h>

#define MM_ATTR_REQ    0x002 // Requested
#define MM_ATTR_SH     0x200 // Shared
#define MM_ATTR_COW    0x400 // Copy on write


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
#define CR0_PE    0x00000001  // Protection Enable
#define CR0_MP    0x00000002  // Monitor coProcessor
#define CR0_EM    0x00000004  // Emulation
#define CR0_TS    0x00000008  // Task Switched
#define CR0_ET    0x00000010  // Extension Type
#define CR0_NE    0x00000020  // Numeric Errror
#define CR0_WP    0x00010000  // Write Protect
#define CR0_AM    0x00040000  // Alignment Mask
#define CR0_NW    0x20000000  // Not Writethrough
#define CR0_CD    0x40000000  // Cache Disable
#define CR0_PG    0x80000000  // Paging
#define CR4_PCE   0x00000100  // Performance counter enable
#define CR4_MCE   0x00000040  // Machine Check Enable
#define CR4_PSE   0x00000010  // Page Size Extensions
#define CR4_DE    0x00000008  // Debugging Extensions
#define CR4_TSD   0x00000004  // Time Stamp Disable
#define CR4_PVI   0x00000002  // Protected-Mode Virtual Interrupts
#define CR4_VME   0x00000001  // V86 Mode Extensions
#define KERNEL_TEMP_PAGE 0xFFFFF000

/* Errores */
#define MM_ERROR_NOTALIGNED -5

//TODO: Este era el error, no ?? no encuentro el libre
#define PF_ERROR_READ_ONLY 7

typedef struct str_mm_page {
  uint_32 attr :12;
  uint_32 base :20;
}__attribute__((__packed__, aligned (4))) mm_page;

//Este tipo es el conjunto de bits usados para saber si un page_frame
//está ocupado o no.
typedef uint_32 page_frame_info;
#define pfi_size (sizeof(page_frame_info)*8) //Tamaño en bits de page_frame_info

#define make_mm_entry(base, attr) (mm_page){(uint_32)(attr), (uint_32)(base)}
#define make_mm_entry_addr(addr, attr) (mm_page){(uint_32)(attr), (uint_32)(addr) >> 12}

#define PAGE_SIZE 4096
#define TABLE_ENTRY_NUM 1024
#define DIR_SIZE (PAGE_SIZE*TABLE_ENTRY_NUM)
#define MAGIC_NUMBER 0x4D324432
#define USR_MEM_START 4194304 //4mb (Inicio de páginas de usuario)
#define KRN_MEM_START 1048576 //1mb (Inicio de páginas de kernel)
#define PFI_OCCUPIED 0xFFFFFFFF

#define SECTOR_SIZE 512

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var, pos) ((var) |= (1<<(pos)))
#define UNSET_BIT(var, pos) ((var) &= (~(1<<(pos))))

#define USR_STD_ATTR MM_ATTR_RW | MM_ATTR_US_U

extern page_frame_info kernel_pf_info[24];
extern page_frame_info usr_pf_info[32736];

void mm_init(void);
void* mm_mem_alloc();
void* mm_mem_kalloc();
void mm_mem_free(void* page);

void mm_table_free(uint_32* t, int dir_index);
uint_32 mm_times_mapped(uint_32 physical_addr, int dir_index, int table_index);

extern void isr_page_fault();

/* Manejador de directorios de página */
mm_page* mm_dir_new(void);
void mm_dir_free(uint_32* d);

/* Syscalls */
void* palloc(void);
sint_32 share_page(void* page);

/* Funciones de inicio de memoria */

// Esta función devuelve el puntero de la última posición de memoria válida contigua.
// Para ello, empezando desde 'start', escribe de a saltos 'jump' y verifica que se mantuvo
// lo escrito. Si es así se considera que el intervalo es válido y salta otro intervalo
// 'jump'. Sino se considera que la memoria es inválida y se devuelve el último valor escrito válido.
uint_32* memory_detect(uint_32* start, const uint_32 jump);

/* Funciones de tablas y directorios de páginas */

//Esta funcion mapea la direccion virtual 'virtual' a la direccion fisica 'fisica' en el directorio dado por 'cr3'.
//Si 'page_size' = 0 mapea usando paginas de 4kb, recorriendo el segundo nivel de la estructura y pidiendo a kernel una nueva pagina de ser necesario.
//Si el kernel no tiene mas paginas falla, devolviendo NULL. En caso de ser exitoso devuelve la dirección de la tabla/directorio de página donde se hizo el mapeo.
void* mm_page_map(uint_32 virtual, mm_page* cr3, uint_32 fisica, uint_32 page_size, uint_32 attr);

//Se encarga de invalidar la entrada en la tabla de páginas a la que se llega mediante la dirección virtual
//parámetro utilizando cr3 como dirección de la tabla de directorios. Devuelve la dirección del page_frame liberado.
//!!Asume que cr3 apunta a una tabla de directorios válida
void* mm_page_free(uint_32 virtual, mm_page* cr3);

//Se encarga de invalidar la entrada en el directorio de páginas ('cr3') direccionada por la dirección virtual.
//No se toma cuidado de que la tabla inferior se deshabilite.
//!! Asume que el cr3 apunta a un directorio válido
void mm_dir_unmap(uint_32 virtual, mm_page* cr3);

/* Esta función se encarga de copiar la paginación de 'old_cr3'. Devuelve el la dirección del 'cr3'
 * nuevo con las páginas copiadas. La función se encarga de mantener los atributos del mapeo,
 * reasginar los mapeos a nuevas páginas físicas y devolver el cotenido.
 * En caso de error devuelve NULL. Un error puede producirse por no alcanzar las páginas físicas
 * disponibles para copiar todo. */
mm_page* mm_dir_fork(mm_page* old_cr3);

/* Esta función es la continuación de dir_fork. Dada la tabla de páginas apuntada por
 * 'page_table', recorre las entradas mapeando y copiando las páginas allí presentes
 * en la tabla de páginas 'new_table'. Recibe además 'dir_entry' el índice en la tabla
 * de directorios para poder construir la dirección virtual base de la tabla.
 * Devuelve NULL si se produce un error por no haber más páginas de usuario. */
void* mm_page_fork(uint_32 dir_entry, uint_32* page_table, uint_32* new_table);

/* Copia 'cant' bytes desde la dirección virtual 'virtual' a la dirección
 * física 'fisica', mapeando esta temporalmente en KERNEL_TEMP_PAGE.
 * Nota: 'virtual' + 'cant' no deben superar el límite de una página.
 * Se devuelve error si esto sucede (MM_ERROR_NOTALIGNED). */
int mm_copy_vf(uint_32* virtual, uint_32 fisica, uint_32 cant);

/* Funciones de sistema */
// Esta es la función de sistema que implementa la funcionalidad de la syscall 'palloc'
// La función busca un marco de página libre en memoria de usuario y lo mapea al cr3 actual.
// La función puede fallar si no hay page frames libres de usuario o si no hay un page frame
// libre de kernel que se requiere para crear una nueva tabla de páginas en el directorio
// de la tarea actual. En caso de fallar devuelve NULL, sino devuelve la dirección virtual
// a la que fue mapeado el marco.
void* sys_palloc();
#endif


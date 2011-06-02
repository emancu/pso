#ifndef __DMA_H__
#define __DMA_H__

#include <tipos.h>
#include <device.h>
#include <i386.h>

/* El siguiente módulo de DMA posee las funciones para trabajar 
 * con el mismo. Ese está considerando que el DMA se utilizará 
 * solo para transferencias con el floppy. */

/* DMA options */
#define DMA_READ        0x46
#define DMA_WRITE       0x4A
#define DMA_RESET_VAL   0x06

#define DMAIO_ADDR       0x004
#define DMAIO_TOP        0x081
#define DMAIO_COUNT      0x005
#define DMAIO_FLIPFLOP   0x00C
#define DMAIO_MODE       0x00B
#define DMAIO_INIT       0x00A
#define DMAIO_MASK       0x00F

/* Constants */

#define DMA_DEFAULT_TIMEOUT 2000
#define DMA_MASK_ALL 0xF
#define DMA_MASK_CH2 0xB

/**********************/
/* Exported functions */
/**********************/

/* Esta función incializa el dma para trabajar con el floppy. 
 * (Ver http://wiki.osdev.org/DMA#Floppy_Disk_DMA_Initialization) */
int dma_setup();

/* Esta función, que considera el dma ya inicializado,
 * completa los registros de forma que el dma este
 * listo para que el floppy realize una lectura. 
 * El dma transferirá los datos a la dirección 
 * 'dest', de la cual solo se usarán los primeros
 * 20 bits. La cantidad es siempre 512. */
void dma_set_floppy_read(void* dest);

/* Esta función, que considera el dma ya inicializado,
 * completa los registros de forma que el dma este
 * listo para realizar una escritura en el floppy.
 * El dma transferirá los datos a la dirección 
 * 'src', de la cual solo se usarán los primeros
 * 20 bits. La cantidad es siempre 512. */
void dma_set_floppy_write(void* src);

/**********************/
/* Internal functions */
/**********************/

/* Esta función setea el contenido del Command Register. Las 
 * secciones del mismo que no son 'controller enable',
 * 'dreq sense high/low' y 'dack sense high/low' se establecen
 * con valores por defecto.
 * Algunas fuentes dicen que no tienen ningún efecto:
 * (http://wiki.osdev.org/DMA#The_Other_Registers) 
 * Los valores para sense funcionan igual que en el floppy. */
int dma_set_cmd_reg(char ce, char dreq_low, char dack_high);

/* Esta función setea el registro de modo */
int dma_set_mode(char ch, char tra, char auto_init, char down, char mode);

/* Devuelve el valor de la máscara de dma */
int dma_read_mask();

/* Setea el valor del la maácara del dma */
void dma_set_mask(uint_8 mask);

#endif

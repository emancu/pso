#ifndef __FDD_H__
#define __FDD_H__

#ifdef __KERNEL__
#include <tipos.h>
#include <device.h>
#include <dma.h>

/* IOCTL Commands */
#define FDD_IOCTL_CYL 1 // Utilizado para configurar cantidad de cilindros
#define FDD_IOCTL_HEAD 2 // Utilizado para configurar cantidad de cabezas
#define FDD_IOCTL_SECT 3 // Utilizado para configurar cantidad de sectores

#define FDD_DEFAULT_TIMEOUT 400000

/* Esta estructura se utiliza para almacenar los resultados
 * de los comandos del fdc. */
typedef struct str_fdc_stat {
  uint_8 st0, st1, st2, c, h, r, n;
  uint_8 dor, srA, srB, dsr, dir, ccr, msr;
  uint_8 config1, config2; //Mantienen los últimos valores de configuración usados, también los completa dumpreg.
  uint_8 pcn0, pcn1, pcn2, pcn3; //Present cilinder number (dumpreg)
  uint_8 srt_hut, hlt_nd, sc_eot, perpendicular; ///(dumpreg)
} __attribute__((packed)) fdc_stat;

/* Esta estructura es la representación lógica de un floppy drive. 
 * Debe mantener información del medio interno para exportar una visión
 * LBA del disco. El tamaño de sector se considera constante en 512 bytes. */
typedef struct str_blockdev_floppy {
  blockdev dev;
  uint_8 drive; //Floppy drive number (0 to 3)
  //A sector n == cylinder_count * h + sect_per_track * c + s - 1
  uint_8 cylinder_count; //Cylinder count of inserted disk
  uint_8 head_count; //Head count of the device
  uint_8 sect_per_track; //Sectors per track of inserted disk
  char buffer[512];
} __attribute__((packed)) blockdev_floppy;


blockdev* fdd_open(int nro);

/**********************/
/* Exported Functions */
/**********************/

/* Esta función se encarga de la lectura de un sector del disquette.
 * Prende el motor si no estaba encendido y, en ese caso, espera un tiempo
 * para que el mismo tenga el ritmo necesario para lectura. 
 * Se establece el data rate por defecto y se selecciona el dispositivo 'drv'.
 * Luego se inicializa el dma y se envía el comando de lectura.
 * Finalmente se espera a la interrupción del floppy.
 * Por último, se revisan los bytes de resultado y se devuelve
 * un estado de error en caso de haberlo. Se devuelve 0 de otra forma. 
 * Las varibles GPL y DTL toman valores por defecto. MFM toma valor 1. 
 * (Ver http://wiki.osdev.org/Floppy_Disk_Controller#Read.2FWrite) */
int fdd_read_sector(fdc_stat* fdc, char c, char h, char r, char eot, char mt, void* dest, char drv);

void fdd_init(void);

/**********************/
/* Internal Functions */
/**********************/

/* Función para captura de interrupciones en floppy */
extern void isr_fdd();

/* Función en c para captura de interrupciones en floppy */
void isr_fdd_c();

/* Esta función espera al suceso de la interrupción de floppy */
void fdd_wait_for_interrupt(uint_32 timeout);

/* Esta función envía un byte al fifo del fdc. Maneja timeout y chequeo de MSR. 
 * Devuelve 0 en caso de éxito. Sino devuelve un código de error. */
int fdd_send_byte(uint_8 byte);

/* Esta función recibe un byte del fifo del fdc.
 * Maneja timeout y chequeo de MSR. Devuelve en 'st' el resultado y
 * en el return el byte leído. *'st' es negativo en caso de error. */
uint_8 fdd_get_byte(int* st);

/* Esta función envía el comando *version* al fdc.
 * Devuelve el resultado del comando. */
int fdd_version();

/* Esta función habilita o deshabilita el motor 'motor' según el parámetro 'st'.
 * Se espera 0 <= 'motor' <= 3. Recibe como parámetro 'fdc' del cual 
 * obtener el último valor del dor para así mantener el resto de los campos. */
void fdd_mot_en(fdc_stat* fdc, uint_8 motor, char st);

/* Esta función selecciona el device 'dev' en el registro dor. */
void fdd_dev_sel(fdc_stat* fdc, uint_8 dev);

/* Esta funcion habilita o desabilita el dma según el parámetro 'st'. 
 * Recibe el parámetro 'fdc' para obtener de allí el último valor de dor. 
 * 'st' == 1 activa dma y 'st' == 0 lo desactiva, independientemente 
 * de la configuración del fdd. */
void fdd_dma_en(fdc_stat* fdc,char st);

/* Esta función verifica el estado del dma en el registro msr. 
 * Cuidado! La función devuelve el resultado opuesto a la configuración
 * del fdd. Resultado = 1 significa dma activado. */
char fdd_dma_st();

/* Esta función ejecuta el comando de configuración del fdc.
 * Puede fallar por timeout a la hora de enviar comandos. 
 * 'eis', 'efifo' y 'poll' activan o desactivan bits en el commando de parámetros.
 * 'fifothr' es el threshold de la fifo, debe ir entre 0 y 15. 'pretrk' es la
 * precompensación de track y varía entre 0x00 y 0xFF.
 * El significado del bit 'poll' está invertido. poll == 1 activa polling y poll == 0 lo desactiva.
 * Lo mismo pasa con el bit 'efifo'. Para el fdc, 'efifo' == 1 significa deshabilitar fifo,
 * para la función 'efifo' == 1 significa habilitar fifo. 
 * Devuelve 0 en caso de éxito, sino devuelve un código de error negativo. */
int fdd_configure(fdc_stat* fdc, char eis, char efifo, char poll, char fifothr, char pretrk);

/* Esta función ejecuta el comando de especificación del fdc.
 * De 'srt' y 'hut' se utiliza solo el primer nibble,
 * de 'hlt' se utilizan solo los primeros 7 bits. 
 * Devuelve 0 en caso de éxito, sino devuelve un código de error negativo. */
int fdd_specify(fdc_stat* fdc, char srt, char hut, char hlt, char nd);

/* Esta función envía una señal de recalibración de devices and device 'ds'
 * con 0 <= 'ds' <= 3. Esta función no solo envía el comando recalibrate, 
 * sino que también se encarga de la terminación, esto es:
 * hacer polling sobre msr para saber cuando terminó, y ejecutar el 
 * sense interrupt para sacar al floppy de estado busy. 
 * Además prende el motor si no estaba prendido y selecciona el dispostivo. 
 * Esta función no maneja la situación en la que el recalibrate no llegó al trk 0. */
int fdd_recalibrate(fdc_stat* fdc, char ds);

/* Esta función ejecuta el comando lock estableciendo
 * el valor según 'lock'. Además verifica que el byte
 * de retorno concuerde con el lock enviado. 
 * En caso de timeout o discordancia se devuelve un error
 * sino se devuelve 0. */
int fdd_lock(fdc_stat* fdc, char lock);

/* Este función ejecuta el comando 'dumpreg' en el fdc y completa 
 * la estructura con la información recibida. */
int fdd_dumpreg(fdc_stat* fdc);

/* Esta función ejecuta el comando 'sense interrupt status' y
 * completa la estructura con la información recibida. */
int fdd_sense_interrupt_status(fdc_stat* fdc);

/* Esta función actualiza el valor de los registros de lectura en 'fdc' */
void fdd_update_R_regs(fdc_stat* fdc);

/* Esta función actualiza el estado de 'fdc'. (Llama a dumpreg y update_R)*/
int fdd_update_status(fdc_stat* fdc);

/* Esta función actualiza el estado de 'fdc' y luego imprime su información. */
void fdd_print_status(fdc_stat* fdc);

/* Esta función ejecuta un reset completo del fdc. Es utilizado para la primera
 * inicialización del mismo. Activa implied seek, dma, fifo, fifo. 
 * Threshold toma valor 8 y precomp toma valor 0 (DEFAULT)
 * (redomendado en: http://wiki.osdev.org/Floppy_Disk_Controller#Configure) 
 * Se establecen valores para srt, hlt y hut. Estos valores son específicos
 * del hw en cuestión, por lo que se utilizan valores seguros
 * (http://wiki.osdev.org/Floppy_Disk_Controller#SRT.2C_HLT_and_HUT). 
 * La función también establece valores para DSR y CCR. El segundo prácticamente
 * no tiene utilidad y refleja el estado de DSR, por lo que se utiliza únicamente este.
 * El valor de CCR se mantiene al igual que luego del reset.
 * En DSR se matiene el valor del DATA Rate y Precompensation.
 * Estos valores se establecen en 0 por defecto siendo los valores más razonables.
 * (Ver http://wiki.osdev.org/Floppy_Disk_Controller#CCR_and_DSR). 
 * DOR se establece en 0x40. Esto apaga los motores y deshabilita las interrupciones,
 * se pretende evitar la IRQ6 que sucede luego del reset.
 * (Ver http://wiki.osdev.org/Floppy_Disk_Controller#Controller_Reset) }
 * Finalmente también apaga todos los motores y deja seleccionado el drive 0
 * que es el más probable que se use. De usarse otro drive debe ser seleccionado. */
int fdd_full_reset(fdc_stat* fdc);

#endif

#endif

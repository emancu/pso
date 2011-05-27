#ifndef __FDD_H__
#define __FDD_H__

#ifdef __KERNEL__
#include <tipos.h>
#include <device.h>

#define DEFAULT_TIMEOUT 2000

/* Esta estructura se utiliza para almacenar los resultados
 * de los comandos del fdc. */
typedef struct str_fdc_stat {
  uint_8 st0, st1, st2, c, h, r, n;
  uint_8 dor, stA, stB, dsr, dir, ccr;
} __attribute__((packed)) fdc_stat;

blockdev* fdd_open(int nro);

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
 * precompensación de track y varía entre 0x00 y 0xFF */
int fdd_configure(fdc_stat* fdc, char eis, char efifo, char poll, char fifothr, char pretrk);

void fdd_init(void);
#endif

#endif

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

/* Esta función habilita o deshabilita el 
 * motor 'motor' según el parámetro 'st'.
 * Se espera 0 <= 'motor' <= 3. */
void fdd_mot_en(fdc_stat* fdc, uint_8 motor, char st);

void fdd_init(void);
#endif

#endif

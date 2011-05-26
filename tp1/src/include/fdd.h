#ifndef __FDD_H__
#define __FDD_H__

#ifdef __KERNEL__
#include <tipos.h>
#include <device.h>

#define DEFAULT_TIMEOUT 2000

blockdev* fdd_open(int nro);

/* Esta función envía un byte al fdc. 
 * Maneja timeout y chequeo de MSR. 
 * Devuelve 0 en caso de éxito.
 * Sino devuelve un código de error. */
int fdd_send_byte(char byte);

void fdd_init(void);
#endif

#endif

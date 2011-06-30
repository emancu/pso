#ifndef __SEM_H__
#define __SEM_H__

#include <tipos.h>

/* ¿¿?? */
typedef sint_32 sem_id;

typedef struct str_sem_t {
  uint_32 vl; /* Free places */
  uint_32 q; /* Wait-event queue */
} sem_t;

#define SEM_NEW(val) (sem_t){.vl = (val), .q = (uint_32)-1}

/* Esta función inicializa el arreglo de semáforos con semáforos vacíos.
 * Distinguimos un semáforo vacío como aquel cuyo valor es -1 */
void sem_init();

void sem_wait(sem_t* s);
void sem_signaln(sem_t* s);
void sem_broadcast(sem_t* s);

#endif

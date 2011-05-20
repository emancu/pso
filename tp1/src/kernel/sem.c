#include <sem.h>

void sem_init() {
}

void sem_wait(sem_t* s) {
  if (s->vl > 0) {
    s->vl--;
  } else {
    loader_enqueue((int*)&(s->q));
  }
}

void sem_signaln(sem_t* s) {
  if (s->vl == 0) {
    loader_unqueue((int*)&(s->q));
  }else
    s->vl++;
}

void sem_broadcast(sem_t* s) {
  while (s->q != -1) {
    loader_unqueue((int*)&(s->q));
  }
  s->vl = 1; //Lo dejo en 1 para que pueda haber un nuevo acceso.
}

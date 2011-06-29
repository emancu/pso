#include <sem.h>
#include <loader.h> //NOTE: Si muevo el loader.h al sem.h no me deja compilar

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
  if (s->vl == 0 && s->q != -1) { //FIXME: Y si no hay a quién desencolar?
    loader_unqueue((int*)&(s->q));
  } else
    s->vl++;
}

void sem_broadcast(sem_t* s) {
  while (s->q != -1) {
    loader_unqueue((int*)&(s->q));
  }
  s->vl = 1; //Lo dejo en 1 para que pueda haber un nuevo acceso.
}

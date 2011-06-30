#include <pipe.h>
#include <errors.h>

sint_32 pipe_open(chardev* pipes[2]) {

  printf(" >pipe_open: creating pipes");
  pipe_page* pipepage = (pipe_page*) mm_mem_kalloc();

  if ((void*)pipepage == NULL) //No pudo pedirse la página de kernel para el pipe
    return PIPE_ERROR_NOPIPE;

  //Inicializo el pipe de lectura
  pipepage->read_pipe.dev.clase = DEV_ID_CHAR_PIPE;
  pipepage->read_pipe.dev.refcount = 0; //Lo incrementa device_descriptor
  pipepage->read_pipe.dev.read = &pipe_read;
  pipepage->read_pipe.dev.write = NULL; //En el de lectura no se escribe
  pipepage->read_pipe.dev.seek = NULL;
  pipepage->read_pipe.dev.flush = &pipe_flush;
  pipepage->read_pipe.type = PIPE_TYPE_READ;
  pipepage->read_pipe.other = &(pipepage->write_pipe);
  pipepage->read_pipe.buffer_size = PIPE_BUFF_SIZE; 
  pipepage->read_pipe.cursor = 0; 
  pipepage->read_pipe.buffer_cant = 0; 
  pipepage->read_pipe.buffer = (char*)pipepage + SECTOR_SIZE; 
  pipepage->read_pipe.page = (void*)pipepage;
  pipepage->read_pipe.sem = SEM_NEW(0); //Variable de condición
  pipepage->read_pipe.busy = 0;
  
  //Inicializo el pipe de escritura
  pipepage->write_pipe.dev.clase = DEV_ID_CHAR_PIPE;
  pipepage->write_pipe.dev.refcount = 0; //Lo incrementa device_descriptor
  pipepage->write_pipe.dev.read = NULL; //En el de escritura no se lee
  pipepage->write_pipe.dev.write = &pipe_write;
  pipepage->write_pipe.dev.seek = NULL;
  pipepage->write_pipe.dev.flush = &pipe_flush;
  pipepage->write_pipe.type = PIPE_TYPE_WRITE;
  pipepage->write_pipe.other = &(pipepage->read_pipe);
  pipepage->write_pipe.buffer_size = PIPE_BUFF_SIZE; 
  pipepage->write_pipe.cursor = 0; 
  pipepage->write_pipe.buffer_cant = 0; 
  pipepage->write_pipe.buffer = (char*)pipepage + SECTOR_SIZE; 
  pipepage->write_pipe.page = (void*)pipepage;
  pipepage->write_pipe.sem = SEM_NEW(1); //Mutex
  pipepage->write_pipe.busy = 0;

  pipes[0] = (chardev*)&(pipepage->read_pipe);
  pipes[1] = (chardev*)&(pipepage->write_pipe);
  printf(" >pipe_open: pipes created R (%x), W (%x)", pipes[0], pipes[1]);
  return 0;
}

int sys_pipe(int pipes[2]) {
  printf(" >sys_pipe: pipes[2] = {%d, %d}", pipes[0], pipes[1]);
  int st;
  chardev* pipes_dev[2] = {0x0, 0x0};

  st = pipe_open(pipes_dev);
  printf(" >sys_pipe: se obtuvieron los pipedev: %x, %x", pipes_dev[0], pipes_dev[1]);
  if (st < 0) { //No se pudieron crear los pipes
    printf("! >sys_pipe: no pudieron crearse los pipes");
    return st;
  }

  pipes[0] = device_descriptor((chardev*)pipes_dev[0]);
  pipes[1] = device_descriptor((chardev*)pipes_dev[1]);
  if ((pipes[0] < 0) | (pipes[1] < 0)) { //No pudieron obtenerse los fd
    printf("! >sys_pipe: no pudieron obtenerse los fd");
    pipes_dev[0]->flush(pipes_dev[0]);
    pipes_dev[1]->flush(pipes_dev[1]);
    return DEV_ERROR_NOFD;
  }
  printf(" >sys_pipe: descriptors: pipes[2] = {%d, %d}", pipes[0], pipes[1]);
  return 0;
}

void pipe_init(void) {
  //NOTE: Todavía no hace nada. Los pipes se manejan de forma dinámica.
  //Hay que revisar si no es necesario tener los chardevs en memoria.
  syscall_list[0x41] = (uint_32) &sys_pipe;
  return;
}

#define min(a, b) (a < b) ? a : b

sint_32 pipe_read(chardev* this, void* buf, uint_32 size) {
  int i;
  uint_32 total_to_copy;
  char* buffer = (char*) buf;
  pipedev* pipe = (pipedev*) this;
  pipedev* other = (pipedev*) (pipe->other);

  printf(" >pipe_read: this (%x), buf (%x), size (%d)", this, buf, size);

  if (other->dev.refcount == 0)  {//El extremo de escritura está cerrado, no puedo leer
    // printf("! >pipe_read: error PIPE_ERROR_CLOSED @ pipe %x", this);
    return PIPE_ERROR_CLOSED;
  }

  //Inicio de sección crítica de lectura
  sem_wait(&(other->sem));
  printf(" >pipe_read: entro en sección crítica @ pipe %x", this);

  if (pipe->buffer_cant >= size) { //Hay suficiente escrito para que lea
    printf(" >pipe_read: hay suficiente para leer (cant = %d) @ pipe %x", pipe->buffer_cant, this);
    total_to_copy = size;
  } else if (other->busy) { //Si el otro pipe está ocupado significa que debo leer para que pueda escribir
    printf(" >pipe_read: la escritura está esperando que lea @ pipe %x", this);
    total_to_copy = pipe->buffer_cant;
  } else { //No hay suficiente para que lea, entonces espero
    printf(" >pipe_read: espero escritura @ pipe %x", this);
    pipe->busy = 1;
    sem_signaln(&(other->sem)); //Libero el mutex
    //TODO: Chequear si no hay race condition
    sem_wait(&(pipe->sem)); //Espero que haya para escribir
    printf(" >pipe_read: escribieron y fui liberado (cant = %d) @ pipe %x", pipe->buffer_cant, this);
    sem_wait(&(other->sem)); //Vuelvo a tomar el mutex
    printf(" >pipe_read: tome el mutex @ pipe %x", this);
    //Una vez liberado puede que el pipe de escritura esté esperando espacio, copio lo que puedo
    // pipe->busy = 0; //Ya terminé de esperar
    if (other->busy)
      total_to_copy = min(pipe->buffer_cant, size);
    else
      total_to_copy = size;
  }
  for (i = 0; i < total_to_copy; i++) { //Leo la cantidad que me interesa
    buffer[i] = pipe->buffer[pipe->cursor];
    pipe->cursor = (pipe->cursor+1) % pipe->buffer_size; //Matengo la circularidad del buffer
  } 
 
  //Actualizo la cantidad en el buffer
  other->buffer_cant -= total_to_copy;
  pipe->buffer_cant -= total_to_copy; 
  if (other->busy) {
    other->busy = 0; //Establezco que no está ocupado porque le voy a liberar la varible
    sem_signaln(&(pipe->sem));
  }

  sem_signaln(&(other->sem));
  //Fin de la sección crítica

  printf(" >pipe_read: termine de leer cant (%d) @ pipe %x", total_to_copy, this);
  return total_to_copy;
}

sint_32 pipe_write(chardev* this, const void* buf, uint_32 size) {
  int i;
  const char* buffer = (char*) buf;
  pipedev* pipe = (pipedev*) this;
  pipedev* other = (pipedev*) (pipe->other);
 
  printf(" >pipe_write: this (%x), buf (%x), size (%d)", this, buf, size);

  if (other->dev.refcount == 0) { //El extremo de lectura está cerrado, no escribo
    // printf(" >pipe_write: error PIPE_ERROR_CLOSED @ pipe %x", this);
    return PIPE_ERROR_CLOSED;
  }

  sem_wait(&(pipe->sem)); //Tomo el mutex para la sección crítica
  printf(" >pipe_write: tomo el mutex para escribir @ pipe %x", this);

  for (i = 0; i < size; i++) {
    if (pipe->buffer_cant == pipe->buffer_size) { //El buffer está lleno no puedo escribir
      printf(" >pipe_write: el buffer esta lleno (cant = %d) @ pipe %x", pipe->buffer_cant, this);
      pipe->busy = 1; //Marco que estoy esperando lectura
      sem_signaln(&(pipe->sem)); //Libero el mutex
      printf(" >pipe_write: libero el mutex @ pipe %x", this);
      if (other->busy) { //Si mi hermano estaba esperando para leer lo libero
        printf(" >pipe_write: libero la variable de condicion (cant = %d) @ pipe %x", pipe->buffer_cant, this);
        other->busy = 0; //Establezco yo que el otro está libre porque voy a señalizar la variable
        sem_signaln(&(other->sem));
      }
      sem_wait(&(other->sem)); //Tomo la variable de condición
      printf(" >pipe_write: tomo la variable de condición (cant = %d) @ pipe %x", pipe->buffer_cant, this);
      // pipe->busy = 0; //Ya no estoy esperando lectura
      sem_wait(&(pipe->sem)); //Tomo el mutex
      printf(" >pipe_write: tomo el mutex @ pipe %x", this);
    }
    pipe->buffer[pipe->cursor] = buffer[i];
    pipe->cursor = (pipe->cursor+1) % pipe->buffer_size;
    pipe->buffer_cant += 1;
    other->buffer_cant += 1;
  }

  if (other->busy) //Si mi hermano espera para leer, lo libero
    sem_signaln(&(other->sem)); 

  sem_signaln(&(pipe->sem)); //Libero la sección crítica
  printf(" >pipe_write: termino de escribir @ pipe %x", this);
	return size;
}

uint_32 pipe_flush(chardev* this) {
  this->refcount--; //Disminuyo las referencias
  pipedev* pipe = (pipedev*) this; 

  //Si mis referencias son 0 y la de mi contraparte también, entonces puedo liberar la memoria
  if (this->refcount == 0 && ((pipedev*)pipe->other)->dev.refcount == 0) {
    mm_mem_free(pipe->page); //TODO: Si fuese de usuario hay que desmapearla
  }
	return 0;
}

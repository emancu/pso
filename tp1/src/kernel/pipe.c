#include <pipe.h>
#include <errors.h>

sint_32 pipe_open(chardev* pipes[2]) {

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
  pipepage->read_pipe.sem = SEM_NEW(1);
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
  pipepage->write_pipe.sem = SEM_NEW(1);
  pipepage->write_pipe.busy = 0);

  pipes[0] = (chardev*)&(pipepage->read_pipe);
  pipes[1] = (chardev*)&(pipepage->write_pipe);
  return 0;
}

int sys_pipe(int* pipes[2]) {
  printf(" >sys_pipe: pipes[2] = {%d, %d}", pipes[0], pipes[1]);
  int st;
  chardev* pipes_dev[2] = {0x0, 0x0};

  st = pipe_open(pipes_dev);
  printf(" >sys_pipe: se obtuvieron los pipedev: %x, %x", pipes_dev[0], pipes_dev[1]);
  if (st < 0) { //No se pudieron crear los pipes
    printf("! >sys_pipe: no pudieron crearse los pipes");
    return st;
  }

  pipes[0] = device_descriptor(pipes_dev[0]);
  pipes[1] = device_descriptor(pipes_dev[1]);
  if (pipes[0] < 0 | pipes[1] < 0) { //No pudieron obtenerse los fd
    printf("! >sys_pipe: no pudieron obtenerse los fd");
    pipes_dev[0]->flush(pipes_dev[0]);
    pipes_dev[1]->flush(pipes_dev[1]);
  }
  printf(" >sys_pipe: descriptors: pipes[2] = {%d, %d}", pipes[0], pipes[1]);
}

void pipe_init(void) {
  //NOTE: Todavía no hace nada. Los pipes se manejan de forma dinámica.
  //Hay que revisar si no es necesario tener los chardevs en memoria.
  syscall_list[0x41] = &sys_pipe;
  return;
}

sint_32 pipe_read(chardev* this, void* buf, uint_32 size) {
  int i;
  uint_32 total_to_copy;
  char signal = 0;
  pipedev* pipe = (pipedev*) this;
  pipedev* other = (pipedev*) (pipe->other);

  if (other->refcount == 0)  //El extremo de escritura está cerrado, no puedo leer
    return PIPE_ERROR_CLOSED;

  //Inicio de sección crítica de lectura
  sem_wait(&(other->sem));

  if (pipe->buffer_cant >= size) { //Hay suficiente escrito para que lea
    total_to_copy = size;
  } else if (other->busy) { //Si el otro pipe está ocupado significa que debo leer para que pueda escribir
    total_to_copy = pipe->buffer_cant;
  } else { //No hay suficiente para que lea, entonces espero
    pipe->busy = 1;
    sem_signal(&(other->sem)); //Libero el mutex
    //TODO: Chequear si no hay race condition
    sem_wait(&(pipe->sem)); //Espero que haya para escribir
    sem_wait(&(other->sem)); //Vuelvo a tomar el mutex
    //Una vez liberado puede que el pipe de escritura esté esperando espacio, copio lo que puedo
    pipe->busy = 0; //Ya terminé de esperar
    if (other->busy)
      total_to_copy = pipe->buffer_cant;
    else
      total_to_copy = size;
  }
  for (i = 0; i < total_to_copy; i++) { //Leo la cantidad que me interesa
    buf[i] = pipe->buffer[pipe->cursor];
    pipe->cursor = pipe->cursor+1 % pipe->buffer_size; //Matengo la circularidad del buffer
  } 
 
  //Actualizo la cantidad en el buffer
  other->buffer_cant -= total_to_copy;
  pipe->buffer_cant -= total_to_copy; 
  if (other->busy) sem_signal(&(pipe->sem));

  sem_signal(&(other->sem));
  //Fin de la sección crítica

  return total_to_copy;
}

sint_32 pipe_write(chardev* this, const void* buf, uint_32 size) {
	return 0;
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

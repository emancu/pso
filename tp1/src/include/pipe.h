#ifndef __PIPE_H__
#define __PIPE_H__

#include <tipos.h>
#include <device.h>
#include <mm.h>

#define PIPE_TYPE_READ 'R'
#define PIPE_TYPE_WRITE 'W'

#define PIPE_ERROR_NOPIPE -5
#define PIPE_ERROR_CLOSED -6

#define PIPE_BUFF_SIZE SECTOR_SIZE*6

/* pipedev */
typedef struct str_pipe {
  chardev dev;
  uint_8 type; //Tipo de pipe (R = lectura, W = escritura)
  void* other; //El otro chardev asociado a este en el pipe
  uint_32 buffer_size; //Tamaño total del buffer (Debe estar sincronizado con su hermano)
  uint_32 cursor; //Cursor de lecto-escritura dentro del buffer
  uint_32 buffer_cant; //Cantidad de contenido válido en el buffer 
  char* buffer; //Puntero al buffer compartido por los pipes
  void* page; //Puntero a la página donde se alojan las estructuras
  char busy; //Define si el pipe está esperando alguna acción de su hermano
  sem_t sem; //Semáforo para mantener separados lectura de escritura
  //El semáforo en un pipe de escritura funciona de mutex par la sección crítica
  //El semáforo en un pipe de lectura funciona como variable de condición respecto del buffer
} pipedev;

/* Esta estructura se usa para agrupar los pipedevs. */
typedef struct str_pipe_page {
  pipedev read_pipe;
  pipedev write_pipe;
} pipe_page;

/* Inicializa las estructruras para mantener los pipes*/
void pipe_init(void);

/* Recibe dos punteros en memoria de usuario para ser completados con los
 * file descriptors de los pipes de lectura y escritura respectivamente. i
 * Devuelve un número negativo en caso de error. */
int sys_pipe(int* pipes[2]);

/* Devuelve en 'pipes' dos punteros a chardevs de pipes asociados.
 * Devuelve 0 si todo sale bien, número negativo en caso de error.
 * Los chardevs devueltos deben ser usados por la syscall sys_pipe
 * para asociarlos a file descriptors. */
sint_32 pipe_open(chardev* pipes[2]);

sint_32 pipe_read(chardev* this, void* buf, uint_32 size);

sint_32 pipe_write(chardev* this, const void* buf, uint_32 size);

uint_32 pipe_flush(chardev* this);

// Syscalls
/*
int pipe(int pipes[2]);
*/

#endif

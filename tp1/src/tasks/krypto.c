#include <syscalls.h>
#include <tipos.h>
#include <fs.h>
#include <i386.h>
#include <lib_str.h>
#include <pipe.h>

#define BUFF_SIZE 512
#define CANT_READ 7

int fd;
int file;
int serial;
int pipeRE[2];
int pipeEW[2];

void reader() {
  close(pipeRE[0]);

  int i = 0;
  //Buffer temporal
  char buffer[BUFF_SIZE];
  while ( i++ < CANT_READ ) {
    read(file, buffer, BUFF_SIZE);
    write(pipeRE[1], buffer, BUFF_SIZE);
  }
  close(file);
  close(pipeRE[1]);
  // close(fd);
  // exit();
}

void encrpyt() {
  close(pipeEW[0]);
  close(serial);

  char xorer = 0xFA;
  char buffer[BUFF_SIZE];
  int i = 0, j = 0, limit;
  while ( i++ < CANT_READ ) {
    limit = read(pipeRE[0], buffer, BUFF_SIZE);
    for (j = 0; j < BUFF_SIZE; j++) {
      buffer[j] ^= xorer;
    }
    // write(pipeEW[1], buffer, BUFF_SIZE);
  }
  close(pipeEW[1]);
  close(fd);
}

void writer() {
  close(pipeEW[1]);
  int i = 0;
  //Buffer temporal
  char buffer[BUFF_SIZE];
  while ( i++ < CANT_READ ) {
    read(pipeEW[0], buffer, BUFF_SIZE);
    write(serial, buffer, BUFF_SIZE);
  }
  close(pipeEW[0]);
  close(serial);
  close(fd);
  // exit();
}


int main(void) {
  // char buff[10];

  char* error = "No pudo abrirse el archivo.";
  char buf[7];

  fd = open("/console", 0x3);
  //Abro el archivo
  file = open("/disk/KERNEL.BIN", 0x3);
  if (file < 0) {
    write(fd, error, 30);
    write(fd, "\n", 1);
    write(fd, "Presione una tecla para continuar...", 36);
    read(fd, buf, 1);
    exit();
  }


  serial = open("/serial0", 0x3);
  if (serial < 0) {
    write(fd, error, 30);
    write(fd, "\n", 1);
    write(fd, "Presione una tecla para continuar...", 36);
    read(fd, buf, 1);
    exit();
  }

  write(fd, "Archivo y serial abiertos satisfactoriamente, presione una tecla. \n", 47);
  read(fd, buf, 1);
  write(fd, "Continuamos...\n", 12);

  pipe(pipeRE);

  int pid = fork();
  if (pid) { //READER
    close(serial);
    reader();
  } else {
    close(pipeRE[1]);
    close(file);
    encrpyt();
    pid = 5;
    // pid = fork();
    if (pid) { //ENCRYPT
      encrpyt();
    } else { //WRITER
      // writer();
    }
  }

  write(fd, "Finished!", 9);
  // read(fd, buf, 1);

  while(1);
  // exit();
  return 0;
}

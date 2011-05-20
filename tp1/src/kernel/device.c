#include <device.h>
#include <errors.h>
#include <syscalls.h>
#include <vga.h>
#include <loader.h>

//tengo un array de file descriptors por cada proceso
chardev* char_devices[MAX_PID][MAX_FD];

int device_descriptor(chardev* dev) {
  //pido el primer fl libre para la tarea que esta ejecutando.
  int new_file_descriptor = get_next_free_fd_for_current_task();

  if (new_file_descriptor == -1)
    return -ENOSYS;

  char_devices[cur_pid][new_file_descriptor] = dev;
  dev->refcount++;
  return new_file_descriptor;
}

void device_init(void) {
  syscall_list[0x33] = (uint_32) &sys_read;
  syscall_list[0x34] = (uint_32) &sys_write;
  syscall_list[0x35] = (uint_32) &sys_seek;
  syscall_list[0x36] = (uint_32) &sys_close;

  //fill empty char_devices
  int i,j;
  for (i = 0; i < MAX_PID; i++)
    for (j = 0; j < MAX_FD; j++)
      char_devices[i][j] = 0x0;

}

int sys_read(int fd, void* buf, uint_32 size) {
  printf("entre a read");
  // Busco en la current task el file descriptor solicitado
  chardev* char_dev = (chardev*) char_devices[cur_pid][fd];
  return char_dev->read(char_dev, buf, size);
}

int sys_write(int fd, const void* buf, uint_32 size) {
  // Busco en la current task el file descriptor solicitado
  chardev* char_dev = (chardev*) char_devices[cur_pid][fd];
  return char_dev->write(char_dev, buf, size);;
}

int sys_seek(int fd, uint_32 size) {
  printf("SEEK");
  printf("llamaron a FD: %d", fd);
  printf("llamaron a SIZE: %d", size);
  return 2;
}

int sys_close(int fd) {
  // Busco en la current task el file descriptor solicitado
  chardev* char_dev = (chardev*) char_devices[cur_pid][fd];
  char_devices[cur_pid][fd] = 0x0;
  return  char_dev->flush(char_dev);
}

uint_32 get_next_free_fd_for_current_task() {
  int i;
  for (i = 0; i < MAX_FD; i++)
    if (char_devices[cur_pid][i] == 0x0)
      return i;

  //if there is no more fl.
  return -1;
}

#include <mm.h>

void* mm_mem_alloc() {
  return NULL;
}
void* mm_mem_kalloc() {
  return NULL;
}
void mm_mem_free(void* page) {
}

mm_page* mm_dir_new(void) {
  return NULL;
}
void mm_dir_free(mm_page* d) {
}


extern void* _end; // Puntero al fin del c'odigo del kernel.bin (definido por LD).
void mm_init(void) {

}

#include <mm.h>

//Esta estructura contiene un bit por cada page_frame asignable a kernel
page_frame_info kernel_pf_info[24];
//Esta estrucutra contiene un bit por cada page_frame asignable a usario (considerando 4gb - 4mb)
page_frame_info user_pf_info[32736];
//Esta variable contiene el total de page_frame de usuario considerando la cantidad de memoria del sistema
uint_32 usr_pf_limit;

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

uint_32* memory_detect(uint_32* start, const uint_32 jump) {
  uint_32* maxfree = start;
  *(maxfree+jump-1) = MAGIC_NUMBER;
  while (*(maxfree+jump-1) == MAGIC_NUMBER) {
    maxfree += jump;
    *(maxfree-1) = 0x0;
    *(maxfree+jump-1) = MAGIC_NUMBER;
  }
  return maxfree;
}


extern void* _end; // Puntero al fin del c'odigo del kernel.bin (definido por LD).
void mm_init(void) {
  usr_pf_limit = (uint_32)memory_detect((uint_32*)USR_MEM_START, PAGE_SIZE);
  printf("Total memory: %x | %d\n", usr_pf_limit, usr_pf_limit);
  usr_pf_limit = (usr_pf_limit - USR_MEM_START)/(PAGE_SIZE*32);
}

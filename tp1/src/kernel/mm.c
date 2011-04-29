#include <mm.h>

//Esta estructura contiene un bit por cada page_frame asignable a kernel
page_frame_info kernel_pf_info[24];
//Esta estrucutra contiene un bit por cada page_frame asignable a usario (considerando 4gb - 4mb)
page_frame_info usr_pf_info[32736];
//Esta variable contiene el total de page_frame de usuario considerando la cantidad de memoria del sistema
uint_32 usr_pf_limit;

void* page_frame_info_alloc(page_frame_info* pfi, uint_32 limit, uint_32 mem_offset) {
  //Recorro pfi y busco el primer bit libre (0),
  //devolviendo la dirección física del inicio de la página correspondiente
  uint_32 i = 0, j = 0;
  while (i < limit && pfi[i++] == PFI_OCCUPIED) ;//Busco la primer entrada no completa
  if (i >= limit) return NULL; //Si me pasé no hay más page_frames
  j = i-1;
  for (i = 0; CHECK_BIT(pfi[j], i); i++) ; //Busco el primer bit 0 (de der a izq)
  // printf("mm_mem_kalloc: %d %d", j, i);
  SET_BIT(pfi[j], i);
  return (void*)mem_offset + (j*8+i) * PAGE_SIZE; //Calculo la dirección de la página
}

void* mm_mem_alloc() {
  return page_frame_info_alloc(usr_pf_info, usr_pf_limit, USR_MEM_START);
}
void* mm_mem_kalloc() {
  return page_frame_info_alloc(kernel_pf_info, sizeof(kernel_pf_info), KRN_MEM_START);
}

void mm_mem_free(void* page) {
  //Verifico si la página pertenece a kernel (page < USR_MEM_START) o
  //usuario. Luego calculo la posición del arreglo correspondiente y ubico un (0)
  page_frame_info* pfi;
  if ((int)page < USR_MEM_START) {
    pfi = kernel_pf_info;
    page -= KRN_MEM_START;
  } else {
    pfi = usr_pf_info;
    page -= USR_MEM_START;
  }
  int index = (int)page/PAGE_SIZE;
  // printf("mm_mem_free: %d %d %x", index/8, index%8, pfi[index/8]);
  UNSET_BIT(pfi[index/8], index%8);
  // printf("mm_mem_free: %d %d %x", index/8, index%8, pfi[index/8]);
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
  int i = 0;
  printf("Initializing memory managment unit...\n");
  usr_pf_limit = (uint_32)memory_detect((uint_32*)USR_MEM_START, PAGE_SIZE);
  printf("Total memory: %x bytes (Assuming at least 4mb of contiguous memory)\n", usr_pf_limit, usr_pf_limit);
  usr_pf_limit = (usr_pf_limit - USR_MEM_START)/(PAGE_SIZE*32);
  printf("User page_frame limit: %x\n", usr_pf_limit);
  printf("Cleaning pf_info structures...");
  for (i = 0; i < sizeof(kernel_pf_info); i++) 
    kernel_pf_info[i] = 0x0;
  for (i = 0; i < sizeof(usr_pf_info); i++)
    usr_pf_info[i] = 0x0;
  printf("Done.");
  //Testeo de funciones de memoria
  printf("Testing mm_alloc functions...\n");
  void* temp1, * temp2;
  temp1 = mm_mem_kalloc();
  temp2 = mm_mem_kalloc();
  printf("Requesting kernel page: %x, requesting second kernel page: %x", temp1, temp2);
  mm_mem_free(temp1);
  printf("Freeing first kernel page. Requesting new kernel page: %x", mm_mem_kalloc());
  mm_mem_free(temp1);
  mm_mem_free(temp2);
  printf("Restoring structures. kernel_pf_info[0] = %x", kernel_pf_info[0]);
  temp1 = mm_mem_alloc();
  temp2 = mm_mem_alloc();
  printf("Requesting user page: %x, requesting second user page: %x", temp1, temp2);
  mm_mem_free(temp1);
  printf("Freeing first user page. Requesting new user page: %x", mm_mem_alloc());
  mm_mem_free(temp1);
  mm_mem_free(temp2);
  printf("Restoring structures. usr_pf_info[0] = %x", usr_pf_info[0]);
  //Fin testeo de funciones de memoria
}

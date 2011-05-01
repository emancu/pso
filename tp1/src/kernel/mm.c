#include <mm.h>

// Esta estructura contiene un bit por cada page_frame asignable a kernel
page_frame_info kernel_pf_info[24];
// Esta estrucutra contiene un bit por cada page_frame asignable a usario (considerando 4gb - 4mb)
page_frame_info usr_pf_info[32736];
// Esta variable contiene el total de page_frame de usuario considerando la cantidad de memoria del sistema
uint_32 usr_pf_limit;
// Esta variable contiene la direccion del directorio de paginas de kernel para la inicializacion
mm_page* kernel_dir;


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
  mm_page* cr3 = (mm_page*)mm_mem_kalloc();
  if (cr3 == NULL) return NULL;
  mm_page_map(0x0, cr3, 0x0, 1, USR_STD_ATTR);
  return cr3;
}

void mm_table_free(mm_page* d) {

}

void mm_dir_free(mm_page* d) {
  int i = 0;
  for (i = 0; i < TABLE_ENTRY_NUM; i++) {
    if (!(d[i].attr && MM_ATTR_SZ_4M)) { // Me encuentro con una entrada de 4 mb
      mm_table_free((mm_page*)d[i]);
    }
    d[i].attr &= ~0x1;
    //NO es tan simple liberar esos page_frames
   // mm_mem_free((void*)((int*)d)[i]);
  }
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

void activate_paging(uint_32 status) {
  int cr0 = rcr0();
  // printf("Activate paging, cr0 = %x", cr0);
  if (status) cr0 |= (1<<31);
  else cr0 &= ~(1<<31);
  // printf("Activate paging, cr0 = %x", cr0);
  lcr0(cr0);
}

void activate_pse(uint_32 status) {
  int cr4 = rcr4();
  printf("Activate pse, cr4 = %x", cr4);
  if (status) cr4 |= (1<<4);
  else cr4 &= ~(1<<4);
  printf("Activate pse, cr4 = %x", cr4);
  lcr4(cr4);
  tlbflush(); // Necesario segun el manual
}

int mm_page_map(uint_32 virtual, mm_page* cr3, uint_32 fisica, uint_32 page_size, uint_32 attr) {
	uint_32 ind_td = virtual >> 22;
	uint_32 ind_tp = (virtual << 10) >> 22;
	uint_32 base_tabla = fisica & ~0xFFF;
  uint_32 base_dir = ((int)cr3) & ~0xFFF;
  void* new_dir;
  printf("ind_td %x, ind_tp %x, base_tabla %x, base_dir %x", ind_td, ind_tp, base_tabla, base_dir);
	//Buscamos descriptor del directorio en cr3
	uint_32* desc_dir = (uint_32 *)(base_dir + (ind_td*4));
  if (page_size) { //Trabajo con paginas de 4 mb, no me importa si ya habia algo mappeado
    *desc_dir = ((fisica & ~0xFFFFF) & (attr & 0x1FFF)) | (MM_ATTR_SZ_4M | 1);
    return 0;
  }
  if (!(*desc_dir & 1)) { //Vemos si no està presente 
    new_dir = mm_mem_kalloc();
    if (!new_dir) return -1;
		*desc_dir = (int)new_dir & ~0xFFF;
		*desc_dir |= attr | 1;
		// *((uint_32 *)(base_dir + (ind_td*4))) = desc_dir;
	}
	uint_32* ptr_desc_tabla = (uint_32*)(((*desc_dir & ~0xFFF) + (ind_tp*4)));
	*ptr_desc_tabla = base_tabla | attr | 1; //!!Preguntar si hay que poner otros atributos
  printf("desc_dir = %x, *desc_dir = %x", desc_dir, *desc_dir);
  printf("desc_tabla = %x, *desc_tabla = %x", ptr_desc_tabla, *ptr_desc_tabla);
  return 0;
}

void* mm_page_free(uint_32 virtual, mm_page* cr3) {
	uint_32 ind_td = virtual >> 22;
	uint_32 ind_tp = (virtual << 10) >> 22;
	uint_32 base_dir = ((int)cr3) & ~0xFFF;
	uint_32* desc_dir = (uint_32 *)(base_dir + (ind_td*4));
  //Veo si llego mediante una pagina de 4mb
  if (*desc_dir & MM_ATTR_SZ_4M) {
    *desc_dir &= ~0x1;
    return (void*)(*desc_dir & ~0xFFF);
  }
	//Buscamos descriptor del directorio en cr3
	if (*desc_dir & 1) { //Vemos si no està presente 
		uint_32* ptr_desc_tabla = (uint_32*)((*desc_dir & ~0xFFF) + (ind_tp*4));
		*ptr_desc_tabla &= ~0x1;
    return (void*)(*ptr_desc_tabla & ~0xFFF);
	} else 
    return NULL;
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
    //Inicializo directorio de paginas de kernel (asumo existe PSE) y activo paginacion
    activate_pse(1);
    kernel_dir = mm_mem_kalloc();
    mm_page_map(0x0, kernel_dir, 0x0, 1, 0x2);
    lcr3((uint_32)kernel_dir);
    activate_paging(1);
    //Testo de funciones de paginacion
    printf("kernel_pf_info[1] = %x", kernel_pf_info[0]);
    printf("cr3[0] = %x, cr3[1] = %x", kernel_dir[0], kernel_dir[1]);
    printf("Mapeo 0x400000 a si misma en kernel_dir");
    mm_page_map(0x00400000, kernel_dir, 0x00400000, 0, 0x2);
    temp1 = (void*)((int*)kernel_dir)[1];
    temp1 = (void*)((int)temp1 & ~0xFFF);
    printf("cr3[0] = %x, cr3[1] = %x, cr3[1][0] = %x", kernel_dir[0], kernel_dir[1], *(int*)temp1);
    printf("Desmapeo 0x400000 en kernel_dir");
    mm_page_free(0x00400000, kernel_dir);
    printf("cr3[0] = %x, cr3[1] = %x, cr3[1][0] = %x", kernel_dir[0], kernel_dir[1], *(int*)temp1);
    printf("kernel_pf_info[1] = %x", kernel_pf_info[0]);


}

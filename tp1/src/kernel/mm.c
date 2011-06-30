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
  while (i < limit && pfi[i++] == PFI_OCCUPIED)
    ;//Busco la primer entrada no completa
  if (i >= limit)
    return NULL; //Si me pasé no hay más page_frames
  j = i - 1;
  i = 0;
  while (CHECK_BIT(pfi[j], i)) {
    i++;
  }; //Busco el primer bit 0 (de der a izq)
  // for (i = 0; CHECK_BIT(pfi[j], i); i++) ;
  // printf("mm_mem_kalloc: %d %d", j, i);
  SET_BIT(pfi[j], i);
  return (void*) mem_offset + (j * pfi_size + i) * PAGE_SIZE; //Calculo la dirección de la página
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
  if ((int) page < USR_MEM_START) {
    pfi = kernel_pf_info;
    page -= KRN_MEM_START;
  } else {
    pfi = usr_pf_info;
    page -= USR_MEM_START;
  }
  int index = (int) page / PAGE_SIZE;
  // printf("mm_mem_free: %d %d %x", index/32, index%32, pfi[index/32]);
  UNSET_BIT(pfi[index/pfi_size], index%pfi_size);
  // printf("mm_mem_free: %d %d %x", index/32, index%32, pfi[index/32]);
}

mm_page* mm_dir_new(void) {
  mm_page* cr3 = (mm_page*) mm_mem_kalloc();
  if (cr3 == NULL)
    return NULL;
  mm_page_map(0x0, cr3, 0x0, 1, USR_STD_ATTR);
  return cr3;
}

void mm_table_free(mm_page* d) {
  int i = 0;
  for (i = 0; i < TABLE_ENTRY_NUM; i++) {
    if (d[i].attr & MM_ATTR_P) { // La página está mappeada
      mm_mem_free((mm_page*) (d[i].base << 12));
      //        d[i].attr &= ~MM_ATTR_P;
    }
  }
}

// Acá tengo que liberar los page frames en espacio de usuario y en espacio de kernel
// En usuario los page frame ocupados van a estar apuntados por las tablas de páginas
// de segundo nivel (asumo que hay una única entrada de 4mb y la de identity mapping
// del kernel). Los page frames en kernel son los usados para guardar las tablas de
// páginas. Estos son apuntados por las direcciones en la tabla de directorios.
void mm_dir_free(mm_page* d) {
  int i = 0;
  for (i = 1; i < TABLE_ENTRY_NUM; i++) {
    if (d[i].attr & MM_ATTR_P) { // Si está presente entro recursivamente a borrar
      mm_table_free((void*) (d[i].base << 12)); // Libero las tablas
      //      d[i].attr &= ~MM_ATTR_P; // La marco como no presente (al pedo?)
      mm_mem_free((void*) (d[i].base << 12)); // Marco como libre el page frame donde estaba la tabla
    }
  }
  //  d[0].attr &= ~MM_ATTR_P;
  mm_mem_free((void*) ((int) d & ~0xFFF)); // Marco como libre el page frame donde está este directorio
  tlbflush();
}

uint_32* memory_detect(uint_32* start, const uint_32 jump) {
  uint_32* maxfree = start;
  *(maxfree + jump - 1) = MAGIC_NUMBER;
  while (*(maxfree + jump - 1) == MAGIC_NUMBER) {
    maxfree += jump;
    *(maxfree - 1) = 0x0;
    *(maxfree + jump - 1) = MAGIC_NUMBER;
  }
  return maxfree;
}

void activate_paging(uint_32 status) {
  int cr0 = rcr0();
  // printf("Activate paging, cr0 = %x", cr0);
  if (status)
    cr0 |= (1 << 31);
  else
    cr0 &= ~(1 << 31);
  // printf("Activate paging, cr0 = %x", cr0);
  lcr0(cr0);
}

void activate_pse(uint_32 status) {
  int cr4 = rcr4();
  // printf("Activate pse, cr4 = %x", cr4);
  if (status)
    cr4 |= (1 << 4);
  else
    cr4 &= ~(1 << 4);
  // printf("Activate pse, cr4 = %x", cr4);
  lcr4(cr4);
  tlbflush(); // Necesario segun el manual
}

void* mm_page_map(uint_32 virtual, mm_page* cr3, uint_32 fisica, uint_32 page_size, uint_32 attr) {
  uint_32 ind_td = virtual >> 22;
  uint_32 ind_tp = (virtual << 10) >> 22;
  uint_32 base_tabla = fisica & ~0xFFF;
  uint_32 base_dir = ((int) cr3) & ~0xFFF;
  void* new_dir;
  // printf("ind_td %x, ind_tp %x, base_tabla %x, base_dir %x", ind_td, ind_tp, base_tabla, base_dir);
  //Buscamos descriptor del directorio en cr3
  uint_32* desc_dir = (uint_32 *) (base_dir + (ind_td * 4));
  // printf("mm_page_map : cr3 = %x : desc_dir = %x : page_size = %d", cr3, desc_dir, page_size);
  if (page_size) { //Trabajo con paginas de 4 mb, no me importa si ya habia algo mappeado
    *desc_dir = ((fisica & ~0xFFFFF) & (attr & 0x1FFF)) | (MM_ATTR_SZ_4M | 1);
    return (void*) desc_dir;
  }
  if (!(*desc_dir & 1)) { //Vemos si no esta presente
    new_dir = mm_mem_kalloc();
    if (!new_dir)
      return NULL;
    *desc_dir = (int) new_dir & ~0xFFF;
    *desc_dir |= attr | 1;
    // *((uint_32 *)(base_dir + (ind_td*4))) = desc_dir;
  }
  uint_32* ptr_desc_tabla = (uint_32*) (((*desc_dir & ~0xFFF) + (ind_tp * 4)));
  *ptr_desc_tabla = base_tabla | attr | 1; //!!Preguntar si hay que poner otros atributos
  // printf("desc_dir = %x, *desc_dir = %x", desc_dir, *desc_dir);
  // printf("desc_tabla = %x, *desc_tabla = %x", ptr_desc_tabla, *ptr_desc_tabla);
  // return (void*)(*desc_dir & ~0xFFF);
  return (void*) (ptr_desc_tabla);
}

void* mm_page_free(uint_32 virtual, mm_page* cr3) {
  uint_32 ind_td = virtual >> 22;
  uint_32 ind_tp = (virtual << 10) >> 22;
  uint_32 base_dir = ((int) cr3) & ~0xFFF;
  uint_32* desc_dir = (uint_32 *) (base_dir + (ind_td * 4));
  //Veo si llego mediante una pagina de 4mb
  if (*desc_dir & MM_ATTR_SZ_4M) {
    *desc_dir &= ~0x1;
    return (void*) (*desc_dir & ~0xFFF);
  }
  //Buscamos descriptor del directorio en cr3
  if (*desc_dir & 1) { //Vemos si no està presente
    uint_32* ptr_desc_tabla = (uint_32*) ((*desc_dir & ~0xFFF) + (ind_tp * 4));
    *ptr_desc_tabla &= ~0x1;
    return (void*) (*ptr_desc_tabla & ~0xFFF);
  } else
    return NULL;
}

void mm_dir_unmap(uint_32 virtual, mm_page* cr3) {
  uint_32 ind_td = virtual >> 22;
  uint_32 base_dir = ((int) cr3) & ~0xFFF;
  uint_32* desc_dir = (uint_32 *) (base_dir + (ind_td * 4));
  *desc_dir &= ~MM_ATTR_P;
}

void* mm_page_fork(uint_32 dir_entry, uint_32* page_table, uint_32* new_table) {
  uint_32* dest_page;
  uint_32 page_index;
  //Recorro la tabla de páginas
  for (page_index = 0; page_index < TABLE_ENTRY_NUM; page_index++) {
    if (page_table[page_index] & MM_ATTR_P) { //La entrada es válida
      printf(" >mm_page_fork: present entry (%d)", page_index);
      //ver si esta shared o no??
      if (page_table[page_index] & MM_ATTR_SH) {
        printf(" >mm_page_fork: pagina shared emiliano gay!!!");
        printf("dir shared = %x", (uint_32*) (dir_entry * DIR_SIZE + page_index * PAGE_SIZE));
        new_table[page_index] = page_table[page_index];
      } else {
        dest_page = mm_mem_alloc(); //Pido una página nueva para el usuario
        if (dest_page == NULL) { //No hay más páginas
          return NULL;
        }
        mm_copy_vf((uint_32*) (dir_entry * DIR_SIZE + page_index * PAGE_SIZE), (uint_32) dest_page, PAGE_SIZE); //Copio el contenido de la página
        //Mapeo en la nueva estructura copiando los atributos
        new_table[page_index] = ((uint_32) dest_page & ~0xFFF) | (page_table[page_index] & 0xFFF);
      }
    }
  }
  return new_table;
}

//Se considera que no hay mapeos de 4mbs
mm_page* mm_dir_fork(mm_page* cr3) {
  uint_32* old_cr3 = (uint_32*) cr3;
  uint_32* new_cr3 = (uint_32*) mm_dir_new(); //Obtengo un nuevo directorio
  printf("new cr3 = %x" , new_cr3);
  if (new_cr3 == NULL) //No pudo tener un nuevo directorio, fallé
    return NULL;

  int dir_index;
  mm_page* page_table;
  uint_32* dest_page;

  for (dir_index = 1; dir_index < TABLE_ENTRY_NUM; dir_index++) {
    if (old_cr3[dir_index] & MM_ATTR_P) { //El directorio está presente
      printf(" >mm_dir_fork: present entry (%d)", dir_index);
      //TODO: Mapeos de 4mbs
      dest_page = mm_mem_kalloc(); //Pido una página para el nuevo directorio
      if (dest_page == NULL) { //No hay más páginas
        mm_dir_free((mm_page*) new_cr3); //Rollback
        return NULL;
      }
      //Armo la entrada para la nueva tabla
      new_cr3[dir_index] = ((uint_32) dest_page & ~0xFFF) | (old_cr3[dir_index] & 0xFFF);

      page_table = (mm_page*) (old_cr3[dir_index] & ~0xFFF); //Obtengo la dirección de la tabla de páginas
      //Empiezo el ciclo de la tabla de páginas
      if (!mm_page_fork(dir_index, (uint_32*) page_table, dest_page)) { //No se pudo hacer fork de la tabla de páginas
        mm_dir_free((mm_page*) new_cr3);
        return NULL;
      }
    }
  }
  return (mm_page*) new_cr3;
}

int mm_copy_vf(uint_32* virtual, uint_32 fisica, uint_32 cant) {
  printf(" >mm_copy_vf: virtual (%x), fisica(%x), cant(%d)", virtual, fisica, cant);
  int i;
  char* addr_to_copy = (char*) KERNEL_TEMP_PAGE;
  char* addr_from_copy = (char*) virtual;
  //Si copiando desde 'virtual' me voy a pasar del límite de la página
  if (((uint_32) virtual / PAGE_SIZE) != (((uint_32) virtual + cant - 1) / PAGE_SIZE))
    return MM_ERROR_NOTALIGNED; //Devuelvo error

  mm_page_map(KERNEL_TEMP_PAGE, (mm_page*) rcr3(), fisica, 0, MM_ATTR_RW | MM_ATTR_US_S);
  tlbflush();
  for (i = 0; i < cant; i++)
    addr_to_copy[i] = addr_from_copy[i];

  mm_page_free(KERNEL_TEMP_PAGE, (mm_page*) rcr3());
  tlbflush();
  return 0;
}

void* sys_palloc() {
  //Primero busco la dirección virtual donde voy a mappear (la primera página virtual no ocupada)
  // para saber si voy necesitar una página de kernel.
  mm_page* dir = (mm_page*) rcr3();
  mm_page* table;
  int i, j, virtual;

  //  if (usr_page == NULL)
  //    return NULL; // No pude obtener una nueva página de usuario
  for (i = 1; i < TABLE_ENTRY_NUM; i++) { //Recorro el directorio sin contar la primer entrada (identity mapping)
    // printf("sys_palloc - recorro dir - i = %d", i);
    if (dir[i].attr & MM_ATTR_P) {
      // printf("sys_palloc - dir[%d] = %x | presente",i,  (dir[i]));
      if (!(dir[i].attr & MM_ATTR_SZ_4M)) { //Si la entrada está ocupada y no es de 4mb, recorro la tabla inferior
        table = (mm_page*) (dir[i].base << 12);
        for (j = 0; j < TABLE_ENTRY_NUM; j++) {
          // printf("sys_palloc - recorro tabla - j = %d - table[%d] & MM_ATTR_P = %x", j, j, table[j].attr & MM_ATTR_P);
          if (!(table[j].attr & MM_ATTR_P) && !(table[j].attr & MM_ATTR_REQ)) {//Si encuentro una entrada en la taba libre, allí mapeo la página nueva
            //Armo la dirección virtual a partir de los índices en el directorio y en la tabla de páginas.
            //El índice de directorio me dice cuandos bloques de 4mb avancé, el de la tabla me dice cuantos de 4kb
            table[j].attr = MM_ATTR_REQ;
            virtual = PAGE_SIZE * TABLE_ENTRY_NUM * i + PAGE_SIZE * j;
            return (void*) virtual; // Devuelvo la dirección virtual a la que lo mapeé si está todo en orden.
          }
        }
      } else {
        // printf("sys_palloc - dir[%d] de 4mb", i);
      }
    } else { // Si encuentro una entrada de directorio libre, necesito una nueva página de kernel
      // printf("sys_palloc - dir[%d] no presente", i);

      if (!(dir[i].attr & MM_ATTR_REQ)) {
        dir[i].attr = MM_ATTR_REQ;
        virtual = PAGE_SIZE * TABLE_ENTRY_NUM * i;
        return (void*) virtual;
      }
    }
  }
  return NULL; // Se llega aquí si el mapa de memoria del cr3 actual está completo
  //printf("LLAMARON A PALLOC");
}

sint_32 mm_share_page(void* page) {
  uint_32 cr3 = rcr3();
  printf("page to share: %x", (uint_32) page);
  uint_32 base_dir = ((int) cr3) & ~0xFFF;
  uint_32 page_to_share = (uint_32) page;

  uint_32 ind_td = page_to_share >> 22;
  uint_32 ind_tp = (page_to_share << 10) >> 22;
  uint_32* desc_dir = (uint_32 *) (base_dir + (ind_td * 4));

  //!todo ver temas privilegio (y ver si esta presente y eso)

  //obtengo el PTE
  uint_32* ptr_desc_tabla = (uint_32*) (((*desc_dir & ~0xFFF) + (ind_tp * 4)));
  printf("dir anted se shared: %x", *ptr_desc_tabla);
  *ptr_desc_tabla |= MM_ATTR_SH;
  printf("dir dps se shared: %x", *ptr_desc_tabla);
}

void isr_page_fault_c(uint_32 error_code) {
  void* usr_page;
  void* kernel_page;
  printf("error code = %x", error_code);
  uint_32 cr3 = rcr3();
  //para que estoo
  uint_32 base_dir = ((int) cr3) & ~0xFFF;
  uint_32 page_fault_address = rcr2();

  uint_32 ind_td = page_fault_address >> 22;
  uint_32 ind_tp = (page_fault_address << 10) >> 22;
  mm_page* desc_dir = (mm_page *) (base_dir + (ind_td * 4));
  printf("page fault in addr: %x", page_fault_address);
  printf("cr3: %x", cr3);
  printf("ind_td %x, ind_tp %x, base_dir %x", ind_td, ind_tp, cr3);
  printf("attributes in page directory %x", (*desc_dir).attr);
  printf("base in page directory %x", (*desc_dir).base);
  printf("attribute");

  //me fijo si esta presente el PDE
  if (!((*desc_dir).attr & MM_ATTR_P)) {
    //el PDE no esta presente
    if ((*desc_dir).attr & MM_ATTR_REQ) {
      //una entrada en el directorio de paginas es requerido
      usr_page = mm_mem_alloc();
      if (usr_page == NULL)
        sys_exit(); // No pude obtener una nueva página de usuario
      kernel_page = mm_page_map(page_fault_address, (mm_page*) cr3, (uint_32) usr_page, 0, USR_STD_ATTR);
      if (kernel_page == NULL) { // Falló el prceso de mapeo a causa de falta de páginas de kernel
        mm_mem_free(usr_page); //Rollbackeo el proceso, devuelvo la página de usuario
        sys_exit();
      }
    } else {
      // no esta presente y no es requerido (por ahi las otras cosas que queria martin)
    }
  } else {
    //el PDE estaba presente

  }

  //obtengo el PTE
  uint_32 * tmp_pointer = (uint_32 *) desc_dir;
  mm_page* ptr_desc_tabla = (mm_page*) (((*tmp_pointer & ~0xFFF) + (ind_tp * 4)));
  //  printf("base in table dir %x", (*ptr_desc_tabla).base);
  //  printf("attributes in table dir %x", (*ptr_desc_tabla).attr);
  //  printf("attribute");

  //me fijo si el PTE esta presente
  if (!((*ptr_desc_tabla).attr & MM_ATTR_P)) {
    if ((*ptr_desc_tabla).attr & MM_ATTR_REQ) {
      //esta solicitada.
      usr_page = mm_mem_alloc();
      if (usr_page == NULL)
        sys_exit(); // No pude obtener una nueva página de usuario
      printf("new page %x", (uint_32) usr_page);
      mm_page_map(page_fault_address, (mm_page*) cr3, (uint_32) usr_page, 0, USR_STD_ATTR);
    } else {
      //la pagina no estaba presente
    }
  } else {
    //la PTE table estaba presente el error fue otro.
  }

  outb(0x20, 0x20);
}

extern void* _end; // Puntero al fin del c'odigo del kernel.bin (definido por LD).
void mm_init(void) {
  idt_register(0x0e, &isr_page_fault, 0);
  int i = 0;
  //    printf("Initializing memory managment unit...\n");
  usr_pf_limit = (uint_32) memory_detect((uint_32*) USR_MEM_START, PAGE_SIZE);
  //    printf("Total memory: %x bytes (Assuming at least 4mb of contiguous memory)\n", usr_pf_limit, usr_pf_limit);
  usr_pf_limit = (usr_pf_limit - USR_MEM_START) / (PAGE_SIZE * 32);
  //  printf("User page_frame limit: %x\n", usr_pf_limit);
  // printf("Cleaning pf_info structures...");
  for (i = 0; i < sizeof(kernel_pf_info); i++)
    kernel_pf_info[i] = 0x0;
  for (i = 0; i < sizeof(usr_pf_info); i++)
    usr_pf_info[i] = 0x0;

  activate_pse(1);
  kernel_dir = mm_dir_new();
  // printf("Done.");
  lcr3((uint_32) kernel_dir);
  // printf("Activating paging.000");
  activate_paging(1);
  //Registro palloc como syscall 0x30
  // printf("Registering system function sys_palloc...");
  syscall_list[0x30] = (uint_32) &sys_palloc;
  syscall_list[0x40] = (uint_32) &mm_share_page;
  // printf("Done.");
}

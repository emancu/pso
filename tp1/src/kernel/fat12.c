#include <fat12.h>

/* Attributes in Directory */

#define ATTR_READ_ONLY      0x01
#define ATTR_HIDDEN         0x02
#define ATTR_SYSTEM         0x04
#define ATTR_VOLUME_ID      0x08
#define ATTR_DIRECTORY      0x10
#define ATTR_ARCHIVE        0x20
#define ATTR_LONG_NAME      0x0F
#define ATTR_LONG_NAME_MASK 0x3F

/* Clusters */
#define CLUST_FREE 0x000
#define CLUST_RESV 0xFF0
#define CLUST_BAD  0xFF7
#define CLUST_EOF  0xFF8

/* Max disk size */
#define FAT12_MAX_SEC 2880
#define FAT12_SECT_SIZE 512
#define FAT12_SECT_PER_PAGE 8

/* Errors */
#define FILE_ERROR_NOFILEOPEN -2
#define FILE_ERROR_NOTFOUND -3

/* Macros */
#define buf_asign(dir, src, dst, isrc, idst, ia, ib, a, b) \
  src = (dir) ? (a) : (b); \
  dst = (dir) ? (b) : (a); \
  (isrc) = (dir) ? &(ia) : &(ib); \
  (idst) = (dir) ? &(ib) : &(ia);

#define isFile(entry) \
  ((entry).Filename[0] != 0x0 && (entry).Filename[0] != 0x05 && (entry).Filename[0] != 0x2E \
   && (entry).Filename[0] != 0xE5 && !((entry).Attributes & 0xDC))

fat12 fat12_node;

/***************/
/* Char device */
/***************/

/* Esta funci3ón copia del chardev al buffer o al revés según 'dir'.
 * 'dir' == 1 significa que copia del buffer al device, 'dir' == 0
 * hace lo contrario. La función maneja todas las jerarquías de datos 
 * utilizadas para guardar el archivo en memoria. */
sint_32 chardev_file_mov(chardev* this, void* buf, uint_32 size, char dir) {
	uint_32 i, c;
	char* char_buf = (char*) buf;
	char* src, *dst;
	uint_32* isrc, *idst;
	chardev_file* this_file = (chardev_file*) this;
	printf(" >chardev_file_mov: chardev (%x) buf (%x) size (%d) dir (%d)", this, buf, size, dir);
	printf("      file->cursor: %d file->size %d", this_file->cursor, this_file->size);
	if (size + this_file->cursor > this_file->size) {
		printf(" >chardev_file_mov: size+cursor > file->size.");
		size = this_file->size - this_file->cursor;
	}

	if (this_file->cursor < 7 * FAT12_SECT_SIZE) {
		printf(" >chardev_file_mov: Reading from the first 7 sectors...");
		//Estoy leyendo los primeros 7 sectores del archivo, los leo a continuación
		//del chardev
		buf_asign(dir, src, dst, isrc, idst, i, this_file->cursor, char_buf, ((char*)this)+512);
		// src = (dir) ? (char_buf) : ((char*)(this+512));
		// dst = (dir) ? ((char*)(this+512)) : (char_buf);
		// (isrc) = (dir) ? &(i) : &(this_file->cursor);
		// (idst) = (dir) ? &(this_file->cursor) : &(i);

		for (i = 0; i < size && this_file->cursor < 7 * FAT12_SECT_SIZE; i++) {
			dst[*idst] = src[*isrc];
			this_file->cursor++;
		}
		if (this_file->cursor >= 7 * FAT12_SECT_SIZE) { //Actualizo cur_page
			this_file->cur_page = this_file->data_pages[0];
		}
		if (i == size)
			return size; //Terminé de leer
	}

	while (this_file->cursor < (7 + DEV_FILE_FIRST_LVL_PAGES * FAT12_SECT_PER_PAGE) * FAT12_SECT_SIZE) {
		//Estoy leyendo entre el sector 7 y 7 + 400*8, uso la primera indirección
		buf_asign(dir, src, dst, isrc, idst, i, this_file->cursor, char_buf, (char*)this_file->cur_page);
		for (i = 0; i < size && (this_file->cursor % (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE)) != (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE) - 1; i++) {
			dst[*idst] = src[*isrc];
			this_file->cursor++;
		}
		//Tengo que leer el último byte de la página, entonces lo leo y actualizo la página actual
		if (i < size && ((this_file->cursor % (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE)) == (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE) - 1)) {
			dst[*idst] = src[*isrc]; //Copio el elemento que falta
			c = this_file->cursor - (7 * FAT12_SECT_SIZE);
			c = c / (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE); //Calculo el índice en el arreglo de primer indirección
			if (c < DEV_FILE_FIRST_LVL_PAGES - 1) // Sigo usando uno de los sectores de primera indirección
				this_file->cur_page = this_file->data_pages[c + 1];
			else { // Uso el primer sector de segunda indirección
				this_file->cur_sectors = this_file->more_pages;
				this_file->cur_page = this_file->more_pages->data_pages[0];
			}
			this_file->cursor++;
		}
		if (i == size)
			return size;
	}

	while (this_file->cursor < this_file->size) {
		//Estoy leyendo entre el sector 7+DEV_FILE_FIRST_LVL_PAGES y el resto, uso la segunda indirección
		buf_asign(dir, src, dst, isrc, idst, i, this_file->cursor, char_buf, (char*)this_file->cur_page);
		for (i = 0; i < size && (this_file->cursor % (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE)) != (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE) - 1; i++) {
			// char_buf[i] = this_file->cur_page[this_file->cursor];
			dst[*idst] = src[*isrc];
			this_file->cursor++;
		}
		//Tengo que leer el último byte de la página, entonces lo leo y actualizo la página actual
		if (i < size && ((this_file->cursor % (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE)) == (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE) - 1)) {
			// char_buf[i] = this_file->cur_page[this_file->cursor];
			dst[*idst] = src[*isrc];
			c = this_file->cursor - (7 + FAT12_SECT_SIZE * DEV_FILE_FIRST_LVL_PAGES) * FAT12_SECT_SIZE;
			c = c / (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE); //Calculo el índice en el arreglo de segunda indirección
			if (c < DEV_FILE_SECND_LVL_PAGES - 1) // Sigo usando uno de los sectores de primera indirección
				this_file->cur_page = this_file->data_pages[c + 1];
			else { // Uso el siguiente sector de segunda indirección
				this_file->cur_sectors = this_file->cur_sectors->next;
				if (this_file->cur_sectors == NULL) {
					return -1; //FIXME: add error
				}
				this_file->cur_page = this_file->cur_sectors->data_pages[0];
			}
			this_file->cursor++;
		}
		if (i == size)
			return size;
	}
	return i;
}

sint_32 chardev_file_read(chardev* this, void* buf, uint_32 size) {
	return chardev_file_mov(this, buf, size, 0);
}

sint_32 chardev_file_write(chardev* this, const void* buf, uint_32 size) {
	char buffer[size];
	strncpy(buffer, buf, size);
	return chardev_file_mov(this, buffer, size, 0);
}

sint_32 chardev_file_seek(chardev* this, uint_32 pos) {
	printf(" >chardev_file_seek: dev (%x) pos (%d)", this, pos);
	chardev_file* file = (chardev_file*) this;
	uint_32 s; //Temporario para cuentas
	uint_32 c; //Índice en la lista de segundas indirecciones
	if (pos < 7 * FAT12_SECT_SIZE) { //La posición está en los primeros 7 sectores
		printf(" >chardev_file_seek: pos is of the first 7 sectors.");
		file->cur_sectors = NULL;
		file->cur_page = (uint_32*) this;
	} else if (pos < (7 + DEV_FILE_FIRST_LVL_PAGES * FAT12_SECT_PER_PAGE) * FAT12_SECT_SIZE) {
		printf(" >chardev_file_seek: pos is greater than 7 sectors but less than %d sectors.", DEV_FILE_FIRST_LVL_PAGES);
		//La posición está en la primer indirección
		file->cur_sectors = NULL;
		file->cur_page = file->data_pages[(pos - 7 * FAT12_SECT_SIZE) / (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE)];
	} else {
		printf(" >chardev_file_seek: pos is greater than 7 + %d sectors.", DEV_FILE_FIRST_LVL_PAGES);
		//La posición está en la segunda indirección
		s = pos - (7 + DEV_FILE_FIRST_LVL_PAGES * FAT12_SECT_PER_PAGE) * FAT12_SECT_SIZE;
		c = s / (DEV_FILE_SECND_LVL_PAGES * FAT12_SECT_SIZE * FAT12_SECT_SIZE);
		file->cur_sectors = file->more_pages; //Sé que estoy al menos en el primer bloque de segunda indirección
		while (c--) {//Avanzo en los bloques lo que sea necesario
			file->cur_sectors = file->cur_sectors->next;
			s -= DEV_FILE_SECND_LVL_PAGES * FAT12_SECT_SIZE * FAT12_SECT_SIZE;
		}
		//Obtengo el índice del bloque
		file->cur_page = file->cur_sectors->data_pages[c / (FAT12_SECT_SIZE * FAT12_SECT_PER_PAGE)];
	}
	file->cursor = pos;
	return pos;
}

uint_32 chardev_file_flush(chardev* this) {
	return 0;
}

chardev* fat12_open(fat12* this, const char* filename, uint_32 flags) {
	printf(" >fat12_open: filename %s", filename);
	//Pido una nueva página para el chardev
	chardev_file* file;
	dir_entry file_entry;
	uint_16 sector;
	uint_16 dataStart;
	uint_32 size, sectCount;
	int st, i;
	st = fat12_find_file(this, filename, &file_entry);
	if (st < 0) {
		printf("! >fat12_open: file %s not found (Errno %d)", filename, st);
		return NULL;
	}

	//!todo ojo aca cambie alloc por kalloc para que pueda acceder el kernel!!!!!!!!!!(poner parametro???)
	file = (chardev_file*) mm_mem_kalloc();
	if (this == NULL) {
		printf("! >fat12_open: Could not alloc page for chardev");
		return NULL;
		// return FILE_ERROR_NOFILEOPEN;
	}

	printf(" >fat12_open: initializing chardev_file.");
	// Inicializo la estrucutra del archivo todo vacío
	file->cursor = 0;
	file->size = file_entry.FileSize;
	file->cur_page = (uint_32*) file;
	file->cur_sectors = NULL;
	file->more_pages = NULL;
	for (st = 0; st < DEV_FILE_FIRST_LVL_PAGES; st++)
		file->data_pages[st] = NULL;
	strncpy(file->name, filename, strlen(filename));
	file->dev.clase = DEV_ID_CHAR_FILE;
	//FIXME: Si dos procesos abren el mismo archivo tienen dos char dev distintos, no?
	file->dev.refcount = 1;
	file->dev.read = &chardev_file_read;
	file->dev.write = &chardev_file_write;
	file->dev.flush = &chardev_file_flush;
	file->dev.seek = &chardev_file_seek;

	//Empiezo a copiar la información del diskette
	dataStart = this->boot_sect.ReservedSectors + (this->boot_sect.FATcount * this->boot_sect.SectorsPerFAT) + ((this->boot_sect.MaxRootEntries * 32)
			/ FAT12_SECT_SIZE) - 2;
	sector = file_entry.FstClusLO;
	size = file->size;
	//Primeros 7
	sectCount = (size > FAT12_SECT_SIZE * 7) ? 7 : (size / FAT12_SECT_SIZE) + (size % FAT12_SECT_SIZE > 0);
	printf(" >fat12_open: copying first 7 sectors to %x (sectCount = %d) (sector = %d)...", ((char*) file) + 0x200, sectCount, sector);
	for (i = 0; i < sectCount; i++) {
		st = this->dev->read(this->dev, dataStart + sector, ((char*) file) + (0x200 * (i + 1)), 0x200);
		if (st < 0) {
			printf("! >fat12_open: Error reading first 7 sectors of file %s (Errno %d)", filename, st);
			return NULL;
		}
		sector = fat12_next_sector(this->fat, sector);
		printf(" >fat12_open: next sector = %d", sector);
	}

	printf(" >fat12_open: File %s opened on chardev_file @ %x", filename, file);
	return (chardev*) file;
}

/**********************/
/* Funciones externas */
/**********************/

void fat12_create(fat12* this, blockdev* dev) {
	int st;
	sint_32 cyl, sect, head;
	uint_16 sectPerFat;
	uint_16 sectPerRoot;
	this->dev = dev;
	if ((st = dev->read(dev, 0, (char*) &(this->boot_sect), sizeof(boot_sector))) < 0) {
		printf("! >fat12_create: Error reading boot sector from blockdev %x - Errno(%d)", dev, st);
		return;
	}
	printf("  >fat12_create: boot sector read from blockdev %x @ %x", dev, &(this->boot_sect));
	fat12_print_boot_sect(this);

	printf(" >fat12_create: configuring drive geometry");
	fat12_media_desc(&(this->boot_sect), &cyl, &sect, &head);
	dev->ioctl(dev, FDD_IOCTL_CYL, cyl);
	dev->ioctl(dev, FDD_IOCTL_HEAD, head);
	dev->ioctl(dev, FDD_IOCTL_SECT, sect);

	// Creando la fat
	sectPerFat = this->boot_sect.SectorsPerFAT;

	// Por cada 8 sectores que mide la fat, pido una página
	this->fat = (fat12_entry*) mm_mem_kalloc(); //FIXME: Si me quedo sin páginas???
	for (st = 1; st < sectPerFat / 8 + (sectPerFat % 8 > 0); st++) //FIXME: Estoy asumiendo el resto de las páginas contiguas
		mm_mem_kalloc();
	printf("Fat12 @ %x | fat table @ %x", this, this->fat);
	// Copio la fat a esas páginas
	if ((st = dev->read(dev, this->boot_sect.ReservedSectors, (char*) (this->fat), 4 * 512)) < 0) {
		printf("! >fat12_create: Error reading fat sectors from blockdev %x - Errno(%d)", dev, st);
		return;
	}
	printf(" >fat12_create: Fat successfully read.");

	// Pido una página para el caché de disco
	this->cache = mm_mem_kalloc(); //FIXME: Si me quedo sin páginas?
	mm_mem_kalloc(); // NOTE: Asumo contiguas
	this->cache->root_dir = (dir_entry*) ((char*)this->cache + 512); // Guardo donde empieza el root directory

	printf(" >fat12_create: Reading root directory from sector %d to addrs %x", this->boot_sect.ReservedSectors + this->boot_sect.FATcount * this->boot_sect.SectorsPerFAT);
	// Leo el root directory (uso sectPerFact para calcular la cantidad de sectores del root)
	sectPerRoot = (this->boot_sect.MaxRootEntries * sizeof(dir_entry)) / 512;
	// FIXME: El root tiene que medir menos de 7 sectores
	if (sectPerRoot >= FAT12_CACHE_SECTORS) {
		printf("! >fat12_create: Error root directory too large");
		return;
	}
	if ((st = dev->read(dev, this->boot_sect.ReservedSectors + this->boot_sect.FATcount * this->boot_sect.SectorsPerFAT, this->cache->root_dir, sectPerRoot
			* 512)) < 0) {
		printf("! >fat12_create: Error reading root directory.");
		return;
	}
	for (st = 0; st < FAT12_CACHE_SECTORS; st++)
		this->cache->sector[st] = 0; //Asigno cache como vacío
	for (st = 0; st < sectPerRoot; st++)
		this->cache->sector[st] = 1; //Asigno caché que tiene el root
	printf(" >fat12_create: Root directory succesfully read @ %x", this->cache->root_dir);

	// fat12_print_directory(this->cache->root_dir);
}

void fat12_init(void) {
	printf("FAT12: Creating fat12 node for drive 0 @ %x", &fat12_node);
}

/**********************/
/* Funciones internas */
/**********************/

void fat12_media_desc(boot_sector* boot, sint_32* tracks_per_side, sint_32* sect_per_track, sint_32* side_num) {
	switch (boot->MediaDescriptor) {
	case 0xF0: //FIXME: No trabaja caso 2.88mb, asume siempre 1.44 (http://en.wikipedia.org/wiki/File_Allocation_Table#Boot_Sector)
		*tracks_per_side = 80;
		*sect_per_track = 18;
		*side_num = 2;
		break;
	case 0xF8: //HDD ?
		break;
	case 0xF9: //FIXME: Falta un caso
		*tracks_per_side = 80;
		*sect_per_track = 9;
		*side_num = 2;
		break;
	case 0xFA:
		*tracks_per_side = 80;
		*sect_per_track = 8;
		*side_num = 1;
		break;
	case 0xFB:
		*tracks_per_side = 80;
		*sect_per_track = 8;
		*side_num = 2;
		break;
	case 0xFC:
		*tracks_per_side = 40;
		*sect_per_track = 9;
		*side_num = 1;
		break;
	case 0xFD:
		*tracks_per_side = 40;
		*sect_per_track = 9;
		*side_num = 2;
		break;
	case 0xFE:
		*tracks_per_side = 40;
		*sect_per_track = 8;
		*side_num = 1;
		break;
	case 0xFF:
		*tracks_per_side = 40;
		*sect_per_track = 8;
		*side_num = 2;
		break;
	}
}

void fat12_print_boot_sect(fat12* this) {
	boot_sector* boot = &this->boot_sect;
	printf("Sysname %s", boot->SysName);
	printf("Bytes per sector %d, Sectors per cluster %d, Reserved Sectors %d", boot->bytesPerSector, boot->SectorsPerCluster, boot->ReservedSectors);
	printf("FAT Count %d, Max Root Entries %d, Total Sectors %d", boot->FATcount, boot->MaxRootEntries, boot->TotalSectors1);
	printf("Media Descriptor %x, Sectors per FAT %d, Sectors per Track %d", boot->MediaDescriptor, boot->SectorsPerFAT, boot->SectorsPerTrack);
	printf("Head Count %d, Hidden Sectors %d, Drive Number %d", boot->HeadCount, boot->HiddenSectors, boot->DriveNumber);
}

void fat12_print_directory(dir_entry* dir) {
	printf("Imprimiendo directorio.");
	char name[8 + 3 + 2];
	name[8] = '.';
	name[8 + 3 + 1] = '\0';
	uint_16 i, j;
	for (i = 0; i < 16; i++) {
		switch (dir[i].Filename[0]) {
		case 0x0:
		case 0x05:
		case 0x2E:
			// case 0xE5:
			continue;
		}
		if (dir[i].Attributes == 0x80)
			continue;

		for (j = 0; j < 8; j++)
			name[j] = dir[i].Filename[j];
		for (j = 0; j < 3; j++)
			name[9 + j] = dir[i].Extension[j];
		printf("%s %x", name, dir[i].Attributes);
	}
}

uint_16 fat12_next_sector(fat12_entry* fat, uint_16 index) {
	uint_16 offset = index / 2;
	return (index % 2) ? fat[offset].second : fat[offset].first;
}

/* 0          12          24
 * | FAT ENTRY | FAT ENTRY |
 * | CHAR  | CHAR  | CHAR  |
 * 0       8      16      24
 * */
// uint_16 fat12_next_sector(char* fat, uint_16 index) {
// uint_16 offset = index / 2;
// char c1;
// switch (index % 2) {
// case 0:
// c1 = 0 | fat[offset];
// c1 |= ((fat[offset+1] & 0x0F) << 8);
// return c1;
// case 1:
// c1 = 0 | (fat[offset+2] << 4);
// c1 |= (fat[offset+1] >> 4);
// return c1;
// }
// return 0x0;
// }

#define min(a, b) (a < b) ? a : b

int fat12_find_file(fat12* fat, const char* filename, dir_entry* entry) {
	//TODO: Solo recorre el root sector
	// char name[8+3+2];
	// name[8] = '.';
	// name[8+3+1] = '\0';
	// int j;
	int i;
	dir_entry* dir = fat->cache->root_dir;
	for (i = 0; i < fat->boot_sect.MaxRootEntries; i++) {
		if (!isFile(dir[i]))
			continue; //No es archivo, sigo
		// for (j = 0; j < 8; j++)
		// name[j]= dir[i].Filename[j];
		// for (j = 0; j < 3; j++)
		// name[9+j] = dir[i].Extension[j];
		// printf(" >fat12_find_dir: Es archivo %s", name);
		if (!strncmp(filename, dir[i].Filename, min(8, strchr(filename, '.'))) && !strncmp(filename + strchr(filename, '.') + 1, dir[i].Extension, 3)) {
			// ) {
			//Si es el que busco termino
			*entry = fat->cache->root_dir[i];
			return 0;
		}
	}
	return FILE_ERROR_NOTFOUND;
}

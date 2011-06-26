#ifndef __FAT12_H__
#define __FAT12_H__

#include <tipos.h>
#include <device.h>
#include <mm.h>
#include <debug.h>
#include <device.h>
#include <fdd.h> 
#include <lib_str.h>

// Cantidad de sectores cacheados (variables + 1 de data)
#define FAT12_CACHE_SECTORS 7+8

/******************/
/* Fat 12 structs */
/******************/

/* Boot sector */
typedef struct str_boot_sector {
	uint_8 jumpBoot[3];
	char SysName[8];
	uint_16 bytesPerSector;
	uint_8 SectorsPerCluster;
	uint_16 ReservedSectors;
	uint_8 FATcount;
	uint_16 MaxRootEntries;
	uint_16 TotalSectors1;
	uint_8 MediaDescriptor;
	uint_16 SectorsPerFAT;
	uint_16 SectorsPerTrack;
	uint_16 HeadCount;
	uint_32 HiddenSectors;
	uint_32 TotalSectors2;
	uint_8 DriveNumber;
	uint_8 Reserved1;
	uint_8 ExtBootSignature;
	uint_32 VolumeSerial;
	char VolumeLabel[11];
	uint_8 Reserved2[8];
} __attribute__((__packed__)) boot_sector;

typedef struct str_fat_date {
	uint_16 day:5;
	uint_16 month:4;
	uint_16 year:7;
} __attribute__((__packed__)) fat_date;

typedef struct str_fat_time {
	uint_16 sec2:5;
	uint_16 min:6;
	uint_16 hour:5;
} __attribute__((__packed__)) fat_time;

typedef struct str_dir_entry {
	char Filename[8];
	char Extension[3];
	uint_8 Attributes;
	uint_8 NTRes;
	uint_8 CrtTimeTenth;
	fat_time CrtTime;
	fat_date CrtDate;
	fat_date LstAccDate;
	uint_16 FstClusHI;
	fat_time WrtTime;
	fat_date WrtDate;
	uint_16 FstClusLO;
	uint_32 FileSize;
} __attribute__((__packed__)) dir_entry;

typedef struct str_lfn_entry {
	uint_8 ordinal;
	uint_16 name0[5];
	uint_8 Attributes;
	uint_8 NTRes;
	uint_8 Checksum;
	uint_16 name1[6];
	uint_16 FstClusLO; // Siempre 0
	uint_16 name2[2];
} __attribute__((__packed__)) lfn_entry;

typedef struct str_fat12_entry {
  uint_16 first:12;
  uint_16 second:12;
} __attribute__((__packed__)) fat12_entry;

typedef struct str_fat12_cache {
  dir_entry* root_dir; //Dirección donde empieza el root directory
  // Sectores guardados en las posiciones 0 a 6 del cache.
  // El valor '1' corresponde al root dir 
  // El valor '0' significa entrada vacía
  int sector[FAT12_CACHE_SECTORS];
} __attribute__((__packed__)) fat12_cache;

/******************/
/* Fat 12 structs */
/******************/

/* Boot sector */
typedef struct str_boot_sector {
	uint_8 jumpBoot[3];
	char SysName[8];
	uint_16 bytesPerSector;
	uint_8 SectorsPerCluster;
	uint_16 ReservedSectors;
	uint_8 FATcount;
	uint_16 MaxRootEntries;
	uint_16 TotalSectors1;
	uint_8 MediaDescriptor;
	uint_16 SectorsPerFAT;
	uint_16 SectorsPerTrack;
	uint_16 HeadCount;
	uint_32 HiddenSectors;
	uint_32 TotalSectors2;
	uint_8 DriveNumber;
	uint_8 Reserved1;
	uint_8 ExtBootSignature;
	uint_32 VolumeSerial;
	char VolumeLabel[11];
	uint_8 Reserved2[8];
} __attribute__((__packed__)) boot_sector;

typedef struct str_fat_date {
	uint_16 day:5;
	uint_16 month:4;
	uint_16 year:7;
} __attribute__((__packed__)) fat_date;

typedef struct str_fat_time {
	uint_16 sec2:5;
	uint_16 min:6;
	uint_16 hour:5;
} __attribute__((__packed__)) fat_time;

typedef struct str_dir_entry {
	char Filename[8];
	char Extension[3];
	uint_8 Attributes;
	uint_8 NTRes;
	uint_8 CrtTimeTenth;
	fat_time CrtTime;
	fat_date CrtDate;
	fat_date LstAccDate;
	uint_16 FstClusHI;
	fat_time WrtTime;
	fat_date WrtDate;
	uint_16 FstClusLO;
	uint_32 FileSize;
} __attribute__((__packed__)) dir_entry;

typedef struct str_lfn_entry {
	uint_8 ordinal;
	uint_16 name0[5];
	uint_8 Attributes;
	uint_8 NTRes;
	uint_8 Checksum;
	uint_16 name1[6];
	uint_16 FstClusLO; // Siempre 0
	uint_16 name2[2];
} __attribute__((__packed__)) lfn_entry;

typedef struct str_fat_entry {
  uint_16 next:12;
} __attribute__((__packed__)) fat_entry;

/* Class fat12 */
typedef struct str_fat12 {
	blockdev* dev;
	/* ... completar ...*/
  boot_sector boot_sect; //Boot sector (no completo)
  fat12_entry* fat; //Puntero a la página de la fat
  //Puntero al cache de páginas ya bajadas y el root directory sector.
  //disco donde se guardan
  fat12_cache* cache;
} __attribute__((__packed__)) fat12;

/************************/
/* Funciones exportadas */
/************************/

/************************/
/* Funciones exportadas */
/************************/

void fat12_init(void);

chardev* fat12_open(fat12* this, const char* filename, uint_32 flags);

void fat12_create(fat12* this, blockdev* dev);

/**********************/
/* Funciones internas */
/**********************/

/* Imprime infromación del sector de booteo. */
void fat12_print_boot_sect(fat12* this); 

/* Obtiene información a partir del campo 'media descriptor' del boot sector. Deja los campos parámetro
 * iguales si el media descriptor no es un identificador conocido.
 * (http://en.wikipedia.org/wiki/File_Allocation_Table#Boot_Sector) */
void fat12_media_desc(boot_sector* boot, sint_32* tracks_per_side, sint_32* sect_per_track, sint_32* side_num);

/* Imprime el contenido del directorio (revisar) */
void fat12_print_directory(dir_entry* dir);

/* Obtiene el siguiente sector a partir de la fat parámetro */
uint_16 fat12_next_sector(fat12_entry* fat, uint_16 index);
// uint_16 fat12_next_sector(char* fat, uint_16 index);

/* Busca un archivo en el filesystem, de encontrarlo devuelve 0
 * y copia su entrada de directorio en 'entry'.
 * Sino devuelve un código de error < 0. */
int fat12_find_file(fat12* fat, const char* filename, dir_entry* entry);
#endif

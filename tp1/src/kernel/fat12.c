#include <fat12.h>
#include <debug.h>
#include <device.h>
#include <fdd.h> //NOTE: Seguramente no debería estar acá (modularidad)

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

fat12 fat12_node;

chardev* fat12_open(fat12* this, const char* filename, uint_32 flags) {

	return NULL;
}

void fat12_create(fat12* this, blockdev* dev) {
  int st;
  sint_32 cyl, sect, head;
  this->dev = dev;
  if ((st = dev->read(dev, 0, (char*)&(this->boot_sect), sizeof(boot_sector))) < 0) {
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

  // printf(" >fat12_create: reading first 4 sectors of fat @ %x", this->fat);
  // if ((st = dev->read(dev, this->boot_sect.ReservedSectors, (char*)&(this->fat), 512*4)) < 0) {
    // printf("! >fat12_create: Error reading fat sectors from blockdev %x - Errno(%d)", dev, st);
    // return;
  // }
  // printf(" >fat12_create: first 4 sectors of fat read @ %x", &(this->fat));
  
  // Creando la fat
  // Por cada 8 sectores que mide la fat, pido una página
  // Copio la fat a esas páginas
  // Apunto this->fat = primera_pagina (asumiendo páginas contiguas)
}


void fat12_init(void) {
  printf("FAT12: Creating fat12 node for drive 0 @ %x", &fat12_node);
  fat12_create(&fat12_node, fdd_open(0));
}

/**********************/
/* Funciones internas */
/**********************/

void fat12_media_desc(boot_sector* boot, sint_32* tracks_per_side, sint_32* sect_per_track, sint_32* side_num) {
  switch(boot->MediaDescriptor) {
    case 0xF0: //FIXME: No trabaja caso 2.88mb, asume siempre 1.44 (http://en.wikipedia.org/wiki/File_Allocation_Table#Boot_Sector)
      *tracks_per_side = 80; *sect_per_track = 18; *side_num = 2; 
      break;
    case 0xF8: //HDD ?
      break;
    case 0xF9: //FIXME: Falta un caso
      *tracks_per_side = 80; *sect_per_track = 9; *side_num = 2;
      break;
    case 0xFA:
      *tracks_per_side = 80; *sect_per_track = 8; *side_num = 1;
      break;
    case 0xFB:
      *tracks_per_side = 80; *sect_per_track = 8; *side_num = 2;
      break;
    case 0xFC:
      *tracks_per_side = 40; *sect_per_track = 9; *side_num = 1;
      break;
    case 0xFD:
      *tracks_per_side = 40; *sect_per_track = 9; *side_num = 2;
      break;
    case 0xFE:
      *tracks_per_side = 40; *sect_per_track = 8; *side_num = 1;
      break;
    case 0xFF:
      *tracks_per_side = 40; *sect_per_track = 8; *side_num = 2;
      break;
  }
}

void fat12_print_boot_sect(fat12* this) {
  boot_sector* boot = &this->boot_sect;
  printf("Sysname %s" , boot->SysName);
  printf("Bytes per sector %d, Sectors per cluster %d, Reserved Sectors %d", \
      boot->bytesPerSector, boot->SectorsPerCluster, boot->ReservedSectors);
  printf("FAT Count %d, Max Root Entries %d, Total Sectors %d", \
      boot->FATcount, boot->MaxRootEntries, boot->TotalSectors1);
  printf("Media Descriptor %x, Sectors per FAT %d, Sectors per Track %d", \
      boot->MediaDescriptor, boot->SectorsPerFAT, boot->SectorsPerTrack);
  printf("Head Count %d, Hidden Sectors %d, Drive Number %d", \
      boot->HeadCount, boot->HiddenSectors, boot->DriveNumber);
}

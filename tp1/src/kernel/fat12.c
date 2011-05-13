#include <fat12.h>
#include <debug.h>
#include <device.h>

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


chardev* fat12_open(fat12* this, const char* filename, uint_32 flags) {

	return NULL;
}

void fat12_create(fat12* this, blockdev* dev) {

}


void fat12_init(void) {

}


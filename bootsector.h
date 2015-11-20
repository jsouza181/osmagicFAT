#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

// fsMetadata definitions
#define BYTES_PER_SECTOR 0
#define SECTORS_PER_CLUSTER 1
// Reserved sectors before the FAT
#define RESERVED_SECTOR_COUNT 2
#define NUMBER_OF_FATS 3
// FAT size (in sectors)
#define FAT_SIZE 4
// Cluster number of the first cluster in the root dir (typically 2).
#define ROOT_CLUSTER 5
// Not used for FAT32? Always set to 0.
#define ROOT_ENT_CNT 6
// Number of sectors used by the root directory? Somehow 0?
#define ROOT_DIR_SECTORS 7
// The first data sector (NOT THE FIRST BYTE!)
#define FIRST_DATA_SECTOR 8

extern unsigned int fsMetadata[16];

int readBootSector(FILE *fileImgPtr);

#endif

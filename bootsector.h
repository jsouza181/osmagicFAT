#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

// fsMetadata definitions
#define BYTES_PER_SECTOR 0
#define SECTORS_PER_CLUSTER 1
#define RESERVED_SECTOR_COUNT 2
#define NUMBER_OF_FATS 3
#define FAT_SIZE 4
#define ROOT_CLUSTER 5

int readBootSector(FILE *fileImgPtr, unsigned int *fsMetadata);

#endif

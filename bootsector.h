#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

int readBootSector(FILE *fileImgPtr, unsigned int *bootSectorData);
void printHex(unsigned char * bootSector);

#endif

#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

int readBootSector(FILE *fileImgPtr);
void printHex(unsigned char * bootSector);

#endif

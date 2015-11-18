#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

int readBootSector(FILE *fileImgPtr);
void printHex(unsigned char * bootSector);

int bytesPerSector(unsigned char * bootSector);
int sectorsPerCluster(unsigned char * bootSector);
int reservedSectorCount(unsigned char * bootSector);
int numberOfFATS(unsigned char * bootSector);
int fatSize32(unsigned char * bootSector);
int rootCluster(unsigned char * bootSector);

#endif

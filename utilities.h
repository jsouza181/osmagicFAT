#ifndef UTILITIES_H
#define UTILITIES_H

unsigned int swapTwoBytes(unsigned int byteA, unsigned int byteB);
unsigned int swapFourBytes(unsigned int byteA, unsigned int byteB,
                            unsigned int byteC, unsigned int byteD);

void printSector(FILE *fileImgPtr, int offset, int sectorSize);
unsigned int getSector(unsigned int clusterNumber);
unsigned int getNextCluster(FILE *fileImgPtr, unsigned int clusterNumber);

#endif

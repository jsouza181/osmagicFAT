#ifndef UTILITIES_H
#define UTILITIES_H

#define EOC 0x0FFFFFF8

typedef struct {
  unsigned char ** dirEntries;
  unsigned int size;
} Directory;
/*
typedef struct {
  char filename[11]
}
*/

unsigned int swapTwoBytes(unsigned int byteA, unsigned int byteB);
unsigned int swapFourBytes(unsigned int byteA, unsigned int byteB,
                            unsigned int byteC, unsigned int byteD);
void capFilename(char *filename);

void printSector(FILE *fileImgPtr, unsigned int sectorNum);
unsigned int getSector(unsigned int clusterNumber);
unsigned int getNextCluster(FILE *fileImgPtr, unsigned int clusterNumber);
void getDirEntries(FILE *fileImgPtr, unsigned int clusterNumber, Directory *dir);


#endif

/*
 * bootsector.c retrieves all important file system metadata from the boot
 * sector, and also calculates any values that will be used again. Everything
 * that is retrieved or calculated is stored in fsMetadata, which is declared
 * in main()'s scope, so that the values can be used in the rest of the program.
 */

#include <stdio.h>
#include "bootsector.h"
#include "utilities.h"

unsigned int bytesPerSector(FILE *fileImgPtr) {
  fseek(fileImgPtr, 11, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);
  unsigned int byteB = getc(fileImgPtr);

  return swapTwoBytes(byteA, byteB);
}
unsigned int sectorsPerCluster(FILE *fileImgPtr) {
  fseek(fileImgPtr, 13, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);

  return byteA;
}
unsigned int reservedSectorCount(FILE *fileImgPtr) {
  fseek(fileImgPtr, 14, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);
  unsigned int byteB = getc(fileImgPtr);

  return swapTwoBytes(byteA, byteB);
}
unsigned int numberOfFATS(FILE *fileImgPtr) {
  fseek(fileImgPtr, 16, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);

  return byteA;
}
unsigned int fatSize32(FILE *fileImgPtr) {
  fseek(fileImgPtr, 36, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);
  unsigned int byteB = getc(fileImgPtr);
  unsigned int byteC = getc(fileImgPtr);
  unsigned int byteD = getc(fileImgPtr);

  return swapFourBytes(byteA, byteB, byteC, byteD);
}
unsigned int rootCluster(FILE *fileImgPtr) {
  fseek(fileImgPtr, 44, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);
  unsigned int byteB = getc(fileImgPtr);
  unsigned int byteC = getc(fileImgPtr);
  unsigned int byteD = getc(fileImgPtr);

  return swapFourBytes(byteA, byteB, byteC, byteD);
}

int readBootSector(FILE *fileImgPtr, unsigned int *fsMetadata) {
  //Gather important boot sector info.
  fsMetadata[BYTES_PER_SECTOR] = bytesPerSector(fileImgPtr);
  fsMetadata[SECTORS_PER_CLUSTER] = sectorsPerCluster(fileImgPtr);
  fsMetadata[RESERVED_SECTOR_COUNT] = reservedSectorCount(fileImgPtr);
  fsMetadata[NUMBER_OF_FATS] = numberOfFATS(fileImgPtr);
  fsMetadata[FAT_SIZE] = fatSize32(fileImgPtr);
  fsMetadata[ROOT_CLUSTER] = rootCluster(fileImgPtr);

  printSector(fileImgPtr, 0, fsMetadata[BYTES_PER_SECTOR]);

  return 0;
}

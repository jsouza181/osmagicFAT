/*
 * bootsector.c retrieves all important file system metadata from the boot
 * sector, and also calculates any values that will be used again. Everything
 * that is retrieved or calculated is stored in fsMetadata, which is declared
 * globally, so that the values can be used in the rest of the program.
 */

#include <stdio.h>
#include "metadata.h"
#include "utilities.h"

//Global array that contains necessary metadata for the file system.
unsigned int fsMetadata[16];

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

unsigned int rootEntryCount(FILE *fileImgPtr) {
  fseek(fileImgPtr, 17, SEEK_SET);
  unsigned int byteA = getc(fileImgPtr);
  unsigned int byteB = getc(fileImgPtr);

  return swapTwoBytes(byteA, byteB);
}

unsigned int rootDirectorySectors(FILE *fileImgPtr) {
  return ((fsMetadata[ROOT_ENT_CNT] * 32) + (fsMetadata[BYTES_PER_SECTOR] -1))
          / fsMetadata[BYTES_PER_SECTOR];
}

unsigned int firstDataSector(FILE *fileImgPtr) {
  return fsMetadata[RESERVED_SECTOR_COUNT] + (fsMetadata[NUMBER_OF_FATS] *
          fsMetadata[FAT_SIZE]) + fsMetadata[ROOT_DIR_SECTORS];
}

int readBootSector(FILE *fileImgPtr) {

  //Gather important boot sector info.
  fsMetadata[BYTES_PER_SECTOR] = bytesPerSector(fileImgPtr);
  fsMetadata[SECTORS_PER_CLUSTER] = sectorsPerCluster(fileImgPtr);
  fsMetadata[RESERVED_SECTOR_COUNT] = reservedSectorCount(fileImgPtr);
  fsMetadata[NUMBER_OF_FATS] = numberOfFATS(fileImgPtr);
  fsMetadata[FAT_SIZE] = fatSize32(fileImgPtr);
  fsMetadata[ROOT_CLUSTER] = rootCluster(fileImgPtr);
  fsMetadata[ROOT_ENT_CNT] = rootEntryCount(fileImgPtr);
  fsMetadata[ROOT_DIR_SECTORS] = rootDirectorySectors(fileImgPtr);
  fsMetadata[FIRST_DATA_SECTOR] = firstDataSector(fileImgPtr);

  return 0;
}

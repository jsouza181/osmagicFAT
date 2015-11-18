#include <stdio.h>
#include "bootsector.h"
#include "utilities.h"

int readBootSector(FILE *fileImgPtr) {
  int i;
  unsigned char bootSector[512];

  for(i = 0; i < 512; ++i)
    bootSector[i] = getc(fileImgPtr);
    //Check for end of file?

  printHex(bootSector);

  return 0;
}

/*
 * Gather important boot sector info.
 */
int bytesPerSector(unsigned char *bootSector) {
  return 1;
}
int sectorsPerCluster(unsigned char *bootSector) {
  return 1;
}
int reservedSectorCount(unsigned char *bootSector) {
  return 1;
}
int numberOfFATS(unsigned char *bootSector) {
  return 1;
}
int fatSize32(unsigned char *bootSector) {
  return 1;
}
int rootCluster(unsigned char *bootSector) {
  return 1;
}

void printHex(unsigned char *bootSector) {
  int i;
  printf("Bytes per sector: %d\n", swapBytes(bootSector[11], bootSector[12]));

  //Print contents in hex
  for(i = 0; i < 512; ++i) {
    printf("%02X ", bootSector[i]);

    //Spacing
    if(i > 0 && (i + 1) % 4 == 0)
    printf("  ");
    if(i > 0 && (i + 1) % 24 == 0)
    printf("\n");
  }
}

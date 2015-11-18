#include <stdio.h>
#include "bootsector.h"
#include "utilities.h"

unsigned int bytesPerSector(unsigned char *bootSector) {
  return swapTwoBytes(bootSector[11], bootSector[12]);
}
unsigned int sectorsPerCluster(unsigned char *bootSector) {
  return bootSector[13];
}
unsigned int reservedSectorCount(unsigned char *bootSector) {
  return swapTwoBytes(bootSector[14], bootSector[15]);
}
unsigned int numberOfFATS(unsigned char *bootSector) {
  return bootSector[16];
}
unsigned int fatSize32(unsigned char *bootSector) {
  return swapFourBytes(bootSector[36], bootSector[37], bootSector[38],
                        bootSector[39]);
}
unsigned int rootCluster(unsigned char *bootSector) {
  return swapFourBytes(bootSector[44], bootSector[45], bootSector[46],
                        bootSector[47]);
}

void printHex(unsigned char *bootSector) {

  //Print contents in hex
  for(int i = 0; i < 512; ++i) {
    printf("%02X ", bootSector[i]);

    //Spacing
    if(i > 0 && (i + 1) % 4 == 0)
    printf("  ");
    if(i > 0 && (i + 1) % 24 == 0)
    printf("\n");
  }
  printf("\n");
}

int readBootSector(FILE *fileImgPtr, unsigned int *bootSectorData) {
  unsigned char bootSector[512];

  for(int i = 0; i < 512; ++i)
    bootSector[i] = getc(fileImgPtr);
    //Check for end of file?

  //Gather important boot sector info.
  bootSectorData[0] = bytesPerSector(bootSector);
  bootSectorData[1] = sectorsPerCluster(bootSector);
  bootSectorData[2] = reservedSectorCount(bootSector);
  bootSectorData[3] = numberOfFATS(bootSector);
  bootSectorData[4] = fatSize32(bootSector);
  bootSectorData[5] = rootCluster(bootSector);

  printHex(bootSector);

  return 0;
}

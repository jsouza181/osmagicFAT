#include <stdio.h>
#include "bootsector.h"

int readBootSector(FILE *fileImgPtr) {
  int i;
  unsigned char bootSector[512];

  for(i = 0; i < 512; ++i)
    bootSector[i] = getc(fileImgPtr);
    //Check for end of file?

  printHex(bootSector);

  return 0;
}

void printHex(unsigned char *bootSector) {
  int i;

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

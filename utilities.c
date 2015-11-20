#include <stdio.h>

//Find the value of two bytes swapped for littleEndian and "concatenated".
unsigned int swapTwoBytes(unsigned int byteA, unsigned int byteB) {
  unsigned int ret;
  ret = byteB << 8;
  ret = ret | byteA;

  return ret;
}

//Same as swapTwoBytes but applied to four bytes.
unsigned int swapFourBytes(unsigned int byteA, unsigned int byteB,
                            unsigned int byteC, unsigned int byteD) {
  unsigned int ret;
  ret = 0;

  ret = byteD;
  ret = ret << 8;
  ret = ret | byteC;
  ret = ret << 8;
  ret = ret | byteB;
  ret = ret << 8;
  ret = ret | byteA;

  return ret;
}

/*
 * Testing function. Used to display the desired sector of the file system image
 * in hex format. offset is the byte to start reading from.
*/
void printSector(FILE *fileImgPtr, int offset, int sectorSize) {
  // Save original position of the file pointer.
  int originalPos;
  originalPos = ftell(fileImgPtr);

  // Seek to the desired byte
  fseek(fileImgPtr, offset, SEEK_SET);

  //Print contents of sector in hex
  for(int i = 0; i < sectorSize; ++i) {
    printf("%02X ", getc(fileImgPtr));

    //Spacing
    if(i > 0 && (i + 1) % 4 == 0)
    printf("  ");
    if(i > 0 && (i + 1) % 16 == 0)
    printf("\n");
  }
  printf("\n");

  // Restore original position of file pointer.
  fseek(fileImgPtr, originalPos, SEEK_SET);

}

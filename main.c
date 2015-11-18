#include <stdio.h>

int main(int argc, char *argv[]) {
  FILE *fileImgPtr;
  unsigned char bootSector[512];
  int i;

  //Check for correct number of arguments.
  if(argc != 2) {
    printf("Error: Incorrect number of arguments.\n");
    printf("Expected: osmagicFAT <file image>\n");
    return 1;
  }

  //Open the file image.
  fileImgPtr = fopen(argv[1], "rb");
    if(fileImgPtr == NULL) {
      printf("Error: could not open file image\n.");
      return 1;
    }

  //Read the boot sector
  for(i = 0; i < 512; ++i)
    bootSector[i] = getc(fileImgPtr);
    //Check for end of file?

  //Close the file image.
  fclose(fileImgPtr);

  //Print contents in hex
  for(i = 0; i < 512; ++i) {
    printf("%02X ", bootSector[i]);

    //Spacing
    if(i > 0 && (i + 1) % 4 == 0)
    printf("  ");
    if(i > 0 && (i + 1) % 24 == 0)
    printf("\n");
  }

  return 0;
}

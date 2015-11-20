#include <stdio.h>
#include "bootsector.h"

int main(int argc, char *argv[]) {
  FILE *fileImgPtr;

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

  //Read and store the boot sector data.
  readBootSector(fileImgPtr);
  for (int i = 0; i < 16; ++i)
    printf("Boot Sector Data %d: %d\n", i, fsMetadata[i]);

  //Close the file image.
  fclose(fileImgPtr);

  return 0;
}

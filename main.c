#include <stdio.h>
#include <stdlib.h>
#include "metadata.h"
#include "utilities.h"

int main(int argc, char *argv[]) {
  FILE *fileImgPtr;
  // Array of current directory entries. Each entry represented as char*.
  Directory currentDir;
  // Initialize the array of entries.
  currentDir.dirEntries = (unsigned char **) malloc(0 * sizeof(unsigned char *));
  currentDir.size = 0;

  // Check for correct number of arguments.
  if(argc != 2) {
    printf("Error: Incorrect number of arguments.\n");
    printf("Expected: osmagicFAT <file image>\n");
    return 1;
  }

  // Open the file image.
  fileImgPtr = fopen(argv[1], "rb");
    if(fileImgPtr == NULL) {
      printf("Error: could not open file image\n.");
      return 1;
    }

  // Read and store the boot sector data.
  readBootSector(fileImgPtr);

  /*
  for (int i = 0; i < 16; ++i)
    printf("Boot Sector Data %d: %d\n", i, fsMetadata[i]);
    */

  // Read and store the root directory.
  getDirEntries(fileImgPtr, fsMetadata[ROOT_CLUSTER], &currentDir);

  for(int i = 0; i < currentDir.size; ++i) {
    for(int j = 0; j < 32; ++j) {
      printf("%c ", currentDir.dirEntries[i][j]);
    }
    printf("\n");
  }
  printf("\n");

  // Close the file image.
  fclose(fileImgPtr);

  // Free the dynamically allocated current directory.
  for(int i = 0; i < currentDir.size; ++i)
    free(currentDir.dirEntries[i]);

  free(currentDir.dirEntries);

  return 0;
}

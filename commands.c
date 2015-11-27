#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "direntries.h"

// Find the names of each file/directory in the given directory and print.
void ls(Directory dir) {
  // Short name maximum of 11 characters
  char entryName[11];

  for(int i = 0; i < dir.size; ++i) {
    getShortName(dir.dirEntries[i], entryName);
    if(i % 4 == 0 && i != 0) // newline spacing
      printf("\n%s ", entryName);
    else
      printf("%s ", entryName);
  }
  printf("\n");
}

void cd(char *dirName) {
  // Free old current directory.
  // Free the dynamically allocated current directory.
  /*for(int i = 0; i < currentDir.size; ++i)
    free(currentDir.dirEntries[i]);

  free(currentDir.dirEntries);
  */
}

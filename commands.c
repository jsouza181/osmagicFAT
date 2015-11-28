#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "direntries.h"

// Find the names of each file/directory in the given directory and print.
void ls(Directory dir) {
  // Short name maximum of MAX_ENTRY characters (11 for name, 1 for '.', 1 for \0)
  int MAX_ENTRY = 13;
  int i = 0;
  int spaceInt = 0;
  char spacing[MAX_ENTRY];
  char entryName[MAX_ENTRY];

  for(i = 0; i < dir.size; ++i) {
    getShortName(dir.dirEntries[i], entryName);

    if(i % 4 == 0 && i != 0) // newline spacing
      printf("\n%s ", entryName);
    else {
      printf("%s ", entryName);
    }
    // printf("size: %d", (int)strlen(entryName));
    for (spaceInt = 0; spaceInt < MAX_ENTRY - (int)strlen(entryName); ++spaceInt) {
      spacing[spaceInt] = ' ';
    } // for
    spacing[spaceInt] = '\0';
    printf("%s", spacing);
  }
  printf("\n");
}

// Return the first cluster of the provided directory.
int cd(Directory currentDir, char *targetDir, unsigned int *clusterNum) {
  // Check if the provided name exists in the current directory.
  if(findFilenameCluster(currentDir, targetDir, clusterNum)) {
    return 1;
  }
  else {
    return 0;
  }

}

//How much space is left in cluster to write?
//Mod file size with number of 512 bytes

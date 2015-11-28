#include <string.h>
#include <stdio.h>
#include "direntries.h"
#include "metadata.h"

void getShortName(unsigned char *dirEntry, char* entryName) {
  // Read the filename as a null term string.
  entryName[8] = '\0';
  for(int i = 0; i < 8; ++i) {
    if(dirEntry[i] != ' ')
      entryName[i] = dirEntry[i];
    else {
      entryName[i] = '\0';
      break;
    }
  }

  // If the 9th character is not whitespace, there is an extension
  if(dirEntry[8] != ' ') {
    // Add a dot, and the extension, then add a new null termination.
    char extension[5];
    extension[0] = '.';
    extension[1] = dirEntry[8];
    extension[2] = dirEntry[9];
    extension[3] = dirEntry[10];
    extension[4] = '\0';

    // Concat the extension to the filename
    strcat(entryName, extension);
  }
}

int isDirectory(unsigned char *dirEntry) {
  unsigned char dirAttribute = dirEntry[11];

  if((dirAttribute | 0xEF) == 0xFF)
    return 1;
  else
    return 0;
}

// Find the first cluster number of a given directory entry.
unsigned int getFirstCluster(unsigned char *dirEntry) {
  unsigned int highWord;
  unsigned int lowWord;

  highWord = swapTwoBytes(dirEntry[20], dirEntry[21]);
  lowWord = swapTwoBytes(dirEntry[26], dirEntry[27]);

  highWord = highWord << 16;
  highWord = highWord | lowWord;

  printf("result is: %d\n", highWord);
  // If the resulting cluster number is zero, return the root directory.
  if(highWord == 0)
    return fsMetadata[ROOT_CLUSTER];
  else
    return highWord;
}

// Given a filename, find it in the current directory.
// If it is not in the current directory, return 0.
// Otherwise, return the file/dir's first cluster number.
int findFilenameCluster(Directory currentDir, char *filename,
                        unsigned int *clusterNum) {
  // Temp filename built from each directory entry.
  char tempString[13];
  // Capitalize the user-given filename for strcmp().
  capFilename(filename);

  // Look through each directory entry of the current directory.
  for(int i = 0; i < currentDir.size; ++i) {
    getShortName(currentDir.dirEntries[i], tempString);

    // Match is found.
    if(strcmp(tempString, filename) == 0) {
      // Check if the provided name is a directory.
      if(!isDirectory(currentDir.dirEntries[i])){
        printf("%s is not a directory.\n", filename);
        return 0;
      }
      *clusterNum = getFirstCluster(currentDir.dirEntries[i]);
      return 1;
    }
  }
  // Filename wasn't found.
  printf("%s not found.\n", filename);
  return 0;
}

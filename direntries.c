#include <string.h>
#include <stdio.h>
#include "direntries.h"

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

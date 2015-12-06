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

unsigned int getFileSize(unsigned char *dirEntry) {
  return swapFourBytes(dirEntry[28], dirEntry[29], dirEntry[30], dirEntry[31]);
}

/*
 * This function sets the size attribute of a specified directory entry.
 * currentDirCluster is the first cluster of the current directory.
 * targetEntry is the entry to be updated.
 * newBytes is the number of bytes to set as the total size.
 */
void setFileSize(FILE *fileImgPtr, unsigned int currentDirCluster,
                unsigned char *targetEntry, int newSize) {

  unsigned char entry[32];
  unsigned int nextCluster;

  // Break the file's size into 4 characters.
  unsigned char sizeA = (newSize & 0xFF000000) >> 24;
  unsigned char sizeB = (newSize & 0x00FF0000) >> 16;
  unsigned char sizeC = (newSize & 0x0000FF00) >> 8;
  unsigned char sizeD = (newSize & 0x000000FF);

  nextCluster = currentDirCluster;

  // Loop until the end of cluster chain is reached or the entry is found.
  do {
    // Seek to the cluster's first sector.
    fseek(fileImgPtr, (getSector(nextCluster) * fsMetadata[BYTES_PER_SECTOR]),
          SEEK_SET);

    // Loop once for each directory entry that can fit in the sector.
    for(int i = 0; i < 16; ++i) {
      // Read in a directory entry.
      for(int j = 0; j < 32; ++j) {
        entry[j] = getc(fileImgPtr);
      }

      // If the entry is a long name entry, do not add it.
      if((entry[11] | 0xF0) == 0xFF)
        continue;
      // If the entry is empty, do not add it.
      if(entry[0] == 0xE5)
        continue;
      // If the entry is free, and is the last entry, end the entire function.
      if(entry[0] == 0x00)
        return;

      // Compare the directory entries. If they have the same first cluster
      // numbers, they are the same entry.
      if(targetEntry[20] == entry[20] && targetEntry[21] == entry[21] &&
        targetEntry[26] == entry[26] && targetEntry[27] == entry[27]) {
        // This is the entry whose size we want to update.
        // Move the file pointer back 4 bytes and write the new size.
        fseek(fileImgPtr, -4, SEEK_CUR);
        putc(sizeD, fileImgPtr);
        putc(sizeC, fileImgPtr);
        putc(sizeB, fileImgPtr);
        putc(sizeA, fileImgPtr);

        return;
      }
    }

    // If next cluster not EOC, set next cluster to next cluster in chain.
    nextCluster = getNextCluster(fileImgPtr, nextCluster);
  } while(nextCluster < EOCMIN);
}

void setShortName(unsigned char* dirEntry, char* entryName) {
  // set all to whitespace
  for (size_t i = 0; i < 11; ++i) {
    dirEntry[i] = ' ';
  }
  for (size_t i = 0; i < 8; ++i) {
    if (entryName[i] != ' ' && entryName[i] != '\0') {
      // handle the extension
      if (entryName[i] == '.') {
        dirEntry[8] = entryName[i+1];
        dirEntry[9] = entryName[i+2];
        dirEntry[10] = entryName[i+3];
        break;
      } // if
      else {
        dirEntry[i] = entryName[i];
      } // else
      printf("direntry[%d]: %c\n",(int)i,dirEntry[i]);
    } // if
    else {
      break;
    } // else
  } // for
} // setShortName

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

  // If the resulting cluster number is zero, return the root directory.
  if(highWord == 0)
    return fsMetadata[ROOT_CLUSTER];
  else
    return highWord;
}

// Given a filename, find it in the current directory.
// If it is not in the current directory, return 0.
// Otherwise, set the file/dir's first cluster number.
int findFilenameCluster(Directory currentDir, char *filename,
                        unsigned int *clusterNum, int *indexPtr) {
  // Temp filename built from each directory entry.
  char tempString[13];

  // Look through each directory entry of the current directory.
  for(int i = 0; i < currentDir.size; ++i) {
    getShortName(currentDir.dirEntries[i], tempString);

    // Match is found.
    if(strcmp(tempString, filename) == 0) {
      *clusterNum = getFirstCluster(currentDir.dirEntries[i]);
      *indexPtr = i;
      return 1;
    }
  }
  // Filename wasn't found.
  printf("%s not found.\n", filename);
  return 0;
}

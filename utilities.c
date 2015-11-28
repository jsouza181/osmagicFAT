/*
 * utilities.c contains functions to perform calculations that are frequently
 * needed across the entire program.
 */

#include <stdio.h>
#include <stdlib.h>
#include "metadata.h"
#include "utilities.h"

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
 * in hex format.
*/
void printSector(FILE *fileImgPtr, unsigned int sectorNum) {
  // Save original position of the file pointer.
  int originalPos;
  originalPos = ftell(fileImgPtr);

  // Seek to the desired byte
  fseek(fileImgPtr, (sectorNum * fsMetadata[BYTES_PER_SECTOR]), SEEK_SET);

  //Print contents of sector in hex
  for(int i = 0; i < fsMetadata[BYTES_PER_SECTOR]; ++i) {
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

// Find the sector number of the first sector of a given cluster.
unsigned int getSector(unsigned int clusterNumber) {
  return ((clusterNumber - 2) * fsMetadata[SECTORS_PER_CLUSTER])
          + fsMetadata[FIRST_DATA_SECTOR];
}

/*
 * Find the value of a given cluster's FAT table entry.
 * If this is not EoC, then the given cluster number has more
 * clusters after it that are part of the cluster chain.
 */
unsigned int getNextCluster(FILE *fileImgPtr, unsigned int clusterNumber) {
  unsigned int ret, byteA, byteB, byteC, byteD;
  unsigned int fatOffset;
  unsigned int fatSectorNumber;
  unsigned int fatEntryOffset;

  // FAT uses 4byte entries
  fatOffset = clusterNumber * 4;

  // First, find the sector within the FAT.
  fatSectorNumber = fsMetadata[RESERVED_SECTOR_COUNT] +
                    (fatOffset / fsMetadata[BYTES_PER_SECTOR]);

  // Then, find the 4byte integer within the sector.
  fatEntryOffset = fatOffset % fsMetadata[BYTES_PER_SECTOR];

  // Now, fseek to the sector, and then to the offset.
  fseek(fileImgPtr, (fatSectorNumber * fsMetadata[BYTES_PER_SECTOR]), SEEK_SET);
  fseek(fileImgPtr, fatEntryOffset, SEEK_CUR);

  // Finally, read 4 bytes into an int.
  byteA = getc(fileImgPtr);
  byteB = getc(fileImgPtr);
  byteC = getc(fileImgPtr);
  byteD = getc(fileImgPtr);

  ret = swapFourBytes(byteA, byteB, byteC, byteD);

  // First four bits of the int are reserved, so ignore them.
  return ret & 0x0FFFFFFF;
}

/*
 * Read the sector(s) of the given directory cluster number.
 * Create a new directory entry (char*) for each directory entry.
 * Add these entries to the Directory object passed in.
 */
void getDirEntries(FILE *fileImgPtr, unsigned int clusterNumber, Directory *dir) {
  unsigned char entry[32];
  unsigned int currentCluster;

  currentCluster = clusterNumber;
  //printSector(fileImgPtr, getSector(clusterNumber));

  // Add the directory entries to the array.
  // Loop until the end of cluster chain is reached.
  do {
    // Seek to the cluster's first sector.
    fseek(fileImgPtr, (getSector(currentCluster) * fsMetadata[BYTES_PER_SECTOR]),
          SEEK_SET);
    //printf("we are at: %02x\n", 512 * getSector(currentCluster));

    // Loop once for each directory entry that can fit in the sector.
    for(int i = 0; i < 16; ++i) {
      // Read in a directory entry.
      for(int j = 0; j < 32; ++j) {
        entry[j] = getc(fileImgPtr);
      }
      //printf("%s ", entry);

      // If the entry is a long name entry, do not add it.
      if((entry[11] | 0xF0) == 0xFF)
        continue;
      // If the entry is empty, do not add it.
      if(entry[0] == 0xE5)
        continue;
      // If the entry is free, and is the last entry, end the entire function.
      if(entry[0] == 0x00)
        return;

      // Allocate space for the new entry.
      dir->dirEntries = (unsigned char **) realloc(dir->dirEntries,
                        (dir->size + 1) * sizeof(char*));
      // Allocate space for each character in the entry.
      dir->dirEntries[dir->size] = (unsigned char*) malloc(32 * sizeof(unsigned char));

      // Add the entry to the array
      for (int i = 0; i < 32; ++i) {
        dir->dirEntries[dir->size][i] = entry[i];
      }

      // Update the array size.
      dir->size = dir->size + 1;

    }

    // If next cluster not EOC, set current cluster to next cluster in chain.
    currentCluster = getNextCluster(fileImgPtr, currentCluster);
    //printf("current cluster is now: %02x\n", currentCluster);
  } while(currentCluster != EOC);

}

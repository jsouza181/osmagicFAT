#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "direntries.h"
#include "metadata.h"
#include "utilities.h"

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
    for (spaceInt = 0; spaceInt < MAX_ENTRY - (int)strlen(entryName); ++spaceInt) {
      spacing[spaceInt] = ' ';
    }
    spacing[spaceInt] = '\0';
    printf("%s", spacing);
  }
  printf("\n");
}

// Find the given directory in the current directory.
// Return 0 if the provided name is not a directory.
int cd(Directory currentDir, char *targetDir, unsigned int *clusterNum) {
  // Index of the found file in the directory entry table.
  // Used to access the file's dir entry.
  int fileIndex;
  int *indexPtr = &fileIndex;
  capFilename(targetDir);

  // Check if the provided name exists in the current directory.
  if(findFilenameCluster(currentDir, targetDir, clusterNum, indexPtr)) {
    // Check if the provided name is a directory.
    if(!isDirectory(currentDir.dirEntries[*indexPtr])){
      printf("%s is not a directory.\n", targetDir);
      return 0;
    }
    return 1;
  }
  else {
    // File not found.
    return 0;
  }
}

// Add a file to the open file table.
// If the file is already opened, or if it is not found, or if it is a dir, return 0.
int open(Directory currentDir, FILE *fileImgPtr, OpenFileTable *ofTable,
          char *targetFile, int flag) {
  // File's first cluster (used to build and then store cluster chain).
  unsigned int firstCluster = 0;
  unsigned int *firstClusterPtr = &firstCluster;
  // Index of the found file in the directory entry table.
  // Used to access the file's dir entry.
  int fileIndex = 0;
  int *indexPtr = &fileIndex;
  capFilename(targetFile);

  // Check if the file already exists in the open file table.
  for(int i = 0; i < ofTable->size; ++i) {
    if(strcmp(ofTable->entries[i].filename, targetFile) == 0) {
      printf("Error: file is already opened.\n");
      return 0;
    }
  }

  // Check if the file exists in the current directory.
  if(findFilenameCluster(currentDir, targetFile, firstClusterPtr, indexPtr)) {
    // Check if the provided name is a directory.
    if(isDirectory(currentDir.dirEntries[*indexPtr])){
      printf("Error: %s is a directory.\n", targetFile);
      return 0;
    }
    // Allocate space for a new open file entry.
    ofTable->entries = (OpenFileEntry *) realloc(ofTable->entries,
                        (ofTable->size + 1) * sizeof(OpenFileEntry));

    // Create the new open file entry.
    // Set the entry's data.
    ofTable->entries[ofTable->size].flag = flag;
    strcpy(ofTable->entries[ofTable->size].filename, targetFile);
    // Allocate the entry's cluster chain.
    ofTable->entries[ofTable->size].clusterOffsets = (unsigned int*) malloc(sizeof(unsigned int));
    ofTable->entries[ofTable->size].clusterCount = 0;

    // Populate the cluster chain.
    unsigned int nextCluster = firstCluster;
    while(nextCluster < EOCMIN) {
      // Allocate memory for the next cluster.
      ofTable->entries[ofTable->size].clusterOffsets =
        (unsigned int*) realloc(ofTable->entries[ofTable->size].clusterOffsets,
                                (ofTable->entries[ofTable->size].clusterCount + 1)
                                  * sizeof(unsigned int));

      // Set the cluster number.
      ofTable->entries[ofTable->size].clusterOffsets
          [ofTable->entries[ofTable->size].clusterCount] = nextCluster;

      // Increment the cluster count.
      ofTable->entries[ofTable->size].clusterCount =
          ofTable->entries[ofTable->size].clusterCount + 1;

      // Find the next cluster associated with the entry.
      nextCluster = getNextCluster(fileImgPtr, nextCluster);
    }

    // Update the entry table size.
    ofTable->size = ofTable->size + 1;

    // Create a new open file entry.
    // Add the entry to the table.
    // Increment the size of the table.
    return 1;
  }
  else
    // File not found.
    return 0;
}

// Remove a file from the open file table.
// Return 0 if the file isn't in the "opened files" table.
int closeFile(OpenFileTable *ofTable, char * targetFile) {
  capFilename(targetFile);
  // Check if the file is in the table.
  for(int i = 0; i < ofTable->size; ++i) {
    if(strcmp(targetFile, ofTable->entries[i].filename) == 0) {
      // Remove the found entry from the open file table.
      // Shift over every open file entry after this one.
      for(int j = i; j < (ofTable->size - 1); ++j) {
        ofTable->entries[j] = ofTable->entries[j + 1];
      }
      // Decrement the file entry's size.
      ofTable->size = ofTable->size - 1;
      return 1;
    }
  }
  return 0;

}

// Given a filename, print the bytes starting at position, until numBytes.
int readFile(Directory currentDir, OpenFileTable *ofTable, FILE *fileImgPtr,
            char *targetFile, int position, int numBytes) {
  OpenFileEntry *targetFileEntry;
  int bytesPerCluster = fsMetadata[BYTES_PER_SECTOR] * fsMetadata[SECTORS_PER_CLUSTER];

  capFilename(targetFile);
  // Check if the file exists in the open files table.
  for(int i = 0; i < ofTable->size; ++i) {
    if(strcmp(targetFile, ofTable->entries[i].filename) == 0) {
      targetFileEntry = &ofTable->entries[i];
      break;
    }
    // File not found.
    else if((i + 1) == ofTable->size || ofTable->size == 0) {
      printf("Error: File has not been opened\n");
      return 0;
    }
  }
  // Check if the file has read privileges.
  if(targetFileEntry->flag == WRITE) {
    printf("Error: File not opened for read.\n");
    return 0;
  }

  // Check if the number of the position is greater than the file size.
  if(position >= size(currentDir, targetFile)) {
    printf("Error: Invalid read starting position.\n");
    return 0;
  }
  // Check if the desired number of bytes to read does not exceed EOF.
  if(size(currentDir, targetFile) - numBytes < position) {
    printf("Error: Attempt to read beyond EOF.\n");
    return 0;
  }

  // Next cluster in the file's cluster chain to read.
  unsigned int nextCluster;
  // Byte to read and print.
  unsigned char nextByte;
  // Track the number of bytes to read.
  int byteCounter = numBytes;
  // Offsets for starting from a specified byte position.
  int targetCluster, targetByte;

  // Seek the file pointer to the desired position's cluster.
  targetCluster = position / bytesPerCluster;
  nextCluster = targetFileEntry->clusterOffsets[targetCluster];
  fseek(fileImgPtr, (getSector(nextCluster) * fsMetadata[BYTES_PER_SECTOR]),
        SEEK_SET);
  // Seek the file pointer to the desired position's byte.
  targetByte = position % bytesPerCluster;
  fseek(fileImgPtr, targetByte, SEEK_CUR);

  // Read the first requested cluster.
  while(targetByte < bytesPerCluster) {
    if(byteCounter == 0) { // Finished reading requested number of bytes.
      printf("\n");
      return 1;
    }

    nextByte = getc(fileImgPtr);
    printf("%c ", nextByte);

    byteCounter = byteCounter - 1;
    targetByte = targetByte + 1;
  }

  // Read the remaining clusters.
  nextCluster = getNextCluster(fileImgPtr, nextCluster);
  while(nextCluster < EOCMIN) {
    // Seek the file pointer to the first byte of the next cluster.
    fseek(fileImgPtr, (getSector(nextCluster) * fsMetadata[BYTES_PER_SECTOR]),
          SEEK_SET);

    for(int i = 0; i < bytesPerCluster; ++i) {
      if(byteCounter == 0) { // Finished reading requested number of bytes.
        printf("\n");
        return 1;
      }

      nextByte = getc(fileImgPtr);
      printf("%c ", nextByte);

      byteCounter = byteCounter - 1;
    }
    nextCluster = getNextCluster(fileImgPtr, nextCluster);
  }

  return 1;
}

int size(Directory currentDir, char *targetFile) {
  int index;
  unsigned int firstCluster, fileSize;
  capFilename(targetFile);

  // Check if the file exists in the current directory.
  if(findFilenameCluster(currentDir, targetFile, &firstCluster, &index)) {
    fileSize = getFileSize(currentDir.dirEntries[index]);
    return fileSize;
  }
  else {
    printf("Error: File not found in current directory.\n");
    return 0;
  }
}

// Given a filename, print the bytes starting at position, until numBytes.
int writeFile(Directory currentDir, unsigned int currentDirCluster,
            OpenFileTable *ofTable, FILE *fileImgPtr, char *targetFile,
            int position, int numBytes, char *stringToWrite) {
  OpenFileEntry *targetFileEntry;
  int bytesPerCluster = fsMetadata[BYTES_PER_SECTOR] * fsMetadata[SECTORS_PER_CLUSTER];

  int stringCounter = 1;

  capFilename(targetFile);

  // Check if the file exists in the open files table.
  for(int i = 0; i < ofTable->size; ++i) {
    if(strcmp(targetFile, ofTable->entries[i].filename) == 0) {
      targetFileEntry = &ofTable->entries[i];
      break;
    }
    // File not found.
    else if((i + 1) == ofTable->size || ofTable->size == 0) {
      printf("Error: File has not been opened\n");
      return 0;
    }
  }

  // Check if the file has write privileges.
  if(targetFileEntry->flag == READ) {
    printf("Error: File not opened for write.\n");
    return 0;
  }

  // Next cluster in the file's cluster chain to write to.
  unsigned int nextCluster;
  // Byte to read and print.
  unsigned char nextByte;
  // Track the number of bytes to write.
  int byteCounter = numBytes;
  // Offsets for starting from a specified byte position.
  int targetCluster, targetByte;

  int index, bytesAdded;
  unsigned int firstCluster;
  findFilenameCluster(currentDir, targetFile, &firstCluster, &index);

  // Check if the desired bytes to write exceed EOF.
  if(size(currentDir, targetFile) - numBytes < position) {
    // Allocate clusters as needed to make write valid.

    // Increase the file's size attribute to reflect the bytes we are adding.
    unsigned int newSize = position + numBytes;
    setFileSize(fileImgPtr, currentDirCluster, currentDir.dirEntries[index],
                newSize);

    // Calculate the number of clusters needed to start writing at position.
    int newClusterCount = (newSize / bytesPerCluster) + 1;

    // Allocate any additional clusters needed.
    unsigned int lastCluster = targetFileEntry->clusterOffsets[targetFileEntry->clusterCount - 1];
    unsigned int newCluster;

    for(int i = newClusterCount - targetFileEntry->clusterCount; i > 0; --i) {
      printf("allocating new cluster\n");
      newCluster = getNewCluster(fileImgPtr);
      // Add the new cluster to open file entry's cluster chain.
      // Allocate memory for the new cluster.
      targetFileEntry->clusterOffsets =
        (unsigned int*) realloc(targetFileEntry->clusterOffsets,
                                (targetFileEntry->clusterCount + 1)
                                  * sizeof(unsigned int));
      // Set the cluster number.
      targetFileEntry->clusterOffsets[targetFileEntry->clusterCount] = newCluster;
      // Increment the cluster count.
      targetFileEntry->clusterCount = targetFileEntry->clusterCount + 1;

      linkClusters(fileImgPtr, lastCluster, newCluster);
      lastCluster = newCluster;
    }

  }
  else {
  bytesAdded = numBytes - (size(currentDir, targetFile) - position);
  setFileSize(fileImgPtr, currentDirCluster, currentDir.dirEntries[index],
              bytesAdded);
  }

  // Seek the file pointer to the desired position's cluster.
  targetCluster = position / bytesPerCluster;
  nextCluster = targetFileEntry->clusterOffsets[targetCluster];
  fseek(fileImgPtr, (getSector(nextCluster) * fsMetadata[BYTES_PER_SECTOR]),
        SEEK_SET);
  // Seek the file pointer to the desired position's byte.
  targetByte = position % bytesPerCluster;
  fseek(fileImgPtr, targetByte, SEEK_CUR);

  // Write to the first requested cluster.
  while(targetByte < bytesPerCluster) {
    if(byteCounter == 0) { // Finished reading requested number of bytes.
      printf("\n");
      return 1;
    }

      // Write the next character of the string.
      nextByte = stringToWrite[stringCounter];
      if(nextByte == '\"') {
        stringCounter = stringCounter + 1;
        nextByte = stringToWrite[stringCounter];
      }
      stringCounter = stringCounter + 1;
      if(stringToWrite[stringCounter] == '\"') {
        stringCounter = 1;
      }
      putc(nextByte, fileImgPtr);


    byteCounter = byteCounter - 1;
    targetByte = targetByte + 1;
  }

  // Write the remaining clusters.
  nextCluster = getNextCluster(fileImgPtr, nextCluster);
  while(nextCluster < EOCMIN) {
    // Seek the file pointer to the first byte of the next cluster.
    fseek(fileImgPtr, (getSector(nextCluster) * fsMetadata[BYTES_PER_SECTOR]),
          SEEK_SET);

    for(int i = 0; i < bytesPerCluster; ++i) {
      if(byteCounter == 0) { // Finished reading requested number of bytes.
        printf("\n");
        return 1;
      }

      // Write the next character of the string.
      nextByte = stringToWrite[stringCounter];
      if(nextByte == '\"') {
        stringCounter = stringCounter + 1;
        nextByte = stringToWrite[stringCounter];
      }
      stringCounter = stringCounter + 1;
      if(stringToWrite[stringCounter] == '\"') {
        stringCounter = 1;
      }
      putc(nextByte, fileImgPtr);

      byteCounter = byteCounter - 1;
    }
    nextCluster = getNextCluster(fileImgPtr, nextCluster);
  }
  return 0;
}

// creates a directory or a file depending on the isDir flag: 0 is for file, > 0 is for directory
int create(Directory currentDir, unsigned int currentDirCluster, FILE *fileImgPtr,
    char* filename, int isDir) {
  fpos_t pos;
  unsigned char newEntry[ENTRY_BYTES];
  unsigned char tmpEntry[ENTRY_BYTES];
  unsigned char byteA, byteB, byteC, byteD;
  unsigned int clusterNum = 0;
  unsigned int entered = 0;
  // File's first cluster (used to build and then store cluster chain).
  unsigned int firstCluster = 0;
  unsigned int *firstClusterPtr = &firstCluster;
  unsigned int currentCluster = currentDirCluster;
  // Index of the found file in the directory entry table.
  // Used to access the file's dir entry.
  int fileIndex = 0;
  int *indexPtr = &fileIndex;
  // int bytesPerCluster = fsMetadata[BYTES_PER_SECTOR] * fsMetadata[SECTORS_PER_CLUSTER];
  // int entriesPerCluster = bytesPerCluster / ENTRY_BYTES;
  // int totalEntries = currentDir.size;

  if (filename != NULL) {
    capFilename(filename);

    // check if filename exists in current directory
    if (findFilenameCluster(currentDir, filename, firstClusterPtr, indexPtr)) {
      printf("Error: a file or directory with that name already exists\n");
      return 0;
    }
    else {
      // lets set all bytes to 0 first
      for (size_t i = 0; i < ENTRY_BYTES; ++i) {
        newEntry[i] = 0x00;
      }
      setShortName(newEntry, filename);

      // get the next cluster number
      clusterNum = getNewCluster(fileImgPtr);
      // Separate clusterNum into four bytes.
      if (0 != clusterNum) {
        byteA = (clusterNum & 0xFF000000) >> 24;
        byteB = (clusterNum & 0x00FF0000) >> 16;
        byteC = (clusterNum & 0x0000FF00) >> 8;
        byteD = (clusterNum & 0x000000FF);

        newEntry[20] = byteB;
        newEntry[21] = byteA;
        newEntry[26] = byteD;
        newEntry[27] = byteC;
      } // if
      else {
        printf("Unable to allocate cluster for new entry\n");
        return 0;
      }

      if (isDir) {
        printf("Creating directory as %s\n", filename);
        newEntry[11] = 0x10; // set all bits to 0 except directory bit
      } // if
      else {
        printf("Creating file as %s\n", filename);
        // create a new entry in the next available cluster
        newEntry[11] = 0x00;
      } // else

      // write to the cluster
      do {
        fseek(fileImgPtr, (getSector(currentCluster) * fsMetadata[BYTES_PER_SECTOR]),
              SEEK_SET);
        // iterate through entries
        for (size_t i = 0; i < NUM_ENTRIES_PER_CLUSTER; ++i) {
          // save position
          fgetpos(fileImgPtr, &pos);
          // iterate through entry data
          for (size_t j = 0; j < ENTRY_BYTES; ++j) {
            tmpEntry[j] = getc(fileImgPtr);
          } // for
          // check if entry is empty, if so enter data
          if (tmpEntry[0] == 0xE5 || tmpEntry[0] == 0x00) {
            fsetpos(fileImgPtr, &pos);
            // overwrite entry with created one
            for (size_t j = 0; j < ENTRY_BYTES; ++j) {
              putc(newEntry[j], fileImgPtr);
            } // for
            entered = 1;
            break; // done, exit loop
          } // if empty
        } // for

        if (!entered) {
          currentCluster = getNextCluster(fileImgPtr, currentCluster);
          if (currentCluster >= EOCMIN ) {
            printf("Allocating new cluster\n");
            // need to allocate a new cluster
            currentCluster = getNewCluster(fileImgPtr);
            if (0 != currentCluster) {
              // link both clusters if it was able to allocate a new one
              linkClusters(fileImgPtr, currentDirCluster, currentCluster);
              fgetpos(fileImgPtr, &pos);
              fseek(fileImgPtr, (getSector(currentCluster) * fsMetadata[BYTES_PER_SECTOR]),
                    SEEK_SET);
              for (size_t i = 0; i < NUM_ENTRIES_PER_CLUSTER; ++i) {
                if (i == NUM_ENTRIES_PER_CLUSTER - 1) {
                  putc(0x00, fileImgPtr);
                }
                else {
                  putc(0xE5, fileImgPtr);
                } // else
              } // for
              fsetpos(fileImgPtr, &pos);
            }
            else {
              printf("Unable to allocate new cluster in current directory chain\n");
            }
          }
        }
      } while (currentCluster < EOCMIN && !entered);

      return 1;
    } // else
  } // if
  else {
    return 0;
  }
} // create


int rm(Directory currentDir, unsigned int currentDirCluster, FILE *fileImgPtr,
        OpenFileTable *ofTable, char *targetFile, int flag){

  unsigned int firstCluster = 0;
  unsigned int *firstClusterPtr = &firstCluster;

  int fileIndex = 0;
  int *indexPtr = &fileIndex;
  capFilename(targetFile);

  for(int i =0; i < ofTable->size; ++i){
    if(strcmp(ofTable->entries[i].filename, targetFile) == 0)
      printf("Error. The file is currently open.\n");
      return 0;
  }

  if(findFilenameCluster(currentDir,targetFile, firstClusterPtr, indexPtr)){
    if (isDirectory(currentDir.dirEntries[*indexPtr])){
      printf("%s is a directory.\n", targetFile);
      return 0;
    }

    OpenFileEntry fileEntry;
    fileEntry.flag = flag;
    fileEntry.clusterOffsets = (unsigned int*) malloc(sizeof(unsigned int));
    fileEntry.clusterCount = 0;

    unsigned int nextCluster = firstCluster;
    while (nextCluster < EOCMIN){
      fileEntry.clusterOffsets = (unsigned int*) realloc(fileEntry.clusterOffsets,
          (fileEntry.clusterCount + 1) * sizeof(unsigned int));

      // Set the cluster number.
      fileEntry.clusterOffsets[fileEntry.clusterCount] = nextCluster;

      // Increment the cluster count.
      fileEntry.clusterCount =  fileEntry.clusterCount + 1;

      // Find the next cluster associated with the entry.
      nextCluster = getNextCluster(fileImgPtr, nextCluster);
    }

    // Reverse through Cluster to zero out bytes
    for(int i = fileEntry.clusterCount; i > -1; --i){
      int clusterNum = fileEntry.clusterOffsets[i];
      setCluster(fileImgPtr, clusterNum, 0, 0, 0, 0);
    }

    // Remove file from Directory entry
    rmDirEntries(fileImgPtr, currentDirCluster, &currentDir, targetFile, 0);

    return 1;
  }

  else{
    printf("File not found.\n");
    return 0;
  }

}

int rmDirectory(Directory currentDir, unsigned int currentDirCluster, FILE *fileImgPtr,
                char *targetFile, int flag){

  unsigned int firstCluster = 0;
  unsigned int *firstClusterPtr = &firstCluster;
  Directory rmDirectory;

  int fileIndex = 0;
  int *indexPtr = &fileIndex;
  capFilename(targetFile);

  if(findFilenameCluster(currentDir,targetFile, firstClusterPtr, indexPtr)){


    if (!isDirectory(currentDir.dirEntries[*indexPtr])){
      printf("%s is not a directory.\n", targetFile);
      return 0;
    }


    rmDirectory.dirEntries = (unsigned char**)malloc(0*sizeof(unsigned char*));
    rmDirectory.size = 0;

    getDirEntries(fileImgPtr,firstCluster,&rmDirectory);

    if (rmDirectory.size > 2){
      printf("Error! Directory is not empty.\n");
      return 0;
    }

    OpenFileEntry fileEntry;
    fileEntry.flag = flag;
    fileEntry.clusterOffsets = (unsigned int*) malloc(0 * sizeof(unsigned int));
    fileEntry.clusterCount = 0;

    unsigned int nextCluster = firstCluster;
    while (nextCluster < EOCMIN){
      fileEntry.clusterOffsets = (unsigned int*) realloc(fileEntry.clusterOffsets,
          (fileEntry.clusterCount + 1) * sizeof(unsigned int));

      // Set the cluster number.
      fileEntry.clusterOffsets[fileEntry.clusterCount] = nextCluster;

      // Increment the cluster count.
      fileEntry.clusterCount =  fileEntry.clusterCount + 1;

      // Find the next cluster associated with the entry.
      nextCluster = getNextCluster(fileImgPtr, nextCluster);
    }

    // Reverse through Cluster to zero out bytes
    for(int i = fileEntry.clusterCount; i > -1; --i){
        int clusterNum = fileEntry.clusterOffsets[i];
        setCluster(fileImgPtr, clusterNum, 0, 0, 0, 0);
    }

    // Remove file from Directory entry
    rmDirEntries(fileImgPtr, currentDirCluster, &currentDir, targetFile, 1);

    return 1;
  }

  else{
    printf("File not found.\n");
    return 0;
  }

}

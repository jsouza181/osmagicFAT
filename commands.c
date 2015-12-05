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
      printf("%s is a directory.\n", targetFile);
      return 0;
    }
    printf("Found file: %s \n", targetFile);
    // Allocate space for a new open file entry.
    ofTable->entries = (OpenFileEntry *) realloc(ofTable->entries,
                        (ofTable->size + 1) * sizeof(OpenFileEntry));

    // Create the new open file entry.
    // Set the entry's data.
    ofTable->entries[ofTable->size].flag = flag;
    strcpy(ofTable->entries[ofTable->size].filename, targetFile);
    // Allocate the entry's cluster chain.
    ofTable->entries[ofTable->size].clusterOffsets = (unsigned int*) malloc(0 * sizeof(unsigned int));
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

//How much space is left in cluster to write?
//Mod file size with number of 512 bytes

// creates a directory or a file depending on the isDir flag: 0 is for file, > 0 is for directory
int create(Directory currentDir, char* filename, int isDir) {
  // File's first cluster (used to build and then store cluster chain).
  unsigned int firstCluster = 0;
  unsigned int *firstClusterPtr = &firstCluster;
  // Index of the found file in the directory entry table.
  // Used to access the file's dir entry.
  int fileIndex = 0;
  int *indexPtr = &fileIndex;

  if (filename != NULL) {
    capFilename(filename);

    // check if filename exists in current directory
    if(findFilenameCluster(currentDir, filename, firstClusterPtr, indexPtr)) {
      printf("Error: a file or directory with that name already exists\n");
      return 0;
    }
    else {
      if (isDir) {
        printf("Creating directory as %s\n", filename);
        return 1;
      } // if
      else {
        printf("Creating file as %s\n", filename);
        // create a new entry in the next available cluster

        return 1;
      } // else
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
      freeCluster(fileImgPtr,clusterNum);
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
        OpenFileTable *ofTable, char *targetFile, int flag){

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

    printf("Directory size is %d\n", rmDirectory.size);
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
      freeCluster(fileImgPtr,clusterNum);
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

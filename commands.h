#ifndef COMMANDS_H
#define COMMANDS_H

#define READ 0
#define WRITE 1
#define READWRITE 2

#include "utilities.h" // Directory struct

void ls(Directory dir);
int cd(Directory currentDir, char *targetDir, unsigned int *clusterNum);
int open(Directory currentDir, FILE *fileImgPtr, OpenFileTable *ofTable,
          char *targetFile, int flag);
int closeFile(OpenFileTable *ofTable, char * targetFile);

int readFile(Directory currentDir, OpenFileTable *ofTable, FILE *fileImgPtr,
  char *targetFile, int position, int numBytes);
int writeFile(Directory currentDir, unsigned int currentDirCluster,
              OpenFileTable *ofTable, FILE *fileImgPtr, char *targetFile,
              int position, int numBytes, char *stringToWrite);

unsigned int size(Directory currentDir, char *targetFile);


#endif

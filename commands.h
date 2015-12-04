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

// create a directory or a file depending on the isDir flag
// 0 - create a file
// 1 - create a directory
// if the filename is already in use, unable to create it (out of mem), returns 0
int create(Directory currentDir, char* filename, int isDir);

#endif

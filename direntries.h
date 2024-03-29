#ifndef DIRENTRIES_H
#define DIRENTRIES_H

#include "utilities.h"

#define MAX_FILENAME_SIZE 11

void getShortName(unsigned char *dirEntry, char *entryName);
unsigned int getFileSize(unsigned char *dirEntry);
void setFileSize(FILE *fileImgPtr, unsigned int currentDirCluster,
                unsigned char *targetEntry, int newSize);
void setShortName(unsigned char *dirEntry, char *entryName);
int isDirectory(unsigned char *dirEntry);
unsigned int getFirstCluster(unsigned char *dirEntry);
int findFilenameCluster(Directory currentDir, char *filename,
                        unsigned int *clusterNum, int *indexPtr);

#endif

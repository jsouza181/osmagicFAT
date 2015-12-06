#ifndef DIRENTRIES_H
#define DIRENTRIES_H

#include "utilities.h"

void getShortName(unsigned char *dirEntry, char *entryName);
unsigned int getFileSize(unsigned char *dirEntry);
void setFileSize(FILE *fileImgPtr, unsigned int currentDirCluster,
                unsigned char *targetEntry, int newSize);
int isDirectory(unsigned char *dirEntry);
unsigned int getFirstCluster(unsigned char *dirEntry);
int findFilenameCluster(Directory currentDir, char *filename,
                        unsigned int *clusterNum, int *indexPtr);

#endif

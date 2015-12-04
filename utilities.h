#ifndef UTILITIES_H
#define UTILITIES_H

// The minimum value that an end of cluster chain can be represented by.
#define EOCMIN 0x0FFFFFF8

// Array of current directory entries. Each entry represented as char*.
typedef struct {
  unsigned char ** dirEntries;
  unsigned int size;
} Directory;

/* Struct to represent an entry in the open files table.
 * clusterOffsets is a dynamic array of all of the cluster
 * numbers associated with the file's data. flag indicates
 * if the file is opened with read/write/readwrite priveleges.
 */
typedef struct {
  char filename[13];
  unsigned int *clusterOffsets;
  int clusterCount;
  int flag;
} OpenFileEntry;

typedef struct {
  OpenFileEntry *entries;
  int size;
} OpenFileTable;

unsigned int swapTwoBytes(unsigned int byteA, unsigned int byteB);
unsigned int swapFourBytes(unsigned int byteA, unsigned int byteB,
                            unsigned int byteC, unsigned int byteD);
void capFilename(char *filename);

void printSector(FILE *fileImgPtr, unsigned int sectorNum);
unsigned int getSector(unsigned int clusterNumber);
unsigned int getNextCluster(FILE *fileImgPtr, unsigned int clusterNumber);
void getDirEntries(FILE *fileImgPtr, unsigned int clusterNumber, Directory *dir);

void rmDirEntries(FILE *fileImgPtr, unsigned int clusterNumber, Directory *dir,
                  char *targetFile);
void freeCluster(FILE *fileImgPtr, unsigned int clusterNumber);

#endif

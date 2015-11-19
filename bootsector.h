#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

/*
 *Function: int fseek (FILE *stream, long int offset, int whence)
    The value of whence must be one of the constants SEEK_SET, SEEK_CUR, or SEEK_END,
 */

int readBootSector(FILE *fileImgPtr, unsigned int *fsMetadata);

#endif

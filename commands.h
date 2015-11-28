#ifndef COMMANDS_H
#define COMMANDS_H

#define READ 0
#define WRITE 1
#define READWRITE 2

#include "utilities.h" // Directory struct

void open(Directory dir, char *filename, int flag);
void ls(Directory dir);
void cd(char *dirName);

#endif

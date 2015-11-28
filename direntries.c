#include <string.h>
#include "direntries.h"

void getShortName(unsigned char *dirEntry, char* entryName) {
  strncpy(entryName, (char *) dirEntry, 11);
}

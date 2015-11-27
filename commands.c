
void ls(void) {
}

void cd(char *dirName) {
  // Free old current directory.
  // Free the dynamically allocated current directory.
  for(int i = 0; i < currentDir.size; ++i)
    free(currentDir.dirEntries[i]);

  free(currentDir.dirEntries);
}

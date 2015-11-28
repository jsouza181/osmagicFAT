#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "main.h"
#include "metadata.h"
#include "utilities.h"

int main(int argc, char *argv[]) {
  FILE *fileImgPtr;
  int runLoop = 1;

  // Check for correct number of arguments.
  if(argc != 2) {
    printf("Error: Incorrect number of arguments.\n");
    printf("Expected: osmagicFAT <file image>\n");
    return 1;
  }

  // Open the file image.
  fileImgPtr = fopen(argv[1], "rb");
    if(fileImgPtr == NULL) {
      printf("Error: could not open file image\n.");
      return 1;
    }

  // Read and store the boot sector data.
  readBootSector(fileImgPtr);
  unsigned int currentDirCluster;
  // Set current directory to the root directory.
  currentDirCluster = fsMetadata[ROOT_CLUSTER];

  printf("Please input a command. Type 'quit' to quit program.\n");
  //Loop to perform commands until user exits.
  while(runLoop){
    // Allocate memory to hold commands.
    char input[256];
    char **cmds = (char **)malloc(6*sizeof(char*));
    for (int itr = 0; itr < 6; itr++)
      cmds[itr]=(char*)malloc(1*sizeof(char));

    /***Initialize current directory data***/
    // Array of current directory entries. Each entry represented as char*.
    Directory currentDir;
    currentDir.dirEntries = (unsigned char **) malloc(0 * sizeof(unsigned char *));
    currentDir.size = 0;

    // Read and store the current directory.
    getDirEntries(fileImgPtr, currentDirCluster, &currentDir);

    // Testing: print contents of current directory.
    /*
    for(int i = 0; i < currentDir.size; ++i) {
      for(int j = 0; j < 32; ++j) {
        printf("%c ", currentDir.dirEntries[i][j]);
      }
      printf("\n");
    }
    printf("\n");
    */

    // Print prompt and get user input.
    printPrompt();
    if (fgets(input, 256, stdin) == NULL){
      printf("Error! Invalid input!\n");
      exit(-1);
    }
    tokenize(input,cmds);

    // Compare first argument to perform command.
    if (strcmp(cmds[0], "quit") == 0){
      runLoop = 0;
    }
    else if (strcmp(cmds[0], "open") == 0){}
    else if (strcmp(cmds[0], "close") == 0){}
    else if (strcmp(cmds[0], "create") == 0){}
    else if (strcmp(cmds[0], "rm") == 0){}
    else if (strcmp(cmds[0], "size") == 0){}
    else if (strcmp(cmds[0], "cd") == 0){}
    else if (strcmp(cmds[0], "ls") == 0) {
      ls(currentDir);
    }
    else if (strcmp(cmds[0], "mkdir") == 0){}
    else if (strcmp(cmds[0], "rmdir") == 0){}
    else if (strcmp(cmds[0], "read") == 0){}
    else if (strcmp(cmds[0], "write") == 0){}
    else {
      printf("Invalid command. Please try again.\n");
    }

    //Free dynamically-allocated memory.
    for (int itr = 0; itr < 6; itr++)
      free(cmds[itr]);
    free(cmds);

    // Free the dynamically allocated current directory.
    for(int i = 0; i < currentDir.size; ++i)
      free(currentDir.dirEntries[i]);
    free(currentDir.dirEntries);
  }

  //Close the file image.
  fclose(fileImgPtr);
  printf("Exiting program...\n");



  return 0;
}

// Print prompt for user.
void printPrompt(){
  char host[128];
  gethostname(host,128);
  printf("%s:=> ", host);
  fflush(stdout);
}

// Tokenize string from input.
void tokenize(char* input, char** cmds){
  int a = 0;
  char* token = strtok(input," \n");
  while (token){
    strcpy(cmds[(a)++], token);
    token = strtok(NULL," \n");
    // Break if too many arguments.
    if (a == 5)
      break;
  }
  cmds[a] = NULL;
}

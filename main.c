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
  Directory currentDir;

  // Initialize open file table.
  OpenFileTable ofTable;
  ofTable.entries = (OpenFileEntry*) malloc(0 * sizeof(OpenFileEntry));
  ofTable.size = 0;

  printf("Please input a command. Type 'quit' to quit program.\n");
  //Loop to perform commands until user exits.
  while(runLoop){
    // Allocate memory to hold commands.
    char input[256];
    // Save the number of tokens(arguments) in each input.
    int tokCount = 0;
    char **cmds = (char **)malloc(6*sizeof(char*));
    for (int itr = 0; itr < 6; itr++)
      cmds[itr]=(char*)malloc(1*sizeof(char));

    // Initialize current directory data
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
    for(int i = 0; i < ofTable.size; ++i) {
      printf("Open file %d: %s, flag %d, clusCount %d, \n",
            i, ofTable.entries[i].filename,
            ofTable.entries[i].flag, ofTable.entries[i].clusterCount);
      printf("Cluster's are: ");

      for(int j = 0; j < ofTable.entries[i].clusterCount; ++j)
        printf("%d ", ofTable.entries[i].clusterOffsets[j]);
      printf("\n");

    }
    printPrompt();
    if (fgets(input, 256, stdin) == NULL){
      printf("Error! Invalid input!\n");
      exit(-1);
    }
    tokCount = tokenize(input,cmds);

    // Compare first argument to perform command.
    if (strcmp(cmds[0], "quit") == 0){
      runLoop = 0;
    }

    else if (strcmp(cmds[0], "open") == 0) {
      if(tokCount != 3) {
        printf("Error: Invalid arguments.\n");
        printf("Expected: open <file> <flag>\n");
      }
      else {
        // Convert flag input string to int.
        int flag;
        if(strcmp(cmds[2], "r") == 0)
          flag = READ;
        else if(strcmp(cmds[2], "w") == 0)
          flag = WRITE;
        else if(strcmp(cmds[2], "rw") == 0)
          flag = READWRITE;
        else if(strcmp(cmds[2], "wr") == 0)
          flag = READWRITE;
        else {
          printf("Error: Invalid flag.\n");
          continue;
        }

        if(open(currentDir, fileImgPtr, &ofTable, cmds[1], flag))
          printf("success!\n");
        else
          printf("failure\n");
      }
    }

    else if (strcmp(cmds[0], "close") == 0){}
    else if (strcmp(cmds[0], "create") == 0){}
    else if (strcmp(cmds[0], "rm") == 0){}
    else if (strcmp(cmds[0], "size") == 0){}

    else if (strcmp(cmds[0], "cd") == 0) {
      if(tokCount != 2) {
        printf("Error: Invalid arguments.\n");
        printf("Expected: cd <directory>\n");
      }
      else {
        // Find the cluster number of the desired directory and set the current
        // directory to that cluster.
        unsigned int newClusterNum = 0;
        unsigned int *newClusterPtr = &newClusterNum;
        if(cd(currentDir, cmds[1], newClusterPtr)) {
          currentDirCluster = *newClusterPtr;
        }
      }
    }

    else if (strcmp(cmds[0], "ls") == 0) {
      //If only 1 argument, lists the current directory
      if(cmds[1] == NULL)
	ls(currentDir);
      else
	{ 
	  unsigned int newDirCluster;
	  //newDir will be populated with entries from the specified directory
	  Directory newDir;
	  
	  newDir.dirEntries = (unsigned char **) malloc(0 * sizeof(unsigned char *));
	  newDir.size = 0;
	  unsigned int newClusterNum = 0;
	  unsigned int *newClusterPtr = &newClusterNum;

	  //Checks if specified directory exists
	  if(cd(currentDir, cmds[1], newClusterPtr))
	    {
	      newDirCluster = *newClusterPtr;
	      getDirEntries(fileImgPtr, newDirCluster, &newDir);
	      ls(newDir);
	    }

	  // Free the dynamically allocated new directory.
	  for(int i = 0; i < newDir.size; ++i)
	    {
	      free(newDir.dirEntries[i]);
	    }
	  free(newDir.dirEntries);
	}
    }

    
    else if (strcmp(cmds[0], "mkdir") == 0){}
    else if (strcmp(cmds[0], "rmdir") == 0){}
    else if (strcmp(cmds[0], "read") == 0){}
    else if (strcmp(cmds[0], "write") == 0){}
    else {
      printf("Invalid command. Please try again.\n");
    }

    // Free dynamically-allocated memory.
    for (int itr = 0; itr < 6; itr++)
      free(cmds[itr]);
    free(cmds);

    // Free the dynamically allocated current directory.
    for(int i = 0; i < currentDir.size; ++i) {
      free(currentDir.dirEntries[i]);
    }
    free(currentDir.dirEntries);

  }

  //Close the file image.
  fclose(fileImgPtr);
  printf("Exiting program...\n");

  // Free the dynamically allocated open file table.
  // Free the cluster chains.
  for(int i = 0; i < ofTable.size; ++i) {
    free(ofTable.entries[i].clusterOffsets);
  }
  // Free the table.
  free(ofTable.entries);

  return 0;
}

// Print prompt for user.
void printPrompt(){
  char host[128];
  gethostname(host,128);
  printf("%s:=> ", host);
  fflush(stdout);
}

// Tokenize string from input. Returns the number of tokens.
int tokenize(char* input, char** cmds){
  int a = 0;
  int tokCount = 0;
  char* token = strtok(input," \n");
  while (token) {
    tokCount = tokCount + 1;
    strcpy(cmds[(a)++], token);
    token = strtok(NULL," \n");
    // Break if too many arguments.
    if (a == 5)
      break;
  }
  cmds[a] = NULL;

  return tokCount;
}

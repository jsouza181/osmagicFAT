# FAT32 UTILITY #

### Members ###
* Jason Souza - jas11k
* Ricardo Castilla - rc12b
* Philip Scott - pcs02c
* Conor Stephens - cps11b

### Purpose ###
* Learn how to read and manage a fat32 file system image.
* Version: 1.0

### Tar file contents ###

-> osmagicFAT/

    -> README.txt
    -> README.md
    -> report.txt

    -> metadata.h
    -> metadata.c

    -> main.h
    -> main.c

    -> utilities.h
    -> utilities.c

    -> direntries.h
    -> direntries.c

    -> commands.h
    -> commands.c

    -> runCheck.bash

    -> fsck.fat # program to check integrity of fat32 filesystem. Compiled on linux

    -> Makefile

### Linux Kernel Version ###
3.16.0-38-generic

### Compiling ###
* Use provided makefile to compile the source code
```
$> make
```

* In order to clean the environment, run
```
$> make clean
```

* Check fat32 image for corruption
```
$> ./runCheck.bash <imagefile>
```
note: make sure fsck.fat is in the same directory as runCheck.bash

### Known Bugs ###
- Program segfaults when only "enter" is the user input
- Program can take from ~10-60 seconds to correctly reflect changes made to the file system when a file is modified/added/removed while the program is running
- After allocating new clusters, FSCK warns that the "free cluster summary" is wrong.
  Upon further inspection, it was found that this was due to FAT32-specific metadata in the reserved sector which stores the "last checked number of free clusters".
  This value does not seem to actually be used for any basic FAT32 operations, so it does not compromise the integrity of the files.

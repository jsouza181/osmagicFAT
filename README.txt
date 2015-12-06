# SHELL #

### Members ###
* Jason Souza
* Ricardo Castilla
* Philip Scott
* Conor Stephens

### Purpose ###
* Learn how to read, and manage a small fat32 image
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

### Assumptions ###
- Writing to a file is based on the assumption that the file exists in the current directory, if it exists at all.

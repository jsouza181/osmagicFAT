CC = gcc
CFLAGS = -Wall -pedantic -std=c99

default: osmagicFAT

osmagicFAT: main.o metadata.o utilities.o commands.o direntries.o
	$(CC) $(CFLAGS) -o osmagicFAT main.o metadata.o utilities.o commands.o direntries.o

main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.c

metadata.o: metadata.c metadata.h
	$(CC) $(CFLAGS) -c metadata.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

commands.o: commands.c commands.h
	$(CC) $(CFLAGS) -c commands.c

direntries.o: direntries.c direntries.h
	$(CC) $(CFLAGS) -c direntries.c

clean:
	rm -f *.o osmagicFAT

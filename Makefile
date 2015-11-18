CC = gcc
CFLAGS = -Wall -pedantic -std=c99

default: osmagicFAT

osmagicFAT: main.o bootsector.o
	$(CC) $(CFLAGS) -o osmagicFAT main.o bootsector.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

bootsector.o: bootsector.c bootsector.h
	$(CC) $(CFLAGS) -c bootsector.c

clean:
	rm *.o osmagicFAT

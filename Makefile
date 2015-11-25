CC = gcc
CFLAGS = -Wall -pedantic -std=c99

default: osmagicFAT

osmagicFAT: main.o bootsector.o utilities.o
	$(CC) $(CFLAGS) -o osmagicFAT main.o bootsector.o utilities.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

bootsector.o: bootsector.c bootsector.h
	$(CC) $(CFLAGS) -c bootsector.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm -f *.o osmagicFAT

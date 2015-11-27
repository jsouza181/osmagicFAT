CC = gcc
CFLAGS = -Wall -pedantic -std=c99

default: osmagicFAT

osmagicFAT: main.o metadata.o utilities.o
	$(CC) $(CFLAGS) -o osmagicFAT main.o metadata.o utilities.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

metadata.o: metadata.c metadata.h
	$(CC) $(CFLAGS) -c metadata.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm *.o osmagicFAT

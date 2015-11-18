CC = gcc
CFLAGS = -Wall -pedantic -std=c99

default: osmagicFAT

osmagicFAT: main.o
	$(CC) -o osmagicFAT main.o

clean:
	rm *.o osmagicFAT

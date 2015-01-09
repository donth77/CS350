CC=gcc
CFLAGS=-Wall -Wextra -g
EXEC=sh

all: assignment_1

assignment_1.o: assignment_1.h
	$(CC) $(CFLAGS) -c assignment_1.c

vector.o: vector.h
	$(CC) $(CFLAGS) -c vector.c

assignment_1: assignment_1.o vector.o 
	$(CC) $(CFLAGS) assignment_1.o vector.o -o $(EXEC)

clean:
	rm -rf *o clean

# Makefile for mysh.
# AUTHOR: James Tanner

CC=gcc
CFLAGS=-g

shell: mysh

mysh: mysh.o splitline.o execute.o parseEscapes.o
	$(CC) $(CFLAGS) -o mysh mysh.o splitline.o execute.o parseEscapes.o

mysh.o: mysh.c mysh.h
	$(CC) $(CFLAGS) -c mysh.c

parseEscapes.o: parseEscapes.c parseEscapes.h
	$(CC) $(CFLAGS) -c parseEscapes.c

splitline.o: splitline.c
	$(CC) $(CFLAGS) -c splitline.c

execute.o: execute.c
	$(CC) $(CFLAGS) -c execute.c

clean:
	rm -f *.o

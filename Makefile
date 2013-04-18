# Makefile for mysh.
# AUTHOR: James Tanner

CC=gcc
CFLAGS=-g

shell: mysh

mysh: mysh.o splitline.o execute.o parser.o
	$(CC) $(CFLAGS) -o mysh mysh.o splitline.o execute.o parser.o

mysh.o: mysh.c mysh.h
	$(CC) $(CFLAGS) -c mysh.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

splitline.o: splitline.c
	$(CC) $(CFLAGS) -c splitline.c

execute.o: execute.c
	$(CC) $(CFLAGS) -c execute.c

clean:
	rm -f *.o

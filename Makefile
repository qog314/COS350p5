#
# makefile for smsh
#
CC=gcc
CFLAGS=-g

smsh1: smsh1.o splitline.o execute.o
	$(CC) $(CFLAGS) -o smsh1 smsh1.o splitline.o execute.o



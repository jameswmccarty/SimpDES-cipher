# Makefile for Simple-DES block cipher implementation

CC = gcc
CFLAGS = -ansi -pedantic -Werror -Wall -O2
LFLAGS = 

all: simpDES

simpDES: simpDES.c
	$(CC) $(CFLAGS) simpDES.c -o simpDES $(LFLAGS)

clean:
	\rm simpDES

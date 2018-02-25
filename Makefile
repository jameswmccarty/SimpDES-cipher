# Makefile for Simple-DES block cipher implementation

CC = gcc
CFLAGS = -ansi -pedantic -Werror -Wall -O2
LFLAGS = 

all: simpDES check_lang

simpDES: simpDES.c
	$(CC) $(CFLAGS) simpDES.c -o simpDES $(LFLAGS)

check_lang: check_lang.c
	$(CC) $(CFLAGS) check_lang.c -o check_lang $(LFLAGS)

clean:
	\rm simpDES check_lang

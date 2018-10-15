RM = rm -f
CC = gcc
CFLAGS = -std=c99 -g -pthread -lssl -lcrypto -lm 

default: all

all: hogwarts

hogwarts: hogwarts.c
	$(CC) $< $(CFLAGS) -o hogwarts

clean:
	$(RM) hogwarts 

.PHONY: hogwarts 

RM = rm -f
CC = gcc
CFLAGS = -std=c99 -g 

default: all

all: feast

feast: feast.c
	$(CC) $< $(CFLAGS) -o feast

clean:
	$(RM) feast 

.PHONY: feast

.PHONY: all

all: create_terminate mutex join condition

create_terminate: create_terminate.c
	gcc -lpthread -o create_terminate create_terminate.c

mutex: mutex.c
	gcc -lpthread -o mutex mutex.c

join: join.c
	gcc -lpthread -o join join.c

condition: condition.c
	gcc -lpthread -o condition condition.c

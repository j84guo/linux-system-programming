.PHONY: all

all: bin bin/create_terminate bin/mutex bin/join bin/condition bin/producer_consumer

bin:
	mkdir -p bin

bin/create_terminate: create_terminate.c
	gcc -lpthread -o $@ create_terminate.c

bin/mutex: mutex.c
	gcc -lpthread -o $@ mutex.c

bin/join: join.c
	gcc -lpthread -o $@ join.c

bin/condition: condition.c
	gcc -lpthread -o $@ condition.c

bin/producer_consumer: producer_consumer.c
	gcc -lpthread -o $@ producer_consumer.c

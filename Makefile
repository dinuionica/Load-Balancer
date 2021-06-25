# Copyright 2021 Dinu Ion Irinel

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o LinkedList.o Hashtable.o CircularDoublyLinkedList.o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

LinkedList.o: LinkedList.c LinkedList.h
	$(CC) $(CFLAGS) $^ -c

Hashtable.o: Hashtable.c Hashtable.h
	$(CC) $(CFLAGS) $^ -c

CircularDoublyLinkedList.o : CircularDoublyLinkedList.c CircularDoublyLinkedList.h
	$(CC) $(CFLAGS) $^ -c
clean:
	rm -f *.o tema2 *.h.gch

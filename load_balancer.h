/* Copyright 2021 Dinu Ion Irinel */

#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include "server.h"
#include "CircularDoublyLinkedList.h"
#include "utils.h"

struct load_balancer;
typedef struct load_balancer load_balancer;

struct load_balancer {
	/* structura load_balancer-ului contine un hash_ring ce este 
	 * reprezentat sub forma de lista circulara dublu inlantuita 
	 */
	doubly_linked_list_t *hash_ring;
};

load_balancer* init_load_balancer();

unsigned int hash_function_servers(void *a);

unsigned int hash_function_key(void *a);

void free_load_balancer(load_balancer* main);

void loader_store(load_balancer* main, char* key, char* value, int* server_id);

char* loader_retrieve(load_balancer* main, char* key, int* server_id);

void loader_add_server(load_balancer* main, int server_id);

void loader_remove_server(load_balancer* main, int server_id);

void add_server(doubly_linked_list_t* list, server_memory *server,
				unsigned int label);

void remove_server(doubly_linked_list_t* list, unsigned int label);

#endif  /* LOAD_BALANCER_H_ */

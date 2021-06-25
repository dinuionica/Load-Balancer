/* Copyright 2021 Dinu Ion Irinel */

#include <stdlib.h>
#include <string.h>

#include "server.h"

#define SIZE_SERVER 10000

/* The function to create and initialize a new server */
server_memory *init_server_memory() {
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "Allocation Error\n");

	server->id = 0;
	server->hash = 0;
	/* initialize hashtable server */
	server->hashtable = ht_create(SIZE_SERVER, hash_function_string,
								  compare_function_strings);
	return server;
}

/* The function that adds a key-value pair to a server's hashtable */
void server_store(server_memory *server, char *key, char *value) {
	if (server == NULL) {
		return;
	}
	ht_put(server->hashtable, key, strlen(key) + 1, value, strlen(value) + 1);
}

/* The function that removes an object from a hashtable*/
void server_remove(server_memory *server, char *key) {
	if (server == NULL) {
		return;
	}
	ht_remove_entry(server->hashtable, key);
}

/* The function that extracts a value from a server's hashtable */
char *server_retrieve(server_memory *server, char *key) {
	if (server == NULL) {
		return NULL;
	}
	return (char *)ht_get(server->hashtable, key);
}

/* The function that frees the memory of a server */
void free_server_memory(server_memory *server) {
	ht_free(server->hashtable);
	server->hashtable = NULL;
	free(server);
	server = NULL;
}

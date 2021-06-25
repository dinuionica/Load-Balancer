/* Copyright 2021 Dinu Ion Irinel */

#include <stdlib.h>
#include <string.h>

#include "server.h"

#define SIZE_SERVER 10000

/* Functia care creaza si initializeaza un server*/
server_memory *init_server_memory() {
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "Allocation Error\n");

	server->id = 0;
	server->hash = 0;
	/* initializare hashtable server */
	server->hashtable = ht_create(SIZE_SERVER, hash_function_string,
								  compare_function_strings);
	return server;
}

/* Functia care adauga o pereche cheie-valoare in hashtable-ul unui server */
void server_store(server_memory *server, char *key, char *value) {
	if (server == NULL) {
		return;
	}
	ht_put(server->hashtable, key, strlen(key) + 1, value, strlen(value) + 1);
}

/* Functia care elimina un obiect dintr-un hashtable */
void server_remove(server_memory *server, char *key) {
	if (server == NULL) {
		return;
	}
	ht_remove_entry(server->hashtable, key);
}

/* Functia care extrage o valoare  din  hashtable-ul unui server */
char *server_retrieve(server_memory *server, char *key) {
	if (server == NULL) {
		return NULL;
	}
	return (char *)ht_get(server->hashtable, key);
}

/* Functia care elibereaza memoria unui server */
void free_server_memory(server_memory *server) {
	ht_free(server->hashtable);
	server->hashtable = NULL;
	free(server);
	server = NULL;
}

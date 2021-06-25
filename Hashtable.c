/* Copyright 2021 Dinu Ion Irinel */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "Hashtable.h"

#define MAX_BUCKET_SIZE 64

/* Functii de comparare a cheilor */
int compare_function_ints(void *a, void *b) {
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b) {
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}


/* Functii de hashing pentru int-uri si string-uri */
unsigned int hash_function_int(void *a) {
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_string(void *a) {
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/* Functie care creeaza si initializeaza un hashtable */
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		               int (*compare_function)(void*, void*)) {
	/* alocare memorie pentru hashtable */
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	DIE(ht == NULL, "Allocation Error\n");

	ht->hmax = hmax;
	ht->size = 0;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;

	/* creeare unui array de liste simplu inlantuite
	 * si initializarea fiecarei liste in parte 
	 */
	ht->buckets = malloc(hmax * sizeof(linked_list_t *));
	DIE(ht->buckets == NULL, "Allocation Error\n");
    for (unsigned int i = 0; i < hmax; ++i) {
        ht->buckets[i] = ll_create(sizeof(struct info));
    }
	return ht;
}

/* Functia care adauga un nou obiect in cadrul unui hashtable */
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	        void *value, unsigned int value_size) {
	/* determinam pozitia in care trebuie sa cautam
	 * folosind functia de hash
	 */
	unsigned int i = ht->hash_function(key) % ht->hmax;
	int pos = 0;
	ll_node_t *current = (ht->buckets[i])->head;

	while (current != NULL) {
		struct info *info = current->data;
		/* daca cheia exista deja in hashtable
		 * doar actualizam valoarea acesteia
		 */
		if (ht->compare_function(info->key, key) == 0) {
			memcpy(info->value, value, value_size);
			return;
		}
		pos++;
		current = current->next;
	}
	/* daca nu am gasit cheia in hashtable alocam memorie
	 * pentru obiect, cheie si valore si apoi le adaugam
	 */
	struct info *new_info = malloc(sizeof(struct info));
	DIE(new_info == NULL, "Allocation Error\n");
	new_info->key = malloc(key_size);
	DIE(new_info->key == NULL, "Allocation Error\n");
	memcpy(new_info->key, key, key_size);

	new_info->value = malloc(value_size);
	DIE(new_info->value == NULL, "Allocation Error\n");
	memcpy(new_info->value, value, value_size);
	ll_add_nth_node(ht->buckets[i], pos, new_info);
	ht->size++;

	free(new_info);
}

/* Functia care extrage un obiect pe baza unei chei*/
void * ht_get(hashtable_t *ht, void *key) {
	int i = ht->hash_function(key) % ht->hmax;
	/* parcurgem lista de pozitia obtinuta cu functia
	 * de hash si daca gasim un obiect cu cheia dorita
	 * ii returnam valoarea
	 */
	ll_node_t *current = (ht->buckets[i])->head;

	while (current != NULL) {
		struct info *info = current->data;
		if (ht->compare_function(info->key, key) == 0) {
			return info->value;
		}
		current = current->next;
	}
	return NULL;
}

/* Functia care verifica daca exista un obiect in hastable */
int ht_has_key(hashtable_t *ht, void *key) {
	int i = ht->hash_function(key) % ht->hmax;
	ll_node_t *current = (ht->buckets[i])->head;

	while (current != NULL) {
		struct info *info = current->data;
		/* daca gasim obiectul cu cheia dorita returnam 1 */
			if (ht->compare_function(info->key, key) == 0) {
			return 1;
		}
		current = current->next;
	}
	return 0;
}

/* Functia care elimina un obiect din hastable */
void ht_remove_entry(hashtable_t *ht, void *key) {
	int i = ht->hash_function(key) % ht->hmax;
	ll_node_t *current = (ht->buckets[i])->head;
	int pos = 0;
	/* parcurgem lista de pe pozitia obtinuta cu
	 * funcita de hash iar daca gasim obiectul 
	 * dorit il stergem si eliberam memoria alocata
	 */
	while (current != NULL) {
		struct info *info = current->data;
		if (ht->compare_function(info->key, key) == 0) {
			free(info->key);
			free(info->value);
			ll_node_t *node = ll_remove_nth_node(ht->buckets[i], pos);
			free(node->data);
			free(node);
			ht->size--;
			node = NULL;
			return;
		}
		pos++;
		current = current->next;
	}
}

/* Functia care elibereaza memoria unui hastable */
void ht_free(hashtable_t *ht) {
	for (unsigned int i = 0; i < ht->hmax; ++i) {
		ll_node_t *node = ht->buckets[i]->head;
		/* parcurgem fiecare obiect din cadrul unui
		 * liste, il stergem, eliberam memoria alocata
		 */
		while (node != NULL) {
			ll_node_t *prev = node;
			node = node->next;
			struct info *info = prev->data;
			free(info->key);
			free(info->value);
			free(prev->data);
			free(prev);
			prev = NULL;
		}
		free(ht->buckets[i]);
	}
	/* eliberam memoria array-ului de liste inlantuite */
	free(ht->buckets);
	free(ht);
	ht = NULL;
}

/* Functia care determina numarul total de noduri 
 * existente in toate bucket-urile
 */
unsigned int ht_get_size(hashtable_t *ht) {
	if (ht == NULL) {
		return 0;
	}
	return ht->size;
}

/* Functia care determina numarul total de bucket-uri */
unsigned int ht_get_hmax(hashtable_t *ht) {
	if (ht == NULL) {
		return 0;
	}
	return ht->hmax;
}

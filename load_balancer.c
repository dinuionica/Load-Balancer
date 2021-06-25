/* Copyright 2021 Dinu Ion Irinel */

#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

#define NUM_SERVERS 100000

/* The hash function for a server  */
unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;

    return uint_a;
}

/* The hash function used to key an object */
unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

/* Function for assigning and initializing load balancer */
load_balancer *init_load_balancer() {
    /* initializign of balancr */
    load_balancer *main_balancer = malloc(sizeof(load_balancer));
    DIE(main_balancer == NULL, "Allocation Error\n");
    /* create hash_ring in the form of
     * double linked circular list
     */
    main_balancer->hash_ring = dll_create(sizeof(server_memory));

    return main_balancer;
}

/* The function for adding an object to a server */
void loader_store(load_balancer *main,
                  char *key, char *value, int *server_id) {
    if (main == NULL) {
        return;
    }
   
    /* we go through the double chained list and if we find one
     * server that has a replica hash greater than
     * key hash, update id and add object
     */
    dll_node_t *current = (main->hash_ring)->head;
    do {
        server_memory *server = current->data;
        if (server->hash > hash_function_key(key)) {
            *server_id = server->id % NUM_SERVERS;
            server_store(server, key, value);
            return;
        }
        current = current->next;
    } while (current != (main->hash_ring)->head);
    /* if the key hash is greater than the hash
     * to the replicas we add the object to the head of the list
     * /
    server_memory *head_server = (main->hash_ring->head)->data;
    *server_id = head_server->id % NUM_SERVERS;
    server_store(head_server, key, value);
}

/* The function to extract an object from a server */
char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    if (main == NULL) {
        return NULL;
    }
    /* we go through the double chained list and if we find one
     * server that has a replica hash greater than
     * key hash, we extract the desired object
     */
    dll_node_t *current = main->hash_ring->head;
    do {
        server_memory *server = current->data;
        if (server->hash > hash_function_key(key)) {
            *server_id = server->id % NUM_SERVERS;
            return server_retrieve(server, key);
        }
        current = current->next;
    } while (current != (main->hash_ring)->head);

    /* if the key hash is greater than the hash
     * to the replicas we extract the object from the head of the list
     */
    server_memory *head_server = (main->hash_ring->head)->data;
    *server_id = head_server->id % NUM_SERVERS;
    return server_retrieve(head_server, key);
}

/* The function that adds the three replicas of a server to the system */
void loader_add_server(load_balancer *main, int server_id) {
    if (main == NULL) {
        return;
    }
    /* we allocate a server, and for each replica we update
     * id and hash and we add it in the hash_ring
     */
    server_memory *server = init_server_memory();
    server->id = server_id;

    for (int i = 0; i < 3; ++i)
    {
        unsigned int label = i * NUM_SERVERS + server_id;
        server->id = label;
        server->hash = hash_function_servers(&label);
        add_server(main->hash_ring, server, server->hash);
    }
    free(server);
}

/* The function that removes the three replicas of a server from the system */
void loader_remove_server(load_balancer *main, int server_id) {
    if (main == NULL) {
        return;
    }
    /* based on the hash of a replica it is removed */
    for (int i = 0; i < 3; ++i) {
        unsigned int label = i * NUM_SERVERS + server_id;
        remove_server(main->hash_ring, hash_function_servers(&label));
    }
}

/* The function that frees the memory allocated to the load_balancer */
void free_load_balancer(load_balancer *main) {
    /* we go through the hash_ring and delete the last reply */
    dll_node_t *current = main->hash_ring->head;
    do {
        server_memory *server = current->data;
        if (server->id / NUM_SERVERS == 2) {
            ht_free(server->hashtable);
        }
        current = current->next;
    } while (current != main->hash_ring->head);

    /* go through the list and release each node */
    dll_free(&main->hash_ring);
    free(main);
    main = NULL;
}

/* The function that adds a replica of a server to the system */
void add_server(doubly_linked_list_t *hash_ring,
                server_memory *server, unsigned int label) {
    int count = 0;
    dll_node_t *new_node = NULL;
    /* if the hash_ring is empty we add it on the first position */
    if (hash_ring->size == 0) {
        new_node = dll_add_nth_node(hash_ring, 0, server);
        new_node = NULL;
        return;
    } else {
        /* in case of contract we go through the list and count servers
         * which have the hash less than the replica we want
         * to add it to the system
         */
        dll_node_t *current = hash_ring->head;
        do {
            server_memory *c_server = current->data;
            if (c_server->hash < label) {
                count++;
            }
            current = current->next;
        } while (current != hash_ring->head);
        /* add the server to the desired position and extract
         * and serve them as a neighbor to the one I just added
         */
        new_node = dll_add_nth_node(hash_ring, count, server);
        server_memory *new_srv = new_node->data;
        server_memory *next_srv = new_node->next->data;
        server_memory *prev_srv = new_node->prev->data;

       /* we are looking for the next server with the id different from the newly added one */
        if (new_srv->id % NUM_SERVERS != next_srv->id % NUM_SERVERS) {
            /* we go through the hashtable and check if a key has the hash anymore
             * smaller than the hash of the next server and larger than that
             * previously, and then we add it to the new server and delete it from the old one
             */
            for (unsigned int i = 0; i < next_srv->hashtable->hmax; ++i) {
                ll_node_t *node = (next_srv->hashtable->buckets[i])->head;
                while (node != NULL) {
                    struct info *info = node->data;
                    unsigned int hash = hash_function_key(info->key);
                    node = node->next;

                    if ((hash < new_srv->hash && hash > prev_srv->hash) ||
                        (new_srv->hash < prev_srv->hash &&
                        (hash < new_srv->hash || hash > prev_srv->hash))) {
                        server_store(new_srv, info->key, info->value);
                        server_remove(next_srv, info->key);
                    }
                }
            }
        }
    }
}

/* Function that removes a replica of a server from the system */
void remove_server(doubly_linked_list_t *hash_ring, unsigned int label) {
    int count = 0;
    /* if the hash_ring is empty a replica cannot be removed */
    if (hash_ring->size == 0) {
        return;
    } else {
        /* in case of contract we go through the list and count servers
         * have the hash smaller than the label we want
         * remove it from the system
         */
        dll_node_t *current = hash_ring->head;
        do {
            server_memory *server = current->data;
            if (server->hash < label) {
                count++;
            }
            current = current->next;
        } while (current != hash_ring->head);

        /* extract the server we want to delete,
         * respectively the next and the previous one
         */
        dll_node_t *node_to_delete = dll_get_nth_node(hash_ring, count);
        server_memory *curr_srv = node_to_delete->data;
        server_memory *next_srv = node_to_delete->next->data;
        server_memory *prev_srv = node_to_delete->prev->data;

        /* we are looking for the next server with a different id from the newly added one */
        if (curr_srv->id % NUM_SERVERS != next_srv->id % NUM_SERVERS) {
             /* we go through the hashtable and check if a key has the hash anymore
              * smaller than the hash of the next server and larger than that
              * previously, and then distribute it to the new one and delete it from
              * old server
              */
            for (unsigned int i = 0; i < curr_srv->hashtable->hmax; ++i) {
                ll_node_t *node = (curr_srv->hashtable->buckets[i])->head;
                while (node != NULL) {
                    struct info *info = node->data;
                    unsigned int hash = hash_function_key(info->key);
                    node = node->next;
                    if ((hash < curr_srv->hash && hash > prev_srv->hash) ||
                        (curr_srv->hash < prev_srv->hash &&
                        (hash < curr_srv->hash || hash > prev_srv->hash))) {
                        server_store(next_srv, info->key, info->value);
                        server_remove(curr_srv, info->key);
                    }
                }
            }
        }
        /* delete hashtable once (last reply) */
        if (curr_srv->id / NUM_SERVERS == 2) {
            ht_free(curr_srv->hashtable);
        }
        dll_remove_nth_node(hash_ring, count);
    }
}

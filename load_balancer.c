/* Copyright 2021 Dinu Ion Irinel */

#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

#define NUM_SERVERS 100000

/* Funcția de hash folosita pentru server */
unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;

    return uint_a;
}

/* Funcția de hash folosita pentru cheia unui obiect */
unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

/* Funcția pentru alocarea și inițializare load balancer */
load_balancer *init_load_balancer() {
    /* alocare load_balancer */
    load_balancer *main_balancer = malloc(sizeof(load_balancer));
    DIE(main_balancer == NULL, "Allocation Error\n");

    /* creare hash_ring ul sub forma de
     * lista circulara dublu inlantuita 
     */
    main_balancer->hash_ring = dll_create(sizeof(server_memory));

    return main_balancer;
}

/* Funcția pentru adăugarea unui obiect într-un server */
void loader_store(load_balancer *main,
                  char *key, char *value, int *server_id) {
    if (main == NULL) {
        return;
    }

    /* parcurgem lista dublu inlantuita și dacă găsim un
     * server care are hash ul replicii mai mare decât 
     * hash-ul cheii, actualizam id ul și adăugăm obiectul 
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
    /* daca hash-ul cheii este mai mare decât hash-ul
     * replicilor adăugăm obiectul la head-ul listei 
     */
    server_memory *head_server = (main->hash_ring->head)->data;
    *server_id = head_server->id % NUM_SERVERS;
    server_store(head_server, key, value);
}

/* Funcția care extrage un obiect dintr-un server */
char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    if (main == NULL) {
        return NULL;
    }

    /* parcurgem lista dublu inlantuita și dacă găsim un
     * server care are hash ul replicii mai mare decât 
     * hash-ul cheii, extragem obiectul dorit
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

    /* daca hash-ul cheii este mai mare decat hash-ul
     * replicilor extragem obiectul din head-ul listei
     */
    server_memory *head_server = (main->hash_ring->head)->data;
    *server_id = head_server->id % NUM_SERVERS;
    return server_retrieve(head_server, key);
}

/* Functia care adauga cele trei replici ale unui server in sistem */
void loader_add_server(load_balancer *main, int server_id) {
    if (main == NULL) {
        return;
    }
    /* alocam un server, si pentru fiecare replica actualizam
     * id-ul si hash-ul si o adaugam in cadrul hash_ring-ului
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

/* Functia care elimina cele trei replici ale unui server din sistem */
void loader_remove_server(load_balancer *main, int server_id) {
    if (main == NULL) {
        return;
    }
    /* pe baza hash-ului unei replici aceasta este eliminata */
    for (int i = 0; i < 3; ++i) {
        unsigned int label = i * NUM_SERVERS + server_id;
        remove_server(main->hash_ring, hash_function_servers(&label));
    }
}

/* Functia care elibereaza memoria alocata load_balancer-ului */
void free_load_balancer(load_balancer *main) {
    /* parcurgem hash_ringul si stergem ultima replica */
    dll_node_t *current = main->hash_ring->head;
    do {
        server_memory *server = current->data;
        if (server->id / NUM_SERVERS == 2) {
            ht_free(server->hashtable);
        }
        current = current->next;
    } while (current != main->hash_ring->head);

    /* se parcurge lista si se elibereaza fiecare nod */
    dll_free(&main->hash_ring);
    free(main);
    main = NULL;
}

/* Functia care adauga un o replica a unui server in sistem */
void add_server(doubly_linked_list_t *hash_ring,
                server_memory *server, unsigned int label) {
    int count = 0;
    dll_node_t *new_node = NULL;
    /* daca hash_ring-ul este gol adaugam pe prima pozitie */
    if (hash_ring->size == 0) {
        new_node = dll_add_nth_node(hash_ring, 0, server);
        new_node = NULL;
        return;
    } else {
        /* in caz contrat parcurgem lista si numaram servere 
         * care au hash-ul mai mic decat replica pe care dorim
         * sa o adaugam in sistem 
         */
        dll_node_t *current = hash_ring->head;
        do {
            server_memory *c_server = current->data;
            if (c_server->hash < label) {
                count++;
            }
            current = current->next;
        } while (current != hash_ring->head);
        /* adaugam server-ul pe pozitia dorita si extragem
         * si serve-le vecine celui pe care tocmai l-am adaugat       
         */
        new_node = dll_add_nth_node(hash_ring, count, server);
        server_memory *new_srv = new_node->data;
        server_memory *next_srv = new_node->next->data;
        server_memory *prev_srv = new_node->prev->data;

        /* cautam server-ul urmator cu id-ul diferit de cel nou adaugat */
        if (new_srv->id % NUM_SERVERS != next_srv->id % NUM_SERVERS) {
            /* parcurgem hashtable-ul si verificam daca o cheie are hash-ul mai 
             * mic decat hash-ul server-ului urmator si mai mare decat cel 
             * anterior,iar apoi o adaugam in noul server si o stergem din cel vechi
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

/* Functia care elimina o replica a unui server din sistem */
void remove_server(doubly_linked_list_t *hash_ring, unsigned int label) {
    int count = 0;
    /* daca hash_ring-ul este gol nu se poate elimina un o replica */
    if (hash_ring->size == 0) {
        return;
    } else {
        /* in caz contrat parcurgem lista si numaram servere 
         * au hash-ul mai mic decat label-ul pe care dorim 
         * sa il eliminam din sistem 
         */
        dll_node_t *current = hash_ring->head;
        do {
            server_memory *server = current->data;
            if (server->hash < label) {
                count++;
            }
            current = current->next;
        } while (current != hash_ring->head);

        /* extragem server-ul pe care dorim sa il stergem,
         * respectiv pe cel urmator si pe cel anterior
         */
        dll_node_t *node_to_delete = dll_get_nth_node(hash_ring, count);
        server_memory *curr_srv = node_to_delete->data;
        server_memory *next_srv = node_to_delete->next->data;
        server_memory *prev_srv = node_to_delete->prev->data;

        /* cautam serverul urmator cu id-ul diferit de cel nou adaugat */
        if (curr_srv->id % NUM_SERVERS != next_srv->id % NUM_SERVERS) {
            /* parcurgem hashtable-ul si verificam daca o cheie are hash-ul mai 
             * mic decat hash-ul server-ului urmator si mai mare decat cel 
             * anterior,iar apoi o distribuim catre cel nou si o stergem din
             * server-ul vechi
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
        /* stergere hashtable o singura data(ultima replica) */
        if (curr_srv->id / NUM_SERVERS == 2) {
            ht_free(curr_srv->hashtable);
        }
        dll_remove_nth_node(hash_ring, count);
    }
}

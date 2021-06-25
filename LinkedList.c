/* Copyright 2021 Dinu Ion Irinel */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

/* Functia care creeaza o lista simplu inlantuita */
linked_list_t *
ll_create(unsigned int data_size) {
    linked_list_t *list;

    list = malloc(sizeof(*list));
    DIE(list == NULL, "Allocation error");

    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;

    return list;
}

/* Functia care adauga un nou nod in cadrul listei */
void ll_add_nth_node(linked_list_t *list, int n, const void *new_data) {
    if (n < 0) {
        printf("Error!\n");
        return;

    } else {
        /* alocam un nou nod,respectiv campul data al acestuia */
        ll_node_t *new = malloc(sizeof(*new));
        DIE(new == NULL, "Allocation error");
        new->data = malloc(list->data_size);
        DIE(new->data == NULL, "Allocation error");
        memcpy(new->data, new_data, list->data_size);
        ll_node_t *current = NULL;

        /* adaugare pe prima pozitie in lista */
        if (n == 0) {
            new->next = NULL;
            current = list->head;
            list->head = new;
            new->next = current;
        } else if (n >= (int) list->size) {
            /* adaugare pe o pozitie mai mare decat lungimea listei */
            new->next = NULL;
            if (list->head == NULL) {
                list->head = new;
                list->size++;
            } else {
                current = list->head;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = new;
                list->size++;
            }
        } else {
            /* adaugare pe o anumita pozitie in lista */
            int count = 0;
            current = list->head;
            /* parcurgem lista si numaram pozitiile */
            while (count != (int)n - 1) {
                current = current->next;
                count++;
            }
            new->next = current->next;
            current->next = new;
            list->size++;
        }
    }
}

/* Functia care returneaza un pointer la pozitia
 * nodului ce urmeaza sa fie sters din lista
 */
ll_node_t *
ll_remove_nth_node(linked_list_t *list, int n) {
    if (n < 0) {
        printf("Error!\n");
        return NULL;

    } else if (n == 0) {
        /* stergere de prima pozitie din lista */
        ll_node_t *first_node = NULL;
        first_node = list->head;
        list->head = first_node->next;
        list->size--;
        return first_node;

    } else {
        /* stergere de pe o anumita pozitie din lista */
        ll_node_t *current = NULL, *previous = NULL;

        current = list->head;
        int index = 0;
        while (current != NULL) {
            if (index == (int)n) {
                previous->next = current->next;
                return current;
            }
            index++;
            previous = current;
            current = current->next;
            list->size--;
        }
    }
    return NULL;
}

/*
 * Functia intoarce numarul de noduri din lista al carei
 * pointer este trimis ca parametru
 */
unsigned int ll_get_size(linked_list_t *list) {
    if (list == NULL) {
        return -1;
    }

    return list->size;
}

/* Functia care elibereaza memorie unui liste */
void ll_free(linked_list_t **pp_list) {
    linked_list_t *list = *pp_list;
    ll_node_t *temp = NULL , *current = NULL;
    current = list->head;

    /* pargugem lista, pastram intr-un nod temporar
     * fiecare nod si apoi ii eliberam memoria 
     */
    while (current  != NULL) {
       temp = current;
       current = current->next;
       free(temp->data);
       free(temp);
    }
    free(list);
    list = NULL;
}

/* Functia care afiseaza o lista ce contin int-uri */
void ll_print_int(linked_list_t *list) {
    ll_node_t *curr;

    if (list == NULL) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int *)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

/* Functia care afiseaza o lista ce contin string-uri */
void ll_print_string(linked_list_t *list) {
    ll_node_t *curr;

    if (list == NULL) {
        return;
    }
    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char *)curr->data);
        curr = curr->next;
    }

    printf("\n");
}

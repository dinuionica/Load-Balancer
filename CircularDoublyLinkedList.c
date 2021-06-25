/* Copyright 2021 Dinu Ion Irinel */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CircularDoublyLinkedList.h"
#include "utils.h"

/* functia pentru crearea unei liste circulare doublu inlantuite */
doubly_linked_list_t *dll_create(unsigned int data_size) {
    /* alocare lista */
    doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
    DIE(list == NULL, "Allocation error\n");

    /* atribuire argumente */
    list->data_size = data_size;
    list->head = NULL;
    list->size = 0;

    return list;
}

/* functia pentru extragerea unui nod din lista */
dll_node_t * dll_get_nth_node(doubly_linked_list_t *list, unsigned int n) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }
    /* ciclare in cadrul listei */
    if (n >= list->size) {
        n = (list->size % n);
    }

    dll_node_t *current = list->head;
    unsigned int index = 0;

    while (index < n) {
        index++;
        current = current->next;
    }
    /* returnam nodul cautat */
    return current;
}

/* functia pentru adaugarea unui nou nod in lista
 * care intoarce un pointer la nodul respectiv
 */
dll_node_t *dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
                             const void *data) {
    dll_node_t *new_node = malloc(sizeof(dll_node_t));
    DIE(new_node == NULL, "Allocation error\n");

    /* alocarea unui nou nod */
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "Allocation error\n");
    memcpy(new_node->data, data, list->data_size);

    /* adaugare nod intr-o lista goala */
    if (list->head == NULL) {
        list->head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        list->size++;

    /* adaugare pe prima pozitie in lista */
    } else if (n == 0) {
        dll_node_t *last_node = (list->head)->prev;
        new_node->next = list->head;
        new_node->prev = last_node;

        last_node->next = new_node;
        (list->head)->prev = new_node;
        list->head = new_node;
        list->size++;

    /* adaugare pe ultima pozitie in lista */
    } else if (n >= list->size) {
        dll_node_t *last = (list->head)->prev;
        new_node->next = list->head;
        (list->head)->prev = new_node;
        new_node->prev = last;
        last->next = new_node;
        list->size++;

    /* adaugare pe o anumita pozitie in lista */
    } else {
        dll_node_t *current = list->head;
        int count = 0;

        while (count != (int)n - 1) {
            current = current->next;
            count++;
        }
        new_node->next = current->next;
        (current->next)->prev = new_node;
        current->next = new_node;
        new_node->prev = current;
        list->size++;
    }
    return new_node;
}

/* functia de adaugarea a unui nod la sfarsitul listei */
void dll_add_nth_push(doubly_linked_list_t *list, const void *data) {
    /* alocare unui nou nod */
    dll_node_t *new_node = malloc(sizeof(dll_node_t));
    DIE(new_node == NULL, "Allocation error\n");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "Allocation error\n");
    memcpy(new_node->data, data, list->data_size);

    /* daca lista este goala adaugam pe prima pozitie */
    if (list->head == NULL) {
        list->head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        list->size++;
    }

    /* adaugare la sfaristul listei */
    dll_node_t *last_node = (list->head)->prev;
    new_node->next = list->head;
    (list->head)->prev = new_node;
    new_node->prev = last_node;
    last_node->next = new_node;
    list->size++;
}

/* functia de stergere a unui nod din lista */
void dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n) {
    if (list == NULL || list->head == NULL) {
        return;
    }
    /* ciclare in cadrul listei */
    n = (n % list->size);

    /* eliminarea singurului nod din lista */
    if (list->size == 1) {
        free(list->head->data);
        free(list->head);
        list->head = NULL;

    } else if (n == 0) {
        /* eliminarea primului nod din lista */
        dll_node_t *first_node = list->head;
        dll_node_t *last_node = (list->head)->prev;

        last_node->next = first_node->next;
        (first_node->next)->prev = last_node;
        list->head = last_node->next;
        free(first_node->data);
        free(first_node);
        first_node = NULL;

    } else {
        /* eliminarea unui nod de pe o anumita pozitie */
        dll_node_t *current = NULL, *previous = NULL;
        current = list->head;
        unsigned int index = 0;
        while (index < n) {
            index++;
            previous = current;
            current = current->next;
        }

        (current->next)->prev = previous;
        previous->next = current->next;
        free(current->data);
        free(current);
        current = NULL;
    }

    list->size--;
}

/* functia pentru determinarea lungimii listei */
unsigned int dll_get_size(doubly_linked_list_t *list) {
    return list->size;
}

/* functia pentru eliberarea memoriei unei liste */
void dll_free(doubly_linked_list_t **pp_list) {
    /* declarea unui pointer catre lista */
    doubly_linked_list_t *list = *pp_list;
    dll_node_t *temp = NULL;
    dll_node_t *current = list->head;
    /* eliberarea memoriei */
    while (current->next != list->head) {
        temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }

    free(current->data);
    free(current);
    free(list);
    *pp_list = NULL;
}

/* functia pentru afisarea elementelor de tip intreg */
void dll_print_int_list(doubly_linked_list_t *list) {
    if (list == NULL) {
        return;
    }
    /* parcurgerea si afisarea fiecarui nod */
    dll_node_t *current = list->head;
    while (current->next != list->head) {
        printf("%d ", *(int *)current->data);
        current = current->next;
    }
    printf("%d ", *(int *)current->data);
    printf("\n");
}

/* functia pentru afisarea elementelor de tip string */
void dll_print_string_list(doubly_linked_list_t *list) {
    if (list == NULL) {
        return;
    }
    /* parcurgerea si afisarea fiecarui nod */
    dll_node_t *current = list->head;
    while (current->next != list->head) {
        printf("%s ", (char *)current->data);
        current = current->next;
    }
    printf("%s ", (char *)current->data);
    printf("\n");
}

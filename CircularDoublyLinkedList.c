/* Copyright 2021 Dinu Ion Irinel */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CircularDoublyLinkedList.h"
#include "utils.h"

/* Function for creating a double-linked circular list*/
doubly_linked_list_t *dll_create(unsigned int data_size) {
    /* allocation of the list*/
    doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
    DIE(list == NULL, "Allocation error\n");

    /* attribute arguments */
    list->data_size = data_size;
    list->head = NULL;
    list->size = 0;

    return list;
}

/* The function to extract a node from the list*/
dll_node_t * dll_get_nth_node(doubly_linked_list_t *list, unsigned int n) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }
    /* cycling within the list */
    if (n >= list->size) {
        n = (list->size % n);
    }

    dll_node_t *current = list->head;
    unsigned int index = 0;

    while (index < n) {
        index++;
        current = current->next;
    }
    /* return the desired node */
    return current;
}

/* function to add a new node to the list
 * which returns a pointer to that node
 */
dll_node_t *dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
                             const void *data) {
    dll_node_t *new_node = malloc(sizeof(dll_node_t));
    DIE(new_node == NULL, "Allocation error\n");

    /* allocation of a new node*/
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "Allocation error\n");
    memcpy(new_node->data, data, list->data_size);

    /* adding a node in a empty list */
    if (list->head == NULL) {
        list->head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        list->size++;

    /* adding at first position */
    } else if (n == 0) {
        dll_node_t *last_node = (list->head)->prev;
        new_node->next = list->head;
        new_node->prev = last_node;

        last_node->next = new_node;
        (list->head)->prev = new_node;
        list->head = new_node;
        list->size++;

    /* adding at the last position */
    } else if (n >= list->size) {
        dll_node_t *last = (list->head)->prev;
        new_node->next = list->head;
        (list->head)->prev = new_node;
        new_node->prev = last;
        last->next = new_node;
        list->size++;

    /* adding at the specific position */
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

/* The function to add a new node at the final of the list */
void dll_add_nth_push(doubly_linked_list_t *list, const void *data) {
    /* allocation of a new node */
    dll_node_t *new_node = malloc(sizeof(dll_node_t));
    DIE(new_node == NULL, "Allocation error\n");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "Allocation error\n");
    memcpy(new_node->data, data, list->data_size);

    /* if the list is empty we add at the first position */
    if (list->head == NULL) {
        list->head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        list->size++;
    }

    /* adding at the last position of the list */
    dll_node_t *last_node = (list->head)->prev;
    new_node->next = list->head;
    (list->head)->prev = new_node;
    new_node->prev = last_node;
    last_node->next = new_node;
    list->size++;
}

/* The function to delete a node from the list */
void dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n) {
    if (list == NULL || list->head == NULL) {
        return;
    }
    /* cycling within the list */
    n = (n % list->size);

    /* removing the only node from the list */
    if (list->size == 1) {
        free(list->head->data);
        free(list->head);
        list->head = NULL;

    } else if (n == 0) {
        /* removing the first node from the list */
        dll_node_t *first_node = list->head;
        dll_node_t *last_node = (list->head)->prev;

        last_node->next = first_node->next;
        (first_node->next)->prev = last_node;
        list->head = last_node->next;
        free(first_node->data);
        free(first_node);
        first_node = NULL;

    } else {
        /* removing a node from a certain position */
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

/* The function to calculate the size of the list */
unsigned int dll_get_size(doubly_linked_list_t *list) {
    return list->size;
}

/* functia pentru eliberarea memoriei unei liste */
void dll_free(doubly_linked_list_t **pp_list) {
    /* declaring a pointer to the list */
    doubly_linked_list_t *list = *pp_list;
    dll_node_t *temp = NULL;
    dll_node_t *current = list->head;
    /* free memory */
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

/* The function for displaying integer elements */
void dll_print_int_list(doubly_linked_list_t *list) {
    if (list == NULL) {
        return;
    }
    /* scrolling and displaying each node */
    dll_node_t *current = list->head;
    while (current->next != list->head) {
        printf("%d ", *(int *)current->data);
        current = current->next;
    }
    printf("%d ", *(int *)current->data);
    printf("\n");
}

/* The function for displaying string elements*/
void dll_print_string_list(doubly_linked_list_t *list) {
    if (list == NULL) {
        return;
    }
    /* scrolling and displaying each node  */
    dll_node_t *current = list->head;
    while (current->next != list->head) {
        printf("%s ", (char *)current->data);
        current = current->next;
    }
    printf("%s ", (char *)current->data);
    printf("\n");
}

/* Copyright 2021 Dinu Ion Irinel */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

/* The function to create a linked list*/
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

/* The function to add a new node in the list */
void ll_add_nth_node(linked_list_t *list, int n, const void *new_data) {
    if (n < 0) {
        printf("Error!\n");
        return;

    } else {
        /* allocation memory of the node */
        ll_node_t *new = malloc(sizeof(*new));
        DIE(new == NULL, "Allocation error");
        new->data = malloc(list->data_size);
        DIE(new->data == NULL, "Allocation error");
        memcpy(new->data, new_data, list->data_size);
        ll_node_t *current = NULL;

        /* adding at the first position */
        if (n == 0) {
            new->next = NULL;
            current = list->head;
            list->head = new;
            new->next = current;
        } else if (n >= (int) list->size) {
            /* adding at the last position */
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
            /* adding at a specific position */
            int count = 0;
            current = list->head;
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

/* The function that returns a pointer to the position
 * the node to be deleted from the list
 */
ll_node_t *
ll_remove_nth_node(linked_list_t *list, int n) {
    if (n < 0) {
        printf("Error!\n");
        return NULL;

    } else if (n == 0) {
        /* deleting the first position in the list */
        ll_node_t *first_node = NULL;
        first_node = list->head;
        list->head = first_node->next;
        list->size--;
        return first_node;

    } else {
        /* delete from a certain position in the list */
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
 * The function returns the number of nodes in the list whose
 * pointer is sent as a parameter
 */
unsigned int ll_get_size(linked_list_t *list) {
    if (list == NULL) {
        return -1;
    }

    return list->size;
}

/* The function that frees memory from a list */
void ll_free(linked_list_t **pp_list) {
    linked_list_t *list = *pp_list;
    ll_node_t *temp = NULL , *current = NULL;
    current = list->head;

   /* pargugem list, keep in a temporary node
    * each node and then we release its memory
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

/* The function that displays a list containing ints */
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

/* The function that displays a list containing strings */
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

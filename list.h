/*
 * Linked list which contains words
 */

#ifndef LISTS_H
#define LISTS_H

#include "common.h"

struct node {
	char *word;
	struct node *next;
};

/* Returns NULL */
struct node *list_create();

/* Deallocates memory and returns NULL */
struct node *list_destroy(struct node *l);

/* Adds a word to the list, making sure there are no duplicates */
enum word_result list_add(struct node **l, char *w);

/* Prints the list */
void list_print(struct node *l);

#endif	/* ifndef LISTS_H */

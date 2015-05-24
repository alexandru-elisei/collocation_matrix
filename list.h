/*
 * Linked list which contains words
 */

#ifndef LISTS_H
#define LISTS_H

#include "common.h"

struct lnode {
	char *word;
	unsigned int count;	/* how many times the node appears */
	float cost;
	struct lnode *next;
};

/* Returns NULL */
struct lnode *list_create();

/* Deallocates memory and returns NULL */
struct lnode *list_destroy(struct lnode *l);

/* Adds a word to the list, making sure there are no duplicates */
enum word_result list_add(struct lnode **l, char *w);

/* Gets the cost for a word */
float list_get_cost(struct lnode *l, char *w);

/* Prints the list */
void list_print(struct lnode *l);

#endif	/* ifndef LISTS_H */

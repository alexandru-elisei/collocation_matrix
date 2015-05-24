/*
 * Graph that holds the words, stored as an adjancency list
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "list.h"

struct vertex {
	char *word;
	struct lnode *adj;	/* adjancency list */
};

struct graph {
	struct vertex *node;
	unsigned int size;
	unsigned int mem_alloc;
};

/* Returns NULL */
struct graph *graph_create();

/* Deallocates memory, including strings, and returns NULL */
struct graph *graph_destroy(struct graph *g);

/* Adds a vertex to the graph */
void graph_add(struct graph **g, char *w);

/* Prints the graph */
void graph_print(struct graph *g);

#endif 	/* ifndef GRAPH_H */

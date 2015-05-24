/*
 * Graph that holds the words, stored as an adjancency list
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "list.h"

struct vertex {
	char *word;
	unsigned int count;	/* how many times the word appears */
	struct lnode *adj;	/* adjancency list */
};

struct wgraph {
	struct vertex *nodes;
	unsigned int size;
	unsigned int mem_alloc;
};

/* Returns a graph with zero elements */
struct wgraph *wgraph_create();

/* Deallocates memory, including strings, and returns NULL */
struct wgraph *wgraph_destroy(struct wgraph *g);

/* Adds a vertex to the graph */
void wgraph_add(struct wgraph *g, char *w, int position);

/* Prints the graph */
void wgraph_print(struct wgraph *g);

#endif 	/* ifndef GRAPH_H */

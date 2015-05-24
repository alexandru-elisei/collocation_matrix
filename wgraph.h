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
void wgraph_add(struct wgraph *g, char *p, char *w, int position);

/* Calculates the costs between vertices */
enum word_result wgraph_calculate_costs(struct wgraph *g,
		unsigned int total_words);

/* Checks for an empty graph */
enum word_result wgraph_empty(struct wgraph *g);

/* Prints the graph */
void wgraph_print(struct wgraph *g);

#endif 	/* ifndef GRAPH_H */

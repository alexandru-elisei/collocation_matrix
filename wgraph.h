/*
 * Graph that holds the words, stored as an adjancency list
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "list.h"
#include "tree.h"
#include "pqueue.h"

struct vertex {
	char *word;
	unsigned int count;		/* number of occurences */
	struct lnode *adj;		/* adjancency list */
};

struct wgraph {
	struct vertex *nodes;
	int last_word_index;		/* no word after the last word */
	unsigned int total_words;
	unsigned int size;
	unsigned int mem_alloc;
};

/* Returns a graph with zero elements */
struct wgraph *wgraph_create();

/* Deallocates memory, including strings, and returns NULL */
struct wgraph *wgraph_destroy(struct wgraph *g);

/* Adds a vertex to the graph */
void wgraph_add(struct wgraph *g, char *w, int index, int *prev, float cost);

/* Calculates the costs between vertices */
enum word_result wgraph_calculate_costs(struct wgraph *g);

/* Returns the cost between two words, where the starting node is a word */
float wgraph_cost_by_name(struct wgraph *g, struct tnode *t,
		char *word, char *neighbour);

/* Returns the cost between two words, where the starting node is an index */
float wgraph_cost_by_index(struct wgraph *g, struct tnode *t,
			int index, char *neighbour);

/* Checks for an empty graph */
enum word_result wgraph_empty(struct wgraph *g);

/* 
 * Finds the minimum cost path between two words by using Dijkstra's algorithm.
 * Returns a pointer to an array of words that are part of the path
 */
enum word_result wgraph_min_path(struct wgraph *g, struct tnode *t,
	       	char *start, char *end, FILE *out);

/* 
 * Finds the fixed cost path between two words.
 * Returns a pointer to an array of words that are part of the path
 */
enum word_result wgraph_fixed_path(struct wgraph *g, struct tnode *t,
	       	int length, char *end, FILE *out);

/* Prints the graph */
void wgraph_print(struct wgraph *g);

#endif 	/* ifndef GRAPH_H */

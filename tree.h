/*
 * Tree holding the words for easier searching
 */

#ifndef TREE_H
#define TREE_H

#include "common.h"
#include "list.h"

struct tnode {
	char *word;		
	int graph_index;		/* index in the graph array */
	struct tnode *left;		/* lexicographically smaller */
	struct tnode *right;		/* lexicographically larger */
};

/* Returns NULL */
struct tnode *tree_create();

/* Deallocates memory for the tree and returns NULL */
struct tnode *tree_destroy(struct tnode *t);

/* 
 * Adds the address of a string to the tree, returning the position where the
 * word should be inserted in the graph based on the number of distinct words
 * read so far.
 */
int tree_add(struct tnode **t, char *w, int distinct_words);

/* Prints the tree, smaller (lexicographically) to larger */
void tree_print(struct tnode *t);

#endif	/* ifndef TREE_H */

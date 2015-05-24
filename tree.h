/*
 * Tree holding the words for easier searching
 */

#ifndef TREE_H
#define TREE_H

#include "common.h"
#include "list.h"

struct tnode {
	char *word;		
	unsigned int count;	/* number of times word appears */
	struct tnode *left;	/* lexicographically smaller */
	struct tnode *right;	/* lexicographically larger */
};

/* Returns NULL */
struct tnode *tree_create();

/* Deallocates memory for the tree and returns NULL */
struct tnode *tree_destroy(struct tnode *t);

/* Adds the address of a string to the tree */
enum word_result tree_add(struct tnode **t, char *w);

/* Prints the tree, smaller (lexicographically) to larger */
void tree_print(struct tnode *h);

#endif	/* ifndef TREE_H */

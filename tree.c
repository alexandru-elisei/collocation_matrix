#include "tree.h"

/* Returns NULL */
struct tnode *tree_create()
{
	return NULL;
}

/* Deallocates memory and returns NULL */
struct tnode *tree_destroy(struct tnode *h)
{
	if (h != NULL) {
		tree_destroy(h->left);
		tree_destroy(h->right);
		free(h);
	}
	return NULL;
}

/* 
 * Adds the address of a string to the tree, returning the position where the
 * word should be inserted in the graph based on the number of distinct words
 * read so far.
 */
int tree_add(struct tnode **t, char *w, int distinct_words)
{
	struct tnode *tmp;
	struct tnode *it;
	int equal;

	tmp = (struct tnode *)malloc(sizeof(struct tnode));
	tmp->word = w;
	tmp->left = tmp->right = NULL;
  
	if (*t == NULL) {
		tmp->graph_index = distinct_words;
		*t = tmp;
		return distinct_words;
	}

	it = *t;
	while (FOREVER) {
		equal = strcmp(w, it->word);
		if (equal < 0) {
			if (it->left == NULL) {
				tmp->graph_index = distinct_words;
				it->left = tmp;
				return distinct_words;
			} else {
				it = it->left;
			}
		} else if (equal == 0) {
			return it->graph_index;
		} else {
			if (it->right == NULL) {
				tmp->graph_index = distinct_words;
				it->right = tmp;
				return distinct_words;
			} else {
				it = it->right;
			}
		}
	}
}

/* Searches for a word and returns the index in the graph */
int tree_search(struct tnode *t, char *w)
{
	int equal;

	if (t == NULL)
		return WORD_NOT_FOUND;

	while (FOREVER) {
		equal = strcmp(w, t->word);
		if (equal < 0)
			t = t->left;
		else if (equal > 0)
			t = t->right;
		else 
			return t->graph_index;

		/* Word not found */
		if (t == NULL)
			return WORD_NOT_FOUND;
	}
}


/* Prints the tree, smaller (lexicographically) to larger */
void tree_print(struct tnode *t)
{
	if (t != NULL) {
		tree_print(t->left);
		printf("|%s| - index: %d\n", t->word, t->graph_index);
		tree_print(t->right);
	}
}


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

/* Adds the address of a string to the tree */
enum word_result tree_add(struct tnode **t, char *w)
{
	struct tnode *tmp;
	struct tnode *it;
	int equal;

	tmp = (struct tnode *)malloc(sizeof(struct tnode));
	tmp->word = w;
	tmp->count = 1;
	tmp->left = tmp->right = NULL;
  
	if (*t == NULL) {
		*t = tmp;
		return WORD_SUCCESS;
	}

	it = *t;
	while (FOREVER) {
		equal = strcmp(w, it->word);
		if (equal < 0) {
			if (it->left == NULL) {
				it->left = tmp;
				return WORD_SUCCESS;
			} else {
				it = it->left;
			}
		} else if (equal == 0) {
			it->count++;
			return WORD_DUPLICATE;
		} else {
			if (it->right == NULL) {
				it->right = tmp;
				return WORD_SUCCESS;
			} else {
				it = it->right;
			}
		}
	}
}

/* Prints the tree, smaller (lexicographically) to larger */
void tree_print(struct tnode *h)
{
	if (h != NULL) {
		tree_print(h->left);
		printf("|%s| = %d\n", h->word, h->count);
		tree_print(h->right);
	}
}


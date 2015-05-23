#include "hash.h"

/* Creates and array of list nodes */
struct node **hash_create()
{
	return (struct node **) malloc(HASH_SIZE * sizeof(struct node *));
}

/* Deallocates memory and return NULL */
struct node **hash_destroy(struct node **h)
{
	int i;

	for (i = 0; i < HASH_SIZE; i++)
		list_destroy(h[i]);

	free(h);

	return NULL;
}

/* Adds an element to the hash */
enum word_result hash_add(struct node **h, char *c)
{
	return WORD_SUCCESS;
}

/* Prints the hash */
void hash_print(struct node **h)
{
}



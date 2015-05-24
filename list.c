#include "list.h"

/* Returns NULL */
struct lnode *list_create()
{
	return NULL;
}

/* Deallocates memory and returns NULL */
struct lnode *list_destroy(struct lnode *l)
{
	struct lnode *tmp;

	while (l != NULL) {
		tmp = l;
		l = l->next;
		free(tmp->word);
		free(tmp);
	}

	return l;
}

/* Adds a word to the list, making sure there are no duplicates */
enum word_result list_add(struct lnode **l, char *w)
{
	struct lnode *tmp;
	struct lnode *aux;

	if (w == NULL)
		return WORD_SUCCESS;

	if (*l == NULL) {
		*l = (struct lnode *) malloc(sizeof(struct lnode));
		(*l)->word = strdup(w);
		(*l)->count = 1;
		(*l)->cost = 0;
		(*l)->next = NULL;
	} else {
		for (tmp = *l; tmp->next != NULL; tmp = tmp->next)
			if (strcmp(tmp->word, w) == 0)
				break;

		if (strcmp(tmp->word, w) != 0) {
			aux = (struct lnode *) malloc(sizeof(struct lnode));	
			aux->word = strdup(w);
			aux->count = 1;
			aux->cost = 0;
			aux->next = tmp->next;
			tmp->next = aux;
		} else {
			tmp->count++;
		}
	}

	return WORD_SUCCESS;
}

/* Prints the list */
void list_print(struct lnode *l)
{
	for (; l != NULL; l = l->next)
		printf(" %-11s - %3d (cost %g)\n", l->word, l->count, l->cost);
}

/* Gets the cost for a word */
float list_get_cost(struct lnode *l, char *w)
{
	if (l == NULL)
		return WORD_NOT_FOUND;

	for (; l != NULL; l = l->next)
		if (strcmp(l->word, w) == 0)
			return l->cost;

	return WORD_NOT_FOUND;
}

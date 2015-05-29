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
		free(tmp);
	}

	return l;
}

/* Adds a word to the list, making sure there are no duplicates */
enum word_result list_add(struct lnode **l, int index, float cost)
{
	struct lnode *tmp;
	struct lnode *aux;

	if (*l == NULL) {
		*l = (struct lnode *) malloc(sizeof(struct lnode));
		(*l)->count = 1;
		(*l)->cost = cost;
		(*l)->graph_index = index;
		(*l)->next = NULL;
	} else {
		for (tmp = *l; tmp->next != NULL; tmp = tmp->next)
			if (tmp->graph_index == index)
				break;

		if (tmp->graph_index != index) {
			aux = (struct lnode *) malloc(sizeof(struct lnode));	
			aux->count = 1;
			aux->cost = cost;
			aux->graph_index = index;
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
		printf(" %3d (cost %7g)(index %4d)\n", 
			l->count, l->cost, l->graph_index);
}

/* Gets the cost for a word */
float list_get_cost(struct lnode *l, int index)
{
	if (l == NULL)
		return INF;

	for (; l != NULL; l = l->next)
		if (l->graph_index == index)
			return l->cost;

	return INF;
}

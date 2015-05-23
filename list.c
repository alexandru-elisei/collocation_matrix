#include "list.h"

/* Returns NULL */
struct node *list_create()
{
	return NULL;
}

/* Deallocates memory and returns NULL */
struct node *list_destroy(struct node *l)
{
	struct node *tmp;

	while (l != NULL) {
		tmp = l;
		l = l->next;
		free(tmp->word);
		free(tmp);
	}

	return l;
}

/* Adds a word to the list, making sure there are no duplicates */
enum word_result list_add(struct node **l, char *w)
{
	struct node *tmp;
	struct node *aux;

	if (*l == NULL) {
		*l = (struct node *) malloc(sizeof(struct node));
		(*l)->word = strdup(w);
		(*l)->next = NULL;
	} else {
		for (tmp = *l; tmp->next != NULL; tmp = tmp->next)
			if (strcmp(tmp->word, w) == 0)
				break;

		if (strcmp(tmp->word, w) != 0) {
			aux = (struct node *) malloc(sizeof(struct node));	
			aux->word = strdup(w);
			aux->next = tmp->next;
			tmp->next = aux;
		}
	}

	return WORD_SUCCESS;
}

/* Prints the list */
void list_print(struct node *l)
{
	for (; l != NULL; l = l->next)
		printf("%s\n", l->word);
}

#include <math.h>

#include "wgraph.h"

/* Memory allocation increment */
#define MEM_INC		(100)

/* Returns a graph with zero elements */
struct wgraph *wgraph_create()
{
	struct wgraph *ret;

	ret = (struct wgraph *)malloc(sizeof(struct wgraph));
	ret->size = 0;
	ret->mem_alloc = MEM_INC;
	ret->nodes = (struct vertex *)malloc(
			ret->mem_alloc * sizeof(struct vertex));

	return ret;
}

/* Deallocates memory, including strings, and returns NULL */
struct wgraph *wgraph_destroy(struct wgraph *g)
{
	int i;

	if (g->size != 0)
		for (i = 0; i < g->size; i++) {
			list_destroy(g->nodes[i].adj);
			free(g->nodes[i].word);
		}
	free(g->nodes);
	free(g);

	return NULL;
}

/* Adds a vertex to the graph */
void wgraph_add(struct wgraph *g, char *prev, char *current, int position)
{
	/* Adding a new word */
	if (position == g->size) {
		if (g->size == g->mem_alloc) {
			g->mem_alloc += MEM_INC;
			g->nodes = (struct vertex *)realloc(g->nodes,
					g->mem_alloc * sizeof(struct vertex));
		}

		g->nodes[position].word = prev;
		g->nodes[position].count = 1;
		g->nodes[position].adj = list_create();
		list_add(&g->nodes[position].adj, current);
		g->size++;
	} else {
		g->nodes[position].count++;
		list_add(&g->nodes[position].adj, current);
	}
}

/* Prints the graph */
void wgraph_print(struct wgraph *g)
{
	int i;

	for (i = 0; i < g->size; i++) {
		printf("|%-10s| - %3d\n", g->nodes[i].word,
				g->nodes[i].count);
		list_print(g->nodes[i].adj);
		printf("\n");
	}
}

/* Checks for an empty graph */
enum word_result wgraph_empty(struct wgraph *g)
{
	if (g == NULL)
		return WORD_ERROR_GRAPH_NOT_INITIALIZED;

	if (g->size == 0)
		return WORD_GRAPH_IS_EMPTY;

	return WORD_GRAPH_NOT_EMPTY;
}

/* Calculates the costs between vertices */
enum word_result wgraph_calculate_costs(struct wgraph *g,
		struct tnode *t, unsigned int total_words)
{
	struct lnode *l;
	enum word_result r;

	int i;
	int o11, o12, o21, o22;
	int tmp_index;
	float max_odds; 

	if ((r = wgraph_empty(g)) != WORD_GRAPH_NOT_EMPTY)
		return r;

	/* Calculating the odds */
	max_odds = -1;
	for (i = 0; i < g->size; i++)
		for (l = g->nodes[i].adj; l != NULL; l = l->next) {
			/* How many times l appeared after node i */
			o11 = l->count;

			/* 
			 * How many times node i appeared minus the times it
			 * appeared next to l 
			 */
			if (i != g->last_word_index) 
				o12 = g->nodes[i].count - o11;
			/* 
			 * If it's the last word, we subtract 1 because there's
			 * no other word after it.
			 */
			else
				o12 = g->nodes[i].count - o11 - 1;

			/* 
			 * How many times l appeared minus the times it appeared
			 * after node i
			 */
			tmp_index = tree_search(t, l->word);
			o21 = g->nodes[tmp_index].count - o11;

			o22 = (total_words - 1) - o11 - o12 - o21;

			l->cost = log(o11 + 0.5) + log(o22 + 0.5) -
				log(o12 + 0.5) - log(o21 + 0.5);

			if (max_odds < l->cost)
				max_odds = l->cost;
		}

	/* Calculating the costs */
	for (i = 0; i < g->size; i++)
		for (l = g->nodes[i].adj; l != NULL; l = l->next)
			l->cost = 1 + max_odds - l->cost;

	return WORD_SUCCESS;
}

/* Returns the cost between two words, where the starting node is a word */
float wgraph_cost_by_name(struct wgraph *g, struct tnode *t,
			char *word, char *neighbour)
{
	int index;

	index = tree_search(t, word);
	if (index == WORD_NOT_FOUND)
		return INF;

	return list_get_cost(g->nodes[index].adj, neighbour);
}

/* Returns the cost between two words, where the starting node is an index */
float wgraph_cost_by_index(struct wgraph *g, struct tnode *t,
			int index, char *neighbour)
{
	return list_get_cost(g->nodes[index].adj, neighbour);
}

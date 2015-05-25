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
		printf("[%4d]%-10s - %3d\n", i, g->nodes[i].word,
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
	if (index == NODE_NOT_FOUND)
		return INF;

	return list_get_cost(g->nodes[index].adj, neighbour);
}

/* Returns the cost between two words, where the starting node is an index */
float wgraph_cost_by_index(struct wgraph *g, struct tnode *t,
			int index, char *neighbour)
{
	return list_get_cost(g->nodes[index].adj, neighbour);
}

void print_path(struct wgraph *g, int *p, int start, int end)
{
	if (p[end] != start)
		print_path(g, p, start, p[end]);
	printf("[ %s - %s ]", g->nodes[p[end]].word, g->nodes[end].word); 
}

/* 
 * Finds the minimum cost path between two words.
 * Returns a pointer to an array of words that are part of the path
 */
char **wgraph_min_path(struct wgraph *g, struct tnode *t,
	       	char *start, char *end)
{
	struct lnode *l;
	struct pqueue *pq;

	/* 
	 * Index is the head of the arc, value is the tail of the arc, both
	 * stored as indices in the graph  
	 */
	int *prev;
	float *costs;	/* cost between start and array index */
	float new_cost;

	int start_index, end_index, neighbour_index;
	int min_index;
	int i;

	if (wgraph_empty (g) != WORD_GRAPH_NOT_EMPTY)
		return NULL;

	start_index = tree_search(t, start);
	end_index = tree_search(t, end);

	if (start_index == NODE_NOT_FOUND ||
			end_index == NODE_NOT_FOUND ||
			g->nodes[start_index].adj == NULL)
		return NULL;

	pq = pqueue_create(g->size);

	prev = (int *)malloc(g->size * sizeof(int));
	costs = (float *)malloc(g->size * sizeof(float));

	for (i = 0; i < g->size; i++) {
		prev[i] = NODE_NOT_FOUND;
		costs[i] = INF;
	}

	prev[start_index] = start_index;
	costs[start_index] = 0;

	for (l = g->nodes[start_index].adj; l != NULL; l = l->next) {
		neighbour_index = tree_search(t, l->word);
		costs[neighbour_index] = l->cost;
		prev[neighbour_index] = start_index;

		/* Adding neighbors of start word to the priority queue */
		pq->insert(neighbour_index, l->cost);
	}

	min_index = g->size;
	while (pq->is_empty() != 1) {
		min_index = pq->extract_min();
	//	printf("\nmin_index = %d\n", min_index);
	//	printf("end_index = %d\n\n", end_index);
		if (min_index == end_index)
			break;

		/* Updating the costs by using the newly found partial min path */
		for (l = g->nodes[min_index].adj; l != NULL; l = l->next) {
			neighbour_index = tree_search(t, l->word);
	//		printf("neighbour_index = %d\n", neighbour_index);
	//		printf("cost[ni] = %g\n", costs[neighbour_index]);
			new_cost = costs[min_index] + l->cost;
	//		printf("new_cost = %g\n", new_cost);

			if (new_cost < costs[neighbour_index]) {
				costs[neighbour_index] = new_cost;
				prev[neighbour_index] = min_index;
				pq->update_node(neighbour_index, new_cost);
			}
		}
	}

	/*
	for (i = 0; i < g->size; i++)
		printf("cost de la %s la %s: %g\n", start, g->nodes[i].word, costs[i]);

	for (i = 0; i < g->size; i++)
		printf("%d -> %d |", prev[i], i);
	puts("");
	*/

	if (min_index == end_index)
		print_path(g, prev, start_index, end_index);
	else
		printf("path impossible.\n");
}

/* 
 * Finds the fixed cost path between two words.
 * Returns a pointer to an array of words that are part of the path
 */
char **wgraph_fixed_path(struct wgraph *g, struct tnode *t,
	       	int length, char *end)
{
	int *nodes;
	int len;

	nodes = (int *)malloc(g->size * sizeof(int));
	len = 0;
}

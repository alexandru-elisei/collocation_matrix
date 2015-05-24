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
void wgraph_add(struct wgraph *g, char *p, char *w, int position)
{
	/* Adding a new word */
	if (position == g->size) {
		if (g->size == g->mem_alloc) {
			g->mem_alloc += MEM_INC;
			g->nodes = (struct vertex *)realloc(g->nodes,
					g->mem_alloc * sizeof(struct vertex));
		}

		g->nodes[position].word = p;
		g->nodes[position].count = 1;
		g->nodes[position].adj = list_create();
		list_add(&g->nodes[position].adj, w);
		g->size++;
	} else {
		g->nodes[position].count++;
		list_add(&g->nodes[position].adj, w);
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
		unsigned int total_words)
{
	int i;
	int o11, o12, o21, o22;
	enum word_result r;

	if ((r = wgraph_empty(g)) != WORD_GRAPH_NOT_EMPTY)
		return r;

	for (i = 0; i < g->size; i++) {
	}
}


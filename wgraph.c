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
void wgraph_add(struct wgraph *g, char *w, int position)
{
	/* Adding a new word */
	if (position == g->size) {
		if (g->size == g->mem_alloc) {
			g->mem_alloc += MEM_INC;
			g->nodes = (struct vertex *)realloc(g->nodes,
					g->mem_alloc * sizeof(struct vertex));
		}
		g->nodes[position].word = w;
		g->nodes[position].count = 1;
		g->nodes[position].adj = list_create();
		g->size++;
	} else {
		g->nodes[position].count++;
	}
}

/* Prints the graph */
void wgraph_print(struct wgraph *g)
{
	int i;

	for (i = 0; i < g->size; i++) {
		printf("word = |%s|, count = %d\n", g->nodes[i].word,
				g->nodes[i].count);
		list_print(g->nodes[i].adj);
	}
}

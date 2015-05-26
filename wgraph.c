#include <math.h>

#include "wgraph.h"

/* Memory allocation increment */
#define MEM_INC		(100)

#define RESET_VISITED(v, step, nodes)			\
	do {						\
		int i;					\
		for (i = 0; i < (nodes); i++)		\
			v[(step)][(i)] = NOT_VISITED;	\
	} while (0)					\
			

/* 
 * Updates neighbours of node index by adding them to the priority queue if a
 * new path if found that costs less than the previous known path
 */
static inline void update_neighbours(struct wgraph *g, struct tnode *t,
		struct pqueue *pq, int index, float *costs, int *prev)
{
	struct lnode *l;
	int neighbour_index;
	float new_cost;

	for (l = g->nodes[index].adj; l != NULL; l = l->next) {
		neighbour_index = tree_search(t, l->word);
		new_cost = costs[index] + l->cost;

		if (new_cost < costs[neighbour_index]) {
			costs[neighbour_index] = new_cost;
			prev[neighbour_index] = index;
			pq->update_node(neighbour_index, new_cost);
		}
	}
}

/* Reverses the path */
static void reverse_path(char **path, int len)
{
	int i;
	char *aux;

	for (i = 0; i < len/2; i++) {
		aux = path[i];
		path[i] = path[len-1-i];
		path[len-1-i] = aux;
	}
}

/* Creates the array of words that form the path from start_index to end_index */
static char **create_path(struct wgraph *g, int start_index,
	       	int end_index, int *prev, int *len)
{
	int previous_node;
	int mem;
	char **path;

	mem = MEM_INC;
	*len = 0;

	path = (char **)malloc(mem * sizeof(char *));
	path[(*len)++] = g->nodes[end_index].word;

	previous_node = prev[end_index];
	while (previous_node != start_index) {
		if (*len == mem - 1) {
			mem += MEM_INC;
			path = (char **)realloc(path, mem * sizeof(char *));
		}

		path[(*len)++] = g->nodes[previous_node].word;
		previous_node = prev[previous_node];
	}
	path[(*len)++] = g->nodes[start_index].word;

	/* The path is from end_index to start_index, so we reverse if */
	reverse_path(path, *len);

	return path;
}

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
void wgraph_add(struct wgraph *g, char *prev, char *current, 
		int position, float cost)
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
		list_add(&g->nodes[position].adj, current, cost);
		g->size++;
	} else {
		g->nodes[position].count++;
		list_add(&g->nodes[position].adj, current, cost);
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
	printf("%s ", g->nodes[p[end]].word);
}

/* 
 * Finds the minimum cost path between two words by using Dijkstra's algorithm.
 * Returns a pointer to an array of words that are part of the path
 */
char **wgraph_min_path(struct wgraph *g, struct tnode *t,
	       	char *start, char *end, int *len)
{
	struct pqueue *pq;

	/* 
	 * Index is the head of the arc, value is the tail of the arc, both
	 * stored as indices in the graph  
	 */
	int *prev;
	float *costs;		/* cost between start and node array index */
	char **path = NULL;

	int start_index, end_index;
	int min_index = -1;
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

	/* Initializing costs and previous nodes array */
	for (i = 0; i < g->size; i++) {
		prev[i] = NODE_NOT_FOUND;
		costs[i] = INF;
	}

	prev[start_index] = start_index;
	costs[start_index] = 0;

	/* Adding the neighbors of start word to the priority queue */
	update_neighbours(g, t, pq, start_index, costs, prev);

	/* Finding minimum cost path for each node until we find the end word */
	while (pq->is_empty() != 1) {
		min_index = pq->extract_min();
		/* Reached the end word */
		if (min_index == end_index)
			break;

		/* Updating the costs by using the new found min path */
		update_neighbours(g, t, pq, min_index, costs, prev);
	}

	/* Found path */
	 if (min_index == end_index) {
		path = create_path(g, start_index, end_index, prev, len);
		return path;
	 } else {
		return NULL;
	 }
}

static void transpose_graph(struct wgraph *orig, struct wgraph **trans,
		struct tnode **trans_tree)
{
	struct lnode *l;
	int i, index;

	*trans = wgraph_create();
	*trans_tree = tree_create();

	for (i = 0; i < orig->size; i++) {
		/* Adding current node */
		index = tree_add(trans_tree, orig->nodes[i].word, (*trans)->size);
		wgraph_add(*trans, orig->nodes[i].word, NULL, index, COST_UNKNOWN);	

		/* And it's neighbours */
		for (l = orig->nodes[i].adj; l != NULL; l = l->next) {
			index = tree_add(trans_tree, l->word, (*trans)->size);
			wgraph_add(*trans, l->word, orig->nodes[i].word,
					index, l->cost);
		}
	}
}

/* 
 * Finds the fixed cost path that ends with word end.
 * Returns a pointer to an array of words that are part of the path
 */
char **wgraph_fixed_path(struct wgraph *g, struct tnode *t,
	       	int length, char *end, int *num_of_paths)
{
	struct wgraph *trans_graph;
	struct tnode *trans_tree;
	struct lnode *nghb;

	int *path;		/* current path as indices in the graph */
	int **visited;		/* visited nodes at current step */
	int nghb_index;
	int step;
	float *costs, cost;
	int *min_cost_path;
	float min_cost;
	int i,j;

	/* Using the transposed graph to find the path */
	transpose_graph(g, &trans_graph, &trans_tree);
	//wgraph_print(trans_graph);
	num_of_paths = 0;

	path = (int *)malloc(length * sizeof(int));
	visited = (int **)malloc(length * sizeof(int *));
	min_cost_path = (int *)malloc(length * sizeof(int));
	costs = (float *)malloc(length * sizeof(float));

	for (i = 0; i < length; i++) {
		visited[i] = (int *)malloc(g->size * sizeof(int));
		for (j = 0; j < g->size; j++)
			visited[i][j] = NOT_VISITED;
		costs[i] = 0;
	}

	path[0] = tree_search(trans_tree, end);
	step = 0;
	cost = 0;
	min_cost = INF;
	/* While I have not yet exhausted all the neighbours */
	while (step >= 0) {	

		nghb = trans_graph->nodes[path[step]].adj;
		for (; nghb != NULL; nghb = nghb->next) {
			nghb_index = tree_search(trans_tree, nghb->word);
			if (visited[step][nghb_index] == NOT_VISITED) {
					costs[step] = nghb->cost;
					break;
			}
		}

		/* Found no unvisited neighbour at current step, backtracking */
		if (nghb == NULL) {
			for (i = 0; i < g->size; i++)
				visited[step][i] = NOT_VISITED;
			step--;
			if (step >= 0)
				cost -= costs[step];
			continue;
		}

		cost += costs[step];
		visited[step++][nghb_index] = VISITED;
		path[step] = nghb_index;
		if (step == length - 1) {

			/*
			for (i = 0; i < length; i++)
				printf("%s ", trans_graph->nodes[path[i]].word);
			printf("\n");
			*/

			if (cost < min_cost) {
				for (i = 0; i < length; i++) {
					min_cost_path[i] = path[i];
				}
				min_cost = cost;
			}
			for (i = 0; i < g->size; i++)
				visited[step][i] = NOT_VISITED;
			step--;
			cost -= costs[step];
		}
	}

	printf("\n\n");

	FILE *out;
	out = fopen("outc", "a+");
	for (i = length-1; i >= 0; i--)
		fprintf(out, "%s ", trans_graph->nodes[min_cost_path[i]].word);
	fprintf(out, "\n");
	fclose(out);

}

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
static inline void update_neighbours(struct wgraph *g, struct pqueue *pq, 
		int index, float *costs, int *prev)
{
	struct lnode *l;
	int neighbour_index;
	float new_cost;

	for (l = g->nodes[index].adj; l != NULL; l = l->next) {
		neighbour_index = l->graph_index;
		new_cost = costs[index] + l->cost;

		if (new_cost < costs[neighbour_index]) {
			costs[neighbour_index] = new_cost;
			prev[neighbour_index] = index;
			pq->update_node(neighbour_index, new_cost);
		}
	}
}

/* Prints the minimum cost path to file */
void print_minpath_to_file(struct wgraph *g, int *path, 
		int start, int end, FILE *out)
{
	if (path[end] != start)
		print_minpath_to_file(g, path, start, path[end], out);

	fprintf(out, "%s ", g->nodes[path[end]].word);
}

/* Returns a graph with zero elements */
struct wgraph *wgraph_create()
{
	struct wgraph *ret;

	ret = (struct wgraph *)malloc(sizeof(struct wgraph));
	ret->size = 0;
	ret->total_words = 0;
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
void wgraph_add(struct wgraph *g, char *w, int index, int *prev, float cost)
{
	/* Adding a new word */
	if (index == g->size) {
		if (g->size == g->mem_alloc) {
			g->mem_alloc += MEM_INC;
			g->nodes = (struct vertex *)realloc(g->nodes,
					g->mem_alloc * sizeof(struct vertex));
		}

		g->nodes[index].word = w;
		g->nodes[index].count = 1;
		g->nodes[index].adj = list_create();
		if (*prev != NODE_NOT_FOUND)
			list_add(&g->nodes[*prev].adj, w, index, cost);
		g->size++;
	/* Updating a previously read word */
	} else {
		g->nodes[index].count++;
		if (*prev != NODE_NOT_FOUND)
			list_add(&g->nodes[*prev].adj, w, index, cost);
	}
	g->total_words++;
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
enum word_result wgraph_calculate_costs(struct wgraph *g)
{
	struct lnode *l;
	enum word_result r;

	int i;
	int o11, o12, o21, o22;
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
			o21 = g->nodes[l->graph_index].count - o11;

			o22 = (g->total_words - 1) - o11 - o12 - o21;

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

/* 
 * Finds the minimum cost path between two words by using Dijkstra's algorithm.
 * Returns a pointer to an array of words that are part of the path
 */
enum word_result wgraph_min_path(struct wgraph *g, struct tnode *t,
	       	char *start, char *end, FILE *out)
{
	struct pqueue *pq;
	enum word_result r;

	/* 
	 * Index is the head of the arc, value is the tail of the arc, both
	 * stored as indices in the graph  
	 */
	int *prev;
	float *costs;		/* cost between start and node array index */

	int start_index, end_index;
	int min_index = -1;
	int i;

	if ((r = wgraph_empty(g)) != WORD_GRAPH_NOT_EMPTY)
		return r;

	start_index = tree_search(t, start);
	end_index = tree_search(t, end);

	if (start_index == NODE_NOT_FOUND ||
			end_index == NODE_NOT_FOUND ||
			g->nodes[start_index].adj == NULL)
		return WORD_NO_MIN_PATH_FOUND;

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
	update_neighbours(g, pq, start_index, costs, prev);

	/* Finding minimum cost path for each node until we find the end word */
	while (pq->is_empty() != 1) {
		min_index = pq->extract_min();
		/* Reached the end word */
		if (min_index == end_index)
			break;

		/* Updating the costs by using the new found min path */
		update_neighbours(g, pq, min_index, costs, prev);
	}

	/* Found path */
	 if (min_index == end_index) {
		print_minpath_to_file(g, prev, start_index, end_index, out);
		fprintf(out, "%s\n", g->nodes[end_index].word);
		return WORD_SUCCESS;
	 } else {
		return WORD_NO_MIN_PATH_FOUND;
	 }
}

static void transpose_graph(struct wgraph *orig, struct wgraph **trans,
		struct tnode **trans_tree)
{
	struct lnode *l;
	int i, head_index, tail_index;
	int no_previous;

	*trans = wgraph_create();
	*trans_tree = tree_create();

	no_previous = NODE_NOT_FOUND;
	for (i = 0; i < orig->size; i++) {
		/* Adding current node */
		head_index = tree_add(trans_tree, orig->nodes[i].word,
				(*trans)->size);
		wgraph_add(*trans, orig->nodes[i].word, head_index, 
				&no_previous, COST_UNKNOWN);	

		/* And it's neighbours */
		for (l = orig->nodes[i].adj; l != NULL; l = l->next) {
			tail_index = tree_add(trans_tree, l->word, (*trans)->size);
			wgraph_add(*trans, l->word, tail_index, 
					&no_previous, l->cost);
			/* Adding current node as a neighbour */
			list_add(&((*trans)->nodes[tail_index].adj),
					orig->nodes[i].word, head_index,
					l->cost);
		}
	}
}

/* 
 * Finds the fixed cost path that ends with word end.
 * Returns a pointer to an array of words that are part of the path
 */
enum word_result wgraph_fixed_path(struct wgraph *g, struct tnode *t,
	       	int length, char *end, FILE *out)
{
	struct wgraph *trans_graph;
	struct tnode *trans_tree;
	struct lnode *nghb;

	int *path;		/* current path as indices in the graph */
	int *visited;		/* number of visited nodes at current step */
	int step;		/* iteration step */

	float *costs;		/* costs added at each step */
       	float path_cost;	/* current path cost */
	float *path_costs;	/* the costs for each discovered path */
	int **discovered_paths;	/* array of all possible paths */
	/* Allocated memory for path_costs and discovered_paths */
	int path_mem;		
	int paths;		/* number of paths found */

	int *min_cost_path;	/* indices in the graph for the min path */
	float min_cost;	
	int min_paths;

	int i,j, nghb_num;

	/* 
	 * Using the transposed graph to find the path, end node becomes the
	 * start node in the path
	 */
	transpose_graph(g, &trans_graph, &trans_tree);

	path = (int *)malloc(length * sizeof(int));
	min_cost_path = (int *)malloc(length * sizeof(int));
	costs = (float *)malloc(length * sizeof(float));

	visited = (int *)malloc(length * sizeof(int *));
	for (i = 0; i < length; i++)
		visited[i] = 0;

	path_mem = MEM_INC;
	path_costs = (float *)malloc(path_mem * sizeof(float));
	discovered_paths = (int **)malloc(path_mem * sizeof(int *));

	for (i = 0; i < path_mem; i++)
		discovered_paths[i] = (int *)malloc(length * sizeof(int));

	path[0] = tree_search(trans_tree, end);
	paths = 0;
	step = 0;
	path_cost = 0;
	min_cost = INF;

	/* While I have not yet exhausted all the neighbours */
	while (step >= 0) {	

		nghb = trans_graph->nodes[path[step]].adj;
		nghb_num = 0;

		/* Searching for the next unvisited neighbour */
		for (; nghb != NULL; nghb = nghb->next) {
			nghb_num++;
			if (nghb_num > visited[step]) {
				costs[step] = nghb->cost;
				break;
			}
		}

		/* 
		 * All neighbours visited or cost larger than previous minimum,
		 * backtracking 
		 */
		if (nghb == NULL) {
			visited[step] = 0;
			step--;
			if (step >= 0)
				path_cost -= costs[step];
			continue;
		}

		path_cost += costs[step];
		visited[step]++;
		step++;
		path[step] = nghb->graph_index;

		/* Found path */
		if (step == length - 1) {
			if (path_cost <= min_cost + PRECISION) {
				for (i = 0; i < length; i++) {
					min_cost_path[i] = path[i];
				}
				min_cost = path_cost;
			}

			/* Reallocating memory */
			if (paths == path_mem) {
				path_mem += MEM_INC;
				path_costs = (float *)realloc(path_costs,
						path_mem * sizeof(float));
				discovered_paths = (int **)realloc(
						discovered_paths,
						path_mem * sizeof(int *));
				for (i = path_mem - MEM_INC; i < path_mem; i++)
					discovered_paths[i] = (int *)malloc(
							length * sizeof(int));
			}

			/* Copying path costs and nodes */
			path_costs[paths] = path_cost;
			for (i = 0; i < length; i++)
				discovered_paths[paths][i] = path[i];
			paths++;

			visited[step] = 0;
			step--;
			path_cost -= costs[step];
		}
	}

	/* Calculating number of paths with the same minimum cost */
	min_paths = 0;
	for (i = 0; i < paths; i++)
		if (path_costs[i] - min_cost > -PRECISION &&
		  path_costs[i] - min_cost < PRECISION ) {
			min_paths++;
		}
	fprintf(out, "%d\n", min_paths);

	/* Printing the paths with same minimum cost */
	for (i = 0; i < paths; i++)
		if (path_costs[i] - min_cost > -PRECISION &&
		  path_costs[i] - min_cost < PRECISION ) {
			fprintf(out, "%s", trans_graph->nodes[discovered_paths[i][length-1]].word);
			for (j = length-2; j >= 0; j--)
				fprintf(out, " %s", trans_graph->nodes[discovered_paths[i][j]].word);
			fprintf(out, "\n");
		}

	/* No path of specified length found */
	if (min_cost == INF)
		return WORD_NO_FIXED_LENGTH_PATH;
	else
		return WORD_SUCCESS;
}

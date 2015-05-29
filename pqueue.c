#include "pqueue.h"

#define HEAP_MEM_INC	(20)

struct stripped_graph_node {
	int index;			/* index in the graph */
	float cost;			/* path cost */
};

struct heap {
	struct stripped_graph_node *nodes;
	int size;
	int mem_alloc;
};

/* Inserts a new element into the heap */
enum word_result insert(int index, float cost);

/* Returns and removes the min cost node */
int extract_min();

/* Returns 1 if empty, 0 otherwise */
int is_empty();

/* Rearranges the heap from the bottom up so the heap conditions are met */
static enum word_result sift_up();

/* Rearranges the heap from the top down so the heap conditions are met */
static enum word_result sift_down(int root);

/* Prints the heap */
static enum word_result print();

/* Updates the cost for a node */
void update_node(int graph_index, float new_cost);

/* The priority queue is stored internally as a heap */
static struct heap *h = NULL;	
static int *map_graph_to_heap = NULL;	/* maps graph index to heap index */
static int map_size;

static inline int get_parent(int index)
{
	if (index == 1 || index == 0 || index == 2)
		return 0;

	if (index % 2 == 0)
		return index / 2 - 1; 
	else
		return index / 2;
}

static inline void delete_node(int index)
{
	map_graph_to_heap[h->nodes[index].index] = NODE_NOT_FOUND;

	h->nodes[index] = h->nodes[h->size - 1];
	map_graph_to_heap[h->nodes[index].index] = index;
	h->size--;
	sift_down(index);
}

#ifdef DEBUG
static void print_map()
{
	int i;

	printf("\tmap:\n");
	for (i = 0; i < map_size; i++)
		printf("(graph_index) %3d -> %3d (heap_index)\n", i, map_graph_to_heap[i]);
	printf("\n");
}
#endif

/* Assigns struct functions and initializes internal variables */
struct pqueue *pqueue_create(int graph_size)
{
	struct pqueue *ret;
	int i;

	ret = (struct pqueue *)malloc(sizeof(struct pqueue));
	ret->insert = insert;
	ret->print = print;
	ret->extract_min = extract_min;
	ret->is_empty = is_empty;
	ret->update_node = update_node;

	h = (struct heap *) malloc(sizeof(struct heap));
	if (h == NULL)
		return NULL;

	h->size = 0;
	/* Allocating memory for the heap elements */
	h->mem_alloc = HEAP_MEM_INC;
	h->nodes = (struct stripped_graph_node *)malloc(
		h->mem_alloc * sizeof(struct stripped_graph_node));

	/* Allocating memory for the map array */
	map_graph_to_heap = (int *)malloc(graph_size * sizeof(int));
	map_size = graph_size;
	for (i = 0; i < map_size; i++)
		map_graph_to_heap[i] = NODE_NOT_FOUND;

	return ret;
}

/* Removes struct functions and frees memory */
struct pqueue *pqueue_destroy(struct pqueue *pq)
{
	free(h->nodes);
	free(h);
	h = NULL;
	free(map_graph_to_heap);
	map_graph_to_heap = NULL;
	free(pq);

	return NULL;
}

/* Prints the heap */
static enum word_result print()
{
	int i;

	if (h == NULL || h->size == 0)
		return WORD_ERROR_QUEUE_NOT_INITIALIZED;

	printf("\n\theap:\n");
	for (i = 0; i < h->size; i++)
		printf("%d: index = %3d, cost = %6.6g\n",
				i, h->nodes[i].index, h->nodes[i].cost);
	printf("\n");

	return WORD_SUCCESS;
}

/* Inserts a new element into the heap */
enum word_result insert(int index, float cost)
{
	enum word_result r;
	int pos;

	if (h == NULL)
		return WORD_ERROR_QUEUE_NOT_INITIALIZED;

	if (h->size == h->mem_alloc) {
		h->mem_alloc += HEAP_MEM_INC;
		h->nodes = (struct stripped_graph_node *)realloc(h->nodes,
			h->mem_alloc * sizeof(struct stripped_graph_node));
	}

	pos = h->size;
	h->nodes[pos].index = index;
	h->nodes[pos].cost = cost;
	h->size++;

	r = sift_up();

	/*
	printf("AFTER INSERT:\n");
	print();
	print_map();
	*/

	return r;
}

/* Returns 1 if empty, 0 otherwise */
int is_empty()
{
	if (h == NULL)
		return 1;

	return !(h->size);
}

/* Rearranges the heap from the bottom up so the heap conditions are met */
static enum word_result sift_up()
{
	struct stripped_graph_node tmp;
	int index, parent;

	if (h == NULL || h->size == 0)
		return WORD_ERROR_QUEUE_NOT_INITIALIZED;

	if (h->size == 1) {
		map_graph_to_heap[h->nodes[0].index] = 0;
		return WORD_SUCCESS;
	}

	index = h->size - 1;
	parent = get_parent(index);
	tmp = h->nodes[index];

	/* Moving the last element up the heap until heap conditions are met */
	while (index > 0 && h->nodes[parent].cost > tmp.cost) {
		h->nodes[index] = h->nodes[parent];
		map_graph_to_heap[h->nodes[index].index] = index;
		index = parent;
		parent = get_parent(index);
	}

	h->nodes[index].index = tmp.index;
	h->nodes[index].cost = tmp.cost;

	/*
	printf("SIFT UP:\n");
	printf("index = %d, tmp.index = %d\n", index, tmp.index);
	*/

	map_graph_to_heap[tmp.index] = index;

	return WORD_SUCCESS;
}

/* Rearranges the heap from the top down so the heap conditions are met */
static enum word_result sift_down(int root)
{
	struct stripped_graph_node tmp;
	int index, left_child, right_child;
	int last_index;
	int min;

	if (h == NULL)
		return WORD_ERROR_QUEUE_NOT_INITIALIZED;

	if (h->size == 0)
		return WORD_SUCCESS;

	/* Moving the root downwards */
	tmp = h->nodes[root];
	index = root;
	last_index = h->size - 1;
	/* While we have a left child */
	while (2 * index < last_index) {
		left_child = 2 * index + 1;
		right_child = 2 * index + 2;

		/* If we have both children we find out which child is smaller */
		if (right_child <= last_index) {
			if (h->nodes[left_child].cost <= h->nodes[right_child].cost)
				min = left_child;
			else
				min = right_child;
		/* We only have a left child */
		} else {
			min = left_child;
		}

		/* Root is smaller than both children */
		if (h->nodes[min].cost >= tmp.cost) {
			break;
		} else {
			h->nodes[index] = h->nodes[min];
			map_graph_to_heap[h->nodes[index].index] = index;
			index = min;
		}
	}
	h->nodes[index].index = tmp.index;
	h->nodes[index].cost = tmp.cost;
	map_graph_to_heap[tmp.index] = index;

	return WORD_SUCCESS;
}

/* Returns and removes the min cost node */
int extract_min()
{
	int res;

	res = h->nodes[0].index;
	delete_node(0);

	return res;
}

/* Updates the cost for a node */
void update_node(int graph_index, float new_cost)
{
	int i;

	if (h == NULL)
		return;

	/* 
	 * Inefficient search for the node - to make an array which maps
	 * graph_index to heap index
	 */
	for (i = 0; i < h->size; i++)
		if (h->nodes[i].index == graph_index)
			break;

	/* Node not found */
	if (map_graph_to_heap[graph_index] == NODE_NOT_FOUND) {
		insert(graph_index, new_cost);
	} else {
		delete_node(i);
		insert(graph_index, new_cost);
	}
}

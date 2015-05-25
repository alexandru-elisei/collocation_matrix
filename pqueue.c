#include "pqueue.h"

#define HEAP_MEM_INC	(20)

static struct stripped_graph_node {
	int index;			/* index in the graph */
	float cost;			/* path cost */
};

static struct heap {
	struct stripped_graph_nodes *nodes;
	unsigned int size;
	unsigned int mem_alloc;
};

/* Inserts a new element into the heap */
enum word_result (*insert)(struct lnode *node)
{
	return WORD_SUCCESS;
}

/* Returns and removes the min cost node */
int extract_min()
{
	return -1;
}

/* Rearranges the heap from the bottom up so the heap conditions are met */
static void sift_up()
{
}

/* Rearranges the heap from the top down so the heap conditions are met */
static void sift_down(unsigned int root)
{
}

/* Prints the heap */
static enum word_result print()

/* The priority queue is stored internally as a heap */
static struct heap *h = NULL;	

static inline void delete_node(unsigned int index)
{
	h->nodes[index] = h->nodes[h->size - 1];
	h->size--;
	sift_down(index);
}

/* Assigns struct functions and initializes internal variables */
struct pqueue *pqueue_create()
{
	struct pqueue *ret;

	ret = (struct pqueue *)malloc(sizeof(struct pqueue));
	ret->insert = insert;
	ret->print = print;
	ret->extract_min = extract_min;

	h = (struct heap *) malloc(sizeof(struct heap));
	if (h == NULL)
		return NULL;

	h->size = 0;
	/* Allocating memory for the heap elements */
	h->mem_alloc = HEAP_MEM_INC;
	h->nodes = (struct stripped_graph_node *)malloc(
			sizeof(struct stripped_graph_node));

	return ret;
}

/* Removes struct functions and frees memory */
struct pqueue *pqueue_destroy(struct pqueue *pq)
{
	free(h->nodes);
	free(h);
	h = NULL;
	free(*pq);

	return NULL;
}

/* Prints the heap */
static enum word_result print()
{
	int i;

	if (h == NULL || h->size == 0)
		return WORD_ERROR_HEAP_NOT_INITIALIZED;

	printf("\n\t\theap:\n");
	for (i = 0; i < h->size; i++)
		printf("%d: index = %3d, cost = %6.6g\n",
				i, h->nodes[i].index, h->nodes[i].cost);
	printf("\n");

	return WORD_SUCCESS;
}

#if 0
/* Inserts a new element into the heap */
static enum huf_result insert(struct tmp_huf_node *c, uint16_t index)
{
	enum huf_result r;
	uint16_t pos;

	if (h == NULL)
		return HUF_ERROR_QUEUE_NOT_INITIALIZED;

	if (h->size == h->max_size)
		return HUF_ERROR_QUEUE_SIZE_EXCEEDED;

	pos = h->size;
	h->huf_nodes[pos].freq = c->freq;
	h->huf_nodes[pos].index = index;
	h->size++;

	r = sift_up();

	return r;
}

/* Rearranges the heap from the bottom up so the heap conditions are met */
static enum huf_result sift_up()
{
	struct heap_huf_node tmp;
	int index, parent;

	if (h == NULL || h->size == 0)
		return HUF_ERROR_QUEUE_NOT_INITIALIZED;

	if (h->size == 1)
		return HUF_SUCCESS;

	index = h->size - 1;
	parent = get_parent(index);
	tmp = h->huf_nodes[index];

	/* Moving the last element up the heap until heap conditions are met */
	while (index > 0 && h->huf_nodes[parent].freq > tmp.freq) {
		h->huf_nodes[index] = h->huf_nodes[parent];
		index = parent;
		parent = get_parent(index);
	}

	h->huf_nodes[index].index = tmp.index;
	h->huf_nodes[index].freq = tmp.freq;

	return HUF_SUCCESS;
}

/* Rearranges the heap from the top down so the heap conditions are met */
static enum huf_result sift_down(uint16_t root)
{
	struct heap_huf_node tmp;
	int index, left_child, right_child;
	int last_index;
	int min;

	if (h == NULL || h->size == 0)
		return HUF_ERROR_QUEUE_NOT_INITIALIZED;

	if (h->size == 1)
		return HUF_SUCCESS;

	/* Moving the root downwards */
	tmp = h->huf_nodes[root];
	index = root;
	last_index = h->size - 1;
	/* While we have a left child */
	while (2 * index < last_index) {
		left_child = 2 * index + 1;
		right_child = 2 * index + 2;

		/* If we have both children we find out which child is smaller */
		if (right_child <= last_index) {
			if (h->huf_nodes[left_child].freq <= h->huf_nodes[right_child].freq)
				min = left_child;
			else
				min = right_child;
		/* We only have a left child */
		} else {
			min = left_child;
		}

		/* Root is smaller than both children */
		if (h->huf_nodes[min].freq >= tmp.freq) {
			break;
		} else {
			h->huf_nodes[index] = h->huf_nodes[min];
			index = min;
		}
	}
	h->huf_nodes[index].index = tmp.index;
	h->huf_nodes[index].freq = tmp.freq;

	return HUF_SUCCESS;
}
#endif
		

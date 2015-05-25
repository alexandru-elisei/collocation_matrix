/*
 * Priority queue, implemented as a heap
 */

#ifndef PQUEUE_H
#define PQUEUE_H

#include "common.h"
#include "list.h"

struct pqueue {
	enum word_result (*insert)(int index, float cost);
	int (*extract_min)();			
	int (*is_empty)();
	void (*update_node)(int graph_index, float new_cost);
	enum word_result (*print)();
};

/* Assigns struct functions and initializes internal variables */
struct pqueue *pqueue_create(int graph_size);

/* Removes struct functions and frees memory */
struct pqueue *pqueue_destroy(struct pqueue *pq);

#endif	/* ifndef PQUEUE.H */

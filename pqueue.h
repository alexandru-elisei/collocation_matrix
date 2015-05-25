/*
 * Priority queue, implemented as a heap
 */

#ifndef PQUEUE_H
#define PQUEUE_H

#include "common.h"
#include "list.h"

struct pqueue {
	enum word_result (*insert)(struct lnode *node);
	int (*get_min)();			
	enum word_result (*print)();
};

/* Assigns struct functions and initializes internal variables */
struct pqueue *pqueue_create();

/* Removes struct functions and frees memory */
struct pqueue *pqueue_destroy(struct pqueue *pq);

#endif	/* ifndef PQUEUE.H */

/*
 * The words will be stored in a hash table with separate chaining with
 * linked lists. The hashing key is the sum of the ASCII codes % HASH_SIZE.
 */

#ifndef HASH_H
#define HASH_H

#include "common.h"

#define HASH_SIZE	(499)

/* Creates and array of list nodes */
struct node **hash_create();

/* Deallocates memory and return NULL */
struct node **hash_destroy(struct node **h);

/* Adds an element to the hash */
enum word_result has_add(struct node **h, char *c);

#endif	/* ifndef HASH_H */

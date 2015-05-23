/*
 * The words will be stored in a hash table with separate chaining with
 * linked lists. The hashing key is the sum of the ASCII codes % HASH_SIZE.
 */

#ifndef HASH_H
#define HASH_H

#include "common.h"
#include "list.h"

/* 
 * Estimating that the words have, on average, five letters, and the sum of
 * their ASCII codes is somewhere between 500 and 600
 */
#define HASH_SIZE	(557)

/* Creates and array of list nodes */
struct node **hash_create();

/* Deallocates memory and return NULL */
struct node **hash_destroy(struct node **h);

/* Adds an element to the hash */
enum word_result hash_add(struct node **h, char *c);

/* Prints the hash */
void hash_print(struct node **h);

#endif	/* ifndef HASH_H */

/*
 * Various data types and functions shared across the program
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FOREVER		(1)
#define LINE_LEN	(10000)
#define WORD_LEN	(30)
#define SEP		("\n- ")
#define WORD_NOT_FOUND	(-1)


/* Defined when compiling */
#ifdef DEBUG

#define DEBMSG(msg)	(printf("## %s:%d: " #msg " is \"%s\" (in %s)\n",			\
		       	__FILE__, __LINE__, (msg), __FUNCTION__))

#define DEBINFO(exp)	(printf("## %s:%d: " #exp " evaluates to %d (in %s)\n",	\
		       	__FILE__, __LINE__, (exp), __FUNCTION__))

#else

#define DEBMSG(msg)	do {} while (0)
#define DEBINFO(exp)	do {} while (0)

#endif

/* Result types for function returns */
enum word_result {
	WORD_SUCCESS			= 1, 	
	WORD_ERROR_FILE_ACCESS		= 2,	/* Cannot open file */
	WORD_ERROR_INVALID_ARGUMENTS	= 3,
	WORD_ERROR_EMPTY_LIST		= 4,
	WORD_GRAPH_IS_EMPTY		= 5,
	WORD_GRAPH_NOT_EMPTY		= 6,
	WORD_DUPLICATE			= 7,
	WORD_ERROR_GRAPH_NOT_INITIALIZED= 8,
	WORD_ERROR_HEAP_NOT_INITIALIZED	= 9,
	WORD_ERROR_UNKNOWN_ERROR	= 99,	
};

/* Prints the messages associated with the result codes */
void word_print_result(enum word_result msg);

#endif	/* #ifndef COMMON_H */

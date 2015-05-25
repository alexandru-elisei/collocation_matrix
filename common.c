#include "common.h"

#define PRINTERR(msg)	fprintf(stderr, "[ ERROR ] %s", msg)

/* Prints the message associated with the defined result codes */
void word_print_result(enum word_result msg)
{
	/* I don't like switch */
	if (msg == WORD_SUCCESS)
		fprintf(stderr, "[  OK!  ] Operation executed successfully.\n");
	if (msg == WORD_DUPLICATE)
		fprintf(stderr, "[  OK!  ] Duplicate word found in binary tree.\n");
	else if (msg == WORD_ERROR_FILE_ACCESS)
		PRINTERR("Cannot access file.\n");
	else if (msg == WORD_ERROR_INVALID_ARGUMENTS)
		PRINTERR("Invalid arguments supplied.\n");
	else if (msg == WORD_ERROR_EMPTY_LIST)
		PRINTERR("Empty list supplied.\n");
	else if (msg == WORD_ERROR_GRAPH_NOT_INITIALIZED)
		PRINTERR("Empty graph supplied.\n");
	else if (msg == WORD_ERROR_HEAP_NOT_INITIALIZED)
		PRINTERR("Empty heap supplied.\n");
	else if (msg == WORD_ERROR_UNKNOWN_ERROR)
		PRINTERR("Unknown error occured.\n");
}

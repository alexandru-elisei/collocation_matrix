#include "common.h"

#define PRINTERR(msg)	fprintf(stderr, "[ ERROR ] %s", msg)

/* Prints the message associated with the defined result codes */
void word_print_result(enum word_result msg)
{
	/* I don't like switch */
	if (msg == WORD_SUCCESS)
		fprintf(stderr, "[  OK!  ] Operation executed successfully.\n");
	else if (msg == WORD_ERROR_FILE_ACCESS)
		PRINTERR("Cannot access file.\n");
	else if (msg == WORD_ERROR_UNKNOWN_ERROR)
		PRINTERR("Unknown error occured.\n");
}

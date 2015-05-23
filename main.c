#include "common.h"

#define CHKRES(r)						\
	do { 							\
		if ((r) != WORD_SUCCESS) {			\
			word_print_result((r));			\
			if (in != NULL)				\
				fclose(in);			\
			if (text != NULL)			\
				fclose(out);			\
			if (out != NULL)			\
				fclose(out);			\
			exit((r));				\
		}						\
	} while (0)		

int main(int argc, char **argv)
{
	FILE *in = NULL; 	/* Fisierul cu datele de intrare */
	FILE *text = NULL;	/* Fisierul cu textul */
	FILE *out = NULL;	/* Fisierul de iesire */

	if (argc < 3)
		CHKRES(WORD_ERROR_INVALID_ARGUMENTS);

	in = fopen(argv[1], "r");
	if (in == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	out = fopen(argv[2], "w");
	if (out == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	return EXIT_SUCCESS;
}



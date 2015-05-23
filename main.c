#include "common.h"
#include "list.h"
#include "tree.h"

#define CHKRES(r)						\
	do { 							\
		if ((r) != WORD_SUCCESS && r!= WORD_DUPLICATE) {\
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

/* Reads the input file */
enum word_result get_input(FILE *in, FILE *text, 
		char ***cost, int *cost_no,
		char ***min, int *min_no,
		char ***fixed_words, int **fixed_lengths, int *fixed_no);
/* Reads the text file */
enum word_result get_text(FILE *text, struct tnode **t)

int main(int argc, char **argv)
{
	FILE *in = NULL; 	/* Fisierul cu datele de intrare */
	FILE *text = NULL;	/* Fisierul cu textul */
	FILE *out = NULL;	/* Fisierul de iesire */

	struct lnode *l;
	struct tnode *search_tree;

	
	char **cost_words;	/* words needed for printing the cost */
	int cost_no;		/* number of words in the cost_words array */
	char **min_words;	/* words for the minimum cost path */
	int min_no;		/* number of words in the min_words array */
	char **fixed_words;	/* words for the fixed length path */
	int *fixed_lengths;	/* length for the fixed length path */
	int fixed_no;		/* number of words in the fixed_words array */

	int i;

	if (argc < 3)
		CHKRES(WORD_ERROR_INVALID_ARGUMENTS);

	if ((in = fopen(argv[1], "r")) == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	if ((out = fopen(argv[2], "w")) == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	CHKRES(get_input(in, text, &cost_words, &cost_no,
				&min_words, &min_no,
				&fixed_words, &fixed_lengths, &fixed_no));
#ifdef DEBUG
	printf("\ncost_words:\n");
	for (i = 0; i < cost_no * 2; i++)
		printf("|%s|\n", cost_words[i]);

	printf("\nmin_words:\n");
	for (i = 0; i < min_no * 2; i++)
		printf("|%s|\n", min_words[i]);

	printf("\nfixed_words:\n");
	for (i = 0; i < fixed_no; i++)
		printf("%d - |%s|\n", fixed_lengths[i], fixed_words[i]);
#endif
	search_tree = tree_create();

	get_text(text, &search_tree);

	return EXIT_SUCCESS;
}

enum word_result get_input(FILE *in, FILE *text, 
		char ***cost, int *cost_no,
		char ***min, int *min_no,
		char ***fixed_words, int **fixed_lengths, int *fixed_no)
{
	char buffer[LINE_LEN];
	int i;
	int index;

	fgets(buffer, LINE_LEN, in);
	buffer[strlen(buffer) - 1] = '\0';
	if ((text = fopen(buffer, "r")) == NULL)
		return WORD_ERROR_FILE_ACCESS;

	fgets(buffer, LINE_LEN, in);
	sscanf(buffer, "%d", cost_no);

	/* Reading the words we need to print the cost */
	*cost = (char **) malloc((2 * (*cost_no)) * sizeof(char *));
	index = 0;
	for (i = 0; i < *cost_no; i++) {
		fgets(buffer, LINE_LEN, in);
		(*cost)[index++] = strdup(strtok(buffer, SEP));
		(*cost)[index++] = strdup(strtok(NULL, SEP));
	}

	fgets(buffer, LINE_LEN, in);
	sscanf(buffer, "%d", min_no);

	/* Reading the words we need to calculate the minimum cost path */
	*min = (char **) malloc((2 * (*min_no)) * sizeof(char *));
	index = 0;
	for (i = 0; i < *min_no; i++) {
		fgets(buffer, LINE_LEN, in);
		(*min)[index++] = strdup(strtok(buffer, SEP));
		(*min)[index++] = strdup(strtok(NULL, SEP));
	}

	fgets(buffer, LINE_LEN, in);
	sscanf(buffer, "%d", fixed_no);

	/* Reading the words we need for the fixed length path */
	*fixed_words = (char **) malloc(*fixed_no * sizeof(char *));
	*fixed_lengths = (int *) malloc(*fixed_no * sizeof(int));
	index = 0;
	for (i = 0; i < *fixed_no; i++) {
		fgets(buffer, LINE_LEN, in);

		sscanf(buffer, "%d", &((*fixed_lengths)[i]));
		strtok(buffer, SEP);
		(*fixed_words)[index++] = strdup(strtok(NULL, SEP));
	}

	return WORD_SUCCESS;
}

/* Reads the text file */
enum word_result get_text(FILE *text, struct tnode **t)
{
	char buffer[LINE_LEN];
	char word[WORD_LEN];
	int word_index;
	int i;

	word[0] = '\0';
	word_index = 0;
	while (fgets(buffer, LINE_LEN, text) != NULL)
	for (i = 0; buffer[i] != '\0'; i++) {
		/* 
		 * Making sure there aren't two separators one after the
		 * other
		 */
		if (strchr(SEP, buffer[i]) && word[0] != '\0') {
			word[word_index] = '\0';
			(*words)[(*index)++]  = strdup(word);
			word[0] = '\0';
			word_index = 0;
		} else if (strchr(IGNORED, buffer[i]) == NULL) {
			word[word_index++] = buffer[i];
		}
	}

	word[word_index] = '\0';
	(*words)[*index]  = strdup(word);
}

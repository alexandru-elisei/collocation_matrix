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

#define LOWER(s)						\
	do {							\
		int i;						\
		for (i = 0; s[i] != '\0'; i++)			\
			if (s[i] >= 'A' && s[i] <= 'Z')		\
				s[i] = s[i] + 32;		\
	} while (0)

/* Reads the input file */
enum word_result get_input(FILE *in, FILE **text, 
		char ***cost, int *cost_no,
		char ***min, int *min_no,
		char ***fixed_words, int **fixed_lengths, int *fixed_no);
/* Reads the text file */
enum word_result get_text(FILE *text, struct tnode **t, 
		unsigned int *total_words, unsigned int *distinct_words);

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
	unsigned int total_words;
	unsigned int distinct_words;

	int i;

	if (argc < 3)
		CHKRES(WORD_ERROR_INVALID_ARGUMENTS);

	if ((in = fopen(argv[1], "r")) == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	if ((out = fopen(argv[2], "w")) == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	CHKRES(get_input(in, &text, &cost_words, &cost_no,
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

	get_text(text, &search_tree, &total_words, &distinct_words);
	printf("\ntree:\n");
	tree_print(search_tree);

	/*
	printf("total_words = %d\n", total_words);
	printf("distinct_words = %d\n", distinct_words);
	*/

	return EXIT_SUCCESS;
}

enum word_result get_input(FILE *in, FILE **text, 
		char ***cost, int *cost_no,
		char ***min, int *min_no,
		char ***fixed_words, int **fixed_lengths, int *fixed_no)
{
	char buffer[LINE_LEN];
	int i;
	int index;

	fgets(buffer, LINE_LEN, in);
	buffer[strlen(buffer) - 1] = '\0';
	if ((*text = fopen(buffer, "r")) == NULL)
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
enum word_result get_text(FILE *text, struct tnode **t, 
		unsigned int *total_words, unsigned int *distinct_words)
{
	char buffer[LINE_LEN];
	char word[WORD_LEN], prev[WORD_LEN];
	/* The address is shared between the search tree and the graph */
	char *word_shared_address;
	int word_index;
	int graph_index;
	int i;

	*total_words = 0;
	*distinct_words = 0;
	while (fgets(buffer, LINE_LEN, text) != NULL) {
		word[0] = '\0';
		word_index = 0;
		for (i = 0; buffer[i] != '\0'; i++) {
			/* All lower case */
			if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
				word[word_index++] = buffer[i] - 'A' + 'a';
			/* Lower case letters */
			} else if (buffer[i] >= 'a' && buffer[i] <= 'z') {
				word[word_index++] = buffer[i];
			/* Digits */
			} else if (buffer[i] >= '0' && buffer[i] <= '9') {
				word[word_index++] = buffer[i];
			/* And "'" */
			} else if (buffer[i] == '\'') {
				word[word_index++] = buffer[i];
			/* Words have at least one letter/digit/' */
			} else if (strchr(SEP, buffer[i]) != NULL &&
					word[0] != '\0') {
				word[word_index] = '\0';
				word_shared_address = strdup(word);

				graph_index = tree_add(t, word_shared_address, 
						*distinct_words);
				/* I inserted a new word in the tree */
				if (graph_index == *distinct_words)
					(*distinct_words)++;

				word[0] = '\0';
				word_index = 0;
				(*total_words)++;
			} 
		}

	}
}

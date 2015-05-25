#include "common.h"
#include "list.h"
#include "tree.h"
#include "wgraph.h"
#include "pqueue.h"

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
enum word_result get_input(FILE *in, FILE **text, 
		char ***cost, int *cost_no,
		char ***min, int *min_no,
		char ***fixed_words, int **fixed_lengths, int *fixed_no);

/* Parses the text file, building the binary word search tree and the graph */
enum word_result parse_text(FILE *text, struct tnode **t, 
		unsigned int *total_words, struct wgraph *graph);

int main(int argc, char **argv)
{
	FILE *in = NULL; 	/* Fisierul cu datele de intrare */
	FILE *text = NULL;	/* Fisierul cu textul */
	FILE *out = NULL;	/* Fisierul de iesire */

	struct tnode *search_tree;
	struct wgraph *word_graph = NULL;
	struct pqueue *pri_queue;
	
	char **cost_words;	/* words needed for printing the cost */
	int cost_no;		/* number of words in the cost_words array */
	char **min_words;	/* words for the minimum cost path */
	int min_no;		/* number of words in the min_words array */
	char **fixed_words;	/* words for the fixed length path */
	int *fixed_lengths;	/* length for the fixed length path */
	int fixed_no;		/* number of words in the fixed_words array */
	unsigned int total_words;

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
	printf("\ncost_words:\n");
	for (i = 0; i < cost_no * 2; i++)
		printf("|%s|\n", cost_words[i]);

	/*
	printf("\nmin_words:\n");
	for (i = 0; i < min_no * 2; i++)
		printf("|%s|\n", min_words[i]);

	printf("\nfixed_words:\n");
	for (i = 0; i < fixed_no; i++)
		printf("%d - |%s|\n", fixed_lengths[i], fixed_words[i]);
	*/

	search_tree = tree_create();
	word_graph = wgraph_create();

	parse_text(text, &search_tree, &total_words, word_graph);

	/*
	printf("\ntree:\n");
	tree_print(search_tree);
	printf("\ngraph:\n");
	wgraph_print(word_graph);
	*/

	wgraph_calculate_costs(word_graph, search_tree, total_words);

	/*
	printf("\nafter odds:\n");
	wgraph_print(word_graph);
	*/

	for (i = 0; i < cost_no; i++)
		fprintf(out, "%g\n", wgraph_cost_by_name(word_graph,
			search_tree, cost_words[2*i], cost_words[2*i+1]));

	printf("testing heap:\n");
	pri_queue = pqueue_create();
	pri_queue->insert(22, 23.93);
	pri_queue->insert(1, 2.33);
	pri_queue->insert(3, 1.33);
	pri_queue->insert(19, 5.00);
	pri_queue->insert(20, 3.00);
	pri_queue->insert(21, 4.00);
	pri_queue->print();

	printf("%d\n", pri_queue->extract_min());
	printf("%d\n", pri_queue->extract_min());
	printf("%d\n", pri_queue->extract_min());
	printf("%d\n", pri_queue->extract_min());

	//printf("destroying search_tree\n");
	search_tree = tree_destroy(search_tree);
	word_graph = wgraph_destroy(word_graph);
	for (i = 0; i < cost_no; i++) {
		free(cost_words[2*i]);
		free(cost_words[2*i+1]);
	}
	free(cost_words);

	for (i = 0; i < min_no; i++) {
		free(min_words[2*i]);
		free(min_words[2*i+1]);
	}
	free(min_words);

	for (i = 0; i < fixed_no; i++)
		free(fixed_words[i]);
	free(fixed_words);
	free(fixed_lengths);

	fclose(in);
	fclose(out);
	fclose(text);

	return EXIT_SUCCESS;
}

enum word_result get_input(FILE *in, FILE **text, 
		char ***cost, int *cost_no,
		char ***min, int *min_no,
		char ***fixed_words, int **fixed_lengths, int *fixed_no)
{
	char buffer[LINE_LEN];
	int i;

	fgets(buffer, LINE_LEN, in);
	buffer[strlen(buffer) - 1] = '\0';
	if ((*text = fopen(buffer, "r")) == NULL)
		return WORD_ERROR_FILE_ACCESS;

	fgets(buffer, LINE_LEN, in);
	sscanf(buffer, "%d", cost_no);

	/* Reading the words we need to print the cost */
	*cost = (char **) malloc((2 * (*cost_no)) * sizeof(char *));
	for (i = 0; i < *cost_no; i++) {
		fgets(buffer, LINE_LEN, in);
		(*cost)[2*i] = strdup(strtok(buffer, SEP));
		(*cost)[2*i+1] = strdup(strtok(NULL, SEP));
	}

	fgets(buffer, LINE_LEN, in);
	sscanf(buffer, "%d", min_no);

	/* Reading the words we need to calculate the minimum cost path */
	*min = (char **) malloc((2 * (*min_no)) * sizeof(char *));
	for (i = 0; i < *min_no; i++) {
		fgets(buffer, LINE_LEN, in);
		(*min)[2*i] = strdup(strtok(buffer, SEP));
		(*min)[2*i+1] = strdup(strtok(NULL, SEP));
	}

	fgets(buffer, LINE_LEN, in);
	sscanf(buffer, "%d", fixed_no);

	/* Reading the words we need for the fixed length path */
	*fixed_words = (char **) malloc(*fixed_no * sizeof(char *));
	*fixed_lengths = (int *) malloc(*fixed_no * sizeof(int));
	for (i = 0; i < *fixed_no; i++) {
		fgets(buffer, LINE_LEN, in);
		sscanf(buffer, "%d", &((*fixed_lengths)[i]));
		strtok(buffer, SEP);
		(*fixed_words)[i] = strdup(strtok(NULL, SEP));
	}

	return WORD_SUCCESS;
}

/* Parses the text file, building the binary word search tree and the graph */
enum word_result parse_text(FILE *text, struct tnode **t, 
		unsigned int *total_words, struct wgraph *graph)
{
	char buffer[LINE_LEN];
	char word[WORD_LEN];
	/* The address is shared between the search tree and the graph */
	char *word_shared_address, *previous_word;
	int word_index;
	int graph_index;
	int i;

	*total_words = 0;
	previous_word = NULL;
	word_shared_address = NULL;
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

				/* 
				 * Adding the previous word and its neighbour,
				 * the current word, to the search tree and 
				 * checking if it was found before in the text
				 */
				if (previous_word != NULL) {
					graph_index = tree_add(t, previous_word, 
							graph->size);
					wgraph_add(graph, previous_word,
						word_shared_address, graph_index);
				}

				previous_word = word_shared_address;

				word[0] = '\0';
				word_index = 0;
				(*total_words)++;
			} 
		}
	}

	/* Adding the last word */
	graph_index = tree_add(t, word_shared_address, graph->size);
	wgraph_add(graph, word_shared_address, NULL, graph_index);
	graph->last_word_index = graph_index;

	return WORD_SUCCESS;
}

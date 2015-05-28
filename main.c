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
		struct wgraph *graph);

int main(int argc, char **argv)
{
	FILE *in = NULL; 	/* Fisierul cu datele de intrare */
	FILE *text = NULL;	/* Fisierul cu textul */
	FILE *out = NULL;	/* Fisierul de iesire */

	struct tnode *search_tree;
	struct wgraph *word_graph = NULL;
	
	char **cost_words;	/* words needed for printing the cost */
	int cost_no;		/* number of words in the cost_words array */
	char **min_words;	/* words for the minimum cost path */
	int min_no;		/* number of words in the min_words array */
	char **fixed_words;	/* words for the fixed length path */
	int *fixed_lengths;	/* length for the fixed length path */
	int fixed_no;		/* number of words in the fixed_words array */

	char **fixed_path;
	int fixed_paths;

	char **min_path;
	int min_path_len;

	int i;
	int j;

	if (argc < 3)
		CHKRES(WORD_ERROR_INVALID_ARGUMENTS);

	if ((in = fopen(argv[1], "r")) == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	if ((out = fopen(argv[2], "w")) == NULL)
		CHKRES(WORD_ERROR_FILE_ACCESS);

	CHKRES(get_input(in, &text, &cost_words, &cost_no,
				&min_words, &min_no,
				&fixed_words, &fixed_lengths, &fixed_no));

	search_tree = tree_create();
	word_graph = wgraph_create();

	parse_text(text, &search_tree, word_graph);
	wgraph_calculate_costs(word_graph);

	for (i = 0; i < cost_no; i++)
		fprintf(out, "%g\n", wgraph_cost_by_name(word_graph,
			search_tree, cost_words[2*i], cost_words[2*i+1]));

	for (i = 0; i < min_no; i++) {
		min_path = wgraph_min_path(word_graph, search_tree, 
				min_words[2*i], min_words[2*i+1], &min_path_len);

		if (min_path == NULL) {
			fprintf(stderr, "No path found between %s and %s\n",
				min_words[2*i], min_words[2*i+1]);
			continue;
		}

		fprintf(out, "%s", min_path[0]);
		for (j = 1; j < min_path_len; j++)
			fprintf(out, " %s", min_path[j]);
		fprintf(out, "\n");
	}

	/*
	printf("\ngraful normal:\n");
	wgraph_print(word_graph);
	*/

	/*
	printf("\nfixed length paths:\n");
	printf("fixed_no = %d\n", fixed_no);
	*/
	for (i = 0; i < fixed_no; i++) {
		fixed_path = wgraph_fixed_path(word_graph, search_tree, 
			fixed_lengths[i], fixed_words[i], &fixed_paths);
		if (fixed_path == NULL) {
			fprintf(stderr, "No path of length %d to %s found\n",
					fixed_lengths[i], fixed_words[i]);
			continue;
		}
		fprintf(out, "%d\n", fixed_paths);
		fprintf(out, "%s", fixed_path[0]);
		for (j = 1; j < fixed_lengths[i]; j++)
			fprintf(out, " %s", fixed_path[j]);
		fprintf(out, "\n");
	}

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
		struct wgraph *graph)
{
	char buffer[LINE_LEN];
	char word[WORD_LEN];
	/* The address is shared between the search tree and the graph */
	char *word_address;
	int prev_index;			/* index of previously inserted word */
	int word_index;
	int graph_index;
	int i;

	word_address = NULL;
	prev_index = graph_index = NODE_NOT_FOUND;
	while (fgets(buffer, LINE_LEN, text) != NULL) {
		word[0] = '\0';
		word_index = 0;
		for (i = 0; buffer[i] != '\0'; i++) {
			/* Lower case letters */
			if (buffer[i] >= 'a' && buffer[i] <= 'z') {
				word[word_index++] = buffer[i];
			/* Converting upper case to lower case */
			} else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
				word[word_index++] = buffer[i] - 'A' + 'a';
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
				word_address = strdup(word);

				/* Adding the word */
				graph_index = tree_add(t, word_address,
						graph->size);
				wgraph_add(graph, word_address, graph_index,
					       &prev_index, COST_UNKNOWN);
				prev_index = graph_index;

				word[0] = '\0';
				word_index = 0;
			} 
		}
	}

	/* 
	 * Storing the index of the last word, there's no word after it for the
	 * o12 formula 
	 */
	graph->last_word_index = graph_index;

	return WORD_SUCCESS;
}

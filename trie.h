#ifndef _TRIE_H
#define _TRIE_H		1

#include <malloc.h>				// for dynamic memory allocations
#include <stdio.h>				// for writing to streams
#include <stdlib.h>				// for exit(), in case of abnormal conditions rearing their ugly head :P


//	============================  Structs/Types  ============================

struct trie_node;										// Forward declaration

typedef struct char_n_addr								// Holds a character-address pair
{
	char ch;
	struct trie_node *addr;
} char_n_addr;


typedef struct trie_node								// Holds information related to a Trie Node.
{
	unsigned int frequency, child_count, children_size;
	char_n_addr **children;
} trie_node;

typedef trie_node* trie_t;


//	============================  Functions/Operations  ============================

static trie_node* create_node()							// Creates a new Trie node with the default values.
{
	trie_node* node = malloc(sizeof(trie_node));		// allocate memory for an instance of a Trie node.

	node->frequency = 0;								// by default, the frequency..
	node->child_count = 0;								// ..child_count..
	node->children_size = 0;							// ..children_size is made 0 since it is a new node..
	node->children = NULL;								// ..and we make the "children" pointer a NULL pointer.

	return node;										// finally, the address of the new node is returned.
}

trie_t create_trie()									// Creates an empty trie which is nothing but a single
{														// node; which is called the root node.
	return create_node();								// hence, we just call create_node() and that node 
}														// is the root node; whose address is returned.

void add_string(trie_t root, const char* string)		// Adds the string pointed to by "string" to the Trie pointed by "root".
{
	int i;
	trie_node* next_node = NULL;						// We need to traverse the Trie to the bottom and then add the new
														// node (or increment the frequency if it is already present). Thus, 
														// this pointer stores the next node that we need to look at. 

	if (*string == '\0')								// if it is the end of the string..
	{
		++root->frequency;								// then just increase the frequency
		return;											// and return.. This is the base/terminating condition
	}													// for the recursive calls made to add_string().

	for(i = 0; i < root->child_count; ++i)				// checking whether the current character is present or not.
		if(root->children[i]->ch == *string)			// if it is..
		{
			next_node = root->children[i]->addr;		// ..then store the address in next_node
			break;										// and break out of the loop.
		}

	if(next_node == NULL)								// if such a character is not present, then create a new node for it.
	{
		int pos = root->child_count;					// This is the position where the new node would be inserted to root->children.
		++root->child_count;							// a new child is created, so increment the child_count.
		char_n_addr *char_n_addr_node = malloc(sizeof(char_n_addr));	// allocate space for a character-address pair. that node's
																		// address is stored in this variable.

		if(root->child_count > root->children_size)		// if the number of children is greater than the size of the children array..
		{
			if(root->children_size == 0)				// ..and if children_size is 0, i.e., its default value
				root->children_size = 1;				// ..then make the size of the array to hold one character-address pair.
			else										// ..else, we double the size of the
				root->children_size <<= 1;				// ..children array by bit-shifting one place to the left
			root->children = realloc(root->children, root->children_size * sizeof(char_n_addr*));	// we reallocate in powers of 2 to
		}																							// reduce the number of reallocations

		if(root->children == NULL)						// if the reallocation failed, then display an error message and quit.
		{
			fprintf(stderr, "Error during re-allocation in function add_recursive() in file header.h\n");
			exit(EXIT_FAILURE);
		}

		next_node = create_node();						// the next Trie node that we need to jump to, does not exist, so create it
		char_n_addr_node->ch = *string;					// we assign the character
		char_n_addr_node->addr = next_node;				// -address pair..
		root->children[pos] = char_n_addr_node;			// and add it to the children array at the position "pos"
	}

	add_string(next_node, ++string);					// move to the next character and add it to the next node.
}


/*	Purpose: Recurses through the input trie; concatenating the character at each node of the trie and writes the frequency to the output stream, fout.
**
**	Idea: Each recursive call to this function concatenates one character to a string. So, to create the string "ABC", 3 calls in total would be made.
**	The position where the new character would be inserted is indicated by the parameter "pos".
**	Say, we have two strings in the trie "AC" and "AD". The trie node "A" has two children, viz. "C" and "D". In such cases, a loop is run over all
**	the children, and each child is appended one at a time to the position indicated by parameter "pos".
**
**	The author wanted to create a dynamically allocated string which is used to contenate the characters at each trie node and would be shared by all
**	the instances of this recursive function to reduce the number of memory allocations (which takes precious time and memory). Also, to decrease the
**	number of reallocations, the author wanted to increase the size of the string in powers of 2; so initially the size would be 8 (it is rare to come
**	across strings that are less than 7 chars; note that an extra char is required to store the NULL); then if extra memory is needed, then the size
**	is increased to 16, then 32 and so on.
**	To achieve this, the address of the length (of the string) and the address of the string is passed among the various recursive instances.
**	A string is a char*; so a pointer to a char* would be char**. We can think of it as a dynamic jagged array where we only use the first string, i.e.
**	string[0] contains the intermediate string (and this is how the author dereferences the string in this function). The length's address is passed,
**	so we need int* length. This explains the parameters- char** string and int *length
*/
static void write_freq_worker(FILE *fout, trie_node* root, char** string, int pos, int* length)
{
	if(root->frequency != 0)									// if the frequency is non-zero, it means that it is a whole word..
		fprintf(fout, "%s\t\t%d\n", *string, root->frequency);	// hence print the string created so far and its frequency.

	if(root->child_count != 0)							// if the root has any children, then we proceed, else the function exits
	{
		int i, prev_length = *length;					// prev_length helps detect whether we need extra memory or not.

		if(*length == 0)								// if *length is 0, it means that the string does not exist.
			*length = 8;								// so, allocate 8 characters - it is rare to have strings less than 7 chars.
		else if (pos + 2 > *length)						// the current position + 2 indicates how many characters there would be..and if that is
			(*length) <<= 1;							// greater than the current length, then double the length of the string.

		if(prev_length != *length)						// if more memory needs to be allocated..
		{
			string[0] = realloc(*string, *length * sizeof(char));	// then reallocate it..
			if(string[0] == NULL)						// and in case reallocation was not possible, output an error message to stderr.
			{
				fprintf(stderr, "Error during re-allocation in function print_freq_rec() in file header.h\n");
				exit(EXIT_FAILURE);
			}
		}

		string[0][pos + 1] = '\0';	// a character would be added at position "pos". pos + 1 should contain a NULL character to terminate the string.
		for(i = 0; i < root->child_count; ++i)			// for each child of the current root..
		{
			string[0][pos] = root->children[i]->ch;		// add the character to the appropriate position..

			// and call the same function, but this time with an updated root and position. The next child is at root->children[i]->addr
			write_freq_worker(fout, root->children[i]->addr, string, pos + 1, length);
		}
		string[0][pos] = '\0';		// "erasing" the newly inserted character so that the previous recusive callers do not accidentally print it.
	}

}

void write_freq(FILE *fout, trie_t root)				// writes the frequency of each string in the trie to the output stream fout
{
	char **string_ptr = malloc(sizeof(char*));			// as explained in the function description of write_freq_worker(), we need a char**.
														// we dynamically allocate the pointer
	int len_var = 0;									// the length is stored in a local variable of this function.
	string_ptr[0] = NULL;								// the 0th string (as explained in the worker function description), is initialised to NULL

	write_freq_worker(fout, root, string_ptr, 0, &len_var);	// call the worker function..

	free(string_ptr[0]);								// free the string..
	free(string_ptr);									// and free the dynamic pointer as well.
}


void destroy_trie(trie_t root)							// frees memory allocated to a trie.
{
	if(root->child_count != 0)							// if the root has any children..
	{
		for(int i = 0; i < root->child_count; ++i)		// ..then, for each child..
		{
			destroy_trie(root->children[i]->addr);		// ..consider the child as the next root and recursively free it..
			free(root->children[i]);					// and free the character-address pair associated with that child.
		}
		free(root->children);							// once all children are freed, free up the children array.
	}

	free(root);											// lastly, destroy the root itself.
}

#endif													// trie.h included

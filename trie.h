#ifndef _TRIE_H
#define _TRIE_H		1

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>


//	============================  Structs/Types  ============================

struct trie_node;										// Forward declaration

typedef struct char_n_addr								// Holds a character-address pair
{
	char ch;
	struct trie_node *addr;
} char_n_addr;


typedef struct trie_node									// Holds information related to a Trie Node.
{
	unsigned int frequency, child_count, children_size;
	char_n_addr **children;
} trie_node;

typedef trie_node* trie;


//	============================  Functions/Operations  ============================

static trie_node* create_node()							// Creates a new Trie node with the default values.
{
	trie_node* node = malloc(sizeof(trie_node));			// allocate space for an instance of a Trie node.

	node->frequency = 0;								// by default, the frequency..
	node->child_count = 0;								// ..child_count..
	node->children_size = 0;							// ..children_size is made 0 since it is a new node..
	node->children = NULL;								// ..and we make the "children" pointer a NULL pointer.

	return node;										// finally, the address of the new node is returned.
}

trie create_trie()										// Creates an empty trie which is nothing but a single
{														// node; which is called the root node.
	return create_node();								// hence, we just call create_node() and that node 
}														// is the root node; whose address is returned.

void add_string(trie root, const char* string)			// Adds the string pointed to by "string" to the Trie pointed by "root".
{
	int i;
	trie_node* next_node = NULL;							// We need to traverse the Trie to the bottom and then add the new
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
			exit(1);
		}

		next_node = create_node();						// the next Trie node that we need to jump to, does not exist, so create it
		char_n_addr_node->ch = *string;					// we assign the character
		char_n_addr_node->addr = next_node;				// -address pair..
		root->children[pos] = char_n_addr_node;			// and add it to the children array at the position "pos"
	}

	add_string(next_node, ++string);					// move to the next character and add it to the next node.
}


static void write_freq_worker(FILE *fout, trie_node* root, char** string, int pos, int* length)
{
	if(root->frequency != 0)
		fprintf(fout, "%s\t\t%d\n", *string, root->frequency);

	if(root->child_count != 0)
	{
		int i, prev_length = *length;

		if(*length == 0)
			*length = 2;
		else if (*length < pos + 2)
			(*length) <<= 1;

		if(prev_length != *length)
		{
			string[0] = realloc(*string, *length);
			if(string[0] == NULL)
			{
				fprintf(stderr, "Error during re-allocation in function print_freq_rec() in file header.h\n");
				exit(2);
			}
		}

		string[0][pos + 1] = '\0';
		for(i = 0; i < root->child_count; ++i)
		{
			string[0][pos] = root->children[i]->ch;
			write_freq_worker(fout, root->children[i]->addr, string, pos + 1, length);
		}
		string[0][pos] = '\0';
	}

}

void write_freq(FILE *fout, trie root)
{
	char **string_ptr = malloc(sizeof(char*));
	int var = 0, *length = &var;
	*string_ptr = NULL;

	write_freq_worker(fout, root, string_ptr, 0, length);

	free(*string_ptr);
	free(string_ptr);
}


void destroy_trie(trie root)
{
	if(root->child_count != 0)
	{
		for(int i = 0; i < root->child_count; ++i)
		{
			destroy_trie(root->children[i]->addr);
			free(root->children[i]);
		}
		free(root->children);
	}

	free(root);
}

#endif														// trie.h included
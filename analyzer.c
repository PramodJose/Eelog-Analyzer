#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "trie.h"
#include "max_heap.h"
#include "parse_file.c"

#define DEFAULT_INP_FILE "eelotracker.log"
#define DEFAULT_OUT_FILE "processed.log"

int main(int argc, char* argv[])
{
	char* inp_file = DEFAULT_INP_FILE, *out_file = DEFAULT_OUT_FILE;
	unsigned int i, top_count = 0;

	for(i = 0; i < argc; ++i)
		if(argv[i][0] == '-' && argv[i][1] == 'i' && i + 1 < argc)
			inp_file = argv[i + 1];
		else if(argv[i][0] == '-' && argv[i][1] == 'o' && i + 1 < argc)
			out_file = argv[i + 1];
		else if(argv[i][0] == '-' && argv[i][1] == 't' && i + 1 < argc)
			top_count = atoi(argv[i + 1]);

	parse_file(inp_file, out_file, top_count);
	return 0;
}
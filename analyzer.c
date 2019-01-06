#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "trie.h"
#include "max_heap.h"

#define INP_FILE "eelotracker.log"
#define MAX_LINE_SIZE 256
#define OUT_FILE "main_out.txt"

#define SPACES_TO_TRACKER_COUNT 3   // Eg: 2018-10-19 09:42:29,320:INFO:====Analysis done...found 16 trackers (There are 3 spaces to the tracker count)
                                    // Here:        /|\                            /|\          /|\

#define SPACES_TO_TRACKER_NAME 2

void process_json(FILE* fin, char* line, size_t len, heap_t apps_heap)
{
	size_t i, pos;
	char read_flag = 0, app_name[MAX_LINE_SIZE], space_count;
	unsigned int tracker_count = 0;

	line[len - 4] = 'a';
	line[len - 3] = 'p';
	line[len - 2] = 'k';
	line[len - 1] = '\0';

	for(i = 0; line[i] != '\0'; ++i)
	{
		if(line[i] == '/')
		{
			read_flag = 1;
			pos = 0;
		}
		else if(read_flag == 1)
		{
			app_name[pos] = line[i];
			++pos;
		}
	}
	app_name[pos] = '\0';

	fgets(line, MAX_LINE_SIZE, fin);
	for(i = 0, space_count = 0; space_count != SPACES_TO_TRACKER_COUNT; ++i)
		if(line[i] == ' ')
			++space_count;

	while(line[i] != ' ')
	{
		tracker_count = tracker_count * 10 + (line[i] - '0');
		++i;
	}

	heap_append(apps_heap, app_name, tracker_count);
}

void process_tracker_name(trie_t tracker_frequency, char *line, size_t pos, size_t len)
{
	size_t t_name_length = len - pos, i = 0;
	char* tracker_name = malloc(t_name_length + 1);

	for(; pos != len; ++i, ++pos)
		tracker_name[i] = line[pos];
	tracker_name[i] = '\0';

	add_string(tracker_frequency, tracker_name);

	free(tracker_name);
}

int main(void)
{
	FILE *fin, *fout;
	trie_t tracker_frequency = create_trie();
	heap_t apps_heap = create_heap();
	char line[MAX_LINE_SIZE];
	size_t len, i;

	fin = fopen(INP_FILE, "r");
	if(fin == NULL)
	{
		fprintf(stderr, "Error opening file %s\n", INP_FILE);
		exit(EXIT_FAILURE);
	}
	fout = fopen(OUT_FILE, "w");
	if(fout == NULL)
	{
		fprintf(stderr, "Error opening file %s\n", OUT_FILE);
		exit(EXIT_FAILURE);
	}


	while(fgets(line, MAX_LINE_SIZE, fin) != NULL)
	{
		len = strlen(line) - 1;

		if(line[len - 5] == '.' && line[len - 4] == 'j' && line[len - 3] == 's')	// It's a json.
			process_json(fin, line, len, apps_heap);
		else
		{
			char space_count = 0;
			for(i = 0; line[i] != '\0'; ++i)
				if(line[i] == ' ' && (++space_count) == SPACES_TO_TRACKER_NAME)
					break;

			if(line[i] != '\0' && (i != 0 && (len - i - 3) > 0) && line[i - 1] ==':' && line[i + 1] == '-' && line[i + 2] ==' ')
				process_tracker_name(tracker_frequency, line, i + 3, len);
		}
	}

	write_freq(fout, tracker_frequency);
	fprintf(fout, "\n\n\n");
	heap_extract_all(fout, apps_heap);
	destroy_trie(tracker_frequency);
	destroy_heap(apps_heap);
	fclose(fin);
	fclose(fout);
	return 0;
}

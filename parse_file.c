#include <time.h>

#define SPACES_TO_TRACKER_COUNT 3   // Eg: 2018-10-19 09:42:29,320:INFO:====Analysis done...found 16 trackers (There are 3 spaces to the tracker count)
                                    // Here:         ^                              ^            ^

#define SPACES_TO_TRACKER_NAME 2    // Eg: 2018-10-19 09:41:49,274:WARNING: - Facebook Login (There are 2 spaces to the tracker name)
                                    // Here:         ^                     ^
                                    // after we see the second space, we check whether a ':'' and '-'' exists before and after it
#define MAX_LINE_SIZE 256

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

void parse_file(const char* inp_file, const char* out_file, const unsigned int top_count)
{
	FILE *fin, *fout;
	clock_t start_time = clock();
	trie_t tracker_frequency = create_trie();
	heap_t apps_heap = create_heap();
	char line[MAX_LINE_SIZE];
	size_t len, i;
	float time_taken;

	fin = fopen(inp_file, "r");
	if(fin == NULL)
	{
		fprintf(stderr, "Error opening file %s\n", inp_file);
		exit(EXIT_FAILURE);
	}
	fout = fopen(out_file, "w");
	if(fout == NULL)
	{
		fprintf(stderr, "Error opening file %s\n", out_file);
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
	heap_extract_top(fout, apps_heap, top_count);
	destroy_trie(tracker_frequency);
	destroy_heap(apps_heap);

	time_taken = clock() - start_time;
	time_taken = time_taken/CLOCKS_PER_SEC;
	fprintf(fout, "\n\nExecution time: %lf seconds\n", time_taken);
	fclose(fin);
	fclose(fout);
	fprintf(stdout, "Processed log has been stored in %s\nExecution time: %lf seconds\n", out_file, time_taken);
}

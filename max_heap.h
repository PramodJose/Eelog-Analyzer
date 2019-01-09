#ifndef _MAX_HEAP_H
#define _MAX_HEAP_H	1

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_HEAP_SIZE 2048

//	============================  Structs/Types  ============================

typedef struct
{
	unsigned int trackers_count;
	char* app_name;
} max_heap_node;

struct heap_t
{
	max_heap_node* app_list;
	unsigned int node_count, allocated_count;
};

typedef struct heap_t* heap_t;

//	============================  Functions/Operations  ============================

static void create_heap_node(max_heap_node* app_list, const unsigned int pos, const char* a_name, const unsigned int t_count)
{
	app_list[pos].trackers_count = t_count;

	app_list[pos].app_name = malloc(strlen(a_name) + 1);
	strcpy(app_list[pos].app_name, a_name);
}

heap_t create_heap()
{
	heap_t heap = malloc(sizeof(struct heap_t));

	heap->app_list = NULL;
	heap->node_count = 0;
	heap->allocated_count = 0;

	return heap;
}

void destroy_heap(heap_t heap)
{
	for(unsigned int i = 0; i < heap->node_count; ++i)
		free(heap->app_list[i].app_name);
	free(heap);
}

void heap_append(heap_t heap, const char* a_name, const unsigned int t_count)
{
	++heap->node_count;

	if(heap->node_count > heap->allocated_count)
	{
		if(heap->allocated_count != 0)
			heap->allocated_count <<= 1;
		else
			heap->allocated_count = DEFAULT_HEAP_SIZE;

		heap->app_list = realloc(heap->app_list, heap->allocated_count * sizeof(max_heap_node));
		if(heap->app_list == NULL)
		{
			fprintf(stderr, "Error during re-allocation in function heap_append() in file max_heap.h\n");
			exit(EXIT_FAILURE);
		}
	}

	create_heap_node(heap->app_list, heap->node_count - 1, a_name, t_count);
}

static void max_heapify(heap_t heap, long int i)
{
	long int left_child = (i << 1) | 1, right_child = left_child + 1, largest;
	// same as: left_child = (i * 2) + 1, right_child = left_child + 1

	if(left_child < heap->node_count && heap->app_list[left_child].trackers_count > heap->app_list[i].trackers_count)
		largest = left_child;
	else
		largest = i;

	if(right_child < heap->node_count && heap->app_list[right_child].trackers_count > heap->app_list[largest].trackers_count)
		largest = right_child;

	if(largest != i)
	{
		char* swap_temp_str = heap->app_list[largest].app_name;
		heap->app_list[largest].app_name = heap->app_list[i].app_name;
		heap->app_list[i].app_name = swap_temp_str;

		unsigned int swap_temp_uint = heap->app_list[largest].trackers_count;
		heap->app_list[largest].trackers_count = heap->app_list[i].trackers_count;
		heap->app_list[i].trackers_count = swap_temp_uint;

		max_heapify(heap, largest);
	}
}

static void build_max_heap(heap_t heap)
{
	long int non_leaf = ((long int)heap->node_count >> 1) - 1;

	while(non_leaf >= 0)
	{
		max_heapify(heap, non_leaf);
		--non_leaf;
	}
}

void heap_extract_top(FILE *fout, heap_t heap, long int top_count)
{
	if(top_count <= 0 || top_count > heap->node_count)
		top_count = heap->node_count;

	build_max_heap(heap);

	fprintf(fout, "%-100s  NO. OF TRACKERS\n\n", "APPLICATION NAME");
	for(; top_count != 0; --top_count)
	{
		fprintf(fout, "%-100s  %d\n", heap->app_list[0].app_name, heap->app_list[0].trackers_count);

		--heap->node_count;

		if(heap->node_count != 0)
		{
			heap->app_list[0] = heap->app_list[heap->node_count];

			max_heapify(heap, 0);

			/*	The following section can be uncommeneted if this code is used in "production".
			**	The following code reduces the size of the heap when heap nodes are "deleted". This returns the dynamically
			**	allocated memory back to the system, but it takes a little bit of time to allocate a new chunk of memory,
			**	copy the required heap nodes and then unallocate the non-required memory space. It has been uncommented because
			**	for	the current requirements, the memory is anyway de-allocated right after this function is called. Thus, we can
			**	save a little bit of time by skipping this step.
			if(heap->allocated_count > DEFAULT_HEAP_SIZE && heap->node_count <= (heap->allocated_count >> 1))
			{
				heap->allocated_count >>= 1;
				heap->app_list = realloc(heap->app_list, heap->allocated_count * sizeof(max_heap_node));
			}*/
		}
	}
}

#endif

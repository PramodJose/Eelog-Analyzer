#ifndef _MAX_HEAP_H
#define _MAX_HEAP_H	1

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_HEAP_SIZE 8

//	============================  Structs/Types  ============================

typedef struct
{
	unsigned int trackers_count;
	char* app_name;
} max_heap_node;

struct heap_t
{
	max_heap_node** app_list;
	unsigned int node_count, allocated_count;
};

typedef struct heap_t* heap_t;

//	============================  Functions/Operations  ============================

static max_heap_node* create_heap_node(const char* a_name, const unsigned int t_count)
{
	max_heap_node* node = malloc(sizeof(max_heap_node));

	node->trackers_count = t_count;
	node->app_name = malloc(strlen(a_name) + 1);
	strcpy(node->app_name, a_name);

	return node;
}

static void destroy_node(max_heap_node* node)
{
	free(node->app_name);
	free(node);
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
		destroy_node(heap->app_list[i]);
	free(heap);
}

void heap_append(heap_t heap, const char* a_name, const unsigned int t_count)
{
	++heap->node_count;

	if(heap->node_count > heap->allocated_count)
	{
		if(heap->allocated_count == 0)
			heap->allocated_count = DEFAULT_HEAP_SIZE;
		else
			heap->allocated_count <<= 1;
		heap->app_list = realloc(heap->app_list, heap->allocated_count * sizeof(max_heap_node*));
		if(heap->app_list == NULL)
		{
			fprintf(stderr, "Error during re-allocation in function heap_append() in file max_heap.h\n");
			exit(EXIT_FAILURE);
		}
	}

	heap->app_list[heap->node_count - 1] = create_heap_node(a_name, t_count);
}

static void max_heapify(heap_t heap, long int i)
{
	long int left_child = (2 * i) + 1, right_child = (2 * i) + 2, largest;

	if(left_child < heap->node_count && heap->app_list[left_child]->trackers_count > heap->app_list[i]->trackers_count)
		largest = left_child;
	else
		largest = i;

	if(right_child < heap->node_count && heap->app_list[right_child]->trackers_count > heap->app_list[largest]->trackers_count)
		largest = right_child;

	if(largest != i)
	{
		max_heap_node* temp = heap->app_list[largest];
		heap->app_list[largest] = heap->app_list[i];
		heap->app_list[i] = temp;

		max_heapify(heap, largest);
	}
}

static void build_max_heap(heap_t heap)
{
	long int non_leaf = (heap->node_count / 2L) - 1;

	while(non_leaf >= 0)
	{
		max_heapify(heap, non_leaf);
		--non_leaf;
	}
}

void heap_extract_top(FILE *fout, heap_t heap, long int top_count)
{
	max_heap_node* temp;

	if(top_count <= 0)
		return;

	build_max_heap(heap);
	for(; top_count != 0; --top_count)
	{
		fprintf(fout, "%s\t\t%d\n", heap->app_list[0]->app_name, heap->app_list[0]->trackers_count);

		--heap->node_count;

		if(heap->node_count != 0)
		{
			temp = heap->app_list[0];
			heap->app_list[0] = heap->app_list[heap->node_count];
			heap->app_list[heap->node_count] = temp;

			max_heapify(heap, 0);

			if(heap->allocated_count > DEFAULT_HEAP_SIZE && heap->node_count <= heap->allocated_count / 2)
			{
				heap->allocated_count >>= 1;
				heap->app_list = realloc(heap->app_list, heap->allocated_count * sizeof(max_heap_node*));
			}
		}
	}
}

void heap_extract_all(FILE *fout, heap_t heap)
{
	heap_extract_top(fout, heap, heap->node_count);
}

#endif

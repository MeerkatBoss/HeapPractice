#include <cstdlib>

#include "heap_sort.h"

template<typename T>
static bool is_less(T a, T b);

template<>
bool is_less<uint32_t> (uint32_t  a, uint32_t  b) { return a  <  b; }

template<>
bool is_less<uint32_t*>(uint32_t* a, uint32_t* b) { return *a < *b; }

// Children indexed 1 through `arity`
__always_inline
static size_t get_kth_child(size_t index, size_t k, size_t arity) { return arity*index + k; }

template<typename T>
__always_inline
static void swap(T* a, T* b) { T tmp = *a; *a = *b; *b = tmp; }

template<typename T>
static void heapify(T* array, size_t size, size_t arity);

template<typename T>
static void sift_down_simple(size_t start_index, T* array, size_t size, size_t arity);

void HeapSort(uint32_t *array, size_t size, size_t arity)
{
    heapify(array, size, arity);
    uint32_t* last = array + size;
    for (size_t i = 1; i < size; i++)
    {
        swap(array, --last);
        sift_down_simple(0, array, size - i, arity);
    }
}


void HeapHeapSort(uint32_t *array, size_t size, size_t arity)
{
    heapify(array, size, arity);
    uint32_t** child_heaps = (uint32_t**)calloc(size, sizeof(*child_heaps));
    uint32_t*  children    = (uint32_t*) calloc(size, sizeof(*children));           //DEBUG
    for (size_t i = 1; i < size; ++i)
        child_heaps[i] = array + i;

    for (size_t i = 1; i < size; i += arity)
    {
        size_t child_size = i + arity <= size ? arity : size - i;
        heapify(child_heaps + i, child_size, 2);
    }
    for (size_t i = 1; i < size; i++) children[i] = *child_heaps[i];                // DEBUG

    uint32_t* last = array + size;
    for (size_t i = 1; i < size; i++)
    {
        swap(array, --last);

        size_t index = 0;
        size_t min_child = get_kth_child(index, 1, arity);
        size_t actual_size = size - i;

        /* rebuild last heap after swap */
        size_t last_child_heap = actual_size - ((actual_size - 1)%arity);
        size_t last_size = actual_size - last_child_heap;

        for (size_t j = last_child_heap; j < actual_size; j++)
            child_heaps[j] = array + j;

        heapify(child_heaps + last_child_heap, last_size , 2);

        /* modified sift_down */
        while (min_child < actual_size && array[index] < *child_heaps[min_child])
        {
            swap(array + index, child_heaps[min_child]);

            index = (size_t) (child_heaps[min_child] - array);
            min_child = get_kth_child(index, 1, arity);
        }

        /* update all child heaps */
        for (size_t j = 1; j < actual_size; ++j)
            child_heaps[j] = array + j;

        for (size_t j = 1; j < actual_size; j += arity)
        {
            size_t child_size = j + arity <= actual_size
                                ? arity
                                : actual_size - j;
            heapify(child_heaps + j, child_size, 2);
        }
        for (size_t j = 1; j < size; j++) children[j] = *child_heaps[j];                //DEBUG
    }

    free(child_heaps);
    free(children); //DEBUG
}

template<typename T>
static void heapify(T* array, size_t size, size_t arity)
{
    for (size_t i = 1; i <= size; i++)
        sift_down_simple(size-i, array, size, arity);
}


template<typename T>
static void sift_down_simple(size_t index, T* array, size_t size, size_t arity)
{
    while (get_kth_child(index, 1, arity) < size)               // while has at least one child
    {
        size_t max_child = index;
        for (size_t i = 1; i <= arity; i++)
        {
            size_t cur_child = get_kth_child(index, i, arity);
            if (cur_child >= size) break;                       // out-of-bounds child

            if (is_less(array[max_child], array[cur_child]))
                max_child = cur_child;
        }

        if (index == max_child) break;                          // max child less than index

        swap(array + index, array + max_child);
        index = max_child;
    }
}


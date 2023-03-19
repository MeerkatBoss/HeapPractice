#include <cstdlib>

#include "heap_sort.h"

// Children indexed 1 through `arity`
__always_inline
static size_t get_kth_child(size_t index, size_t k, size_t arity)
{
    return arity*index + k;
}

static void swap(uint32_t* a, uint32_t* b)
{
    uint32_t tmp = *a; *a = *b; *b = tmp;
}

static void heapify(uint32_t* array, size_t size, size_t arity);

static void sift_down_simple(size_t start_index,
                             uint32_t* array,
                             size_t size,
                             size_t arity);

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

static void heapify(uint32_t* array, size_t size, size_t arity)
{
    for (size_t i = 1; i <= size; i++)
        sift_down_simple(size-i, array, size, arity);
}


static void sift_down_simple(size_t index,
                             uint32_t* array,
                             size_t size,
                             size_t arity)
{
    // Continue while there is at least one child
    while (get_kth_child(index, 1, arity) < size)
    {
        size_t max_child = index;
        for (size_t i = 1; i <= arity; i++)
        {
            size_t cur_child = get_kth_child(index, i, arity);
            if (cur_child >= size) break;   // out-of-bounds child

            if (array[max_child] < array[cur_child])
                max_child = cur_child;
        }

        if (index == max_child) break; // max child less than index

        swap(array + index, array + max_child);
        index = max_child;
    }
}


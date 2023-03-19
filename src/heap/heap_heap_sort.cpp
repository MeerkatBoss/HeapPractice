#include <stdlib.h>

#include "heap_helpers.h"

#include "heap_sort.h"

struct main_node
{
    uint32_t value;
    size_t index_in_parent;
};

struct child_node
{
    uint32_t value;
    size_t index_in_main;
};

static void heapify_main(main_node* main_arr,
                         child_node* child_arr,
                         size_t size,
                         size_t arity);
static void heapify_children(main_node* main_arr,
                             child_node* child_arr,
                             size_t size,
                             size_t arity);

static void sift_down_main(size_t index,
                           main_node* main_arr,
                           child_node* child_arr,
                           size_t size,
                           size_t arity);
static void sift_down_child(size_t parent_index,
                            size_t index,
                            main_node* main_arr,
                            child_node* child_arr,
                            size_t size,
                            size_t arity);
static void sift_up_child(size_t parent_index,
                          size_t index,
                          main_node* main_arr,
                          child_node* child_arr,
                          size_t arity);
static void detach_max_child(size_t parent_index,
                             main_node* main_arr,
                             child_node* child_arr,
                             size_t size,
                             size_t arity);
                


void HeapHeapSort(uint32_t *array, size_t size, size_t arity)
{
    main_node*  main_arr  = (main_node*)  calloc(size, sizeof(*main_arr ));
    child_node* child_arr = (child_node*) calloc(size, sizeof(*child_arr));

    // Initialize helper arrays
    for (size_t i = 0; i < size; i++)
    {
        main_arr [i].value = array[i];
        child_arr[i].value = array[i];

        if (!i) main_arr [i].index_in_parent = 0;
        else    main_arr [i].index_in_parent = (i - 1) % arity;

        child_arr[i].index_in_main   = i;
    }

    heapify_children(main_arr, child_arr, size, arity);
    heapify_main(main_arr, child_arr, size, arity);

    const size_t root = 0;
    for (size_t i = 1; i < size; i++)
    {
        const size_t last = size - i;
        const size_t last_parent = get_parent(last, arity);

        const size_t last_parent_index = main_arr[last].index_in_parent;

        const size_t root_index_in_child = 0;
        const size_t last_index_in_child = get_kth_child(last_parent, 1 , arity)
                                            + last_parent_index;

        // All indices are unchanged in swaps
        swap(
            &main_arr[root].value,
            &main_arr[last].value);
        swap(
            &child_arr[root_index_in_child].value,
            &child_arr[last_index_in_child].value);
        
        sift_up_child(  // Heap maximum will become child maximum in last heap
            last_parent,
            last_parent_index,
            main_arr, child_arr,
            arity);
        
        detach_max_child(   // Detach new maximum to never use it in the future
            last_parent,
            main_arr, child_arr,
            size - i + 1,   // Heap size is not yet changed
            arity);

        // After updating children we can safely sift down in heap with new size
        sift_down_main(root, main_arr, child_arr, size - i, arity);
    }

    // Store results in `array`
    for (size_t i = 0; i < size; i++)
        array[i] = main_arr[i].value;

    free(main_arr);
    free(child_arr);
}

static void heapify_main(main_node* main_arr,
                    child_node* child_arr,
                    size_t size,
                    size_t arity)
{
    for (size_t i = 1; i <= size; i++)
        sift_down_main(size - i, main_arr, child_arr, size, arity);
}

static void heapify_children(main_node* main_arr,
                             child_node* child_arr,
                             size_t size,
                             size_t arity)
{
    for (size_t i = 0; i*arity <= size; i++)
        for (size_t j = 1; j <= arity; j++)
                sift_down_child(i, arity - j, main_arr, child_arr, size, arity);
}

static void sift_down_main(size_t index,
                           main_node* main_arr,
                           child_node* child_arr,
                           size_t size,
                           size_t arity)
{
    for(size_t  max_child = get_kth_child(index, 1, arity);
        max_child < size;
        max_child = get_kth_child(index, 1, arity))
    {
        if (main_arr[index].value >= child_arr[max_child].value)
            break;
        const size_t   cur_parent_index = main_arr[index    ].index_in_parent;
        const size_t child_parent_index = 0;    // Max child is root
        const size_t child_main_index   = child_arr[max_child].index_in_main;

        // Absolute index in `child_arr`
        const size_t index_in_child = (index
                                        ? get_kth_child(
                                            get_parent(index, arity),
                                            1, arity) + cur_parent_index
                                        : index); // Different rule for root

        // All indices are unchanged in swaps
        swap(
            &main_arr[index           ].value,
            &main_arr[child_main_index].value);
        swap(
            &child_arr[index_in_child].value,
            &child_arr[max_child     ].value);
        if (index) // Possible sift-up in parent heap (only if parent exists)
            sift_up_child(
                get_parent(index, arity),
                cur_parent_index,
                main_arr, child_arr,
                arity);
        sift_down_child( // Possible sift-down in child heap
            index, // == get_parent(child_main_index, arity)
            child_parent_index,
            main_arr, child_arr,
            size, arity);

        index = child_main_index;
    }
}

static void sift_down_child(size_t parent_index,
                            size_t index,
                            main_node* main_arr,
                            child_node* child_arr,
                            size_t size,
                            size_t arity)
{
    const size_t heap_start = get_kth_child(parent_index, 1, arity);
    if (heap_start >= size) return;

    const size_t heap_size  = heap_start + arity <= size
                                    ? arity
                                    : size - heap_start;
    child_node* heap  = child_arr + heap_start;

    while (2*index + 1 < heap_size)
    {
        const size_t left_child  = 2*index + 1;
        const size_t right_child = 2*index + 2;

        size_t max_child = left_child;
        if (right_child < heap_size &&
            heap[right_child].value > heap[left_child].value)
        {
            max_child = right_child;
        }
        
        if (heap[index].value > heap[max_child].value)
            break;  // Nothing to do from this point onwards

        const size_t cur_main_index   = heap[index    ].index_in_main;
        const size_t child_main_index = heap[max_child].index_in_main;

        swap(heap + index, heap + max_child);

        // Update back-reference indices
        main_arr[cur_main_index  ].index_in_parent = max_child;
        main_arr[child_main_index].index_in_parent = index;

        index = max_child;
    }
}

static void sift_up_child(size_t parent_index,
                          size_t index,
                          main_node* main_arr,
                          child_node* child_arr,
                          size_t arity)
{
    const size_t heap_start = get_kth_child(parent_index, 1, arity);
    child_node* heap  = child_arr + heap_start;

    while(index)
    {
        size_t parent = (index - 1) / 2;
        if (heap[parent].value > heap[index].value)
            break;  // Nothing to do from this point onwards

        const size_t cur_main_index    = heap[index ].index_in_main;
        const size_t parent_main_index = heap[parent].index_in_main;

        // Update back-reference indices
        swap(heap + index, heap + parent);

        main_arr[cur_main_index   ].index_in_parent = parent;
        main_arr[parent_main_index].index_in_parent = index;

        index = parent;
    }
}

static void detach_max_child(size_t parent_index,
                             main_node* main_arr,
                             child_node* child_arr,
                             size_t size,
                             size_t arity)
{
    const size_t heap_start = get_kth_child(parent_index, 1, arity);
    const size_t heap_size  = heap_start + arity <= size
                                    ? arity
                                    : size - heap_start;
    child_node* heap  = child_arr + heap_start;

    if (heap_size <= 1) return;     // One-element child heap will be discarded


    const size_t root = 0;
    const size_t last = heap_size - 1;

    const size_t last_main_index = heap[last].index_in_main;

    swap(heap + root, heap + last);

    // Only `last` parent index is updated, as `root` will never be used again
    main_arr[last_main_index].index_in_parent = root;

    sift_down_child(
        parent_index,
        root,
        main_arr, child_arr,
        size - 1,   // Discard extracted maximum
        arity
    );
}

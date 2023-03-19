#ifndef _HEAP_HELPERS_H
#define _HEAP_HELPERS_H

#include <stdint.h>
#include <string.h>

// Children indexed 1 through `arity`
__always_inline
size_t get_kth_child(size_t index, size_t k, size_t arity)
{
    return arity*index + k;
}

__always_inline
size_t get_parent(size_t index, size_t arity)
{
    if (!index) return 0;
    return (index - 1)/arity;
}

template<typename T>
__always_inline
void swap(T* a, T* b)
{
    T tmp = {}; 
    memcpy(&tmp, a,    sizeof(T));
    memcpy(a,    b,    sizeof(T));
    memcpy(b,    &tmp, sizeof(T));
}

template<>
__always_inline
void swap(uint32_t* a, uint32_t* b)
{
    uint32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

#endif

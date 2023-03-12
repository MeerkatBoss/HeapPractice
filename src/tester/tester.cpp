#include <random>
#include <cassert>

#include "tester.h"
#include "tester/helpers.h"
#include "heap/heap_sort.h"

struct test_data
{
    uint32_t*   data;
    size_t      data_size;
    size_t      arity;
};

static void fill_data(test_data* tdata);

#define EXPAND_ARGS(tdata) (tdata.data, tdata.data_size, tdata.arity)
#define FUNCTION_ADAPTER(name) name EXPAND_ARGS

#define REPEAT_TEST(function, repetitions, tdata) do                        \
{                                                                           \
    for (size_t repeat = 0; repeat < repetitions; repeat++)                 \
    {                                                                       \
        uint64_t time_delta = 0;                                            \
        fill_data(&tdata);                                                  \
        PROFILE_FUNCTION(FUNCTION_ADAPTER(function), tdata, time_delta);    \
        mean_time_ms += time_delta;                                         \
        if (time_delta >= 120*1000)                                         \
        {                                                                   \
            mean_time_ms = time_delta*repetitions;                          \
            break;                                                          \
        }                                                                   \
    }                                                                       \
    mean_time_ms /= repetitions;                                            \
} while(0)

#define RUN_TEST(test_num, function, tdata, seed) do                                            \
{                                                                                               \
    srand((unsigned)seed);                                                                      \
    for (size_t sample_size = initial_size; sample_size <= end_size; sample_size+=step_size)    \
    {                                                                                           \
        tdata.data_size = sample_size;                                                          \
        uint64_t mean_time_ms = 0;                                                              \
        REPEAT_TEST(function, repetitions, tdata);                                              \
        fprintf(output_file, STR(function)"%zu,%zu,%lu\n",                                      \
                            test_num, sample_size, mean_time_ms);                               \
        if (mean_time_ms >= 60*1000) break;                                                     \
    }                                                                                           \
} while(0)

void run_all_tests()
{
#define INITIAL_SIZE(val) size_t initial_size = val;
#define END_SIZE(val)     size_t end_size = val;
#define STEP_SIZE(val)    size_t step_size = val;
#define REPEAT(val)       size_t repetitions = val;
#define HEAP_TYPES(...)   size_t heap_types[] = {__VA_ARGS__};

    FILE* output_file = fopen("results/" STR(TEST_CASE_NAME) ".csv", "w+");
    setvbuf(output_file, NULL, _IONBF, 0);

    fputs("Algorithm,Sample Size,Elapsed Time (ms)\n", output_file);

#include STR(TEST_CASE)

    size_t test_count = sizeof(heap_types) / sizeof(*heap_types);

    time_t seed = time(NULL);
    test_data tdata = {};
    tdata.data = (uint32_t*) calloc(end_size, sizeof(*tdata.data));

    for (size_t i = 0; i < test_count; i++)
    {
        tdata.arity = heap_types[i];

        RUN_TEST(tdata.arity, HeapSort, tdata, seed);
    }

    for (size_t i = 0; i < test_count; i++)
    {
        if (heap_types[i] <= 2) continue;
        tdata.arity = heap_types[i];
        
        RUN_TEST(tdata.arity, HeapHeapSort, tdata, seed);
    }

    fclose(output_file);

#undef INITIAL_SIZE
#undef END_SIZE
#undef STEP_SIZE
#undef REPEAT
#undef HEAP_TYPES
}

static void fill_data(test_data *tdata)
{
    for (size_t i = 0; i < tdata->data_size; i++)
        tdata->data[i] = (unsigned) rand();
}

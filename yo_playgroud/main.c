#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

#include "../yo/yo_internal.h"
#include "../yo/yo_string.h"
#include "../yo/yo_memory.h"

#include "../yo/yo_string.c"
#include "../yo/yo_memory.c"

//
// NOTE(rune): Chained arena test
//
#if 1
int main()
{
    yo_arena_t arena;

    printf("I am the Walrus\n");
}

#endif

//
// NOTE(rune): Quick and dirty HeapAlloc benchmarking
//
#if 0
static void benchmark(uint32_t block_count, uint32_t block_size)
{
    void **blocks = yo_heap_allocate(block_count * sizeof(void *), false);
    YO_ASSERT(blocks);

    yo_perf_timing_t timing_allocate = { 0 };
    yo_perf_timing_t timing_free     = { 0 };

    yo_begin_performance_timing(&timing_allocate);
    for (uint32_t i = 0; i < block_count; i++)
    {
        blocks[i] = yo_heap_allocate(block_size, false);
        YO_ASSERT(blocks[i]);
    }
    yo_end_performance_timing(&timing_allocate);

    yo_begin_performance_timing(&timing_free);
    for (uint32_t i = 0; i < block_count; i++)
    {
        yo_heap_free(blocks[i]);
    }
    yo_end_performance_timing(&timing_free);

    yo_heap_free(blocks);

#if 0
    printf("Block count: %u\tBlock size: %u\tAvg. block allocate: %f ms\tAvg. block free: %f ms\n", block_count, block_size, timing_allocate.millis, timing_free.millis);
#else
    printf("%u,\t%u,\t%f,\t%f\n", block_count, block_size, timing_allocate.millis, timing_free.millis);
#endif
}

int main()
{
    printf("I am the Walrus\n");

#if 0
    benchmark(1024, YO_KILOBYTES(4));
    benchmark(1024, YO_KILOBYTES(64));
    benchmark(1024, YO_MEGABYTES(1));
    benchmark(1024, YO_MEGABYTES(8));

    benchmark(64, YO_KILOBYTES(4));
    benchmark(64, YO_KILOBYTES(64));
    benchmark(64, YO_MEGABYTES(1));
    benchmark(64, YO_MEGABYTES(8));

    benchmark(512, YO_KILOBYTES(4));
    benchmark(512, YO_KILOBYTES(64));
    benchmark(512, YO_MEGABYTES(1));
    benchmark(512, YO_MEGABYTES(8));
#else
    for (uint32_t i = 1; i <= 1024; i++)
    {
        benchmark(1, i * YO_MEGABYTES(1));
    }
#endif
}
#endif







#if 1
#include <stdio.h>
int main()
{
    printf("static const uint8_t decode_table[256] =\n{");

    for (int i = 0; i < 256; i++)
    {
        if (i % 32 == 0) printf("\n    ");

        if ((i >> 3) == 0b11110)      printf("4, "); // 11110xxx
        else if ((i >> 4) == 0b1110)  printf("3, "); // 1110xxxx
        else if ((i >> 5) == 0b110)   printf("2, "); // 111xxxxx
        else if ((i >> 7) == 0)       printf("1, "); // 0xxxxxxx
        else                          printf("0, "); // continuation
    }

    printf("\n};\n");
}
#endif

//
// NOTE(rune): Chained arena test
//
#if 0
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





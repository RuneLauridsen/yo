#pragma once

#ifndef IDK_MAX_TRACKED_ALLOCATIONS
#define IDK_MAX_TRACKED_ALLOCATIONS 64
#endif

#ifndef IDK_ASSERT
#include <assert.h>
#define IDK_ASSERT assert
#endif

#ifndef IDK_PRINTF
#include <stdio.h>
#define IDK_PRINTF printf
#endif

#ifndef IDK_REUSE_TRACKED_ALLOCATION_SLOT
#define IDK_REUSE_TRACKED_ALLOCATION_SLOT 1
#endif

// NOTE(rune): When true, mimir prints on every idk_track_xyz() call.
#ifndef IDK_PRINT_ALLOCATIONS
#endif
#define IDK_PRINT_ALLOCATIONS 1

#ifndef IDK_CONSOLE_STACKTRACE_COLOR
#define IDK_CONSOLE_STACKTRACE_COLOR "\033[90m"
#endif

#ifndef IDK_CONSOLE_RESET
#define IDK_CONSOLE_RESET "\033[0m"
#endif

#define IDK_COALESCE(a,b) ((a)?(a):(b))

#include <stdlib.h>

#define B_STACKTRACE_IMPL
#define B_STACKTRACE_API static
#include "thirdparty/b_stacktrace.h"

typedef struct idk_tracked_allocation idk_tracked_allocation_t;
struct idk_tracked_allocation
{
    void *ptr;
    size_t size;

    char *location_alloc;
    char *location_realloc;
    char *location_free;

    uint32_t realloc_count;
    bool occupied;

    char *stacktrace_alloc;
    char *stacktrace_realloc;
    char *stacktrace_free;
};

idk_tracked_allocation_t idk_tracked_allocations[IDK_MAX_TRACKED_ALLOCATIONS];

// NOTE(rune): Remove first three lines of stack trace, which are from b_stacktrace and mimir functions,
// which the user usually isn't interested in.
static char *idk_strip_stracktrace(char *stacktrace)
{
    if (!stacktrace) return "";

    uint32_t n = 0;

    while (n < 3 && *stacktrace)
    {
        if (*stacktrace++ == '\n') n++;
    }

    return stacktrace;
}

static void idk_print_single_tracked_allocation(char *action, idk_tracked_allocation_t *t)
{
    IDK_PRINTF("=== %s ===\n"
                 "Slot:                 0x%zx\n"
                 "Address:              0x%p\n"
                 "Size:                 0x%zx\n"
                 "Realloc count:        %i\n"
                 "Stacktrace alloc:   \n" IDK_CONSOLE_STACKTRACE_COLOR "%s" IDK_CONSOLE_RESET
                 "Stacktrace realloc: \n" IDK_CONSOLE_STACKTRACE_COLOR "%s" IDK_CONSOLE_RESET
                 "Stacktrace free:    \n" IDK_CONSOLE_STACKTRACE_COLOR "%s" IDK_CONSOLE_RESET
                 "\n",
                 action,
                 t - idk_tracked_allocations,
                 t->ptr,
                 t->size,
                 t->realloc_count,
                 IDK_COALESCE(idk_strip_stracktrace(t->stacktrace_alloc), ""),
                 IDK_COALESCE(idk_strip_stracktrace(t->stacktrace_realloc), ""),
                 IDK_COALESCE(idk_strip_stracktrace(t->stacktrace_free), ""));
}

static idk_tracked_allocation_t *idk_find_tracked_allocation(void *ptr)
{
    idk_tracked_allocation_t *ret = NULL;

    for (size_t i = 0; i < IDK_MAX_TRACKED_ALLOCATIONS; i++)
    {
        idk_tracked_allocation_t *t = &idk_tracked_allocations[i];
        if (t->ptr == ptr && (!t->occupied || ptr != NULL))
        {
            ret = t;
            break;
        }
    }

    return ret;
}

static void idk_track_alloc(char *location, void *ptr, size_t size)
{
    idk_tracked_allocation_t *t = idk_find_tracked_allocation(NULL);
    IDK_ASSERT(t && "Not enough tracked allocation slots. Increase IDK_MAX_TRACKED_ALLOCATIONS.");

    t->ptr                  = ptr;
    t->size                 = size;
    t->location_alloc       = location;
    t->location_realloc     = NULL;
    t->location_free        = NULL;
    t->stacktrace_alloc     = b_stacktrace_get_string();
    t->stacktrace_realloc   = NULL;
    t->stacktrace_free      = NULL;
    t->realloc_count        = 0;
    t->occupied             = true;

    if (IDK_PRINT_ALLOCATIONS) { idk_print_single_tracked_allocation("ALLOCATE", t); }
}

static void idk_track_realloc(char *location, void *old_ptr, void *new_ptr, size_t new_size)
{
    idk_tracked_allocation_t *t = idk_find_tracked_allocation(old_ptr);
    IDK_ASSERT(t && "Bad realloc pointer (old_ptr is not tracked).");

    t->ptr                  = new_ptr;
    t->size                 = new_size;
    t->location_realloc     = location;
    t->stacktrace_realloc   = b_stacktrace_get_string();
    t->realloc_count++;

    if (IDK_PRINT_ALLOCATIONS) { idk_print_single_tracked_allocation("RELLOCATE", t); }
}

static void idk_track_free(char *location, void *ptr)
{
    idk_tracked_allocation_t *t = idk_find_tracked_allocation(ptr);
    IDK_ASSERT(t && "Bad free pointer (ptr is not tracked).");

    t->location_free = location;
    t->stacktrace_free = b_stacktrace_get_string();

    if (IDK_PRINT_ALLOCATIONS)
    {
        idk_print_single_tracked_allocation("FREE", t);
    }

    if (IDK_REUSE_TRACKED_ALLOCATION_SLOT)
    {
        t->ptr = NULL;
        t->occupied = false;
        free(t->stacktrace_alloc);
        free(t->stacktrace_realloc);
        free(t->stacktrace_free);
    }
}

static void idk_print_tracked_allocations(bool print_summary, bool print_individual)
{
    if (print_summary)
    {
        size_t total_count = 0;
        size_t total_size  = 0;
        for (size_t i = 0; i < IDK_MAX_TRACKED_ALLOCATIONS; i++)
        {
            idk_tracked_allocation_t *t = &idk_tracked_allocations[i];
            if (t->occupied)
            {
                total_count++;
                total_size += t->size;
            }
        }

        IDK_PRINTF("=== TRACKED ALLOCATIONS SUMMARY ===\n"
                     "Total count: %zu\n"
                     "Total size:  0x%zx\n\n",
                     total_count,
                     total_size);
    }

    if (print_individual)
    {
        for (size_t i = 0; i < IDK_MAX_TRACKED_ALLOCATIONS; i++)
        {
            idk_tracked_allocation_t *t = &idk_tracked_allocations[i];
            if (t->occupied)
            {
                idk_print_single_tracked_allocation("TRACKED ALLOCATION", t);
            }
        }
    }
}

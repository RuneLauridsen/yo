#pragma once

// NOTE(rune): Small utility for tracking heap allocations, and debugging memory leaks.
// Implementation uses b_stackstrace.h to store stack traces of heap allocations.

#define IDK_STRINGIFY_(a) #a
#define IDK_STRINGIFY(x) IDK_STRINGIFY_(x)
#define IDK_LOCATION (__FILE__ "(" IDK_STRINGIFY(__LINE__) ")")

static void idk_track_alloc(char *location, void *ptr, size_t size);
static void idk_track_realloc(char *location, void *old_ptr, void *new_ptr, size_t new_size);
static void idk_track_free(char *location, void *ptr);
static void idk_print_tracked_allocations(bool print_summary, bool print_individual);
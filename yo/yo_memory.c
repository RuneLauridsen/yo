#pragma once

#if 1
#define IDK_PRINT_ALLOCATIONS 0
#include "../idk/idk.h"
#include "../idk/idk.c"
#endif

#ifndef YO_HEAP_ALLOC
#define YO_HEAP_ALLOC(size, init_to_zero) HeapAlloc(GetProcessHeap(), init_to_zero ? HEAP_ZERO_MEMORY : 0, size)
#endif

#ifndef YO_HEAP_REALLOC
#define YO_HEAP_REALLOC(p, size, init_to_zero) HeapReAlloc(GetProcessHeap(), init_to_zero ? HEAP_ZERO_MEMORY : 0, p, size)
#endif

#ifndef YO_HEAP_FREE
#define YO_HEAP_FREE(p) HeapFree(GetProcessHeap(), 0, p)
#endif

////////////////////////////////////////////////////////////////
//
//
// Basic
//
//
////////////////////////////////////////////////////////////////

static void *yo_memcpy(void *destination, void *source, size_t size)
{
    return memcpy(destination, source, size);
}

static void *yo_memmove(void *destination, void *source, size_t size)
{
    return memmove(destination, source, size);
}

static void *yo_memset(void *destination, uint8_t val, size_t size)
{
    return memset(destination, val, size);
}

static void yo_memset0(void *ptr, size_t size)
{
    yo_memset(ptr, 0, size);
}

static int yo_memcmp(void *a, void *b, size_t size)
{
    return memcmp(a, b, size);
}

static bool yo_memequ(void *a, void *b, size_t size)
{
    bool ret = yo_memcmp(a, b, size) == 0;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Heap
//
//
////////////////////////////////////////////////////////////////

static void *yo_heap_alloc(size_t size, bool init_to_zero)
{
    void *ret = YO_HEAP_ALLOC(size, init_to_zero);
    idk_track_alloc(IDK_LOCATION, ret, size);
    return ret;
}

static void *yo_heap_realloc(void *p, size_t new_size, bool init_to_zero)
{
    void *ret =  YO_HEAP_REALLOC(p, new_size, init_to_zero);
    idk_track_realloc(IDK_LOCATION, p, ret, new_size);
    return ret;
}

static void yo_heap_free(void *p)
{
    idk_track_free(IDK_LOCATION, p);
    YO_HEAP_FREE(p);
}

////////////////////////////////////////////////////////////////
//
//
// Arena
//
//
////////////////////////////////////////////////////////////////

static void yo_arena_init(yo_arena_t *arena, void *memory, size_t size)
{
    yo_zero_struct(arena);

    arena->self.base           = memory;
    arena->self.size_used      = 0;
    arena->self.size_allocated = size;
}

static bool yo_arena_create(yo_arena_t *arena, size_t size, bool init_to_zero, yo_arena_type_t type)
{
    bool ret = false;

    if (!size) size = YO_MEGABYTES(1);

    void *memory = yo_heap_alloc(size, init_to_zero);
    if (memory)
    {
        yo_arena_init(arena, memory, size);
        arena->type = type;
        ret = true;
    }

    return ret;
}

static void yo_arena_destroy(yo_arena_t *arena)
{
    yo_arena_block_t *block = &arena->self;

    while (block)
    {
        // NOTE(rune): We need to store to next pointer locally on the stack, since chained arena blocks
        // are stored inside themselves, and their next pointers would therefore be invalid, after the
        // chain blocks's memory is freed. See: yo_alloc_arena_block.
        yo_arena_block_t *next = next = block->next;
        yo_heap_free(block->base);
        block = next;
    }
}

static void yo_arena_reset(yo_arena_t *arena)
{
    arena->self.size_used = 0;
    arena->chain_head = NULL;
}

static bool yo_fits_in_arena_block(yo_arena_block_t *block, size_t size)
{
    bool ret = block->size_allocated >= block->size_used + size;
    return ret;
}

static yo_arena_block_t *yo_alloc_arena_block(yo_arena_t *arena)
{
    yo_arena_block_t *current = &arena->self;
    if (arena->chain_head) current = arena->chain_head;

    yo_arena_block_t *ret = current->next;
    if (!ret)
    {
        size_t new_size = current->size_allocated;
        if (arena->type == YO_ARENA_TYPE_CHAIN_EXPONENTIAL) { new_size *= 2; }
        void *new_memory = yo_heap_alloc(new_size, false);
        if (new_memory)
        {
            // NOTE(rune): Store the new chain block inside itself.
            ret                 = new_memory;
            ret->base           = new_memory;
            ret->size_allocated = new_size;
            ret->size_used      = sizeof(*ret);
            ret->next           = NULL;
            ret->prev           = NULL;
        }
    }

    yo_dlist_stack_push(arena->chain_head, ret);

    // NOTE(m2dx): In case current was arena->self, we need to set the next-prev pointers manually,
    // since a pointer to self can't be stored in the chain list, because it would be invalidated
    // we arena is copied by value.
    current->next     = ret;
    ret->prev      = current;
    ret->size_used = sizeof(*ret);

    return ret;
}

static void *yo_arena_push_size(yo_arena_t *arena, size_t size, bool init_to_zero)
{
    yo_arena_block_t *block = &arena->self;

    if (arena->type != YO_ARENA_TYPE_NO_CHAIN)
    {
        if (arena->chain_head) { block = arena->chain_head; }

        if (!yo_fits_in_arena_block(block, size))
        {
            yo_arena_block_t *new =  yo_alloc_arena_block(arena);
            if (new)
            {
                block = new;
            }
        }
    }
    else
    {
        YO_ASSERT(arena->chain_head == NULL);
    }

    void *ret = NULL;

    if (yo_fits_in_arena_block(block, size))
    {
        ret = block->base + block->size_used;
        block->size_used += size;

        if (init_to_zero) yo_memset(ret, 0, size);
    }

    return ret;
}

static void *yo_arena_push_size_ensure_pad(yo_arena_t *arena, size_t size, bool init_to_zero, size_t pad)
{
    void *ret = yo_arena_push_size(arena, size + pad, init_to_zero);

    if (ret)
    {
        yo_arena_block_t *block = YO_COALESCE(arena->chain_head, &arena->self);
        YO_ASSERT(block->size_used >= size + pad);
        block->size_used -= pad;
    }

    return ret;
}

static bool yo_arena_begin_temp(yo_arena_t *arena)
{
    bool ret = false;
    arena->temp_depth++;

    yo_arena_temp_t *temp  = yo_arena_push_size(arena, sizeof(yo_arena_temp_t), false);
    if (temp)
    {
        yo_slist_stack_push(arena->temp_stack_head, temp);
        temp->chain_head = arena->chain_head;
        temp->depth = arena->temp_depth;
        ret = true;
    }

    return ret;
}

static void yo_arena_end_temp(yo_arena_t *arena)
{
    YO_ASSERT(arena->temp_depth > 0 && "Arena temp stack underflow.");

    yo_arena_temp_t *temp = arena->temp_stack_head;
    if (temp)
    {
        YO_ASSERT(temp->depth <= arena->temp_depth);

        if (temp->depth == arena->temp_depth)
        {
            yo_slist_stack_pop(arena->temp_stack_head);
            yo_arena_block_t *restore_to_block = YO_COALESCE(temp->chain_head, &arena->self);
            restore_to_block->size_used = (uint8_t *)temp - restore_to_block->base;
            arena->chain_head = temp->chain_head;
        }
    }

    arena->temp_depth--;
}

////////////////////////////////////////////////////////////////
//
//
// Dynamic array
//
//
////////////////////////////////////////////////////////////////

static bool yo_array_void_create(yo_array_void_t *array, size_t elem_size, size_t initial_capacity, bool init_to_zero)
{
    YO_ASSERT(array);
    YO_ASSERT(initial_capacity);
    YO_ASSERT(elem_size);

    // TODO(rune): Error handling
    array->elems            = yo_heap_alloc(initial_capacity * elem_size, init_to_zero);
    array->elem_size        = elem_size;
    array->count_allocated  = initial_capacity;
    array->count            = 0;
    return true;
}

static void yo_array_void_destroy(yo_array_void_t *array)
{
    if (array)
    {
        yo_heap_free(array->elems);
        yo_zero_struct(array);
    }
}

static bool yo_array_void_reserve(yo_array_void_t *array, size_t elem_size, size_t reserve_count, bool init_to_zero)
{
    YO_ASSERT(array->elem_size == elem_size);

    bool ret = false;

    if (array->count_allocated < reserve_count)
    {
        // NOTE(rune): Protects against infinite loop
        YO_ASSERT(array->count_allocated);

        size_t new_count_allocated = array->count_allocated;
        while (new_count_allocated < reserve_count)
        {
            new_count_allocated *= 2;
        }

        void *new_elems = yo_heap_realloc(array->elems, new_count_allocated * elem_size, init_to_zero);
        if (new_elems)
        {
            array->elems           = new_elems;
            array->count_allocated = new_count_allocated;

            ret = true;
        }
    }
    else
    {
        ret = true;
    }

    return ret;
}

static void *yo_array_void_add(yo_array_void_t *array, size_t elem_size, size_t count, bool init_to_zero)
{
    YO_ASSERT(array->elem_size == elem_size);

    void *ret = NULL;

    if (yo_array_void_reserve(array, elem_size, array->count + count, init_to_zero))
    {
        ret = (uint8_t *)array->elems + array->count * elem_size;
        array->count += count;
    }

    return ret;
}

static void *yo_array_void_pop(yo_array_void_t *array, size_t elem_size, size_t count)
{
    YO_ASSERT(array->elem_size == elem_size);

    void *ret = NULL;

    if (array->count >= count)
    {
        array->count -= count;
        ret = (uint8_t *)array->elems + array->count * elem_size;
    }

    return ret;
}

static bool yo_array_void_remove(yo_array_void_t *array, size_t elem_size, size_t idx, size_t remove_count)
{
    YO_ASSERT(array->elem_size == elem_size);

    bool ret = false;

    if (array->count >= idx + remove_count)
    {
        yo_memmove((uint8_t *)array->elems + elem_size * idx,
                   (uint8_t *)array->elems + elem_size * (idx + remove_count),
                   elem_size * (array->count - idx - remove_count));

        array->count -= remove_count;
        ret = true;
    }

    return ret;
}

static void *yo_array_void_push(yo_array_void_t *array, size_t elem_size, bool init_to_zero)
{
    void *ret = yo_array_void_add(array, elem_size, 1, init_to_zero);
    return ret;
}

static void yo_array_void_reset(yo_array_void_t *array, size_t elem_size, bool clear_to_zero)
{
    array->count = 0;

    if (clear_to_zero)
    {
        yo_memset0(array->elems, array->count_allocated * elem_size);
    }
}

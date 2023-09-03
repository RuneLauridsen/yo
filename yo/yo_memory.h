#pragma once

////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

#define yo_zero_struct(a)           yo_memset0((a), sizeof(*(a)))
#define yo_zero_struct_array(a,c)   yo_memset0((a), sizeof(*(a)) * c)
#define yo_equal_struct(a,b)        (yo_memcmp(a, b, sizeof(a)) == 0)

////////////////////////////////////////////////////////////////
//
//
// Basic
//
//
////////////////////////////////////////////////////////////////

static void *   yo_memcpy(void *destination, void *source, size_t size);
static void *   yo_memmove(void *destination, void *source, size_t size);
static void *   yo_memset(void *destination, uint8_t val, size_t size);
static void     yo_memset0(void *ptr, size_t size);
static int      yo_memcmp(void *a, void *b, size_t size);
static bool     yo_memequ(void *a, void *b, size_t size);

////////////////////////////////////////////////////////////////
//
//
// Heap
//
//
////////////////////////////////////////////////////////////////

// NOTE(rune): MSVC magic memory numbers: https://stackoverflow.com/a/127404
// 0xabababab : Used by Microsoft's HeapAlloc() to mark "no man's land" guard bytes after allocated heap memory
// 0xcccccccc : Used by Microsoft's C++ debugging runtime library to mark uninitialised stack memory
// 0xcdcdcdcd : Used by Microsoft's C++ debugging runtime library to mark uninitialised heap memory
// 0xdddddddd : Used by Microsoft's C++ debugging heap to mark freed heap memory
// 0xfdfdfdfd : Used by Microsoft's C++ debugging heap to mark "no man's land" guard bytes before and after allocated heap memory
// 0xfeeefeee : Used by Microsoft's HeapFree() to mark freed heap memory

static void *   yo_heap_alloc(size_t size, bool init_to_zero);
static void *   yo_heap_realloc(void *p, size_t new_size, bool init_to_zero);
static void     yo_heap_free(void *p);

////////////////////////////////////////////////////////////////
//
//
// Linked lists
//
//
////////////////////////////////////////////////////////////////

#define YO_SLQUEUE_JOIN(fa,la,fb,lb,next)                                                \
    if ((fa) == NULL) { YO_ASSERT(la == NULL); (fa) = (fb); (la) = (lb); }               \
    else              { YO_ASSERT((la)->next == NULL); (la)->next = (fb); (la) = (lb); } \

#define YO_SLQUEUE_PUSH(f,l,next,n)                     \
    YO_SLQUEUE_JOIN(f,l,n,n,next)

#define YO_SLQUEUE_POP(f,l,next)                        \
    if ((f) == (l)) { (f) = (l) = NULL; }               \
    else            { (f) = (f)->next; }                \

#define YO_SLSTACK_PUSH(f,next,n)                       \
    (n)->next = (f);                                    \
    (f) = (n);

#define YO_SLSTACK_POP(f,next)                          \
    if((f)) { (f) = (f)->next; }

#define YO_DLIST_PUSH_BACK(f,l,next,prev,n)             \
    if ((f) == NULL)                                    \
    {                                                   \
        YO_ASSERT((l) == NULL);                         \
        (f) = n;                                        \
        (l) = n;                                        \
    }                                                   \
    else                                                \
    {                                                   \
        YO_ASSERT((f)->prev == NULL);                   \
        YO_ASSERT((l)->next == NULL);                   \
        (l)->next = (n);                                \
        (n)->prev = (l);                                \
        (l) = (n);                                      \
    }

#define YO_DLIST_PUSH_FRONT(f,l,next,prev,n)            \
    YO_DLIST_PUSH_BACK(l,f,prev,next,n)                 \

#define YO_DLIST_REMOVE(f,l,next,prev,n)                \
    if((f) == (n))                                      \
    {                                                   \
        if ((f)->next)                                  \
        {                                               \
            (f) = (f)->next;                            \
            (f)->prev = NULL;                           \
        }                                               \
        else                                            \
        {                                               \
            (f) = NULL;                                 \
        }                                               \
    }                                                   \
    else                                                \
    {                                                   \
        (n)->prev->next = (n)->next;                    \
    }                                                   \
                                                        \
    if ((l) == (n))                                     \
    {                                                   \
        if ((l)->prev)                                  \
        {                                               \
            (l) = (l)->prev;                            \
            (l)->next = NULL;                           \
        }                                               \
        else                                            \
        {                                               \
            (l) = NULL;                                 \
        }                                               \
    }                                                   \
    else                                                \
    {                                                   \
        (n)->next->prev = (n)->prev;                    \
    }                                                   \
                                                        \
    (n)->next = NULL;                                   \
    (n)->prev = NULL;

#define YO_DLIST_INSERT(f, l, next, prev, after, n)     \
    if ((after) == (l))                                 \
    {                                                   \
        YO_DLIST_PUSH_BACK(f, l, next, prev, n);        \
    }                                                   \
    else                                                \
    {                                                   \
        (n)->next = (after)->next;                      \
        (n)->prev = (after);                            \
        (after)->next->prev = n;                        \
        (after)->next = n;                              \
    }

#define YO_DLIST_STACK_PUSH(head, next, prev, n)        \
    if (head) (head)->next = (n);                       \
    (n)->prev = (head);                                 \
    (head) = (n);

#define YO_DLIST_STACK_POP(head, next, prev)            \
    (head) = (head)->prev;                              \
    if (head) (head)->next = NULL;

#define YO_DLIST_STACK_REMOVE(head, next, prev, n)          \
    if ((n)->next) (n)->next->prev = (n)->prev;             \
    if ((n)->prev) (n)->prev->next = (n)->next;             \
    if ((n) == (head)) YO_DLIST_STACK_POP(head, next, prev) \
    (n)->next = NULL;                                       \
    (n)->prev = NULL;

// TODO(rune): Better naming than slist, slist_stack, dlist and dlist_stack. Depending on the combination
// of 'first-last' <> 'head' and 'next-prev' <> 'next' pointers, we end up with have 4 types of linked list.
//
//    slist_queue:    first-last      next
//    slist_stack:    head            next
//    dlist:          first-last      next-prev
//    dlist_stack:    head            next-prev
//

#define yo_slist(T)                       struct { T *first, *last; }
#define yo_slist_queue_join(a,b)          YO_SLQUEUE_JOIN((a)->first, (a)->last, (b)->first, (b)->last,next)
#define yo_slist_queue_push(list,n)       YO_SLQUEUE_PUSH((list)->first, (list)->last, next, n)
#define yo_slist_queue_pop(list)          YO_SLQUEUE_POP((list)->first, (list)->last, next)
#define yo_slist_each(T, it, list)        T it = (list); it; it = it->next

#define yo_slist_stack_push(head, n)      YO_SLSTACK_PUSH(head, next, n)
#define yo_slist_stack_pop(head)          YO_SLSTACK_POP(head, next)

#define yo_dlist(T)                       struct { T *first, *last; }
#define yo_dlist_push_front(list, node)   YO_DLIST_PUSH_FRONT((list)->first, (list)->last, next, prev, node)
#define yo_dlist_push_back(list, node)    YO_DLIST_PUSH_BACK ((list)->first, (list)->last, next, prev, node)
#define yo_dlist_insert_after(list, node, after) YO_DLIST_INSERT   ((list)->first, (list)->last, next, prev, after, node)
#define yo_dlist_remove(list, node)       YO_DLIST_REMOVE    ((list)->first, (list)->last, next, prev, node)
#define yo_dlist_add(list, node)          yo_dlist_push_back(list, node)
#define yo_dlist_each(T, it, list)        T it = (list)->first; it; it = it->next

#define yo_dlist_stack_push(head, node)   YO_DLIST_STACK_PUSH(head, next, prev, node)
#define yo_dlist_stack_pop(head)          YO_DLIST_STACK_POP(head, next, prev)
#define yo_dlist_stack_remove(head, node) YO_DLIST_STACK_REMOVE(head, next, prev, node)

////////////////////////////////////////////////////////////////
//
//
// Arena
//
//
////////////////////////////////////////////////////////////////

typedef uint32_t yo_arena_type_t;
enum
{
    YO_ARENA_TYPE_NO_CHAIN,
    YO_ARENA_TYPE_CHAIN_LINEAR,
    YO_ARENA_TYPE_CHAIN_EXPONENTIAL
};

typedef struct yo_arena_block yo_arena_block;
struct yo_arena_block
{
    uint8_t *base;
    size_t   size_used;
    size_t   size_allocated;
    yo_arena_block *next;
    yo_arena_block *prev;
};

// NOTE(rune): Stores a snapshot of the arena's state, so that it can be restored with yo_arena_end_temp.
typedef struct yo_arena_temp yo_arena_temp_t;
struct yo_arena_temp
{
    yo_arena_block  *chain_head;
    yo_arena_temp_t *next;

    // NOTE(rune): In case the there wasn't enough space to allocate a yo_arena_temp_t in
    // yo_arena_begin_temp, we still force the user to call yo_end_temp, and use arena.temp_depth
    // to determine if we are popping the right yo_arena_temp_t.
    uint32_t depth;
};

typedef struct yo_arena yo_arena_t;
struct yo_arena
{
    // NOTE(rune): We store the first chain_link by value, to avoid double-indirection in
    // the typical YO_ARENA_TYPE_NO_CHAIN case.
    yo_arena_block self;

    // WARNING(rune): Since arena_t is allowed to be copied by value, we can't store a pointer
    // to arena.self in chain list. This makes to arena_push_size implementation slightly more
    // complex, since its means that, if chain_head is NULL, the current chain link is arena.self.
    yo_arena_block *chain_head;

    yo_arena_temp_t *temp_stack_head;
    uint32_t         temp_depth;

    yo_arena_type_t type;
};

#define yo_arena_push_struct(arena, T, init_to_zero)        (T *)(yo_arena_push_size((arena), sizeof(T), init_to_zero))
#define yo_arena_push_array(arena, count, T, init_to_zero)  (T *)(yo_arena_push_size((arena), sizeof(T) * (count), init_to_zero))

static void     yo_arena_init(yo_arena_t *arena, void *memory, size_t size);
static bool     yo_arena_create(yo_arena_t *arena, size_t size, bool init_to_zero, yo_arena_type_t type);
static void     yo_arena_destroy(yo_arena_t *arena);
static void     yo_arena_reset(yo_arena_t *arena);

static void *   yo_arena_push_size(yo_arena_t *arena, size_t size, bool init_to_zero);
static char *   yo_arena_push_cstring(yo_arena_t *arena, char *cstring);

static bool     yo_arena_begin_temp(yo_arena_t *arena);
static void     yo_arena_end_temp(yo_arena_t *arena);

#define YO_ARENA_TEMP_SCOPE(arena)  YO_DEFER_LOOP(yo_arena_begin_temp(arena), yo_arena_end_temp(arena))

////////////////////////////////////////////////////////////////
//
//
// Dynamic array
//
//
////////////////////////////////////////////////////////////////

#define yo_array(T)                                                 yo_array_##T
#define yo_array_count(array)                                       ((array)->count)
#define yo_array_size(array)                                        ((array)->count * yo_array_elem_size(array))
#define yo_array_size_allocated(array)                              ((array)->count_allocated * yo_array_elem_size(array))
#define yo_array_is_empty(array)                                    ((array)->count == 0)
#define yo_array_first(array)                                       (*((array)->count > 0 ? &(array)->elems[0]                  : NULL))
#define yo_array_first_or_default(array, default)                   ( ((array)->count > 0 ?  (array)->elems[0]                  : default))
#define yo_array_last(array)                                        (*((array)->count > 0 ? &(array)->elems[(array)->count - 1] : NULL))
#define yo_array_last_or_default(array, default)                    ( ((array)->count > 0 ?  (array)->elems[(array)->count - 1] : default))
#define yo_array_elem_size(array)                                   (sizeof(*(array)->elems))

#define yo_array_create(array, initial_capacity, init_to_zero)      yo_array_void_create     (&(array)->as_void_array, yo_array_elem_size(array), initial_capacity, init_to_zero)
#define yo_array_destroy(array)                                     yo_array_void_destroy    (&(array)->as_void_array)
#define yo_array_reserve(array, reserve_count, init_to_zero)        yo_array_void_reserve    (&(array)->as_void_array, yo_array_elem_size(array), reserve_count, init_to_zero)
#define yo_array_add(array, add_count, init_to_zero)                yo_array_void_add        (&(array)->as_void_array, yo_array_elem_size(array), add_count, init_to_zero)
#define yo_array_pop(array, pop_count)                              yo_array_void_pop        (&(array)->as_void_array, yo_array_elem_size(array), pop_count)
#define yo_array_remove(array, idx, remove_count)                   yo_array_void_remove     (&(array)->as_void_array, yo_array_elem_size(array), idx, remove_count)
#define yo_array_reset(array, clear_to_zero)                        yo_array_void_reset      (&(array)->as_void_array, yo_array_elem_size(array), clear_to_zero)
#define yo_array_put(array, val)                                    (yo_array_add(array, 1, false) ? ((array)->elems[(array)->count - 1] = (val), true) : false)

// NOTE(rune): The elem_size field is not strictly necessary, since elem_size is passed as
// a compile time constant argument to all yo_array_void_x functions anyway, buts its nice if you
// want to write code, that works on any array(T).

#define YO_ARRAY_FIELDS(T)      T *elems; size_t count, count_allocated, elem_size
#define YO_TYPEDEF_ARRAY(T)     typedef union { struct { YO_ARRAY_FIELDS(T); }; yo_array_void_t as_void_array; }  yo_array_##T

typedef struct { YO_ARRAY_FIELDS(void); } yo_array_void_t;

YO_TYPEDEF_ARRAY(char);
YO_TYPEDEF_ARRAY(uint8_t);
YO_TYPEDEF_ARRAY(uint16_t);
YO_TYPEDEF_ARRAY(uint32_t);
YO_TYPEDEF_ARRAY(uint64_t);
YO_TYPEDEF_ARRAY(int8_t);
YO_TYPEDEF_ARRAY(int16_t);
YO_TYPEDEF_ARRAY(int32_t);
YO_TYPEDEF_ARRAY(int64_t);

static bool     yo_array_void_create(yo_array_void_t *array, size_t elem_size, size_t initial_capacity, bool init_to_zero);
static void     yo_array_void_destroy(yo_array_void_t *array);
static bool     yo_array_void_reserve(yo_array_void_t *array, size_t elem_size, size_t reserve_count, bool init_to_zero);
static void *   yo_array_void_add(yo_array_void_t *array, size_t elem_size, size_t count, bool init_to_zero);
static void *   yo_array_void_pop(yo_array_void_t *array, size_t elem_size, size_t count);
static bool     yo_array_void_remove(yo_array_void_t *array, size_t elem_size, size_t idx, size_t remove_count);
static void *   yo_array_void_push(yo_array_void_t *array, size_t elem_size, bool init_to_zero);
static void     yo_array_void_reset(yo_array_void_t *array, size_t elem_size, bool clear_to_zero);

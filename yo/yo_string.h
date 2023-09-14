#pragma once

////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

#define yo_string(literal)      (yo_string_t) { (uint8_t *)literal, sizeof(literal) - 1 }
#define yo_string_fmt(s)        (int)(s).length, (s).data   // NOTE(rune): For using yo_string_t in printf
#define YO_EMPTY_STRING         (yo_string_t) { NULL, 0 }
#define YO_NOT_FOUND            (size_t)SIZE_MAX

////////////////////////////////////////////////////////////////
//
//
// Null terminated string
//
//
////////////////////////////////////////////////////////////////

static size_t       yo_cstring_length(char *s);
static bool         yo_cstring_equal(char *a, char *b);

////////////////////////////////////////////////////////////////
//
//
// Counted string
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_string yo_string_t;
struct yo_string
{
    uint8_t *data;
    size_t length;
};

static yo_string_t  yo_from_cstring(char *cstring);
static bool         yo_string_equal(yo_string_t a, yo_string_t b);
static size_t       yo_index_of_char(yo_string_t s, uint8_t c);
static size_t       yo_index_of_string(yo_string_t a, yo_string_t b);
static yo_string_t  yo_substring(yo_string_t s, size_t index);
static yo_string_t  yo_substring_len(yo_string_t s, size_t index, size_t length);
static yo_string_t  yo_split_pop_first(yo_string_t *a, yo_string_t b);
static yo_string_t  yo_trim(yo_string_t s);
static yo_string_t  yo_trim_start(yo_string_t s);
static yo_string_t  yo_trim_end(yo_string_t s);
static bool         yo_starts_with(yo_string_t s, yo_string_t prefix);
static bool         yo_ends_with(yo_string_t s, yo_string_t suffix);

////////////////////////////////////////////////////////////////
//
//
// Unicode
//
//
////////////////////////////////////////////////////////////////

static uint32_t     yo_utf8_codepoint_advance(yo_string_t *s);

////////////////////////////////////////////////////////////////
//
//
// String builder
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_builder yo_builder_t;
struct yo_builder
{
    yo_arena_t arena;
    size_t length;

    // NOTE(rune): Set to false, whenever an append operation fails. Subsequent append operations will be no-ops.
    bool ok;
};

// TODO(rune): It would be nice to have a yo_builder_init(yo_arena_t *), in case you want use a temp storage arena.

static bool         yo_builder_create(yo_builder_t *builder, size_t initial_size);
static void         yo_builder_destroy(yo_builder_t *builder);
static void         yo_builder_reset(yo_builder_t *builder);
static void         yo_builder_append(yo_builder_t *builder, yo_string_t s);
static void         yo_builder_append_c(yo_builder_t *builder, char c);
static void         yo_builder_append_f(yo_builder_t *builder, char *format, ...);
static void         yo_builder_append_v(yo_builder_t *builder, char *format, va_list args);
static yo_string_t  yo_builder_to_string(yo_builder_t *builder, yo_arena_t *arena);
static yo_string_t  yo_builder_to_string_and_destroy(yo_builder_t *builder, yo_arena_t *arena);
static yo_string_t  yo_builder_to_string_and_reset(yo_builder_t *builder, yo_arena_t *arena);

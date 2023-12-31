#pragma once

////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

#define yo_string(s)        (yo_string_t) { (s), strlen(s) }
#define YO_EMPTY_STRING     (yo_string_t) { NULL, 0 }
#define YO_NOT_FOUND        (size_t)SIZE_MAX

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
// Known length string
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_string yo_string_t;
struct yo_string
{
    char *chars;
    size_t length;
};

static bool         yo_string_equal(yo_string_t a, yo_string_t b);
static size_t       yo_index_of_char(yo_string_t s, char c);
static size_t       yo_index_of_string(yo_string_t a, yo_string_t b);
static yo_string_t  yo_substring(yo_string_t s, size_t index);
static yo_string_t  yo_substringl(yo_string_t s, size_t index, size_t length);
static yo_string_t  yo_split_pop_first(yo_string_t *a, yo_string_t b);

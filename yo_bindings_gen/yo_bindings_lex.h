#pragma once

// WARNING(rune): This is far from a robust C-lexer, and should no be used on anything other than yo_xyz headers!

typedef uint32_t yo_token_type_t;
enum yo_token_type
{
    // NOTE(rune): Reserve first 128 values to match ASCII.

    YO_TOKEN_IDEN = 128,
    YO_TOKEN_EOF,
    YO_TOKEN_COUNT,
};

typedef struct yo_token yo_token_t;
struct yo_token
{
    yo_token_type_t type;
    yo_string_t text;

    yo_token_t *next;
    yo_token_t *prev;
};

typedef struct yo_lexer yo_lex_t;
struct yo_lexer
{
    yo_string_t text;
    size_t mark;
    size_t at;
    yo_dlist(yo_token_t) tokens;
};

static yo_token_t *yo_lex(yo_string_t source_code, yo_arena_t *arena);

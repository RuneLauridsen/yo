#pragma once

// WARNING(rune): This is far from a robust C-parser, and should no be used on anything other than yo_xyz headers!

typedef struct yo_parsed_type yo_parsed_type_t;
struct yo_parsed_type
{
    yo_string_t name;
    uint32_t indirection;
};

typedef struct yo_parsed_arg yo_parsed_arg_t;
struct yo_parsed_arg
{
    yo_string_t name;
    yo_parsed_type_t type;
    bool is_va_arg;
    bool invalid;

    yo_parsed_arg_t *next;
};

typedef struct yo_parsed_func yo_parsed_func_t;
struct yo_parsed_func
{
    yo_string_t name;
    yo_parsed_type_t return_type;
    yo_slist(yo_parsed_arg_t) args;
    bool invalid;

    yo_parsed_func_t *next;
};

typedef struct yo_parsed_enum_member yo_parsed_enum_member_t;
struct yo_parsed_enum_member
{
    yo_string_t name;
    yo_string_t expression;
    yo_parsed_enum_member_t *next;
};

typedef struct yo_parsed_enum yo_parsed_enum_t;
struct yo_parsed_enum
{
    yo_string_t name;
    yo_parsed_type_t type;
    yo_slist(yo_parsed_enum_member_t) members;
    yo_parsed_enum_t *next;
};

typedef struct yo_parsed_header yo_parsed_header_t;
struct yo_parsed_header
{
    yo_slist(yo_parsed_func_t) funcs;
    yo_slist(yo_parsed_enum_t) enums;
};

typedef struct yo_parser yo_parser_t;
struct yo_parser
{
    yo_token_t *token;
    yo_arena_t *arena;
    yo_parsed_header_t result;
};

static yo_parsed_header_t yo_parse_header(yo_token_t *tokens, yo_arena_t *arena);
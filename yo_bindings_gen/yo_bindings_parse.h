#pragma once

// WARNING(rune): This is far from a robust C-parser, and should no be used on anything other than yo_xyz headers!

typedef struct yo_type_spec yo_type_spec_t;
struct yo_type_spec
{
    yo_string_t name;
    uint32_t indirection;
};

typedef struct yo_arg_spec yo_arg_spec_t;
struct yo_arg_spec
{
    yo_string_t name;
    yo_type_spec_t type;
    bool is_va_arg;
    bool invalid;

    yo_arg_spec_t *next;
};

typedef struct yo_func_spec yo_func_spec_t;
struct yo_func_spec
{
    yo_string_t name;
    yo_type_spec_t return_type;
    yo_slist(yo_arg_spec_t) args;
    bool invalid;

    yo_func_spec_t *next;
};

typedef struct yo_enum_member_spec yo_enum_member_spec_t;
struct yo_enum_member_spec
{
    yo_string_t name;
    yo_string_t expression;
    yo_enum_member_spec_t *next;
};

typedef struct yo_enum_spec yo_enum_spec_t;
struct yo_enum_spec
{
    yo_string_t name;
    yo_type_spec_t type;
    yo_slist(yo_enum_member_spec_t) members;
    yo_enum_spec_t *next;
};

typedef struct yo_struct_spec yo_struct_spec_t;
struct yo_struct_spec
{
    yo_string_t name;
};

typedef struct yo_api_spec yo_api_spec_t;
struct yo_api_spec
{
    yo_slist(yo_func_spec_t) funcs;
    yo_slist(yo_enum_spec_t) enums;
    yo_slist(yo_struct_spec_t) structs;
};

typedef struct yo_parser yo_parse_t;
struct yo_parser
{
    yo_token_t *token;
    yo_arena_t *arena;
    yo_api_spec_t result;
};

static yo_api_spec_t yo_parse(yo_token_t *tokens, yo_arena_t *arena);

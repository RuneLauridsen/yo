#pragma once

// WARNING(rune): This is far from a robust C-lexer, and should no be used on anything other than yo_xyz headers!

static inline bool yo_lex_done(yo_lex_t *l)
{
    bool ret = l->at >= l->text.length;
    return ret;
}

static inline char yo_lex_at(yo_lex_t *l)
{
    YO_ASSERT(l->at < l->text.length);
    char ret = l->text.data[l->at];
    return ret;
}

static inline char yo_lex_at_offset(yo_lex_t *l, size_t offset)
{
    YO_ASSERT(l->at + offset < l->text.length);
    char ret = l->text.data[l->at + offset];
    return ret;
}

static void yo_lex_advance(yo_lex_t *l)
{
    YO_ASSERT(l->at + 1 < l->text.length);
    l->at += 1;
}

static void yo_lex_skip(yo_lex_t *l)
{
    YO_ASSERT(l->at   + 1 <= l->text.length);
    YO_ASSERT(l->mark + 1 <= l->text.length);

    l->at   += 1;
    l->mark += 1;
}

static void yo_lex_skip_line(yo_lex_t *l)
{
    while (yo_lex_at(l) != '\n' && yo_lex_at(l) != '\r')
    {
        yo_lex_skip(l);
    }
}

static void yo_lex_skip_whitespace(yo_lex_t *l)
{
    while (!yo_lex_done(l) && yo_is_whitespace_c(yo_lex_at(l)))
    {
        yo_lex_skip(l);
    }
}

static yo_token_t *yo_lex_add_token(yo_lex_t *l, yo_token_type_t type, yo_arena_t *arena)
{
    YO_ASSERT(l->mark <= l->at);

    yo_token_t *t  = yo_arena_push_struct(arena, yo_token_t, true);
    t->text.data   = l->text.data + l->mark;
    t->text.length = l->at - l->mark;
    t->type        = type;

    yo_dlist_add(&l->tokens, t);

    l->mark = l->at;

    return t;
}

static yo_token_t *yo_lex(yo_string_t source_code, yo_arena_t *arena)
{
    yo_lex_t _l = { .text = source_code }, *l = &_l;

    while (!yo_lex_done(l))
    {
        char c      = yo_lex_at(l);
        char c_next = yo_lex_at_offset(l, 1);

        if (yo_is_whitespace_c(c))
        {
            yo_lex_skip_whitespace(l);
        }
        else if (c == '#')
        {
            yo_lex_skip_line(l);
        }
        else if (c == '/' && c_next == '/')
        {
            yo_lex_skip_line(l);
        }
        else if (
            c == '(' || c == ')' || c == '[' || c == ']' ||
            c == '{' || c == '}' || c == ';' || c == ':' ||
            c == '.' || c == ',' || c == '"' || c == '\'' ||
            c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '=' || c == '!' || c == '<' || c == '>')
        {
            yo_lex_advance(l);
            yo_lex_add_token(l, c, arena);
        }
        // TODO(rune): Numbers are currently lexed as identifiers
        else if (yo_is_word_c(c) || yo_is_digit_c(c))
        {
            while ((!yo_lex_done(l)) && (yo_is_word_c(c) || yo_is_digit_c(c)))
            {
                yo_lex_advance(l);
                c = yo_lex_at(l);
            }

            yo_lex_add_token(l, YO_TOKEN_IDEN, arena);
        }
        else
        {
            yo_lex_skip(l);
        }
    }


    // NOTE(rune): Make the ends of the linked list point to themselves, so the parser can assume,
    // that token->next will never be NULL.

    yo_token_t *eof = yo_arena_push_struct(arena, yo_token_t, true);
    eof->next = eof;
    eof->prev = eof;
    eof->type = YO_TOKEN_EOF;
    eof->text = YO_EMPTY_STRING;

    l->tokens.first->prev = eof;

    yo_token_t *ret = l->tokens.first;

    return ret;
}

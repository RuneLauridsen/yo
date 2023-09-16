#pragma once

////////////////////////////////////////////////////////////////
//
//
// Null terminated string
//
//
////////////////////////////////////////////////////////////////

static size_t yo_cstring_length(char *s)
{
    char *c = s;

    while (*c)
    {
        c++;
    }

    size_t ret = c - s;
    return ret;
}

static bool yo_cstring_equal(char *a, char *b)
{
    bool ret = false;

    if (a && b)
    {
        while ((*a) && (*a == *b))
        {
            a++;
            b++;
        }

        ret = (*a == *b);
    }

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Counted string
//
//
////////////////////////////////////////////////////////////////

static yo_string_t  yo_from_cstring(char *cstring)
{
    yo_string_t ret =
    {
        .data   = (uint8_t *)cstring,
        .length = strlen(cstring), // TODO(rune): strlen
    };

    return ret;
}

static bool yo_string_equal(yo_string_t a, yo_string_t b)
{
    bool ret = false;

    if (a.length == b.length)
    {
        if (a.length)
        {
            ret = yo_memequ(a.data, b.data, a.length);
        }
        else
        {
            ret = true;
        }
    }

    return ret;
}

static size_t yo_index_of_char(yo_string_t s, uint8_t c)
{
    size_t ret = YO_NOT_FOUND;

    uint8_t *found = memchr(s.data, c, s.length);
    if (found)
    {
        ret = found - s.data;
    }

    return ret;
}

static size_t yo_index_of_string(yo_string_t a, yo_string_t b)
{
    size_t ret = YO_NOT_FOUND;

    if (b.length == 0)
    {
        ret = 0;
    }
    else if (a.length >= b.length)
    {
        char first = b.data[0];
        char last  = b.data[b.length - 1];

        for (size_t i = 0; i <= a.length - b.length; i++)
        {
            if ((a.data[i] == first) && (a.data[i + b.length - 1] == last))
            {
                if (yo_memequ(a.data + i, b.data, b.length))
                {
                    ret = i;
                    break;
                }
            }
        }
    }

    return ret;
}

static yo_string_t yo_substring(yo_string_t s, size_t index)
{
    yo_string_t ret = YO_EMPTY_STRING;

    if (s.length > index)
    {
        ret.data  = s.data + index;
        ret.length = s.length - index;
    }

    return ret;
}

static yo_string_t yo_substring_len(yo_string_t s, size_t index, size_t length)
{
    yo_string_t ret = YO_EMPTY_STRING;

    if (s.length >= index + length)
    {
        ret.data  = s.data + index;
        ret.length = length;
    }

    return ret;
}

static yo_string_t yo_split_pop_first(yo_string_t *a, yo_string_t b)
{
    yo_string_t ret = YO_EMPTY_STRING;

    size_t index = yo_index_of_string(*a, b);
    if (index != YO_NOT_FOUND)
    {
        ret    = yo_substring_len(*a, 0, index);
        *a     = yo_substring(*a, index + b.length);
    }

    return ret;
}

static bool yo_is_whitespace_c(char c)
{
    bool ret = false;

    ret |= (c == ' ');
    ret |= (c == '\n');
    ret |= (c == '\r');
    ret |= (c == '\t');

    return ret;
}

static bool yo_is_alpha_c(char c)
{
    bool ret = false;

    ret |= (c >= 'A' && c <= 'Z');
    ret |= (c >= 'a' && c <= 'z');

    return ret;
}

static bool yo_is_digit_c(char c)
{
    bool ret = false;

    ret |= (c >= '0' && c <= '9');

    return ret;
}

static bool yo_is_word_c(char c)
{
    bool ret = false;

    ret |= yo_is_alpha_c(c);
    ret |= (c == '_');

    return ret;
}

static char yo_to_upper_c(char c)
{
    if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
    return c;
}

static char yo_to_lower_c(char c)
{
    if (c >= 'A' && c <= 'Z') c += ('a' - 'A');
    return c;

}

static yo_string_t  yo_trim(yo_string_t s)
{
    s = yo_trim_start(s);
    s = yo_trim_end(s);

    return s;
}

static yo_string_t  yo_trim_start(yo_string_t s)
{
    while (s.length && yo_is_whitespace_c(s.data[0]))
    {
        s.data++;
        s.length--;
    }

    return s;
}

static yo_string_t  yo_trim_end(yo_string_t s)
{
    while (s.length && yo_is_whitespace_c(s.data[s.length - 1]))
    {
        s.length--;
    }

    return s;
}

static bool yo_starts_with(yo_string_t s, yo_string_t prefix)
{
    size_t index = yo_index_of_string(s, prefix);
    bool ret = index == 0;
    return ret;
}

static bool yo_ends_with(yo_string_t s, yo_string_t suffix)
{
    // TODO(rune): Optimize -> no need to search through the whole string.

    size_t index = yo_index_of_string(s, suffix);
    bool ret = index == s.length - suffix.length;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Unicode
//
//
////////////////////////////////////////////////////////////////

static uint32_t yo_utf8_codepoint_advance(yo_string_t *s)
{
    YO_PROFILE_BEGIN(yo_utf8_codepoint_advance);

    // https://en.wikipedia.org/wiki/UTF-8#Encoding

    // NOTE(rune): Table generated with:
#if 0
#include <stdio.h>
    int main()
    {
        printf("static const uint8_t decode_table[256] =\n{");

        for (int i = 0; i < 256; i++)
        {
            if (i % 32 == 0) printf("\n    ");

            if ((i >> 3) == 0b11110)      printf("4, "); // 11110xxx
            else if ((i >> 4) == 0b1110)  printf("3, "); // 1110xxxx
            else if ((i >> 5) == 0b110)   printf("2, "); // 111xxxxx
            else if ((i >> 7) == 0)       printf("1, "); // 0xxxxxxx
            else                          printf("0, "); // continuation
        }

        printf("\n};\n");
    }
#endif

    static const uint8_t table[256] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    uint32_t ret = 0;

    //
    // 1-byte encoding
    //

    if ((s->length >= 1) &&
        (table[s->data[0]] == 1))
    {
        ret = s->data[0];

        *s = yo_substring(*s, 1);
    }

    //
    // 2-byte encoding
    //

    if ((s->length >= 2) &&
        (table[s->data[0]] == 2) &&
        (table[s->data[1]] == 0))
    {
        ret = (((s->data[0] & 0b0001'1111) << 6) |
               ((s->data[1] & 0b0011'1111) << 0));

        *s = yo_substring(*s, 2);
    }

    //
    // 3-byte encoding
    //

    if ((s->length >= 3) &&
        (table[s->data[0]] == 3) &&
        (table[s->data[1]] == 0) &&
        (table[s->data[2]] == 0))
    {
        ret = (((s->data[0] & 0b0000'1111) << 12) |
               ((s->data[1] & 0b0011'1111) <<  6) |
               ((s->data[2] & 0b0011'1111) <<  0));

        *s = yo_substring(*s, 3);
    }

    //
    // 4-byte encoding
    //

    if ((s->length >= 4) &&
        (table[s->data[0]] == 4) &&
        (table[s->data[1]] == 0) &&
        (table[s->data[2]] == 0) &&
        (table[s->data[3]] == 0))
    {
        ret = (((s->data[0] & 0b0000'0111) << 18) |
               ((s->data[1] & 0b0011'1111) << 12) |
               ((s->data[2] & 0b0011'1111) <<  6) |
               ((s->data[3] & 0b0011'1111) <<  0));

        *s = yo_substring(*s, 4);
    }

    YO_PROFILE_END(yo_utf8_codepoint_advance);

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// String builder
//
//
////////////////////////////////////////////////////////////////

static bool yo_builder_create(yo_builder_t *builder, size_t initial_size)
{
    builder->length = 0;
    builder->ok = true;
    bool ret = yo_arena_create(&builder->arena, initial_size, false, YO_ARENA_TYPE_CHAIN_EXPONENTIAL);
    return ret;
}

static void yo_builder_destroy(yo_builder_t *builder)
{
    yo_arena_destroy(&builder->arena);
    builder->length = 0;
    builder->ok = false;
}

static void yo_builder_reset(yo_builder_t *builder)
{
    yo_arena_reset(&builder->arena);
    builder->length = 0;
    builder->ok = true;
}

static void yo_builder_append(yo_builder_t *builder, yo_string_t s)
{
    if (builder->ok && s.length)
    {
        void *ptr = yo_arena_push_size(&builder->arena, s.length, false);
        if (ptr)
        {
            yo_memcpy(ptr, s.data, s.length);
            builder->length += s.length;
        }
        else
        {
            builder->ok = false;
        }
    }
}

static void yo_builder_append_c(yo_builder_t *builder, char c)
{
    yo_string_t s = { .data = (uint8_t *)&c, .length = 1 };
    yo_builder_append(builder, s);
}

static void yo_builder_append_f(yo_builder_t *builder, char *format, ...)
{
    if (builder->ok)
    {
        va_list args;
        va_start(args, format);
        yo_builder_append_v(builder, format, args);
        va_end(args);
    }
}

static void yo_builder_append_v(yo_builder_t *builder, char *format, va_list args)
{
    if (builder->ok)
    {
        size_t length = vsnprintf(NULL, 0, format, args);

        // NOTE(rune): vsnprintf must have space enough to add a \0, even though we don't need it.
        void *ptr = yo_arena_push_size_ensure_pad(&builder->arena, length, false, 1);

        if (ptr)
        {
            vsnprintf(ptr, length + 1, format, args);
            builder->length += length;
        }
        else
        {
            builder->ok = false;
        }
    }
}

static yo_string_t yo_builder_to_string(yo_builder_t *builder, yo_arena_t *dest_arena)
{
    yo_string_t ret = YO_EMPTY_STRING;

    if (builder->ok)
    {
        uint8_t *ptr = yo_arena_push_size(dest_arena, builder->length + 1, false);
        if (ptr)
        {
            size_t current_length = 0;

            for (yo_arena_block_t *block = &builder->arena.self; block; block = block->next)
            {
                uint8_t *src      = block->base;
                size_t   src_size = block->size_used;

                // NOTE(rune): Chained blocks have a yo_arena_block_t header.
                if (block->prev)
                {
                    src      += sizeof(yo_arena_block_t);
                    src_size -= sizeof(yo_arena_block_t);
                }

                YO_ASSERT(current_length + src_size <= builder->length);
                yo_memcpy(ptr + current_length, src, src_size);
                current_length += src_size;
            }

            YO_ASSERT(current_length == builder->length);

            ret.data  = ptr;
            ret.length = current_length;
            ptr[current_length] = 0;
        }
    }

    return ret;
}

static yo_string_t  yo_builder_to_string_and_destroy(yo_builder_t *builder, yo_arena_t *arena)
{
    yo_string_t ret = yo_builder_to_string(builder, arena);
    yo_builder_destroy(builder);
    return ret;
}

static yo_string_t  yo_builder_to_string_and_reset(yo_builder_t *builder, yo_arena_t *arena)
{
    yo_string_t ret = yo_builder_to_string(builder, arena);
    yo_builder_reset(builder);
    return ret;
}

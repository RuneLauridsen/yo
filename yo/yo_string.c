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
// Known length string
//
//
////////////////////////////////////////////////////////////////

static yo_string_t  yo_from_cstring(char *cstring)
{
    yo_string_t ret =
    {
        .chars = (uint8_t *)cstring,
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
            ret = yo_memequ(a.chars, b.chars, a.length);
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

    uint8_t *found = memchr(s.chars, c, s.length);
    if (found)
    {
        ret = found - s.chars;
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
        char first = b.chars[0];
        char last = b.chars[b.length - 1];

        for (size_t i = 0; i < a.length - b.length; i++)
        {
            if ((a.chars[i] == first) && (a.chars[i + b.length - 1] == last))
            {
                if (yo_memequ(a.chars + i, b.chars, b.length))
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
        ret.chars  = s.chars + index;
        ret.length = s.length - index;
    }

    return ret;
}

static yo_string_t yo_substringl(yo_string_t s, size_t index, size_t length)
{
    yo_string_t ret = YO_EMPTY_STRING;

    if (s.length >= index + length)
    {
        ret.chars  = s.chars + index;
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
        ret = yo_substringl(*a, 0, index);
        *a     = yo_substring(*a, index + b.length);
    }

    return ret;
}

static uint32_t yo_utf8_codepoint_advance(yo_string_t *s)
{
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
        (table[s->chars[0]] == 1))
    {
        ret = s->chars[0];

        *s = yo_substring(*s, 1);
    }

    //
    // 2-byte encoding
    //

    if ((s->length >= 2) &&
        (table[s->chars[0]] == 2) &&
        (table[s->chars[1]] == 0))
    {
        ret = (((s->chars[0] & 0b0001'1111) << 6) |
               ((s->chars[1] & 0b0011'1111) << 0));

        *s = yo_substring(*s, 2);
    }

    //
    // 3-byte encoding
    //

    if ((s->length >= 3) &&
        (table[s->chars[0]] == 3) &&
        (table[s->chars[1]] == 0) &&
        (table[s->chars[2]] == 0))
    {
        ret = (((s->chars[0] & 0b0000'1111) << 12) |
               ((s->chars[1] & 0b0011'1111) <<  6) |
               ((s->chars[2] & 0b0011'1111) <<  0));

        *s = yo_substring(*s, 3);
    }

    //
    // 4-byte encoding
    //

    if ((s->length >= 4) &&
        (table[s->chars[0]] == 4) &&
        (table[s->chars[1]] == 0) &&
        (table[s->chars[2]] == 0) &&
        (table[s->chars[3]] == 0))
    {
        ret = (((s->chars[0] & 0b0000'0111) << 18) |
               ((s->chars[1] & 0b0011'1111) << 12) |
               ((s->chars[2] & 0b0011'1111) <<  6) |
               ((s->chars[3] & 0b0011'1111) <<  0));

        *s = yo_substring(*s, 4);
    }

    return ret;
}
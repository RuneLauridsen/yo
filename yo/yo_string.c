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

static size_t yo_index_of_char(yo_string_t s, char c)
{
    size_t ret = YO_NOT_FOUND;

    char *found = memchr(s.chars, c, s.length);
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

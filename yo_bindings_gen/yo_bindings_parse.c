#pragma once

// WARNING(rune): This is far from a robust C-parser, and should no be used on anything other than yo_xyz headers!
// TODO(rune): Is it worth it to tokenize separately?

static yo_token_t *yo_parse_consume(yo_parse_t *p, yo_token_type_t type)
{
    YO_ASSERT(p->token->type == type);
    yo_token_t *ret = p->token;
    p->token = p->token->next;
    return ret;
}

static void yo_parse_consume_keyword(yo_parse_t *p, yo_string_t s)
{
    yo_token_t *token = yo_parse_consume(p, YO_TOKEN_IDEN);
    yo_string_t text = token->text;
    YO_ASSERT(yo_string_equal(text, s));
}

static yo_string_t yo_parse_consume_name(yo_parse_t *p)
{
    yo_token_t *token = yo_parse_consume(p, YO_TOKEN_IDEN);
    yo_string_t ret = token->text;
    return ret;
}

static yo_type_spec_t yo_parse_type(yo_parse_t *p)
{
    yo_type_spec_t ret = { 0 };

    if (yo_string_equal(p->token->text, yo_string("YO_PRINTF_FORMAT_STRING")))
    {
        p->token = p->token->next;
    }

    ret.name = yo_parse_consume_name(p);

    while (p->token->type == '*')
    {
        ret.indirection++;
        yo_parse_consume(p, '*');
    }

    return ret;
}

static yo_arg_spec_t yo_parse_arg(yo_parse_t *p)
{
    yo_arg_spec_t ret = { 0 };

    // TODO(rune): This is more of a lexer thing?

    if (p->token->type             == '.' &&
        p->token->next->type       == '.' &&
        p->token->next->next->type == '.')
    {
        yo_parse_consume(p, '.');
        yo_parse_consume(p, '.');
        yo_parse_consume(p, '.');
        ret.is_va_arg = true;
    }
    else
    {
        ret.type = yo_parse_type(p);

        if (p->token->type == YO_TOKEN_IDEN)
        {
            ret.name = yo_parse_consume_name(p);
        }
    }

    if (p->token->type == '[')
    {
        ret.invalid = true;

        while (p->token->type != ']')
        {
            p->token = p->token->next;
        }

        yo_parse_consume(p, ']');
    }

    if (p->token->type == ',')
    {
        yo_parse_consume(p, ',');
    }

    return ret;
}

static yo_func_spec_t yo_parse_func(yo_parse_t *p)
{
    yo_func_spec_t ret = { 0 };

    ret.return_type = yo_parse_type(p);
    ret.name        = yo_parse_consume_name(p);

    yo_parse_consume(p, '(');

    while (p->token->type != ')')
    {
        yo_arg_spec_t *arg = yo_arena_push_struct(p->arena, yo_arg_spec_t, false);
        *arg = yo_parse_arg(p);

        if (!arg->invalid)
        {
            yo_slist_queue_push(&ret.args, arg);
        }
        else
        {
            ret.invalid = true;
        }
    }

    yo_parse_consume(p, ')');


    // NOTE(rune): Checks for 0-argument functions, e.g. my_function(void);
    if ((ret.args.first) &&
        (ret.args.first == ret.args.last) &&
        (ret.args.first->name.length == 0) &&
        (yo_string_equal(ret.args.first->type.name, yo_string("void"))))
    {
        ret.args.first = NULL;
        ret.args.last  = NULL;
    }


    return ret;
}

static yo_enum_spec_t yo_parse_enum(yo_parse_t *p)
{
    //
    // HACK(rune): In yo_xyz headers enums are declared like this:
    //
    //     typedef uint32_t yo_enum_name_t;
    //     enum yo_enum_name
    //     { ... }
    //
    // So the fist thing we do, is reset the parsers state to the typedef.
    //

    p->token = p->token->prev; // ;
    p->token = p->token->prev; // yo_enum_name_t
    p->token = p->token->prev; // uint32_t
    p->token = p->token->prev; // typedef

    yo_enum_spec_t ret = { 0 };

    yo_parse_consume_keyword(p, yo_string("typedef"));

    ret.type = yo_parse_type(p);
    ret.name = yo_parse_consume_name(p);

    yo_parse_consume(p, ';');
    yo_parse_consume_keyword(p, yo_string("enum"));
    yo_parse_consume_name(p);
    yo_parse_consume(p, '{');

    while (p->token->type != '}')
    {
        yo_enum_member_spec_t *member = yo_arena_push_struct(p->arena, yo_enum_member_spec_t, true);
        member->name = yo_parse_consume_name(p);

        // NOTE(rune): Just store the whole expression in a string.
        member->expression.data   = p->token->text.data;
        member->expression.length = 0;
        while ((p->token->type != ',') && (p->token->type != '}'))
        {
            member->expression.length = p->token->text.data + p->token->text.length - member->expression.data;
            p->token = p->token->next;
        }

        // NOTE(rune): Optional trailing comma
        if (p->token->type == ',') yo_parse_consume(p, ',');


        yo_slist_add(&ret.members, member);
    }

    yo_parse_consume(p, '}');
    yo_parse_consume(p, ';');

    return ret;
}

static yo_api_spec_t yo_parse(yo_token_t *tokens, yo_arena_t *arena)
{
    yo_parse_t p = { .token = tokens, arena = arena };

    while (p.token->type != YO_TOKEN_EOF)
    {
        if ((yo_string_equal(p.token->text, yo_string("YO_API"))) ||
            (yo_string_equal(p.token->text, yo_string("YO_PLATFORM_API"))))
        {
            yo_parse_consume(&p, YO_TOKEN_IDEN);

            yo_func_spec_t *func = yo_arena_push_struct(p.arena, yo_func_spec_t, false);
            *func = yo_parse_func(&p);
            yo_slist_add(&p.result.funcs, func);
        }
        else if (yo_string_equal(p.token->text, yo_string("enum")))
        {
            yo_enum_spec_t *enum_ = yo_arena_push_struct(p.arena, yo_enum_spec_t, false);
            *enum_ = yo_parse_enum(&p);
            yo_slist_add(&p.result.enums, enum_);
        }
        else
        {
            p.token = p.token->next;
        }
    }

    return p.result;
}

#pragma once

static yo_string_t yo_strip_yo_prefix(yo_string_t s)
{
    if ((yo_starts_with(s, yo_string("yo_"))) ||
        (yo_starts_with(s, yo_string("YO_"))))
    {
        s = yo_substring(s, 2);
    }

    return s;
}

static void yo_builder_append_camel_case(yo_builder_t *builder, yo_string_t s, bool upper_first)
{
    bool next_upper = upper_first;

    while (s.length)
    {
        char c = s.data[0];

        if (next_upper) { c = yo_to_upper_c(c); }

        if (c == '_')
        {
            next_upper = true;
        }
        else
        {
            next_upper = false;
            yo_builder_append_c(builder, c);
        }

        s.data++;
        s.length--;
    }
}

static void yo_builder_append_csharp_type(yo_builder_t *builder, yo_parsed_type_t type)
{
    if (yo_string_equal(type.name, yo_string("char")) && type.indirection == 1)
    {
        yo_builder_append(builder, yo_string("string"));
    }
    else if (yo_string_equal(type.name, yo_string("va_list")) && type.indirection == 0)
    {
        yo_builder_append(builder, yo_string("nint"));
    }
    else
    {
        yo_builder_append(builder, type.name);
        for (uint32_t i = 0; i < type.indirection; i++)
        {
            yo_builder_append_c(builder, '*');
        }
    }

    yo_builder_append_c(builder, ' ');
}

static bool yo_ignore_name(yo_string_t s)
{
    yo_string_t skip_names[] =
    {
        yo_string("yo_slider_ex"), // NOTE(rune): We don't have the yo_slider_style_t struct in C# yet.
    };

    bool ret = false;

    for (uint32_t i = 0; i < countof(skip_names); i++)
    {
        if (yo_string_equal(s, skip_names[i]))
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static yo_string_t yo_get_bindings_for_csharp(yo_parsed_header_t header, yo_arena_t *arena)
{
    yo_builder_t b;
    yo_builder_create(&b, YO_MEGABYTES(1));

    //
    // Enums
    //

    for (yo_parsed_enum_t *enum_ = header.enums.first;
         enum_;
         enum_ = enum_->next)
    {
        if (!yo_ignore_name(enum_->name))
        {
            yo_builder_append(&b, yo_string("public enum "));
            yo_builder_append(&b, enum_->name);
            yo_builder_append(&b, yo_string(" : "));
            yo_builder_append(&b, enum_->type.name);
            yo_builder_append(&b, yo_string("\n{\n"));

            for (yo_parsed_enum_member_t *member = enum_->members.first;
                 member;
                 member = member->next)
            {
                yo_builder_append(&b, yo_string("    "));
                yo_builder_append(&b, member->name);
                yo_builder_append(&b, member->expression);
                yo_builder_append(&b, yo_string(",\n"));
            }

            yo_builder_append(&b, yo_string("}\n\n"));
        }
    }

    //
    // Functions
    //

    for (yo_parsed_func_t *func = header.funcs.first;
         func;
         func = func->next)
    {
        if (!yo_ignore_name(func->name))
        {
            yo_builder_append(&b, yo_string("[DllImport(\"yo_dll.dll\")] public static extern "));
            yo_builder_append_csharp_type(&b, func->return_type);
            yo_builder_append(&b, func->name);
            yo_builder_append_c(&b, '(');

            for (yo_parsed_arg_t *arg = func->args.first;
                 arg;
                 arg = arg->next)
            {
                if (arg->is_va_arg)
                {
                    yo_builder_append(&b, yo_string("__arglist"));
                }
                else
                {
                    yo_builder_append_csharp_type(&b, arg->type);
                    if (yo_string_equal(arg->name, yo_string("string"))) yo_builder_append_c(&b, '@');
                    yo_builder_append(&b, arg->name);
                }

                if (arg->next) yo_builder_append(&b, yo_string(", "));
            }

            yo_builder_append_c(&b, ')');
            yo_builder_append_c(&b, ';');
            yo_builder_append_c(&b, '\n');
        }
    }

    yo_string_t ret = yo_builder_to_string_and_destroy(&b, arena);
    return ret;

}
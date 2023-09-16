#pragma once

////////////////////////////////////////////////////////////////
//
//
// Structs
//
//
////////////////////////////////////////////////////////////////

typedef uint32_t yo_widget_layout_t;
enum yo_widget_layout
{
    YO_W_LAYOUT_NONE,
    YO_W_LAYOUT_CHILDREN_X,
    YO_W_LAYOUT_CHILDREN_Y,
};

typedef struct yo_widget yo_widget_t;
struct yo_widget
{
    yo_widget_t *parent;
    yo_widget_t *next;
    yo_slist(yo_widget_t) children;

    union
    {
        struct { yo_length_t length_x, length_y; };
        struct { yo_length_t length_a[2]; };
    };

    float spec;
    float volume;
    float padding;

    yo_widget_layout_t layout;
    yo_v4f_t color;
    yo_v2f_t final_dim;
    yo_v2f_t final_p0; // NOTE(rune): Relative to parent
    yo_v2f_t final_p1; // NOTE(rune): Relative to parent
};

typedef yo_widget_t *yo_widget_ptr_t;

YO_TYPEDEF_ARRAY(yo_widget_ptr_t);

static struct
{
    yo_arena_t arena;
    yo_array(yo_widget_ptr_t) parent_stack;
    yo_widget_t *latest;
    yo_widget_t *root;


    bool initialized;
} state;

////////////////////////////////////////////////////////////////
//
//
// Draw
//
//
////////////////////////////////////////////////////////////////

static void yo_exp_draw_rect(yo_v2f_t p0, yo_v2f_t p1, yo_v4f_t color)
{
    yo_box(0, 0);
    yo_set_fill(color);
    yo_set_align_h(YO_ALIGN_LEFT);
    yo_set_align_v(YO_ALIGN_TOP);
    yo_set_margin_left(p0.x);
    yo_set_margin_top(p0.y);
    yo_set_dim_h(yo_px(p1.x - p0.x));
    yo_set_dim_v(yo_px(p1.y - p0.y));
}

static void yo_exp_draw_final_recurse(yo_widget_t *w, yo_v2f_t origin)
{
    yo_exp_draw_rect(yo_v2f((w->final_p0.x + origin.x), (w->final_p0.y + origin.y)),
                     yo_v2f((w->final_p1.x + origin.x), (w->final_p1.y + origin.y)),
                     w->color);

    origin.x += w->final_p0.x;
    origin.y += w->final_p0.y;

    for (yo_slist_each(yo_widget_t *, it, w->children.first))
    {
        yo_exp_draw_final_recurse(it, origin);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Hierarchy
//
//
////////////////////////////////////////////////////////////////

static void yo_exp_begin_children()
{
    yo_array_put(&state.parent_stack, state.latest);
}

static void yo_exp_end_children()
{
    state.latest = yo_array_last(&state.parent_stack);
    yo_array_pop(&state.parent_stack, 1);
}

#define YO_LENGTH_AUTO ((yo_length_t) { .min = 0, .max = YO_FLOAT32_MAX })

static yo_widget_t *yo_exp_new2(yo_v4f_t color, float volume, float padding, yo_widget_layout_t layout, yo_length_t length_x, yo_length_t length_y)
{
    yo_widget_t *ret    = yo_arena_push_struct(&state.arena, yo_widget_t, true);
    yo_widget_t *parent = yo_array_last_or_default(&state.parent_stack, NULL);


    if (parent)
    {
        yo_slist_add(&parent->children, ret);
        ret->parent = parent;
    }
    else
    {
        YO_ASSERT(state.root == NULL);
        state.root = ret;
    }

    state.latest = ret;

    ret->length_x  = length_x;
    ret->length_y  = length_y;
    ret->layout    = layout;
    ret->padding   = padding;
    ret->color     = color;
    ret->volume    = volume;

    return ret;
}

static yo_widget_t *yo_exp_new(yo_v4f_t color, float volume, float padding, yo_widget_layout_t layout)
{
    yo_length_t length = { .min = 0, .max = YO_FLOAT32_MAX, .rel = 0, .is_rel = false };
    yo_widget_t *ret = yo_exp_new2(color, volume, padding, layout, length, length);
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Print
//
//
////////////////////////////////////////////////////////////////

static void yo_exp_print_recurse(yo_widget_t *w, uint32_t depth)
{
    for (uint32_t i = 0; i < depth; i++) printf("--- ");

    printf("w rgb(%f, %f, %f)\n", w->color.r, w->color.g, w->color.b);

    for (yo_slist_each(yo_widget_t *, it, w->children.first))
    {
        yo_exp_print_recurse(it, depth + 1);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Layout calculations
//
//
////////////////////////////////////////////////////////////////

static yo_v2f_t yo_exp_dim_from_constraint(yo_widget_t *w, yo_v2f_t avail_min, yo_v2f_t avail_max)
{
    yo_v2f_t ret = { 0 };

    avail_min.x -= w->padding * 2;
    avail_min.y -= w->padding * 2;
    avail_max.x -= w->padding * 2;
    avail_max.y -= w->padding * 2;

    if (avail_max.x == 0)
        __nop();

    yo_v2f_t length_max = yo_v2f(YO_MIN(w->length_x.max, avail_max.x),
                                 YO_MIN(w->length_y.max, avail_max.y));

    switch (w->layout)
    {
        case YO_W_LAYOUT_NONE:
        {
            YO_ASSERT(w->children.first == NULL);
            YO_ASSERT(w->children.last  == NULL);

            // TODO(rune): Choose x/y based on constraints instead

            if (w->parent->layout == YO_W_LAYOUT_CHILDREN_X)
            {
                ret.y = length_max.y;
                ret.x = w->volume / ret.y;
            }
            else
            {
                YO_ASSERT(w->parent->layout == YO_W_LAYOUT_CHILDREN_Y);

                ret.x = length_max.x;
                ret.y = w->volume / ret.x;
            }


        } break;

        // TODO(rune): Combine into a single axis generic case.

        case YO_W_LAYOUT_CHILDREN_X:
        {
            for (yo_slist_each(yo_widget_t *, it, w->children.first))
            {
                it->final_dim = yo_exp_dim_from_constraint(it, avail_min, avail_max);

                avail_min.x -= it->final_dim.x;
                avail_max.x -= it->final_dim.x;

                ret.x += it->final_dim.x;
                ret.y = YO_MAX(ret.y, it->final_dim.y);
            }

            float x = 0.0f;
            float y = 0.0f;

            for (yo_slist_each(yo_widget_t *, it, w->children.first))
            {
                it->final_p0 = yo_v2f(w->padding + x,
                                      w->padding + y);

                it->final_p1 = yo_v2f(w->padding + x + it->final_dim.x,
                                      w->padding + y + it->final_dim.y);

                x += it->final_dim.x;
            }

        } break;

        case YO_W_LAYOUT_CHILDREN_Y:
        {
            for (yo_slist_each(yo_widget_t *, it, w->children.first))
            {
                it->final_dim = yo_exp_dim_from_constraint(it, avail_min, avail_max);

                avail_min.y -= it->final_dim.y;
                avail_max.y -= it->final_dim.y;

                ret.y += it->final_dim.y;
                ret.x = YO_MAX(ret.x, it->final_dim.x);
            }

            float x = 0.0f;
            float y = 0.0f;

            for (yo_slist_each(yo_widget_t *, it, w->children.first))
            {
                it->final_p0 = yo_v2f(w->padding + x,
                                      w->padding + y);

                it->final_p1 = yo_v2f(w->padding + x + it->final_dim.x,
                                      w->padding + y + it->final_dim.y);

                y += it->final_dim.y;
            }

        } break;

        default:
        {
            YO_ASSERT(!"Invalid layout.");
        } break;
    }

    ret.x += w->padding * 2;
    ret.y += w->padding * 2;

    ret.x = YO_MIN(ret.x, length_max.x);
    ret.y = YO_MIN(ret.y, length_max.y);

    return ret;
}

static void yo_exp_layout_recurse(yo_widget_t *w, yo_v2f_t avail_min, yo_v2f_t avail_max)
{
    w->final_dim = yo_exp_dim_from_constraint(w, avail_min, avail_max);
}

////////////////////////////////////////////////////////////////
//
//
// Builder
//
//
////////////////////////////////////////////////////////////////

static void yo_exp(yo_v2i_t dim)
{
    // NOTE(rune): First time initialization
    if (!state.initialized)
    {
        yo_zero_struct(&state);
        yo_array_create(&state.parent_stack, 256, true);
        yo_arena_create(&state.arena, 0, 0, 0);
        state.initialized = true;
    }

    dim.x = 300;
    dim.y = 80;

    yo_array_reset(&state.parent_stack, true);
    yo_arena_reset(&state.arena);
    state.root   = NULL;
    state.latest = NULL;

    yo_exp_new(yo_rgb(40, 40, 40), -1, 5, YO_W_LAYOUT_CHILDREN_Y);
    yo_exp_begin_children();
    {
        yo_exp_new2(YO_RED, 100*100, 0, 0, YO_LENGTH_AUTO, YO_LENGTH_AUTO);

        yo_exp_new(yo_rgb(100, 100, 100), -1, 5, YO_W_LAYOUT_CHILDREN_X);
#if 1
        yo_exp_begin_children();
        {
            yo_exp_new2(YO_BLUE, 80*80, 0, 0, yo_px(120), YO_LENGTH_AUTO);
            yo_exp_new2(YO_CYAN, 80*80, 0, 0, YO_LENGTH_AUTO, YO_LENGTH_AUTO);
        }
        yo_exp_end_children();
#endif
        yo_exp_new2(YO_ORANGE, 100*100, 0, 0, yo_px(100), YO_LENGTH_AUTO);
    }
    yo_exp_end_children();


    //yo_exp_print_recurse(state.root, 0);

    yo_v2f_t avail_min = yo_v2f(80, 30);
    yo_v2f_t avail_max = yo_v2f(300, 400);

    state.root->final_p0 = yo_v2f(0, 0);
    state.root->final_p1 = yo_v2f(avail_max.x, avail_max.y);
    yo_exp_layout_recurse(state.root, avail_min, avail_max);

    yo_exp_draw_final_recurse(state.root, yo_v2f(0, 0));
}

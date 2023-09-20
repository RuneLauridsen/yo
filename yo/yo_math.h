#pragma once

////////////////////////////////////////////////////////////////
//
//
// Misc macros
//
//
////////////////////////////////////////////////////////////////

#define YO_MAX(a,b)                ((a)>(b)?(a):(b))
#define YO_MIN(a,b)                ((a)<(b)?(a):(b))

#define YO_CLAMP(x,l,h)            YO_MIN(YO_MAX((x),(l)),(h))
#define YO_CLAMP_LOW(x,l)          YO_MAX((x),(l))
#define YO_CLAMP_HIGH(x,h)         YO_MIN((x),(h))
#define YO_CLAMP01(x)              YO_CLAMP(x, 0, 1)

////////////////////////////////////////////////////////////////
//
//
// Constructor macros
//
//
////////////////////////////////////////////////////////////////

#define yo_v2i(x,y)         ((yo_v2i_t){x,y})
#define yo_v2f(x,y)         ((yo_v2f_t){x,y})
#define yo_v3f(x,y,z)       ((yo_v3f_t){x,y,z})
#define yo_v4f(x,y,z,w)     ((yo_v4f_t){x,y,z,w})
#define yo_recti(x,y,w,h)   ((yo_recti_t){x,y,w,h})
#if 0
#define yo_rectf(p0,p1)     ((yo_rectf_t){p0,p1})
#else
#define yo_rectf(x,y,w,h)   ((yo_rectf_t){x,y,w,h})
#endif
#define yo_rgba(r,g,b,a)    ((yo_v4f_t){(float)(r)/255.0f, (float)(g)/255.0f, (float)(b)/255.0f, (float)(a)/255.0f})
#define yo_rgba32(r,g,b,a)  ((((uint32_t)(r) << 0)) | (((uint32_t)(g) << 8)) | (((uint32_t)(b) << 16)) | (((uint32_t)(a) << 24)))

#define yo_rgb(r,g,b)       yo_rgba(r,g,b,255)
#define yo_argb(a,r,g,b)    yo_rgba(r,g,b,a)
#define yo_rgb32(r,g,b)     yo_rgba32(r,g,b,255)
#define yo_argb32(a,r,g,b)  yo_rgba32(r,g,b,a)

////////////////////////////////////////////////////////////////
//
//
// Scalars
//
//
////////////////////////////////////////////////////////////////

#if 0
typedef int8_t   int8_t;
typedef int16_t  int16_t;
typedef int32_t  int32_t;
typedef int64_t  int64_t;
typedef uint8_t  uint8_t;
typedef uint16_t uint16_t;
typedef uint32_t uint32_t;
typedef uint64_t uint64_t;
typedef size_t   size_t;
typedef float    float;
typedef double   double;
#endif

////////////////////////////////////////////////////////////////
//
//
// Vectors
//
//
////////////////////////////////////////////////////////////////

typedef union yo_v2f yo_v2f_t;
union yo_v2f
{
    struct { float x, y; };
    struct { float axis[2]; };
    struct { float v[2]; };
};

typedef union yo_v3f yo_v3f_t;
union yo_v3f
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    struct { float axis[3]; };
    struct { float v[3]; };
};

typedef union yo_v4f yo_v4f_t;
union yo_v4f
{
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
    struct { float v[4]; };
    struct { float rgba[4]; };
};

typedef union yo_v2i yo_v2i_t;
union yo_v2i
{
    struct { int32_t x, y; };
    struct { int32_t w, h; }; // TODO(rune): Remove
    struct { int32_t dim_x, dim_y; };
    struct { int32_t axis[2]; };
    struct { int32_t v[2]; };
};

static bool yo_v2i_equal(yo_v2i_t a, yo_v2i_t b)
{
    bool ret = (a.x == b.x) && (a.y == b.y);
    return ret;
}

static bool yo_v2f_equal(yo_v2f_t a, yo_v2f_t b)
{
    bool ret = (a.x == b.x) && (a.y == b.y);
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Rectangles
//
//
////////////////////////////////////////////////////////////////

// TODO(rune): Should use (p0, p1) instead like rectf
typedef union yo_recti yo_recti_t;
typedef union yo_recti yo_recti;
union yo_recti
{
    struct
    {
        int32_t x, y;
        uint32_t w, h;
    };
    struct
    {
        int32_t left, top;
        uint32_t dim_x, dim_y;
    };
    struct
    {
        int32_t pos[2];
        uint32_t dim_a[2];
    };
};

typedef union yo_rectf yo_rectf_t;
typedef union yo_rectf yo_rectf;
union yo_rectf
{
#if 0
    struct { yo_v2f_t p0, p1; };
    struct { yo_v2f_t p[2]; };
#else
    struct
    {
        float x, y;
        float w, h;
    };
    struct
    {
        float left, top;
        float dim_x, dim_y;
    };
    struct
    {
        float pos[2];
        float dim_a[2];
    };
#endif
};

// TODO(rune): Completely switch over to yo_rectf2_t
typedef union yo_rectf2 yo_rectf2_t;
union yo_rectf2
{
    struct { yo_v2f_t p0, p1; };
    struct { yo_v2f_t p[2]; };
    struct { float x0, y0, x1, y1; };
    struct { float left, top, right, bottom; };
};

static inline bool yo_clips_rect(yo_recti_t rect, yo_v2i_t point)
{
    bool ret = ((point.x >= rect.x) &&
                (point.x < (rect.x + (int32_t)rect.w)) &&
                (point.y >= rect.y) &&
                (point.y < (rect.y + (int32_t)rect.h)));

    return ret;
}

static inline bool yo_clips_rectf(yo_rectf_t rect, yo_v2f_t point)
{
    bool ret = ((point.x >= rect.x) &&
                (point.x < (rect.x + rect.w)) &&
                (point.y >= rect.y) &&
                (point.y < (rect.y + rect.h)));

    return ret;
}

static inline bool yo_recti_equal(yo_recti_t a, yo_recti_t b)
{
    if ((a.x == b.x) &&
        (a.y == b.y) &&
        (a.w == b.w) &&
        (a.h == b.h))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static inline int32_t yo_recti_width(yo_recti rect)
{
    int32_t ret = rect.dim_x;
    return ret;
}

static inline int32_t yo_recti_height(yo_recti rect)
{
    int32_t ret = rect.dim_y;
    return ret;
}

// TODO(rune): yo_recti_intersection

////////////////////////////////////////////////////////////////
//
//
// Floating point
//
//
////////////////////////////////////////////////////////////////

static inline float yo_lerp(float a, float b, float amount)
{
    float ret = (a * (1 - YO_CLAMP01(amount))) + (b * YO_CLAMP01(amount));
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Overflow protected arithmetic
//
//
////////////////////////////////////////////////////////////////

static inline uint32_t yo_safe_add_u32(uint32_t a, uint32_t b)  { return (a + b >= a) ? (a + b) : (UINT32_MAX); }
static inline uint64_t yo_safe_add_u64(uint64_t a, uint64_t b)  { return (a + b >= a) ? (a + b) : (UINT64_MAX); }
static inline size_t   yo_safe_add_usize(size_t a, size_t b)    { return (a + b >= a) ? (a + b) : (SIZE_MAX); }
static inline uint32_t yo_safe_sub_u32(uint32_t a, uint32_t b)  { return (a > b) ? (a - b) : (0); }
static inline uint64_t yo_safe_sub_u64(uint64_t a, uint64_t b)  { return (a > b) ? (a - b) : (0); }
static inline size_t   yo_safe_sub_usize(size_t a, size_t b)    { return (a > b) ? (a - b) : (0); }

////////////////////////////////////////////////////////////////
//
//
// Bit fiddling
//
//
////////////////////////////////////////////////////////////////

static inline uint32_t yo_popcnt(uint32_t a)
{
    uint32_t ret = __popcnt(a);
    return ret;
}

static inline bool yo_is_power_of_two(uint32_t a)
{
    uint32_t popcnt = yo_popcnt(a);
    bool ret = (popcnt == 1) || (popcnt == 0);
    return ret;
}

static inline int32_t yo_round_up_to_power_of_two(int32_t a)
{
    // TODO(rune): Better implementation, fix infite loop, etc.

    int32_t ret = 1;
    while (ret <= a)
    {
        ret *= 2;
    }
    return ret;
}

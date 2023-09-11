#pragma once

#ifndef YO_FONT_TABLE_SLOT_COUNT
#define YO_FONT_TABLE_SLOT_COUNT 128 // NOTE(rune): Must be less than UINT16_MAX
#endif

typedef struct yo_font_slot yo_font_slot_t;
struct yo_font_slot
{
    yo_font_id_t id;
    bool occupied;

    // NOTE(rune): Font metrics
    // yo_font_metrics_t metrics;

    // NOTE(rune): Font backend data
    yo_font_backend_info_t backend_info;
};

// TODO(rune): Consider placing inside yo_context_t
static yo_font_slot_t yo_font_slots[YO_FONT_TABLE_SLOT_COUNT];

static yo_font_id_t         yo_font_id_none();
static yo_font_slot_t *     yo_font_table_slot_get(yo_font_id_t id);
static yo_font_slot_t *     yo_font_table_slot_alloc();
static void                 yo_font_table_slot_free(yo_font_slot_t *slot);

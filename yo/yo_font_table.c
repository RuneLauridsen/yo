#pragma once

static yo_font_id_t yo_font_id_none()
{
    yo_font_id_t ret = { 0 };
    return ret;
}

static yo_font_slot_t *yo_font_table_slot_get(yo_font_id_t id)
{
    yo_font_slot_t *ret = NULL;

    if ((id.slot >= 1) && (id.slot <= YO_FONT_TABLE_SLOT_COUNT))
    {
        yo_font_slot_t *slot = &yo_font_slots[id.slot - 1];
        if ((slot->id.generation == id.generation) && (slot->occupied))
        {
            ret = slot;
        }
    }

    return ret;
}

static yo_font_slot_t *yo_font_table_slot_alloc()
{
    yo_font_slot_t *ret = NULL;

    for (uint16_t i = 1; i <= YO_FONT_TABLE_SLOT_COUNT; i++)
    {
        yo_font_slot_t *slot = &yo_font_slots[i - 1];
        if (!slot->occupied)
        {
            slot->id.slot = i;
            slot->id.generation++;
            slot->occupied = true;
            ret = slot;
            break;
        }
    }

    return ret;
}

static void yo_font_table_slot_free(yo_font_slot_t *slot)
{
    if (slot)
    {
        slot->occupied = false;
    }
}
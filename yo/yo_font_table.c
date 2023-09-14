#pragma once

static yo_font_id_t yo_font_id_none()
{
    yo_font_id_t ret = { 0 };
    return ret;
}

static yo_font_slot_t *yo_font_table_slot_find(yo_font_id_t id)
{
    yo_font_slot_t *ret = NULL;

    uint16_t get_slot_id    = (uint16_t)((id.u64 & 0x0000'0000'0000'ffff) >> 0);
    uint32_t get_generation = (uint32_t)((id.u64 & 0xffff'ffff'0000'0000) >> 32);

    if ((get_slot_id >= 1) && (get_slot_id <= YO_FONT_TABLE_SLOT_COUNT))
    {
        uint16_t slot_idx = get_slot_id - 1;
        yo_font_slot_t *slot = &yo_font_slots[slot_idx];

        YO_ASSERT(slot->slot_id == get_slot_id);

        if ((slot->generation == get_generation) && (slot->occupied))
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
            slot->slot_id = i;
            slot->generation++;
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

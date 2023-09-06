#pragma once

static void             yo_init_glyph_atlas(yo_atlas_t *atlas);
static yo_atlas_node_t *yo_get_glyph(yo_atlas_t *atlas, uint32_t codepoint, uint32_t fontsize);
static void             yo_get_glyph_uv(yo_atlas_t *atlas, yo_atlas_node_t *glyph, yo_v2f_t *uv0, yo_v2f_t *uv1);

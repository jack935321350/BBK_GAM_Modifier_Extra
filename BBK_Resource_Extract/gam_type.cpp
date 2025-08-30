#include "gam_type.h"

int16_t calc_drama_size(drama_head_t *drama_head)
{
    return (int16_t)sizeof(drama_head_t) + drama_head->data_size - (int16_t)sizeof(uint16_t);
}

int16_t calc_map_size(map_head_t *map_head)
{
    return (int16_t)sizeof(map_head_t) + map_head->width * map_head->height * sizeof(map_block_t);
}

int16_t calc_effects_size(effects_head_t *effects_head)
{
    int16_t effects_size = sizeof(effects_head_t) + effects_head->frames * sizeof(effects_block_t);
    effects_icon_head_t *effects_icon_head = reinterpret_cast<effects_icon_head_t *>(reinterpret_cast<uint8_t *>(effects_head) + effects_size);
    for(uint8_t i = 0; i < effects_head->icon_max; i++) {
        int16_t effects_icon_size = calc_effects_icon_size(effects_icon_head);
        effects_icon_head = reinterpret_cast<effects_icon_head_t *>(reinterpret_cast<uint8_t *>(effects_icon_head) + effects_icon_size);
        effects_size += effects_icon_size;
    }
    return effects_size;
}

int16_t calc_effects_icon_size(effects_icon_head_t *effects_icon_head)
{
    int width = effects_icon_head->width;
    if(width % 8 > 0) {
        width += (8 - width % 8);
    }
    return (int16_t)sizeof(effects_icon_head_t) + width * effects_icon_head->height * effects_icon_head->count * effects_icon_head->transparency / 8;
}

int16_t calc_magic_list_size(magic_list_head_t *magic_list_head)
{
    return (int16_t)sizeof(magic_list_head_t) + magic_list_head->count * sizeof(magic_list_block_t);
}

int16_t calc_level_list_size(level_list_head_t *level_list_head)
{
    return (int16_t)sizeof(level_list_head_t) + level_list_head->level_max * sizeof(level_list_block_t);
}

bool check_effects_icon_valid(effects_icon_head_t *effects_icon_head, int max_type)
{
    if(effects_icon_head->type == 0x00 || effects_icon_head->type > max_type) {
        return false;
    }
    if(effects_icon_head->width > 160 || effects_icon_head->height > 96) {
        return false;
    }
    if(effects_icon_head->count > 12 || effects_icon_head->transparency > 2) {
        return false;
    }
    return true;
}

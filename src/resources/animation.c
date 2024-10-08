#include "resources/animation.h"
#include "formats/animation.h"
#include "utils/allocator.h"
#include <stdlib.h>

void animation_create(animation *ani, void *src, int id) {
    sd_animation *sdani = (sd_animation *)src;

    // Copy simple stuff
    ani->id = id;
    ani->start_pos = vec2i_create(sdani->start_x, sdani->start_y);
    str_from_c(&ani->animation_string, sdani->anim_string);

    // Copy collision coordinates
    vector_create(&ani->collision_coords, sizeof(collision_coord));
    collision_coord tmp_coord;
    for(int i = 0; i < sdani->coord_count; i++) {
        tmp_coord.pos = vec2i_create(sdani->coord_table[i].x, sdani->coord_table[i].y);
        tmp_coord.frame_index = sdani->coord_table[i].frame_id;
        vector_append(&ani->collision_coords, &tmp_coord);
    }

    ani->extra_string_count = sdani->extra_string_count;
    // Copy extra strings
    vector_create(&ani->extra_strings, sizeof(str));
    str tmp_string;
    for(int i = 0; i < sdani->extra_string_count; i++) {
        str_from_c(&tmp_string, sdani->extra_strings[i]);
        vector_append(&ani->extra_strings, &tmp_string);
        // don't str_free tmp_str here because it will free the pointers
        // inside it, which vector_append does not copy
    }

    // Handle sprites
    vector_create(&ani->sprites, sizeof(sprite));
    sprite tmp_sprite;
    for(int i = 0; i < sdani->sprite_count; i++) {
        sprite_create(&tmp_sprite, (void *)sdani->sprites[i], i);
        vector_append(&ani->sprites, &tmp_sprite);
    }
}

animation *create_animation_from_single(sprite *sp, vec2i pos) {
    animation *a = omf_calloc(1, sizeof(animation));
    a->start_pos = pos;
    a->id = -1;
    str_from_c(&a->animation_string, "A9999999999");
    vector_create(&a->collision_coords, sizeof(collision_coord));
    vector_create(&a->extra_strings, sizeof(str));
    vector_create(&a->sprites, sizeof(sprite));
    vector_append(&a->sprites, sp);
    omf_free(sp);
    return a;
}

int animation_clone(animation *src, animation *dst) {
    iterator it;
    memcpy(dst, src, sizeof(animation));
    str_from(&dst->animation_string, &src->animation_string);
    vector_create(&dst->collision_coords, sizeof(collision_coord));
    vector_iter_begin(&src->collision_coords, &it);
    collision_coord *tmp_coord = NULL;
    while((tmp_coord = iter_next(&it)) != NULL) {
        vector_append(&dst->collision_coords, tmp_coord);
    }
    str *tmp_str = NULL;
    vector_create(&dst->extra_strings, sizeof(str));
    vector_iter_begin(&src->extra_strings, &it);
    while((tmp_str = iter_next(&it)) != NULL) {
        str new_str;
        str_create(&new_str);
        str_from(&new_str, tmp_str);
        vector_append(&dst->extra_strings, &new_str);
    }
    vector_create(&dst->sprites, sizeof(sprite));
    vector_iter_begin(&src->sprites, &it);
    sprite *tmp_sprite = NULL;
    while((tmp_sprite = iter_next(&it)) != NULL) {
        sprite new_sprite;
        sprite_clone(tmp_sprite, &new_sprite);
        vector_append(&dst->sprites, &new_sprite);
    }

    return 0;
}

sprite *animation_get_sprite(animation *ani, int sprite_id) {
    return (sprite *)vector_get(&ani->sprites, sprite_id);
}

int animation_get_sprite_count(animation *ani) {
    return vector_size(&ani->sprites);
}

void animation_free(animation *ani) {
    iterator it;

    // Free animation string
    str_free(&ani->animation_string);

    // Free collision coordinates
    vector_free(&ani->collision_coords);

    // Free extra strings
    vector_iter_begin(&ani->extra_strings, &it);
    str *tmp_str = NULL;
    while((tmp_str = iter_next(&it)) != NULL) {
        str_free(tmp_str);
    }
    vector_free(&ani->extra_strings);

    // Free animations
    vector_iter_begin(&ani->sprites, &it);
    sprite *tmp_sprite = NULL;
    while((tmp_sprite = iter_next(&it)) != NULL) {
        sprite_free(tmp_sprite);
    }
    vector_free(&ani->sprites);
}

#include <stdlib.h>

#include "game/scenes/cutscene.h"
#include "resources/languages.h"
#include "game/text/text.h"
#include "game/game_state.h"
#include "audio/music.h"
#include "video/video.h"
#include "resources/ids.h"
#include "utils/log.h"

#define END_TEXT 992
#define END1_TEXT 993
#define END2_TEXT 1003

typedef struct cutscene_local_t {
    char *text;
    char *current;
    int len;
    int pos;
    int text_x;
    int text_y;
    int text_width;
    color color;

} cutscene_local;

int cutscene_next_scene(scene *scene) {
  switch (scene->id) {
    case SCENE_END:
      return SCENE_END1;
    case SCENE_END1:
      return SCENE_END2;
    case SCENE_END2:
      return SCENE_SCOREBOARD;
    default:
      return SCENE_NONE;
  }
}

void cutscene_input_tick(scene *scene) {
    cutscene_local *local = scene_get_userdata(scene);
    game_player *player1 = game_state_get_player(scene->gs, 0);
    ctrl_event *p1=NULL, *i;

    controller_poll(player1->ctrl, &p1);

    i = p1;
    if (i) {
        do {
            if(i->type == EVENT_TYPE_ACTION) {
                if (
                        i->event_data.action == ACT_KICK ||
                        i->event_data.action == ACT_PUNCH) {

                    if (strlen(local->current) + local->pos < local->len) {
                        local->pos += strlen(local->current)+1;
                        local->current += strlen(local->current)+1;
                        char * p;
                        if ((p = strchr(local->current, '\n'))) {
                            // null out the byte
                            *p = '\0';
                        }
                    } else {
                        game_state_set_next(scene->gs, cutscene_next_scene(scene));
                    }
                }
            }
        } while((i = i->next));
    }
    controller_free_chain(p1);
}

void cutscene_render_overlay(scene *scene) {
    cutscene_local *local = scene_get_userdata(scene);
    font_render_wrapped(&font_small, local->current, local->text_x, local->text_y, local->text_width, local->color);
}

void cutscene_free(scene *scene) {
    cutscene_local *local = scene_get_userdata(scene);
    free(local->text);
    free(local);
}

void cutscene_startup(scene *scene, int id, int *m_load, int *m_repeat) {
    if(scene->id == SCENE_END || scene->id == SCENE_END1) {
        if(id == 1) {
            *m_load = 1;
        }
    } else if(scene->id == SCENE_END2) {
        if(id == 1 || id == 11) {
            *m_load = 1;
        }
    }
}

int cutscene_create(scene *scene) {
    cutscene_local *local = malloc(sizeof(cutscene_local));

    game_player *p1 = game_state_get_player(scene->gs, 0);

    const char *text = "";
    switch (scene->id) {
      case SCENE_END:
        music_play(PSM_END);
        text = lang_get(END_TEXT);
        local->text_x = 10;
        local->text_y = 5;
        local->text_width = 300;
        local->color = COLOR_YELLOW;
        break;

      case SCENE_END1:
        text = lang_get(END1_TEXT+p1->pilot_id);
        local->text_x = 10;
        local->text_y = 160;
        local->text_width = 300;
        local->color = COLOR_RED;

        // Pilot face
        animation *ani = &bk_get_info(&scene->bk_data, 3)->ani;
        object *obj = malloc(sizeof(object));
        object_create(obj, scene->gs, vec2i_create(0,0), vec2f_create(0, 0));
        object_set_animation(obj, ani);
        object_select_sprite(obj, p1->pilot_id);
        obj->halt=1;
        game_state_add_object(scene->gs, obj, RENDER_LAYER_TOP, 0, 0);

        // Face effects
        ani = &bk_get_info(&scene->bk_data, 10+p1->pilot_id)->ani;
        obj = malloc(sizeof(object));
        object_create(obj, scene->gs, vec2i_create(0,0), vec2f_create(0, 0));
        object_set_animation(obj, ani);
        game_state_add_object(scene->gs, obj, RENDER_LAYER_TOP, 0, 0);
        break;

      case SCENE_END2:
        text = lang_get(END2_TEXT+p1->pilot_id);
        local->text_x = 10;
        local->text_y = 160;
        local->text_width = 300;
        local->color = COLOR_GREEN;
        break;
    }

    local->len = strlen(text)-1;
    local->pos = 0;
    local->text = malloc(strlen(text)+1);
    strcpy(local->text, text);
    local->current = local->text;
    char *p;

    if ((p = strchr(local->text, '\n'))) {
      // null out the byte
      *p = '\0';
    }

    // Callbacks
    scene_set_userdata(scene, local);
    scene_set_free_cb(scene, cutscene_free);
    scene_set_input_poll_cb(scene, cutscene_input_tick);
    scene_set_startup_cb(scene, cutscene_startup);
    scene_set_render_overlay_cb(scene, cutscene_render_overlay);

    // Pick renderer
    video_select_renderer(VIDEO_RENDERER_HW);

    return 0;
}

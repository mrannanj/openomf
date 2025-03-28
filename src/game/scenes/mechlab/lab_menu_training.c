#include <stdio.h>

#include "game/gui/label.h"
#include "game/gui/sizer.h"
#include "game/gui/spritebutton.h"
#include "game/gui/trn_menu.h"
#include "game/scenes/mechlab.h"
#include "game/scenes/mechlab/button_details.h"
#include "game/scenes/mechlab/lab_menu_training.h"
#include "game/utils/formatting.h"
#include "resources/bk.h"
#include "resources/languages.h"

// I don't care anymore, sorry
static component *label1;
static component *label2;

static int32_t prices[] = {50,   80,   120,   180,   240,   300,   450,   600,   800,   1100,   1500,   2500,
                           4000, 7000, 10000, 14000, 20000, 28000, 40000, 55000, 75000, 100000, 140000, 200000};

static void lab_menu_focus_power(component *c, bool focused, void *userdata);
static void lab_menu_focus_agility(component *c, bool focused, void *userdata);
static void lab_menu_focus_endurance(component *c, bool focused, void *userdata);

void lab_menu_training_power(component *c, void *userdata) {
    scene *s = userdata;
    game_player *p1 = game_state_get_player(s->gs, 0);
    sd_pilot *pilot = game_player_get_pilot(p1);
    int32_t price = prices[pilot->power];
    pilot->money -= price;
    pilot->power++;
    mechlab_update(s);
    lab_menu_focus_power(c, true, userdata);
}

void lab_menu_training_check_power_price(component *c, void *userdata) {
    scene *s = userdata;
    game_player *p1 = game_state_get_player(s->gs, 0);
    sd_pilot *pilot = game_player_get_pilot(p1);
    if(pilot->power > 23) {
        component_disable(c, 1);
        return;
    }
    int32_t price = prices[pilot->power];
    if(price > pilot->money) {
        component_disable(c, 1);
    }
}

void lab_menu_training_agility(component *c, void *userdata) {
    scene *s = userdata;
    game_player *p1 = game_state_get_player(s->gs, 0);
    sd_pilot *pilot = game_player_get_pilot(p1);
    int32_t price = prices[pilot->agility];
    pilot->money -= price;
    pilot->agility++;
    mechlab_update(s);
    lab_menu_focus_agility(c, true, userdata);
}

void lab_menu_training_check_agility_price(component *c, void *userdata) {
    scene *s = userdata;
    game_player *p1 = game_state_get_player(s->gs, 0);
    sd_pilot *pilot = game_player_get_pilot(p1);
    if(pilot->agility > 23) {
        component_disable(c, 1);
        return;
    }
    int32_t price = prices[pilot->agility];
    if(price > pilot->money) {
        component_disable(c, 1);
    }
}

void lab_menu_training_endurance(component *c, void *userdata) {
    scene *s = userdata;
    game_player *p1 = game_state_get_player(s->gs, 0);
    sd_pilot *pilot = game_player_get_pilot(p1);
    int32_t price = prices[pilot->endurance];
    pilot->money -= price;
    pilot->endurance++;
    mechlab_update(s);
    lab_menu_focus_endurance(c, true, userdata);
}

void lab_menu_training_check_endurance_price(component *c, void *userdata) {
    scene *s = userdata;
    game_player *p1 = game_state_get_player(s->gs, 0);
    sd_pilot *pilot = game_player_get_pilot(p1);
    if(pilot->endurance > 23) {
        component_disable(c, 1);
        return;
    }
    int32_t price = prices[pilot->endurance];
    if(price > pilot->money) {
        component_disable(c, 1);
    }
}

void lab_menu_training_done(component *c, void *userdata) {
    trnmenu_finish(c->parent);
}

// clang-format off
static const button_details details_list[] = {
    {lab_menu_training_power,     "POWER",   TEXT_ROW_HORIZONTAL, TEXT_ALIGN_CENTER, TEXT_ALIGN_TOP,    {0, 0, 2, 0}, false},
    {lab_menu_training_agility,   "AGILITY", TEXT_ROW_HORIZONTAL, TEXT_ALIGN_CENTER, TEXT_ALIGN_TOP,    {0, 0, 2, 0}, false},
    {lab_menu_training_endurance, "ENDUR.",  TEXT_ROW_HORIZONTAL, TEXT_ALIGN_CENTER, TEXT_ALIGN_TOP,    {0, 0, 2, 0}, false},
    {lab_menu_training_done,      "DONE",    TEXT_ROW_VERTICAL,   TEXT_ALIGN_CENTER, TEXT_ALIGN_MIDDLE, {1, 0, 0, 0}, false},
};
// clang-format on

static void lab_menu_focus_power(component *c, bool focused, void *userdata) {
    if(focused) {
        scene *s = userdata;
        game_player *p1 = game_state_get_player(s->gs, 0);
        sd_pilot *pilot = game_player_get_pilot(p1);
        label_set_text(label1, lang_get(512));
        if(pilot->power > 23) {
            label_set_text(label2, "UNAVAILABLE");
        } else {
            char tmp[32];
            char price_str[16];
            score_format(prices[pilot->power], price_str, sizeof(price_str));
            snprintf(tmp, sizeof(tmp), "$ %sK", price_str);
            label_set_text(label2, tmp);
        }
        mechlab_set_hint(s, lang_get(533));
    }
}

static void lab_menu_focus_agility(component *c, bool focused, void *userdata) {
    if(focused) {
        scene *s = userdata;
        game_player *p1 = game_state_get_player(s->gs, 0);
        sd_pilot *pilot = game_player_get_pilot(p1);
        label_set_text(label1, lang_get(513));
        if(pilot->agility > 23) {
            label_set_text(label2, "UNAVAILABLE");
        } else {
            char tmp[32];
            char price_str[16];
            score_format(prices[pilot->agility], price_str, sizeof(price_str));
            snprintf(tmp, sizeof(tmp), "$ %sK", price_str);
            label_set_text(label2, tmp);
        }
        mechlab_set_hint(s, lang_get(534));
    }
}

static void lab_menu_focus_endurance(component *c, bool focused, void *userdata) {
    if(focused) {
        scene *s = userdata;
        game_player *p1 = game_state_get_player(s->gs, 0);
        sd_pilot *pilot = game_player_get_pilot(p1);
        label_set_text(label1, lang_get(514));
        if(pilot->endurance > 23) {
            label_set_text(label2, "UNAVAILABLE");
        } else {
            char tmp[32];
            char price_str[16];
            score_format(prices[pilot->endurance], price_str, sizeof(price_str));
            snprintf(tmp, sizeof(tmp), "$ %sK", price_str);
            label_set_text(label2, tmp);
        }
        mechlab_set_hint(s, lang_get(535));
    }
}

void lab_menu_focus_training_done(component *c, bool focused, void *userdata) {
    if(focused) {
        scene *s = userdata;
        label_set_text(label1, "");
        label_set_text(label2, "");
        mechlab_set_hint(s, lang_get(536));
    }
}

static const spritebutton_focus_cb focus_cbs[] = {lab_menu_focus_power, lab_menu_focus_agility,
                                                  lab_menu_focus_endurance, lab_menu_focus_training_done};

component *lab_menu_training_create(scene *s) {
    animation *main_sheets = &bk_get_info(s->bk_data, 1)->ani;
    animation *main_buttons = &bk_get_info(s->bk_data, 9)->ani;
    animation *hand_of_doom = &bk_get_info(s->bk_data, 29)->ani;

    // Initialize menu, and set button sheet
    sprite *msprite = animation_get_sprite(main_sheets, 1);
    component *menu = trnmenu_create(msprite->data, msprite->pos.x, msprite->pos.y, false);

    // Init GUI buttons with locations from the "select" button sprites
    for(int i = 0; i < animation_get_sprite_count(main_buttons); i++) {
        sprite *button_sprite = animation_get_sprite(main_buttons, i);
        component *button = sprite_button_from_details(&details_list[i], NULL, button_sprite->data, s);
        spritebutton_set_font(button, FONT_SMALL);
        spritebutton_set_text_color(button, TEXT_TRN_BLUE);
        component_set_pos_hints(button, button_sprite->pos.x, button_sprite->pos.y);

        if(i == 0) {
            spritebutton_set_tick_cb(button, lab_menu_training_check_power_price);
        } else if(i == 1) {
            spritebutton_set_tick_cb(button, lab_menu_training_check_agility_price);
        } else if(i == 2) {
            spritebutton_set_tick_cb(button, lab_menu_training_check_endurance_price);
        }
        component_tick(button);
        spritebutton_set_focus_cb(button, focus_cbs[i]);

        trnmenu_attach(menu, button);
    }

    label1 = label_create("");
    label_set_text_color(label1, 0xA5);
    label_set_font(label1, FONT_SMALL);
    component_set_size_hints(label1, 90, 110);
    component_set_pos_hints(label1, 200, 148);
    trnmenu_attach(menu, label1);

    label2 = label_create("");
    label_set_text_color(label2, 0xA7);
    label_set_font(label2, FONT_SMALL);
    component_set_size_hints(label2, 90, 110);
    component_set_pos_hints(label2, 200, 186);
    trnmenu_attach(menu, label2);

    // Bind hand animation
    trnmenu_bind_hand(menu, hand_of_doom, s->gs);

    return menu;
}

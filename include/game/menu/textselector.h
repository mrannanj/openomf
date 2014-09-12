#ifndef _TEXTSELECTOR_H
#define _TEXTSELECTOR_H

#include "game/menu/component.h"
#include "game/text/text.h"
#include "utils/vector.h"

void textselector_create(component *c, font *font, const char *text, const char *initialoption);
void textselector_add_option(component *c, const char *option);
void textselector_free(component *c);
void textselector_clear_options(component *c);
void textselector_render(component *c);
int textselector_event(component *c, SDL_Event *event);
int textselector_action(component *c, int action);
void textselector_tick(component *c);
void textselector_bindvar(component *c, int *var);
const char* textselector_get_current_text(component *c);
int textselector_get_pos(component *c);
void textselector_set_pos(component *c, int pos);

#endif // _TEXTSELECTOR_H

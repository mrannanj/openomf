#include "game/animationplayer.h"
#include "game/animation.h"
#include "utils/log.h"
#include <shadowdive/shadowdive.h>
#include "audio/music.h"
#include "audio/soundloader.h"
#include "video/video.h"
#include "video/texture.h"
#include <stdlib.h>

#include "game/scene.h"


void cmd_tickjump(animationplayer *player, int tick) {
    player->ticks = tick;
}

void cmd_sound(animationplayer *player, int sound) {
    soundloader_play(player->ani->soundtable[sound]-1);
}

void cmd_music_off() {
    music_stop();
}

void cmd_music_on(int music) {
    switch(music) {
        case 0: music_stop(); break;
        case 1: music_play("resources/END.PSM"); break;
        case 2: music_play("resources/MENU.PSM"); break;
        case 3: music_play("resources/ARENA0.PSM"); break;
        case 4: music_play("resources/ARENA1.PSM"); break;
        case 5: music_play("resources/ARENA2.PSM"); break;
        case 6: music_play("resources/ARENA3.PSM"); break;
        case 7: music_play("resources/ARENA4.PSM"); break;
    }
}

void cmd_anim_create(animationplayer *player, int id, int mx, int my, scene *scene) {
    animation *ani = array_get(player->anims, id);
    if(ani != NULL) {
        animationplayer *np = malloc(sizeof(animationplayer));
        animationplayer_create(id, np, ani, player->anims);
        np->x = ani->sdani->start_x + mx;
        np->y = ani->sdani->start_y + my;
        np->scene = scene;
        np->add_player = player->add_player;
        np->del_player = player->del_player;
        player->add_player(player->scene, np);
        DEBUG("Create animation %d @ x,y = %d,%d", id, np->x, np->y);
        return;
    } 
    PERROR("Attempted to create an instance of nonexistent animation!");
}

void cmd_anim_destroy(animationplayer *player, int id) {
    player->del_player(player->scene, id);
    DEBUG("Animation %d killed animation %d.", player->id, id);
}

void incinerate_obj(animationplayer *player) {
    if(player->obj) {
        free(player->obj);
        player->obj = 0;
    }
}

int animationplayer_create(unsigned int id, animationplayer *player, animation *animation, array *anims) {
    player->ani = animation;
    player->id = id;
    player->parser = sd_stringparser_create();
    player->ticks = 1;
    player->obj = 0;
    player->finished = 0;
    player->anims = anims;
    player->scene = 0;
    if(sd_stringparser_set_string(player->parser, animation->sdani->anim_string)) {
        sd_stringparser_delete(player->parser);
        PERROR("Unable to initialize stringparser w/ '%s'", animation->sdani->anim_string);
        return 1;
    }
    DEBUG("P: '%s'", animation->sdani->anim_string);
    return 0;
}

int isset(sd_stringparser *parser, const char *tagstr) {
    const sd_stringparser_tag_value *tag;
    if(sd_stringparser_get_tag(parser, tagstr, &tag) == 0 && tag->is_set) {
        return 1;
    }
    return 0;
}

int get(sd_stringparser *parser, const char *tagstr) {
    const sd_stringparser_tag_value *tag;
    sd_stringparser_get_tag(parser, tagstr, &tag);
    return tag->value;
}

void animationplayer_free(animationplayer *player) {
    // Free sprite
    incinerate_obj(player);
    
    // Free parser
    sd_stringparser_delete(player->parser);
    player->parser = 0;
}

void animationplayer_render(animationplayer *player) {
    if(player != NULL && player->finished) return;

    // Render self
    if(player->obj) {
        aniplayer_sprite *s = player->obj;
        video_render_sprite(s->tex, s->x, s->y, s->blendmode);
    }
}

void animationplayer_run(animationplayer *player) {
    if(player && player->finished) return;

    sd_stringparser_frame param;
    sd_stringparser *p = player->parser;
    if(sd_stringparser_run(player->parser, player->ticks-1, &param) == 0) {
        int real_frame = param.frame - 65;
        
        // Do something if animation is finished!
        if(param.is_animation_end || player->finished) {
            player->finished = 1;
            DEBUG("Animationplayer %d asks for removal ...", player->id);
            goto exit_0;
        }
        
        // Kill old sprite, if defined
        incinerate_obj(player);
        
        // Tick management
        if(isset(p, "d"))   { cmd_tickjump(player, get(p, "d")); }
    
        // Animation management
        if(isset(p, "m")) {
            int mx = isset(p, "mx") ? get(p, "mx") : 0;
            int my = isset(p, "my") ? get(p, "my") : 0;
            cmd_anim_create(player, get(p, "m"), mx, my, player->scene);
        }
        if(isset(p, "md")) { 
            cmd_anim_destroy(player, get(p, "md")); 
        }
    
        // Handle music and sounds
        if(isset(p, "smo")) { cmd_music_on(get(p, "smo"));    }
        if(isset(p, "smf")) { cmd_music_off();                }
        if(isset(p, "s"))   { cmd_sound(player, get(p, "s")); }
        
        // Draw frame sprite
        if(real_frame < 25) {
            sd_sprite *sprite = player->ani->sdani->sprites[real_frame];
            texture *tex = array_get(&player->ani->sprites, real_frame);
            if(tex) {
                aniplayer_sprite *anisprite = malloc(sizeof(aniplayer_sprite));
                anisprite->x = player->x + sprite->pos_x;
                anisprite->y = player->y + sprite->pos_y;
                anisprite->blendmode = isset(p, "br") ? BLEND_ADDITIVE : BLEND_ALPHA;
                anisprite->tex = tex;
                player->obj = anisprite;
            } else {
                PERROR("No texture @ %u", real_frame);
            }
        } 
    }
    player->ticks++;
    
exit_0:
    return;
}
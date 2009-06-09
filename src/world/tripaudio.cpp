/* 
 * Triplane Classic - a side-scrolling dogfighting game.
 * Copyright (C) 1996,1997,2009  Dodekaedron Software Creations Oy
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * tjt@users.sourceforge.net
 */

#include "world/tripaudio.h"
#include "settings.h"
#include "world/plane.h"
#include "triplane.h"
#include "util/wutil.h"

sb_mod_file *triplane_mod;
sb_mod_file *aces_mod;

sb_sample *sample_itexp[3];
sb_sample *sample_bomb[4];
sb_sample *sample_hitti;
sb_sample *sample_konsu[4];
sb_sample *sample_propelli;
sb_sample *sample_hit[4];
sb_sample *sample_aagun;
sb_sample *sample_splash[3];
sb_sample *sample_spcrash;
sb_sample *sample_hurr;
sb_sample *sample_itshot;
sb_sample *sample_crash[2];
sb_sample *sample_alku;
sb_sample *sample_die[9];

int alku_sample_over = 0;
int sfx_loaded = 0;
int music_loaded = 0;

void kkbase_sound(int type, int kkbase_x) {
    if (config.ground_i_sounds && config.sound_on && config.sfx_on) {
        if (type)
            play_2d_sample(sample_itshot, player_x_8[solo_country], kkbase_x);
        else
            play_2d_sample(sample_aagun, player_x_8[solo_country], kkbase_x);
    }
}

void itgun_sound(int itgun_x) {
    if (config.ground_i_sounds && config.sound_on && config.sfx_on) {
        play_2d_sample(sample_itexp[wrandom(3)], player_x_8[solo_country], itgun_x);
    }
}

void load_all_samples(void) {
    int l;

    sample_itexp[0] = sample_load("Itexp1");
    sample_itexp[0]->left_volume = SAMPLE_VOLUME;
    sample_itexp[0]->right_volume = SAMPLE_VOLUME;

    sample_itexp[1] = sample_load("Itexp2");
    sample_itexp[1]->left_volume = SAMPLE_VOLUME;
    sample_itexp[1]->right_volume = SAMPLE_VOLUME;

    sample_itexp[2] = sample_load("Itexp3");
    sample_itexp[2]->left_volume = SAMPLE_VOLUME;
    sample_itexp[2]->right_volume = SAMPLE_VOLUME;

    sample_bomb[1] = sample_load("Bomb1");
    sample_bomb[1]->left_volume = SAMPLE_VOLUME;
    sample_bomb[1]->right_volume = SAMPLE_VOLUME;

    sample_bomb[2] = sample_load("Bomb2");
    sample_bomb[2]->left_volume = SAMPLE_VOLUME;
    sample_bomb[2]->right_volume = SAMPLE_VOLUME;

    sample_bomb[3] = sample_load("Bomb3");
    sample_bomb[3]->left_volume = SAMPLE_VOLUME;
    sample_bomb[3]->right_volume = SAMPLE_VOLUME;

    sample_konsu[1] = sample_load("Guns1");
    sample_konsu[1]->left_volume = SAMPLE_VOLUME;
    sample_konsu[1]->right_volume = SAMPLE_VOLUME;

    sample_konsu[2] = sample_load("Guns2");
    sample_konsu[2]->left_volume = SAMPLE_VOLUME;
    sample_konsu[2]->right_volume = SAMPLE_VOLUME;

    sample_konsu[3] = sample_load("Guns3");
    sample_konsu[3]->left_volume = SAMPLE_VOLUME;
    sample_konsu[3]->right_volume = SAMPLE_VOLUME;

    sample_hit[0] = sample_load("Hit1");
    sample_hit[0]->left_volume = SAMPLE_VOLUME;
    sample_hit[0]->right_volume = SAMPLE_VOLUME;

    sample_hit[1] = sample_load("Hit2");
    sample_hit[1]->left_volume = SAMPLE_VOLUME;
    sample_hit[1]->right_volume = SAMPLE_VOLUME;

    sample_hit[2] = sample_load("Hit3");
    sample_hit[2]->left_volume = SAMPLE_VOLUME;
    sample_hit[2]->right_volume = SAMPLE_VOLUME;

    sample_hit[3] = sample_load("Hit4");
    sample_hit[3]->left_volume = SAMPLE_VOLUME;
    sample_hit[3]->right_volume = SAMPLE_VOLUME;

    sample_aagun = sample_load("Aagun");
    sample_aagun->left_volume = SAMPLE_VOLUME;
    sample_aagun->right_volume = SAMPLE_VOLUME;

    sample_splash[0] = sample_load("Spla1");
    sample_splash[0]->left_volume = SAMPLE_VOLUME;
    sample_splash[0]->right_volume = SAMPLE_VOLUME;

    sample_splash[1] = sample_load("Spla2");
    sample_splash[1]->left_volume = SAMPLE_VOLUME;
    sample_splash[1]->right_volume = SAMPLE_VOLUME;

    sample_splash[2] = sample_load("Spla3");
    sample_splash[2]->left_volume = SAMPLE_VOLUME;
    sample_splash[2]->right_volume = SAMPLE_VOLUME;

    sample_hurr = sample_load("Hurr");
    sample_hurr->left_volume = SAMPLE_VOLUME;
    sample_hurr->right_volume = SAMPLE_VOLUME;

    sample_itshot = sample_load("Itshot");
    sample_itshot->left_volume = SAMPLE_VOLUME;
    sample_itshot->right_volume = SAMPLE_VOLUME;

    sample_spcrash = sample_load("Spcra1");
    sample_spcrash->left_volume = SAMPLE_VOLUME;
    sample_spcrash->right_volume = SAMPLE_VOLUME;

    sample_crash[0] = sample_load("Crash1");
    sample_crash[0]->left_volume = SAMPLE_VOLUME;
    sample_crash[0]->right_volume = SAMPLE_VOLUME;

    sample_crash[1] = sample_load("Crash2");
    sample_crash[1]->left_volume = SAMPLE_VOLUME;
    sample_crash[1]->right_volume = SAMPLE_VOLUME;

    sample_alku = sample_load("Alku");
    sample_alku->left_volume = 32;
    sample_alku->right_volume = 32;

    sample_die[0] = sample_load("Die1");
    sample_die[1] = sample_load("Die2");
    sample_die[2] = sample_load("Die3");
    sample_die[3] = sample_load("Die4");
    sample_die[4] = sample_load("Die5");
    sample_die[5] = sample_load("Die6");
    sample_die[6] = sample_load("Die7");
    sample_die[7] = sample_load("Die8");
    sample_die[8] = sample_load("Die9");

    for (l = 0; l < 9; l++) {
        sample_die[l]->left_volume = SAMPLE_VOLUME - 8;
        sample_die[l]->right_volume = SAMPLE_VOLUME - 8;
    }
}

void load_sfx(void) {
    sfx_loaded = 1;
    load_all_samples();
}

void load_music(void) {
    music_loaded = 1;

    triplane_mod = sdl_load_mod_file("music1");
    aces_mod = sdl_load_mod_file("maces");
}

void clear_sfx(void) {
    int l;

    sfx_loaded = 0;

    sdl_free_sample(sample_itexp[0]);
    sdl_free_sample(sample_itexp[1]);
    sdl_free_sample(sample_itexp[2]);
    sdl_free_sample(sample_bomb[1]);
    sdl_free_sample(sample_bomb[2]);
    sdl_free_sample(sample_bomb[3]);
    sdl_free_sample(sample_konsu[1]);
    sdl_free_sample(sample_konsu[2]);
    sdl_free_sample(sample_konsu[3]);
    sdl_free_sample(sample_hit[0]);
    sdl_free_sample(sample_hit[1]);
    sdl_free_sample(sample_hit[2]);
    sdl_free_sample(sample_hit[3]);
    sdl_free_sample(sample_aagun);
    sdl_free_sample(sample_splash[0]);
    sdl_free_sample(sample_splash[1]);
    sdl_free_sample(sample_splash[2]);
    sdl_free_sample(sample_hurr);
    sdl_free_sample(sample_spcrash);
    sdl_free_sample(sample_itshot);
    sdl_free_sample(sample_crash[0]);
    sdl_free_sample(sample_crash[1]);
    sdl_free_sample(sample_alku);

    for (l = 0; l < 9; l++) {
        sdl_free_sample(sample_die[l]);


    }
}


void clear_music(void) {
    music_loaded = 0;
    sdl_free_mod_file(triplane_mod);
    sdl_free_mod_file(aces_mod);
}

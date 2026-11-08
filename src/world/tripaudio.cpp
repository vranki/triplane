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

#include "../world/tripaudio.h"
#include "../settings.h"
#include "../triplane.h"
#include "../util/wutil.h"
#include "../world/plane.h"

sb_mod_file *triplane_mod;
sb_mod_file *aces_mod;

sb_sample *sample_itexp[3];
sb_sample *sample_bomb[4];
sb_sample *sample_konsu[4];
sb_sample *sample_hit[4];
sb_sample *sample_aagun;
sb_sample *sample_splash[3];
sb_sample *sample_spcrash;
sb_sample *sample_hurr;
sb_sample *sample_itshot;
sb_sample *sample_crash[2];
sb_sample *sample_alku;
sb_sample *sample_die[9];

int sfx_loaded = 0;
int music_loaded = 0;

void kkbase_sound(const int type, const int kkbase_sound_x) {
  if (config.ground_i_sounds && config.sound_on && config.sfx_on) {
    if (type)
      play_2d_sample(sample_itshot, player_x_8[solo_country], kkbase_sound_x);
    else
      play_2d_sample(sample_aagun, player_x_8[solo_country], kkbase_sound_x);
  }
}

void itgun_sound(int itgun_x) {
  if (config.ground_i_sounds && config.sound_on && config.sfx_on) {
    play_2d_sample(sample_itexp[util::wutil::wrandom(3)], player_x_8[solo_country], itgun_x);
  }
}

/* Every sound effect: where it is kept, its name in the data file and its
 * volume, in loading order. */
struct sample_slot {
  sb_sample **sample;
  const char *name;
  int volume;
};

static const sample_slot sample_slots[] = {
    {&sample_itexp[0], "Itexp1", SAMPLE_VOLUME},
    {&sample_itexp[1], "Itexp2", SAMPLE_VOLUME},
    {&sample_itexp[2], "Itexp3", SAMPLE_VOLUME},
    {&sample_bomb[1], "Bomb1", SAMPLE_VOLUME},
    {&sample_bomb[2], "Bomb2", SAMPLE_VOLUME},
    {&sample_bomb[3], "Bomb3", SAMPLE_VOLUME},
    {&sample_konsu[1], "Guns1", SAMPLE_VOLUME},
    {&sample_konsu[2], "Guns2", SAMPLE_VOLUME},
    {&sample_konsu[3], "Guns3", SAMPLE_VOLUME},
    {&sample_hit[0], "Hit1", SAMPLE_VOLUME},
    {&sample_hit[1], "Hit2", SAMPLE_VOLUME},
    {&sample_hit[2], "Hit3", SAMPLE_VOLUME},
    {&sample_hit[3], "Hit4", SAMPLE_VOLUME},
    {&sample_aagun, "Aagun", SAMPLE_VOLUME},
    {&sample_splash[0], "Spla1", SAMPLE_VOLUME},
    {&sample_splash[1], "Spla2", SAMPLE_VOLUME},
    {&sample_splash[2], "Spla3", SAMPLE_VOLUME},
    {&sample_hurr, "Hurr", SAMPLE_VOLUME},
    {&sample_itshot, "Itshot", SAMPLE_VOLUME},
    {&sample_spcrash, "Spcra1", SAMPLE_VOLUME},
    {&sample_crash[0], "Crash1", SAMPLE_VOLUME},
    {&sample_crash[1], "Crash2", SAMPLE_VOLUME},
    {&sample_alku, "Alku", 32},
    {&sample_die[0], "Die1", SAMPLE_VOLUME - 8},
    {&sample_die[1], "Die2", SAMPLE_VOLUME - 8},
    {&sample_die[2], "Die3", SAMPLE_VOLUME - 8},
    {&sample_die[3], "Die4", SAMPLE_VOLUME - 8},
    {&sample_die[4], "Die5", SAMPLE_VOLUME - 8},
    {&sample_die[5], "Die6", SAMPLE_VOLUME - 8},
    {&sample_die[6], "Die7", SAMPLE_VOLUME - 8},
    {&sample_die[7], "Die8", SAMPLE_VOLUME - 8},
    {&sample_die[8], "Die9", SAMPLE_VOLUME - 8},
};

void load_all_samples() {
  for (const sample_slot &slot : sample_slots) {
    *slot.sample = sample_load(slot.name);
    (*slot.sample)->left_volume = slot.volume;
    (*slot.sample)->right_volume = slot.volume;
  }
}

void load_sfx() {
  sfx_loaded = 1;
  load_all_samples();
}

void load_music() {
  music_loaded = 1;

  triplane_mod = sdl_load_mod_file("music1");
  aces_mod = sdl_load_mod_file("maces");
}

void clear_sfx() {
  sfx_loaded = 0;

  for (const sample_slot &slot : sample_slots)
    sdl_free_sample(*slot.sample);
}

void clear_music() {
  music_loaded = 0;
  sdl_free_mod_file(triplane_mod);
  sdl_free_mod_file(aces_mod);
}

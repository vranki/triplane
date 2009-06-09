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

#ifndef TRIPAUDIO_H
#define TRIPAUDIO_H

#include "io/sound.h"

extern sb_mod_file *triplane_mod;
extern sb_mod_file *aces_mod;

extern sb_sample *sample_itexp[3];
extern sb_sample *sample_bomb[4];
extern sb_sample *sample_hitti;
extern sb_sample *sample_konsu[4];
extern sb_sample *sample_propelli;
extern sb_sample *sample_hit[4];
extern sb_sample *sample_aagun;
extern sb_sample *sample_splash[3];
extern sb_sample *sample_spcrash;
extern sb_sample *sample_hurr;
extern sb_sample *sample_crash[2];
extern sb_sample *sample_alku;
extern sb_sample *sample_die[9];

extern int sfx_loaded;
extern int music_loaded;

extern void kkbase_sound(int type, int kkbase_x);
extern void itgun_sound(int itgun_x);
extern void load_sfx(void);
extern void load_music(void);
extern void clear_sfx(void);
extern void clear_music(void);

#endif

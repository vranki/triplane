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

#ifndef SDL_COMPAT_H
#define SDL_COMPAT_H
#include <cstdint>
#ifdef HAVE_SDL_MIXER
#include <SDL/SDL_mixer.h>
#endif

constexpr int32_t SAMPLE_VOLUME = 20;

#define PAUSE_KEY SDLK_PAUSE

constexpr int32_t SOUNDCARD_NONE = 0;
constexpr int32_t SOUNDCARD_GUS = 1;
constexpr int32_t SOUNDCARD_SB = 2;
constexpr int32_t SOUNDCARD_SDL = 3;

int kbhit();
int getch();

extern unsigned char *key;

typedef struct {
  int right_volume, left_volume;
#ifdef HAVE_SDL_MIXER
  Mix_Chunk *chunk;
#endif
} sb_sample;

typedef struct {
#ifdef HAVE_SDL_MIXER
  Mix_Music *music;
#endif
} sb_mod_file;

void update_key_state();
int sdl_init_sounds();
void sdl_uninit_sounds();
void sdl_play_sample(sb_sample *sample, int looping = 0);
void sdl_stop_all_samples();
sb_sample *sdl_sample_load(const char *name);
void sdl_free_sample(sb_sample *sample);

sb_mod_file *sdl_load_mod_file(const char *name);
void sdl_free_mod_file(sb_mod_file *mod);
void sdl_play_music(sb_mod_file *mod);
void sdl_stop_music();

#endif

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

#include <SDL.h>
#include <ctype.h>

#ifdef HAVE_SDL_MIXER
/* 
 * SDL_mixer version 1.2.6 requires USE_RWOPS for Mix_LoadMUS_RW
 * (1.2.7 and later do not require it)
 */
#define USE_RWOPS
#include <SDL_mixer.h>
#endif

#include <assert.h>
#include "sdl_compat.h"
#include "io/dksfile.h"
#include "util/wutil.h"
#include "io/timing.h"

int kbhit(void) {
    SDL_Event e;
    int ret;

    nopeuskontrolli();

    ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, ~0);
    if (ret) {
        if (e.type == SDL_KEYUP) {
            return 1;
        } else {
            SDL_PollEvent(&e);
        }
    }
    return 0;
}

int getch(void) {
    SDL_Event e;

    for (;;) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYUP) {
                int s, m;
                s = e.key.keysym.sym;
                m = e.key.keysym.mod;
                if (s == SDLK_RSHIFT || s == SDLK_LSHIFT) {
                    continue;
                }
                if (m == KMOD_LSHIFT || m == KMOD_RSHIFT) {
                    if (s >= SDLK_a && s <= SDLK_z) {
                        s = toupper(s);
                    }
                }
                return s;
            }
        }
    }
}

void update_key_state(void) {
    SDL_Event e;
    SDL_PumpEvents();
    while (SDL_PollEvent(&e)) {
        ;
    }
}

/**
 * Initialize SDL sounds so that sdl_play_sample can be called.
 * @return 0 on success, nonzero otherwise.
 */
int sdl_init_sounds(void) {
#ifdef HAVE_SDL_MIXER
    int ret;

    ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (ret != 0)
        return 1;

    /*
     * use a 4096 byte buffer:
     * 44100*2*2 bytes/sec / 4096 bytes = about 1/43 seconds
     */
    ret = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    if (ret < 0)
        return 1;
    Mix_AllocateChannels(16);   /* max. 16 simultaneous samples */
    return 0;
#else
    printf("This version of triplane has been compiled without sound support.\n");
    return 1;
#endif
}

/** Deinitalize SDL sounds */
void sdl_uninit_sounds(void) {
#ifdef HAVE_SDL_MIXER
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
#endif
}

/**
 * Play sample.
 * @param sample sample loaded with sdl_load_sample.
 */
void sdl_play_sample(sb_sample * sample, int looping) {
#ifdef HAVE_SDL_MIXER
    /* scale of left_volume and right_volume is [0..32] */
    /* SDL_SetPanning wants [0..255] */
    int ch, l = sample->left_volume * 8, r = sample->right_volume * 8;
    if (l > 0)
        l--;
    if (r > 0)
        r--;
    ch = Mix_PlayChannel(-1, sample->chunk, looping ? -1 : 0);
    if (ch == -1) {             /* no free channels (or possibly another error) */
        return;                 /* ignore the error, so this sound won't be played */
    }

    Mix_SetPanning(ch, l, r);
#endif
}

/** Stop playing all samples started with sdl_play_sample */
void sdl_stop_all_samples(void) {
#ifdef HAVE_SDL_MIXER
    Mix_HaltChannel(-1);
#endif
}

/**
 * Load sample with given name.
 * @param name audio name used inside fokker.dks
 * @return loaded sample or NULL on error.
 */
sb_sample *sdl_sample_load(const char *name) {
#ifdef HAVE_SDL_MIXER
    int len, ret;
    uint8_t *p;
    sb_sample *sample;

    ret = dksopen(name);
    if (ret != 1) {
        return NULL;
    }

    len = dkssize();
    p = (uint8_t *) walloc(len);

    ret = dksread(p, len);
    assert(ret == 1);

    dksclose();

    sample = (sb_sample *) walloc(sizeof(sb_sample));

    sample->chunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(p, len), 1);
    if (sample->chunk == NULL) {
        fprintf(stderr, "sdl_sample_load: %s\n", Mix_GetError());
        exit(1);
    }

    free(p);

    return sample;
#else
    return NULL;
#endif
}

/**
 * Free sample loaded using sdl_sample_load.
 * @param sample to be free'd.
 */
void sdl_free_sample(sb_sample * sample) {
#ifdef HAVE_SDL_MIXER
    Mix_FreeChunk(sample->chunk);
    free(sample);
#endif
}

sb_mod_file *sdl_load_mod_file(const char *name) {
#ifdef HAVE_SDL_MIXER
    int len, ret;
    uint8_t *p;
    sb_mod_file *mod;
    SDL_RWops *rwops;

    ret = dksopen(name);
    if (ret != 1) {
        return NULL;
    }

    len = dkssize();
    p = (uint8_t *) walloc(len);

    ret = dksread(p, len);
    assert(ret == 1);

    dksclose();

    mod = (sb_mod_file *) walloc(sizeof(sb_mod_file));

    rwops = SDL_RWFromConstMem(p, len);
    mod->music = Mix_LoadMUS_RW(rwops);
    SDL_FreeRW(rwops);
    if (mod->music == NULL) {
        fprintf(stderr, "sdl_load_mod_file: %s\n", Mix_GetError());
        exit(1);
    }

    free(p);

    return mod;
#else
    return NULL;
#endif
}

void sdl_free_mod_file(sb_mod_file * mod) {
#ifdef HAVE_SDL_MIXER
    Mix_FreeMusic(mod->music);
    free(mod);
#endif
}

void sdl_play_music(sb_mod_file * mod) {
#ifdef HAVE_SDL_MIXER
    sdl_stop_all_samples();
    Mix_PlayMusic(mod->music, 1);
#endif
}

void sdl_stop_music(void) {
#ifdef HAVE_SDL_MIXER
    Mix_HaltMusic();
#endif
}

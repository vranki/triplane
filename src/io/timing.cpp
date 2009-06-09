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
#include <assert.h>
#include "sdl_compat.h"

static int enabled = 1;

void nopeuskontrolli(int fps) {
    static uint32_t viimeinen_aika = 0;
    uint32_t target_tick;

    if (!enabled) {
        return;
    }

    target_tick = viimeinen_aika + 1000 / fps;
    while (SDL_GetTicks() <= target_tick) {
        uint32_t diff = target_tick - SDL_GetTicks();
        if (diff > 10 && diff < 1000) {
            SDL_Delay(diff);
        }
    }

    viimeinen_aika += 1000 / fps;

    // Recover from stopped process
    if (SDL_GetTicks() > viimeinen_aika + 1000 / fps) {
        viimeinen_aika = SDL_GetTicks();
    }
}

void nopeuskontrolli_enable(int enable) {
    enabled = enable;
}

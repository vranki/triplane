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

#include "timing.h"
#include <SDL3/SDL.h>

namespace io::timing {

    static int enabled = 1;

    void nopeuskontrolli(int fps) {
        static uint32_t viimeinen_aika = 0;
        uint32_t target_tick;

        if (!enabled) {
            return;
        }

        target_tick = viimeinen_aika + 1000 / fps;
        for (;;) {
            uint32_t now = (uint32_t)SDL_GetTicks();
            if (now > target_tick)
                break;
            uint32_t diff = target_tick - now;
            if (diff >= 1000)
                break; /* the clock jumped; recovered below */
            /* sleeps, with a short spin at the end for accuracy */
            SDL_DelayPrecise((Uint64)(diff + 1) * SDL_NS_PER_MS);
        }

        viimeinen_aika += 1000 / fps;

        // Recover from stopped process
        if ((uint32_t)SDL_GetTicks() > viimeinen_aika + 1000 / fps) {
            viimeinen_aika = (uint32_t)SDL_GetTicks();
        }
    }

    void nopeuskontrolli_enable(int enable) { enabled = enable; }

} // namespace io::timing

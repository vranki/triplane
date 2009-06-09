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

#include <stdint.h>

/* Simple LCG */

static uint64_t state;
static const uint64_t a = 0x5DEECE66DLL;
static const uint32_t c = 0xB;

void triplane_srandom(uint64_t seed) {
    state = seed & ((1LL << 48) - 1);
}

uint32_t triplane_random(void) {
    state = (a * state + c) & ((1LL << 48) - 1);
    return state >> 16;
}

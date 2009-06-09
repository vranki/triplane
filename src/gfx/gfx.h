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

#ifndef GFX_H
#define GFX_H

#include "gfx/bitmap.h"
#include "gfx/font.h"
#include "gfx/fades.h"
#include "io/video.h"

void fill_vircr(int x1, int y1, int x2, int y2, unsigned char vari);
void boxi(int x1, int y1, int x2, int y2, unsigned char vari);
void draw_line(int x1, int y1, int x2, int y2, unsigned char vari);
void putpix(int x, int y, unsigned char c, int x1, int y1, int x2, int y2);
void tyhjaa_vircr(void);
void do_all_clear(int do_retrace = 0);



#endif

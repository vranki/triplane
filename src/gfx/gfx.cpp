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

#include <string.h>
#include <assert.h>
#include "gfx/gfx.h"
#include "io/video.h"

static void do_isa_clear(void);

Bitmap *standard_background = NULL;

void putpix(int x, int y, unsigned char c, int x1, int y1, int x2, int y2) {
    if (x < x1)
        return;
    if (x > x2)
        return;
    if (y < y1)
        return;
    if (y > y2)
        return;
    if (update_vircr_mode) {
        if (current_mode == VGA_MODE) {
            vircr[x + (y << 8) + (y << 6)] = c;
        } else {
            vircr[x + y * 800] = c;
        }
    }

    if (!draw_with_vircr_mode)
        fillrect(x, y, 1, 1, c);
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char vari) {
    int omalask;

    if (x1 == x2) {
        if (y1 > y2) {
            omalask = y1;
            y1 = y2;
            y2 = omalask;
        }
        fill_vircr(x1, y1, x2, y2, vari);
    } else if (y1 == y2) {
        if (x1 > x2) {
            omalask = x1;
            x1 = x2;
            x2 = omalask;
        }
        fill_vircr(x1, y1, x2, y2, vari);
    }


}

void boxi(int x1, int y1, int x2, int y2, unsigned char vari) {
    draw_line(x1, y1, x2, y1, vari);
    draw_line(x1, y1, x1, y2, vari);
    draw_line(x2, y2, x2, y1, vari);
    draw_line(x2, y2, x1, y2, vari);
}

void fill_vircr(int x1, int y1, int x2, int y2, unsigned char vari) {
    int lasky;

    if (update_vircr_mode) {
        for (lasky = y1; lasky <= y2; lasky++)
            memset(&vircr[x1 + lasky * 320], vari, x2 - x1 + 1);
    }

    if (!draw_with_vircr_mode)
        fillrect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, vari);
}

void tyhjaa_vircr(void) {
    if (standard_background == NULL) {
        if (current_mode == VGA_MODE) {
            fill_vircr(0, 0, 319, 199, 0);
        } else {
            assert(current_mode == SVGA_MODE);
            fill_vircr(0, 0, 799, 599, 0);
        }

        do_isa_clear();
    }
}

static void do_isa_clear(void) {
    if (!standard_background) {
        return;
    }

    if (current_mode == VGA_MODE) {
        standard_background->blit_fullscreen();
    } else {
        standard_background->blit(0, 0, 0, 0, 799, 599);
    }

}

void do_all_clear(int do_retrace) {
    do_all();
    do_isa_clear();
}

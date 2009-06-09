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

#include <assert.h>
#include "gfx/fades.h"
#include "gfx/gfx.h"
#include "util/wutil.h"

void horisontal_split(void) {
    Bitmap *upper;
    Bitmap *lower;
    int c1, c2 = 1;

    upper = new Bitmap(0, 0, 320, 100);
    lower = new Bitmap(0, 100, 320, 100);

    c1 = 0;
    while (c1 < 150) {
        upper->blit(0, 0 - c1);
        lower->blit(0, 100 + c1);
        do_all_clear();
        c1 += c2;
        c2++;
    }

    delete upper;
    delete lower;

}

void vertical_split() {
    Bitmap *left;
    Bitmap *right;
    int c1, c2;

    left = new Bitmap(0, 0, 160, 200);
    right = new Bitmap(160, 0, 160, 200);

    c1 = 0;
    c2 = 1;

    while (c1 < 200) {
        left->blit(0 - c1, 0);
        right->blit(160 + c1, 0);
        do_all_clear();
        c1 += c2;
        c2++;
    }

    delete left;
    delete right;


}

void pixel_fade(void) {
    int c1, c2;

    for (c1 = 0; c1 < 20; c1++) {
        for (c2 = 0; c2 < 7500; c2++) {
            int r = wrandom(64000);
            int x = r % 320, y = r / 320;
            putpix(x, y, 0, 0, 0, 319, 199);
        }
        do_all();
    }
    tyhjaa_vircr();
    do_all();

}

void partial_fade(void) {
    unsigned char next_color[256];
    int hit_value, temp_hit_value;
    int c, c2, c3, temp;

    assert(update_vircr_mode);

    next_color[0] = 0;

    for (c = 1; c < 256; c++) {
        hit_value = 1500000;
        for (c2 = 0; c2 < 255; c2++) {
            if (c2 == c)
                continue;

            temp = ((ruutu.paletti[c][0] - 2) - ruutu.paletti[c2][0]);
            if (temp < 0)
                temp = 63;
            temp_hit_value = temp * temp * temp;
            temp = ((ruutu.paletti[c][1] - 2) - ruutu.paletti[c2][1]);
            if (temp < 0)
                temp = 63;
            temp_hit_value += temp * temp * temp;
            temp = ((ruutu.paletti[c][2] - 2) - ruutu.paletti[c2][2]);
            if (temp < 0)
                temp = 63;
            temp_hit_value += temp * temp * temp;
            if ((temp_hit_value < hit_value)) {
                hit_value = temp_hit_value;
                next_color[c] = c2;
            }
        }
    }


    for (c = 0; c < 232; c += 3) {
        for (c2 = 0; c2 < 63; c2++) {
            if ((c - c2) >= 0 && (c - c2) <= 199)
                for (c3 = 0; c3 < 320; c3++)
                    putpix(c3, c - c2, next_color[vircr[c3 + (c - c2) * 320]], 0, 0, 319, 199);
        }
        do_all();
    }
    tyhjaa_vircr();
    do_all();
}

void random_fade_out(void) {
    int t;

    t = wrandom(5);

    if (current_mode == SVGA_MODE) {
        do_all_clear();
        return;
    }

    switch (t) {
    case 0:
        horisontal_split();
        break;

    case 1:
        vertical_split();
        break;

    case 2:
        vertical_split();
        break;

    case 3:
        pixel_fade();
        break;

    case 4:
        partial_fade();
        break;
    }



}

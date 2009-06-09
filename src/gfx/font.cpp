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

#include "gfx/gfx.h"
#include "io/trip_io.h"
#include "util/wutil.h"
#include <stdarg.h>
#include <string.h>

Font::Font(const char *font_name) {
    int temp;
    int temppi;
    int kokox, kokoy;
    char valiteksti[7];
    Bitmap *valikuva;

    scaled = 0;
    scaled_space = 3;

    for (temppi = 0; temppi < 256; temppi++)
        glyphs[temppi] = NULL;

    if (strlen(font_name) == 6) {
        valikuva = new Bitmap(font_name);
        valikuva->info(&kokox, &kokoy);
        kokox--;
        kokoy--;
        width = kokox / 16 - 1;
        height = kokoy / 16 - 1;
        for (temp = 0; temp < 256; temp++)
            glyphs[temp] = new Bitmap(1 + (temp - ((temp >> 4) << 4)) * (width + 1), 1 + (temp >> 4) * (height + 1), width, height, valikuva);
        delete valikuva;
    } else
        for (temp = 0; temp < 256; temp++) {
            strcpy(valiteksti, font_name);
            valiteksti[5] = (char) temp;
            valiteksti[6] = 0;

            if (dksopen(valiteksti)) {
                dksclose();
                glyphs[temp] = new Bitmap(valiteksti);
                glyphs[temp]->info(&width, &height);
            } else
                glyphs[temp] = NULL;
        }

    count_scale();
}

Font::~Font() {

    int temp;

    for (temp = 0; temp < 256; temp++)
        if (glyphs[temp] != NULL)
            delete glyphs[temp];


}

void Font::set_space(int space) {
    scaled_space = space;
}

int Font::printf(int x, int y, const char *fmt, ...) {
    int temp = 0;
    int xkohta = 0;
    char teksti[2800];
    va_list arg;

    va_start(arg, fmt);
    vsprintf(teksti, fmt, arg);
    va_end(arg);

    if (!scaled) {
        while (teksti[temp]) {
            if (teksti[temp] == '\n') {
                y += height;
                xkohta = 0;
            } else {
                if (glyphs[(unsigned char) teksti[temp]] != NULL) {
                    glyphs[(unsigned char) teksti[temp]]->blit(x + xkohta, y);

                }
                xkohta += width + 1;
            }
            temp++;
        }

        return (x + temp * (width + 1));
    }

    while (teksti[temp]) {
        if (teksti[temp] == '\n') {
            y += height;
            xkohta = 0;
        }

        else {
            if ((glyphs[(unsigned char) teksti[temp]] != NULL) && ((unsigned char) teksti[temp] != 32)) {
                glyphs[(unsigned char) teksti[temp]]->blit(x + xkohta - char_start[(unsigned char) teksti[temp]], y);
                xkohta += char_width[(unsigned char) teksti[temp]];
            } else {
                xkohta += scaled_space;
            }


        }
        temp++;
    }

    return (xkohta + x);


}

int Font::scanf(int x, int y, char *str, int max_len) {
    int kohta = 0;
    Bitmap *tausta_roska;
    char ch;

    tausta_roska = new Bitmap(x, y, (width + 1) * (max_len + 1), height);

    kohta = strlen(str);

    ch = 0;

    while (ch != 13) {
        tausta_roska->blit(x, y);
        printf(printf(x, y, "%s", str), y, "_");
        do_all();
        ch = getch();
        if (!ch) {
            getch();
            ch = 0;
        } else if (ch != 13) {
            if (ch == 8) {
                if (kohta) {
                    kohta--;
                    str[kohta] = 0;
                }
            } else if (glyphs[(unsigned char) ch] != NULL && kohta < max_len) {
                str[kohta] = ch;
                kohta++;
                str[kohta] = 0;
            } else if (kohta < max_len) {
                if (ch >= 97 && ch <= 122)
                    ch -= 32;

                if (glyphs[(unsigned char) ch] != NULL && kohta < max_len) {
                    str[kohta] = ch;
                    kohta++;
                    str[kohta] = 0;
                }
            }
        }
    }

    delete tausta_roska;
    return kohta;
}


void Font::count_scale(void) {
    int ytemp, xtemp, temp, valileveys;
    unsigned char *kuvadata;

    for (temp = 0; temp < 256; temp++)
        if (glyphs[temp] != NULL) {
            kuvadata = glyphs[temp]->info(&char_width[temp], &height);

            char_start[temp] = -1;

            for (xtemp = 0; xtemp < char_width[temp]; xtemp++) {
                for (ytemp = 0; ytemp < height; ytemp++) {
                    if (kuvadata[xtemp + ytemp * char_width[temp]] != 255)
                        char_start[temp] = xtemp;
                }
                if (char_start[temp] != -1)
                    break;
            }

            if (char_start[temp] == -1) {
                char_start[temp] = 0;
                char_width[temp] = 3;
            } else {
                valileveys = -1;
                for (xtemp = char_width[temp] - 1; xtemp >= 0; xtemp--) {
                    for (ytemp = 0; ytemp < height; ytemp++) {
                        if (kuvadata[xtemp + ytemp * char_width[temp]] != 255)
                            valileveys = xtemp;
                    }
                    if (valileveys != -1)
                        break;
                }

                char_width[temp] = valileveys - char_start[temp] + 2;
            }

        }
}

void Font::scale(void) {
    scaled = 1;

}

void Font::unscale(void) {
    scaled = 0;
}

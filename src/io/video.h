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

#ifndef VIDEO_H
#define VIDEO_H

#define VGA_MODE 0
#define SVGA_MODE 1

#include "gfx/gfx.h"
#include "sdl_compat.h"

struct naytto {
    char paletti[256][3];
    char normaalipaletti[256][3];
    char paletti_muutos;
};
extern naytto ruutu;

struct video_state_t {
    SDL_Surface *surface;
    int init_done;
    int haverealpalette;
};
extern struct video_state_t video_state;
extern SDL_Color curpal[256];

void alusta_naytto(const char *paletin_nimi);
void setpal(int vari, char R, char G, char B);
void setpal_range(const char pal[][3], int firstcolor, int n, int reverse = 0);
void fillrect(int x, int y, int w, int h, int c);
void do_all(int do_retrace = 0);
int init_vesa(const char *paletname);
void init_vga(const char *paletname);
void init_video(void);

extern unsigned char *vircr;
extern Bitmap *standard_background;
extern int current_mode;
extern int pixel_multiplier;
extern int update_vircr_mode;
extern int draw_with_vircr_mode;
extern int pixel_multiplier_vga, pixel_multiplier_svga;
extern int wantfullscreen;

#endif

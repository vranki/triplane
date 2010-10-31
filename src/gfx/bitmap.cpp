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

/*******************************************************************************

   Purpose: 
   	Bitmap handling part of Wsystem 2.0

*******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfx/bitmap.h"
#include "gfx/gfx.h"
#include "io/trip_io.h"
#include "util/wutil.h"
#include <assert.h>
#include <SDL.h>
#include <SDL_endian.h>

#define RLE_REPETITION_MARK 192

#define MAX_BITMAPS 8192
Bitmap *all_bitmaps[MAX_BITMAPS];
int all_bitmaps_n = 0;

unsigned char *pointti;

static void all_bitmaps_add(Bitmap * b) {
    if (draw_with_vircr_mode)
        return;

    assert(all_bitmaps_n < MAX_BITMAPS);
    all_bitmaps[all_bitmaps_n++] = b;
}

static void all_bitmaps_delete(Bitmap * b) {
    int i;

    if (draw_with_vircr_mode)
        return;

    for (i = 0; i < all_bitmaps_n; i++)
        if (all_bitmaps[i] == b)
            break;
    if (i < all_bitmaps_n)
        all_bitmaps[i] = all_bitmaps[--all_bitmaps_n];
}

void all_bitmaps_refresh(void) {
    int i;

    if (draw_with_vircr_mode)
        return;

    for (i = 0; i < all_bitmaps_n; i++)
        all_bitmaps[i]->refresh_sdlsurface();
}

/* Make a copy of the image data in source, enlarged zoom times */
static unsigned char *duplicate_enlarged(const unsigned char *source, int width, int height, int zoom) {
    uint8_t *target = (uint8_t *) walloc(width * height * zoom * zoom);
    int i, j, k;
    const uint8_t *in = source;
    uint8_t *out = target;

    /* optimized versions using 32-bit and 16-bit writes when possible */
    if (zoom == 4 && sizeof(char *) >= 4) {     /* word size >= 4 */
        uint32_t cccc;
        for (j = 0; j < height * zoom; j += zoom) {
            for (i = 0; i < width * zoom; i += zoom) {
                cccc = *in | (*in << 8) | (*in << 16) | (*in << 24);
                in++;
                for (k = 0; k < zoom; k++) {
                    *(uint32_t *) (&out[(j + k) * (width * zoom) + i]) = cccc;
                }
            }
        }
    } else if (zoom == 3) {
        uint16_t cc, c;
        for (j = 0; j < height * zoom; j += zoom) {
            for (i = 0; i < width * zoom; i += zoom) {
                c = *in++;
                cc = c | (c << 8);
                for (k = 0; k < zoom; k++) {
                    *(uint16_t *) (&out[(j + k) * (width * zoom) + i]) = cc;
                    out[(j + k) * (width * zoom) + i + 2] = c;
                }
            }
        }
    } else if (zoom == 2) {
        uint16_t cc;
        for (j = 0; j < height * zoom; j += zoom) {
            for (i = 0; i < width * zoom; i += zoom) {
                cc = *in | (*in << 8);
                in++;
                for (k = 0; k < zoom; k++) {
                    *(uint16_t *) (&out[(j + k) * (width * zoom) + i]) = cc;
                }
            }
        }
    } else {                    /* unoptimized version */
        int l;
        uint8_t c;
        for (j = 0; j < height * zoom; j += zoom) {
            for (i = 0; i < width * zoom; i += zoom) {
                c = *in++;
                for (k = 0; k < zoom; k++) {
                    for (l = 0; l < zoom; l++) {
                        out[(j + k) * (width * zoom) + (i + l)] = c;
                    }
                }
            }
        }
    }

    return target;
}

void Bitmap::refresh_sdlsurface() {
    unsigned char *imgmult = NULL;
    SDL_Surface *tmps;

    if (sdlsurface != NULL) {
        SDL_FreeSurface(sdlsurface);
        sdlsurface = NULL;
    }

    if (draw_with_vircr_mode)
        return;                 /* sdlsurfaces are not used */

    if (pixel_multiplier > 1) {
        imgmult = duplicate_enlarged(image_data, width, height, pixel_multiplier);
        tmps = SDL_CreateRGBSurfaceFrom(imgmult, width * pixel_multiplier, height * pixel_multiplier, 8, width * pixel_multiplier, 0, 0, 0, 0);
    } else {
        tmps = SDL_CreateRGBSurfaceFrom(image_data, width, height, 8, width, 0, 0, 0, 0);
    }

    if (tmps == NULL) {
        fprintf(stderr, "SDL_CreateRGBSurfaceFrom: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetPalette(tmps, SDL_LOGPAL, curpal, 0, 256);
    if (hastransparency)
        SDL_SetColorKey(tmps, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0xff);
    sdlsurface = SDL_DisplayFormat(tmps);
    if (sdlsurface == NULL) {
        fprintf(stderr, "SDL_DisplayFormat: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_FreeSurface(tmps);
    if (pixel_multiplier > 1) {
        wfree(imgmult);
    }
}

Bitmap::Bitmap(const char *image_name, int transparent) {
    unsigned int xx, yy, lask, lask2;
    int laskx;
    uint32_t koko;
    char nimmi[7];
    char longname[12];
    unsigned char *pointteri2;

    if (dksopen(image_name) == 0) {

        strcpy(longname, image_name);
        strcat(longname, ".pgd");
        dks_faili = fopen(longname, "rb");

        if (dks_faili == NULL) {
            printf("Error opening data %s\n", image_name);
            exit(1);
        }
    }

    dksread(&width, sizeof(width));
    dksread(&height, sizeof(height));
    dksread(&koko, sizeof(koko));
    dksread(nimmi, sizeof(nimmi));

    width = SDL_SwapLE16(width);
    height = SDL_SwapLE16(height);
    koko = SDL_SwapLE32(koko);

    xx = width;
    yy = height;


    image_data = (unsigned char *) walloc(xx * yy);
    external_image_data = 0;

    pointteri2 = (unsigned char *) walloc((unsigned int) koko);

    dksread(pointteri2, koko);
    dksclose();

    lask = 0;

    for (lask2 = 0; lask2 < (koko - 1); lask2++) {

        if (pointteri2[lask2] > RLE_REPETITION_MARK) {
            for (laskx = 1; laskx <= (pointteri2[lask2] - RLE_REPETITION_MARK); laskx++)
                image_data[lask + laskx - 1] = pointteri2[lask2 + 1];
            lask2++;
            lask += (laskx - 1);
        } else {
            image_data[lask] = pointteri2[lask2];
            lask++;

        }
    }

    if ((lask - 1) > (xx * yy)) {
        printf("Unpacking failure %d %d %s\n", lask, lask2, nimmi);
        exit(1);

    }

    free(pointteri2);

    name = image_name;
    hastransparency = transparent;
    sdlsurface = NULL;
    refresh_sdlsurface();
    all_bitmaps_add(this);
}


Bitmap::Bitmap(int width, int height, unsigned char *image_data, const char *name) {
    this->image_data = image_data;
    this->width = width;
    this->height = height;
    this->external_image_data = 1;
    this->name = name;
    this->hastransparency = 1;
    this->sdlsurface = NULL;
    refresh_sdlsurface();
    all_bitmaps_add(this);
}


Bitmap::~Bitmap() {
    all_bitmaps_delete(this);
    if (sdlsurface != NULL) {
        SDL_FreeSurface(sdlsurface);
        sdlsurface = NULL;
    }
    if (!external_image_data)
        free(image_data);
}

void Bitmap::blit_fullscreen(void) {
    assert(current_mode == VGA_MODE);
    assert(!hastransparency);
    pointti = image_data;

    if (update_vircr_mode)
        memcpy(vircr, image_data, 320 * 200);

    if (!draw_with_vircr_mode)
        SDL_BlitSurface(sdlsurface, NULL, video_state.surface, NULL);
}

/*
 * @param xx, yy target coordinate in vircr
 * @param rx, ry, rx2, ry2 clip window in circr coordinates
 */
void Bitmap::blit(int xx, int yy, int rx, int ry, int rx2, int ry2) {
    int fromminy, fromminx, frommaxy, frommaxx, bwidth;
    int xi, yi, tx, ty;
    SDL_Rect clip, pos;

    if (current_mode == SVGA_MODE) {
        if (rx == 0 && ry == 0 && rx2 == 319 && ry2 == 199) {
            rx2 = 799;
            ry2 = 599;
        }
        if (rx2 > 799)
            rx2 = 799;
        if (ry2 > 599)
            ry2 = 599;
        bwidth = 800;
    } else {
        if (rx2 > 319)
            rx2 = 319;
        if (ry2 > 199)
            ry2 = 199;
        bwidth = 320;
    }

    if (rx < 0)
        rx = 0;
    if (ry < 0)
        ry = 0;

    if ((ry > ry2) || (rx > rx2))
        return;

    if (update_vircr_mode) {
        fromminy = (yy >= ry) ? 0 : ry - yy;
        fromminx = (xx >= rx) ? 0 : rx - xx;
        frommaxy = (yy + height - 1 <= ry2) ? height - 1 : ry2 - yy;
        frommaxx = (xx + width - 1 <= rx2) ? width - 1 : rx2 - xx;

        if (fromminx <= frommaxx) {
            if (hastransparency) {
                for (yi = fromminy, ty = fromminy + yy; yi <= frommaxy; yi++, ty++) {
                    for (xi = fromminx, tx = fromminx + xx; xi <= frommaxx; xi++, tx++) {
                        unsigned char val = image_data[width * yi + xi];
                        if (val != 0xff) {
                            vircr[bwidth * ty + tx] = val;
                        }
                    }
                }
            } else {            /* can use memcpy without transparency */
                for (yi = fromminy, ty = fromminy + yy; yi <= frommaxy; yi++, ty++)
                    memcpy(&vircr[bwidth * ty + fromminx + xx], &image_data[width * yi + fromminx], frommaxx - fromminx + 1);
            }
        }
    }

    if (!draw_with_vircr_mode) {
        clip.x = rx;
        clip.y = ry;
        clip.w = rx2 - rx + 1;
        clip.h = ry2 - ry + 1;
        pos.x = xx;
        pos.y = yy;
        if (pixel_multiplier > 1) {
            clip.x *= pixel_multiplier;
            clip.y *= pixel_multiplier;
            clip.w *= pixel_multiplier;
            clip.h *= pixel_multiplier;
            pos.x *= pixel_multiplier;
            pos.y *= pixel_multiplier;
        }
        SDL_SetClipRect(video_state.surface, &clip);
        if (SDL_BlitSurface(sdlsurface, NULL, video_state.surface, &pos) != 0) {
            fprintf(stderr, "SDL_BlitSurface: %s\n", SDL_GetError());
            exit(1);
        }
        SDL_SetClipRect(video_state.surface, NULL);
    }
}

unsigned char *Bitmap::info(int *width, int *height) {
    if (width != NULL && height != NULL) {
        *width = this->width;
        *height = this->height;
    }
    return image_data;
}

Bitmap::Bitmap(int x1, int y1, int xl, int yl, Bitmap * source_image) {
    int kokox, kokoy;
    int laskx, lasky;
    unsigned char *lahtopointti;

    laskx = xl;
    lasky = yl;

    image_data = (unsigned char *) walloc(laskx * lasky);
    external_image_data = 0;

    lahtopointti = source_image->info(&kokox, &kokoy);
    width = xl;
    height = yl;

    for (lasky = y1; lasky < (y1 + yl); lasky++)
        for (laskx = x1; laskx < (x1 + xl); laskx++)
            image_data[(laskx - x1) + (lasky - y1) * width] = lahtopointti[laskx + lasky * kokox];

    name = source_image->name;
    hastransparency = source_image->hastransparency;
    sdlsurface = NULL;
    refresh_sdlsurface();
    all_bitmaps_add(this);
}

/* Create a new Bitmap from the contents of vircr at (x,y) to (x+w,y+h) */
Bitmap::Bitmap(int x, int y, int w, int h) {
    int vircrw = (current_mode == VGA_MODE) ? 320 : 800;
    int fromy, toy;

    assert(update_vircr_mode);  /* otherwise vircr may not be valid */

    width = w;
    height = h;
    image_data = (unsigned char *) walloc(w * h);
    external_image_data = 0;

    for (fromy = y, toy = 0; toy < h; fromy++, toy++)
        memcpy(&image_data[toy * w], &vircr[fromy * vircrw + x], w);

    name = "from_vircr";
    hastransparency = 0;
    sdlsurface = NULL;
    refresh_sdlsurface();
    all_bitmaps_add(this);
}

void Bitmap::blit_to_bitmap(Bitmap * to, int xx, int yy) {
    unsigned char *to_point;
    int laskx, lasky;
    int kokox, kokoy;

    to_point = to->info(&kokox, &kokoy);

    if ((xx >= kokox) | (yy >= kokoy) | (xx + width < 0) | (yy + height < 0))
        return;

    for (laskx = 0; laskx < width; laskx++)
        for (lasky = 0; lasky < height; lasky++) {
            if ((laskx + xx >= kokox) | (laskx + xx < 0))
                continue;
            if ((lasky + yy >= kokoy) | (lasky + yy < 0))
                continue;
            if (image_data[laskx + lasky * width] == 255)
                continue;

            to_point[laskx + xx + (lasky + yy) * kokox] = image_data[laskx + lasky * width];
        }

    to->refresh_sdlsurface();
}

Bitmap *rotate_bitmap(Bitmap * picture, int degrees) {
    Bitmap *picture2;
    unsigned char *picture_data;
    unsigned char *temp_data;
    unsigned char *old_picture_data;
    int xl, yl;
    int nxl, nyl;
    int count, count2;

    old_picture_data = picture->info(&xl, &yl);
    picture_data = (unsigned char *) walloc(xl * yl);

    nxl = xl << 1;
    nyl = yl << 1;

    temp_data = (unsigned char *) walloc(nxl * nyl);

    for (count = 0; count < (nxl * nyl); count++)
        temp_data[count] = 255;
    picture2 = new Bitmap(xl, yl, picture_data, "rotated");

    for (count = 0; count < xl; count++)
        for (count2 = 0; count2 < yl; count2++)
            temp_data[(xl / 2 + count) + (yl / 2 + count2) * nxl] = old_picture_data[count + count2 * xl];

    for (count = (-(xl >> 1)); count < (xl >> 1); count++)
        for (count2 = (-(yl >> 1)); count2 < (yl >> 1); count2++) {

            picture_data[(xl >> 1) + count + ((yl >> 1) + count2) * xl]
                = temp_data[(xl + (((count * cosinit[degrees]) - (count2 * sinit[degrees]) + 128) >> 8))
                            + (yl + (((count * sinit[degrees]) + (count2 * cosinit[degrees]) + 128) >> 8)) * nxl];
        }
    free(temp_data);

    picture2->refresh_sdlsurface();
    return picture2;
}

int bitmap_exists(const char *name) {
    char longname[80];

    if (dksopen(name)) {
        dksclose();
        return 1;
    }

    strcpy(longname, name);
    strcat(longname, ".pgd");
    dks_faili = fopen(longname, "rb");
    if (dks_faili == NULL) {
        return 0;
    } else {
        dksclose();
        return 1;
    }
}

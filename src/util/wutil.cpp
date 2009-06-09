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

#include "wutil.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "triplane.h"
#include "io/dksfile.h"
#include "random.h"
#include <SDL.h>
#include <SDL_endian.h>

#define SIN_COS_NRO 361
#define ASIN_NRO 257

int32_t cosinit[SIN_COS_NRO];
int32_t sinit[SIN_COS_NRO];
int32_t asinit[ASIN_NRO];

static int trigs_initialized = 0;

void setwrandom(int seed) {
    triplane_srandom(seed);
}

int wrandom(int limit) {
    return ((triplane_random() % limit));
}

static int wrandom_test_data[] = {
    0x0029187a,
    0x1e2582ce,
    0x5ad57f9a,
    0x11a8b5fc,
    0x5f5d7d81,
    0x0043f46b,
    0x1dc84952,
    0x31b69ea3,
    0x6f6e3e14,
    0x733318dd,
    0x05dbeb00,
    0x08151371,
    0x61d2c131,
    0x3e04fb7f,
    0x5ef51817,
    0x0d53f52d,
    0x4a7ca460,
    0x4dc8160f,
    0x4812493d,
    0x1ed40d5f,
    0x053b721c,
    0x5309d13a,
    0x63d17356,
    0x79d39300,
    0x473ada6d,
    0x0aa7fa76,
    0x00ebaabc,
    0x0771c671,
    0x179951d1,
    0x1ae3430a,
    0x57b136ca,
    0x79bbd8ad
};

void wrandom_sanity_check(void) {
    int i;

    setwrandom(7);
    for (i = 0; i < 32; i++) {
        int ret;
        ret = wrandom(2147483647);
        if (ret != wrandom_test_data[i]) {
            printf("Internal error. wrandom returned 0x%08x but wrandom_test_data[%d] has 0x%08x\n", ret, i, wrandom_test_data[i]);
            exit(1);
        }
    }
}

void *walloc(size_t size) {
    void *ptr;
    ptr = malloc(size);

    if (ptr == NULL) {
        printf("\nError in memory allocation: %d bytes.\n", (int) size);
        exit(1);

    }

    return ptr;
}


void wfree(void *ptr) {
    assert(ptr != NULL);
    free(ptr);
}

void wtoggle(int *what) {
    if (*what)
        *what = 0;
    else
        *what = 1;

}

void init_trigs(void) {
    int i;

    dksopen("trigdt");
    dksread(sinit, 1444);
    dksread(cosinit, 1444);
    dksclose();

    dksopen("arcsin");
    dksread(asinit, 1028);
    dksclose();

    for (i = 0; i < SIN_COS_NRO; i++) {
        sinit[i] = SDL_SwapLE32(sinit[i]);
        cosinit[i] = SDL_SwapLE32(cosinit[i]);
    }

    for (i = 0; i < ASIN_NRO; i++) {
        asinit[i] = SDL_SwapLE32(asinit[i]);
    }

    trigs_initialized = 1;
}

int arcsinit(int luku) {
    assert(trigs_initialized);

    if (luku <= 0)
        luku = 0;

    if (luku > 256)
        luku = 256;

    return asinit[luku];
}

// #1 is the observer and #2 is the target
void calculate_difference(int x1, int y1, int x2, int y2, int *distance, int *angle) {
    int xdiff, ydiff;

    xdiff = x2 - x1;            // negative is left
    ydiff = y1 - y2;            // negative is down

    if (!xdiff) {
        *distance = abs(ydiff);

        if (angle == NULL)
            return;
        if (ydiff > 0)
            *angle = 90;
        else
            *angle = 270;

        return;
    }

    if (!ydiff) {
        *distance = abs(xdiff);

        if (angle == NULL)
            return;
        if (xdiff > 0)
            *angle = 0;
        else
            *angle = 180;

        return;
    }

    *distance = squareroot(xdiff * xdiff + ydiff * ydiff);
    if (*distance == 0) {
        *angle = 0;
        *distance = 1;
    }
    if (angle == NULL)
        return;

    if (xdiff > 0 && ydiff > 0) {
        *angle = arcsinit((ydiff << 8) / (*distance));
        return;
    }

    if (xdiff > 0 && ydiff < 0) {
        *angle = 360 - arcsinit((-(ydiff << 8)) / (*distance));
        if (*angle == 360)
            *angle = 0;
        return;
    }

    if (xdiff < 0 && ydiff > 0) {
        *angle = 180 - arcsinit((ydiff << 8) / (*distance));
        return;
    }

    if (xdiff < 0 && ydiff < 0) {
        *angle = 180 + arcsinit((-(ydiff << 8)) / (*distance));
        return;
    }
}

int squareroot(int number) {
    int l, old_result, new_result;

    if (!(old_result = new_result = number >> 1))
        return 1;

    for (l = 0; l < 14; l++) {
        new_result = (old_result + number / old_result + 1) >> 1;
        if (new_result == old_result || !new_result)
            return new_result;

        old_result = new_result;

    }

    return new_result;

}

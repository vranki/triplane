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
#include <stdio.h>
#include "io/sdl_compat.h"
#include <SDL.h>
#include <SDL_endian.h>
#include <string.h>

#define MAX_STRUCTURES 100
#define MAX_TYPES 3
#define AIRFIELD_LENGHT 110

#define MIN_AIRFIELD_LENGHT 40
#define MAX_AIRFIELD_LENGHT 200

struct level_struct {
    char pb_name[8];
    char pd_name[MAX_STRUCTURES][8];
    int32_t struct_x[MAX_STRUCTURES];
    int32_t struct_y[MAX_STRUCTURES];
    int32_t struct_hit[MAX_STRUCTURES];
    int32_t struct_owner[MAX_STRUCTURES];
    int32_t struct_type[MAX_STRUCTURES];
    int32_t airfield_x[4];
    int32_t airfield_y[4];
    int32_t airfield_lenght[4];
    int32_t plane_direction[4];
} leveldata;

Bitmap *pb_picture;
Bitmap *struct_picture[MAX_STRUCTURES];
Bitmap *hiirikursori = NULL;
Bitmap *hiirik;
Bitmap *lappu1;
Bitmap *lappu2;
//Bitmap *plane[4];

int picture_width[MAX_STRUCTURES];
int picture_height[MAX_STRUCTURES];

Font *fontti;

const char hitting_names[2][16] = {
    "UNHITTABLE", "HITTABLE"
};


const char owner_names[5][12] = {
    "GERMAN", "FINNISH", "ENGLISH", "JAPANESE", "NEUTRAL"
};

char filename[80];

const char type_names[MAX_TYPES][16] = {
    "Decoration",
    "Dest",
    "Survi"
};

void swap_byte_order(void) {
    int i;

    for (i = 0; i < MAX_STRUCTURES; i++) {
        leveldata.struct_x[i] = SDL_SwapLE32(leveldata.struct_x[i]);
        leveldata.struct_y[i] = SDL_SwapLE32(leveldata.struct_y[i]);
        leveldata.struct_hit[i] = SDL_SwapLE32(leveldata.struct_hit[i]);
        leveldata.struct_owner[i] = SDL_SwapLE32(leveldata.struct_owner[i]);
        leveldata.struct_type[i] = SDL_SwapLE32(leveldata.struct_type[i]);
    }

    for (i = 0; i < 4; i++) {
        leveldata.airfield_x[i] = SDL_SwapLE32(leveldata.airfield_x[i]);
        leveldata.airfield_y[i] = SDL_SwapLE32(leveldata.airfield_y[i]);
        leveldata.airfield_lenght[i] = SDL_SwapLE32(leveldata.airfield_lenght[i]);
        leveldata.plane_direction[i] = SDL_SwapLE32(leveldata.plane_direction[i]);
    }
}

void omaline(int x1, int y1, int x2, int y2, unsigned char c) {
    if (x1 >= 320 || x2 < 0)
        return;

    if (y1 >= 320 || y2 < 0)
        return;

    if (x1 < 0)
        x1 = 0;

    if (x2 >= 320)
        x2 = 319;

    if (y1 < 0)
        y1 = 0;

    if (y2 >= 200)
        y2 = 199;

    draw_line(x1, y1, x2, y2, c);
}

void edit_level(void) {
    int ch = 0;
    int kohta = 0;
    int c;
    int x, y, n1, n2;
    int current = -1;
    int active = -1;
    int airfield_set = -1;
    unsigned char colori = 0;
    FILE *faili;

    Bitmap *pointti;
    pointti = new Bitmap(1, 1, &colori);

    while (!(ch == SDLK_ESCAPE)) {
        colori++;
        pb_picture->blit(-kohta, 0);
        for (c = 0; c < MAX_STRUCTURES; c++) {
            if (leveldata.struct_x[c]) {
                struct_picture[c]->blit(leveldata.struct_x[c] - kohta, leveldata.struct_y[c]);
            }
        }

        if (active != -1) {
            if (leveldata.struct_x[active] >= kohta && leveldata.struct_x[active] <= (kohta + 300))

                boxi(leveldata.struct_x[active] - kohta,
                     leveldata.struct_y[active],
                     leveldata.struct_x[active] - kohta + picture_width[active], leveldata.struct_y[active] + picture_height[active], 1);
        }

        for (c = 0; c < 4; c++)
            if (leveldata.airfield_x[c])
                omaline(leveldata.airfield_x[c] - kohta, leveldata.airfield_y[c],
                        leveldata.airfield_x[c] - kohta + leveldata.airfield_lenght[c], leveldata.airfield_y[c], colori);


        if (active != -1) {
            lappu1->blit(220, 0);
            fontti->printf(235, 4, "%s", leveldata.pd_name[active]);
            fontti->printf(235, 14, "X:%d Y:%d", leveldata.struct_x[active], leveldata.struct_y[active]);
            fontti->printf(235, 23, "%s", owner_names[leveldata.struct_owner[active]]);
            fontti->printf(235, 32, "%s", hitting_names[leveldata.struct_hit[active]]);
            fontti->printf(235, 41, "%s", type_names[leveldata.struct_type[active]]);
        }

        lappu2->blit(262, 184);
        fontti->printf(265, 188, "%s", leveldata.pb_name);

        koords(&x, &y, &n1, &n2);
        hiirikursori->blit(x, y);

        if (airfield_set != -1 && (n1 || n2)) {
            hiirikursori = hiirik;
            leveldata.airfield_x[airfield_set] = x + kohta;
            leveldata.airfield_y[airfield_set] = y;
            if (n1)
                leveldata.plane_direction[airfield_set] = 0;
            else
                leveldata.plane_direction[airfield_set] = 1;
            airfield_set = -1;
        }

        if (n1 && (current != -1)) {
            leveldata.struct_x[current] = x + kohta;
            leveldata.struct_y[current] = y;
            hiirikursori = hiirik;
            current = -1;
        } else if (n1 && (active == -1)) {
            for (c = 0; c < MAX_STRUCTURES; c++) {
                if ((x + kohta) >= leveldata.struct_x[c] &&
                    (x + kohta) <= (leveldata.struct_x[c] + picture_width[c]) && y >= leveldata.struct_y[c] && y <= (leveldata.struct_y[c] + picture_height[c]))
                    break;

            }

            if (c != MAX_STRUCTURES) {
                active = c;

            }
        }


        if (n2 && (active != -1)) {
            active = -1;
        }

        do_all();
        //while(!kbhit());

        if (kbhit()) {
            ch = getch();

            switch (ch) {
            case SDLK_F2:
                swap_byte_order();
                faili = fopen(filename, "wb");
                fwrite(&leveldata, sizeof(leveldata), 1, faili);
                fclose(faili);
                swap_byte_order();
                break;

            case SDLK_INSERT:
                if (active != -1)
                    break;

                for (c = 0; c < MAX_STRUCTURES; c++)
                    if (!leveldata.struct_x[c])
                        break;
                if (c == MAX_STRUCTURES)
                    break;

                fontti->printf(10, 10, "StructureNAME");
                fontti->scanf(10, 20, leveldata.pd_name[c], 6);
                if (!leveldata.pd_name[c][0])
                    break;
                if (!bitmap_exists(leveldata.pd_name[c]))
                    break;

                struct_picture[c] = new Bitmap(leveldata.pd_name[c]);
                hiirikursori = struct_picture[c];
                hiirikursori->info(&picture_width[c], &picture_height[c]);
                current = c;
                break;

            case SDLK_F5:
                if (active != -1)
                    break;

                for (c = 0; c < MAX_STRUCTURES; c++)
                    if (!leveldata.struct_x[c])
                        break;

                if (c == MAX_STRUCTURES)
                    break;

                strcpy(leveldata.pd_name[c], "INFAN");

                if (!bitmap_exists(leveldata.pd_name[c]))
                    break;

                leveldata.struct_owner[c] = 0;
                leveldata.struct_type[c] = 0;
                struct_picture[c] = new Bitmap(leveldata.pd_name[c]);
                hiirikursori = struct_picture[c];
                hiirikursori->info(&picture_width[c], &picture_height[c]);
                current = c;
                break;

            case SDLK_F6:
                if (active != -1)
                    break;

                for (c = 0; c < MAX_STRUCTURES; c++)
                    if (!leveldata.struct_x[c])
                        break;
                if (c == MAX_STRUCTURES)
                    break;

                strcpy(leveldata.pd_name[c], "INFAN");

                if (!bitmap_exists(leveldata.pd_name[c]))
                    break;

                leveldata.struct_owner[c] = 1;
                leveldata.struct_type[c] = 0;
                struct_picture[c] = new Bitmap(leveldata.pd_name[c]);
                hiirikursori = struct_picture[c];
                hiirikursori->info(&picture_width[c], &picture_height[c]);
                current = c;
                break;

            case SDLK_F7:
                if (active != -1)
                    break;

                for (c = 0; c < MAX_STRUCTURES; c++)
                    if (!leveldata.struct_x[c])
                        break;
                if (c == MAX_STRUCTURES)
                    break;

                strcpy(leveldata.pd_name[c], "INFAN");

                if (!bitmap_exists(leveldata.pd_name[c]))
                    break;

                leveldata.struct_owner[c] = 2;
                leveldata.struct_type[c] = 0;
                struct_picture[c] = new Bitmap(leveldata.pd_name[c]);
                hiirikursori = struct_picture[c];
                hiirikursori->info(&picture_width[c], &picture_height[c]);
                current = c;
                break;

            case SDLK_F8:
                if (active != -1)
                    break;

                for (c = 0; c < MAX_STRUCTURES; c++)
                    if (!leveldata.struct_x[c])
                        break;
                if (c == MAX_STRUCTURES)
                    break;

                strcpy(leveldata.pd_name[c], "INFAN");

                if (!bitmap_exists(leveldata.pd_name[c]))
                    break;

                leveldata.struct_owner[c] = 3;
                leveldata.struct_type[c] = 0;
                struct_picture[c] = new Bitmap(leveldata.pd_name[c]);
                hiirikursori = struct_picture[c];
                hiirikursori->info(&picture_width[c], &picture_height[c]);
                current = c;
                break;


            case SDLK_DELETE:
                if (active != -1) {
                    leveldata.struct_x[active] = 0;
                    delete struct_picture[active];
                    active = -1;

                }
                break;

            case SDLK_HOME:
                if (active != -1) {
                    leveldata.struct_owner[active]++;
                    if (leveldata.struct_owner[active] >= 5)
                        leveldata.struct_owner[active] = 0;

                }
                break;

            case SDLK_END:
                if (active != -1) {
                    leveldata.struct_hit[active]++;
                    if (leveldata.struct_hit[active] >= 2)
                        leveldata.struct_hit[active] = 0;
                }
                break;
            case SDLK_RIGHT:
                kohta += 106;
                if (kohta > 2080)
                    kohta = 2080;
                break;

            case SDLK_LEFT:
                kohta -= 106;
                if (kohta < 0)
                    kohta = 0;
                break;

            case SDLK_PAGEUP:
                char temp_stringi[10];
                temp_stringi[0] = 0;
                fontti->scanf(265, 178, temp_stringi, 6);

                if (!bitmap_exists(temp_stringi))
                    break;


                strcpy(leveldata.pb_name, temp_stringi);
                delete pb_picture;
                pb_picture = new Bitmap(temp_stringi);
                break;

            case SDLK_PAGEDOWN:
                if (active != -1) {
                    leveldata.struct_type[active]++;
                    if (leveldata.struct_type[active] >= MAX_TYPES)
                        leveldata.struct_type[active] = 0;
                }
                break;
            case SDLK_q:
                leveldata.airfield_lenght[0]++;
                if (leveldata.airfield_lenght[0] > MAX_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[0] = MAX_AIRFIELD_LENGHT;
                break;
            case SDLK_w:
                leveldata.airfield_lenght[1]++;
                if (leveldata.airfield_lenght[1] > MAX_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[1] = MAX_AIRFIELD_LENGHT;
                break;
            case SDLK_e:
                leveldata.airfield_lenght[2]++;
                if (leveldata.airfield_lenght[2] > MAX_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[2] = MAX_AIRFIELD_LENGHT;
                break;
            case SDLK_r:
                leveldata.airfield_lenght[3]++;
                if (leveldata.airfield_lenght[3] > MAX_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[3] = MAX_AIRFIELD_LENGHT;
                break;

            case SDLK_a:
                leveldata.airfield_lenght[0]--;
                if (leveldata.airfield_lenght[0] < MIN_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[0] = MIN_AIRFIELD_LENGHT;
                break;

            case SDLK_s:
                leveldata.airfield_lenght[1]--;
                if (leveldata.airfield_lenght[1] < MIN_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[1] = MIN_AIRFIELD_LENGHT;
                break;

            case SDLK_d:
                leveldata.airfield_lenght[2]--;
                if (leveldata.airfield_lenght[2] < MIN_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[2] = MIN_AIRFIELD_LENGHT;
                break;

            case SDLK_f:
                leveldata.airfield_lenght[3]--;
                if (leveldata.airfield_lenght[3] < MIN_AIRFIELD_LENGHT)
                    leveldata.airfield_lenght[3] = MIN_AIRFIELD_LENGHT;
                break;

            case SDLK_PLUS:
                if (active == -1 || active == 99)
                    break;

                if (leveldata.struct_x[active + 1]);
                active++;
                break;

            case SDLK_MINUS:
                if (active < 1)
                    break;

                if (leveldata.struct_x[active - 1]);
                active--;
                break;
            default:
                if ((ch >= SDLK_1) && (ch <= SDLK_4)) {
                    airfield_set = ch - SDLK_1;
                    hiirikursori = pointti;
                }
                break;
            }
        }
    }

    delete pointti;

}

int main(int argc, char **argv) {

    FILE *faili;
    int c;

    printf("\n");
    printf(" Lvledit for Triplane   Copyright (c) 1996 Dodekaedron Software Creations, Inc.");
    printf("\n\r\n\r");

    if (argc != 2) {
        printf("Usage: LVLEDIT level_filename\n");
        exit(1);
    }

    printf("Loading directory structure\n\r");
    dksinit("fokker.dks");

    strcpy(filename, argv[1]);

    if ((faili = fopen(filename, "rb")) == NULL) {
        printf("Creating new level.\n\r");

        strcpy(leveldata.pb_name, "MAISDT");
        for (c = 0; c < MAX_STRUCTURES; c++) {
            leveldata.struct_x[c] = 0;
            leveldata.struct_y[c] = 0;
            leveldata.struct_hit[c] = 0;
            leveldata.pd_name[c][0] = 0;
            leveldata.struct_owner[c] = 4;
            leveldata.struct_type[c] = 0;



        }
        for (c = 0; c < 4; c++) {
            leveldata.airfield_x[c] = 0;
            leveldata.airfield_y[c] = 0;
            leveldata.airfield_lenght[c] = 110;
            leveldata.plane_direction[c] = 0;
        }

    } else {
        printf("Loading existing level.\n\r");
        fread(&leveldata, sizeof(leveldata), 1, faili);
        fclose(faili);

        for (c = 0; c < MAX_STRUCTURES; c++) {

            if (leveldata.struct_x[c]) {
                struct_picture[c] = new Bitmap(leveldata.pd_name[c]);
                struct_picture[c]->info(&picture_width[c], &picture_height[c]);
            }
        }
    }

    printf("Loading font \"FROST\"\n\r");
    fontti = new Font("G2FONT");

    printf("Loading cursor\n\r");
    hiirik = new Bitmap("FONTT1");
    hiirikursori = hiirik;

    if (leveldata.pb_name[0]) {
        printf("Loading background picture\n\r");

        pb_picture = new Bitmap(leveldata.pb_name);

    }

    printf("Loading background papers\n\r");

    lappu1 = new Bitmap("LAPPU1");
    lappu2 = new Bitmap("LAPPU2");

    init_vga("PALET5");

    edit_level();

    swap_byte_order();
    faili = fopen(filename, "wb");
    fwrite(&leveldata, sizeof(leveldata), 1, faili);
    fclose(faili);
    swap_byte_order();

    delete pb_picture;
    delete lappu1;
    delete lappu2;
    delete hiirik;
    delete fontti;
    for (c = 0; c < MAX_STRUCTURES; c++)
        if (leveldata.struct_x[c])
            delete struct_picture[c];

    return 0;

}

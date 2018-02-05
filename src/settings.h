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

#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>

#define ROSTER_FILENAME "roster.dta"
#define MAX_PLAYERS_IN_ROSTER 150
#define CONFIGURATION_FILENAME "triplane.cfg"
#define KEYSET_FILENAME "keyset.dta"

struct keymap {
    int32_t up;
    int32_t down;
    int32_t roll;
    int32_t power;
    int32_t guns;
    int32_t bombs;
};

extern keymap player_keys[4];

//\\ Rosterdata

#define ROSTERI_MAGIC 0x5a409187
#define ROSTERI_VERSION 0

struct rosteri_header {
    uint32_t magic;
    uint32_t version;
};

struct rosteri {
    char pilotname[32];
    int32_t solo_mis_flown;
    int32_t solo_mis_success;
    int32_t solo_mis_drops;
    int32_t solo_mis_shotsf;
    int32_t solo_mis_shotshit;
    int32_t solo_mis_bombs;
    int32_t solo_mis_bombshit;
    int32_t solo_mis_totals;
    int32_t solo_mis_dropped;
    int32_t solo_mis_scores[4][7];

    int32_t multi_mis_flown;
    int32_t multi_mis_success;
    int32_t multi_mis_drops;
    int32_t multi_mis_shotsf;
    int32_t multi_mis_shotshit;
    int32_t multi_mis_bombs;
    int32_t multi_mis_bombshit;
    int32_t multi_mis_totals;
    int32_t multi_mis_dropped;

    int32_t up;
    int32_t down;
    int32_t roll;
    int32_t power;
    int32_t guns;
    int32_t bombs;
};

struct dos_roster {
    char pilotname[32];
    int32_t solo_mis_flown;
    int32_t solo_mis_success;
    int32_t solo_mis_drops;
    int32_t solo_mis_shotsf;
    int32_t solo_mis_shotshit;
    int32_t solo_mis_bombs;
    int32_t solo_mis_bombshit;
    int32_t solo_mis_totals;
    int32_t solo_mis_dropped;
    int32_t solo_mis_scores[4][7];

    int32_t multi_mis_flown;
    int32_t multi_mis_success;
    int32_t multi_mis_drops;
    int32_t multi_mis_shotsf;
    int32_t multi_mis_shotshit;
    int32_t multi_mis_bombs;
    int32_t multi_mis_bombshit;
    int32_t multi_mis_totals;
    int32_t multi_mis_dropped;

    uint8_t up;
    uint8_t down;
    uint8_t roll;
    uint8_t power;
    uint8_t guns;
    uint8_t bombs;
};


extern rosteri roster[MAX_PLAYERS_IN_ROSTER];

struct configuration {
    int32_t current_multilevel;
    int32_t player_type[4];
    int32_t player_number[4];

    // Graphics
    int32_t shots_visible;
    int32_t it_shots_visible;
    int32_t aa_mg_shots_visible;
    int32_t flags;
    int32_t flames;
    int32_t structure_smoke;
    int32_t svga;

    // Audio
    int32_t sound_on;
    int32_t music_on;
    int32_t sfx_on;
    int32_t explosion_sounds;
    int32_t gunshot_sounds;
    int32_t ground_i_sounds;
    int32_t splash;
    int32_t infantry_sounds;

    // General Flying
    int32_t poweronoff;
    int32_t powerrev;

    // Multiplayer Game
    int32_t all_planes_are;
    int32_t nocollision;
    int32_t partcollision;
    int32_t stop;
    int32_t alliance;
    int32_t aa_mgs;
    int32_t it_guns;
    int32_t infantry;
    int32_t unlimited_ammo;
    int32_t unlimited_gas;

    int32_t joystick[4];
    int32_t joystick_calibrated[4];


};

extern configuration config;

void load_keyset(void);
void save_keyset(void);
int select_key(int player, int old);
void wait_relase(void);

void load_roster(void);
void save_roster(void);

void load_config(void);
void save_config(void);

FILE *settings_open(const char *filename, const char *mode);
#endif

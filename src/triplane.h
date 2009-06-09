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

/* Main headerfile */

#ifndef TRIPLANE_H
#define TRIPLANE_H

#include <SDL.h>
#include "world/constants.h"
#include "io/sdl_compat.h"
#include "gfx/gfx.h"
#include "io/video.h"
#include "settings.h"

#define DKS_FILENAME "fokker.dks"
#define CALIBRATION_FILENAME "joystic.dta"

//\ Picturedata

extern Bitmap *board;
extern Bitmap *board2;
extern Bitmap *pwon;
extern Bitmap *pwoff;
extern Bitmap *picons[4];
extern Bitmap *plane1;
extern Bitmap *planes[16][61][4][2];
extern Bitmap *bomb[61];
extern Bitmap *boards[4];
extern Bitmap *closed;
extern Bitmap *bomb_icon;
extern Bitmap *gas_icon;
extern Bitmap *small_ammo_icon;
extern Bitmap *big_ammo_icon;
extern Bitmap *plane_crash[6];
extern Bitmap *smoke[SMOKE_FRAMES];
extern Bitmap *wave1[WAVE1_FRAMES];
extern Bitmap *wave2[WAVE2_FRAMES];
extern Bitmap *explox[4][EXPLOX_FRAMES];
extern Bitmap *maisema;
extern Bitmap *bites[NUMBER_OF_BITES];
extern Bitmap *menu1;
extern Bitmap *structures[MAX_STRUCTURES][2];
extern Bitmap *temp_bitti;
extern Bitmap *cursor;
extern Bitmap *kkbase[2][3][7];
extern Bitmap *infantry_walking[4][2][12];
extern Bitmap *infantry_dying[4][2][7];
extern Bitmap *infantry_aiming[4][2][6];
extern Bitmap *infantry_wavedeath[4][2][10];
extern Bitmap *infantry_shooting[4][2][6];
extern Bitmap *infantry_bdying[4][2][10];
extern Bitmap *infantry_dropping[4][2];
extern Bitmap *infantry_after_drop[4][2];
extern Bitmap *itexplosion[ITEXPLOSION_FRAMES];
extern Bitmap *flames[6];
extern Bitmap *status_icons[2][2];
extern Bitmap *hangarmenu;
extern Bitmap *hangaractive, *hangarinactive;
extern Bitmap *radar[4][8];
extern Bitmap *rifle[12];
extern Bitmap *hruks;
extern Bitmap *ssmoke[17];
extern Bitmap *ovi[13];
extern Bitmap *mekan_running[14][2];
extern Bitmap *mekan_pushing[2][14][2];

extern int hangar_x[4];
extern int hangar_y[4];
extern int hangar_door_frame[4];
extern int mekan_x[4];
extern int mekan_y[4];
extern int mekan_frame[4];
extern int mekan_status[4];
extern int mekan_target[4];
extern int mekan_subtarget[4];
extern int mekan_direction[4];
extern int hangar_door_opening[4];
extern int hangar_door_closing[4];
extern int mekan_mission[4];
extern int plane_wants_in[16];
extern int plane_wants_out[16];

extern int player_exists[16];
extern int plane_present[16];
extern int player_sides[16];
extern int player_tsides[16];
extern int plane_coming[16];

extern int playing_solo;
extern int solo_country;
extern int solo_mission;

//\ struct_information:

extern int struct_state[MAX_STRUCTURES];
extern int struct_width[MAX_STRUCTURES];
extern int struct_heigth[MAX_STRUCTURES];

//\ Fonts

extern Font *fontti;
extern Font *frost;
extern Font *grid2;

//\ Parameter control

extern char parametrit[40][40];
extern int parametri_kpl;

//\ Shots control

extern int pohja;
extern int play_shot[16];
extern int shots_flying_x[MAX_SHOTS];
extern int shots_flying_y[MAX_SHOTS];
extern int shots_flying_x_speed[MAX_SHOTS];
extern int shots_flying_y_speed[MAX_SHOTS];
extern int shots_flying_owner[MAX_SHOTS];
extern int shots_flying_age[MAX_SHOTS];
extern int shots_flying_infan[MAX_SHOTS];

//\ Player planes

extern int in_closing[16];
extern int player_shown_x[16];
extern int player_shown_y[16];
extern unsigned char *plane_p[16][61][4][2];
extern int hangarmenu_active[16];
extern int hangarmenu_position[16];
extern int hangarmenu_gas[16];
extern int hangarmenu_ammo[16];
extern int hangarmenu_bombs[16];

extern int hangarmenu_max_gas[16];
extern int hangarmenu_max_ammo[16];
extern int hangarmenu_max_bombs[16];


//\ Graphics locating

extern int x_muutos[4];
extern int y_muutos[4];
extern int x1_raja[4];
extern int y1_raja[4];
extern int x2_raja[4];
extern int y2_raja[4];

//\ Airfields

extern int player_on_airfield[16];

//\ General

extern int collision_detect;
extern int part_collision_detect;
extern int end_game_after;
extern int power_reverse;
extern int power_on_off;
extern int loading_texts;
extern int solo_mode;
extern int aftermath;

//\ Flying objects control

struct flying_objects_data {
    int x;
    int y;
    int x_speed;
    int y_speed;
    int hit_plane;
    int width;
    int height;
    int type;
    int subtype;
    int phase;
    int owner;

};

extern flying_objects_data fobjects[MAX_FLYING_OBJECTS];

//\ Scoring data

extern int player_fired[16];
extern int player_hits[16];
extern int player_shots_down[16][16];

extern int player_bombed[16];
extern int player_bomb_hits[16];

//\ Leveldata

extern char levelname[80];

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
};

extern level_struct leveldata;

//\ Bombs

extern int bomb_x[MAX_BOMBS];
extern int bomb_y[MAX_BOMBS];
extern int bomb_speed[MAX_BOMBS];
extern int bomb_angle[MAX_BOMBS];
extern int bomb_owner[MAX_BOMBS];
extern int bomb_x_speed[MAX_BOMBS];
extern int bomb_y_speed[MAX_BOMBS];

extern int roll_key_down[16];
extern int plane_tire_y;

extern int flags_state[MAX_FLAGS];
extern int flags_frame[MAX_FLAGS];
extern int flags_x[MAX_FLAGS];
extern int flags_y[MAX_FLAGS];
extern int flags_owner[MAX_FLAGS];
extern Bitmap *flags[4][12];

//\\ AA-MG && AA-Gun

extern int kkbase_x[MAX_AA_GUNS];
extern int kkbase_y[MAX_AA_GUNS];
extern int kkbase_last_shot[MAX_AA_GUNS];
extern int kkbase_shot_number[MAX_AA_GUNS];
extern int kkbase_frame[MAX_AA_GUNS];
extern int kkbase_status[MAX_AA_GUNS];
extern int kkbase_country[MAX_AA_GUNS];
extern int kkbase_type[MAX_AA_GUNS];
extern int kkbase_mission[MAX_AA_GUNS];
extern int kkbase_number[MAX_AA_GUNS];

//\\ Computer players

extern int computer_active[16];
extern int going_left;
extern int going_up;
extern int terrain_level[2400];
extern int wide_terrain_level[2400];
extern int current_mission[16];
extern int mission_phase[16];
extern int mission_target[16];

extern int distances[16];
extern int angles[16];

extern int bombs_going[100];

//\\ Configuration

extern short joystick_exists;



extern int modem_com_port;
extern int modem_com_speed;

extern unsigned char *level_bitmap;
extern int current_mode;

extern int level_loaded;

extern int solo_failed;
extern int solo_success;
extern int solo_dest_remaining;

extern int status_frames;
extern int status_state;

extern char mission_names[24][30];

extern int mission_re_fly;

extern int mission_duration;

extern int modem_port_open;
extern int modem_master;


extern int mc_up[16];
extern int mc_down[16];
extern int mc_bomb[16];
extern int mc_roll[16];
extern int mc_guns[16];
extern int mc_power[16];

extern int main_version;
extern int sub_version;

extern int main_engine_random_seed;

/***************************** Functions **************************************/

extern int small_warning(const char *message);
extern int findparameter(const char *jono);
extern void kangas_terrain_to_screen(int leftx);
extern void main_engine(void);
extern void do_aftermath(int show_it_all);
extern void clear_level(void);
extern void init_player(int l, int pommit = 1);
extern void cause_damage(int amount, int plane);
extern void do_flags(void);


void loading_text(const char *);

#endif

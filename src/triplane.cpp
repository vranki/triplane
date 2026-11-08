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

//\\\\ Includes

#include "gfx/gfx.h"
#include "io/joystick.h"
#include "io/sdl_compat.h"
#include "io/trip_io.h"
#include "menus/tripmenu.h"
#include "settings.h"
#include "triplane.h"
#include "util/wutil.h"
#include "world/fobjects.h"
#include "world/plane.h"
#include "world/terrain.h"
#include "world/tmexept.h"
#include "world/tripmis.h"
#include "world/tripaudio.h"
#include "world/tripai.h"
#include <SDL3/SDL.h>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <memory>
#include <array>

//\\\\ Variables

//\ Picturedata

std::unique_ptr<Bitmap> board;
std::unique_ptr<Bitmap> board2;
std::unique_ptr<Bitmap> pwon;
std::unique_ptr<Bitmap> pwoff;
std::unique_ptr<Bitmap> picons[4];
std::unique_ptr<Bitmap> plane1;
std::shared_ptr<Bitmap> planes[16][61][4][2];
std::unique_ptr<Bitmap> bomb[61];
std::unique_ptr<Bitmap> boards[4];
std::unique_ptr<Bitmap> closed;
std::unique_ptr<Bitmap> bomb_icon;
std::unique_ptr<Bitmap> gas_icon;
std::unique_ptr<Bitmap> small_ammo_icon;
std::unique_ptr<Bitmap> big_ammo_icon;
std::unique_ptr<Bitmap> plane_crash[6];
std::unique_ptr<Bitmap> smoke[SMOKE_FRAMES];
std::unique_ptr<Bitmap> wave1[WAVE1_FRAMES];
std::unique_ptr<Bitmap> wave2[WAVE2_FRAMES];
std::unique_ptr<Bitmap> explox[4][EXPLOX_FRAMES];
std::unique_ptr<Bitmap> maisema;
std::unique_ptr<Bitmap> bites[NUMBER_OF_BITES];
std::unique_ptr<Bitmap> menu1;
std::unique_ptr<Bitmap> structures[MAX_STRUCTURES][2];
std::unique_ptr<Bitmap> temp_bitti;
std::unique_ptr<Bitmap> cursor;
std::unique_ptr<Bitmap> kkbase[2][3][7];
std::unique_ptr<Bitmap> infantry_walking[4][2][12];
std::unique_ptr<Bitmap> infantry_dying[4][2][7];
std::unique_ptr<Bitmap> infantry_aiming[4][2][6];
std::unique_ptr<Bitmap> infantry_wavedeath[4][2][10];
std::unique_ptr<Bitmap> infantry_shooting[4][2][6];
std::unique_ptr<Bitmap> infantry_bdying[4][2][10];
std::unique_ptr<Bitmap> infantry_dropping[4][2];
std::unique_ptr<Bitmap> infantry_after_drop[4][2];
std::unique_ptr<Bitmap> itexplosion[ITEXPLOSION_FRAMES];
std::unique_ptr<Bitmap> flames[6];
std::unique_ptr<Bitmap> status_icons[2][2];
std::unique_ptr<Bitmap> hangarmenu;
std::unique_ptr<Bitmap> hangaractive;
std::unique_ptr<Bitmap> hangarinactive;
std::unique_ptr<Bitmap> radar[4][8];
std::unique_ptr<Bitmap> rifle[12];
std::unique_ptr<Bitmap> hruks;
std::unique_ptr<Bitmap> ssmoke[17];
std::unique_ptr<Bitmap> ovi[13];
std::unique_ptr<Bitmap> mekan_running[14][2];
std::unique_ptr<Bitmap> mekan_pushing[2][14][2];

int hangar_x[4];
int hangar_y[4];
int hangar_door_frame[4];
int mekan_x[4];
int mekan_y[4];
int mekan_frame[4];
int mekan_status[4];
int mekan_target[4];
int mekan_subtarget[4];
int mekan_direction[4];
int hangar_door_opening[4];
int hangar_door_closing[4];
int mekan_mission[4];
int plane_wants_in[16];
int plane_wants_out[16];

int player_exists[16] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int plane_present[16] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int player_sides[16] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
int player_tsides[16] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};

int plane_coming[16];

unsigned char *level_bitmap;

int playing_solo;
int solo_country = 0;
int solo_mission;

int solo_failed;
int solo_success;
int solo_dest_remaining;

//\ struct_information:

int struct_state[MAX_STRUCTURES];
int struct_width[MAX_STRUCTURES];
int struct_heigth[MAX_STRUCTURES];

//\ Fonts

std::unique_ptr<gfx::font::Font> fontti;
std::unique_ptr<gfx::font::Font> frost;
std::unique_ptr<gfx::font::Font> grid2;

//\ Parameter control

char **parametrit; /* argv */
int parametri_kpl; /* argc */

//\ Shots control

int shots_flying_x[MAX_SHOTS];
int shots_flying_y[MAX_SHOTS];
int shots_flying_x_speed[MAX_SHOTS];
int shots_flying_y_speed[MAX_SHOTS];
int shots_flying_owner[MAX_SHOTS];
int shots_flying_age[MAX_SHOTS];
int shots_flying_infan[MAX_SHOTS];

//\ Player planes

int in_closing[16];
int player_shown_x[16];
int player_shown_y[16];
unsigned char *plane_p[16][61][4][2];
int hangarmenu_active[16];
int hangarmenu_position[16];
int hangarmenu_gas[16];
int hangarmenu_ammo[16];
int hangarmenu_bombs[16];

int hangarmenu_max_gas[16];
int hangarmenu_max_ammo[16];
int hangarmenu_max_bombs[16];

/* Timing */
int frame_laskuri = 0;
/* Timing*/

//\ Graphics locating

int x_muutos[4] = {0, 160, 0, 160};
int y_muutos[4] = {0, 0, 100, 100};
int x1_raja[4] = {2, 162, 2, 162};
int y1_raja[4] = {2, 2, 102, 102};
int x2_raja[4] = {157, 317, 157, 317};
int y2_raja[4] = {89, 89, 187, 187};

//\ Airfields

int player_on_airfield[16];

//\ General

int collision_detect = 1;
int part_collision_detect = 1;
int power_reverse = 0;
int power_on_off = 0;
int loading_texts = 0;
int solo_mode = -1;
int aftermath;
int level_loaded;

//\ Flying objects control

struct flying_objects_data fobjects[MAX_FLYING_OBJECTS];

//\ Scoring data

int player_fired[16];
int player_hits[16];
int player_shots_down[16][16];

int player_bombed[16];
int player_bomb_hits[16];

//\ Leveldata

char levelname[80];

struct level_struct leveldata;

//\ Bombs

int bomb_x[MAX_BOMBS];
int bomb_y[MAX_BOMBS];
int bomb_speed[MAX_BOMBS];
int bomb_angle[MAX_BOMBS];
int bomb_owner[MAX_BOMBS];
int bomb_x_speed[MAX_BOMBS];
int bomb_y_speed[MAX_BOMBS];
int bomb_age[MAX_BOMBS];

int roll_key_down[16];
int bomb_key_down[16];
int hangarkey_up_down[16];
int hangarkey_down_down[16];
int hangarkey_right_down[16];
int hangarkey_left_down[16];

int plane_tire_y;

int flags_state[MAX_FLAGS];
int flags_frame[MAX_FLAGS];
int flags_x[MAX_FLAGS];
int flags_y[MAX_FLAGS];
int flags_owner[MAX_FLAGS];
std::unique_ptr<Bitmap> flags[4][12];

//\\ AA-MG && AA-Gun

int kkbase_x[MAX_AA_GUNS];
int kkbase_y[MAX_AA_GUNS];
int kkbase_last_shot[MAX_AA_GUNS];
int kkbase_shot_number[MAX_AA_GUNS];
int kkbase_frame[MAX_AA_GUNS];
int kkbase_status[MAX_AA_GUNS];
int kkbase_country[MAX_AA_GUNS];
int kkbase_type[MAX_AA_GUNS];
int kkbase_mission[MAX_AA_GUNS];
int kkbase_number[MAX_AA_GUNS];

//\\ Infantry

extern int infan_x[MAX_INFANTRY];
extern int infan_y[MAX_INFANTRY];
extern int infan_direction[MAX_INFANTRY];
extern int infan_last_shot[MAX_INFANTRY];
extern int infan_state[MAX_INFANTRY];
extern int infan_country[MAX_INFANTRY];
extern int infan_frame[MAX_INFANTRY];
extern int infan_x_speed[MAX_INFANTRY];
extern int infan_stop[8];

//\\ Computer players

int computer_active[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int terrain_level[2400];
int wide_terrain_level[2400];
int current_mission[16];
int mission_phase[16];
int mission_target[16];

int distances[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int angles[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int bombs_going[MAX_STRUCTURES];

//\\ Configuration

short joystick_exists;


// Solo mode success/failure text blinking variables
int status_frames = 0;
int status_state = 0;

int mission_duration;
int mission_interrupted;
int mission_re_fly = -1;

/* Recorded games hold the controls of 16 players for up to this many frames. */
constexpr int RECORD_FRAMES = 24 * 1280;

FILE *record_file = nullptr;
char *record_data = nullptr;
int *record_random = nullptr;
int record_counter = 0;


int quit_flag = 0;

int mc_up[16];
int mc_down[16];
int mc_bomb[16];
int mc_roll[16];
int mc_guns[16];
int mc_power[16];

int new_mc_up[16];
int new_mc_down[16];
int new_mc_bomb[16];
int new_mc_roll[16];
int new_mc_guns[16];
int new_mc_power[16];

// This is used to prevent player from taking off
// when reversed throttle is enabled.
// If it is 1, the player won't start until
// throttle button is pressed, even with reverse throttle.
int power_break_active[16];

/* -record / -playback, looked up once per game instead of every frame. */
static bool recording_game = false;
static bool playing_back_game = false;

#include "gfx/extra.h"

//\\\\ Externs

extern int dirty_area;
extern int isa_counter;
extern int dirty_x[100];
extern int dirty_y[100];
extern int dirty_yl[100];
extern int dirty_xl[100];
extern unsigned char *dirty_marker;

extern int itgun_shot_x[MAX_ITGUN_SHOTS];
extern int itgun_shot_y[MAX_ITGUN_SHOTS];

//\\\\ Prototypes

void hangarmenu_handle();
int findparameter(const char *jono);
void controls();
void detect_collision();
void main_engine();
void load_up();
void init_data();
void do_aftermath(int show_it_all);
void airfield_checks();
void handle_parameters();
void init_resolution(int width, int height);
void load_level();

void do_flags();
sb_sample *sample_load(const char *name);
void kkbase_sound(int type, int kkbase_x);
void itgun_sound(int itgun_x);
void rotate_water_palet();
void cause_damage(int amount, int plane);
int small_warning(const char *message);
void load_all_samples();
void init_sologame();
void write_files();

void load_sfx();
void load_music();
void clear_sfx();
void clear_music();
void init_sounds();
void uninit_sounds();

int get_player_fired(int player);
int get_player_hits(int player);
int get_player_shots_down(int player, int player2);

extern void start_it_shot(int x, int y, int angle);

extern "C" {
extern void init_alkucallback();
}

constexpr int32_t NUMBER_OF_STRUCT_NAMES = 35;

char struct_names[NUMBER_OF_STRUCT_NAMES * 2][7] = {
    "TOWER",  "DTOWER", "FAC1",   "DFAC1",  "FAC2",   "DFAC2",  "BIRCH1",
    "DBIR1",  "BIRCH2", "DBIR2",  "BIRCH3", "DBIR3",  "TENT2",  "DTENT2",
    "OAK1",   "DOAK1",  "OAK2",   "DOAK2",  "OAK3",   "DOAK3",  "PALM1",
    "DPALM1", "PALM2",  "DPALM2", "PALM3",  "DPALM3", "HOU1",   "DHOU1",
    "HOU2",   "DHOU2",  "HOU3",   "DHOU3",  "HOU4",   "DHOU4",  "HOU5",
    "DHOU5",  "WATERT", "DWAT",   "CHOU1",  "DCHOU1", "BARR2",  "DBARR2",
    "BARRA",  "DBARRA", "CBARR2", "DCBAR2", "CBARRA", "DCBARR", "CHOU5",
    "DCHOU5", "WOAK1",  "DOAK1",  "WOAK2",  "DOAK2",  "WOAK3",  "DOAK3",
    "STORE1", "DSTORE", "IGLU1",  "DIGLU1", "VEC1",   "DVEC1",  "VEC2",
    "DVEC2",  "AUT1",   "DBIR1",  "AUT2",   "DBIR2",  "AUT3",   "DBIR3"};

//\\\\ Functions

static void record_random_swap_endianess() {
  int i;
  for (i = 0; i < RECORD_FRAMES; i++) {
    record_random[i] = SDL_Swap32LE(record_random[i]);
  }
}

/* Put plane l on the ground in front of its hangar, facing the runway. */
static void place_plane_at_hangar(int l) {
  int side = player_tsides[l];

  player_x[l] = (hangar_x[side] + 38) << 8;
  if (leveldata.plane_direction[side]) {
    player_angle[l] = TAIL_HIT_ANGLE << 8;
    player_upsidedown[l] = 0;
  } else {
    player_angle[l] = (180 + 6 - TAIL_HIT_ANGLE) << 8;
    player_upsidedown[l] = 1;
  }

  player_y[l] = (leveldata.airfield_y[side] - PLANE_MODIFICATION) << 8;
  player_on_airfield[l] = side + 1;
  player_was_on_airfield[l] = side + 1;
  controls_power[l] = 0;
  player_speed[l] = 0;
}

void hangarmenu_handle() {

  int l;

  for (l = 0; l < 8; l++) {
    if (!hangarmenu_active[l])
      continue;

    if (mc_guns[l]) {
      if (!hangarkey_right_down[l]) {
        hangarkey_right_down[l] = 1;
        hangarmenu_position[l]++;
        if (hangarmenu_position[l] > 2)
          hangarmenu_position[l] = 2;
      }
    } else
      hangarkey_right_down[l] = 0;

    if (mc_bomb[l]) {
      if (!hangarkey_left_down[l]) {
        hangarkey_left_down[l] = 1;
        hangarmenu_position[l]--;
        if (hangarmenu_position[l] < 0)
          hangarmenu_position[l] = 0;
      }
    } else
      hangarkey_left_down[l] = 0;

    if (mc_down[l]) {
      if (!hangarkey_up_down[l]) {
        hangarkey_up_down[l] = 1;

        switch (hangarmenu_position[l]) {
        case 0:
          if (++hangarmenu_bombs[l] > hangarmenu_max_bombs[l])
            hangarmenu_bombs[l] = hangarmenu_max_bombs[l];
          break;

        case 1:
          hangarmenu_ammo[l] += 16;
          if (hangarmenu_ammo[l] > hangarmenu_max_ammo[l])
            hangarmenu_ammo[l] = hangarmenu_max_ammo[l];
          break;

        case 2:
          hangarmenu_gas[l] += 256;
          if (hangarmenu_gas[l] > hangarmenu_max_gas[l])
            hangarmenu_gas[l] = hangarmenu_max_gas[l];
          break;
        }
      }

    } else
      hangarkey_up_down[l] = 0;

    if (mc_up[l]) {
      if (!hangarkey_down_down[l]) {
        hangarkey_down_down[l] = 1;

        switch (hangarmenu_position[l]) {
        case 0:
          if (--hangarmenu_bombs[l] < 0)
            hangarmenu_bombs[l] = 0;
          break;

        case 1:
          if (!playing_solo && config.unlimited_ammo)
            break;
          hangarmenu_ammo[l] -= 16;
          if (hangarmenu_ammo[l] < 0)
            hangarmenu_ammo[l] = 0;
          break;

        case 2:
          if (!playing_solo && config.unlimited_gas)
            break;
          hangarmenu_gas[l] -= 256;
          if (hangarmenu_gas[l] < 2)
            hangarmenu_gas[l] = 2;
          break;
        }
      }

    } else
      hangarkey_down_down[l] = 0;

    if (mc_roll[l]) {
      plane_wants_out[l] = 1;
      power_break_active[l] = 1;
      mc_power[l] = 0;
      new_mc_power[l] = 0;
      player_rolling[l] = 0;
      player_spinning[l] = 0;
      spinning_remaining[l] = 0;
      player_last_shot[l] = 0;

      hangarmenu_active[l] = 0;
    }

    place_plane_at_hangar(l);

    player_x_8[l] = player_x[l] >> 8;
    player_y_8[l] = player_y[l] >> 8;
    controls_up[l] = 0;
    controls_down[l] = 0;

    player_gas[l] = hangarmenu_gas[l];
    player_ammo[l] = hangarmenu_ammo[l];
    player_bombs[l] = hangarmenu_bombs[l];

    player_endurance[l] = plane_mass[l] >> 3; // 4
  }
}

void init_sologame() { init_mission(solo_country, solo_mission); }

int small_warning(const char *message) {
  int flag = 1;
  int x, y, n1, n2;
  int response = 0;

  std::unique_ptr<Bitmap> warnkuva(new Bitmap("WARN2"));

  while (flag) {
    koords(&x, &y, &n1, &n2);

    tyhjaa_vircr();
    warnkuva->blit(87, 59);
    frost->printf(91, 77, message);
    cursor->blit(x - 10, y - 10);

    do_all();

    if (n1 || n2) {
      if (x >= 92 && x <= 131 && y >= 115 && y <= 134) {
        flag = 0;
        response = 1;
      }

      if (x >= 187 && x <= 226 && y >= 115 && y <= 134) {
        flag = 0;
        response = 0;
      }
    }
  }

  wait_mouse_relase(1);

  return response;
}

/* The plane breaks up: it leaves the game and its parts fly off. splash
 * selects the sound of hitting water. */
static void crash_plane(int plane, bool splash = false) {
  in_closing[plane] = 2;
  plane_present[plane] = 0;
  start_parts(plane);
  if (config.sound_on && config.sfx_on) {
    if (splash)
      play_2d_sample(sample_spcrash, player_x_8[solo_country],
                     player_x_8[plane]);
    else
      play_2d_sample(sample_crash[util::wutil::wrandom(2)],
                     player_x_8[solo_country], player_x_8[plane]);
  }
}

void cause_damage(int amount, int plane) {

  player_endurance[plane] -= amount;

  if (player_endurance[plane] < 1) {
    if (!player_spinning[plane] && !in_closing[plane]) {

      if (util::wutil::wrandom(2) == 1 && (!player_on_airfield[plane]))
        player_spinning[plane] = 1;
      else
        crash_plane(plane);
    }
  }
}

void rotate_water_palet() {
  int l, l2;
  int seivi;
  static int nytko = 0;

  /* The water colours 224-231 rotate every other frame. */
  if (++nytko == 2) {
    nytko = 0;
    return;
  }

  for (l2 = 0; l2 < 3; l2++) {
    seivi = ruutu.normaalipaletti[224][l2];
    for (l = 0; l < 7; l++)
      ruutu.normaalipaletti[l + 224][l2] = ruutu.normaalipaletti[l + 225][l2];
    ruutu.normaalipaletti[231][l2] = seivi;
  }

  setpal_range(&ruutu.normaalipaletti[224], 224, 8, 1);
}

void do_flags() {
  int l;

  for (l = 0; l < MAX_FLAGS; l++)
    if (flags_x[l])
      if ((++flags_state[l]) >= FLAGS_SPEED) {
        flags_state[l] = 0;
        flags_frame[l]++;
        if (flags_frame[l] > 11)
          flags_frame[l] = 0;
      }
}

void airfield_checks() {
  int l, l2;
  int yyy, xxx;

  for (l = 0; l < 16; l++) {
    if (!plane_present[l])
      continue;

    player_was_on_airfield[l] = 0;

    xxx = 5;
    yyy = player_upsidedown[l] ? -5 : 5;

    plane_tire_y = ((player_y[l]) +
                    (((-xxx * util::wutil::sinit[player_angle[l] >> 8] +
                       yyy * util::wutil::cosinit[player_angle[l] >> 8] + 128) >>
                      8)
                     << 8) +
                    256) >>
                   8;

    player_on_airfield[l] = 0;
    for (l2 = 0; l2 < 4; l2++)
      if (leveldata.airfield_x[l2] &&
          ((player_x_8[l]) >= leveldata.airfield_x[l2]) &&
          ((player_x_8[l]) <=
           (leveldata.airfield_x[l2] + leveldata.airfield_lenght[l2])) &&
          (plane_tire_y >= (leveldata.airfield_y[l2] - 1))) {

        if (((player_angle[l] >> 8) <= TAIL_HIT_ANGLE) ||
            ((player_angle[l] >> 8) >= (360 - HIT_ANGLE))) {
          if (!player_upsidedown[l]) {
            if ((plane_tire_y >= (leveldata.airfield_y[l2]))) {
              player_on_airfield[l] = l2 + 1;
              player_was_on_airfield[l] = l2 + 1;
            } else
              player_was_on_airfield[l] = l2 + 1;
          }
        }

        if (((player_angle[l] >> 8) <= (180 + 6 + HIT_ANGLE)) &&
            ((player_angle[l] >> 8) >= (180 + 6 - TAIL_HIT_ANGLE))) {
          if (player_upsidedown[l]) {
            if ((plane_tire_y >= (leveldata.airfield_y[l2]))) {
              player_on_airfield[l] = l2 + 1;
              player_was_on_airfield[l] = l2 + 1;
            } else
              player_was_on_airfield[l] = l2 + 1;
          }
        }
      }
  }
}

void init_player(int l, int pommit) {
  if (!player_exists[l])
    return;

  if (playing_solo) {

    if (in_closing[l]) {

      number_of_planes[l]--;

      if (number_of_planes[l] < 0) {
        if (l != solo_country) {
          in_closing[l] = 0;
          plane_present[l] = 0;
          player_exists[l] = 0;
          return;
        }
      }
    }
  }

  in_closing[l] = 0;
  plane_coming[l] = 0;
  plane_present[l] = 0;
  plane_wants_in[l] = 0;
  player_x_speed[l] = 0;
  player_y_speed[l] = 0;
  // no need fix in 2nd Solo mission of England
  if (playing_solo && solo_country == 2 && solo_mission == 1)
    power_break_active[l] = 0;
  else
    power_break_active[l] = 1;

  if ((!computer_active[l]) && !(playing_solo && miss_pl_x[l])) {
    plane_coming[l] = 1;

    hangarmenu_active[l] = 1;

  } else {
    if (playing_solo && miss_pl_x[l]) {
      plane_coming[l] = 0;
      plane_present[l] = 1;
      player_x[l] = miss_pl_x[l] << 8;
      player_angle[l] = 180 << 8;
      player_upsidedown[l] = 1;

      player_y[l] = miss_pl_y[l] << 8;
      player_on_airfield[l] = 0;
      player_was_on_airfield[l] = 0;
      controls_power[l] = 1;
      player_speed[l] = 1500;

      if (computer_active[l]) {
        current_mission[l] = AIM_NOMISSION;
        mission_phase[l] = 0;
        mission_target[l] = 0;
      }

    } else {
      plane_coming[l] = 1;
      plane_wants_out[l] = 1;
      place_plane_at_hangar(l);

      if (computer_active[l]) {
        current_mission[l] = AIM_TAKEOFF;
        mission_phase[l] = 0;
        mission_target[l] = 0;
      }
    }

    player_x_8[l] = player_x[l] >> 8;
    player_y_8[l] = player_y[l] >> 8;
    controls_up[l] = 0;
    controls_down[l] = 0;

    if (playing_solo) {
      switch (fighter[l]) {
      case 0:
        player_ammo[l] = 0;
        player_bombs[l] = plane_bombs[l];
        break;

      case 1:
        player_ammo[l] = plane_ammo[l];
        player_bombs[l] = (pommit ? plane_bombs[l] : 0);
        break;
      case 2:
        player_ammo[l] = plane_ammo[l];
        player_bombs[l] = 0;
        break;
      }

    } else {
      player_ammo[l] = plane_ammo[l];
      player_bombs[l] =
          computer_active[l]
              ? ((util::wutil::wrandom(mission_duration + 1) < 1440) ? plane_bombs[l] : 0)
              : (pommit ? plane_bombs[l] : 0);
    }

    player_gas[l] = plane_gas[l];
    player_rolling[l] = 0;
    player_spinning[l] = 0;
    spinning_remaining[l] = 0;
    player_last_shot[l] = 0;
    player_endurance[l] = plane_mass[l] >> 3; // 4
  }
}

/* Index of the command line option, or 0 if it is not given. */
int findparameter(const char *jono) {
  int laskuri;

  for (laskuri = 1; laskuri < parametri_kpl; laskuri++)
    if (!strcmp(parametrit[laskuri], jono))
      return (laskuri);

  return (0);
}

/* Value glued to an option, e.g. "-levelfoo" gives "foo", or nullptr. */
const char *findparameter_value(const char *jono) {
  int laskuri;
  size_t len = strlen(jono);

  for (laskuri = 1; laskuri < parametri_kpl; laskuri++)
    if (!strncmp(parametrit[laskuri], jono, len) && parametrit[laskuri][len])
      return parametrit[laskuri] + len;

  return nullptr;
}

/* Read joystick joy as the controls of player l. */
static void read_joystick(int joy, int l) {
  int inmenu = hangarmenu_active[l] || in_closing[l];

  get_joystick_action(joy, inmenu, &new_mc_down[l], &new_mc_up[l],
                      &new_mc_power[l], &new_mc_roll[l], &new_mc_guns[l],
                      &new_mc_bomb[l], power_on_off, power_reverse,
                      &controls_power2[l], in_closing[l],
                      &power_break_active[l]);
  if (!joystick_has_roll_button(joy) && !inmenu) {
    // Autoroll code
    new_mc_roll[l] = 0;
    if (new_mc_down[l] == new_mc_up[l]) /* not turning up/down */
      if ((player_upsidedown[l] &&
           (player_angle[l] < 23040 || player_angle[l] > 69120)) ||
          (!player_upsidedown[l] &&
           (player_angle[l] < 69120 && player_angle[l] > 23040)))
        if (!player_rolling[l])
          new_mc_roll[l] = 1;
  }
}

void controls() {
  int l;

  for (l = 0; l < 16; l++) {
    mc_up[l] = new_mc_up[l];
    mc_down[l] = new_mc_down[l];
    mc_roll[l] = new_mc_roll[l];
    mc_power[l] = new_mc_power[l];
    mc_bomb[l] = new_mc_bomb[l];
    mc_guns[l] = new_mc_guns[l];
  }

  //// Recorder

  unsigned char rbyte;
  int rcount;

  /* stop one frame early, record_random is indexed with the next frame */
  bool record_full = record_counter + 16 >= 16 * RECORD_FRAMES;

  if (recording_game && !record_full) {
    for (rcount = 0; rcount < 16; rcount++) {

      rbyte = 0;
      rbyte += mc_up[rcount];
      rbyte += mc_down[rcount] << 1;
      rbyte += mc_roll[rcount] << 2;
      rbyte += mc_power[rcount] << 3;
      rbyte += mc_bomb[rcount] << 4;
      rbyte += mc_guns[rcount] << 5;

      record_data[record_counter++] = rbyte;
    }

    record_random[record_counter >> 4] = util::wutil::wrandom(2147483647);
  }

  if (playing_back_game) {
    if (record_full) {
      printf("End of recording\n");
      exit(0);
    }
    for (rcount = 0; rcount < 16; rcount++) {
      rbyte = record_data[record_counter++];

      mc_up[rcount] = rbyte & 1;
      mc_down[rcount] = rbyte & 2;
      mc_roll[rcount] = rbyte & 4;
      mc_power[rcount] = rbyte & 8;
      mc_bomb[rcount] = rbyte & 16;
      mc_guns[rcount] = rbyte & 32;
    }

    if (record_random[record_counter >> 4] != util::wutil::wrandom(2147483647)) {
      printf("Random failure at %d\n", record_counter >> 4);
    }
  }
  //// Recorder

  if (!(playing_solo && hangarmenu_active[solo_country])) {
    for (l = 0; l < 16; l++) {

      if (!player_exists[l])
        continue;

      if (hangarmenu_active[l])
        continue;

      if (computer_active[l]) {
        world::tripai::do_ai(l);
        continue;
      }

      if (l > 7)
        continue;

      if (mc_down[l])
        controls_down[l] = 1;
      else
        controls_down[l] = 0;

      if (mc_up[l])
        controls_up[l] = 1;
      else
        controls_up[l] = 0;

      controls_power[l] = mc_power[l];

      if (mc_bomb[l]) {
        if (!bomb_key_down[l]) {
          if (!player_on_airfield[l]) {
            if (player_bombs[l] && !in_closing[l]) {

              player_bombs[l]--;
              drop_bomb(l);
            }
          } else if (player_speed[l] == 0 && (player_on_airfield[l] - 1) == l &&
                     !in_closing[l] && !player_spinning[l]) {
            if (!plane_wants_out[l])
              plane_wants_in[l] = 1;
          }

          bomb_key_down[l] = 1;
        }

      } else
        bomb_key_down[l] = 0;

      if (mc_roll[l]) {
        if (!roll_key_down[l]) {

          roll_key_down[l] = 1;
          if (!player_on_airfield[l]) {

            if (!player_rolling[l])
              player_rolling[l] = 1;

          } else {
            if (!player_speed[l] && !in_closing[l] && !player_spinning[l] &&
                (player_on_airfield[l] - 1) == l) {
              if (!plane_wants_out[l])
                plane_wants_in[l] = 1;
            }
          }
        }

      } else
        roll_key_down[l] = 0;

      if (mc_guns[l] && (player_last_shot[l] > SHOTS_RATE) && player_ammo[l]) {

        start_shot(l);
      }

    } // control

    for (l = 0; l < 16; l++)
      if (player_rolling[l])
        player_rolling[l]++;

    for (l = 0; l < 16; l++)
      if (player_rolling[l] == 4) {
        player_rolling[l] = 0;
        if (player_upsidedown[l])
          player_upsidedown[l] = 0;
        else
          player_upsidedown[l] = 1;
      }
  }

  for (l = 0; l < 16; l++) {
    if (!player_exists[l])
      continue;

    if (l > 3)
      continue;

    int is_joy = 0;

    if (playing_solo && config.joystick[4] != -1) {
      read_joystick(4, l);
      is_joy = 1;
    }

    if (is_joy == 0) {
      for (int i = 0; i < 4; i++) {
        if (config.joystick[i] == l) {
          read_joystick(i, l);
          is_joy = 1;
          break;
        }
      }
    }

    if (is_joy == 0) {
      /* The solo player flies with the keys of the pilot in the roster,
         multiplayer pilots with the keyset of their seat. */
      keymap keys;
      if (playing_solo) {
        const rosteri &r = roster[config.player_number[solo_country]];
        keys = {r.up, r.down, r.roll, r.power, r.guns, r.bombs};
      } else {
        keys = player_keys[l];
      }

      if (key[keys.down])
        new_mc_down[l] = 1;
      else
        new_mc_down[l] = 0;

      if (key[keys.up])
        new_mc_up[l] = 1;
      else
        new_mc_up[l] = 0;

      if (!power_on_off) {
        // Toggle power is off
        if (!power_reverse) {
          // ..and no reverse
          if (key[keys.power])
            new_mc_power[l] = 1;
          else
            new_mc_power[l] = 0;
        } else {
          // Toggle off but reverse - we need to prevent takeoff with
          // power_break_active
          if (key[keys.power]) {
            // Power is pressed
            new_mc_power[l] = 0;
            if (power_break_active[l])
              power_break_active[l] = 0;
          } else {
            // Power is not pressed
            if (!power_break_active[l])
              new_mc_power[l] = 1;
            else
              new_mc_power[l] = 0;
          }
        }
      } else {
        // Toggle power is on
        if (key[keys.power]) {
          if (!controls_power2[l]) {
            if (new_mc_power[l])
              new_mc_power[l] = 0;
            else
              new_mc_power[l] = 1;
          }
          controls_power2[l] = 1;

        } else
          controls_power2[l] = 0;

        if (in_closing[l])
          new_mc_power[l] = 0;
      }

      new_mc_bomb[l] = 0;
      if (key[keys.bombs])
        new_mc_bomb[l] = 1;

      new_mc_roll[l] = 0;
      if (key[keys.roll])
        new_mc_roll[l] = 1;

      new_mc_guns[l] = 0;
      if (key[keys.guns])
        new_mc_guns[l] = 1;
    }

    if (player_spinning[l]) {
      if (!player_rolling[l])
        player_rolling[l] = 1;
    }

    if (spinning_remaining[l]) {
      spinning_remaining[l]--;

      if (!player_rolling[l])
        player_rolling[l] = 1;
    }
  }
}

/* Plane l crashes if the level pixel (nx, ny) is not sky. Returns whether
 * it did. */
static bool crash_into_terrain(int l, int nx, int ny) {
  if (nx < 0 || nx >= 2400 || ny >= 200) {
    nx = 0;
    ny = 199;
  }

  if (ny < 0)
    ny = 0;

  unsigned char kohta = level_bitmap[nx + ny * 2400];

  if ((kohta >= 112 && kohta <= 119) || in_closing[l] || player_on_airfield[l])
    return false;

  if (!player_spinning[l]) {
    player_shots_down[l][l]++;
    player_points[l]--;
  }

  crash_plane(l, kohta < 231 && kohta > 224);
  return true;
}

void detect_collision() {
  int xl, yl, xa, ya, sx, sy;
  int l, l2;
  int laskx, lasky;
  int px[16], py[16];
  int nx, ny;
  int yyy, xxx;
  const unsigned char *sprite, *sprite2;

  for (l = 0; l < 16; l++) {
    if (!plane_present[l])
      continue;

    px[l] = (player_x_8[l]) - 10;
    py[l] = (player_y_8[l]) - 10;
  }

  for (l = 0; l < 16; l++) {
    if (!plane_present[l])
      continue;

    sprite = plane_p[l][(player_angle[l] >> 8) / 6][player_rolling[l]]
                    [player_upsidedown[l]];
    for (l2 = 0; l2 < 16; l2++) {
      if (l2 == l)
        continue;

      if (!plane_present[l2])
        continue;

      if ((px[l] + 19) >= px[l2] && px[l] <= (px[l2] + 19) &&
          (py[l] + 19) >= py[l2] && py[l] <= (py[l2] + 19)) {
        sprite2 = plane_p[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]]
                         [player_upsidedown[l2]];
        sx = px[l2] - px[l];
        sy = py[l2] - py[l];

        xa = sx;
        if (xa < 0) {
          xl = 19 + xa;
          xa = 0;

        } else {
          xl = 19;
        }

        ya = sy;
        if (ya < 0) {
          yl = 19 + ya;
          ya = 0;
        } else {
          yl = 19;
        }

        /* Once both planes are breaking up (or collisions are off), the
           remaining overlapping pixels change nothing, so the first one
           is enough. */
        bool hit = false;
        for (lasky = ya; lasky <= yl && !hit; lasky++)
          for (laskx = xa; laskx <= xl && !hit; laskx++) {
            if (sprite[lasky * 20 + laskx] != 255 &&
                sprite2[(lasky - sy) * 20 + (laskx - sx)] != 255) {
              hit = true;
              if (collision_detect) {
                if (!in_closing[l]) {
                  crash_plane(l);
                  player_shots_down[l][l]++;
                  player_points[l]--;
                }
                if (!in_closing[l2]) {
                  crash_plane(l2);
                  player_shots_down[l2][l2]++;
                  player_points[l2]--;
                }
              }
            }
          }
      }
    }

    /* The nose ... */
    nx = (player_x[l] + 12 * util::wutil::cosinit[player_angle[l] >> 8]) >> 8;
    ny = (player_y[l] - 12 * util::wutil::sinit[player_angle[l] >> 8]) >> 8;

    if (crash_into_terrain(l, nx, ny))
      continue;

    /* ... and the wheels must stay in the sky. */
    xxx = 5;
    yyy = player_upsidedown[l] ? -5 : 5;

    nx = (player_x[l] >> 8) + ((+xxx * util::wutil::cosinit[player_angle[l] >> 8] +
                                yyy * util::wutil::sinit[player_angle[l] >> 8] + 128) >>
                               8);
    ny = (player_y[l] >> 8) + ((-xxx * util::wutil::sinit[player_angle[l] >> 8] +
                                yyy * util::wutil::cosinit[player_angle[l] >> 8] + 128) >>
                               8);

    crash_into_terrain(l, nx, ny);
  }
}

static void solo_do_all() {
  int x_offset = 160;

  // Compute player's plane location on screen and corresponding
  // offset of the background bitmap

  if (((player_x_8[solo_country]) - x_offset + 320) > NUMBER_OF_SCENES * 160)
    x_offset -=
        NUMBER_OF_SCENES * 160 - ((player_x_8[solo_country]) - x_offset + 320);

  if (((player_x_8[solo_country]) - x_offset) < 0)
    x_offset += player_x_8[solo_country] - x_offset;

  x_offset -= player_x_8[solo_country];
  x_offset = -x_offset;

  // draw double bufffer to screen
  do_all();

  // initialize double buffer back to background image
  // background image 2400x200 (2400 == NUMBER_OF_SCENES*160)
  // surface 320x200
  maisema->blit(-x_offset, 0);
}

/*
 * The following function has been adapted from
 * linux-2.6.18/storage/multipath-tools/kpartx/crc32.c with the
 * following disclamer:
 *
 * This code is in the public domain; copyright abandoned.
 * Liability for non-performance of this code is limited to the amount
 * you paid for it.  Since it is distributed for free, your refund will
 * be very very small.  If it breaks, you get to keep both pieces.
 */
#define CRCPOLY_LE 0xedb88320
static uint32_t crc32_le(uint32_t crc, unsigned char const *p, size_t len) {
  int i;

  while (len--) {
    crc ^= *p++;
    for (i = 0; i < 8; i++)
      crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
  }
  return crc;
}

/* -debugtrace prints a checksum of every frame (see tools/compare-baseline). */
void do_debug_trace() {
  static int first_call = 1;
  static int enabled = 0;

  if (first_call) {
    if (findparameter("-debugtrace")) {
      enabled = 1;
      /* keep the trace complete even if the game crashes */
      setvbuf(stdout, nullptr, _IOLBF, 0);
    }
    first_call = 0;
  }

  if (enabled) {
    int len;
    uint32_t vircr_checksum;

    if (current_mode == VGA_MODE) {
      len = 320 * 200;
    } else {
      len = screen_width * screen_height;
    }

    vircr_checksum = crc32_le(~0, vircr, len);

    printf("%d %08x\n", frame_laskuri, vircr_checksum);
  }
}

/* The pause key is down: wait until it is released, then pressed and
 * released again. */
static void pause_game(int pause_key) {
  while (key[pause_key]) { // still pressed
    io::timing::nopeuskontrolli();
    update_key_state();
  }
  while (!key[pause_key]) { // released
    io::timing::nopeuskontrolli();
    update_key_state();
  }
  while (key[pause_key]) { // pressed again
    io::timing::nopeuskontrolli();
    update_key_state();
  }
}

void main_engine() {
  int preview_mode = 0;
  int l, l2, flag = 1;
  int xx, yy;

  quit_flag = 0;

  if (playing_solo) {
    collision_detect = 0;
    part_collision_detect = 1;
  } else {
    collision_detect = config.nocollision;
    part_collision_detect = config.partcollision;
  }

  power_on_off = config.poweronoff;
  power_reverse = config.powerrev;

  for (l = 0; l < 16; l++) {
    player_sides[l] = player_tsides[l];
  }

  if (playing_solo) {

    if (!(preview_mode = solo_player_menu())) {
      aftermath = 0;
      return;
    }

    if (preview_mode == 2)
      preview_mode = 0;
  }
  aftermath = 1;

  util::wutil::setwrandom(7);

  load_level();
  level_loaded = 1;

  if (playing_solo) {
    init_sologame();
  }

  init_data();

  for (l = 0; l < 16; l++) {
    in_closing[l] = 1;
    init_player(l);
    player_points[l] = 0;
  }

  for (l = 0; l < MAX_STRUCTURES; l++) {
    if (!leveldata.struct_hit[l])
      continue;

    if (structures[l][0] == nullptr)
      continue;

    structures[l][0]->blit_to_bitmap(maisema, leveldata.struct_x[l],
                                     leveldata.struct_y[l]);
  }

  if (config.flags) { do_flags(); }

  world::tripai::do_kkbase();

  if (preview_mode) {
    if (!kangas_menu()) {
      aftermath = 0;
      return;
    }
  }

  if (playing_solo) {

    if (config.svga) {
      if (!findparameter("-debugnographics")) {
        if (findparameter("-black"))
          init_vesa("PALET3");
        else
          init_vesa("PALET5");
      }
    }

    solo_failed = 0;
    solo_success = 0;
    solo_dest_remaining = 0;

    for (l = 0; l < MAX_STRUCTURES; l++) {
      if (!leveldata.struct_x[l])
        continue;

      if (leveldata.struct_type[l] == 1)
        solo_dest_remaining++;
    }
  }

  if (current_mode == SVGA_MODE) {

    tyhjaa_vircr();

    /* The level is shown cut into split_num + 1 strips, one above the
       other, the last one drawn first. */
    for (l = split_num; l >= 0; l--)
      maisema->blit(-l * screen_width, l * 196 - 4, 0, 0, screen_width_less,
                    screen_height_less);

    for (l = 0; l < MAX_STRUCTURES; l++) {
      if (structures[l][0] != nullptr) {
        if (leveldata.struct_hit[l])
          continue;

        int strip = leveldata.struct_x[l] / screen_width;
        int strip_x = leveldata.struct_x[l] - strip * screen_width;

        structures[l][0]->blit(strip_x, leveldata.struct_y[l] + strip * 196 - 4);

        structures[l][0]->info(&xx, &yy);

        /* A structure crossing the cut continues on the next strip. */
        if (strip_x + xx > screen_width)
          structures[l][0]->blit(strip_x - screen_width,
                                 leveldata.struct_y[l] + (strip + 1) * 196 - 4);
      }
    }

    standard_background = std::make_unique<Bitmap>(0, 0, screen_width, screen_height);
    standard_background->blit(0, 0, 0, 0, screen_width_less,
                              screen_height_less);

  } else {
    board->blit(0, 0);
  }

  for (l = 0; l < 2400; l++) {
    terrain_level[l] = 0;

    for (l2 = 0; l2 < 200; l2++)
      if (level_bitmap[l + l2 * 2400] < 112 ||
          level_bitmap[l + l2 * 2400] > 119) {
        terrain_level[l] = l2 - 1;
        if (terrain_level[l] < 0)
          terrain_level[l] = 0;
        break;
      }
  }

  for (l = 0; l < 2400; l++) {
    wide_terrain_level[l] = 200;

    for (l2 = 0; l2 < 25; l2++) {
      if (l + l2 >= 2400)
        break;

      if (terrain_level[l + l2] < wide_terrain_level[l])
        wide_terrain_level[l] = terrain_level[l + l2];
    }

    for (l2 = 0; l2 < 25; l2++) {
      if (l - l2 < 0)
        break;

      if (terrain_level[l - l2] < wide_terrain_level[l])
        wide_terrain_level[l] = terrain_level[l - l2];
    }
  }

  for (l = 0; l < 100; l++) {
    if (!infan_x[l])
      continue;

    infan_y[l] = terrain_level[infan_x[l]] - 13;
  }

  if (findparameter("-pixelhunt"))
    setpal_range(nullptr, 112, 8);

  mission_duration = 0;
  mission_interrupted = 0;

  if (playing_solo) {
    world::tmexept::init_exeptions(solo_country, solo_mission);
    tyhjaa_vircr();
  }
  //// Open joysticks
  if (playing_solo) {
    open_close_joysticks(config.joystick[4] != -1, 0, 0, 0);
  } else {
    open_close_joysticks(config.joystick[0] != -1, config.joystick[1] != -1,
                         config.joystick[2] != -1, config.joystick[3] != -1);
  }

  //// Record

  recording_game = findparameter("-record") != 0;
  playing_back_game = findparameter("-playback") != 0;

  if (recording_game || playing_back_game) {
    record_data = (char *)util::wutil::walloc(16 * RECORD_FRAMES);
    record_random = (int *)util::wutil::walloc(4 * RECORD_FRAMES);
  }

  if (playing_back_game) {
    record_file = settings_open("record.dta", "rb");
    if (record_file == nullptr) {
      fprintf(stderr, "No recording (record.dta) in the settings directory\n");
      exit(1);
    }
    fread(record_data, RECORD_FRAMES, 16, record_file);
    fclose(record_file);

    record_file = settings_open("record.000", "rb");
    if (record_file == nullptr) {
      fprintf(stderr, "No recording (record.000) in the settings directory\n");
      exit(1);
    }
    fread(record_random, RECORD_FRAMES, 4, record_file);
    record_random_swap_endianess();
    fclose(record_file);
  }
  //// Record
  util::wutil::wrandom(7);

  if (!draw_with_vircr_mode)
    update_vircr_mode = 0;

  while (flag) {
    update_key_state();

    if (key[TK_PAUSE])
      pause_game(TK_PAUSE);
    // use F4 as an alias for the Pause key
    // because Pause does not always work reliably
    // (and is not present on all keyboards)
    if (key[TK_F4])
      pause_game(TK_F4);

    if (key[TK_ESCAPE]) {
      flag = 0;
      mission_interrupted = 1;
    }

    if (quit_flag) {
      quit_flag = 0;
      flag = 0;
      mission_interrupted = 1;
    }

    if (playing_solo && hangarmenu_active[solo_country]) {

    } else {
      do_shots();
      do_shots();

      world::tripai::do_it_shots();

      airfield_checks();

      for (l = 0; l < 16; l++) {
        if (!computer_active[l]) {
          if (plane_wants_in[l]) {
            if (mekan_target[player_tsides[l]] == l)
              if (mekan_subtarget[player_tsides[l]] == 2) {
                controls_power[l] = 0;
                controls_up[l] = 0;
                controls_down[l] = 0;
              }
          }

          if (plane_wants_out[l]) {
            if (mekan_target[player_tsides[l]] == l) {
              if (mekan_subtarget[player_tsides[l]] == 0) {
                controls_power[l] = 0;
              }

            } else {
              controls_power[l] = 0;
            }
          }
        } else {
          if (plane_wants_out[l] || plane_wants_in[l]) {
            controls_power[l] = 0;
          }
        }
      }

      model_planes();
      do_bombs();
      do_bombs();
      detect_collision();

      detect_damage();

      if (config.flames) { do_flames(); }
      do_fobjects();

      if (config.flags) { do_flags(); }
      world::tripai::do_infan();
      world::tripai::do_kkbase();
      world::tripai::do_mekan();
    }

    if (config.svga) { world::terrain::vesa_terrain_to_screen(); }
    else {
      if (solo_mode == -1) { world::terrain::terrain_to_screen(); }
      else {
        solo_do_all();
        world::terrain::solo_terrain_to_screen();
      }
    }

    hangarmenu_handle();

    if (playing_solo && hangarmenu_active[solo_country]) {

    } else {
      for (l = 0; l < 16; l++) {
        if (!player_exists[l]) { continue; }

        player_last_shot[l]++;
        if (!playing_solo && config.stop) { if (get_player_points(l) >= config.stop) { flag = 0; }}

        if (in_closing[l]) { plane_present[l] = 0; }
      }

      if (playing_solo) { world::tmexept::game_exeptions(solo_country, solo_mission); }
    }

    if (playing_solo) {
      for (l = 0; l < 12; l++) {
        if (player_exists[l] && player_sides[l] == 1) { break; }
      }

      if (l == 12) {
        if (!solo_dest_remaining) { solo_success = 1; }
      }

      if (key[TK_F1] && key[TK_F2] && key[TK_F3]) {
        frost->printf(40, 40, "SoloDestRemaining: %d. l=%d\n",
                      solo_dest_remaining, l);
      }

      if (number_of_planes[solo_country] < 0) { solo_failed = 1; }

      world::tmexept::endgame_exeptions(solo_country, solo_mission);

      if ((solo_success || solo_failed) && player_on_airfield[solo_country] &&
          !player_speed[solo_country]) { flag = 0; }

      if (miss_pl_x[solo_country] && solo_failed) { flag = 0; }
    }

    do_debug_trace();

    if (current_mode == SVGA_MODE) {
      do_all_clear(0);
    }

    rotate_water_palet();

    if (current_mode == VGA_MODE) {
      if (solo_mode == -1) { do_all(1); }
    }

    if (playing_solo && hangarmenu_active[solo_country]) {
      controls();
      io::timing::nopeuskontrolli();
    } else {
      controls();
      io::timing::nopeuskontrolli();
      frame_laskuri++;
      mission_duration++;
    }
  }

  if (!draw_with_vircr_mode)
    update_vircr_mode = 1;

  wait_relase();
  mission_re_fly = -1;

  //// Close joysticks
  open_close_joysticks(0, 0, 0, 0);

  //// Record

  if (recording_game) {
    record_file = settings_open("record.dta", "wb");
    fwrite(record_data, RECORD_FRAMES, 16, record_file);
    fclose(record_file);

    record_file = settings_open("record.000", "wb");
    record_random_swap_endianess();
    fwrite(record_random, RECORD_FRAMES, 4, record_file);
    fclose(record_file);
  }

  if (recording_game || playing_back_game) {
    free(record_data);
    free(record_random);
  }
  //// Record

  if (current_mode == SVGA_MODE) {
    standard_background = nullptr;
  }
}

void do_aftermath(int show_it_all) {

  int l, l2;
  int tempt;
  int firedi;
  int infantry_score = 0;
  int struct_score = 0;
  int some_score;
  int aaa_score = 0;
  int x, y, n1, n2;
  int need_for_letter = 0;
  char ch;
  int x_coord;
  int best_in_record = 0;
  int sisennys;
  int mission_success = 0;
  int objective_missed = 0;

  std::unique_ptr<Bitmap> temp;
  std::unique_ptr<Bitmap> temp2;
  std::unique_ptr<Bitmap> fly(new Bitmap("FLY"));
  std::unique_ptr<Bitmap> exitkuva(new Bitmap("EXIT"));

  gfx::fades::random_fade_out();

  mission_re_fly = -1;

  if (show_it_all && (!playing_solo)) {
    if (!findparameter("-debugnographics"))
      init_vga("PALET5");

    temp = std::make_unique<Bitmap>("SCORE");
    temp->blit(0, 0);

    for (l = 0; l < 4; l++) {
      for (l2 = 0; l2 < 4; l2++) {
        fontti->printf(69 + l2 * 30, 80 + l * 21, "%4d",
                       get_player_shots_down(l, l2));
      }
      int points = get_player_points(l);
      fontti->printf(194, 80 + l * 21, "%3d", abs(points));
      if (points < 0)
        fontti->printf(194, 80 + l * 21, "-");

      int fired = get_player_fired(l);
      fontti->printf(224, 80 + l * 21, "%5d", fired);

      if (fired == 0)
        firedi = 1;
      else
        firedi = fired;

      fontti->printf(254, 80 + l * 21, "%4d%%",
                     (get_player_hits(l) * 1000) / (firedi));
    }

    do_all();
  }

  if (playing_solo) {
    init_vga("PALET3");

    sisennys = grid2->printf(0, 0, "%s",
                             mission_names[solo_mission + solo_country * 6]);
    sisennys = (320 - sisennys) >> 1;

    temp = std::make_unique<Bitmap>("KANGAS", 0);
    temp->blit_fullscreen();
    temp2 = std::make_unique<Bitmap>(260, 188, 37, 10, temp);
    temp2->blit(0, 190);

    tempt = 0;

    for (l = 0; l < MAX_STRUCTURES; l++) {
      if (!leveldata.struct_x[l])
        continue;

      if (!strcmp(leveldata.pd_name[l], "INFAN")) {

        if (leveldata.struct_type[l] == 1) {
          if (infan_x[l] && (infan_state[l] != 3) && (infan_state[l] != 4) &&
              (infan_state[l] != 5))
            objective_missed = 1;
          else {
            tempt += 1;
            infantry_score += 1;
          }
        }

        if (leveldata.struct_type[l] == 2) {
          if (!infan_x[l] || (infan_state[l] == 3) || (infan_state[l] == 4) ||
              (infan_state[l] == 5))
            objective_missed = 1;
        }

        if (player_sides[leveldata.struct_owner[l]] == 0) {
          if (!infan_x[l] || (infan_state[l] == 3) || (infan_state[l] == 4) ||
              (infan_state[l] == 5)) {
            tempt -= 2;
            infantry_score -= 2;
          }
        }

        if (player_sides[leveldata.struct_owner[l]] == 1) {
          if (!infan_x[l] || (infan_state[l] == 3) || (infan_state[l] == 4) ||
              (infan_state[l] == 5)) {
            tempt += 4;
            infantry_score += 4;
          }
        }

      } else {
        if (structures[l][1] == nullptr)
          continue;

        if (leveldata.struct_type[l] == 1) {
          if (!struct_state[l])
            objective_missed = 1;
          else {
            tempt += 5;
            struct_score += 5;
          }
        }

        if (leveldata.struct_type[l] == 2) {
          if (struct_state[l])
            objective_missed = 1;
        }

        if (leveldata.struct_owner[l] != 4 &&
            player_sides[leveldata.struct_owner[l]] == 0) {
          if (struct_state[l]) {
            tempt -= 20;
            struct_score -= 20;
          }
        }

        if (player_sides[leveldata.struct_owner[l]] == 1) {
          if (struct_state[l]) {
            tempt += 7;
            struct_score += 7;
          }
        }
      }
    }

    for (l = 0; l < MAX_AA_GUNS; l++) {
      if (!kkbase_x[l])
        continue;

      if (kkbase_mission[l] == 1) {
        if (kkbase_status[l] != 2) {
          objective_missed = 1;
        } else {
          tempt += 5;
          aaa_score += 5;
        }
      }

      if (kkbase_mission[l] == 2) {
        if (kkbase_status[l] == 2)
          objective_missed = 1;
      }

      if (player_sides[leveldata.struct_owner[kkbase_number[l]]] == 0) {
        if (kkbase_status[l] == 2) {
          tempt -= 10;
          aaa_score -= 10;
        }
      }

      if (player_sides[leveldata.struct_owner[kkbase_number[l]]] == 1) {
        if (kkbase_status[l] == 2) {
          tempt += 6;
          aaa_score += 6;
        }
      }
    }

    grid2->printf(sisennys, 25, "%s",
                  mission_names[solo_mission + solo_country * 6]);

    frost->printf(110, 60, "AAA score :");
    frost->printf(210, 60, "%-6d", aaa_score);

    frost->printf(110, 70, "Infantry score :");
    frost->printf(210, 70, "%-6d", infantry_score);

    frost->printf(110, 80, "Structure score :");
    frost->printf(210, 80, "%-6d", struct_score);

    some_score =
        player_points[solo_country] * 25 + number_of_planes[solo_country] * 20;
    tempt += some_score;

    frost->printf(110, 90, "Plane score :");
    frost->printf(210, 90, "%-6d", some_score);

    some_score = 110 - mission_duration / 42;
    tempt += some_score;

    frost->printf(110, 100, "Time score :");
    frost->printf(210, 100, "%-6d", some_score);

    some_score = -(player_fired[solo_country] >> 5);
    some_score -= player_bombed[solo_country] >> 1;
    tempt += some_score;

    frost->printf(110, 110, "Ammunition score :");
    frost->printf(210, 110, "%-6d", some_score);

    some_score = solo_mission * 25;
    tempt += some_score;

    frost->printf(110, 120, "Difficulty bonus :");
    frost->printf(210, 120, "%-6d", some_score);

    if (!objective_missed && (number_of_planes[solo_country] >= 0) &&
        (!mission_interrupted)) {
      if (tempt < 1)
        tempt = 1;

      for (l = 0; l < MAX_PLAYERS_IN_ROSTER; l++) {
        if (!roster[l].pilotname[0])
          break;

        if (roster[l].solo_mis_scores[solo_country][solo_mission] >
            best_in_record)
          best_in_record =
              roster[l].solo_mis_scores[solo_country][solo_mission];
      }

      roster[config.player_number[solo_country]].solo_mis_success++;
      if (tempt > 0 && solo_mission == 5 &&
          !roster[config.player_number[solo_country]]
               .solo_mis_scores[solo_country][solo_mission]) {
        need_for_letter = 1;
      }

      if (tempt > roster[config.player_number[solo_country]]
                      .solo_mis_scores[solo_country][solo_mission])
        roster[config.player_number[solo_country]]
            .solo_mis_scores[solo_country][solo_mission] = tempt;
      x_coord =
          grid2->printf(35, 160, "Mission Successful. Total score: %d.", tempt);

      if (best_in_record < tempt) {
        grid2->printf(x_coord + 10, 160, "(New Record) (Old %d)",
                      best_in_record);

      } else {
        if (best_in_record == tempt)
          grid2->printf(x_coord + 10, 160, "(Same as old record)");
        else
          grid2->printf(x_coord + 10, 160, "(Record is %d)", best_in_record);
      }

      mission_success = 1;
    } else {
      tempt = 0;
      grid2->printf(35, 160, "Mission failed. Total score: 0.");
    }

    do_all();

    if (tempt < 0)
      tempt = 0;

    roster[config.player_number[solo_country]].solo_mis_totals += tempt;

    if (tempt > 0) {
      hiiri_to(62, 195);

    } else {
      hiiri_to(80, 195);
    }

    tempt = 0;
  }

  if (show_it_all) {
    standard_background = std::make_unique<Bitmap>(0, 0, 320, 200);
    do_all_clear();
  }

  for (l = 0; l < 4; l++) {
    if (config.player_type[l] == 1 && config.player_number[l] != -1) {

      roster[config.player_number[l]].solo_mis_flown++;

      tempt = 0;
      for (l2 = 0; l2 < 16; l2++) {
        if (player_sides[l2] == player_sides[l])
          continue;

        tempt += player_shots_down[l][l2];
      }

      roster[config.player_number[l]].solo_mis_drops += tempt;

      tempt = 0;
      for (l2 = 0; l2 < 16; l2++) {

        tempt += player_shots_down[l2][l];
      }

      roster[config.player_number[l]].solo_mis_dropped += tempt;

      roster[config.player_number[l]].solo_mis_shotsf += player_fired[l];
      roster[config.player_number[l]].solo_mis_shotshit += player_hits[l];

      roster[config.player_number[l]].solo_mis_bombs += player_bombed[l];
      roster[config.player_number[l]].solo_mis_bombshit += player_bomb_hits[l];
    }

    if (config.player_type[l] == 3 && config.player_number[l] != -1) {
      roster[config.player_number[l]].multi_mis_flown++;

      tempt = 0;
      for (l2 = 0; l2 < 4; l2++) {
        if (l2 == l)
          continue;

        tempt += player_shots_down[l][l2];
      }

      roster[config.player_number[l]].multi_mis_drops += tempt;

      tempt = 0;
      for (l2 = 0; l2 < 4; l2++) {

        tempt += player_shots_down[l2][l];
      }

      for (l2 = 0; l2 < 4; l2++) {
        if (player_points[l2] > player_points[l])
          break;
      }
      if (l2 == 4)
        roster[config.player_number[l]].multi_mis_success++;

      roster[config.player_number[l]].multi_mis_dropped += tempt;

      roster[config.player_number[l]].multi_mis_shotsf += player_fired[l];
      roster[config.player_number[l]].multi_mis_shotshit += player_hits[l];

      roster[config.player_number[l]].multi_mis_bombs += player_bombed[l];
      roster[config.player_number[l]].multi_mis_bombshit += player_bomb_hits[l];
    }
  }

  if (show_it_all) {
    l = 1;

    while (l == 1) {
      if (kbhit()) {
        if ((ch = getch()) == 27) {
          l = 0;
        }

        if (ch == 13) {
          l = 2;
          if (!need_for_letter) {
            if (!mission_success)
              mission_re_fly = solo_mission;
            else
              mission_re_fly = 999;
          }
        }
      }

      koords(&x, &y, &n1, &n2);

      if (n1 || n2) {
        if (playing_solo) {
          if (x >= 72 && y >= 191 && x <= 88 && y <= 199) {

            fly->blit(72, 191);

            if (n1)
              l = 0;
            else
              l = 2;

            if (!need_for_letter) {
              if (!mission_success)
                mission_re_fly = solo_mission;
              else
                mission_re_fly = 999;
            }
          }

          if (x >= 54 && y >= 191 && x <= 70 && y <= 199) {
            exitkuva->blit(54, 191);

            if (n1)
              l = 0;
            else
              l = 2;
          }

        } else {

          l = 2;
        }
        cursor->blit(x - 10, y - 10);
        do_all_clear();
        wait_mouse_relase();
      } else {
        cursor->blit(x - 10, y - 10);
        do_all_clear();
      }
    }

    standard_background = nullptr;

    if (!findparameter("-debugnoaftermathfadeout") && l == 0)
        gfx::fades::random_fade_out();
  }
  save_roster();
  init_vga("PALET5");
  if (need_for_letter)
    letter_menu();
}

/* A copy of the picture mirrored left to right (mirror_x) or upside down
 * (mirror_y), for the sprites that the data holds facing one way only. */
static std::unique_ptr<Bitmap> mirror_x(Bitmap *source, const char *name) {
  int w, h;
  unsigned char *src = source->info(&w, &h);
  unsigned char *dst = (unsigned char *)util::wutil::walloc(w * h);
  std::unique_ptr<Bitmap> pic = std::make_unique<Bitmap>(w, h, dst, name, true);

  for (int x = 0; x < w; x++)
    for (int y = 0; y < h; y++)
      dst[(w - 1 - x) + y * w] = src[x + y * w];

  return pic;
}

static std::unique_ptr<Bitmap> mirror_y(Bitmap *source, const char *name) {
  int w, h;
  unsigned char *src = source->info(&w, &h);
  unsigned char *dst = (unsigned char *)util::wutil::walloc(w * h);
  std::unique_ptr<Bitmap> pic = std::make_unique<Bitmap>(w, h, dst, name, true);

  for (int x = 0; x < w; x++)
    for (int y = 0; y < h; y++)
      dst[x + (h - 1 - y) * w] = src[x + y * w];

  return pic;
}

/* Sprite of plane colour l1 at angle index angle (0-60, 6 degrees each),
 * roll frame l3, flying right (side 0) or left (side 1). */
static void set_plane_sprite(int l1, int angle, int l3, int side,
                             std::unique_ptr<Bitmap> pic) {
  plane_p[l1][angle][l3][side] = pic->info();
  planes[l1][angle][l3][side] = std::move(pic);
}

void load_up() {
  int l, l1, l2, l3;
  unsigned char *point1;

  loading_text("DKS-file directory structure loaded.");
  loading_text("Loading Trigonometric functions.");

    util::wutil::init_trigs();

  if (!findparameter("-debugnofonts")) {

    loading_text("Loading fonts.");
    frost = std::make_unique<gfx::font::Font>("GRFONT");
    frost->scale();
    fontti = std::make_unique<gfx::font::Font>("FONTT");
    grid2 = std::make_unique<gfx::font::Font>("G2FONT");
    grid2->scale();
  }

  loading_text("Loading and initializing board-graphics.");
  board = std::make_unique<Bitmap>("BOARD", 0);
  boards[0] = std::make_unique<Bitmap>(2, 90, 159, 12, board);
  boards[1] = std::make_unique<Bitmap>(162, 90, 158, 12, board);
  boards[2] = std::make_unique<Bitmap>(2, 188, 159, 12, board);
  boards[3] = std::make_unique<Bitmap>(162, 188, 158, 12, board);

  closed = std::make_unique<Bitmap>("CLOSED", 0);
  board2 = std::make_unique<Bitmap>("BOARD2", 0);

  loading_text("Loading status icons.");
  plane1 = std::make_unique<Bitmap>("STATUS");

  status_icons[0][0] = std::make_unique<Bitmap>(1, 1, 31, 11, plane1);
  status_icons[0][1] = std::make_unique<Bitmap>(1, 13, 31, 11, plane1);
  status_icons[1][0] = std::make_unique<Bitmap>(33, 1, 31, 11, plane1);
  status_icons[1][1] = std::make_unique<Bitmap>(33, 13, 31, 11, plane1);

  loading_text("Loading smoke.");

  plane1 = std::make_unique<Bitmap>("SMOKE");
  for (l = 0; l < SMOKE_FRAMES; l++)
    smoke[l] = std::make_unique<Bitmap>(1 + l * 21, 1, 20, 20, plane1);

  plane1 = std::make_unique<Bitmap>("SSMOKE");
  for (l = 0; l < 17; l++)
    ssmoke[l] = std::make_unique<Bitmap>(1 + l * 10, 1, 9, 9, plane1);

  loading_text("Loading hangar.");

  hangarmenu = std::make_unique<Bitmap>("HMENU", 0);
  hangaractive = std::make_unique<Bitmap>("HACTIV");
  hangarinactive = std::make_unique<Bitmap>("HINACT");
  hruks = std::make_unique<Bitmap>("HRUKS");

  loading_text("Loading radaricons.");

  plane1 = std::make_unique<Bitmap>("RADAR");

  for (l = 0; l < 4; l++)
    for (l2 = 0; l2 < 8; l2++) {
      radar[l][l2] = std::make_unique<Bitmap>(1 + l2 * 4, 1 + l * 4, 3, 3, plane1);
    }

  loading_text("Loading waves.");

  plane1 = std::make_unique<Bitmap>("WAVE1");
  for (l = 0; l < WAVE1_FRAMES; l++)
    wave1[l] = std::make_unique<Bitmap>(1 + l * 24, 1, 23, 23, plane1);

  plane1 = std::make_unique<Bitmap>("WAVE2");
  for (l = 0; l < WAVE2_FRAMES; l++)
    wave2[l] = std::make_unique<Bitmap>(1 + l * 4, 1, 3, 5, plane1);

  loading_text("Loading flames");

  plane1 = std::make_unique<Bitmap>("FLAME");
  for (l = 0; l < NUMBER_OF_FLAMES; l++)
    flames[l] = std::make_unique<Bitmap>(1 + l * 8, 1, 7, 14, plane1);

  loading_text("Loading AAA Explosion.");

  plane1 = std::make_unique<Bitmap>("ITEXP1");
  for (l = 0; l < ITEXPLOSION_FRAMES; l++)
    itexplosion[l] = std::make_unique<Bitmap>(1 + l * 24, 1, 23, 14, plane1);

  loading_text("Loading explosion frames.");

  point1 = (unsigned char *)util::wutil::walloc(61 * 41);
  for (l = 0; l < 2501; l++)
    point1[l] = gfx::extra::converted_explosion[l];

  plane1 = std::make_unique<Bitmap>(61, 41, point1, "explosion", true);

  for (l = 0; l < 4; l++)
    for (l2 = 0; l2 < EXPLOX_FRAMES; l2++)
      explox[l][l2] = std::make_unique<Bitmap>(1 + l2 * 10, 1 + l * 10, 9, 9, plane1);

  loading_text("Loading AA-MG animations");

  plane1 = std::make_unique<Bitmap>("KKPESA");
  for (l = 0; l < 2; l++)
    for (l2 = 0; l2 < 7; l2++) {
      kkbase[0][l][l2] = std::make_unique<Bitmap>(1 + l2 * 27, 1 + l * 22, 26, 21, plane1);
    }

  for (l2 = 0; l2 < 5; l2++)
    kkbase[0][2][l2] = std::make_unique<Bitmap>(1 + l2 * 27, 1 + l * 22, 26, 21, plane1);

  loading_text("Loading AA-GUN animations");

  plane1 = std::make_unique<Bitmap>("ITGUNS");
  for (l = 0; l < 2; l++)
    for (l2 = 0; l2 < 7; l2++) {
      kkbase[1][l][l2] = std::make_unique<Bitmap>(1 + l2 * 27, 1 + l * 22, 26, 21, plane1);
    }

  for (l2 = 0; l2 < 5; l2++)
    kkbase[1][2][l2] = std::make_unique<Bitmap>(1 + l2 * 27, 1 + l * 22, 26, 21, plane1);

  loading_text("Loading hangar doors");

  plane1 = std::make_unique<Bitmap>("OVI");

  for (l = 0; l < 13; l++)
    ovi[l] = std::make_unique<Bitmap>(1 + l * 26, 1, 25, 13, plane1);

  loading_text("Loading mechanic");

  plane1 = std::make_unique<Bitmap>("MEKAN1");
  for (l = 0; l < 14; l++) {
    mekan_running[l][0] = std::make_unique<Bitmap>(1 + 14 * l, 1, 13, 11, plane1);
    mekan_running[l][1] = mirror_x(mekan_running[l][0].get(), "mekan1");
  }

  plane1 = std::make_unique<Bitmap>("MEKAN2");
  for (l = 0; l < 14; l++) {
    mekan_pushing[0][l][0] = std::make_unique<Bitmap>(1 + 14 * l, 1, 13, 11, plane1);
    mekan_pushing[0][l][1] = mirror_x(mekan_pushing[0][l][0].get(), "mekan2");
  }

  plane1 = std::make_unique<Bitmap>("MEKAN3");
  for (l = 0; l < 9; l++) {
    mekan_pushing[1][l][1] = std::make_unique<Bitmap>(1 + 14 * l, 1, 13, 11, plane1);
    mekan_pushing[1][l][0] = mirror_x(mekan_pushing[1][l][1].get(), "mekan3");
  }

  loading_text("Loading Infantry");

  if (!findparameter("-debugnoinfantry")) {

    plane1 = std::make_unique<Bitmap>("INFANT");

    for (l = 0; l < 4; l++) // Down X
    {
      infantry_dropping[l][0] = std::make_unique<Bitmap>(257, 16 + l * 45, 15, 14, plane1);
      infantry_after_drop[l][0] = std::make_unique<Bitmap>(257, 31 + l * 45, 15, 14, plane1);

      for (l2 = 0; l2 < 12; l2++)
        infantry_walking[l][0][l2] =
                std::make_unique<Bitmap>(1 + l2 * 16, 1 + l * 45, 15, 14, plane1);

      for (l2 = 0; l2 < 7; l2++)
        infantry_dying[l][0][l2] =
                std::make_unique<Bitmap>(193 + l2 * 16, 1 + l * 45, 15, 14, plane1);

      for (l2 = 0; l2 < 6; l2++) {
        infantry_aiming[l][0][l2] =
                std::make_unique<Bitmap>(1 + l2 * 16, 16 + l * 45, 15, 14, plane1);

        infantry_shooting[l][0][l2] =
                std::make_unique<Bitmap>(1 + l2 * 16, 31 + l * 45, 15, 14, plane1);
      }

      for (l2 = 0; l2 < 10; l2++) {
        infantry_wavedeath[l][0][l2] =
                std::make_unique<Bitmap>(97 + l2 * 16, 16 + l * 45, 15, 14, plane1);

        infantry_bdying[l][0][l2] =
                std::make_unique<Bitmap>(97 + l2 * 16, 31 + l * 45, 15, 14, plane1);
      }
    }

    loading_text("Mirroring infantry.");

    for (l = 0; l < 4; l++) {
      infantry_dropping[l][1] =
          mirror_x(infantry_dropping[l][0].get(), "mirr_inf_dropping");
      infantry_after_drop[l][1] =
          mirror_x(infantry_after_drop[l][0].get(), "mirr_inf_after_drop");

      for (l2 = 0; l2 < 12; l2++)
        infantry_walking[l][1][l2] =
            mirror_x(infantry_walking[l][0][l2].get(), "mirr_inf_walking");

      for (l2 = 0; l2 < 7; l2++)
        infantry_dying[l][1][l2] =
            mirror_x(infantry_dying[l][0][l2].get(), "mirr_inf_dying");

      for (l2 = 0; l2 < 6; l2++) {
        infantry_aiming[l][1][l2] =
            mirror_x(infantry_aiming[l][0][l2].get(), "mirr_inf_aiming");
        infantry_shooting[l][1][l2] =
            mirror_x(infantry_shooting[l][0][l2].get(), "mirr_inf_shooting");
      }

      for (l2 = 0; l2 < 10; l2++) {
        infantry_wavedeath[l][1][l2] =
            mirror_x(infantry_wavedeath[l][0][l2].get(), "mirr_inf_wavedeath");
        infantry_bdying[l][1][l2] =
            mirror_x(infantry_bdying[l][0][l2].get(), "mirr_inf_bdying");
      }
    }
  } // debug

  if (!findparameter("-debugnorotate")) {

    loading_text("Loading and rotating bomb.");
    bomb[0] = std::make_unique<Bitmap>("BOMB");

    for (l = 1; l < 61; l++) {
      bomb[l] = rotate_bitmap(bomb[0].get(), l * 6);
    }

    loading_text("Loading main planepicture.");
    plane1 = std::make_unique<Bitmap>("PLANES");

    /* The data holds the planes flying right. Rotating gives the angles
       from -90 to 90 degrees (indices 45-59 and 0-15), the rest comes
       from mirroring. */
    loading_text("Generating rotated pictures.");
    for (l3 = 0; l3 < 4; l3++)
      for (l2 = 0; l2 < 4; l2++) {
        set_plane_sprite(l2, 0, l3, 0,
                         std::make_unique<Bitmap>(1 + l3 * 21, 1 + l2 * 21, 20,
                                                  20, plane1));
        for (l = 1; l < 16; l++)
          set_plane_sprite(l2, l, l3, 0,
                           rotate_bitmap(planes[l2][0][l3][0].get(), l * 6));
        for (l = 45; l < 60; l++)
          set_plane_sprite(l2, l, l3, 0,
                           rotate_bitmap(planes[l2][0][l3][0].get(), l * 6));
      }

    /* Mirrored left to right the planes fly left, i.e. at 180 - angle.
       Mirrored upside down they fly right inverted, at -angle (index
       60 - l2, with 0 kept at 0). The upside down pass runs second, so
       straight up and down (15 and 45) come from it, as before. */
    for (l3 = 0; l3 < 4; l3++)
      for (l1 = 0; l1 < 4; l1++) {
        for (l2 = 0; l2 < 16; l2++)
          set_plane_sprite(l1, 30 - l2, l3, 1,
                           mirror_x(planes[l1][l2][l3][0].get(), "mirr_plane"));
        for (l2 = 45; l2 < 60; l2++)
          set_plane_sprite(l1, 90 - l2, l3, 1,
                           mirror_x(planes[l1][l2][l3][0].get(), "mirr_plane"));
      }

    for (l3 = 0; l3 < 4; l3++)
      for (l1 = 0; l1 < 4; l1++) {
        for (l2 = 0; l2 < 16; l2++)
          set_plane_sprite(l1, l2 ? 60 - l2 : 0, l3, 1,
                           mirror_y(planes[l1][l2][l3][0].get(), "mirr_plane"));
        for (l2 = 45; l2 < 60; l2++)
          set_plane_sprite(l1, 60 - l2, l3, 1,
                           mirror_y(planes[l1][l2][l3][0].get(), "mirr_plane"));
      }

    /* Planes flying left and inverted (right side up on the screen) give
       the remaining angles of the planes flying right. */
    for (l3 = 0; l3 < 4; l3++)
      for (l1 = 0; l1 < 4; l1++)
        for (l2 = 16; l2 < 45; l2++)
          set_plane_sprite(l1, 60 - l2, l3, 0,
                           mirror_y(planes[l1][l2][l3][1].get(), "mirr_plane"));

    /* Players 5-16 fly with the sprites of players 1-4. */
    static const int sprite_owner[16] = {0, 1, 2, 3, 0, 1, 2, 3,
                                         0, 1, 2, 3, 1, 2, 3, 0};
    for (l1 = 4; l1 < 16; l1++)
      for (l = 0; l < 61; l++)
        for (l2 = 0; l2 < 4; l2++)
          for (l3 = 0; l3 < 2; l3++) {
            plane_p[l1][l][l2][l3] = plane_p[sprite_owner[l1]][l][l2][l3];
            planes[l1][l][l2][l3] = planes[sprite_owner[l1]][l][l2][l3];
          }

  } // debug

  loading_text("Loading explosions.");

  plane1 = std::make_unique<Bitmap>("CRASH");
  for (l = 0; l < 6; l++)
    plane_crash[l] = std::make_unique<Bitmap>(1 + 21 * l, 1, 20, 20, plane1);

  loading_text("Loading icons.");

  bomb_icon = std::make_unique<Bitmap>("ASE1", 0);
  gas_icon = std::make_unique<Bitmap>("ASE2", 0);
  small_ammo_icon = std::make_unique<Bitmap>("ASE4", 0);
  big_ammo_icon = std::make_unique<Bitmap>("ASE3", 0);

  plane1 = std::make_unique<Bitmap>("PICONS", 0);
  for (l = 0; l < 4; l++)
      picons[l] = std::make_unique<Bitmap>(9 * l, 0, 9, 9, plane1);

  pwon = std::make_unique<Bitmap>("PWON");
  pwoff = std::make_unique<Bitmap>("PWOFF");

  loading_text("Loading small parts.");

  plane1 = std::make_unique<Bitmap>("BITES");
  for (l = 0; l < NUMBER_OF_BITES; l++)
    bites[l] = std::make_unique<Bitmap>(1 + 11 * l, 1, 10, 10, plane1);

  plane1 = std::make_unique<Bitmap>("RIFLE");
  for (l = 0; l < 12; l++)
    rifle[l] = std::make_unique<Bitmap>(1 + 9 * l, 1, 8, 8, plane1);

  loading_text("Loading menu graphics.");

  menu1 = std::make_unique<Bitmap>("MENU01");

  loading_text("Loading flags.");

  temp_bitti = std::make_unique<Bitmap>("FLAGS");

  for (l = 0; l < 4; l++) {
    for (l2 = 0; l2 < 12; l2++) {
      flags[l][l2] =  std::make_unique<Bitmap>(1 + l2 * 14, 1 + l * 10, 13, 9, temp_bitti);
    }
  }

  loading_text("Loading mouse cursor.");
  cursor = std::make_unique<Bitmap>("CURSOR");
}

void load_level() {
  int l, l2, i;
  int xx, yy;
  int c_flag;
  std::unique_ptr<Bitmap> temppic;

  loading_text("Loading levelinfo.");

  const char *level_arg = findparameter_value("-level");
  if (level_arg == nullptr) {
    if (!playing_solo) {
      snprintf(levelname, sizeof(levelname), "level%d", config.current_multilevel + 1);
    } else {
      snprintf(levelname, sizeof(levelname), "%d-%d", solo_country, solo_mission);
    }
  } else {
    snprintf(levelname, sizeof(levelname), "%s", level_arg);
  }

  if (!dksopen(levelname)) {
    loading_text("Loading external level.");
  } else {
    dksclose();
  }

  if (!extdksopen(levelname)) {
    printf("\nError locating leveldata %s.\n\n", levelname);
    exit(1);
  }

  dksread(&leveldata, sizeof(leveldata));
  dksclose();

  // Byte order conversion

  for (i = 0; i < MAX_STRUCTURES; i++) {
    leveldata.struct_x[i] = SDL_Swap32LE(leveldata.struct_x[i]);
    leveldata.struct_y[i] = SDL_Swap32LE(leveldata.struct_y[i]);
    leveldata.struct_hit[i] = SDL_Swap32LE(leveldata.struct_hit[i]);
    leveldata.struct_owner[i] = SDL_Swap32LE(leveldata.struct_owner[i]);
    leveldata.struct_type[i] = SDL_Swap32LE(leveldata.struct_type[i]);
  }

  for (i = 0; i < 4; i++) {
    leveldata.airfield_x[i] = SDL_Swap32LE(leveldata.airfield_x[i]);
    leveldata.airfield_y[i] = SDL_Swap32LE(leveldata.airfield_y[i]);
    leveldata.airfield_lenght[i] = SDL_Swap32LE(leveldata.airfield_lenght[i]);
    leveldata.plane_direction[i] = SDL_Swap32LE(leveldata.plane_direction[i]);
  }

  for (l = 0; l < 4; l++) {
    if (!leveldata.airfield_x[l])
      continue;
    leveldata.airfield_x[l] += 4;
    leveldata.airfield_lenght[l] -= 8;
  }

  for (l = 0; l < 4; l++)
    plane_present[l] = 0;

  loading_text("Loading scenery.");

  maisema = std::make_unique<Bitmap>(leveldata.pb_name, 0);

  loading_text("Loading structures.");

  for (l = 0; l < MAX_FLAGS; l++)
    flags_x[l] = 0;

  for (l = 0; l < MAX_INFANTRY; l++) {
    infan_x[l] = 0;
  }

  for (l = 0; l < MAX_AA_GUNS; l++) {
    kkbase_x[l] = 0;
  }

  for (l = 0; l < 8; l++)
    infan_stop[l] = 0;

  for (l = 0; l < MAX_STRUCTURES; l++) {
    struct_state[l] = 0;
    structures[l][1] = nullptr;
    structures[l][0] = nullptr;

    if (leveldata.struct_x[l]) {
      if (!strncmp(leveldata.pd_name[l], "FLAGS", 5)) {

        c_flag = atoi(leveldata.pd_name[l] + 5);

        for (l2 = 0; l2 < MAX_FLAGS; l2++)
          if (!flags_x[l2])
            break;

        if (l2 == MAX_FLAGS)
          continue;

        flags_x[l2] = leveldata.struct_x[l];
        flags_y[l2] = leveldata.struct_y[l];
        flags_owner[l2] = c_flag;
        continue;
      }

      if (!strncmp(leveldata.pd_name[l], "INFAN", 5)) {
        if (!playing_solo && !config.infantry)
          continue;

        infan_x[l] = leveldata.struct_x[l];
        infan_y[l] = leveldata.struct_y[l];
        infan_direction[l] = leveldata.struct_hit[l];
        infan_last_shot[l] = 0;
        infan_state[l] = 0;
        infan_country[l] = leveldata.struct_owner[l];
        infan_frame[l] = util::wutil::wrandom(12);
        continue;
      }

      if (!strncmp(leveldata.pd_name[l], "INSTOP", 6)) {

        for (l2 = 0; l2 < 8; l2++)
          if (!infan_stop[l2])
            break;

        if (l2 != 8) {
          infan_stop[l2] = leveldata.struct_x[l];
        }

        continue;
      }

      if (!strncmp(leveldata.pd_name[l], "KKBASE", 6)) {
        if (!playing_solo && !config.aa_mgs)
          continue;

        for (l2 = 0; l2 < MAX_AA_GUNS; l2++)
          if (!kkbase_x[l2])
            break;

        if (l2 == MAX_AA_GUNS)
          continue;

        kkbase_x[l2] = leveldata.struct_x[l];
        kkbase_y[l2] = leveldata.struct_y[l];
        kkbase_last_shot[l2] = 0;
        kkbase_status[l2] = 0;
        kkbase_country[l2] = leveldata.struct_owner[l];
        kkbase_frame[l2] = 3;
        kkbase_shot_number[l2] = 0;
        kkbase_type[l2] = 0;
        kkbase_mission[l2] = leveldata.struct_type[l];
        kkbase_number[l2] = l;
        continue;
      }

      if (!strncmp(leveldata.pd_name[l], "ITGUN", 5)) {
        if (!playing_solo && !config.it_guns)
          continue;

        for (l2 = 0; l2 < MAX_AA_GUNS; l2++)
          if (!kkbase_x[l2])
            break;

        if (l2 == MAX_AA_GUNS)
          continue;

        kkbase_x[l2] = leveldata.struct_x[l];
        kkbase_y[l2] = leveldata.struct_y[l];
        kkbase_last_shot[l2] = 0;
        kkbase_status[l2] = 0;
        kkbase_country[l2] = leveldata.struct_owner[l];
        kkbase_frame[l2] = 3;
        kkbase_shot_number[l2] = 0;
        kkbase_type[l2] = 1;
        kkbase_mission[l2] = leveldata.struct_type[l];
        kkbase_number[l2] = l;
        continue;
      }

      if (leveldata.struct_hit[l]) {
        structures[l][0] = std::make_unique<Bitmap>(leveldata.pd_name[l]);

      } else {
        for (l2 = 0; l2 < NUMBER_OF_STRUCT_NAMES; l2++)
          if (!strcmp(struct_names[l2 * 2], leveldata.pd_name[l]))
            break;

        if (l2 == NUMBER_OF_STRUCT_NAMES) {
          structures[l][0] = std::make_unique<Bitmap>(leveldata.pd_name[l]);
          structures[l][0]->info(&struct_width[l], &struct_heigth[l]);

        } else {
          /* A structure that can be destroyed is drawn in both states on
             a copy of the scenery behind it, so that drawing the ruin
             replaces the whole building. */
          temppic = std::make_unique<Bitmap>(leveldata.pd_name[l]);
          temppic->info(&struct_width[l], &struct_heigth[l]);

          structures[l][0] =
                  std::make_unique<Bitmap>(leveldata.struct_x[l], leveldata.struct_y[l],
                         struct_width[l], struct_heigth[l], maisema);

          temppic->blit_to_bitmap(structures[l][0], 0, 0);

          temppic = std::make_unique<Bitmap>(struct_names[l2 * 2 + 1]);

          structures[l][1] =
                  std::make_unique<Bitmap>(leveldata.struct_x[l], leveldata.struct_y[l],
                         struct_width[l], struct_heigth[l], maisema);

          temppic->blit_to_bitmap(structures[l][1], 0, 0);
        }
      }
    }
  }

  for (l = 0; l < 4; l++) {
    if (!leveldata.airfield_x[l]) {
      hangar_x[l] = 0;
      hangar_y[l] = 0;

    } else {
      for (l2 = 0; l2 < MAX_STRUCTURES; l2++) {
        if (!strcmp(leveldata.pd_name[l2], "HANGAR")) {
          if (leveldata.struct_x[l2] + 4 >= leveldata.airfield_x[l])
            if (leveldata.struct_x[l2] <=
                (leveldata.airfield_x[l] + leveldata.airfield_lenght[l]))
              break;
        }
      }

      if (l2 == MAX_STRUCTURES) {
        hangar_x[l] = 0;
        hangar_y[l] = 0;

      } else {
        hangar_x[l] = leveldata.struct_x[l2];
        hangar_y[l] = leveldata.struct_y[l2];
      }
    }
  }

  loading_text("Updating terrainpointer.");

  level_bitmap = maisema->info(&xx, &yy);

  if (current_mode == VGA_MODE && !findparameter("-debugnographics")) {
    loading_text("Initializing screen.");
    if (!findparameter("-black"))
      init_vga("PALET5");
    else
      init_vga("PALET3");
  }
}

/* Plane l flies with the stats of plane type src. */
static void copy_plane_stats(int l, int src) {
  plane_power[l] = t_plane_power[src];
  plane_manover[l] = t_plane_manover[src];
  plane_mass[l] = t_plane_mass[src];
  plane_bombs[l] = t_plane_bombs[src];
  plane_gas[l] = t_plane_gas[src];
  plane_ammo[l] = t_plane_ammo[src];
}

void init_data() {
  int l, l2;

  record_counter = 0;

  for (l = 0; l < 4; l++) {
    hangar_door_frame[l] = 0;
    hangar_door_closing[l] = 0;
    hangar_door_opening[l] = 0;
    mekan_x[l] = 0;
    mekan_y[l] = leveldata.airfield_y[l] - 11;
    mekan_frame[l] = 0;
    mekan_status[l] = 0;
    mekan_target[l] = 0;
    mekan_subtarget[l] = 0;
    mekan_direction[l] = 0;
    mekan_mission[l] = 0;
  }

  for (l = 0; l < 16; l++) {
    mc_up[l] = 0;
    mc_down[l] = 0;
    mc_bomb[l] = 0;
    mc_roll[l] = 0;
    mc_guns[l] = 0;
    mc_power[l] = 0;

    new_mc_up[l] = 0;
    new_mc_down[l] = 0;
    new_mc_bomb[l] = 0;
    new_mc_roll[l] = 0;
    new_mc_guns[l] = 0;
    new_mc_power[l] = 0;

    roll_key_down[l] = 0;
    bomb_key_down[l] = 0;
    hangarkey_up_down[l] = 0;
    hangarkey_down_down[l] = 0;
    hangarkey_right_down[l] = 0;
    hangarkey_left_down[l] = 0;

    player_was_on_airfield[l] = 0;
    plane_wants_in[l] = 0;
    plane_wants_out[l] = 0;
  }

  for (l = 0; l < MAX_STRUCTURES; l++)
    bombs_going[l] = -1;

  for (l = 0; l < MAX_FLAMES; l++)
    flame_x[l] = 0;

  for (l = 0; l < MAX_FLAGS; l++) {
    flags_state[l] = 0;
    flags_frame[l] = util::wutil::wrandom(10);
  }

  for (l = 0; l < MAX_ITGUN_SHOTS; l++)
    itgun_shot_x[l] = 0;

  for (l = 0; l < MAX_BOMBS; l++)
    bomb_x[l] = 0;

  for (l = 0; l < MAX_SHOTS; l++)
    shots_flying_x[l] = 0;

  for (l = 0; l < MAX_FLYING_OBJECTS; l++)
    fobjects[l].x = 0;

  if (playing_solo) {
    for (l = 0; l < 12; l++) {
      player_exists[l] = 0;
      computer_active[l] = 0;
      if (number_of_planes[l]) {
        player_exists[l] = 1;
        if (l != solo_country) {
          computer_active[l] = 1;

        } else {
          computer_active[l] = 0;
        }
      }
    }

    for (l = 0; l < 4; l++)
      copy_plane_stats(l, l);
  } else {
    /* In multiplayer every plane can be set to the same type. */
    for (l = 0; l < 16; l++)
      copy_plane_stats(l, config.all_planes_are ? config.all_planes_are - 1
                                                : l);

    /* Alliances: which of the four colours (l & 3) is on which side. */
    static const int alliance_sides[3][4] = {
        {0, 0, 1, 1}, {0, 1, 0, 1}, {0, 1, 1, 0}};
    if (config.alliance >= 1 && config.alliance <= 3)
      for (l = 0; l < 16; l++)
        player_sides[l] = alliance_sides[config.alliance - 1][l & 3];
  }

  for (l = 0; l < 16; l++) {
    player_x_8[l] = 25000;
    player_y_8[l] = 1000;
    player_x[l] = 2500000;
    player_y[l] = 250000;
    plane_present[l] = 0;
    plane_coming[l] = 0;
    player_on_airfield[l] = 0;
    player_fired[l] = 0;
    player_hits[l] = 0;
    player_bombed[l] = 0;
    player_bomb_hits[l] = 0;

    player_speed[l] = 0;
    player_angle[l] = 0;
    player_upsidedown[l] = 0;
    player_rolling[l] = 0;
    player_spinning[l] = 0;
    spinning_remaining[l] = 0;
    player_x_speed[l] = 0;
    player_y_speed[l] = 0;
    player_last_shot[l] = 0;

    hangarmenu_active[l] = 0;
    hangarmenu_position[l] = 0;

    if (playing_solo) {
      switch (fighter[l]) {
      case 0:
        hangarmenu_max_ammo[l] = 0;
        hangarmenu_bombs[l] = hangarmenu_max_bombs[l] = plane_bombs[l];
        break;
      case 1:
        hangarmenu_max_ammo[l] = plane_ammo[l];
        hangarmenu_max_bombs[l] = plane_bombs[l];
        break;
      case 2:
        hangarmenu_max_ammo[l] = plane_ammo[l];
        hangarmenu_max_bombs[l] = 0;
        break;
      }
    } else {
      hangarmenu_max_ammo[l] = plane_ammo[l];
      hangarmenu_max_bombs[l] = plane_bombs[l];
    }

    hangarmenu_max_gas[l] = plane_gas[l];

    // Set sane default amounts (max value / 2 + 1)
    hangarmenu_gas[l] = hangarmenu_max_gas[l] / 2 + 1;
    hangarmenu_ammo[l] = hangarmenu_max_ammo[l];
    hangarmenu_bombs[l] = hangarmenu_max_bombs[l];

    if (hangarmenu_ammo[l])
      hangarmenu_ammo[l] = hangarmenu_ammo[l] / 2 + 1;
    if (hangarmenu_bombs[l])
      hangarmenu_bombs[l] = hangarmenu_bombs[l] / 2 + 1;

    for (l2 = 0; l2 < 16; l2++)
      player_shots_down[l][l2] = 0;
  }
}

void handle_parameters() {
  if (findparameter("-reverse"))
    reverbed_channels = 1;

  if ((findparameter("-sound")) || (findparameter("-l"))) {
    is_there_sound = 1;
    config.sound_on = 1;
    loading_text("Sounds enabled.");
  }

  if ((findparameter("-nosound")) || (findparameter("-s"))) {
    is_there_sound = 0;
    config.sound_on = 0;
    loading_text("Sounds disabled.");
  }

  if (findparameter("-2")) {
    pixel_multiplier_vga = 2;
  }

  if (findparameter("-3")) {
    pixel_multiplier_vga = 3;
  }

  if (findparameter("-4")) {
    pixel_multiplier_vga = 4;
  }

  if (findparameter("-2svga")) {
    pixel_multiplier_svga = 2;
  }

  if ((findparameter("-fullscreen")) || (findparameter("-f"))) {
    wantfullscreen = 1;
  }

  if ((findparameter("-nofullscreen")) || (findparameter("-w"))) {
    wantfullscreen = 0;
  }

  if (findparameter("-sdldraw")) {
    draw_with_vircr_mode = 0;
  }

  if (findparameter("-nosplit")) {
    split_num = 0;
    init_resolution(2400, 208);
  }

  if (findparameter("-1split")) {
    split_num = 1;
    init_resolution(1200, 404);
  }
}

void init_resolution(int width, int height) {
  screen_width = width;
  screen_height = height;
  screen_width_less = screen_width - 1;
  screen_height_less = screen_height - 1;
}

int main(int argc, char *argv[]) {
  int x, y, n1, n2;
  int laskuri;
  FILE *faili;
  std::unique_ptr<Bitmap> lakuva1;

  parametrit = argv;
  parametri_kpl = argc;

  if (findparameter("-?") || findparameter("-h") || findparameter("--help") ||
      findparameter("-help")) {
    printf("Triplane Classic " TRIPLANE_VERSION
           " - a side-scrolling dogfighting game.\n");
    printf("Copyright (C) 1996,1997,2009  Dodekaedron Software Creations Oy\n");
    printf("This program is free software; you may redistribute it under the "
           "terms of\n");
    printf("the GNU General Public License version 3 or (at your option) a "
           "later version.\n");
    printf("This program has absolutely no warranty.\n\n");
    printf("-help,-h         Help on options\n");
    printf("-fullscreen,-f   Start game in fullscreen mode (default)\n");
    printf("-nofullscreen,-w Start game in windowed mode\n");
    printf("-nosound,-s      Start game without sounds and it will be saved in "
           "the setting\n");
    printf("-sound,-l        Start game with sounds and it will be saved in "
           "the setting\n");
    printf(
        "-2, -3, -4       Zoom the 320x200-pixel game window 2x, 3x or 4x\n");
    printf("-2svga           Zoom the 800x600-pixel window 2x to produce "
           "1600x1200-pixel window\n");
    printf("-nosplit         Use the 2400x200-pixel window instead of 800x600 "
           "to have window with no splits\n");
    printf("-1split          Use the 1200x400-pixel window instead of 800x600 "
           "to have window with 1 split only\n\n");
    exit(0);
  }

  if (findparameter("-loadtexts"))
    loading_texts = 1;

  if (!dksinit(DKS_FILENAME)) {
    printf("\n\nError locating main datafile\n");
    exit(1);
  }

  util::wutil::wrandom_sanity_check();

  load_config();

  loading_text("Starting parameter handling.");
  handle_parameters();

  /* needed to find joysticks */
  /* needs draw_with_vircr_mode parameter from handle_parameters() */
  init_video();

  loading_text("Looking for joystick");
  if (!(joystick_exists = init_joysticks())) {
    loading_text("Joystick(s) not detected and thus not calibrated.");
  }

  if (config.joystick_calibrated[1] || config.joystick_calibrated[0] ||
      config.joystick_calibrated[2] || config.joystick_calibrated[3] ||
      config.joystick_calibrated[4]) {
    if (!load_joysticks_data(CALIBRATION_FILENAME)) {
      for (int i = 0; i < 5; i++) {
        config.joystick_calibrated[i] = 0;
        config.joystick[i] = -1;
      }

      loading_text("Unable to load calibration data.");
    }
  }

  loading_text("\nLoading keyset.");

  if (!findparameter("-debugnokeyset"))
    load_keyset();

  if (findparameter("-speed")) {
    io::timing::nopeuskontrolli_enable(0);
  }

  if (is_there_sound) {
    loading_text("Initializing sounds.");
    init_sounds(); /* can zero is_there_sound */
  }

  if (is_there_sound) {
    if (config.music_on) {
      loading_text("Loading music.");
      load_music();
    }

    if (config.sfx_on) {
      loading_text("Loading samples.");
      load_sfx();
    }

    loading_text("Soundsystem initialized correctly.\n\n");
  }

  init_vga("PALETD");
  lakuva1 = std::make_unique<Bitmap>("STARTD", 0);

  lakuva1->blit(0, 0);
  do_all();

  if (sfx_loaded && config.sound_on) {
    play_sample(sample_alku);
  }

  loading_text("\nData loading started.");
  load_up();

  if (loading_texts) {
    printf("\nLoading complete. Press a key to continue.");
    fflush(stdout);
  }

  n1 = 0;
  while (!n1 && !findparameter("-autostart") && !findparameter("-nosplash")) {
    if (kbhit())
      break;

    koords(&x, &y, &n1, &n2);

    if (n1 || n2) {
      wait_mouse_relase();
      break;
    }
  }

  while (kbhit() && !findparameter("-autostart"))
    getch();

  if (sfx_loaded) {
    sample_alku->right_volume = 0;
    sample_alku->left_volume = 0;
  }

  loading_text("Loading roster.");
  load_roster();

  loading_text("\nInitializing VGA and starting game.");
  if (!findparameter("-debugnographics"))
    init_vga("PALET5");

  while (kbhit())
    getch();

  main_menu();
  save_roster();

  if (music_loaded) {
    sdl_stop_music();
    clear_music();
  }

  if (sfx_loaded) {
    clear_sfx();
  }

  if (is_there_sound) {
    uninit_sounds();
  }

  if (findparameter("-writehsfile")) {
    faili = settings_open("higsscor.txt", "wt");

    fprintf(faili,
            "Triplane Classic Acelist. Total highscores in Sologame\n\n");

    for (laskuri = 0; laskuri < MAX_PLAYERS_IN_ROSTER; laskuri++) {

      if (!roster[laskuri].pilotname[0])
        break;

      x = 0;

      fprintf(faili, "Scores for %s :\n", roster[laskuri].pilotname);

      for (n1 = 0; n1 < 4; n1++)
        for (n2 = 0; n2 < 6; n2++) {
          x += roster[laskuri].solo_mis_scores[n1][n2];
          fprintf(faili, "%d/%d  : %d\n", n1, n2,
                  roster[laskuri].solo_mis_scores[n1][n2]);
        }

      fprintf(faili, "-------------------------------\nTotal : %d\n\n", x);
    }

    fclose(faili);
  }

  save_config();

  return 0;
}

void loading_text(const char *teksti) {
  if (loading_texts) {
    printf("%s\n", teksti);
  }
}

int get_player_fired(int player) {
  return playing_solo || player > 3
             ? player_fired[player]
             : player_fired[player] + player_fired[player + 4] +
                   player_fired[player + 8];
}

int get_player_hits(int player) {
  return playing_solo || player > 3
             ? player_hits[player]
             : player_hits[player] + player_hits[player + 4] +
                   player_hits[player + 8];
}
int get_player_shots_down(int player, int player2) {
  return playing_solo || player > 3 || player2 > 3
             ? player_shots_down[player][player2]
             : player_shots_down[player][player2] +
                   player_shots_down[player][player2 + 4] +
                   player_shots_down[player][player2 + 8] +
                   player_shots_down[player + 4][player2] +
                   player_shots_down[player + 4][player2 + 4] +
                   player_shots_down[player + 4][player2 + 8] +
                   player_shots_down[player + 8][player2] +
                   player_shots_down[player + 8][player2 + 4] +
                   player_shots_down[player + 8][player2 + 8];
}

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

/* Triplane Turmoil menusystem */

#include "tripmenu.h"
#include "../io/joystick.h"
#include "../io/sdl_compat.h"
#include "../io/trip_io.h"
#include "../settings.h"
#include "../triplane.h"
#include "../util/wutil.h"
#include "../world/plane.h"
#include "../world/tripmis.h"
#include "../world/tripaudio.h"
#include <SDL3/SDL.h>
#include <array>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iterator>
#include <memory>
#include "../gfx/fades.h"
#include "../gfx/font.h"
#include "../io/dksfile.h"
#include "../world/terrain.h"

extern sb_mod_file *triplane_mod;
extern sb_mod_file *aces_mod;
extern sb_sample *sample_hurr;


static char mission_description[80 * 16];
static char mission_lines[16][80];
static int mission_pixels[16];
static int mission_headline_pixels;

static int aces_number_of_entries;
static int aces_score[MAX_PLAYERS_IN_ROSTER];

constexpr int32_t CHARS_PER_LINE = 70;

/* The game (src/triplane.cpp) sets mission_re_fly to this when the player
 * returns to the mission menu instead of re-flying a mission. */
constexpr int32_t MISSION_RE_FLY_BACK_TO_MENU = 999;

/* The pilot the roster menu falls back to when a pilot cannot be created
 * because the roster is full. */
constexpr int32_t ROSTER_FULL_FALLBACK_PILOT = 99;

static sb_mod_file *national_mod = nullptr;

static const char *const rank_names[6] = {"2nd Lt.", "1st Lt.", "Capt.",
                                          "Major",   "Lt Col.", "Colonel"};

static constexpr const char *country_names[4] = {"German", "Finnish",
                                                 "English", "Japanese"};
static constexpr const char *national_modnames[4] = {"mgerma", "mfinla",
                                                     "mengla", "mjapan"};

/* A key the roster and controls menus ask for: the prompt and the key
 * field of a pilot (rosteri) or a player (keymap). */
template <typename Keys>
struct key_prompt {
  const char *text;
  int32_t Keys::*key;
};

/* The six keys in the order they are asked. */
static constexpr key_prompt<rosteri> roster_key_prompts[] = {
    {"Key for upward turn", &rosteri::up},
    {"Key for downward turn", &rosteri::down},
    {"Key for roll", &rosteri::roll},
    {"Key for engine power", &rosteri::power},
    {"Key for bomb drop", &rosteri::bombs},
    {"Key for guns", &rosteri::guns},
};

static constexpr key_prompt<keymap> player_key_prompts[] = {
    {"Key for upward turn", &keymap::up},
    {"Key for downward turn", &keymap::down},
    {"Key for roll", &keymap::roll},
    {"Key for engine power", &keymap::power},
    {"Key for bomb drop", &keymap::bombs},
    {"Key for guns", &keymap::guns},
};

/* joystick_config[0..3] are the multiplayer joysticks, joystick_config[4]
 * the solo game joystick of the roster menu. */
static constexpr int ROSTER_JOYSTICK = 4;

/**************************** Functions ***************************************/

static void joystick_setup(int joy, Bitmap *background, int text_x,
                           int text_y);
static void print_clear_roster(Bitmap *rosteri);
static void print_filled_roster(int number);

/* The national anthems in the solo game menus are only played when a
 * sound card was found, the player wants music and -nomusic is not given. */
static bool national_music_enabled() {
  return is_there_sound && config.music_on && !findparameter("-nomusic");
}

static int get_rank(const int player) {
  int l, l2, l3;

  l = roster[player].solo_mis_flown + roster[player].multi_mis_flown;

  for (l2 = 0; l2 < 4; l2++)
    for (l3 = 0; l3 < 6; l3++)
      l += roster[player].solo_mis_scores[l2][l3];

  l += roster[player].solo_mis_totals >> 2;
  l += roster[player].multi_mis_success * 3;
  l += roster[player].multi_mis_drops * 5;

  if (l < 1000)
    return 0;
  if (l < 5000)
    return 1;
  if (l < 10000)
    return 2;
  if (l < 20000)
    return 3;
  if (l < 50000)
    return 4;

  return 5;
}

static int calculate_multitotal(const int player) {
  int ts = 0;

  if (!roster[player].pilotname[0])
    return 0;

  ts += roster[player].multi_mis_flown;
  ts += roster[player].multi_mis_success * 10;
  ts += roster[player].multi_mis_drops * 4;
  ts -= roster[player].multi_mis_dropped;
  ts -= roster[player].multi_mis_shotsf / 128;
  ts += roster[player].multi_mis_shotshit / 60;
  ts -= roster[player].multi_mis_bombs / 12;
  ts += roster[player].multi_mis_bombshit / 6;

  return ts;
}

static void sort_and_show(const int percent) {
  int alkuosa, loppuosa;
  int flag = 1;
  int aces_number[MAX_PLAYERS_IN_ROSTER];
  int c;
  int temp;

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++)
    aces_number[c] = c;

  while (flag) {
    flag = 0;

    for (c = 0; c < (aces_number_of_entries - 1); c++) {
      if (aces_score[c] < aces_score[c + 1]) {
        flag = 1;

        temp = aces_score[c];
        aces_score[c] = aces_score[c + 1];
        aces_score[c + 1] = temp;

        temp = aces_number[c];
        aces_number[c] = aces_number[c + 1];
        aces_number[c + 1] = temp;
      }
    }
  }

  if (aces_number_of_entries > 10)
    aces_number_of_entries = 10;

  int c2 = 0;
  for (c = 0; c < aces_number_of_entries; c++) {
    if (!aces_score[c])
      continue;

    frost->printf(85, 90 + c2 * 9, "%2d.", c2 + 1);
    frost->printf(95, 90 + c2 * 9, "%-35s", roster[aces_number[c]].pilotname);
    frost->unscale();
    if (!percent)
      frost->printf(190, 90 + c2 * 9, "%6d", aces_score[c]);
    else {
      alkuosa = aces_score[c] / 100;
      loppuosa = aces_score[c] - alkuosa * 100;
      loppuosa /= 10;
      frost->printf(190, 90 + c2 * 9, "%d.%d%%", alkuosa, loppuosa);
    }
    frost->scale();
    c2++;
  }
}

/* Draws one TOP10 page of the aces menu: the title at (title_x, 45) (no
 * title for the grand total, whose title is part of the ACESB bitmap),
 * then the active pilots sorted by score(c). With percent the scores are
 * hundredths of a percent. */
template <typename Score>
static void aces_show(const int title_x, const char *title, const int percent,
                      Score score) {
  int c;

  if (title)
    frost->printf(title_x, 45, "%s", title);

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = score(c);
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show(percent);
  }
}

struct aces_page {
  const char *title;
  int percent;
  int (*score)(int c);
};

/* Pages 0-2 and 27-33 of the aces menu. Pages 3-26 are the solo missions,
 * see aces_one_solo. */
static constexpr aces_page aces_pages[] = {
    {nullptr, 0,
     [](int c) -> int {
       return calculate_multitotal(c) + roster[c].solo_mis_totals;
     }},
    {"Multiplayer TOP10", 0,
     [](int c) -> int { return calculate_multitotal(c); }},
    {"Sologame TOP10", 0,
     [](int c) -> int { return roster[c].solo_mis_totals; }},
    {"The most triggerhappy pilots", 0,
     [](int c) -> int {
       return roster[c].solo_mis_shotsf + roster[c].multi_mis_shotsf;
     }},
    {"Bombusers TOP10", 0,
     [](int c) -> int {
       return roster[c].solo_mis_bombs + roster[c].multi_mis_bombs;
     }},
    {"The most accurate shooters", 1,
     [](int c) -> int {
       const int shots = roster[c].solo_mis_shotsf + roster[c].multi_mis_shotsf;
       if (shots < 1000)
         return 0;
       return (roster[c].solo_mis_shotshit + roster[c].multi_mis_shotshit) *
              10000 / shots;
     }},
    {"The most accurate bombers", 1,
     [](int c) -> int {
       const int bombs = roster[c].solo_mis_bombs + roster[c].multi_mis_bombs;
       if (bombs < 100)
         return 0;
       return (roster[c].solo_mis_bombshit + roster[c].multi_mis_bombshit) *
              10000 / bombs;
     }},
    {"Drops TOP10", 0,
     [](int c) -> int {
       return roster[c].solo_mis_drops + roster[c].multi_mis_drops;
     }},
    {"The most active decoys", 0,
     [](int c) -> int {
       return roster[c].solo_mis_dropped + roster[c].multi_mis_dropped;
     }},
    {"The most missions flown", 0,
     [](int c) -> int {
       return roster[c].solo_mis_flown + roster[c].multi_mis_flown;
     }},
};

static void aces_one_solo(const int country, const int mission) {
  char title[80];

  snprintf(title, sizeof(title), "%d. %s mission (%s)", mission + 1,
           country_names[country], mission_names[country * 6 + mission]);

  aces_show(82, title, 0, [=](int c) -> int {
    return roster[c].solo_mis_scores[country][mission];
  });
}

static void show_descriptions(const int number) {
  int c;

  frost->printf((320 - mission_headline_pixels) >> 1, 24, "%s",
                mission_names[number]);

  for (c = 0; c < 16; c++) {
    if (!mission_lines[c][0])
      break;
    frost->printf((320 - mission_pixels[c]) >> 1, 40 + c * 10, "%s",
                  mission_lines[c]);
  }
}

static void load_descriptions(const int number) {
  char *temp_storage;
  int c;
  int locator = 0;
  int index = 0;
  int ch;
  int desclenght;
  int wordcount;

  if (!dksopen("midesc")) {
    printf("Unable to locate data midesc\n");
    exit(1);
  }

  temp_storage = (char *)util::wutil::walloc(dkssize());
  dksread(temp_storage, dkssize());

  for (c = 0; c <= number; c++) {

    while (temp_storage[locator] != '#')
      locator++;

    locator++;
  }

  while (locator < dkssize() && temp_storage[locator] != '#') {
    ch = temp_storage[locator];

    if (ch == 0xa) {
      mission_description[index] = ' ';
      index++;

    } else {
      if (ch != 0xd) {
        mission_description[index] = temp_storage[locator];
        index++;
      }
    }

    locator++;
  }

  mission_description[index] = 0;
  desclenght = index + 1;

  dksclose();

  locator = 0;
  int line_going = 0;
  int chars_to_copy;

  while (locator < desclenght) {
    wordcount = 0;

    for (index = 0; index < CHARS_PER_LINE; index++) {
      if (index + locator >= desclenght) {
        wordcount = index - 1;

      } else {
        if (mission_description[index + locator] == ' ') {
          wordcount = index;
        }
      }
    }

    /* A word longer than a line is cut at the line end; without this the
     * loop would not advance. */
    if (wordcount == 0)
      wordcount = CHARS_PER_LINE;

    chars_to_copy = wordcount;

    for (index = 0; index < chars_to_copy; index++) {
      mission_lines[line_going][index] = mission_description[index + locator];
    }

    mission_lines[line_going][index] = 0;

    locator += chars_to_copy;
    line_going++;
  }

  for (index = line_going; index < 16; index++) {
    mission_lines[index][0] = 0;
    mission_pixels[index] = 0;
  }

  for (index = 0; index < line_going; index++) {
    mission_pixels[index] = frost->printf(0, 0, "%s", mission_lines[index]);
  }

  mission_headline_pixels = frost->printf(0, 0, "%s", mission_names[number]);

  tyhjaa_vircr();

  util::wutil::wfree(temp_storage);
}

int solo_player_menu() {
  static constexpr const char *facenames[4] = {"GERFAC", "FINFAC", "ENGFAC",
                                               "JAPFAC"};
  static constexpr const char *missionnames[4] = {"MISSI0", "MISSI1",
                                                  "MISSI2", "MISSI3"};

  std::unique_ptr<Bitmap> misboa;
  std::unique_ptr<Bitmap> misbak;
  std::unique_ptr<Bitmap> face;
  std::unique_ptr<Bitmap> mission;
  int flag = 0;
  int l;
  int x, y, n1, n2;

  int highest_mission = 0;

  if (findparameter("-solomenumission")) {
    sscanf(parametrit[findparameter("-solomenumission") + 1], "%d",
           &mission_re_fly);
  }

  if (mission_re_fly == MISSION_RE_FLY_BACK_TO_MENU) {
    mission_re_fly = -1;
  }

  for (l = 0; l < 5; l++) {
    if (roster[config.player_number[solo_country]]
            .solo_mis_scores[solo_country][l])
      highest_mission = l + 1;
  }

  if (mission_re_fly != -1) {
    solo_mission = mission_re_fly;
    flag = 3;

  } else {
    misboa = std::make_unique<Bitmap>("MISBOA");
    misbak = std::make_unique<Bitmap>("MISBAK", 0);

    face = std::make_unique<Bitmap>(facenames[solo_country]);
    mission = std::make_unique<Bitmap>(missionnames[solo_country]);

    misbak->blit(0, 0);
    misboa->blit(9, 4);
    face->blit(0, 0);
    mission->blit(22, 19, 0, 0, 319, 46 + highest_mission * 27);

    standard_background = std::make_unique<Bitmap>(0, 0, 320, 200);
    do_all_clear();
  }

  if ((mission_re_fly == -1) && national_music_enabled()) {
    sdl_stop_music();
    national_mod = sdl_load_mod_file(national_modnames[solo_country]);
    if (national_mod == nullptr) {
      printf("Error locating music.\n");
      exit(1);
    }

    sdl_play_music(national_mod);
  }

  while (flag == 0) {
    if (kbhit() && getch() == TK_ESCAPE) {
      flag = 2;
    }

    koords(&x, &y, &n1, &n2);

    cursor->blit(x - 10, y - 10);
    do_all_clear();

    if (n1 || n2) {
      for (l = 0; l <= highest_mission; l++) {
        if (x >= 23 && x <= 125 && y > (20 + 27 * l) && y < (45 + 27 * l)) {
          solo_mission = l;
          if (n1)
            flag = 1;
          else
            flag = 3;
        }
      }

      if (x >= 140) {
        flag = 2;
      }
    }
  }

  if (mission_re_fly == -1) {
    standard_background = nullptr;
  }

  if (flag == 1)
    gfx::fades::random_fade_out();
  else {
    tyhjaa_vircr();
    do_all();
  }

  wait_mouse_relase(1);
  switch (flag) {

  case 1:

    return 1;

  case 3:
    if ((mission_re_fly == -1) && national_music_enabled()) {
      sdl_stop_music();
      sdl_free_mod_file(national_mod);
    }
    return 2;

  case 2:
    if (national_music_enabled()) {
      sdl_stop_music();
      sdl_free_mod_file(national_mod);
    }

    return 0;

  default:

    return 0;
  }
}

void roster_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  static int number = -1;
  int keysetmode = 0;
  int help_on = 0;
  int ch;
  std::unique_ptr<Bitmap> help;
  std::unique_ptr<Bitmap> rosteri;
  std::unique_ptr<Bitmap> buttl;
  std::unique_ptr<Bitmap> buttr;
  std::unique_ptr<Bitmap> setk1;
  std::unique_ptr<Bitmap> setk2;
  std::unique_ptr<Bitmap> medal2[4];
  std::unique_ptr<Bitmap> medal3;
  std::unique_ptr<Bitmap> medal5;
  std::unique_ptr<Bitmap> temp;
  std::unique_ptr<Bitmap> ribbon[6];
  int l, l2, l3, l4;
  int rank;

  if (number == -1 && roster[0].pilotname[0]) {
      number = 0;
  }

  rosteri = std::make_unique<Bitmap>("ROSTER");
  buttl = std::make_unique<Bitmap>("BUTTL"); // 187,174
  buttr = std::make_unique<Bitmap>("BUTTR"); // 205,174
  setk1 = std::make_unique<Bitmap>("SETK1");
  setk2 = std::make_unique<Bitmap>("SETK2");
  help = std::make_unique<Bitmap>("HELP2");
  temp = std::make_unique<Bitmap>("MEDAL2");
  medal3 = std::make_unique<Bitmap>("MEDAL3");

  for (l = 0; l < 4; l++) {
    medal2[l] = std::make_unique<Bitmap>(1 + l * 17, 1, 16, 28, temp);
  }

  medal5 = std::make_unique<Bitmap>(1 + 4 * 17, 1, 16, 28, temp);

  temp = std::make_unique<Bitmap>("RIBBON");

  for (l = 0; l < 6; l++)
    ribbon[l] = std::make_unique<Bitmap>(36 * l, 0, 35, 8, temp);

  hiiri_to(264, 109);

  while (!exit_flag) {
    if (kbhit()) {
      ch = getch();
      if (ch == TK_F1) {
        util::wutil::wtoggle(&help_on);

      } else {
        if (ch == 27)
          exit_flag = 1;
      }
    }

    koords(&x, &y, &n1, &n2);

    if (number != -1) {
      if (!keysetmode) {
        print_clear_roster(rosteri.get());
        print_filled_roster(number);
        setk1->blit(286, 190);

      } else {
        rosteri->blit(0, 0);
        grid2->printf(125, 50, "%s %s", rank_names[get_rank(number)],
                      roster[number].pilotname);
        grid2->printf(152, 40, "Keys Info");
        setk2->blit(286, 190);
        setk1->blit(125, 172);

        frost->printf(125, 162, "Set Joystick");

        if (config.joystick[4] == -1) {
          frost->printf(125, 93, "Up [%s]\nDown [%s]\nRoll [%s]",
                        key_name((int)roster[number].up),
                        key_name((int)roster[number].down),
                        key_name((int)roster[number].roll));
          frost->printf(170, 114, "Power [%s]\nBombs [%s]\nGuns [%s]\n",
                        key_name((int)roster[number].power),
                        key_name((int)roster[number].bombs),
                        key_name((int)roster[number].guns));
        } else {
          char *ups = get_joy_action_string(&joystick_config[4].up);
          char *downs = get_joy_action_string(&joystick_config[4].down);
          char *rolls = get_joy_action_string(&joystick_config[4].roll);
          char *powers = get_joy_action_string(&joystick_config[4].power);
          char *gunss = get_joy_action_string(&joystick_config[4].guns);
          char *bombss = get_joy_action_string(&joystick_config[4].bombs);
          char *brakes = get_joy_action_string(&joystick_config[4].brake);

          frost->printf(125, 93, "J1: Up [%s]\nDown [%s]\nRoll [%s]", ups,
                        downs, rolls);
          frost->printf(170, 114,
                        "Power [%s]\nBombs [%s]\nGuns [%s]\nBreak [%s]", powers,
                        bombss, gunss, brakes);

          util::wutil::wfree(ups);
          util::wutil::wfree(downs);
          util::wutil::wfree(rolls);
          util::wutil::wfree(powers);
          util::wutil::wfree(gunss);
          util::wutil::wfree(bombss);
          util::wutil::wfree(brakes);
        }
      }
    } else {
      rosteri->blit(0, 0);
    }

    if (number != -1)
      for (l = 0; l < 4; l++)
        if (roster[number].solo_mis_scores[l][5])
          medal2[l]->blit(127 + l * 20, 5);

    if (number != -1 &&
        roster[number].solo_mis_drops + roster[number].multi_mis_drops) {
      l4 = 1;
      l3 = roster[number].solo_mis_drops * 2 - roster[number].solo_mis_dropped;
      l3 += roster[number].multi_mis_drops * 3 -
            roster[number].multi_mis_dropped * 2;

      for (l = 0; l < MAX_PLAYERS_IN_ROSTER; l++) {
        if (!roster[l].pilotname[0])
          break;

        if (l == number)
          continue;

        if (!(roster[l].solo_mis_drops + roster[l].multi_mis_drops))
          continue;

        l2 = roster[l].solo_mis_drops * 2 - roster[l].solo_mis_dropped;
        l2 += roster[l].multi_mis_drops * 3 - roster[l].multi_mis_dropped * 2;

        if (l2 >= l3) {
          l4 = 0;
          break;
        }
      }

      if (l4 && l3 > 0) {
        medal3->blit(207, 5);
      }
    }

    if (number != -1 && (roster[number].multi_mis_flown +
                         roster[number].solo_mis_flown) >= 1000)
      medal5->blit(227, 5);

    if (number != -1) {
      rank = get_rank(number);

      for (l = 0; l <= rank; l++) {
        ribbon[l]->blit(283, 80 + l * 9);
      }
    }

    menuselect = 0;

    if (!keysetmode)
      if (x >= 257 && x <= 272 && y >= 53 && y <= 173) {
        frost->printf(15, 24, "Create new\n pilot");
        menuselect = 1;
      }

    if (x >= 36 && x <= 80 && y >= 118 && y <= 177) {
      frost->printf(15, 24, "Back to\n main menu");
      menuselect = 2;
    }

    if (x >= 205 && x <= 221 && y >= 174 && y <= 182) {
      frost->printf(15, 24, "Next pilot");
      menuselect = 3;
    }

    if (x >= 187 && x <= 203 && y >= 174 && y <= 182) {
      frost->printf(15, 24, "Previous pilot");
      menuselect = 4;
    }

    if (!keysetmode)
      if (x >= 263 && x <= 301 && y >= 8 && y <= 55) {
        frost->printf(15, 24, "Erase pilot");
        menuselect = 5;
      }

    if (x >= 286 && x <= 318 && y >= 190 && y <= 198) {
      frost->printf(15, 24, "Toggle setkeys\n mode");
      menuselect = 6;
    }

    if (keysetmode && x >= 125 && x <= 157 && y >= 172 && y <= 180) {
      frost->printf(15, 24, "Set Sologame\n Keys");
      menuselect = 7;
    }

    if (keysetmode && x >= 125 && x <= 172 && y >= 161 && y <= 166) {
      frost->printf(15, 24, "Set Sologame\n Joystick");
      menuselect = 8;
    }

    if (help_on)
      help->blit(0, 0);

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {
      case 1:
        for (number = 0; number < MAX_PLAYERS_IN_ROSTER; number++)
          if (!roster[number].pilotname[0])
            break;

        if (number == MAX_PLAYERS_IN_ROSTER) {
          number = ROSTER_FULL_FALLBACK_PILOT;
          break;
        }

        print_clear_roster(rosteri.get());
        frost->printf(15, 24, "Type name of\nthe pilot\nand press\nenter.");

        roster[number] = {};
        roster[number].up = TK_x;
        roster[number].down = TK_w;
        roster[number].roll = TK_s;
        roster[number].power = TK_TAB;
        roster[number].guns = TK_2;
        roster[number].bombs = TK_1;

        grid2->scanf(125, 50, roster[number].pilotname, 21);
        if (!roster[number].pilotname[0]) {
          number--;
          break;
        }

        break;

      case 2:
        if (n1)
          gfx::fades::random_fade_out();
        else {
          tyhjaa_vircr();
          do_all();
        }

        exit_flag = 1;
        break;

      case 4:
        buttl->blit(187, 174);
        cursor->blit(x - 10, y - 10);
        do_all();
        wait_mouse_relase();
        if (number <= 0)
          break;
        number--;
        break;

      case 3:
        buttr->blit(205, 174);
        cursor->blit(x - 10, y - 10);
        do_all();
        wait_mouse_relase();
        number++;
        if ((number == MAX_PLAYERS_IN_ROSTER) ||
            (roster[number].pilotname[0] == 0))
          number--;
        break;

      case 6:
        setk2->blit(286, 190);
        cursor->blit(x - 10, y - 10);
        do_all();
        wait_mouse_relase();
        util::wutil::wtoggle(&keysetmode);
        break;

      case 7:
        setk2->blit(125, 172);
        cursor->blit(x - 10, y - 10);
        do_all();
        wait_mouse_relase();

        config.joystick[4] = -1;

        for (const auto &prompt : roster_key_prompts) {
          int32_t &keycode = roster[number].*prompt.key;

          rosteri->blit(0, 0);
          frost->printf(125, 100, "%s [%s]", prompt.text,
                        key_name((int)keycode));
          do_all();

          keycode = select_key(number, keycode);
        }
        break;

      case 8:
        if (joystick_exists & JOY1) {
          config.joystick[ROSTER_JOYSTICK] = 1;
          joystick_setup(ROSTER_JOYSTICK, rosteri.get(), 125, 100);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[ROSTER_JOYSTICK] = 1;
        } else {
          rosteri->blit(0, 0);
          frost->printf(125, 100, "Joystick 1 not connected");
          do_all();
          wait_mouse_relase();
        }
        break;

      case 5:
        if (number == -1)
          break;

        if (!small_warning(" You are about to permanently\n"
                           " remove this pilot.\n"
                           "\n"
                           "       Are you sure?"))
          break;

        for (l = 0; l < 4; l++) {
          if (config.player_number[l] == number) {
            config.player_number[l] = -1;
            config.player_type[l] = 0;
          }
        }

        l = number + 1;
        while (roster[l].pilotname[0]) {
          roster[l - 1] = roster[l];
          l++;
        }
        roster[l - 1].pilotname[0] = 0;
        number--;
        if (roster[0].pilotname[0] && number == -1)
          number = 0;

        wait_mouse_relase();
        break;
      }
    }
  }

  wait_mouse_relase();
}

static void check_other_joys(int num, int active) {
  for (int i = 0; i < 4; i++) {
    if (i == num)
      continue;
    if (config.joystick[i] == active)
      config.joystick[i] = -1;
  }
}

/* A yes/no row of the options menu: its label, its row y and the
 * configuration field it sets. The yes box is at x 214-221 and the no box
 * at x 224-231 of the same y. */
struct option_row {
  const char *label;
  int y;
  int32_t configuration::*field;
};

/* Rows that are just a label and a checkbox pair. The rows of the
 * multiplayer page with extra controls (all_planes_are and its plane
 * selector, the stop counter, alliance and its selector) are handled in
 * options_menu. */
static constexpr option_row visual_options[] = {
    {"Shots visible?", 60, &configuration::shots_visible},
    {"AAA shots visible?", 70, &configuration::it_shots_visible},
    {"AA-MG shots visible?", 80, &configuration::aa_mg_shots_visible},
    {"Flags?", 90, &configuration::flags},
    {"Structure flames?", 100, &configuration::flames},
    {nullptr, 110, &configuration::svga}, // label depends on split_num
    {"Structure smoke?", 120, &configuration::structure_smoke},
};

static constexpr option_row audio_options[] = {
    {"Sounds on?", 60, &configuration::sound_on},
    {"Musics on?", 70, &configuration::music_on},
    {"SFX on?", 80, &configuration::sfx_on},
    {"Explosion sounds?", 90, &configuration::explosion_sounds},
    {"Gunfire sounds?", 100, &configuration::gunshot_sounds},
    {"AAA-fire sounds?", 110, &configuration::ground_i_sounds},
    {"Water splash sound?", 120, &configuration::splash},
    {"Infantry sounds?", 130, &configuration::infantry_sounds},
};

static constexpr option_row flying_options[] = {
    {"Do you want on/off power?", 60, &configuration::poweronoff},
    {"Do you want a reversed\n  powerswitch?", 70, &configuration::powerrev},
};

static constexpr option_row multiplayer_options[] = {
    {"Are all planes the same?", 60, &configuration::all_planes_are},
    {"Collisions on?", 70, &configuration::nocollision},
    {"Flying parts hit plane?", 80, &configuration::partcollision},
    {"Battle ends after n points", 90, &configuration::stop},
    {"Alliances enabled?", 100, &configuration::alliance},
    {"AA-MGs?", 110, &configuration::aa_mgs},
    {"AAAs?", 120, &configuration::it_guns},
    {"Infantry?", 130, &configuration::infantry},
    {"Unlimited ammo?", 140, &configuration::unlimited_ammo},
    {"Unlimited gas?", 150, &configuration::unlimited_gas},
};

struct option_page {
  const option_row *rows;
  int row_count;
};

static constexpr option_page option_pages[4] = {
    {visual_options, std::size(visual_options)},
    {audio_options, std::size(audio_options)},
    {flying_options, std::size(flying_options)},
    {multiplayer_options, std::size(multiplayer_options)},
};

/* Rows of the multiplayer page (optimode 3) with extra controls. */
static constexpr int ALL_PLANES_ARE_Y = 60;
static constexpr int STOP_Y = 90;
static constexpr int ALLIANCE_Y = 100;

/* The menuselect ids used while an option row is clicked. */
static constexpr int MENUSELECT_OPTION_ROW = 10; // + row index
static constexpr int MENUSELECT_STOP = 28;
static constexpr int MENUSELECT_PLANE_TYPE = 51;  // + plane type - 1
static constexpr int MENUSELECT_ALLIANCE = 55;    // + alliance - 1

void options_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int menusubselect = 0;
  std::unique_ptr<Bitmap> optionme(new Bitmap("OPTION"));
  std::unique_ptr<Bitmap> kohta[4];
  std::unique_ptr<Bitmap> right(new Bitmap("RIGHT"));
  std::unique_ptr<Bitmap> wrong(new Bitmap("WRONG"));
  std::unique_ptr<Bitmap> opt1(new Bitmap("OPT1"));
  std::unique_ptr<Bitmap> opt2(new Bitmap("OPT2"));
  int optimode = 0;
  int l;

  static constexpr const char *selitykset[4] = {
      "This form has questions about your vision.",
      "This form has questions about your hearing.",
      "Phychological questions about flying.",
      "Questions about hostile environments."};

  kohta[3] = std::make_unique<Bitmap>("OPTI1");
  kohta[2] = std::make_unique<Bitmap>("OPTI2");
  kohta[1] = std::make_unique<Bitmap>("OPTI3");
  kohta[0] = std::make_unique<Bitmap>("OPTI4");

  while (!exit_flag) {
    if (kbhit())
      if (getch() == 27)
        exit_flag = 1;

    koords(&x, &y, &n1, &n2);
    optionme->blit(0, 0);
    kohta[3 - optimode]->blit(248, 13);
    frost->printf(73, 43, "%s", selitykset[optimode]);

    const option_page &page = option_pages[optimode];
    const bool multiplayer_page = (optimode == 3);

    for (l = 0; l < page.row_count; l++) {
      const option_row &row = page.rows[l];

      if (row.label)
        frost->printf(73, row.y, "%s", row.label);
      else
        frost->printf(73, row.y, "Use %s window?",
                      split_num == 2
                          ? "800x600"
                          : (split_num == 1 ? "1200x400" : "2400:200"));
    }

    for (l = 0; l < page.row_count; l++) {
      const option_row &row = page.rows[l];

      if (multiplayer_page && row.y == STOP_Y)
        continue;
      boxi(214, row.y, 221, row.y + 8, 0);
      boxi(224, row.y, 231, row.y + 8, 0);
    }

    for (l = 0; l < page.row_count; l++) {
      const option_row &row = page.rows[l];
      const int32_t value = config.*row.field;

      if (multiplayer_page && row.y == STOP_Y) {
        if (value)
          frost->printf(216, 91, "%d", value);
        else
          frost->printf(216, 91, "UnLtd");
        continue;
      }

      if (value) {
        right->blit(215, row.y);

        if (multiplayer_page && row.y == ALL_PLANES_ARE_Y) {
          opt1->blit(1, 1);
          boxi(34, 10, 41, 18, 0);
          boxi(34, 31, 41, 39, 0);
          boxi(34, 52, 41, 60, 0);
          boxi(34, 73, 41, 81, 0);

          right->blit(35, value * 21 - 11);
        }

        if (multiplayer_page && row.y == ALLIANCE_Y) {
          opt2->blit(1, 92);
          boxi(5, 92 + value * 32 - 28, 53, 92 + value * 32 + 4, 38);
        }
      } else
        wrong->blit(225, row.y);
    }

    menuselect = 0;

    if (x >= 267 && x <= 301 && y >= 155 && y <= 190) {
      frost->printf(102, 26, "Back to main menu");
      menuselect = 2;
    }

    if (x >= 248 && x <= 260 && y >= 117 && y <= 145) {
      frost->printf(102, 26, "Change to visual settings");
      menuselect = 3;
    }

    if (x >= 248 && x <= 260 && y >= 80 && y <= 116) {
      frost->printf(102, 26, "Change to audio settings");
      menuselect = 4;
    }

    if (x >= 248 && x <= 260 && y >= 47 && y <= 79) {
      frost->printf(102, 26, "Change to general flying settings");
      menuselect = 5;
    }

    if (x >= 248 && x <= 260 && y >= 13 && y <= 46) {
      frost->printf(102, 26, "Change to multiplayer settings");
      menuselect = 6;
    }

    /* The stop counter is also hit between its (undrawn) boxes. */
    if (multiplayer_page && x >= 214 && x <= 231 && y >= STOP_Y &&
        y <= STOP_Y + 8)
      menuselect = MENUSELECT_STOP;

    if ((x >= 214 && x <= 221) || (x >= 224 && x <= 231)) {
      if (x <= 221)
        menusubselect = 1;
      else
        menusubselect = 0;

      for (l = 0; l < page.row_count; l++) {
        const option_row &row = page.rows[l];

        if (y >= row.y && y <= row.y + 8) {
          if (multiplayer_page && row.y == STOP_Y)
            menuselect = MENUSELECT_STOP;
          else
            menuselect = MENUSELECT_OPTION_ROW + l;
        }
      }
    }

    if (multiplayer_page) {
      if (config.all_planes_are && x >= 34 && x <= 41) {
        for (l = 0; l < 4; l++)
          if (y >= 10 + l * 21 && y <= 18 + l * 21)
            menuselect = MENUSELECT_PLANE_TYPE + l;
      }

      if (config.alliance && x >= 5 && x <= 53) {
        if (y >= 96 && y <= 128)
          menuselect = MENUSELECT_ALLIANCE;
        if (y >= 129 && y <= 160)
          menuselect = MENUSELECT_ALLIANCE + 1;
        if (y >= 161 && y <= 182)
          menuselect = MENUSELECT_ALLIANCE + 2;
      }
    }

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      if (menuselect == 2) {
        if (n1)
          gfx::fades::random_fade_out();
        else {
          tyhjaa_vircr();
          do_all();
        }
        exit_flag = 1;
      }

      if (menuselect >= 3 && menuselect <= 6)
        optimode = menuselect - 3;

      if (menuselect >= MENUSELECT_OPTION_ROW &&
          menuselect < MENUSELECT_OPTION_ROW + page.row_count)
        config.*page.rows[menuselect - MENUSELECT_OPTION_ROW].field =
            menusubselect;

      if (menuselect == MENUSELECT_STOP) {
        if (n1) {
          if (++config.stop > 999)
            config.stop = 999;

        } else {
          if (--config.stop < 0)
            config.stop = 0;
        }
      }

      if (menuselect >= MENUSELECT_PLANE_TYPE &&
          menuselect <= MENUSELECT_PLANE_TYPE + 3) {
        config.all_planes_are = menuselect - MENUSELECT_PLANE_TYPE + 1;
      }

      if (menuselect >= MENUSELECT_ALLIANCE &&
          menuselect <= MENUSELECT_ALLIANCE + 2) {
        config.alliance = menuselect - MENUSELECT_ALLIANCE + 1;
      }
    }
  }

  if (config.sound_on && (is_there_sound == 0)) { init_sounds(); }

  if (is_there_sound && config.music_on && (!music_loaded)) {

    load_music();
    sdl_play_music(triplane_mod);
  }

  if (music_loaded && (!is_there_sound || !config.music_on)) {
    sdl_stop_music();
      clear_music();
  }

  if (is_there_sound && config.sfx_on && (!sfx_loaded)) { load_sfx(); }

  if (sfx_loaded && (!is_there_sound || !config.sfx_on)) { clear_sfx(); }

  if (!config.sound_on && (is_there_sound == 1))
    uninit_sounds();

  wait_mouse_relase();
}

static void transfer_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  std::unique_ptr<Bitmap> color_bites[6];
  std::unique_ptr<Bitmap> descs[6];
  int l;

  std::unique_ptr<Bitmap> optionme(new Bitmap("TRANS2"));

  color_bites[0] = std::make_unique<Bitmap>(39, 46, 80, 50, optionme);
  color_bites[1] = std::make_unique<Bitmap>(119, 46, 80, 50, optionme);
  color_bites[2] = std::make_unique<Bitmap>(199, 46, 80, 50, optionme);
  color_bites[3] = std::make_unique<Bitmap>(39, 96, 80, 50, optionme);
  color_bites[4] = std::make_unique<Bitmap>(119, 96, 80, 50, optionme);
  color_bites[5] = std::make_unique<Bitmap>(199, 96, 80, 50, optionme);

  descs[0] = std::make_unique<Bitmap>("DESC1");
  descs[1] = std::make_unique<Bitmap>("DESC2");
  descs[2] = std::make_unique<Bitmap>("DESC3");
  descs[3] = std::make_unique<Bitmap>("DESC4");
  descs[4] = std::make_unique<Bitmap>("DESC5");
  descs[5] = std::make_unique<Bitmap>("DESC6");

  optionme = std::make_unique<Bitmap>("TRANSF");

  while (!exit_flag) {
    if (kbhit())
      if (getch() == 27)
        exit_flag = 1;

    koords(&x, &y, &n1, &n2);
    optionme->blit(0, 0);

    color_bites[config.current_multilevel]->blit(
        39 + config.current_multilevel * 80 -
            (config.current_multilevel / 3) * 240,
        46 + (config.current_multilevel / 3) * 50);

    descs[config.current_multilevel]->blit(0, 154);

    menuselect = 0;

    if ((x >= 0 && x <= 319 && y >= 0 && y <= 41) ||
        (x >= 0 && x <= 319 && y >= 151 && y <= 199) ||
        (x >= 0 && x <= 32 && y >= 0 && y <= 199) ||
        (x >= 282 && x <= 319 && y >= 0 && y <= 199)) {
      menuselect = 2;
    }

    for (l = 0; l < 6; l++)
      if (x >= (39 + l * 80 - (l / 3) * 240) &&
          x <= (118 + l * 80 - (l / 3) * 240) && y >= (46 + (l / 3) * 50) &&
          y <= (95 + (l / 3) * 50)) {
        menuselect = 3 + l;
      }

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 2:
        if (n1)
          gfx::fades::random_fade_out();
        else {
          tyhjaa_vircr();
          do_all();
        }
        exit_flag = 1;
        break;
      }

      if (menuselect >= 3 && menuselect <= 8) {
        config.current_multilevel = menuselect - 3;
      }
    }
  }

  wait_mouse_relase();
}

/* Calibrates joystick_config[joy] interactively. The ROSTER_JOYSTICK is
 * always the first physical joystick. The prompts are drawn over
 * background at (text_x, text_y) and 7 pixels below. */
static void joystick_setup(const int joy, Bitmap *background, const int text_x,
                           const int text_y) {
  Sint16 *idle, *current;
  int i, c;
  struct {
    const char *prompt;
    joystick_action *act;
  } acts[] = {{"Up", &joystick_config[joy].up},
              {"Down", &joystick_config[joy].down},
              {"Roll", &joystick_config[joy].roll},
              {"Power", &joystick_config[joy].power},
              {"Guns", &joystick_config[joy].guns},
              {"Bombs", &joystick_config[joy].bombs},
              {"Brake", &joystick_config[joy].brake},
              {nullptr, nullptr}};

  if (joy == ROSTER_JOYSTICK)
    open_close_joysticks(1, 0, 0, 0);
  else
    open_close_joysticks(joy == 0, joy == 1, joy == 2, joy == 3);

  idle = allocate_axis_state(joy);
  current = allocate_axis_state(joy);

  background->blit(0, 0);
  frost->printf(text_x, text_y, "Keep joystick idle and press");
  frost->printf(text_x, text_y + 7, "Space (Esc=use old settings)");
  do_all();
  do {
    c = getch();
  } while (c != 27 && c != ' ');
  if (c == 27)
    goto joystick_setup_exit;

  save_axis_state(idle, joy);

  for (i = 0; acts[i].prompt != nullptr; i++) {
    background->blit(0, 0);
    frost->printf(text_x, text_y, "Do '%s' on joystick and", acts[i].prompt);
    frost->printf(text_x, text_y + 7, "press Space or D=disable this");
    do_all();
    do {
      c = getch();
    } while (c != 27 && c != ' ' && c != 'd' && c != 'D');
    if (c == 27) {
      goto joystick_setup_exit;
    } else if (c == 'd' || c == 'D') {
      set_disabled_action(acts[i].act);
    } else {
      if (!find_changed_button(acts[i].act, joy)) {
        save_axis_state(current, joy);
        find_changed_axis(acts[i].act, idle, current, joy);
      }
    }
  }

joystick_setup_exit:
  util::wutil::wfree(idle);
  util::wutil::wfree(current);

  open_close_joysticks(0, 0, 0, 0);
}

void controls_menu() {
  int ch;
  int help_on = 0;
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  std::unique_ptr<Bitmap> controlme(new Bitmap("NAPPIS"));
  std::array<std::unique_ptr<Bitmap>, 4> napp;
  std::unique_ptr<Bitmap> help(new Bitmap("HELP4"));
  int active = 0;

  napp[0] = std::make_unique<Bitmap>("NAPPRE");
  napp[1] = std::make_unique<Bitmap>("NAPPBL");
  napp[2] = std::make_unique<Bitmap>("NAPPGR");
  napp[3] = std::make_unique<Bitmap>("NAPPYL");

  while (!exit_flag) {
    if (kbhit()) {
      ch = getch();
      if (ch == TK_F1) {
        util::wutil::wtoggle(&help_on);

      } else {
        if (ch == 27)
          exit_flag = 1;
      }
    }

    koords(&x, &y, &n1, &n2);
    controlme->blit(0, 0);

    napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);

    if (help_on)
      help->blit(0, 0);

    menuselect = 0;

    if ((x >= 0 && x <= 106 && y >= 73 && y <= 107) ||
        (x >= 68 && x <= 106 && y >= 0 && y <= 73) ||
        (x >= 108 && x <= 319 && y >= 88 && y <= 109) ||
        (x >= 301 && x <= 319 && y >= 111 && y <= 199) ||
        (x >= 0 && x <= 7 && y >= 114 && y <= 199)) {
      frost->printf(61, 97, "Back to main menu");
      menuselect = 2;
    }

    if ((x >= 11 && x <= 296 && y >= 112 && y <= 195)) {
      frost->printf(61, 97, "Set keys");
      menuselect = 3;
    }

    if ((x >= 10 && x <= 24 && y >= 22 && y <= 35)) {
      frost->printf(61, 97, "Select Red Plane");
      menuselect = 4;
    }

    if ((x >= 37 && x <= 51 && y >= 22 && y <= 35)) {
      frost->printf(61, 97, "Select Blue Plane");
      menuselect = 5;
    }

    if ((x >= 10 && x <= 24 && y >= 45 && y <= 58)) {
      frost->printf(61, 97, "Select Green Plane");
      menuselect = 6;
    }

    if ((x >= 37 && x <= 51 && y >= 45 && y <= 58)) {
      frost->printf(61, 97, "Select Yellow Plane");
      menuselect = 7;
    }

    if ((x >= 108 && x <= 159 && y >= 0 && y <= 42)) {
      frost->printf(61, 97,
                    joystick_exists & JOY1 ? "Select Joystick 1"
                                           : "Joystick 1 not connected");
      menuselect = 8;
    }

    if ((x >= 161 && x <= 212 && y >= 0 && y <= 42)) {
      frost->printf(61, 97,
                    joystick_exists & JOY2 ? "Select Joystick 2"
                                           : "Joystick 2 not connected");
      menuselect = 9;
    }

    if ((x >= 108 && x <= 159 && y >= 44 && y <= 87)) {
      frost->printf(61, 97,
                    joystick_exists & JOY3 ? "Select Joystick 3"
                                           : "Joystick 3 not connected");
      menuselect = 10;
    }

    if ((x >= 161 && x <= 212 && y >= 44 && y <= 87)) {
      frost->printf(61, 97,
                    joystick_exists & JOY4 ? "Select Joystick 4"
                                           : "Joystick 4 not connected");
      menuselect = 11;
    }

    if (config.joystick[0] != active && config.joystick[1] != active &&
        config.joystick[2] != active && config.joystick[3] != active) {
      frost->printf(170, 93, "Up [%s] Down [%s] Roll [%s]",
                    key_name((int)player_keys[active].up),
                    key_name((int)player_keys[active].down),
                    key_name((int)player_keys[active].roll));
      frost->printf(170, 100, "Power [%s] Bombs [%s] Guns [%s]",
                    key_name((int)player_keys[active].power),
                    key_name((int)player_keys[active].bombs),
                    key_name((int)player_keys[active].guns));
    } else {
      int joy = 0;
      if (config.joystick[0] == active)
        joy = 0;
      else if (config.joystick[1] == active)
        joy = 1;
      else if (config.joystick[2] == active)
        joy = 2;
      else if (config.joystick[3] == active)
        joy = 3;
      char *ups = get_joy_action_string(&joystick_config[joy].up);
      char *downs = get_joy_action_string(&joystick_config[joy].down);
      char *rolls = get_joy_action_string(&joystick_config[joy].roll);
      char *powers = get_joy_action_string(&joystick_config[joy].power);
      char *gunss = get_joy_action_string(&joystick_config[joy].guns);
      char *bombss = get_joy_action_string(&joystick_config[joy].bombs);
      char *brakes = get_joy_action_string(&joystick_config[joy].brake);

      frost->printf(170, 93, "J%d: Up [%s] Down [%s] Roll [%s]", joy + 1, ups,
                    downs, rolls);
      frost->printf(170, 100, "Pwr [%s] Bmb [%s] Gun [%s] Brk [%s]", powers,
                    bombss, gunss, brakes);

      util::wutil::wfree(ups);
      util::wutil::wfree(downs);
      util::wutil::wfree(rolls);
      util::wutil::wfree(powers);
      util::wutil::wfree(gunss);
      util::wutil::wfree(bombss);
      util::wutil::wfree(brakes);
    }

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 2:
        if (n1)
          gfx::fades::random_fade_out();
        exit_flag = 1;
        break;

      case 3:
        for (int i = 0; i < 4; i++) {
          if (config.joystick[i] == active)
            config.joystick[i] = -1;
        }

        for (const auto &prompt : player_key_prompts) {
          int32_t &keycode = player_keys[active].*prompt.key;

          controlme->blit(0, 0);
          napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
          frost->printf(56, 97, "%s [%s]", prompt.text,
                        key_name((int)keycode));
          do_all();

          keycode = select_key(active, keycode);
        }

        save_keyset();
        break;

      case 8:
      case 9:
      case 10:
      case 11: {
        const int joy = menuselect - 8;
        static constexpr int joystick_bits[4] = {JOY1, JOY2, JOY3, JOY4};

        if (joystick_exists & joystick_bits[joy]) {
          config.joystick[joy] = active;
          check_other_joys(joy, active);
          joystick_setup(joy, controlme.get(), 54, 93);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[joy] = 1;
        }
        break;
      }
      }

      if (menuselect >= 4 && menuselect <= 7) {
        active = menuselect - 4;
      }
    }
  }

  wait_mouse_relase();
}

static void assign_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int menusubselect1 = 0, menusubselect2 = 0;
  std::unique_ptr<Bitmap> acesme(new Bitmap("ASSIGN"));
  std::unique_ptr<Bitmap> ruksi(new Bitmap("RUKSI"));
  std::unique_ptr<Bitmap> help(new Bitmap("HELP5"));
  int l, lx, ly;
  int l2, l3;
  int lym[4] = {0, 11, 24, 36};
  int response;
  int help_on = 0;
  int ch;

  if (!roster[0].pilotname[0]) {
    response = small_warning("You have no pilots in the roster and\n"
                             "therefore you can't assign players\n"
                             "You will be automaticly transfered to\n"
                             "roster menu to create a pilot.\n");

    if (response) {
      roster_menu();

    } else
      return;
  }

  while (!exit_flag) {
    if (kbhit()) {
      ch = getch();
      if (ch == TK_F1) {
        util::wutil::wtoggle(&help_on);

      } else {
        if (ch == 27)
          exit_flag = 1;
      }
    }

    koords(&x, &y, &n1, &n2);
    acesme->blit(0, 0);

    menuselect = 0;

    if ((x >= 0 && x <= 8 && y >= 0 && y <= 199) ||
        (x >= 308 && x <= 319 && y >= 0 && y <= 199) ||
        (x >= 0 && x <= 319 && y >= 0 && y <= 28) ||
        (x >= 0 && x <= 319 && y >= 168 && y <= 199)) {
      menuselect = 2;
    }

    for (l = 0; l < 4; l++) {
      ly = (l / 2) * 73;
      lx = (l % 2) ? 153 : 0;

      ruksi->blit(28 + lx, 45 + ly + lym[config.player_type[l]]);

      if (config.player_number[l] != -1) {
        frost->printf(46 + lx, 82 + ly,
                      roster[config.player_number[l]].pilotname);
      }

      // for multiplayer - plane count
      if (config.player_type[l] > 1)
        frost->printf(46 + lx, 70 + ly, "%d plane%s", config.plane_count[l],
                      config.plane_count[l] > 1 ? "s" : "");

      for (l2 = 0; l2 < 4; l2++) {
        if (x >= (28 + lx) && x <= (35 + lx) && y >= (45 + lym[l2] + ly) &&
            y <= (49 + lym[l2] + ly)) {
          menusubselect1 = l;
          menusubselect2 = l2;
          menuselect = 1;
        }
      }

      // next pilot
      if (x >= (134 + lx) && x < (143 + lx) && y >= (79 + ly) &&
          y <= (89 + ly)) {
        menusubselect1 = l;
        menuselect = 3;
      }

      // previous pilot
      if (x >= (143 + lx) && x <= (151 + lx) && y >= (79 + ly) &&
          y <= (89 + ly)) {
        menusubselect1 = l;
        menuselect = 4;
      }

      // add plane
      if (x >= (134 + lx) && x < (143 + lx) && y >= (67 + ly) &&
          y <= (77 + ly)) {
        menusubselect1 = l;
        menuselect = 5;
      }

      // remove plane
      if (x >= (143 + lx) && x <= (151 + lx) && y >= (67 + ly) &&
          y <= (77 + ly)) {
        menusubselect1 = l;
        menuselect = 6;
      }
    }

    cursor->blit(x - 10, y - 10);
    if (help_on)
      help->blit(0, 0);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 1:
        config.player_type[menusubselect1] = menusubselect2;

        if (menusubselect2 == 1) {
          for (l = 0; l < 4; l++) {
            if (l == menusubselect1)
              continue;

            config.player_type[l] = 0;
            config.player_number[l] = -1;
          }
        } else {
          for (l = 0; l < 4; l++) {
            if (l == menusubselect1)
              continue;

            if (config.player_type[l] == 1) {
              config.player_type[l] = 0;
              config.player_number[l] = -1;
            }
          }
        }

        if (menusubselect2 == 0 || menusubselect2 == 2)
          config.player_number[menusubselect1] = -1;
        else {
          if (config.player_number[menusubselect1] == -1) {

            for (l = 0; l < MAX_PLAYERS_IN_ROSTER; l++) {
              if (!roster[l].pilotname[0]) {
                config.player_type[menusubselect1] = 0;
                config.player_number[menusubselect1] = -1;
                break;
              } else {
                for (l3 = 0; l3 < 4; l3++) {
                  if (l3 == menusubselect1 || config.player_number[l3] == -1)
                    continue;

                  if (config.player_number[l3] == l)
                    break;
                }
                if (l3 == 4) {
                  config.player_number[menusubselect1] = l;
                  break;
                }
              }
            }
            if (l == MAX_PLAYERS_IN_ROSTER) {
              config.player_type[menusubselect1] = 0;
              config.player_number[menusubselect1] = -1;
            }
          }
        }

        break;

      case 2:
        if (n1)
          gfx::fades::random_fade_out();
        exit_flag = 1;
        break;

      case 3:
        ly = 0;
        lx = config.player_number[menusubselect1];
        l2 = lx + 1;

        while (!ly) {
          ly = 1;

          if (lx == l2) {
            l2 = -1;
            break;
          }

          if (l2 != -1) {
            if (!roster[l2].pilotname[0]) {
              ly = 0;
              l2 = -1;
              continue;
            }

            for (l = 0; l < 4; l++) {
              if (l == menusubselect1)
                continue;

              if (config.player_number[l] == l2) {
                l2++;
                ly = 0;
                continue;
              }
            }

          } else {

            l2 = -1;
            break;
          }
        }

        config.player_number[menusubselect1] = l2;

        break;

      case 4:
        ly = 0;
        lx = config.player_number[menusubselect1];
        l2 = lx - 1;
        if (l2 < -1)
          l2 = -1;

        while (!ly) {
          ly = 1;

          if (l2 != -1) {

            for (l = 0; l < 4; l++) {
              if (l == menusubselect1)
                continue;

              if (config.player_number[l] == l2) {
                l2--;
                ly = 0;
                break;
              }
            }

          } else {

            l2 = -1;
            break;
          }
        }

        if (l2 == -1) {
          for (l2 = MAX_PLAYERS_IN_ROSTER - 1; l2 >= 0; l2--) {
            if (roster[l2].pilotname[0]) {
              for (l = 0; l < 4; l++) {
                if (l == menusubselect1)
                  continue;

                if (config.player_number[l] == l2) {
                  break;
                }
              }

              if (l == 4)
                break;
            }
          }

          config.player_number[menusubselect1] = l2;
          break;
        }

        config.player_number[menusubselect1] = l2;

        break;

      case 5:
        if (config.plane_count[menusubselect1] < 3)
          config.plane_count[menusubselect1]++;

        break;

      case 6:
        if (config.plane_count[menusubselect1] > 1)
          config.plane_count[menusubselect1]--;

        break;
      }
    }

    if (n1 || n2)
      wait_mouse_relase();
  }

  for (l = 0; l < 4; l++) {
    if (config.player_number[l] == -1 &&
        (config.player_type[l] == 1 || config.player_type[l] == 3)) {
      config.player_type[l] = 0;
      config.player_number[l] = -1;
    }
  }

  wait_mouse_relase();
}

void aces_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int current_page = 0;
  int help_on = 0;
  int ch;
  std::unique_ptr<Bitmap> acesme(new Bitmap("ACESA"));
  std::unique_ptr<Bitmap> firstpage(new Bitmap("ACESB"));
  std::unique_ptr<Bitmap> buttl(new Bitmap("BUTTL"));
  std::unique_ptr<Bitmap> buttr(new Bitmap("BUTTR"));
  std::unique_ptr<Bitmap> help(new Bitmap("HELP3"));

  if (is_there_sound && config.music_on) {
    sdl_stop_music();
    sdl_play_music(aces_mod);
  }

  acesme->blit(0, 0);
  standard_background = std::make_unique<Bitmap>(0, 0, 320, 200);
  do_all_clear();

  while (!exit_flag) {

    if (kbhit()) {
      ch = getch();
      if (ch == TK_F1) {
        util::wutil::wtoggle(&help_on);

      } else {
        if (ch == 27)
          exit_flag = 1;
      }
    }

    koords(&x, &y, &n1, &n2);

    if (!current_page)
      firstpage->blit(75, 34);

    menuselect = 0;

    if ((x >= 0 && x <= 66 && y >= 0 && y <= 199) ||
        (x >= 269 && x <= 319 && y >= 0 && y <= 199) ||
        (x >= 0 && x <= 319 && y >= 0 && y <= 11) ||
        (x >= 0 && x <= 319 && y >= 189 && y <= 199)) {
      // main menu
      menuselect = 2;
    }

    if (x >= 231 && x <= 247 && y >= 177 && y <= 185) {
      // prev. page
      menuselect = 3;
    }

    if (x >= 249 && x <= 265 && y >= 177 && y <= 185) {
      // next page
      menuselect = 4;
    }

    if (help_on)
      help->blit(0, 0);

    if (current_page > 2 && current_page <= 26) {
      aces_one_solo((current_page - 3) / 6, (current_page - 3) % 6);
    } else {
      const aces_page &page =
          aces_pages[current_page < 3 ? current_page : current_page - 24];
      aces_show(90, page.title, page.percent, page.score);
    }

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 2:

        exit_flag = 1;
        break;
      case 3:
        buttl->blit(231, 177);
        if ((--current_page) < 0)
          current_page = 33;
        break;
      case 4:
        buttr->blit(249, 177);
        if ((++current_page) > 33)
          current_page = 0;
        break;
      }
    }

    cursor->blit(x - 10, y - 10);
    do_all_clear();
    if (n1 || n2)
      wait_mouse_relase();
  }

  if (is_there_sound && config.music_on) {
    sdl_stop_music();
    sdl_play_music(triplane_mod);
  }

  standard_background = nullptr;
  if (n1)
    gfx::fades::random_fade_out();

  wait_mouse_relase();
}

int kangas_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int place_x = -2079;
  int showing_texts = 1;

  std::unique_ptr<Bitmap> kangas(new Bitmap("KANGAS", 0));
  std::unique_ptr<Bitmap> buttr(new Bitmap("BUTTR"));
  std::unique_ptr<Bitmap> buttl(new Bitmap("BUTTL"));
  std::unique_ptr<Bitmap> fly(new Bitmap("FLY"));
  std::unique_ptr<Bitmap> exit_button(new Bitmap("EXIT"));

  init_vga("PALET3");

  if (is_there_sound && config.sfx_on) {
    sample_hurr->left_volume = SAMPLE_VOLUME - 5;
    sample_hurr->right_volume = SAMPLE_VOLUME - 5;
    sdl_stop_music();
    sdl_play_sample(sample_hurr, 1);
  }

  hiiri_to(80, 195);

  if (miss_pl_x[solo_country]) {
    place_x = -(miss_pl_x[solo_country] - 160);

  } else {
    place_x = -(leveldata.airfield_x[solo_country] - 160);
  }

  load_descriptions(solo_mission + solo_country * 6);

  while (!exit_flag) {
    if (kbhit() && getch() == TK_ESCAPE) {
      exit_flag = 1;
    }

    koords(&x, &y, &n1, &n2);
    kangas->blit_fullscreen();
    world::terrain::kangas_terrain_to_screen(place_x);

    if (showing_texts)
      show_descriptions(solo_mission + solo_country * 6);

    menuselect = 0;
    if (x >= 72 && x <= 88 && y >= 191 && y <= 199) {
      // fly mission
      menuselect = 1;
    }

    if (x >= 54 && x <= 70 && y >= 191 && y <= 199) {
      // back to mission menu
      menuselect = 2;
    }

    if (x >= 0 && x <= 16 && y >= 191 && y <= 199) {
      // scroll map leftwards
      menuselect = 3;
    }

    if (x >= 18 && x <= 34 && y >= 191 && y <= 199) {
      // scroll map rightwards
      menuselect = 4;
    }

    if (x >= 5 && x <= 315 && y >= 8 && y <= 178) {
      // texts on/off
      menuselect = 5;
    }

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 1: // fly mission
        fly->blit(72, 191);
        do_all();
        exit_flag = 2;
        break;

      case 2:

        exit_button->blit(54, 191);
        do_all();
        if (n1)
          gfx::fades::random_fade_out();
        exit_flag = 1;
        break;

      case 3:

        place_x += 12;
        buttl->blit(0, 191);
        do_all();
        break;

      case 4:

        place_x -= 12;
        buttr->blit(18, 191);
        do_all();
        break;

      case 5:
        wait_mouse_relase();
        util::wutil::wtoggle(&showing_texts);
      }
    }

    if (place_x > 0)
      place_x = 0;
    if (place_x < -2079)
      place_x = -2079;
  }

  if (is_there_sound && config.sfx_on) {
    sdl_stop_all_samples(); /* stop hurr sound */
  }

  if ((mission_re_fly == -1) && national_music_enabled()) {
    sdl_stop_music();
    sdl_free_mod_file(national_mod);
  }

  wait_mouse_relase();

  init_vga("PALET5");

  if (exit_flag == 2)
    return 1;
  else
    return 0;
}

void credits_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int kohta1, kohta2;

  std::unique_ptr<gfx::font::Font> grid3(new gfx::font::Font("G3FONT"));
  std::unique_ptr<Bitmap> credi1(new Bitmap("CREDI1"));

  grid3->scale();

  kohta1 = 160 - (grid3->printf(0, 25, "Triplane Turmoil Crew") >> 1);
  kohta2 = 160 - (grid3->printf(0, 25, "Special Thanks:") >> 1);

  if (!findparameter("-debugnographics"))
    init_vga("PALET7");

  while (!exit_flag) {
    if (kbhit()) {
      getch();
      exit_flag = 1;
    }

    koords(&x, &y, &n1, &n2);
    credi1->blit(0, 0);

    grid3->printf(kohta1, 25, "Triplane Turmoil Crew");

    grid3->printf(45, 50, "Original idea:\nMarkku Rankala");
    grid3->printf(
        45, 80,
        "Graphics:\nHenrikki Merikallio\nMarkku Rankala\nRisto Puhakka");
    grid3->printf(205, 50, "Music & Sound:\nMarkku Rankala");
    grid3->printf(205, 80, "Sologame levels:\nMarkku Rankala");
    grid3->printf(205, 110, "Code & Project management:\nTeemu J. Takanen");
    grid3->printf(kohta2, 140, "Special Thanks:");
    grid3->printf(45, 160, "Mikko Kinnunen\nNiko Salminen\nPekka Pulli");
    grid3->printf(205, 160, "Antti Lehtoranta\nJuha Rytkonen\nJoonas Joensuu");

    do_all();

    if (n1 || n2) {

      if (n1)
        gfx::fades::random_fade_out();
      exit_flag = 1;
      break;
    }
  }

  tyhjaa_vircr();
  do_all();
  if (!findparameter("-debugnographics"))
    init_vga("PALET5");

  wait_mouse_relase();
}

void letter_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;

  std::unique_ptr<Bitmap> temp(new Bitmap("MEDAL1"));
  std::unique_ptr<Bitmap> medal1(new Bitmap(1 + 32 * solo_country, 1, 31, 57, temp));

  if (national_music_enabled()) {
    national_mod = sdl_load_mod_file(national_modnames[solo_country]);
    if (national_mod == nullptr) {
      printf("Error locating music.\n");
      exit(1);
    }
    sdl_play_music(national_mod);
  }

  std::unique_ptr<Bitmap> letter(new Bitmap("LETTER"));

  while (!exit_flag) {
    if (kbhit()) {
      getch();
      exit_flag = 1;
    }

    koords(&x, &y, &n1, &n2);
    letter->blit(0, 0);
    medal1->blit(15, 80);
    frost->printf(145, 104, "From: %s central command headquaters.",
                  country_names[solo_country]);
    frost->printf(145, 114, "To: %s %s",
                  rank_names[get_rank(config.player_number[solo_country])],
                  roster[config.player_number[solo_country]].pilotname);
    frost->printf(180, 134, "Congratulations");
    frost->printf(119, 160,
                  "You have done outstanding work defending our country\n"
                  "and therefore the central command wants formally\n"
                  "express its feelings by giving you the Best Pilot in\n"
                  "the Country medal. We wish you luck for your future\n"
                  "career.\n");
    do_all();

    if (n1 || n2) {

      if (n1)
        gfx::fades::random_fade_out();
      exit_flag = 1;
      break;
    }
  }

  tyhjaa_vircr();
  do_all();

  wait_mouse_relase();

  if (national_music_enabled()) {
    sdl_stop_music();
    sdl_free_mod_file(national_mod);
  }
}

void main_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int l, l2, l3;
  int help_on = 0;
  int ch;
  std::unique_ptr<Bitmap> help(new Bitmap("HELP1"));

  if (is_there_sound && config.music_on) {

    sdl_play_music(triplane_mod);
  }

  hiiri_to(254, 120);

  while (!exit_flag) {
    if (kbhit()) {
      ch = getch();
      if (ch == TK_F1) {
        util::wutil::wtoggle(&help_on);
      } else {
        if (ch == TK_ESCAPE)
          exit_flag = 1;
      }
    }

    koords(&x, &y, &n1, &n2);
    menu1->blit(0, 0); // 0,0,screen_width_less,screen_height_less
    grid2->printf(34, 156, "Press F1\nfor Help");

    for (l = 0; l < 4; l++)
      if (config.player_type[l] == 1)
        break;

    if (l == 4) {
      for (l = 0; l < 4; l++)
        if (config.player_type[l] == 3)
          break;

      if (l == 4) {
        frost->printf(100, 100, "No assigned pilots");

      } else {
        frost->printf(100, 85, "Multiplayergame active");
        frost->printf(100, 95, "%d. theater", config.current_multilevel + 1);

        if (config.stop) {
          frost->printf(100, 105, "Game ends with %d pts.", config.stop);

        } else {
          frost->printf(100, 105, "Unlimited game");
        }

        if (config.alliance) {
          switch (config.alliance) {
          case 1:
            frost->printf(100, 115, "Alliance: 1&2 vs. 3&4");
            break;

          case 2:
            frost->printf(100, 115, "Alliance: 1&3 vs. 2&4");
            break;

          case 3:
            frost->printf(100, 115, "Alliance: 1&4 vs. 2&3");
            break;
          }
        }

        for (l = 0; l < 4; l++) {
          if (config.player_type[l] == 3) {
            frost->printf(110, 130 + l * 11, "%d. %s", l + 1,
                          roster[config.player_number[l]].pilotname);
          }

          if (config.player_type[l] == 2) {
            frost->printf(110, 130 + l * 11, "%d. Computer pilot", l + 1);
          }

          if (config.player_type[l] == 0) {
            frost->printf(110, 130 + l * 11, "%d. Not active", l + 1);
          }
        }
      }

    } else {
      frost->printf(100, 100, "Sologame active");
      frost->printf(100, 110, "%s selected", plane_name[l]);

      frost->printf(110, 130, "%s flying",
                    roster[config.player_number[l]].pilotname);
    }

    if (help_on)
      help->blit(0, 0);

    menuselect = 0;
    if (x >= 223 && x <= 283 && y >= 101 && y <= 139) {
      frost->printf(247, 32, "Start Game");
      menuselect = 1;
    }

    if (x >= 253 && x <= 279 && y >= 154 && y <= 184) {
      frost->printf(247, 32, "Quit to OS");
      menuselect = 2;
    }

    if (x >= 193 && x <= 236 && y >= 27 && y <= 69) {
      frost->printf(247, 32, "Edit\n Pilotroster");
      menuselect = 3;
    }

    if (x >= 131 && x <= 182 && y >= 26 && y <= 77) {
      frost->printf(247, 32, "List of Aces");
      menuselect = 4;
    }

    if (x >= 81 && x <= 125 && y >= 27 && y <= 67) {
      frost->printf(247, 32, "Set Controls");
      menuselect = 5;
    }

    if (x >= 30 && x <= 73 && y >= 27 && y <= 54) {
      frost->printf(247, 32, "Game Options");
      menuselect = 6;
    }

    if (x >= 30 && x <= 73 && y >= 57 && y <= 97) {
      frost->printf(247, 32, "Select Level");
      menuselect = 7;
    }

    if (x >= 30 && x <= 73 && y >= 101 && y <= 141) {
      frost->printf(247, 32, "Assign\n players");
      menuselect = 8;
    }

    if (x >= 242 && x <= 295 && y >= 27 && y <= 53) {
      frost->printf(247, 32, "Credits");
      menuselect = 9;
    }

    cursor->blit(x - 10, y - 10);

    if (mission_re_fly != -1) {
      n2 = 1;
      do_all();
    } else
      do_all();

    if (findparameter("-autostart")) {
      n1 = 1;
      menuselect = 1;
    }

    if ((n1 || n2) && menuselect) {
      help_on = 0;

      switch (menuselect) {

      case 1:
        int full_seed;
        int mc_c;

        for (mc_c = 0; mc_c < 16; mc_c++) {
          mc_up[mc_c] = 0;
          mc_down[mc_c] = 0;
          mc_roll[mc_c] = 0;
          mc_power[mc_c] = 0;
          mc_bomb[mc_c] = 0;
          mc_guns[mc_c] = 0;
        }

        full_seed = time(0);

        //// Record

        FILE *rfile;

        if (findparameter("-record")) {
          rfile = settings_open("record.ran", "wb");
          fwrite(&full_seed, sizeof(full_seed), 1, rfile);

          fclose(rfile);
        }

        if (findparameter("-playback")) {
          rfile = settings_open("record.ran", "rb");
          fread(&full_seed, sizeof(full_seed), 1, rfile);

          fclose(rfile);
        }

        ////

        init_vga("PALET5");

        for (l = 0; l < 16; l++) {
          player_exists[l] = 0;
          plane_present[l] = 0;
        }
        for (l = 0; l < 4; l++) {
          if (config.player_type[l] == 1 || config.player_type[l] == 3) {
            if (config.player_number[l] != -1) {
              break;
            }
          }
        }
        if (l == 4) {
          small_warning("You haven't selected any players.\n\n"
                        "Please move on to the\n assignment menu\n");
          break;
        }

        playing_solo = 0;
        solo_mode = -1;

        for (l = 0; l < 4; l++) {
          switch (config.player_type[l]) {
          case 0:
            player_exists[l] = 0;
            break;

          case 1:
            player_exists[l] = 1;
            computer_active[l] = 0;
            playing_solo = 1;
            solo_country = l;
            solo_mode = l;
            break;

          case 2:
            for (l2 = 0; l2 < config.plane_count[l]; l2++) {
              player_exists[l + l2 * 4] = 1;
              computer_active[l + l2 * 4] = 1;
            }

            break;

          case 3:
            for (l2 = 0; l2 < config.plane_count[l]; l2++) {
              player_exists[l + l2 * 4] = 1;
              computer_active[l + l2 * 4] = 1;
            }
            computer_active[l] = 0;

            break;
          }
        }

        if (is_there_sound && (!findparameter("-nomusic"))) {

          sdl_stop_music();
        }
        if (n1)
          gfx::fades::random_fade_out();

        wait_mouse_relase();

        if (config.svga && solo_mode == -1)
          if (!findparameter("-debugnographics")) {
            if (findparameter("-black"))
              init_vesa("PALET3");
            else
              init_vesa("PALET5");
          }

        main_engine();

        if (config.svga) {
          if (!findparameter("-debugnographics")) {

            init_vga("PALET5");
          }
          tyhjaa_vircr();
          do_all();
        }

        if (findparameter("-autoquit")) {
          exit(1);
        }

        if (!findparameter("-debugnoaftermath")) {
          if (aftermath && !findparameter("-playback")) {
            if (!(playing_solo && !mission_duration)) {
              if (playing_solo) {
                do_aftermath(1);

              } else {
                l3 = 0;
                for (l = 0; l < 4; l++)
                  for (l2 = 0; l2 < 4; l2++)
                    if (player_shots_down[l][l2])
                      l3 = 1;

                if (l3) {
                  do_aftermath(1);
                }
              }
            }
          }
        }

        if (level_loaded) {
          level_loaded = 0;
        }

        if (is_there_sound && config.music_on) {

          sdl_play_music(triplane_mod);
        }

        hiiri_to(254, 120);
        break;

      case 2:
        if (n1)
          gfx::fades::random_fade_out();
        exit_flag = 1;
        break;

      case 3:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        roster_menu();

        break;

      case 4:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        aces_menu();

        break;

      case 5:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        controls_menu();

        break;

      case 6:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        options_menu();

        break;

      case 7:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        transfer_menu();

        break;

      case 8:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        assign_menu();
        hiiri_to(254, 120);
        break;

      case 9:
        if (n1)
          gfx::fades::random_fade_out();
        wait_mouse_relase();
        credits_menu();

        break;
      }
    }
  }
}

static void print_clear_roster(Bitmap *rosteri) {
  rosteri->blit(0, 0);
  grid2->printf(152, 40, "Pilot Info");
}

static void print_filled_roster(int number) {
  int ts;

  if (number == -1)
    return;

  grid2->printf(125, 50, "%s %s", rank_names[get_rank(number)],
                roster[number].pilotname);

  frost->printf(122, 65, "Solo Mission Totals:");
  frost->printf(
      122, 75, "Missions flown: %d (%d%% Suc.)", roster[number].solo_mis_flown,
      (roster[number].solo_mis_flown ? roster[number].solo_mis_success * 100 /
                                           roster[number].solo_mis_flown
                                     : 0));
  frost->printf(122, 83, "Kills : %d, Died : %d times",
                roster[number].solo_mis_drops, roster[number].solo_mis_dropped);
  frost->printf(
      122, 91, "Shots used : %d (%d%% Ac.)", roster[number].solo_mis_shotsf,
      (roster[number].solo_mis_shotsf ? roster[number].solo_mis_shotshit * 100 /
                                            roster[number].solo_mis_shotsf
                                      : 0));
  frost->printf(
      122, 99, "Bombs drop : %d (%d%% Ac.)", roster[number].solo_mis_bombs,
      (roster[number].solo_mis_bombs ? roster[number].solo_mis_bombshit * 100 /
                                           roster[number].solo_mis_bombs
                                     : 0));
  frost->printf(122, 107, "- Solo total score: %d",
                roster[number].solo_mis_totals);

  frost->printf(122, 125, "Multiplayer Game Totals:");
  frost->printf(
      122, 135, "Missions flown: %d (%d%% Won)", roster[number].multi_mis_flown,
      (roster[number].multi_mis_flown ? roster[number].multi_mis_success * 100 /
                                            roster[number].multi_mis_flown
                                      : 0));
  frost->printf(122, 143, "Kills : %d, Died : %d times",
                roster[number].multi_mis_drops,
                roster[number].multi_mis_dropped);
  frost->printf(122, 151, "Shots used : %d (%d%% Ac.)",
                roster[number].multi_mis_shotsf,
                (roster[number].multi_mis_shotsf
                     ? roster[number].multi_mis_shotshit * 100 /
                           roster[number].multi_mis_shotsf
                     : 0));
  frost->printf(
      122, 159, "Bombs drop : %d (%d%% Ac.)", roster[number].multi_mis_bombs,
      (roster[number].multi_mis_bombs ? roster[number].multi_mis_bombshit *
                                            100 / roster[number].multi_mis_bombs
                                      : 0));

  ts = calculate_multitotal(number);

  frost->printf(122, 167, "- Multiplayer score: %d", ts);

  frost->printf(122, 177, "Total: %d", ts + roster[number].solo_mis_totals);
}

void wait_mouse_relase(int nokb) {
  int n1 = 1, n2 = 1, x, y;

  while (n1 || n2) {
    koords(&x, &y, &n1, &n2);

    if (!nokb)
      while (kbhit())
        getch();
  }
}

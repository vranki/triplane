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
#include "../gfx/bitmap.h"
#include "../io/joystick.h"
#include "../io/sdl_compat.h"
#include "../io/trip_io.h"
#include "../settings.h"
#include "../triplane.h"
#include "../util/wutil.h"
#include "../world/plane.h"
#include "../world/tripaudio.h"
#include <SDL/SDL.h>
#include <cstring>
#include <ctime>
#include <memory>
#include <array>
#include "../gfx/fades.h"
#include "../gfx/font.h"
#include "../io/dksfile.h"

extern int miss_pl_x[16];
extern int miss_pl_y[16];

char mission_description[80 * 16];
char mission_lines[16][80];
int mission_pixels[16];
int mission_headline_pixels;

int aces_number_of_entries;
int aces_score[MAX_PLAYERS_IN_ROSTER];

constexpr int32_t CHARS_PER_LINE = 70;
constexpr int32_t LINELENGHT = 100;

sb_mod_file *national_mod = nullptr;

char rank_names[6][10] = {"2nd Lt.", "1st Lt.", "Capt.",
                          "Major",   "Lt Col.", "Colonel"};

/**************************** Functions ***************************************/
void joystick_roster_setup(Bitmap *controlme);

void show_feat5() {
  std::unique_ptr<Bitmap> feat5(new Bitmap("FEAT5"));
  int n1 = 0, n2 = 0;
  int x, y;

  wait_mouse_relase();

  feat5->info(&x, &y);

  feat5->blit((320 - x) >> 1, (200 - y) >> 1);
  do_all();

  while (!(n1 || n2)) {
    koords(&x, &y, &n1, &n2);
  }

  wait_mouse_relase();
}

int get_rank(const int player) {
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

int calculate_multitotal(const int player) {
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

void sort_and_show(const int percent) {
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

void aces_grand_total() {
  int c;

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = calculate_multitotal(c) + roster[c].solo_mis_totals;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_multi_total() {
  int c;

  frost->printf(90, 45, "Multiplayer TOP10");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = calculate_multitotal(c);
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_solo_total() {
  int c;

  frost->printf(90, 45, "Sologame TOP10");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_totals;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_triggerhappy() {
  int c;

  frost->printf(90, 45, "The most triggerhappy pilots");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_shotsf + roster[c].multi_mis_shotsf;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_bombdropper() {
  int c;

  frost->printf(90, 45, "Bombusers TOP10");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_bombs + roster[c].multi_mis_bombs;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_shotaccuracy() {
  int c;

  frost->printf(90, 45, "The most accurate shooters");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    if ((roster[c].solo_mis_shotsf + roster[c].multi_mis_shotsf) < 1000)
      aces_score[c] = 0;
    else
      aces_score[c] =
          (roster[c].solo_mis_shotshit + roster[c].multi_mis_shotshit) * 10000 /
          (roster[c].solo_mis_shotsf + roster[c].multi_mis_shotsf);
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show(1);
  }
}

void aces_bombaccuracy() {
  int c;

  frost->printf(90, 45, "The most accurate bombers");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    if ((roster[c].solo_mis_bombs + roster[c].multi_mis_bombs) < 100)
      aces_score[c] = 0;
    else
      aces_score[c] =
          (roster[c].solo_mis_bombshit + roster[c].multi_mis_bombshit) * 10000 /
          (roster[c].solo_mis_bombs + roster[c].multi_mis_bombs);
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show(1);
  }
}

void aces_mostkills() {
  int c;

  frost->printf(90, 45, "Drops TOP10");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_drops + roster[c].multi_mis_drops;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_decoy() {
  int c;

  frost->printf(90, 45, "The most active decoys");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_dropped + roster[c].multi_mis_dropped;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_totalmissions() {
  int c;

  frost->printf(90, 45, "The most missions flown");

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_flown + roster[c].multi_mis_flown;
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void aces_one_solo(const int country, const int mission) {
  char country_names_genetive[4][15] = {"German", "Finnish", "English",
                                        "Japanese"};

  int c;

  frost->printf(82, 45, "%d. %s mission (%s)", mission + 1,
                country_names_genetive[country],
                mission_names[country * 6 + mission]);

  for (c = 0; c < MAX_PLAYERS_IN_ROSTER; c++) {
    if (!roster[c].pilotname[0])
      break;

    aces_score[c] = roster[c].solo_mis_scores[country][mission];
  }

  if (!c) {
    frost->printf(100, 100, "No active pilots present.");

  } else {
    aces_number_of_entries = c;
    sort_and_show();
  }
}

void show_descriptions(const int number) {
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

void load_descriptions(const int number) {
  char *temp_storage;
  int c;
  int locator = 0;
  int index = 0;
  char ch;
  int desclenght;
  int wordcount;

  if (!dksopen("midesc")) {
    printf("Unable to locate data midesc\n");
    exit(1);
  }

  temp_storage = (char *)walloc(dkssize());
  dksread(temp_storage, dkssize());

  for (c = 0; c <= number; c++) {

    while (temp_storage[locator] != '#')
      locator++;

    locator++;
  }

  while (temp_storage[locator] != '#' && locator < dkssize()) {
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

    chars_to_copy = wordcount;

    for (index = 0; index < chars_to_copy; index++) {
      mission_lines[line_going][index] = mission_description[index + locator];
    }

    mission_lines[line_going][index] = 0;

    locator += chars_to_copy;
    line_going++;
  }

  for (index = line_going; index < 16; index++) {
    mission_lines[line_going][0] = 0;
    mission_pixels[line_going] = 0;
  }

  for (index = 0; index < line_going; index++) {
    mission_pixels[index] = frost->printf(0, 0, "%s", mission_lines[index]);
  }

  mission_headline_pixels = frost->printf(0, 0, "%s", mission_names[number]);

  tyhjaa_vircr();

  wfree(temp_storage);
}

int solo_player_menu() {
  char facenames[4][7] = {"GERFAC", "FINFAC", "ENGFAC", "JAPFAC"};
  char missionnames[4][7] = {"MISSI0", "MISSI1", "MISSI2", "MISSI3"};
  char modnames[4][7] = {"mgerma", "mfinla", "mengla", "mjapan"};

  std::unique_ptr<Bitmap> misboa = 0;
  std::unique_ptr<Bitmap> misbak = 0;
  std::unique_ptr<Bitmap> face = 0;
  std::unique_ptr<Bitmap> mission = 0;
  int flag = 0;
  int l;
  int x, y, n1, n2;

  int highest_mission = 0;

  if (findparameter("-solomenumission")) {
    sscanf(parametrit[findparameter("-solomenumission") + 1], "%d",
           &mission_re_fly);
  }

  if (mission_re_fly == 999) {
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

    standard_background = new Bitmap(0, 0, 320, 200);
    do_all_clear();
  }

  if ((mission_re_fly == -1) && is_there_sound && config.music_on &&
      !findparameter("-nomusic")) {
    sdl_stop_music();
    national_mod = sdl_load_mod_file(modnames[solo_country]);
    if (national_mod == nullptr) {
      printf("Error locating music.\n");
      exit(1);
    }

    sdl_play_music(national_mod);
  }

  while (flag == 0) {
    if (kbhit() && getch() == SDLK_ESCAPE) {
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
    delete standard_background;
    standard_background = nullptr;
  }

  if (flag == 1)
    random_fade_out();
  else {
    tyhjaa_vircr();
    do_all();
  }

  wait_mouse_relase(1);
  switch (flag) {

  case 1:

    return 1;

  case 3:
    if ((mission_re_fly == -1) && is_there_sound && config.music_on &&
        !findparameter("-nomusic")) {
      sdl_stop_music();
      sdl_free_mod_file(national_mod);
    }
    return 2;

  case 2:
    if (is_there_sound && config.music_on && !findparameter("-nomusic")) {
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
  char ch;
  Bitmap *help;
  Bitmap *rosteri;
  Bitmap *buttl, *buttr;
  Bitmap *setk1, *setk2;
  Bitmap *medal2[4];
  Bitmap *medal3;
  Bitmap *medal5;
  Bitmap *temp;
  Bitmap *ribbon[6];
  int l, l2, l3, l4;
  int rank;

  if (number == -1 && roster[0].pilotname[0])
    number = 0;

  rosteri = new Bitmap("ROSTER");
  buttl = new Bitmap("BUTTL"); // 187,174
  buttr = new Bitmap("BUTTR"); // 205,174
  setk1 = new Bitmap("SETK1");
  setk2 = new Bitmap("SETK2");
  help = new Bitmap("HELP2");
  temp = new Bitmap("MEDAL2");
  medal3 = new Bitmap("MEDAL3");

  for (l = 0; l < 4; l++) {
    medal2[l] = new Bitmap(1 + l * 17, 1, 16, 28, temp);
  }

  medal5 = new Bitmap(1 + 4 * 17, 1, 16, 28, temp);

  delete temp;

  temp = new Bitmap("RIBBON");

  for (l = 0; l < 6; l++)
    ribbon[l] = new Bitmap(36 * l, 0, 35, 8, temp);

  delete temp;

  hiiri_to(264, 109);

  while (!exit_flag) {
    if (kbhit()) {
      if (!(ch = getch())) {
        ch = getch();
        if (ch == 59)
          wtoggle(&help_on);

      } else {
        if (ch == 27)
          exit_flag = 1;
      }
    }

    koords(&x, &y, &n1, &n2);

    if (number != -1) {
      if (!keysetmode) {
        print_clear_roster(rosteri);
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
                        SDL_GetKeyName((SDLKey)roster[number].up),
                        SDL_GetKeyName((SDLKey)roster[number].down),
                        SDL_GetKeyName((SDLKey)roster[number].roll));
          frost->printf(170, 114, "Power [%s]\nBombs [%s]\nGuns [%s]\n",
                        SDL_GetKeyName((SDLKey)roster[number].power),
                        SDL_GetKeyName((SDLKey)roster[number].bombs),
                        SDL_GetKeyName((SDLKey)roster[number].guns));
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

          wfree(ups);
          wfree(downs);
          wfree(rolls);
          wfree(powers);
          wfree(gunss);
          wfree(bombss);
          wfree(brakes);
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
          number = 99;
          break;
        }

        print_clear_roster(rosteri);
        frost->printf(15, 24, "Type name of\nthe pilot\nand press\nenter.");

        roster[number].solo_mis_flown = 0;
        roster[number].solo_mis_success = 0;
        roster[number].solo_mis_drops = 0;
        roster[number].solo_mis_shotsf = 0;
        roster[number].solo_mis_shotshit = 0;
        roster[number].solo_mis_bombs = 0;
        roster[number].solo_mis_bombshit = 0;
        roster[number].solo_mis_dropped = 0;
        roster[number].solo_mis_totals = 0;

        roster[number].multi_mis_flown = 0;
        roster[number].multi_mis_success = 0;
        roster[number].multi_mis_drops = 0;
        roster[number].multi_mis_shotsf = 0;
        roster[number].multi_mis_shotshit = 0;
        roster[number].multi_mis_bombs = 0;
        roster[number].multi_mis_bombshit = 0;
        roster[number].multi_mis_dropped = 0;
        roster[number].multi_mis_totals = 0;

        for (l = 0; l < 4; l++)
          for (l2 = 0; l2 < 6; l2++)
            roster[number].solo_mis_scores[l][l2] = 0;

        roster[number].up = SDLK_x;
        roster[number].down = SDLK_w;
        roster[number].roll = SDLK_s;
        roster[number].power = SDLK_TAB;
        roster[number].guns = SDLK_2;
        roster[number].bombs = SDLK_1;

        grid2->scanf(125, 50, roster[number].pilotname, 21);
        if (!roster[number].pilotname[0]) {
          number--;
          break;
        }

        break;

      case 2:
        if (n1)
          random_fade_out();
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
        if ((number == MAX_PLAYERS_IN_ROSTER) |
            (roster[number].pilotname[0] == 0))
          number--;
        break;

      case 6:
        setk2->blit(286, 190);
        cursor->blit(x - 10, y - 10);
        do_all();
        wait_mouse_relase();
        wtoggle(&keysetmode);
        break;

      case 7:
        setk2->blit(125, 172);
        cursor->blit(x - 10, y - 10);
        do_all();
        wait_mouse_relase();

        config.joystick[4] = -1;

        rosteri->blit(0, 0);

        frost->printf(125, 100, "Key for upward turn [%s]",
                      SDL_GetKeyName((SDLKey)roster[number].up));
        do_all();

        roster[number].up = select_key(number, roster[number].up);

        rosteri->blit(0, 0);
        frost->printf(125, 100, "Key for downward turn [%s]",
                      SDL_GetKeyName((SDLKey)roster[number].down));
        do_all();

        roster[number].down = select_key(number, roster[number].down);

        rosteri->blit(0, 0);
        frost->printf(125, 100, "Key for roll [%s]",
                      SDL_GetKeyName((SDLKey)roster[number].roll));
        do_all();

        roster[number].roll = select_key(number, roster[number].roll);

        rosteri->blit(0, 0);

        frost->printf(125, 100, "Key for engine power [%s]",
                      SDL_GetKeyName((SDLKey)roster[number].power));
        do_all();

        roster[number].power = select_key(number, roster[number].power);

        rosteri->blit(0, 0);

        frost->printf(125, 100, "Key for bomb drop [%s]",
                      SDL_GetKeyName((SDLKey)roster[number].bombs));
        do_all();

        roster[number].bombs = select_key(number, roster[number].bombs);

        rosteri->blit(0, 0);

        frost->printf(125, 100, "Key for guns [%s]",
                      SDL_GetKeyName((SDLKey)roster[number].guns));
        do_all();

        roster[number].guns = select_key(number, roster[number].guns);
        break;

      case 8:
        if (joystick_exists & JOY1) {
          config.joystick[4] = 1;
          joystick_roster_setup(rosteri);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[4] = 1;
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
          strcpy(roster[l - 1].pilotname, roster[l].pilotname);
          roster[l - 1].solo_mis_flown = roster[l].solo_mis_flown;
          roster[l - 1].solo_mis_success = roster[l].solo_mis_success;
          roster[l - 1].solo_mis_drops = roster[l].solo_mis_drops;
          roster[l - 1].solo_mis_shotsf = roster[l].solo_mis_shotsf;
          roster[l - 1].solo_mis_shotshit = roster[l].solo_mis_shotshit;
          roster[l - 1].solo_mis_bombs = roster[l].solo_mis_bombs;
          roster[l - 1].solo_mis_bombshit = roster[l].solo_mis_bombshit;
          roster[l - 1].solo_mis_dropped = roster[l].solo_mis_dropped;
          roster[l - 1].solo_mis_totals = roster[l].solo_mis_totals;

          for (l3 = 0; l3 < 4; l3++)
            for (l2 = 0; l2 < 6; l2++)
              roster[l - 1].solo_mis_scores[l3][l2] =
                  roster[l].solo_mis_scores[l3][l2];

          roster[l - 1].multi_mis_flown = roster[l].multi_mis_flown;
          roster[l - 1].multi_mis_success = roster[l].multi_mis_success;
          roster[l - 1].multi_mis_drops = roster[l].multi_mis_drops;
          roster[l - 1].multi_mis_shotsf = roster[l].multi_mis_shotsf;
          roster[l - 1].multi_mis_shotshit = roster[l].multi_mis_shotshit;
          roster[l - 1].multi_mis_bombs = roster[l].multi_mis_bombs;
          roster[l - 1].multi_mis_bombshit = roster[l].multi_mis_bombshit;
          roster[l - 1].multi_mis_dropped = roster[l].multi_mis_dropped;
          roster[l - 1].multi_mis_totals = roster[l].multi_mis_totals;

          roster[l - 1].up = roster[l].up;
          roster[l - 1].down = roster[l].down;
          roster[l - 1].roll = roster[l].roll;
          roster[l - 1].power = roster[l].power;
          roster[l - 1].guns = roster[l].guns;
          roster[l - 1].bombs = roster[l].bombs;

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

  delete help;
  delete buttl;
  delete buttr;
  delete setk1;
  delete setk2;
  delete rosteri;
  for (l = 0; l < 4; l++)
    delete medal2[l];
  delete medal3;
  delete medal5;

  for (l = 0; l < 6; l++)
    delete ribbon[l];

  wait_mouse_relase();
}

void check_other_joys(int num, int active) {
  for (int i = 0; i < 4; i++) {
    if (i == num)
      continue;
    if (config.joystick[i] == active)
      config.joystick[i] = -1;
  }
}

void options_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int menusubselect = 0;
  Bitmap *optionme;
  Bitmap *kohta[4];
  Bitmap *right, *wrong;
  Bitmap *opt1, *opt2;
  int optimode = 0;
  int l;

  char selitykset[4][80] = {"This form has questions about your vision.",
                            "This form has questions about your hearing.",
                            "Phychological questions about flying.",
                            "Questions about hostile environments."};

  optionme = new Bitmap("OPTION");
  right = new Bitmap("RIGHT");
  wrong = new Bitmap("WRONG");
  kohta[3] = new Bitmap("OPTI1");
  kohta[2] = new Bitmap("OPTI2");
  kohta[1] = new Bitmap("OPTI3");
  kohta[0] = new Bitmap("OPTI4");
  opt1 = new Bitmap("OPT1");
  opt2 = new Bitmap("OPT2");

  while (!exit_flag) {
    if (kbhit())
      if (getch() == 27)
        exit_flag = 1;

    koords(&x, &y, &n1, &n2);
    optionme->blit(0, 0);
    kohta[3 - optimode]->blit(248, 13);
    frost->printf(73, 43, "%s", selitykset[optimode]);

    switch (optimode) {
    case 0:
      frost->printf(73, 60, "Shots visible?");
      frost->printf(73, 70, "AAA shots visible?");
      frost->printf(73, 80, "AA-MG shots visible?");
      frost->printf(73, 90, "Flags?");
      frost->printf(73, 100, "Structure flames?");
      frost->printf(73, 110, "Use %s window?",
                    split_num == 2
                        ? "800x600"
                        : (split_num == 1 ? "1200x400" : "2400:200"));
      frost->printf(73, 120, "Structure smoke?");

      for (l = 0; l < 7; l++) {
        boxi(214, 60 + l * 10, 221, 68 + l * 10, 0);
        boxi(224, 60 + l * 10, 231, 68 + l * 10, 0);
      }

      if (config.shots_visible)
        right->blit(215, 60);
      else
        wrong->blit(225, 60);

      if (config.it_shots_visible)
        right->blit(215, 70);
      else
        wrong->blit(225, 70);

      if (config.aa_mg_shots_visible)
        right->blit(215, 80);
      else
        wrong->blit(225, 80);

      if (config.flags)
        right->blit(215, 90);
      else
        wrong->blit(225, 90);

      if (config.flames)
        right->blit(215, 100);
      else
        wrong->blit(225, 100);

      if (config.structure_smoke)
        right->blit(215, 120);
      else
        wrong->blit(225, 120);

      if (config.svga)
        right->blit(215, 110);
      else
        wrong->blit(225, 110);

      break;

    case 1:

      frost->printf(73, 60, "Sounds on?");
      frost->printf(73, 70, "Musics on?");
      frost->printf(73, 80, "SFX on?");
      frost->printf(73, 90, "Explosion sounds?");
      frost->printf(73, 100, "Gunfire sounds?");
      frost->printf(73, 110, "AAA-fire sounds?");
      frost->printf(73, 120, "Water splash sound?");
      frost->printf(73, 130, "Infantry sounds?");

      for (l = 0; l < 8; l++) {
        boxi(214, 60 + l * 10, 221, 68 + l * 10, 0);
        boxi(224, 60 + l * 10, 231, 68 + l * 10, 0);
      }

      if (config.sound_on)
        right->blit(215, 60);
      else
        wrong->blit(225, 60);

      if (config.music_on)
        right->blit(215, 70);
      else
        wrong->blit(225, 70);

      if (config.sfx_on)
        right->blit(215, 80);
      else
        wrong->blit(225, 80);

      if (config.explosion_sounds)
        right->blit(215, 90);
      else
        wrong->blit(225, 90);

      if (config.gunshot_sounds)
        right->blit(215, 100);
      else
        wrong->blit(225, 100);

      if (config.ground_i_sounds)
        right->blit(215, 110);
      else
        wrong->blit(225, 110);

      if (config.splash)
        right->blit(215, 120);
      else
        wrong->blit(225, 120);

      if (config.infantry_sounds)
        right->blit(215, 130);
      else
        wrong->blit(225, 130);

      break;

    case 2:

      frost->printf(73, 60, "Do you want on/off power?");
      frost->printf(73, 70, "Do you want a reversed\n  powerswitch?");

      for (l = 0; l < 2; l++) {
        boxi(214, 60 + l * 10, 221, 68 + l * 10, 0);
        boxi(224, 60 + l * 10, 231, 68 + l * 10, 0);
      }

      if (config.poweronoff)
        right->blit(215, 60);
      else
        wrong->blit(225, 60);

      if (config.powerrev)
        right->blit(215, 70);
      else
        wrong->blit(225, 70);

      break;

    case 3:

      frost->printf(73, 60, "Are all planes the same?");
      frost->printf(73, 70, "Collisions on?");
      frost->printf(73, 80, "Flying parts hit plane?");
      frost->printf(73, 90, "Battle ends after n points");
      frost->printf(73, 100, "Alliances enabled?");
      frost->printf(73, 110, "AA-MGs?");
      frost->printf(73, 120, "AAAs?");
      frost->printf(73, 130, "Infantry?");
      frost->printf(73, 140, "Unlimited ammo?");
      frost->printf(73, 150, "Unlimited gas?");

      for (l = 0; l < 10; l++) {
        if (l == 3)
          continue;
        boxi(214, 60 + l * 10, 221, 68 + l * 10, 0);
        boxi(224, 60 + l * 10, 231, 68 + l * 10, 0);
      }

      if (config.all_planes_are) {
        right->blit(215, 60);
        opt1->blit(1, 1);
        boxi(34, 10, 41, 18, 0);
        boxi(34, 31, 41, 39, 0);
        boxi(34, 52, 41, 60, 0);
        boxi(34, 73, 41, 81, 0);

        right->blit(35, config.all_planes_are * 21 - 11);
      } else
        wrong->blit(225, 60);

      if (config.nocollision)
        right->blit(215, 70);
      else
        wrong->blit(225, 70);

      if (config.partcollision)
        right->blit(215, 80);
      else
        wrong->blit(225, 80);

      /*
         if(config.stop)
         right->blit(215,90);
         else
         wrong->blit(225,90);
       */

      if (config.stop)
        frost->printf(216, 91, "%d", config.stop);
      else
        frost->printf(216, 91, "UnLtd");

      if (config.alliance) {
        right->blit(215, 100);
        opt2->blit(1, 92);
        boxi(5, 92 + config.alliance * 32 - 28, 53,
             92 + config.alliance * 32 + 4, 38);
      } else
        wrong->blit(225, 100);

      if (config.aa_mgs)
        right->blit(215, 110);
      else
        wrong->blit(225, 110);

      if (config.it_guns)
        right->blit(215, 120);
      else
        wrong->blit(225, 120);

      if (config.infantry)
        right->blit(215, 130);
      else
        wrong->blit(225, 130);

      if (config.unlimited_ammo)
        right->blit(215, 140);
      else
        wrong->blit(225, 140);

      if (config.unlimited_gas)
        right->blit(215, 150);
      else
        wrong->blit(225, 150);

      break;
    }

    menuselect = 0;
    /*if(x>=257 && x<=272 && y>=53 && y<=173)
       {
       frost->printf(15,24,"Create new\n pilot");
       menuselect=1;
       } */

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

    if (optimode == 0)
      if ((x >= 214 && x <= 221) || (x >= 224 && x <= 231)) {
        if (x <= 221)
          menusubselect = 1;
        else
          menusubselect = 0;

        if (y >= 60 && y <= 68) {
          // shots_visible
          menuselect = 10;
        }

        if (y >= 70 && y <= 78) {
          // it_shots_visible
          menuselect = 11;
        }

        if (y >= 80 && y <= 88) {
          // aa_mg_shots_visible
          menuselect = 12;
        }

        if (y >= 90 && y <= 98) {
          // flags
          menuselect = 13;
        }

        if (y >= 100 && y <= 108) {
          // flames
          menuselect = 14;
        }

        if (y >= 120 && y <= 128) {
          // structure_smoke
          menuselect = 64;
        }

        if (y >= 110 && y <= 118) {
          // svga
          menuselect = 15;
        }
      }

    if (optimode == 1)
      if ((x >= 214 && x <= 221) || (x >= 224 && x <= 231)) {
        if (x <= 221)
          menusubselect = 1;
        else
          menusubselect = 0;

        if (y >= 60 && y <= 68) {
          // sounds_on
          menuselect = 16;
        }

        if (y >= 70 && y <= 78) {
          // music_on
          menuselect = 17;
        }

        if (y >= 80 && y <= 88) {
          // sfx_on
          menuselect = 18;
        }

        if (y >= 90 && y <= 98) {
          // explosion_sounds
          menuselect = 19;
        }

        if (y >= 100 && y <= 108) {
          // gunshot_sounds
          menuselect = 20;
        }

        if (y >= 110 && y <= 118) {
          // ground_i_sounds
          menuselect = 21;
        }

        if (y >= 120 && y <= 128) {
          // splash
          menuselect = 22;
        }

        if (y >= 130 && y <= 138) {
          // infantry_sounds
          menuselect = 50;
        }
      }

    if (optimode == 2)
      if ((x >= 214 && x <= 221) || (x >= 224 && x <= 231)) {
        if (x <= 221)
          menusubselect = 1;
        else
          menusubselect = 0;

        if (y >= 60 && y <= 68) {
          // poweronoff
          menuselect = 23;
        }

        if (y >= 70 && y <= 78) {
          // powerrev
          menuselect = 24;
        }
      }

    if (optimode == 3) {
      if (x >= 214 && x <= 231 && y >= 90 && y <= 98)
        menuselect = 28;

      if ((x >= 214 && x <= 221) || (x >= 224 && x <= 231)) {
        if (x <= 221)
          menusubselect = 1;
        else
          menusubselect = 0;

        if (y >= 60 && y <= 68) {
          // all_planes_are
          menuselect = 25;
        }

        if (y >= 70 && y <= 78) {
          // nocollision
          menuselect = 26;
        }

        if (y >= 80 && y <= 88) {
          // partcollision
          menuselect = 27;
        }

        if (y >= 90 && y <= 98) {
          // stop
          menuselect = 28;
        }

        if (y >= 100 && y <= 108) {
          // alliance
          menuselect = 29;
        }

        if (y >= 110 && y <= 118) {
          // aa_mgs
          menuselect = 30;
        }

        if (y >= 120 && y <= 128) {
          // it_gungs
          menuselect = 31;
        }

        if (y >= 130 && y <= 138) {
          // infantry
          menuselect = 32;
        }

        if (y >= 140 && y <= 148) {
          // unlimited_ammo
          menuselect = 33;
        }

        if (y >= 150 && y <= 158) {
          // unlimited_gas
          menuselect = 34;
        }
      }

      if (config.all_planes_are && x >= 34 && y >= 10 && x <= 41 && y <= 18) {
        menuselect = 51;
      }

      if (config.all_planes_are && x >= 34 && y >= 31 && x <= 41 && y <= 39) {
        menuselect = 52;
      }

      if (config.all_planes_are && x >= 34 && y >= 52 && x <= 41 && y <= 60) {
        menuselect = 53;
      }

      if (config.all_planes_are && x >= 34 && y >= 73 && x <= 41 && y <= 81) {
        menuselect = 54;
      }

      if (config.alliance && x >= 5 && y >= 96 && x <= 53 && y <= 128) {
        menuselect = 55;
      }

      if (config.alliance && x >= 5 && y >= 129 && x <= 53 && y <= 160) {
        menuselect = 56;
      }

      if (config.alliance && x >= 5 && y >= 161 && x <= 53 && y <= 182) {
        menuselect = 57;
      }
    }

    cursor->blit(x - 10, y - 10);
    do_all(1);

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 2:
        if (n1)
          random_fade_out();
        else {
          tyhjaa_vircr();
          do_all();
        }
        exit_flag = 1;
        break;

      case 10:
        config.shots_visible = menusubselect;
        break;

      case 11:
        config.it_shots_visible = menusubselect;
        break;

      case 12:
        config.aa_mg_shots_visible = menusubselect;
        break;

      case 13:
        config.flags = menusubselect;
        break;

      case 14:
        config.flames = menusubselect;
        break;

      case 64:
        config.structure_smoke = menusubselect;
        break;

      case 15:
        config.svga = menusubselect;
        break;

      case 16:
        config.sound_on = menusubselect;
        break;

      case 17:
        config.music_on = menusubselect;
        break;

      case 18:
        config.sfx_on = menusubselect;
        break;

      case 19:
        config.explosion_sounds = menusubselect;
        break;

      case 20:
        config.gunshot_sounds = menusubselect;
        break;

      case 21:
        config.ground_i_sounds = menusubselect;
        break;

      case 22:
        config.splash = menusubselect;
        break;

      case 50:
        config.infantry_sounds = menusubselect;
        break;

      case 23:
        config.poweronoff = menusubselect;
        break;

      case 24:
        config.powerrev = menusubselect;
        break;

      case 25:
        config.all_planes_are = menusubselect;
        break;

      case 26:
        config.nocollision = menusubselect;
        break;

      case 27:
        config.partcollision = menusubselect;
        break;

      case 28:
        if (n1) {
          if (++config.stop > 999)
            config.stop = 999;

        } else {
          if (--config.stop < 0)
            config.stop = 0;
        }

        break;

      case 29:
        config.alliance = menusubselect;
        break;

      case 30:
        config.aa_mgs = menusubselect;
        break;

      case 31:
        config.it_guns = menusubselect;
        break;

      case 32:
        config.infantry = menusubselect;
        break;

      case 33:
        config.unlimited_ammo = menusubselect;
        break;

      case 34:
        config.unlimited_gas = menusubselect;
        break;
      }

      if (menuselect >= 3 && menuselect <= 6)
        optimode = menuselect - 3;

      if (menuselect >= 51 && menuselect <= 54) {
        config.all_planes_are = menuselect - 50;
      }

      if (menuselect >= 55 && menuselect <= 57) {
        config.alliance = menuselect - 54;
      }
    }
  }

  delete kohta[0];
  delete kohta[1];
  delete kohta[2];
  delete kohta[3];
  delete optionme;
  delete right;
  delete wrong;
  delete opt1;
  delete opt2;

  if (config.sound_on && (is_there_sound == 0))
    init_sounds();

  if (is_there_sound && config.music_on && (!music_loaded)) {

    load_music();
    sdl_play_music(triplane_mod);
  }

  if (music_loaded && (!is_there_sound || !config.music_on)) {
    sdl_stop_music();
    clear_music();
  }

  if (is_there_sound && config.sfx_on && (!sfx_loaded))
    load_sfx();

  if (sfx_loaded && (!is_there_sound || !config.sfx_on))
    clear_sfx();

  if (!config.sound_on && (is_there_sound == 1))
    uninit_sounds();

  wait_mouse_relase();
}

void transfer_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  Bitmap *optionme;
  Bitmap *color_bites[6];
  Bitmap *descs[6];
  int l;

  optionme = new Bitmap("TRANS2");

  color_bites[0] = new Bitmap(39, 46, 80, 50, optionme);
  color_bites[1] = new Bitmap(119, 46, 80, 50, optionme);
  color_bites[2] = new Bitmap(199, 46, 80, 50, optionme);
  color_bites[3] = new Bitmap(39, 96, 80, 50, optionme);
  color_bites[4] = new Bitmap(119, 96, 80, 50, optionme);
  color_bites[5] = new Bitmap(199, 96, 80, 50, optionme);

  descs[0] = new Bitmap("DESC1");
  descs[1] = new Bitmap("DESC2");
  descs[2] = new Bitmap("DESC3");
  descs[3] = new Bitmap("DESC4");
  descs[4] = new Bitmap("DESC5");
  descs[5] = new Bitmap("DESC6");

  delete optionme;

  optionme = new Bitmap("TRANSF");

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
      // frost->printf(25,180,"Back to main menu");
      menuselect = 2;
    }

    for (l = 0; l < 6; l++)
      if (x >= (39 + l * 80 - (l / 3) * 240) &&
          x <= (118 + l * 80 - (l / 3) * 240) && y >= (46 + (l / 3) * 50) &&
          y <= (95 + (l / 3) * 50)) {
        // frost->printf(25,180,"Select level %d",l+1);
        menuselect = 3 + l;
      }

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 2:
        if (n1)
          random_fade_out();
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

  for (x = 0; x < 6; x++) {
    delete color_bites[x];
    delete descs[x];
  }

  delete optionme;

  wait_mouse_relase();
}

static void joystick_setup(int joy, std::unique_ptr<Bitmap>& controlme) {
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

  if (joy == 0)
    open_close_joysticks(1, 0, 0, 0);
  else if (joy == 1)
    open_close_joysticks(0, 1, 0, 0);
  else if (joy == 2)
    open_close_joysticks(0, 0, 1, 0);
  else if (joy == 3)
    open_close_joysticks(0, 0, 0, 1);

  idle = allocate_axis_state(joy);
  current = allocate_axis_state(joy);

  controlme->blit(0, 0);
  frost->printf(54, 93, "Keep joystick idle and press");
  frost->printf(54, 100, "Space (Esc=use old settings)");
  do_all();
  do {
    c = getch();
  } while (c != 27 && c != ' ');
  if (c == 27)
    goto joystick_setup_exit;

  save_axis_state(idle, joy);

  for (i = 0; acts[i].prompt != nullptr; i++) {
    controlme->blit(0, 0);
    frost->printf(54, 93, "Do '%s' on joystick and", acts[i].prompt);
    frost->printf(54, 100, "press Space or D=disable this");
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
  wfree(idle);
  wfree(current);

  open_close_joysticks(0, 0, 0, 0);
}

void joystick_roster_setup(Bitmap *controlme) {
  Sint16 *idle, *current;
  int i, c;
  struct {
    const char *prompt;
    joystick_action *act;
  } acts[] = {{"Up", &joystick_config[4].up},
              {"Down", &joystick_config[4].down},
              {"Roll", &joystick_config[4].roll},
              {"Power", &joystick_config[4].power},
              {"Guns", &joystick_config[4].guns},
              {"Bombs", &joystick_config[4].bombs},
              {"Brake", &joystick_config[4].brake},
              {nullptr, nullptr}};

  open_close_joysticks(1, 0, 0, 0);

  idle = allocate_axis_state(4);
  current = allocate_axis_state(4);

  controlme->blit(0, 0);
  frost->printf(125, 100, "Keep joystick idle and press");
  frost->printf(125, 107, "Space (Esc=use old settings)");
  do_all();
  do {
    c = getch();
  } while (c != 27 && c != ' ');
  if (c == 27)
    goto joystick_setup_exit;

  save_axis_state(idle, 4);

  for (i = 0; acts[i].prompt != nullptr; i++) {
    controlme->blit(0, 0);
    frost->printf(125, 100, "Do '%s' on joystick and", acts[i].prompt);
    frost->printf(125, 107, "press Space or D=disable this");
    do_all();
    do {
      c = getch();
    } while (c != 27 && c != ' ' && c != 'd' && c != 'D');
    if (c == 27) {
      goto joystick_setup_exit;
    } else if (c == 'd' || c == 'D') {
      set_disabled_action(acts[i].act);
    } else {
      if (!find_changed_button(acts[i].act, 4)) {
        save_axis_state(current, 4);
        find_changed_axis(acts[i].act, idle, current, 4);
      }
    }
  }

joystick_setup_exit:
  wfree(idle);
  wfree(current);

  open_close_joysticks(0, 0, 0, 0);
}

void controls_menu() {
  char ch;
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
      if (!(ch = getch())) {
        ch = getch();
        if (ch == 59)
          wtoggle(&help_on);

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
                    SDL_GetKeyName((SDLKey)player_keys[active].up),
                    SDL_GetKeyName((SDLKey)player_keys[active].down),
                    SDL_GetKeyName((SDLKey)player_keys[active].roll));
      frost->printf(170, 100, "Power [%s] Bombs [%s] Guns [%s]",
                    SDL_GetKeyName((SDLKey)player_keys[active].power),
                    SDL_GetKeyName((SDLKey)player_keys[active].bombs),
                    SDL_GetKeyName((SDLKey)player_keys[active].guns));
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

      wfree(ups);
      wfree(downs);
      wfree(rolls);
      wfree(powers);
      wfree(gunss);
      wfree(bombss);
      wfree(brakes);
    }

    cursor->blit(x - 10, y - 10);
    do_all();

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 2:
        if (n1)
          random_fade_out();
        exit_flag = 1;
        break;

      case 3:
        for (int i = 0; i < 4; i++) {
          if (config.joystick[i] == active)
            config.joystick[i] = -1;
        }

        controlme->blit(0, 0);
        napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
        frost->printf(56, 97, "Key for upward turn [%s]",
                      SDL_GetKeyName((SDLKey)player_keys[active].up));
        do_all();

        player_keys[active].up = select_key(active, player_keys[active].up);

        controlme->blit(0, 0);
        napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
        frost->printf(56, 97, "Key for downward turn [%s]",
                      SDL_GetKeyName((SDLKey)player_keys[active].down));
        do_all();

        player_keys[active].down = select_key(active, player_keys[active].down);

        controlme->blit(0, 0);
        napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
        frost->printf(56, 97, "Key for roll [%s]",
                      SDL_GetKeyName((SDLKey)player_keys[active].roll));
        do_all();

        player_keys[active].roll = select_key(active, player_keys[active].roll);

        controlme->blit(0, 0);
        napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
        frost->printf(56, 97, "Key for engine power [%s]",
                      SDL_GetKeyName((SDLKey)player_keys[active].power));
        do_all();

        player_keys[active].power =
            select_key(active, player_keys[active].power);

        controlme->blit(0, 0);
        napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
        frost->printf(56, 97, "Key for bomb drop [%s]",
                      SDL_GetKeyName((SDLKey)player_keys[active].bombs));
        do_all();

        player_keys[active].bombs =
            select_key(active, player_keys[active].bombs);

        controlme->blit(0, 0);
        napp[active]->blit((active % 2) * 27 + 10, (active / 2) * 23 + 22);
        frost->printf(56, 97, "Key for guns [%s]",
                      SDL_GetKeyName((SDLKey)player_keys[active].guns));
        do_all();

        player_keys[active].guns = select_key(active, player_keys[active].guns);

        save_keyset();
        break;

      case 8:
        if (joystick_exists & JOY1) {
          config.joystick[0] = active;
          check_other_joys(0, active);
          joystick_setup(0, controlme);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[0] = 1;
        }
        break;

      case 9:
        if (joystick_exists & JOY2) {
          config.joystick[1] = active;
          check_other_joys(1, active);
          joystick_setup(1, controlme);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[1] = 1;
        }
        break;

      case 10:
        if (joystick_exists & JOY3) {
          config.joystick[2] = active;
          check_other_joys(2, active);
          joystick_setup(2, controlme);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[2] = 1;
        }
        break;

      case 11:
        if (joystick_exists & JOY4) {
          config.joystick[3] = active;
          check_other_joys(3, active);
          joystick_setup(3, controlme);
          save_joysticks_data(CALIBRATION_FILENAME);
          config.joystick_calibrated[3] = 1;
        }
        break;
      }

      if (menuselect >= 4 && menuselect <= 7) {
        active = menuselect - 4;
      }
    }
  }

  wait_mouse_relase();
}

void assign_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int menusubselect1 = 0, menusubselect2 = 0;
  Bitmap *acesme;
  Bitmap *ruksi;
  Bitmap *help;
  int l, lx, ly;
  int l2, l3;
  int lym[4] = {0, 11, 24, 36};
  int response;
  int help_on = 0;
  char ch;

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

  acesme = new Bitmap("ASSIGN");

  ruksi = new Bitmap("RUKSI");
  help = new Bitmap("HELP5");

  while (!exit_flag) {
    if (kbhit()) {
      if (!(ch = getch())) {
        ch = getch();
        if (ch == 59)
          wtoggle(&help_on);

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
      // frost->printf(82,177,"Back to main menu");
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

      if (x >= (134 + lx) && x < (143 + lx) && y >= (79 + ly) &&
          y <= (89 + ly)) {
        menusubselect1 = l;
        menuselect = 3;
        // frost->printf(82,177,"Select next pilot");
      }

      if (x >= (143 + lx) && x <= (151 + lx) && y >= (79 + ly) &&
          y <= (89 + ly)) {
        menusubselect1 = l;
        menuselect = 4;
        // frost->printf(82,177,"Select previous pilot");
      }

      if (x >= (134 + lx) && x < (143 + lx) && y >= (67 + ly) &&
          y <= (77 + ly)) {
        menusubselect1 = l;
        menuselect = 5;
        // frost->printf(82,177,"Add plane");
      }

      if (x >= (143 + lx) && x <= (151 + lx) && y >= (67 + ly) &&
          y <= (77 + ly)) {
        menusubselect1 = l;
        menuselect = 6;
        // frost->printf(82,177,"Remove plane");
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
          random_fade_out();
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

  delete acesme;
  delete ruksi;
  delete help;

  wait_mouse_relase();
}

void aces_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int current_page = 0;
  int help_on = 0;
  char ch;
  Bitmap *acesme;
  Bitmap *firstpage;
  Bitmap *buttl;
  Bitmap *buttr;
  Bitmap *help;

  if (is_there_sound && config.music_on) {
    sdl_stop_music();
    sdl_play_music(aces_mod);
  }

  acesme = new Bitmap("ACESA");
  firstpage = new Bitmap("ACESB");
  buttl = new Bitmap("BUTTL");
  buttr = new Bitmap("BUTTR");
  help = new Bitmap("HELP3");

  acesme->blit(0, 0);
  standard_background = new Bitmap(0, 0, 320, 200);
  do_all_clear();

  while (!exit_flag) {

    if (kbhit()) {
      if (!(ch = getch())) {
        ch = getch();
        if (ch == 59)
          wtoggle(&help_on);

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

    if (!current_page)
      aces_grand_total();
    if (current_page == 1)
      aces_multi_total();
    if (current_page == 2)
      aces_solo_total();
    if (current_page > 2 && current_page <= 26)
      aces_one_solo((current_page - 3) / 6,
                    (current_page - 3) - ((current_page - 3) / 6) * 6);

    switch (current_page) {
    case 27:
      aces_triggerhappy();
      break;

    case 28:
      aces_bombdropper();
      break;

    case 29:
      aces_shotaccuracy();
      break;

    case 30:
      aces_bombaccuracy();
      break;

    case 31:
      aces_mostkills();
      break;

    case 32:
      aces_decoy();
      break;

    case 33:
      aces_totalmissions();
      break;
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

  delete standard_background;
  standard_background = nullptr;
  if (n1)
    random_fade_out();
  delete help;
  delete acesme;
  delete firstpage;
  delete buttl;
  delete buttr;

  wait_mouse_relase();
}

int kangas_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  int menuselect;
  int place_x = -2079;
  int showing_texts = 1;

  Bitmap *kangas = new Bitmap("KANGAS", 0);
  Bitmap *buttr = new Bitmap("BUTTR");
  Bitmap *buttl = new Bitmap("BUTTL");
  Bitmap *fly = new Bitmap("FLY");
  Bitmap *exit = new Bitmap("EXIT");

  init_vga("PALET3");

  if (is_there_sound && config.sfx_on) {
    if (soundcard_type != SOUNDCARD_GUS) {
      sample_hurr->left_volume = SAMPLE_VOLUME - 5;
      sample_hurr->right_volume = SAMPLE_VOLUME - 5;
      sdl_stop_music();
      sdl_play_sample(sample_hurr, 1);
    }
  }

  hiiri_to(80, 195);

  if (miss_pl_x[solo_country]) {
    place_x = -(miss_pl_x[solo_country] - 160);

  } else {
    place_x = -(leveldata.airfield_x[solo_country] - 160);
  }

  load_descriptions(solo_mission + solo_country * 6);

  if (is_there_sound && config.sfx_on) {
    // sb_mix_sample(sample_hurr);
  }

  while (!exit_flag) {
    if (kbhit() && getch() == SDLK_ESCAPE) {
      exit_flag = 1;
    }

    koords(&x, &y, &n1, &n2);
    kangas->blit_fullscreen();
    kangas_terrain_to_screen(place_x);

    if (showing_texts)
      show_descriptions(solo_mission + solo_country * 6);

    menuselect = 0;
    if (x >= 72 && x <= 88 && y >= 191 && y <= 199) {
      // frost->printf(150,188,"Fly mission");
      menuselect = 1;
    }

    if (x >= 54 && x <= 70 && y >= 191 && y <= 199) {
      // frost->printf(150,188,"Back to mission menu");
      menuselect = 2;
    }

    if (x >= 0 && x <= 16 && y >= 191 && y <= 199) {
      // frost->printf(150,188,"Scroll map leftwards");
      menuselect = 3;
    }

    if (x >= 18 && x <= 34 && y >= 191 && y <= 199) {
      // frost->printf(150,188,"Scroll map rightwards");
      menuselect = 4;
    }

    if (x >= 5 && x <= 315 && y >= 8 && y <= 178) {
      // frost->printf(150,188,"Texts on/off");
      menuselect = 5;
    }

    cursor->blit(x - 10, y - 10);
    do_all(1);

    if ((n1 || n2) && menuselect) {
      switch (menuselect) {

      case 1: // fly mission
        fly->blit(72, 191);
        do_all();
        exit_flag = 2;
        break;

      case 2:

        exit->blit(54, 191);
        do_all();
        if (n1)
          random_fade_out();
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
        wtoggle(&showing_texts);
      }
    }

    if (place_x > 0)
      place_x = 0;
    if (place_x < -2079)
      place_x = -2079;
  }

  delete kangas;
  delete buttr;
  delete buttl;
  delete fly;
  delete exit;

  if (is_there_sound && config.sfx_on) {
    if (soundcard_type != SOUNDCARD_GUS) {
      sdl_stop_all_samples(); /* stop hurr sound */
    }
  }

  if ((mission_re_fly == -1) && is_there_sound && config.music_on &&
      !findparameter("-nomusic")) {
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

  Font *grid3;
  Bitmap *credi1;

  grid3 = new Font("G3FONT");
  grid3->scale();

  credi1 = new Bitmap("CREDI1");

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
    grid3->printf(205, 160, "Antti Lehtoranta\nJuha Rytk�nen\nJoonas Joensuu");

    do_all();

    if (n1 || n2) {

      if (n1)
        random_fade_out();
      exit_flag = 1;
      break;
    }
  }

  tyhjaa_vircr();
  do_all();
  if (!findparameter("-debugnographics"))
    init_vga("PALET5");

  delete credi1;
  delete grid3;

  wait_mouse_relase();
}

void letter_menu() {
  int exit_flag = 0;
  int x, y, n1, n2;
  char country_names[4][10] = {"German", "Finnish", "English", "Japanese"};
  char modnames[4][7] = {"mgerma", "mfinla", "mengla", "mjapan"};

  Bitmap *letter;
  Bitmap *temp;
  Bitmap *medal1;

  temp = new Bitmap("MEDAL1");
  medal1 = new Bitmap(1 + 32 * solo_country, 1, 31, 57, temp);
  delete temp;

  if (is_there_sound && config.music_on && !findparameter("-nomusic")) {
    national_mod = sdl_load_mod_file(modnames[solo_country]);
    if (national_mod == nullptr) {
      printf("Error locating music.\n");
      exit(1);
    }
    sdl_play_music(national_mod);
  }

  letter = new Bitmap("LETTER");

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
        random_fade_out();
      exit_flag = 1;
      break;
    }
  }

  tyhjaa_vircr();
  do_all();

  delete letter;
  delete medal1;

  wait_mouse_relase();

  if (is_there_sound && config.music_on && !findparameter("-nomusic")) {
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
  Bitmap *help;

  help = new Bitmap("HELP1");

  if (is_there_sound && config.music_on) {

    sdl_play_music(triplane_mod);
  }

  hiiri_to(254, 120);

  while (!exit_flag) {
    if (kbhit()) {
      ch = getch();
      if (ch == SDLK_F1) {
        wtoggle(&help_on);
      } else {
        if (ch == SDLK_ESCAPE)
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

        main_engine_random_seed = full_seed;
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
          random_fade_out();

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
          clear_level();
          level_loaded = 0;
        }

        if (is_there_sound && config.music_on) {

          sdl_play_music(triplane_mod);
        }

        hiiri_to(254, 120);
        break;

      case 2:
        if (n1)
          random_fade_out();
        exit_flag = 1;
        break;

      case 3:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        roster_menu();

        break;

      case 4:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        aces_menu();

        break;

      case 5:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        controls_menu();

        break;

      case 6:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        options_menu();

        break;

      case 7:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        transfer_menu();

        break;

      case 8:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        assign_menu();
        hiiri_to(254, 120);
        break;

      case 9:
        if (n1)
          random_fade_out();
        wait_mouse_relase();
        credits_menu();

        break;
      }
    }
  }

  delete help;
}

void print_clear_roster(Bitmap *rosteri) {
  rosteri->blit(0, 0);
  grid2->printf(152, 40, "Pilot Info");
}

void print_filled_roster(int number) {
  int ts = 0;

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

  ts += roster[number].multi_mis_flown;
  ts += roster[number].multi_mis_success * 10;
  ts += roster[number].multi_mis_drops * 4;
  ts -= roster[number].multi_mis_dropped;
  ts -= roster[number].multi_mis_shotsf / 128;
  ts += roster[number].multi_mis_shotshit / 60;
  ts -= roster[number].multi_mis_bombs / 12;
  ts += roster[number].multi_mis_bombshit / 6;

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

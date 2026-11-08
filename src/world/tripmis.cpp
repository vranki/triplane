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

/* Solo mission data */

#include "tripmis.h"
#include "../triplane.h"
#include "constants.h"
#include <array>

int number_of_planes[16];
int miss_pl_x[16];
int miss_pl_y[16];
int fighter[16];

namespace {

constexpr int NUMBER_OF_COUNTRIES = 4;
constexpr int NUMBER_OF_MISSIONS = 6;
constexpr int SLOTS = 12;

/* A slot that starts the mission in the air (facing left) at x, y. */
struct Spawn {
  int slot;
  int x;
  int y;
};

struct Mission {
  const char *name;
  std::array<int, SLOTS> sides;   /* player_sides[] */
  std::array<int, SLOTS> planes;  /* number_of_planes[] */
  std::array<int, SLOTS> fighter; /* fighter[] */
  std::array<Spawn, 3> spawns;    /* miss_pl_x[] and miss_pl_y[]; the rest is 0 */
};

/* One row per mission, in mission_names order: six German, six Finnish,
   six English and six Japanese missions. Slot order in each array is
   GERMAN_1, FINLAND_1, ENGLAND_1, JAPAN_1, GERMAN_2, ... JAPAN_3. */
constexpr std::array<Mission, NUMBER_OF_COUNTRIES * NUMBER_OF_MISSIONS> missions = {{
    /* German mission 0 */
    {"Coast Islands",
     {
      ALLIED, NEUTRAL, NEUTRAL, ENEMY, ALLIED, NEUTRAL,
      NEUTRAL, ENEMY, ALLIED, NEUTRAL, NEUTRAL, ENEMY,
     },
     {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      BOMBER, BOMBER, BOMBER, BOMBER, BOMBER, BOMBER,
      BOMBER, BOMBER, BOMBER, BOMBER, BOMBER, BOMBER,
     },
     {{}}},
    /* German mission 1 */
    {"Japanese Counter-Attack",
     {
      ALLIED, NEUTRAL, NEUTRAL, ENEMY, ALLIED, NEUTRAL,
      NEUTRAL, ENEMY, ALLIED, NEUTRAL, NEUTRAL, ENEMY,
     },
     {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
     {
      PLANE, PLANE, PLANE, BOMBER, PLANE, PLANE,
      PLANE, FIGHTER, PLANE, PLANE, PLANE, BOMBER,
     },
     {{{JAPAN_1, 1750, 150}, {JAPAN_3, 2002, 145}}}},
    /* German mission 2 */
    {"English Channel",
     {
      ALLIED, NEUTRAL, ENEMY, NEUTRAL, ALLIED, NEUTRAL,
      ENEMY, NEUTRAL, ALLIED, NEUTRAL, NEUTRAL, NEUTRAL,
     },
     {1, 0, 2, 0, 2, 0, 2, 0, 1, 0, 0, 0},
     {
      FIGHTER, BOMBER, FIGHTER, BOMBER, BOMBER, BOMBER,
      BOMBER, BOMBER, FIGHTER, BOMBER, BOMBER, BOMBER,
     },
     {{}}},
    /* German mission 3 */
    {"Treachery",
     {
      ALLIED, ENEMY, ENEMY, ALLIED, ALLIED, ENEMY,
      ENEMY, ALLIED, ALLIED, ENEMY, ENEMY, ALLIED,
     },
     {3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, BOMBER, BOMBER,
      BOMBER, BOMBER, BOMBER, BOMBER, BOMBER, BOMBER,
     },
     {{}}},
    /* German mission 4 */
    {"Capture",
     {
      ALLIED, ENEMY, ENEMY, ENEMY, ALLIED, ENEMY,
      ENEMY, ENEMY, ALLIED, ENEMY, ENEMY, ENEMY,
     },
     {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, BOMBER, BOMBER, BOMBER, BOMBER, BOMBER,
      BOMBER, BOMBER, BOMBER, BOMBER, BOMBER, BOMBER,
     },
     {{}}},
    /* German mission 5 */
    {"Against Odds",
     {
      ALLIED, ENEMY, ENEMY, ENEMY, ALLIED, ENEMY,
      ENEMY, ENEMY, ALLIED, ENEMY, ENEMY, ENEMY,
     },
     {7, 3, 3, 3, 6, 3, 3, 3, 6, 3, 3, 3},
     {
      PLANE, PLANE, PLANE, PLANE, FIGHTER, FIGHTER,
      FIGHTER, FIGHTER, BOMBER, BOMBER, BOMBER, BOMBER,
     },
     {{}}},
    /* Finnish mission 0 */
    {"Beach-Head",
     {
      ENEMY, ALLIED, NEUTRAL, NEUTRAL, ENEMY, ALLIED,
      NEUTRAL, NEUTRAL, ENEMY, ALLIED, NEUTRAL, NEUTRAL,
     },
     {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Finnish mission 1 */
    {"Complex Wipeout",
     {
      NEUTRAL, ALLIED, ENEMY, NEUTRAL, NEUTRAL, ALLIED,
      ENEMY, NEUTRAL, NEUTRAL, ALLIED, ENEMY, NEUTRAL,
     },
     {0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Finnish mission 2 */
    {"Beach-Head II",
     {
      ENEMY, ALLIED, NEUTRAL, NEUTRAL, ENEMY, ALLIED,
      NEUTRAL, NEUTRAL, ENEMY, ALLIED, NEUTRAL, NEUTRAL,
     },
     {3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Finnish mission 3 */
    {"Troop Escort",
     {
      ENEMY, ALLIED, ALLIED, ENEMY, ENEMY, ALLIED,
      NEUTRAL, NEUTRAL, ENEMY, ALLIED, NEUTRAL, NEUTRAL,
     },
     {0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Finnish mission 4 */
    {"Wood War",
     {
      NEUTRAL, ALLIED, ENEMY, ENEMY, NEUTRAL, ALLIED,
      ENEMY, ENEMY, NEUTRAL, ALLIED, ENEMY, ENEMY,
     },
     {0, 3, 3, 3, 0, 2, 3, 3, 0, 2, 0, 0},
     {
      PLANE, PLANE, BOMBER, FIGHTER, PLANE, PLANE,
      BOMBER, FIGHTER, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Finnish mission 5 */
    {"Showtime",
     {
      ENEMY, ALLIED, ENEMY, ENEMY, ENEMY, ALLIED,
      ENEMY, ENEMY, ENEMY, ALLIED, ENEMY, ENEMY,
     },
     {3, 7, 3, 3, 3, 6, 3, 3, 3, 6, 3, 3},
     {
      FIGHTER, PLANE, FIGHTER, FIGHTER, PLANE, FIGHTER,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* English mission 0 */
    {"Storehunt",
     {
      NEUTRAL, ENEMY, ALLIED, NEUTRAL, NEUTRAL, ENEMY,
      ALLIED, NEUTRAL, NEUTRAL, ENEMY, ALLIED, NEUTRAL,
     },
     {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* English mission 1 */
    {"Patriot Flight",
     {
      NEUTRAL, NEUTRAL, ALLIED, ENEMY, NEUTRAL, NEUTRAL,
      ALLIED, ENEMY, NEUTRAL, NEUTRAL, ALLIED, ENEMY,
     },
     {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, FIGHTER, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{{ENGLAND_1, 2333, 15}}}},
    /* English mission 2 */
    {"Bombing Raid",
     {
      NEUTRAL, NEUTRAL, ALLIED, ENEMY, NEUTRAL, NEUTRAL,
      ALLIED, ENEMY, NEUTRAL, NEUTRAL, ALLIED, ENEMY,
     },
     {0, 0, 3, 5, 0, 0, 2, 5, 0, 0, 2, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* English mission 3 */
    {"Radio Interference",
     {
      ALLIED, NEUTRAL, ALLIED, ENEMY, ALLIED, NEUTRAL,
      ALLIED, ENEMY, ALLIED, NEUTRAL, ALLIED, ENEMY,
     },
     {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, FIGHTER, PLANE, PLANE,
      PLANE, FIGHTER, PLANE, PLANE, PLANE, FIGHTER,
     },
     {{{JAPAN_1, 2300, 10}}}},
    /* English mission 4 */
    {"Channel Tunnel",
     {
      ENEMY, ENEMY, ALLIED, ENEMY, ENEMY, ENEMY,
      ALLIED, ENEMY, ENEMY, ENEMY, ALLIED, ENEMY,
     },
     {1, 1, 2, 1, 0, 0, 1, 0, 0, 0, 1, 0},
     {
      FIGHTER, FIGHTER, PLANE, FIGHTER, PLANE, PLANE,
      FIGHTER, PLANE, PLANE, PLANE, BOMBER, PLANE,
     },
     {{{GERMAN_1, 2300, 10}, {FINLAND_1, 2100, 50}, {JAPAN_1, 2200, 30}}}},
    /* English mission 5 */
    {"Lord of the Lagoon",
     {
      ENEMY, ENEMY, ALLIED, ENEMY, ENEMY, ENEMY,
      ALLIED, ENEMY, ENEMY, ENEMY, ALLIED, ENEMY,
     },
     {3, 3, 7, 3, 3, 3, 6, 3, 3, 3, 6, 3},
     {
      FIGHTER, FIGHTER, FIGHTER, FIGHTER, FIGHTER, FIGHTER,
      FIGHTER, FIGHTER, FIGHTER, FIGHTER, FIGHTER, FIGHTER,
     },
     {{}}},
    /* Japanese mission 0 */
    {"Tactical Bombardment",
     {
      NEUTRAL, ENEMY, NEUTRAL, ALLIED, NEUTRAL, ENEMY,
      NEUTRAL, ALLIED, NEUTRAL, ENEMY, NEUTRAL, ALLIED,
     },
     {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Japanese mission 1 */
    {"Finnish Outpost",
     {
      ENEMY, ENEMY, NEUTRAL, ALLIED, ENEMY, ENEMY,
      NEUTRAL, ALLIED, ENEMY, ENEMY, NEUTRAL, ALLIED,
     },
     {3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 3},
     {
      PLANE, PLANE, PLANE, PLANE, FIGHTER, FIGHTER,
      FIGHTER, FIGHTER, BOMBER, BOMBER, BOMBER, BOMBER,
     },
     {{}}},
    /* Japanese mission 2 */
    {"Ransom",
     {
      ENEMY, NEUTRAL, NEUTRAL, ALLIED, ENEMY, NEUTRAL,
      NEUTRAL, ALLIED, ENEMY, NEUTRAL, NEUTRAL, ALLIED,
     },
     {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Japanese mission 3 */
    {"Harbor Hazard",
     {
      ALLIED, ENEMY, ENEMY, ALLIED, ALLIED, ENEMY,
      ENEMY, ALLIED, ALLIED, ENEMY, ENEMY, ALLIED,
     },
     {5, 1, 5, 5, 5, 1, 5, 5, 0, 1, 0, 0},
     {
      FIGHTER, FIGHTER, BOMBER, PLANE, FIGHTER, FIGHTER,
      FIGHTER, PLANE, FIGHTER, FIGHTER, BOMBER, PLANE,
     },
     {{{FINLAND_1, 2160, 100}, {FINLAND_2, 2200, 130}, {FINLAND_3, 2240, 160}}}},
    /* Japanese mission 4 */
    {"Sweating Sauna",
     {
      ENEMY, ENEMY, ENEMY, ALLIED, ENEMY, ENEMY,
      ENEMY, ALLIED, ENEMY, ENEMY, ENEMY, ALLIED,
     },
     {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
     {
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
      PLANE, PLANE, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
    /* Japanese mission 5 */
    {"Oasis Offence",
     {
      ENEMY, ENEMY, ENEMY, ALLIED, ENEMY, ENEMY,
      ENEMY, ALLIED, ENEMY, ENEMY, ENEMY, ALLIED,
     },
     {3, 3, 3, 6, 3, 3, 3, 6, 3, 3, 3, 6},
     {
      FIGHTER, FIGHTER, FIGHTER, PLANE, PLANE, PLANE,
      PLANE, FIGHTER, PLANE, PLANE, PLANE, PLANE,
     },
     {{}}},
}};

constexpr std::array<const char *, missions.size()> make_names() {
  std::array<const char *, missions.size()> names{};
  for (std::size_t i = 0; i < missions.size(); i++)
    names[i] = missions[i].name;
  return names;
}

} // namespace

constexpr std::array<const char *, 24> mission_names = make_names();

void init_mission(int country, int number) {
  if (country < 0 || country >= NUMBER_OF_COUNTRIES || number < 0 ||
      number >= NUMBER_OF_MISSIONS)
    return;

  const Mission &m = missions[country * NUMBER_OF_MISSIONS + number];

  for (int slot = 0; slot < SLOTS; slot++) {
    player_sides[slot] = m.sides[slot];
    number_of_planes[slot] = m.planes[slot];
    miss_pl_x[slot] = 0;
    miss_pl_y[slot] = 0;
    fighter[slot] = m.fighter[slot];
  }

  for (const Spawn &s : m.spawns) {
    if (s.x || s.y) {
      miss_pl_x[s.slot] = s.x;
      miss_pl_y[s.slot] = s.y;
    }
  }
}

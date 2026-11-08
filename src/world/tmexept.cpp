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

/* Solomission exceptions code */

#include "tmexept.h"
#include "../triplane.h"
#include "../world/constants.h"
#include "../world/plane.h"
#include "tripai.h"
#include "tripmis.h"

/******************************************************************************/

namespace world::tmexept {

namespace {

/* Finnish missions 0 and 2 (Beach-Head, Beach-Head II): infantry of
   country 0 stops advancing once it has passed x = 325. */
void finnish_beachhead_game_exeptions() {
  int l;

  for (l = 0; l < 100; l++) {
    if (!infan_x[l])
      continue;

    if (infan_country[l])
      continue;

    if (infan_x[l] > 325) {
      if (infan_direction[l])
        infan_direction[l] = 0;
    }
  }
}

} // namespace

void init_exeptions(int country, int mission) {
  /* German mission 1 (Japanese Counter-Attack): the two Japanese planes
     that start in the air begin at an angle of 150 degrees. */
  if (country == GERMANY && mission == 1) {
    player_angle[JAPAN_1] = 256 * 150;
    player_angle[JAPAN_3] = 256 * 150;
  }
}

void game_exeptions(int country, int mission) {
  if (country == GERMANY && mission == 2) {
    /* English Channel: once the second German slot has no planes left and
       targets remain, the mission fails as soon as no bomb is in the air. */
    if (number_of_planes[GERMAN_2] < 0 && solo_dest_remaining) {
      int l;

      for (l = 0; l < 100; l++) {
        if (bombs_going[l] != -1)
          break;
      }

      if (l == 100)
        solo_failed = 1;
    }
  } else if (country == FINLAND && (mission == 0 || mission == 2)) {
    finnish_beachhead_game_exeptions();
  }
}

void endgame_exeptions(int /* country */, int /* mission */) {
  /* No mission has end-of-game exceptions. */
}

} // namespace world::tmexept

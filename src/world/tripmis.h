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

/* Solo mission data headerfile */

#ifndef TRIPMIS_H
#define TRIPMIS_H

#include <array>
#include <cstdint>

/* Player slots of a solo mission: slot = country + 4 * group. */
constexpr int32_t GERMAN_1 = 0;
constexpr int32_t FINLAND_1 = 1;
constexpr int32_t ENGLAND_1 = 2;
constexpr int32_t JAPAN_1 = 3;
constexpr int32_t GERMAN_2 = 4;
constexpr int32_t FINLAND_2 = 5;
constexpr int32_t ENGLAND_2 = 6;
constexpr int32_t JAPAN_2 = 7;
constexpr int32_t GERMAN_3 = 8;
constexpr int32_t FINLAND_3 = 9;
constexpr int32_t ENGLAND_3 = 10;
constexpr int32_t JAPAN_3 = 11;

/* Values of player_sides[] in a solo mission. */
constexpr int32_t ALLIED = 0;
constexpr int32_t ENEMY = 1;
constexpr int32_t NEUTRAL = 2;

/* Values of fighter[]: what kind of plane a slot flies. */
constexpr int32_t BOMBER = 0;
constexpr int32_t PLANE = 1;
constexpr int32_t FIGHTER = 2;

/* Planes left per slot (slot 0..11); the mission's own slot counts the
   player's lives. */
extern int number_of_planes[16];
/* Non-zero: the slot starts in the air at this position instead of in the
   hangar. */
extern int miss_pl_x[16];
extern int miss_pl_y[16];
extern int fighter[16];

/* Mission names, indexed by country * 6 + mission. */
extern const std::array<const char *, 24> mission_names;

/* Loads the mission's slot data into the arrays above and player_sides[]. */
void init_mission(int country, int number);

#endif

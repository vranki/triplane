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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

constexpr int32_t ITGUN_SHOT_COLOR = 32;
constexpr int32_t ITGUN_SHOT_SPEED = 8000;
constexpr int32_t ITGUN_SHOT_GRAVITY = 500;
constexpr int32_t MAX_ITGUN_SHOTS = 20;
constexpr int32_t ITGUN_AGE_VARIETY = 10;
constexpr int32_t ITGUN_BASE_AGE = 32;
constexpr int32_t ITGUN_SHOT_RATE = 15;
constexpr int32_t ITEXPLOSION_FRAMES = 9; // 5

constexpr int32_t AA_MG_SHOT_SPEED = 4800;
constexpr int32_t MAX_AA_GUNS = 16;

constexpr int32_t INFANT_SHOTS_SPEED = 4000;
constexpr int32_t INFANTRY_AIM_RANDOM = 40;
constexpr int32_t MAX_INFANTRY = 100;

constexpr int32_t MAX_FLAGS = 12;
constexpr int32_t FLAGS_SPEED = 2;

constexpr int32_t BOMB_GRAVITY = 2000;
constexpr int32_t MIN_BOMB_PARTS = 25; // 25
constexpr int32_t MAX_BOMB_PARTS = 50; // 50
constexpr int32_t MAX_BOMBS = 25;      // 25
constexpr int32_t MAX_STRUCTURES = 100;
constexpr int32_t BOMB_PRIME_TIME = 20;

constexpr int32_t NUMBER_OF_SCENES = 15;

constexpr int32_t MAX_SHOTS = 500;
constexpr int32_t SHOTS_SPEED = 4000;
constexpr int32_t SHOTS_RATE = 3;
constexpr int32_t SHOTS_GRAVITY = 1000; // was: 400
constexpr int32_t SHOTS_COLOR = 8;
constexpr int32_t SHOTS_RANGE = 80; // was: 55
constexpr int32_t SHOT_RANDOMNESS = 30000;

constexpr int32_t MAX_FLYING_OBJECTS = 800;
constexpr int32_t SMOKE_FRAMES = 17;
constexpr int32_t WAVE1_FRAMES = 15;
constexpr int32_t WAVE2_FRAMES = 5;
constexpr int32_t EXPLOX_FRAMES = 6;
constexpr int32_t EXPLOX_RANDOM = 12;
constexpr int32_t EXPLOX_CONST = 10;
constexpr int32_t EXPLOX_VARIETY = 25 * 256;
constexpr int32_t EXPLOX_PHASE_DIFF = 7;
constexpr int32_t SMOKE_SPEED = -3500;
constexpr int32_t NUMBER_OF_BITES = 7;
constexpr int32_t PARTS_SPEED = 270000;
constexpr int32_t FOBJECTS_GRAVITY = 3000;        // 1600
constexpr int32_t NUMBER_OF_EXPLOSION_PARTS = 30; // 50
constexpr int32_t FOBJECTS_DAMAGE = 8;
constexpr int32_t NUMBER_OF_FLAMES = 6;
constexpr int32_t MAX_FLAMES = 20;
constexpr int32_t FLAME_AGE = 125;

constexpr int32_t FOBJECTS_SMOKE = 0;
constexpr int32_t FOBJECTS_PARTS = 1;
constexpr int32_t FOBJECTS_EXPLOX = 2;
constexpr int32_t FOBJECTS_ITEXPLOSION = 3;
constexpr int32_t FOBJECTS_WAVE1 = 4;
constexpr int32_t FOBJECTS_WAVE2 = 5;
constexpr int32_t FOBJECTS_FLAME = 6;
constexpr int32_t FOBJECTS_RIFLE = 7;
constexpr int32_t FOBJECTS_SSMOKE = 8;

constexpr int32_t HIT_ANGLE = 10;
constexpr int32_t TAIL_HIT_ANGLE = 20;
constexpr int32_t PLANE_MODIFICATION = 5;
constexpr int32_t PLANE_AIRFIELD_POSITION = 15;
constexpr int32_t STOP_SPEED_LIMIT = 16;

constexpr int32_t RED_MANOVER = 475;
constexpr int32_t BLUE_MANOVER = 425;
constexpr int32_t GREEN_MANOVER = 450;
constexpr int32_t YELLOW_MANOVER = 500;
constexpr int32_t GREY_MANOVER = 500;
constexpr int32_t BLACK_MANOVER = 500;

constexpr int32_t RED_POWER = 80;
constexpr int32_t BLUE_POWER = 135;
constexpr int32_t GREEN_POWER = 80;
constexpr int32_t YELLOW_POWER = 69;
constexpr int32_t GREY_POWER = 120;
constexpr int32_t BLACK_POWER = 135;

constexpr int32_t SHOT_AT_RANGE = 200;
constexpr int32_t SHOT_AT_ANGLE = 30;
constexpr int32_t AVOID_TERRAIN_FRAMES = 24;
constexpr int32_t AI_GOING_UP_LIMIT = 25 * 256;
constexpr int32_t AI_ANGLE_MARGINAL = 10 * 256;
constexpr int32_t AI_ATTACK_DISTANCE = 200;
constexpr int32_t AIM_GIVEUPFIGHT_DISTANCE = 350;
constexpr int32_t AI_STALL_WARNING = 1400;
constexpr int32_t AI_STALL_WARNING2 = 2200;

constexpr int32_t AIM_NOMISSION = -1;
constexpr int32_t AIM_TAKEOFF = 0;
constexpr int32_t AIM_LAND = 1;
constexpr int32_t AIM_IMMELMAN = 2;
constexpr int32_t AIM_SPLITS = 3;
constexpr int32_t AIM_FLY_LEVEL = 4;
constexpr int32_t AIM_CHASE_PLANE = 5;
constexpr int32_t AIM_BOMB_STRUCTURE = 6;
constexpr int32_t AIM_EVADE_TERRAIN = 7;
constexpr int32_t AIM_GAIN_SPEED = 8;
constexpr int32_t AIM_HEAD_FOR_HOME = 9;

constexpr int32_t GERMANY = 0;
constexpr int32_t FINLAND = 1;
constexpr int32_t ENGLAND = 2;
constexpr int32_t JAPAN = 3;

constexpr int32_t STATUS_SPEED = 24;

#endif

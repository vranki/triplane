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

#define ITGUN_SHOT_COLOR 32
#define ITGUN_SHOT_SPEED 8000
#define ITGUN_SHOT_GRAVITY 500
#define MAX_ITGUN_SHOTS 20
#define ITGUN_AGE_VARIETY 10
#define ITGUN_BASE_AGE 32
#define ITGUN_SHOT_RATE 15
#define ITEXPLOSION_FRAMES 9    // 5

#define AA_MG_SHOT_SPEED 4800
#define MAX_AA_GUNS 16

#define INFANT_SHOTS_SPEED 4000
#define INFANTRY_AIM_RANDOM 40
#define MAX_INFANTRY 100

#define MAX_FLAGS 12
#define FLAGS_SPEED 2

#define BOMB_GRAVITY 2000
#define MIN_BOMB_PARTS 25       //25
#define MAX_BOMB_PARTS 50       //50
#define MAX_BOMBS 25            //25
#define MAX_STRUCTURES 100

#define NUMBER_OF_SCENES 15

#define MAX_SHOTS 500
#define SHOTS_SPEED 4000
#define SHOTS_RATE 3
#define SHOTS_GRAVITY 400
#define SHOTS_COLOR 8
#define SHOTS_RANGE 55

#define MAX_FLYING_OBJECTS 200
#define SMOKE_FRAMES 17
#define WAVE1_FRAMES 15
#define WAVE2_FRAMES 5
#define EXPLOX_FRAMES 6
#define EXPLOX_RANDOM 12
#define EXPLOX_CONST 10
#define EXPLOX_VARIETY 25*256
#define EXPLOX_PHASE_DIFF 7
#define SMOKE_SPEED -3500
#define NUMBER_OF_BITES 7
#define PARTS_SPEED 270000
#define FOBJECTS_GRAVITY 3000   // 1600
#define NUMBER_OF_EXPLOSION_PARTS 30    // 50
#define FOBJECTS_DAMAGE 8
#define NUMBER_OF_FLAMES 6
#define MAX_FLAMES 20
#define FLAME_AGE 125

#define FOBJECTS_SMOKE 0
#define FOBJECTS_PARTS 1
#define FOBJECTS_EXPLOX 2
#define FOBJECTS_ITEXPLOSION 3
#define FOBJECTS_WAVE1 4
#define FOBJECTS_WAVE2 5
#define FOBJECTS_FLAME 6
#define FOBJECTS_RIFLE 7
#define FOBJECTS_SSMOKE 8

#define HIT_ANGLE 10
#define TAIL_HIT_ANGLE 20
#define PLANE_MODIFICATION 5
#define PLANE_AIRFIELD_POSITION 15
#define STOP_SPEED_LIMIT 16

#define RED_MANOVER 475
#define BLUE_MANOVER 425
#define GREEN_MANOVER 450
#define YELLOW_MANOVER 500
#define GREY_MANOVER 500
#define BLACK_MANOVER 500

#define RED_POWER 80
#define BLUE_POWER 135
#define GREEN_POWER 80
#define YELLOW_POWER 69
#define GREY_POWER 120
#define BLACK_POWER 135

#define SHOT_AT_RANGE 200
#define SHOT_AT_ANGLE 30
#define AVOID_TERRAIN_FRAMES 24
#define AI_GOING_UP_LIMIT 25*256
#define AI_ANGLE_MARGINAL 10*256
#define AI_ATTACK_DISTANCE 200
#define AIM_GIVEUPFIGHT_DISTANCE 350
#define AI_STALL_WARNING 1400
#define AI_STALL_WARNING2 2200

#define AIM_NOMISSION -1
#define AIM_TAKEOFF 0
#define AIM_LAND 1
#define AIM_IMMELMAN 2
#define AIM_SPLITS 3
#define AIM_FLY_LEVEL 4
#define AIM_CHASE_PLANE 5
#define AIM_BOMB_STRUCTURE 6
#define AIM_EVADE_TERRAIN 7
#define AIM_GAIN_SPEED 8
#define AIM_HEAD_FOR_HOME 9

#define GERMANY 0
#define FINLAND 1
#define ENGLAND 2
#define JAPAN   3

#define STATUS_SPEED 24

#endif

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

#define NUMBER_OF_COUNTRIES 4
#define NUMBER_OF_MISSIONS 6

#define GERMANY   0
#define FINLAND   1
#define ENGLAND   2
#define JAPAN     3

#define TRUE      1
#define FALSE     0

#define GERMAN_1  0
#define FINLAND_1 1
#define ENGLAND_1 2
#define JAPAN_1   3
#define GERMAN_2  4
#define FINLAND_2 5
#define ENGLAND_2 6
#define JAPAN_2   7
#define GERMAN_3  8
#define FINLAND_3 9
#define ENGLAND_3 10
#define JAPAN_3   11

#define ALLIED  0
#define ENEMY   1
#define NEUTRAL 2

#define BOMBER  0
#define PLANE   1
#define FIGHTER 2

#define LEFT    0
#define RIGHT   1

extern int player_sides[16];

int number_of_planes[16];
int miss_plane_direction[16];
int miss_pl_x[16];
int miss_pl_y[16];
int fighter[16];
char mission_names[24][30] = { "Coast Islands", "Japanese Counter-Attack", "English Channel", "Treachery", "Capture", "Against Odds",
    "Beach-Head", "Complex Wipeout", "Beach-Head II", "Troop Escort", "Wood War", "Showtime",
    "Storehunt", "Patriot Flight", "Bombing Raid", "Radio Interference", "Channel Tunnel", "Lord of the Lagoon",
    "Tactical Bombardment", "Finnish Outpost", "Ransom", "Harbor Hazard", "Sweating Sauna", "Oasis Offence"
};

void init_mission(int coutry, int number);
void init_germany(int number);
void init_finland(int number);
void init_england(int number);
void init_japan(int number);


void init_mission(int country, int number) {
    switch (country) {
    case GERMANY:
        init_germany(number);
        break;

    case FINLAND:
        init_finland(number);
        break;

    case ENGLAND:
        init_england(number);
        break;

    case JAPAN:
        init_japan(number);
        break;


    }



}

void init_germany(int number) {
    switch (number) {
    case 0:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = 0;
        fighter[FINLAND_1] = 0;
        fighter[ENGLAND_1] = 0;
        fighter[JAPAN_1] = 0;
        fighter[GERMAN_2] = 0;
        fighter[FINLAND_2] = 0;
        fighter[ENGLAND_2] = 0;
        fighter[JAPAN_2] = 0;
        fighter[GERMAN_3] = 0;
        fighter[FINLAND_3] = 0;
        fighter[ENGLAND_3] = 0;
        fighter[JAPAN_3] = 0;
        break;
        //Japanese counter attack
    case 1:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 1;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 1;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 1;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = LEFT;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = LEFT;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = LEFT;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 1750;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 2002;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 150;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 145;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = BOMBER;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = BOMBER;
        break;
        //English Canal
    case 2:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = NEUTRAL;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = NEUTRAL;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = NEUTRAL;

        number_of_planes[GERMAN_1] = 1;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 2;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 2;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 2;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 1;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = FIGHTER;
        fighter[FINLAND_1] = 0;
        fighter[ENGLAND_1] = FIGHTER;
        fighter[JAPAN_1] = 0;
        fighter[GERMAN_2] = BOMBER;
        fighter[FINLAND_2] = 0;
        fighter[ENGLAND_2] = BOMBER;
        fighter[JAPAN_2] = 0;
        fighter[GERMAN_3] = FIGHTER;
        fighter[FINLAND_3] = 0;
        fighter[ENGLAND_3] = 0;
        fighter[JAPAN_3] = 0;
        break;
        //Treachery
    case 3:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 3;
        number_of_planes[JAPAN_1] = 3;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = 0;
        fighter[FINLAND_2] = 0;
        fighter[ENGLAND_2] = 0;
        fighter[JAPAN_2] = 0;
        fighter[GERMAN_3] = 0;
        fighter[FINLAND_3] = 0;
        fighter[ENGLAND_3] = 0;
        fighter[JAPAN_3] = 0;
        break;
        //Capture
    case 4:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 5;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = 0;
        fighter[ENGLAND_1] = 0;
        fighter[JAPAN_1] = 0;
        fighter[GERMAN_2] = 0;
        fighter[FINLAND_2] = 0;
        fighter[ENGLAND_2] = 0;
        fighter[JAPAN_2] = 0;
        fighter[GERMAN_3] = 0;
        fighter[FINLAND_3] = 0;
        fighter[ENGLAND_3] = 0;
        fighter[JAPAN_3] = 0;
        break;
        //Against odds
    case 5:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 7;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 3;
        number_of_planes[JAPAN_1] = 3;
        number_of_planes[GERMAN_2] = 6;
        number_of_planes[FINLAND_2] = 3;
        number_of_planes[ENGLAND_2] = 3;
        number_of_planes[JAPAN_2] = 3;
        number_of_planes[GERMAN_3] = 6;
        number_of_planes[FINLAND_3] = 3;
        number_of_planes[ENGLAND_3] = 3;
        number_of_planes[JAPAN_3] = 3;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = FIGHTER;
        fighter[FINLAND_2] = FIGHTER;
        fighter[ENGLAND_2] = FIGHTER;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = BOMBER;
        fighter[FINLAND_3] = BOMBER;
        fighter[ENGLAND_3] = BOMBER;
        fighter[JAPAN_3] = BOMBER;
        break;
    }

}

void init_finland(int number) {
    switch (number) {
        //Beach head
    case 0:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ALLIED;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = NEUTRAL;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ALLIED;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = NEUTRAL;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ALLIED;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = NEUTRAL;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 2;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Complex Wipeout
    case 1:
        player_sides[GERMAN_1] = NEUTRAL;
        player_sides[FINLAND_1] = ALLIED;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = NEUTRAL;
        player_sides[GERMAN_2] = NEUTRAL;
        player_sides[FINLAND_2] = ALLIED;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = NEUTRAL;
        player_sides[GERMAN_3] = NEUTRAL;
        player_sides[FINLAND_3] = ALLIED;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = NEUTRAL;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Beach head 2
    case 2:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ALLIED;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = NEUTRAL;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ALLIED;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = NEUTRAL;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ALLIED;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = NEUTRAL;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 3;
        number_of_planes[FINLAND_2] = 3;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 3;
        number_of_planes[FINLAND_3] = 3;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Troop escort
    case 3:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ALLIED;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ALLIED;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = NEUTRAL;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ALLIED;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = NEUTRAL;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 2;
        number_of_planes[ENGLAND_1] = 2;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Wood war
    case 4:
        player_sides[GERMAN_1] = NEUTRAL;
        player_sides[FINLAND_1] = ALLIED;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = NEUTRAL;
        player_sides[FINLAND_2] = ALLIED;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = NEUTRAL;
        player_sides[FINLAND_3] = ALLIED;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 3;
        number_of_planes[JAPAN_1] = 3;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 2;
        number_of_planes[ENGLAND_2] = 3;
        number_of_planes[JAPAN_2] = 3;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 2;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = BOMBER;
        fighter[JAPAN_1] = FIGHTER;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = BOMBER;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Show time
    case 5:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ALLIED;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ALLIED;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ALLIED;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 7;
        number_of_planes[ENGLAND_1] = 3;
        number_of_planes[JAPAN_1] = 3;
        number_of_planes[GERMAN_2] = 3;
        number_of_planes[FINLAND_2] = 6;
        number_of_planes[ENGLAND_2] = 3;
        number_of_planes[JAPAN_2] = 3;
        number_of_planes[GERMAN_3] = 3;
        number_of_planes[FINLAND_3] = 6;
        number_of_planes[ENGLAND_3] = 3;
        number_of_planes[JAPAN_3] = 3;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = FIGHTER;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = FIGHTER;
        fighter[JAPAN_1] = FIGHTER;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = FIGHTER;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;


    }

}

void init_england(int number) {
    switch (number) {
        //Storehunt
    case 0:
        player_sides[GERMAN_1] = NEUTRAL;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = NEUTRAL;
        player_sides[GERMAN_2] = NEUTRAL;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ALLIED;
        player_sides[JAPAN_2] = NEUTRAL;
        player_sides[GERMAN_3] = NEUTRAL;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ALLIED;
        player_sides[JAPAN_3] = NEUTRAL;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 1;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Patriot Flight
    case 1:
        player_sides[GERMAN_1] = NEUTRAL;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = NEUTRAL;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = ALLIED;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = NEUTRAL;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = ALLIED;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 1;
        number_of_planes[JAPAN_1] = 0;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = LEFT;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 2333;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 15;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = FIGHTER;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Bombing raid
    case 2:
        player_sides[GERMAN_1] = NEUTRAL;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = NEUTRAL;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = ALLIED;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = NEUTRAL;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = ALLIED;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 3;
        number_of_planes[JAPAN_1] = 5;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 2;
        number_of_planes[JAPAN_2] = 5;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 2;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Radio interference
    case 3:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = ALLIED;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = ALLIED;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 1;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 1;
        number_of_planes[JAPAN_1] = 1;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 2300;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 10;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = FIGHTER;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = FIGHTER;
        break;
        //Canal tunnel
    case 4:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ALLIED;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ALLIED;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 1;
        number_of_planes[FINLAND_1] = 1;
        number_of_planes[ENGLAND_1] = 2;
        number_of_planes[JAPAN_1] = 1;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 1;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 1;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 2300;
        miss_pl_x[FINLAND_1] = 2100;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 2200;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 10;
        miss_pl_y[FINLAND_1] = 50;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 30;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = FIGHTER;
        fighter[FINLAND_1] = FIGHTER;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = FIGHTER;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = FIGHTER;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = BOMBER;
        fighter[JAPAN_3] = PLANE;
        break;
        //Carrier command
    case 5:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ALLIED;
        player_sides[JAPAN_1] = ENEMY;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ALLIED;
        player_sides[JAPAN_2] = ENEMY;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ALLIED;
        player_sides[JAPAN_3] = ENEMY;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 7;
        number_of_planes[JAPAN_1] = 3;
        number_of_planes[GERMAN_2] = 3;
        number_of_planes[FINLAND_2] = 3;
        number_of_planes[ENGLAND_2] = 6;
        number_of_planes[JAPAN_2] = 3;
        number_of_planes[GERMAN_3] = 3;
        number_of_planes[FINLAND_3] = 3;
        number_of_planes[ENGLAND_3] = 6;
        number_of_planes[JAPAN_3] = 3;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = FIGHTER;
        fighter[FINLAND_1] = FIGHTER;
        fighter[ENGLAND_1] = FIGHTER;
        fighter[JAPAN_1] = FIGHTER;
        fighter[GERMAN_2] = FIGHTER;
        fighter[FINLAND_2] = FIGHTER;
        fighter[ENGLAND_2] = FIGHTER;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = FIGHTER;
        fighter[FINLAND_3] = FIGHTER;
        fighter[ENGLAND_3] = FIGHTER;
        fighter[JAPAN_3] = FIGHTER;
        break;


    }

}

void init_japan(int number) {
    switch (number) {
        //Tactical bombardment
    case 0:
        player_sides[GERMAN_1] = NEUTRAL;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = NEUTRAL;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = NEUTRAL;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 1;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Finnish outpost
    case 1:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 3;
        number_of_planes[GERMAN_2] = 3;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 3;
        number_of_planes[GERMAN_3] = 3;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 3;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = FIGHTER;
        fighter[FINLAND_2] = FIGHTER;
        fighter[ENGLAND_2] = FIGHTER;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = BOMBER;
        fighter[FINLAND_3] = BOMBER;
        fighter[ENGLAND_3] = BOMBER;
        fighter[JAPAN_3] = BOMBER;
        break;
        //Ransom
    case 2:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = NEUTRAL;
        player_sides[ENGLAND_1] = NEUTRAL;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = NEUTRAL;
        player_sides[ENGLAND_2] = NEUTRAL;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = NEUTRAL;
        player_sides[ENGLAND_3] = NEUTRAL;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 2;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Harbor Hazard
    case 3:
        player_sides[GERMAN_1] = ALLIED;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = ALLIED;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = ALLIED;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 5;
        number_of_planes[FINLAND_1] = 1;
        number_of_planes[ENGLAND_1] = 5;
        number_of_planes[JAPAN_1] = 5;
        number_of_planes[GERMAN_2] = 5;
        number_of_planes[FINLAND_2] = 1;
        number_of_planes[ENGLAND_2] = 5;
        number_of_planes[JAPAN_2] = 5;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 1;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = LEFT;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = LEFT;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = LEFT;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 2160;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 2200;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 2240;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 100;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 130;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 160;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = FIGHTER;
        fighter[FINLAND_1] = FIGHTER;
        fighter[ENGLAND_1] = BOMBER;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = FIGHTER;
        fighter[FINLAND_2] = FIGHTER;
        fighter[ENGLAND_2] = FIGHTER;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = FIGHTER;
        fighter[FINLAND_3] = FIGHTER;
        fighter[ENGLAND_3] = BOMBER;
        fighter[JAPAN_3] = PLANE;
        break;
        //Sweating sauna
    case 4:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 0;
        number_of_planes[FINLAND_1] = 0;
        number_of_planes[ENGLAND_1] = 0;
        number_of_planes[JAPAN_1] = 2;
        number_of_planes[GERMAN_2] = 0;
        number_of_planes[FINLAND_2] = 0;
        number_of_planes[ENGLAND_2] = 0;
        number_of_planes[JAPAN_2] = 0;
        number_of_planes[GERMAN_3] = 0;
        number_of_planes[FINLAND_3] = 0;
        number_of_planes[ENGLAND_3] = 0;
        number_of_planes[JAPAN_3] = 0;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = PLANE;
        fighter[FINLAND_1] = PLANE;
        fighter[ENGLAND_1] = PLANE;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = PLANE;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;
        //Oasis Offence
    case 5:
        player_sides[GERMAN_1] = ENEMY;
        player_sides[FINLAND_1] = ENEMY;
        player_sides[ENGLAND_1] = ENEMY;
        player_sides[JAPAN_1] = ALLIED;
        player_sides[GERMAN_2] = ENEMY;
        player_sides[FINLAND_2] = ENEMY;
        player_sides[ENGLAND_2] = ENEMY;
        player_sides[JAPAN_2] = ALLIED;
        player_sides[GERMAN_3] = ENEMY;
        player_sides[FINLAND_3] = ENEMY;
        player_sides[ENGLAND_3] = ENEMY;
        player_sides[JAPAN_3] = ALLIED;

        number_of_planes[GERMAN_1] = 3;
        number_of_planes[FINLAND_1] = 3;
        number_of_planes[ENGLAND_1] = 3;
        number_of_planes[JAPAN_1] = 6;
        number_of_planes[GERMAN_2] = 3;
        number_of_planes[FINLAND_2] = 3;
        number_of_planes[ENGLAND_2] = 3;
        number_of_planes[JAPAN_2] = 6;
        number_of_planes[GERMAN_3] = 3;
        number_of_planes[FINLAND_3] = 3;
        number_of_planes[ENGLAND_3] = 3;
        number_of_planes[JAPAN_3] = 6;

        miss_plane_direction[GERMAN_1] = 0;
        miss_plane_direction[FINLAND_1] = 0;
        miss_plane_direction[ENGLAND_1] = 0;
        miss_plane_direction[JAPAN_1] = 0;
        miss_plane_direction[GERMAN_2] = 0;
        miss_plane_direction[FINLAND_2] = 0;
        miss_plane_direction[ENGLAND_2] = 0;
        miss_plane_direction[JAPAN_2] = 0;
        miss_plane_direction[GERMAN_3] = 0;
        miss_plane_direction[FINLAND_3] = 0;
        miss_plane_direction[ENGLAND_3] = 0;
        miss_plane_direction[JAPAN_3] = 0;

        miss_pl_x[GERMAN_1] = 0;
        miss_pl_x[FINLAND_1] = 0;
        miss_pl_x[ENGLAND_1] = 0;
        miss_pl_x[JAPAN_1] = 0;
        miss_pl_x[GERMAN_2] = 0;
        miss_pl_x[FINLAND_2] = 0;
        miss_pl_x[ENGLAND_2] = 0;
        miss_pl_x[JAPAN_2] = 0;
        miss_pl_x[GERMAN_3] = 0;
        miss_pl_x[FINLAND_3] = 0;
        miss_pl_x[ENGLAND_3] = 0;
        miss_pl_x[JAPAN_3] = 0;

        miss_pl_y[GERMAN_1] = 0;
        miss_pl_y[FINLAND_1] = 0;
        miss_pl_y[ENGLAND_1] = 0;
        miss_pl_y[JAPAN_1] = 0;
        miss_pl_y[GERMAN_2] = 0;
        miss_pl_y[FINLAND_2] = 0;
        miss_pl_y[ENGLAND_2] = 0;
        miss_pl_y[JAPAN_2] = 0;
        miss_pl_y[GERMAN_3] = 0;
        miss_pl_y[FINLAND_3] = 0;
        miss_pl_y[ENGLAND_3] = 0;
        miss_pl_y[JAPAN_3] = 0;

        fighter[GERMAN_1] = FIGHTER;
        fighter[FINLAND_1] = FIGHTER;
        fighter[ENGLAND_1] = FIGHTER;
        fighter[JAPAN_1] = PLANE;
        fighter[GERMAN_2] = PLANE;
        fighter[FINLAND_2] = PLANE;
        fighter[ENGLAND_2] = PLANE;
        fighter[JAPAN_2] = FIGHTER;
        fighter[GERMAN_3] = PLANE;
        fighter[FINLAND_3] = PLANE;
        fighter[ENGLAND_3] = PLANE;
        fighter[JAPAN_3] = PLANE;
        break;

    }

}

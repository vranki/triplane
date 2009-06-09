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

#include "world/constants.h"
#include "triplane.h"
#include "tripai.h"
#include "world/plane.h"

extern int number_of_planes[16];

void init_exeptions(int country, int mission);
void game_exeptions(int country, int mission);
void endgame_exeptions(int country, int mission);

void german_init_exeptions(int mission);
void finnish_init_exeptions(int mission);
void english_init_exeptions(int mission);
void japanese_init_exeptions(int mission);

void german_game_exeptions(int mission);
void finnish_game_exeptions(int mission);
void english_game_exeptions(int mission);
void japanese_game_exeptions(int mission);

void german_end_exeptions(int mission);
void finnish_end_exeptions(int mission);
void english_end_exeptions(int mission);
void japanese_end_exeptions(int mission);

/******************************************************************************/

void german_init_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        player_angle[3] = 256 * 150;
        player_angle[11] = 256 * 150;
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void finnish_init_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:

        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void english_init_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void japanese_init_exeptions(int mission)
{
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void init_exeptions(int country, int mission) {
    switch (country) {
    case GERMANY:
        german_init_exeptions(mission);
        break;

    case FINLAND:
        finnish_init_exeptions(mission);
        break;

    case ENGLAND:
        english_init_exeptions(mission);
        break;

    case JAPAN:
        japanese_init_exeptions(mission);


    }


}

void german_game_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        if (number_of_planes[4] < 0 && solo_dest_remaining) {
            int l;

            for (l = 0; l < 100; l++) {
                if (bombs_going[l] != -1)
                    break;

            }

            if (l == 100)
                solo_failed = 1;


        }

        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void finnish_game_exeptions(int mission) {
    int l;

    switch (mission) {
    case 0:
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

        break;

    case 1:
        break;

    case 2:
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

        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void english_game_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void japanese_game_exeptions(int mission)
{
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void game_exeptions(int country, int mission) {
    switch (country) {
    case GERMANY:
        german_game_exeptions(mission);
        break;

    case FINLAND:
        finnish_game_exeptions(mission);
        break;

    case ENGLAND:
        english_game_exeptions(mission);
        break;

    case JAPAN:
        japanese_game_exeptions(mission);


    }


}

void german_end_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void finnish_end_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void english_end_exeptions(int mission) {
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void japanese_end_exeptions(int mission)
{
    switch (mission) {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    }

}

void endgame_exeptions(int country, int mission) {
    switch (country) {
    case GERMANY:
        german_end_exeptions(mission);
        break;

    case FINLAND:
        finnish_end_exeptions(mission);
        break;

    case ENGLAND:
        english_end_exeptions(mission);
        break;

    case JAPAN:
        japanese_end_exeptions(mission);


    }


}

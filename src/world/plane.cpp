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

#include "world/plane.h"
#include "triplane.h"
#include "util/wutil.h"

#define PLANE_TURN_SPEED 5
#define SPEED 4

/* Variables */

unsigned char controls_up[16], controls_down[16], controls_power[16], controls_power2[16];
                /* Note: 1== enable, 0== disable */
int player_x[16], player_y[16];
int player_speed[16];
int player_angle[16];
int player_ammo[16], player_bombs[16], player_gas[16];
int player_upsidedown[16];
int player_rolling[16];
int player_spinning[16];
int spinning_remaining[16];
int player_x_speed[16], player_y_speed[16];
int player_last_shot[16];
int player_endurance[16];
int player_points[16];

int player_x_8[16];
int player_y_8[16];

/* Ammo varies from 0-128 (One big is 16 small)
   Bombs varie from 0-6 
   Gas varies from 0-2048 (One line is 256 units) */

int plane_power[16] = { RED_POWER, BLUE_POWER, GREEN_POWER, YELLOW_POWER,
    RED_POWER, BLUE_POWER, GREEN_POWER, YELLOW_POWER,
    RED_POWER, BLUE_POWER, GREEN_POWER, YELLOW_POWER,
    GREY_POWER, GREY_POWER, GREY_POWER, BLACK_POWER
};

int plane_manover[16] = { RED_MANOVER * PLANE_TURN_SPEED, BLUE_MANOVER * PLANE_TURN_SPEED, GREEN_MANOVER * PLANE_TURN_SPEED, YELLOW_MANOVER * PLANE_TURN_SPEED,
    RED_MANOVER * PLANE_TURN_SPEED, BLUE_MANOVER * PLANE_TURN_SPEED, GREEN_MANOVER * PLANE_TURN_SPEED, YELLOW_MANOVER * PLANE_TURN_SPEED,
    RED_MANOVER * PLANE_TURN_SPEED, BLUE_MANOVER * PLANE_TURN_SPEED, GREEN_MANOVER * PLANE_TURN_SPEED, YELLOW_MANOVER * PLANE_TURN_SPEED,
    GREY_MANOVER * PLANE_TURN_SPEED, GREY_MANOVER * PLANE_TURN_SPEED, GREY_MANOVER * PLANE_TURN_SPEED, BLACK_MANOVER * PLANE_TURN_SPEED
};

int plane_mass[16] = { 200, 275, 200, 180,
    200, 275, 200, 180,
    200, 275, 200, 180,
    200, 200, 200, 180
};

char plane_name[16][15] = { "Germany", "Finland", "England", "Japan",
    "Germany", "Finland", "England", "Japan",
    "Germany", "Finland", "England", "Japan",
    "Neutral", "Neutral", "Neutral", "Black"
};

int plane_bombs[16] = { 4, 3, 5, 6,
    4, 3, 5, 6,
    4, 3, 5, 6,
    4, 4, 4, 6
};

int plane_gas[16] = { 1600, 1800, 2047, 1400,
    1600, 1800, 2047, 1400,
    1600, 1800, 2047, 1400,
    1800, 1800, 1800, 2047
};

int plane_ammo[16] = { 96, 96, 127, 80,
    96, 96, 127, 80,
    96, 96, 127, 80,
    96, 96, 96, 127
};


int t_plane_power[16] = { RED_POWER, BLUE_POWER, GREEN_POWER, YELLOW_POWER,
    RED_POWER, BLUE_POWER, GREEN_POWER, YELLOW_POWER,
    RED_POWER, BLUE_POWER, GREEN_POWER, YELLOW_POWER,
    GREY_POWER, GREY_POWER, GREY_POWER, BLACK_POWER
};

int t_plane_manover[16] =
    { RED_MANOVER * PLANE_TURN_SPEED, BLUE_MANOVER * PLANE_TURN_SPEED, GREEN_MANOVER * PLANE_TURN_SPEED, YELLOW_MANOVER * PLANE_TURN_SPEED,
    RED_MANOVER * PLANE_TURN_SPEED, BLUE_MANOVER * PLANE_TURN_SPEED, GREEN_MANOVER * PLANE_TURN_SPEED, YELLOW_MANOVER * PLANE_TURN_SPEED,
    RED_MANOVER * PLANE_TURN_SPEED, BLUE_MANOVER * PLANE_TURN_SPEED, GREEN_MANOVER * PLANE_TURN_SPEED, YELLOW_MANOVER * PLANE_TURN_SPEED,
    GREY_MANOVER * PLANE_TURN_SPEED, GREY_MANOVER * PLANE_TURN_SPEED, GREY_MANOVER * PLANE_TURN_SPEED, BLACK_MANOVER * PLANE_TURN_SPEED
};

int t_plane_mass[16] = { 200, 275, 200, 180,
    200, 275, 200, 180,
    200, 275, 200, 180,
    200, 200, 200, 180
};


int t_plane_bombs[16] = { 4, 3, 5, 6,
    4, 3, 5, 6,
    4, 3, 5, 6,
    4, 4, 4, 6
};

int t_plane_gas[16] = { 1600, 1800, 2047, 1400,
    1600, 1800, 2047, 1400,
    1600, 1800, 2047, 1400,
    1800, 1800, 1800, 2047
};

int t_plane_ammo[16] = { 96, 96, 127, 80,
    96, 96, 127, 80,
    96, 96, 127, 80,
    96, 96, 96, 127
};


int player_was_on_airfield[16];

void model_planes(void) {
    int c;
    int mass;
    int initial_turn;
    int oldup;
    int yyy, xxx;
    int temp;

    for (c = 0; c < 16; c++) {
        if (!player_exists[c])
            continue;

        if (player_upsidedown[c]) {
            oldup = controls_up[c];
            controls_up[c] = controls_down[c];
            controls_down[c] = oldup;
        }

        if (player_spinning[c] || spinning_remaining[c]) {
            controls_up[c] = 0;
            controls_down[c] = 0;
        }

        mass = plane_mass[c];
        mass += player_ammo[c] / 10;
        if (plane_bombs[c] != 6)
            mass += player_bombs[c] * 12;
        else
            mass += player_bombs[c] * 10;


        mass += player_gas[c] / 160;

        if (player_y[c] < 0 && !player_spinning[c]) {
            spinning_remaining[c] = wrandom(80) + 30;
        }

        if (controls_power[c]) {
            if (player_speed[c] < (1000 + mass)) {
                player_y[c] += (1000 - player_speed[c] + mass) >> 3;

            }
        } else {
            if (player_speed[c] < (1500 + mass)) {
                player_y[c] += (1350 - player_speed[c] + mass) >> 3;

            }
        }

        initial_turn = (plane_manover[c] << 8) / (mass + 200);

        if (player_speed[c] < 768 || player_spinning[c] || spinning_remaining[c]) {
            if ((!player_on_airfield[c]) && (!player_was_on_airfield[c])) {
                if (player_y[c] < 0)
                    player_y[c] += 768;

                initial_turn = 0;
                if (player_angle[c] < (90 << 8) && player_angle[c] >= 0)
                    player_angle[c] -= 1024;
                else if (player_angle[c] > (90 << 8) && player_angle[c] < (270 << 8))
                    player_angle[c] += 1024;
                else if (player_angle[c] > (270 << 8))
                    player_angle[c] -= 1024;
                player_y[c] += 256;
            } else {
                player_on_airfield[c] = player_was_on_airfield[c];
                if (player_speed[c]) {

                    if (player_angle[c] <= (TAIL_HIT_ANGLE << 8) || player_angle[c] >= ((360 - HIT_ANGLE) << 8)) {
                        player_angle[c] += (768 - player_speed[c]) / 2;
                        if (player_angle[c] > (TAIL_HIT_ANGLE << 8))
                            player_angle[c] = TAIL_HIT_ANGLE << 8;
                    }

                    if (player_angle[c] <= ((180 - TAIL_HIT_ANGLE) << 8) && player_angle[c] >= ((180 + HIT_ANGLE) << 8)) {
                        player_angle[c] -= (768 - player_speed[c]) / 2;
                        if (player_angle[c] < ((180 + 6 - TAIL_HIT_ANGLE) << 8))
                            player_angle[c] = (180 + 6 - TAIL_HIT_ANGLE) << 8;
                    }
                }
                player_y[c] += 512;
            }
        } else {
            initial_turn /= 1 + (((player_speed[c] - 768) / 20) >> 8);

        }


        if (player_angle[c] >= (360 << 8))
            player_angle[c] -= 360 << 8;
        if (player_angle[c] < 0)
            player_angle[c] += 360 << 8;

        if (controls_up[c] || controls_down[c]) {

            if (player_speed[c] > 0) {
                player_speed[c] -= initial_turn / 100;

            }
            if (player_speed[c] < 0) {
                player_speed[c] += initial_turn / 100;
            }


        }

        if (player_speed[c] < 512) {
            controls_up[c] = 0;
            controls_down[c] = 0;

        }

        if (player_on_airfield[c] && !(controls_power[c] && player_gas[c]))
            if (player_speed[c] < STOP_SPEED_LIMIT && player_speed[c] > -STOP_SPEED_LIMIT)
                player_speed[c] = 0;

        if (!player_on_airfield[c])
            if (player_angle[c] > (180 << 8)) {
                player_speed[c] += ((90 << 8) - abs(player_angle[c] - (270 << 8))) * mass / 32000;

            }
        if (!player_on_airfield[c])
            if (player_angle[c] < (180 << 8) && player_angle[c] > 0)
                player_speed[c] -= ((90 << 8) - abs(player_angle[c] - (90 << 8))) * mass / 32000;

        if (controls_power[c] && player_gas[c]) {
            player_speed[c] += (plane_power[c] << 8) / mass;
            player_gas[c]--;
        }

        player_speed[c] -= player_speed[c] / 50;

        if (player_speed[c]) {
            if (controls_up[c])
                player_angle[c] += initial_turn;
            if (controls_down[c])
                player_angle[c] -= initial_turn;
        }

        if (player_angle[c] >= (360 << 8))
            player_angle[c] -= 360 << 8;
        if (player_angle[c] < 0)
            player_angle[c] += 360 << 8;

        if (player_on_airfield[c] && !player_speed[c]) {
            if (((player_angle[c] >> 8) <= TAIL_HIT_ANGLE) || ((player_angle[c] >> 8) >= (360 - HIT_ANGLE)))
                player_angle[c] = TAIL_HIT_ANGLE << 8;
            else
                player_angle[c] = (180 + 6 - TAIL_HIT_ANGLE) << 8;
        }

        if (player_on_airfield[c] && player_speed[c] < 512) {
            if (((player_angle[c] >> 8) <= TAIL_HIT_ANGLE) || ((player_angle[c] >> 8) >= (360 - HIT_ANGLE)))
                temp = (TAIL_HIT_ANGLE << 8) - (player_speed[c] * 10);
            else
                temp = ((180 + 6 - TAIL_HIT_ANGLE) << 8) + (player_speed[c] * 10);

            if (temp > player_angle[c]) {
                if ((temp - player_angle[c]) < 256)
                    player_angle[c] = temp;
                else
                    player_angle[c] += 256;

            }

            if (temp < player_angle[c]) {
                if ((player_angle[c] - temp) < 256)
                    player_angle[c] = temp;
                else
                    player_angle[c] -= 256;

            }

        }

        player_x_speed[c] = (cosinit[player_angle[c] >> 8] * player_speed[c]) / SPEED;
        player_y_speed[c] = (sinit[player_angle[c] >> 8] * player_speed[c]) / SPEED;
        player_x[c] += player_x_speed[c] >> 8;
        if (!(player_on_airfield[c] && player_speed[c] < 768))
            player_y[c] -= player_y_speed[c] >> 8;

        if (player_on_airfield[c]) {
            if (!player_upsidedown[c]) {
                yyy = 5;
                xxx = 5;

            } else {
                yyy = -5;
                xxx = 5;
            }

            plane_tire_y = (player_y[c]) + (((-xxx * sinit[player_angle[c] >> 8] + yyy * cosinit[player_angle[c] >> 8] + 128) >> 8) << 8) + 256;

            if ((plane_tire_y) > (leveldata.airfield_y[player_on_airfield[c] - 1] << 8))
                player_y[c] -= (plane_tire_y) - ((leveldata.airfield_y[player_on_airfield[c] - 1]) << 8);
        }

        player_x_8[c] = player_x[c] >> 8;
        player_y_8[c] = player_y[c] >> 8;

        if ((!playing_solo) && config.unlimited_ammo) {
            if (player_ammo[c] < 1)
                player_ammo[c] = 1;
        }

        if ((!playing_solo) && config.unlimited_gas) {
            if (player_gas[c] < 1)
                player_gas[c] = 1;
        }

    }


}

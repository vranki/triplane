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

#include "world/constants.h"
#include "world/plane.h"
#include "triplane.h"
#include "fobjects.h"
#include "util/wutil.h"
#include "io/sound.h"
#include "world/tripaudio.h"

#define SPEED 4


int infan_x[MAX_INFANTRY];
int infan_y[MAX_INFANTRY];
int infan_direction[MAX_INFANTRY];
int infan_last_shot[MAX_INFANTRY];
int infan_state[MAX_INFANTRY];
int infan_country[MAX_INFANTRY];
int infan_frame[MAX_INFANTRY];
int infan_stop[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

int infan_x_speed[MAX_INFANTRY];

int itgun_shot_x[MAX_ITGUN_SHOTS];
int itgun_shot_y[MAX_ITGUN_SHOTS];
int itgun_shot_x_speed[MAX_ITGUN_SHOTS];
int itgun_shot_y_speed[MAX_ITGUN_SHOTS];
int itgun_shot_age[MAX_ITGUN_SHOTS];

int bomb_target;

void do_infan(void);

void infan_to_plane(int l);
void infan_to_infan(int l);
void infan_to_struct(int l);
void infan_take_hits(int l);

void do_kkbase(void);
void start_it_shot(int x, int y, int angle);
void do_it_shots(void);
void start_itgun_explosion(int number);

void do_ai(int number);
void ai_turn_down(int number);
void ai_turn_up(int number);
void ai_evade_terrain(int number);
void ai_turnplus(int number);
void ai_turnminus(int number);
void ai_do_bombing(int number);
int check_structs(int x, int y, int number);
void do_mekan(void);
void do_doors(void);

#define MEKAN_DIRECTION_LEFT 0
#define MEKAN_DIRECTION_RIGHT 1

#define MEKAN_MISSION_IDLE 0
#define MEKAN_MISSION_RETURN 1
#define MEKAN_MISSION_PUSH_OUT 2
#define MEKAN_MISSION_PUSH_IN 3

/* Submissions of MEKAN_MISSION_PUSH_IN */
#define MEKAN_PUSH_IN_MOVE_TO_PLANE 0
#define MEKAN_PUSH_IN_PUSH 1
#define MEKAN_PUSH_IN_WAIT_DOOR_CLOSE 2

/* Submissions of MEKAN_MISSION_PUSH_OUT */
#define MEKAN_PUSH_OUT_WAIT_DOOR_OPEN 0
#define MEKAN_PUSH_OUT_MOVE_TO_PLANE 1
#define MEKAN_PUSH_OUT_PUSH 2

/* These only affect graphics. */
#define MEKAN_ANIMATION_INVISIBLE 0
#define MEKAN_ANIMATION_WALK 1
#define MEKAN_ANIMATION_PUSH_PROPELLER 2
#define MEKAN_ANIMATION_PUSH_TAIL 3


/******************************************************************************/

void do_doors(void) {
    int l;

    for (l = 0; l < 4; l++) {
        if (!hangar_x[l])
            continue;

        if (hangar_door_opening[l] && hangar_door_closing[l])
            hangar_door_closing[l] = 0;

        if (hangar_door_opening[l]) {
            if (++hangar_door_frame[l] >= 13) {
                hangar_door_frame[l] = 12;
                hangar_door_opening[l] = 0;

            }

        }

        if (hangar_door_closing[l]) {
            if (--hangar_door_frame[l] < 0) {
                hangar_door_frame[l] = 0;
                hangar_door_closing[l] = 0;

            }

        }

    }

}

void do_mekan(void) {
    int l, l2;


    for (l = 0; l < 4; l++) {
        if (!hangar_x[l])
            continue;

        for (l2 = 0; l2 < 16; l2++) {
            if (plane_wants_in[l2]) {
                if (player_speed[l2] || (in_closing[l2])) {
                    plane_wants_in[l2] = 0;
                }

            }

        }

        switch (mekan_mission[l]) {
        case MEKAN_MISSION_RETURN:
            if (mekan_direction[l] == MEKAN_DIRECTION_RIGHT) {
                if (mekan_x[l] >= (hangar_x[l] + 38)) {
                    mekan_mission[l] = MEKAN_MISSION_IDLE;
                    mekan_status[l] = MEKAN_ANIMATION_INVISIBLE;
                    hangar_door_closing[l] = 1;
                }

            } else {
                if (mekan_x[l] <= (hangar_x[l] + 38)) {
                    mekan_mission[l] = MEKAN_MISSION_IDLE;
                    mekan_status[l] = MEKAN_ANIMATION_INVISIBLE;
                    hangar_door_closing[l] = 1;
                }


            }
            break;
        case MEKAN_MISSION_IDLE:
            for (l2 = l + 8; l2 >= 0; l2 -= 4) {
                if (plane_wants_in[l2]) {
                    mekan_mission[l] = MEKAN_MISSION_PUSH_IN;
                    mekan_target[l] = l2;
                    mekan_subtarget[l] = MEKAN_PUSH_IN_MOVE_TO_PLANE;
                    mekan_x[l] = hangar_x[l] + 38;
                    mekan_status[l] = MEKAN_ANIMATION_WALK;
                    if ((player_x_8[l2] - 38) > hangar_x[l]) {
                        mekan_direction[l] = MEKAN_DIRECTION_RIGHT;

                    } else {
                        mekan_direction[l] = MEKAN_DIRECTION_LEFT;

                    }
                }

            }

            for (l2 = l + 8; l2 >= 0; l2 -= 4) {
                if (plane_wants_out[l2]) {
                    mekan_mission[l] = MEKAN_MISSION_PUSH_OUT;
                    mekan_target[l] = l2;
                    mekan_subtarget[l] = MEKAN_PUSH_OUT_WAIT_DOOR_OPEN;
                    mekan_x[l] = hangar_x[l] + 38;
                    mekan_status[l] = MEKAN_ANIMATION_WALK;
                    if (leveldata.plane_direction[player_tsides[l2]]) {
                        mekan_direction[l] = MEKAN_DIRECTION_RIGHT;

                    } else {
                        mekan_direction[l] = MEKAN_DIRECTION_LEFT;

                    }
                }

            }

            if (mekan_mission[l] == MEKAN_MISSION_IDLE)
                hangar_door_closing[l] = 1;

            break;

        case MEKAN_MISSION_PUSH_IN:

            if (!mekan_subtarget[l] && hangar_door_frame[l] != 12) {
                hangar_door_opening[l] = 1;

            }

            if (!plane_wants_in[mekan_target[l]]) {
                mekan_mission[l] = MEKAN_MISSION_RETURN;

                mekan_status[l] = MEKAN_ANIMATION_WALK;

                if (mekan_x[l] > (hangar_x[l] + 38)) {
                    mekan_direction[l] = MEKAN_DIRECTION_LEFT;

                } else {
                    mekan_direction[l] = MEKAN_DIRECTION_RIGHT;

                }
                break;
            }



            switch (mekan_subtarget[l]) {
            case MEKAN_PUSH_IN_MOVE_TO_PLANE:
                if (mekan_direction[l] == MEKAN_DIRECTION_RIGHT && mekan_x[l] >= (player_x_8[mekan_target[l]] + 10)) {
                    mekan_x[l] = player_x_8[mekan_target[l]] + 10;
                    mekan_direction[l] = MEKAN_DIRECTION_LEFT;


                    if (player_angle[mekan_target[l]] < 90 * 256) {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_PROPELLER;

                    } else {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_TAIL;

                    }

                    mekan_subtarget[l] = MEKAN_PUSH_IN_PUSH;

                } else if (mekan_direction[l] == MEKAN_DIRECTION_LEFT && (mekan_x[l] + 13) <= (player_x_8[mekan_target[l]] - 10)) {
                    mekan_x[l] = player_x_8[mekan_target[l]] - 10 - 13;
                    mekan_direction[l] = MEKAN_DIRECTION_RIGHT;


                    if (player_angle[mekan_target[l]] < 90 * 256) {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_TAIL;

                    } else {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_PROPELLER;

                    }

                    mekan_subtarget[l] = MEKAN_PUSH_IN_PUSH;

                }

                break;
            case MEKAN_PUSH_IN_PUSH:

                if (mekan_direction[l] == MEKAN_DIRECTION_RIGHT) {
                    player_x_8[mekan_target[l]] = (mekan_x[l] + 10 + 13);
                    player_x[mekan_target[l]] = player_x_8[mekan_target[l]] << 8;

                    if ((mekan_x[l] + 13) >= (hangar_x[l] + 28)) {

                        hangar_door_closing[l] = 1;
                        plane_present[mekan_target[l]] = 0;
                        plane_coming[mekan_target[l]] = 1;
                        mekan_subtarget[l] = MEKAN_PUSH_IN_WAIT_DOOR_CLOSE;
                        mekan_status[l] = MEKAN_ANIMATION_INVISIBLE;
                    }
                } else {
                    player_x_8[mekan_target[l]] = (mekan_x[l] - 10);
                    player_x[mekan_target[l]] = player_x_8[mekan_target[l]] << 8;

                    if (mekan_x[l] <= (hangar_x[l] + 48)) {

                        hangar_door_closing[l] = 1;
                        plane_present[mekan_target[l]] = 0;
                        plane_coming[mekan_target[l]] = 1;
                        mekan_subtarget[l] = MEKAN_PUSH_IN_WAIT_DOOR_CLOSE;
                        mekan_status[l] = MEKAN_ANIMATION_INVISIBLE;
                    }



                }

                break;

            case MEKAN_PUSH_IN_WAIT_DOOR_CLOSE:
                if (!hangar_door_frame[l]) {
                    mekan_mission[l] = MEKAN_MISSION_IDLE;
                    mekan_status[l] = MEKAN_ANIMATION_INVISIBLE;
                    plane_wants_in[mekan_target[l]] = 0;
                    init_player(mekan_target[l]);


                }
                break;
            }

            break;

        case MEKAN_MISSION_PUSH_OUT:

            if (!mekan_subtarget[l] && hangar_door_frame[l] != 12) {
                hangar_door_opening[l] = 1;

            }

            if (mekan_subtarget[l] && (player_speed[mekan_target[l]] || !plane_present[mekan_target[l]])) {

                plane_wants_out[mekan_target[l]] = 0;

            }

            if (!plane_wants_out[mekan_target[l]]) {
                mekan_mission[l] = MEKAN_MISSION_RETURN;

                mekan_status[l] = MEKAN_ANIMATION_WALK;

                if (mekan_x[l] > (hangar_x[l] + 38)) {
                    mekan_direction[l] = MEKAN_DIRECTION_LEFT;

                } else {
                    mekan_direction[l] = MEKAN_DIRECTION_RIGHT;

                }

                break;
            }


            switch (mekan_subtarget[l]) {
            case MEKAN_PUSH_OUT_WAIT_DOOR_OPEN:
                if (hangar_door_frame[l] == 12) {
                    plane_present[mekan_target[l]] = 1;
                    plane_coming[mekan_target[l]] = 0;
                    mekan_subtarget[l] = MEKAN_PUSH_OUT_MOVE_TO_PLANE;
                }

                break;
            case MEKAN_PUSH_OUT_MOVE_TO_PLANE:

                if (mekan_direction[l] == MEKAN_DIRECTION_RIGHT && mekan_x[l] >= (player_x_8[mekan_target[l]] + 10)) {
                    mekan_x[l] = player_x_8[mekan_target[l]] + 10;
                    mekan_direction[l] = MEKAN_DIRECTION_LEFT;


                    if (player_angle[mekan_target[l]] < 90 * 256) {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_PROPELLER;

                    } else {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_TAIL;

                    }

                    mekan_subtarget[l] = MEKAN_PUSH_OUT_PUSH;

                } else if (mekan_direction[l] == MEKAN_DIRECTION_LEFT && (mekan_x[l] + 13) <= (player_x_8[mekan_target[l]] - 10)) {
                    mekan_x[l] = player_x_8[mekan_target[l]] - 10 - 13;
                    mekan_direction[l] = MEKAN_DIRECTION_RIGHT;


                    if (player_angle[mekan_target[l]] < 90 * 256) {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_TAIL;

                    } else {
                        mekan_status[l] = MEKAN_ANIMATION_PUSH_PROPELLER;

                    }

                    mekan_subtarget[l] = MEKAN_PUSH_OUT_PUSH;

                }
                break;

            case MEKAN_PUSH_OUT_PUSH:

                if (mekan_direction[l] == MEKAN_DIRECTION_RIGHT) {
                    player_x_8[mekan_target[l]] = (mekan_x[l] + 10 + 13);
                    player_x[mekan_target[l]] = player_x_8[mekan_target[l]] << 8;

                    if ((mekan_x[l] + 13) >= (leveldata.airfield_x[l] + leveldata.airfield_lenght[l] - PLANE_AIRFIELD_POSITION - 5)) {

                        plane_wants_out[mekan_target[l]] = 0;
                        mekan_mission[l] = MEKAN_MISSION_RETURN;

                        mekan_status[l] = MEKAN_ANIMATION_WALK;

                        if (mekan_x[l] > (hangar_x[l] + 38)) {
                            mekan_direction[l] = MEKAN_DIRECTION_LEFT;

                        } else {
                            mekan_direction[l] = MEKAN_DIRECTION_RIGHT;

                        }



                    }
                } else {
                    player_x_8[mekan_target[l]] = (mekan_x[l] - 10);
                    player_x[mekan_target[l]] = player_x_8[mekan_target[l]] << 8;

                    if (mekan_x[l] <= (leveldata.airfield_x[l] + PLANE_AIRFIELD_POSITION + 5)) {
                        plane_wants_out[mekan_target[l]] = 0;
                        mekan_mission[l] = MEKAN_MISSION_RETURN;

                        mekan_status[l] = MEKAN_ANIMATION_WALK;

                        if (mekan_x[l] > (hangar_x[l] + 38)) {
                            mekan_direction[l] = MEKAN_DIRECTION_LEFT;

                        } else {
                            mekan_direction[l] = MEKAN_DIRECTION_RIGHT;

                        }





                    }



                }


                break;


            }

            break;



        }


        if (hangar_door_frame[l] == 12 && !hangar_door_closing[l]) {
            if (mekan_status[l] != MEKAN_ANIMATION_PUSH_TAIL) {
                if (++mekan_frame[l] >= 14) {
                    mekan_frame[l] = 0;

                }
            } else {
                if (++mekan_frame[l] >= 9) {
                    mekan_frame[l] = 0;

                }
            }

            if (mekan_direction[l] == MEKAN_DIRECTION_RIGHT) {
                mekan_x[l] += 2;

            } else {
                mekan_x[l] -= 2;

            }
        }

        /*if(!hangar_door_opening[l] && !hangar_door_closing[l])
           {
           if(hangar_door_frame[l])
           hangar_door_closing[l]=1;
           else
           hangar_door_opening[l]=1;

           } */

    }

    do_doors();
}

int check_structs(int x, int y, int number) {
    int l;

    for (l = 0; l < MAX_AA_GUNS; l++)
        if (kkbase_x[l] && player_sides[kkbase_country[l]] == number && (kkbase_status[l] != 2) && (bombs_going[kkbase_number[l]] == -1)) {
            if (x >= kkbase_x[l] && x <= (kkbase_x[l] + 25) && y >= (kkbase_y[l] - 5) && y <= (kkbase_y[l] + 30)) {
                bomb_target = kkbase_number[l];

                return 1;
            }
        }

    for (l = 0; l < MAX_STRUCTURES; l++)
        if (leveldata.struct_owner[l] != 4)
            if ((structures[l][1] != NULL) && (!struct_state[l]) && (player_sides[leveldata.struct_owner[l]] == number) && (bombs_going[l] == -1)) {
                if (x >= leveldata.struct_x[l] && x <= (leveldata.struct_x[l] + struct_width[l]) &&
                    y >= (leveldata.struct_y[l] - 5) && y <= (leveldata.struct_y[l] + 5 + struct_heigth[l])) {
                    bomb_target = l;
                    return 1;

                }
            }

    return 0;

}

int check_multi_structs(int x, int y, int number) {
    int l;

    for (l = 0; l < MAX_AA_GUNS; l++)
        if (kkbase_x[l] && kkbase_country[l] != 4 && player_sides[kkbase_country[l]] != player_sides[number] && (kkbase_status[l] != 2)
            && (bombs_going[kkbase_number[l]] == -1)) {
            if (x >= kkbase_x[l] && x <= (kkbase_x[l] + 25) && y >= (kkbase_y[l] - 5) && y <= (kkbase_y[l] + 30)) {
                bomb_target = kkbase_number[l];

                return 1;
            }
        }

    for (l = 0; l < MAX_STRUCTURES; l++)
        if (leveldata.struct_owner[l] != 4)
            if ((structures[l][1] != NULL) && (!struct_state[l]) && (player_sides[leveldata.struct_owner[l]] != player_sides[number]) && (bombs_going[l] == -1)) {
                if (x >= leveldata.struct_x[l] && x <= (leveldata.struct_x[l] + struct_width[l]) &&
                    y >= (leveldata.struct_y[l] - 5) && y <= (leveldata.struct_y[l] + 5 + struct_heigth[l])) {
                    bomb_target = l;
                    return 1;

                }
            }

    return 0;

}


void ai_do_bombing(int number) {
    int fake_y_speed;
    int fake_x_speed;
    int fake_x;
    int fake_y;
    int side;

    if (player_sides[number] == 1)
        side = 0;
    else
        side = 1;

    //bomb_speed[l]=player_speed[player];
    //bomb_angle[l]=player_angle[player];
    fake_x_speed = (cosinit[player_angle[number] >> 8] * player_speed[number]) >> 2;
    fake_y_speed = (sinit[player_angle[number] >> 8] * player_speed[number]) >> 2;
    fake_x = player_x[number] + (9 - player_upsidedown[number] * 18) * sinit[player_angle[number] >> 8];
    fake_y = player_y[number] + (9 - player_upsidedown[number] * 18) * cosinit[player_angle[number] >> 8];

    while (1) {

        fake_y_speed -= BOMB_GRAVITY;
        fake_x += fake_x_speed >> 9;
        fake_y -= fake_y_speed >> 9;

        if ((fake_x < 0) | ((fake_x >> 8) >= 2400)) {
            return;
        }

        if (fake_y >= 0)
            if (terrain_level[fake_x >> 8] < (fake_y >> 8) || ((fake_y >> 8) >= 200)) {
                if (fake_y >= 614400)
                    return;

                if (!playing_solo) {
                    if (check_multi_structs(fake_x >> 8, fake_y >> 8, number)) {
                        player_bombs[number]--;
                        drop_bomb(number, bomb_target);
                    }
                } else if (check_structs(fake_x >> 8, fake_y >> 8, side)) {
                    player_bombs[number]--;
                    drop_bomb(number, bomb_target);
                }

                return;
            }

    }



}

void start_itgun_explosion(int number) {
    int l;
    int distance;

    itgun_sound(itgun_shot_x[number] >> 8);

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {
        fobjects[l].x = itgun_shot_x[number];
        fobjects[l].y = itgun_shot_y[number];
        fobjects[l].x_speed = 0;
        fobjects[l].y_speed = 0;
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 23;
        fobjects[l].height = 14;
        fobjects[l].type = FOBJECTS_ITEXPLOSION;
        fobjects[l].phase = 0;
    }

    for (l = 0; l < 16; l++) {
        if (!plane_present[l])
            continue;

        calculate_difference(itgun_shot_x[number] >> 8, itgun_shot_y[number] >> 8, player_x_8[l], player_y_8[l], &distance);

        if (distance <= 10)
            cause_damage(80, l);

        if (distance >= 40)
            continue;

        cause_damage(40 - distance, l);

    }

    itgun_shot_x[number] = 0;

}

void start_it_shot(int x, int y, int angle) {
    int l;

    for (l = 0; l < MAX_ITGUN_SHOTS; l++)
        if (!itgun_shot_x[l])
            break;

    if (l != MAX_ITGUN_SHOTS) {
        itgun_shot_age[l] = wrandom(ITGUN_AGE_VARIETY) + ITGUN_BASE_AGE;
        itgun_shot_x[l] = (x << 8);
        itgun_shot_y[l] = (y << 8);
        itgun_shot_x_speed[l] = (cosinit[angle] * ITGUN_SHOT_SPEED) >> 2;
        itgun_shot_y_speed[l] = (sinit[angle] * ITGUN_SHOT_SPEED) >> 2;
        itgun_shot_x[l] += itgun_shot_x_speed[l] >> 6;
        itgun_shot_y[l] -= itgun_shot_y_speed[l] >> 6;
    }


}

void do_it_shots(void) {
    int l, l2;

    for (l = 0; l < MAX_ITGUN_SHOTS; l++) {
        if (itgun_shot_x[l]) {

            itgun_shot_y_speed[l] -= ITGUN_SHOT_GRAVITY;
            itgun_shot_x[l] += itgun_shot_x_speed[l] >> 9;
            itgun_shot_y[l] -= itgun_shot_y_speed[l] >> 9;
            if ((itgun_shot_x[l] < 0) || ((itgun_shot_x[l] >> 8) >= NUMBER_OF_SCENES * 160) || (itgun_shot_y[l] >> 8) >= 200 || itgun_shot_y[l] < 0)
                itgun_shot_x[l] = 0;
            else if ((level_bitmap[(itgun_shot_x[l] >> 8) + (itgun_shot_y[l] >> 8) * 2400]) < 112
                     || ((level_bitmap[(itgun_shot_x[l] >> 8) + (itgun_shot_y[l] >> 8) * 2400])) > 119)
                itgun_shot_x[l] = 0;

            if (!(itgun_shot_age[l]--))
                start_itgun_explosion(l);

            for (l2 = 0; l2 < 16; l2++) {
                if (plane_present[l2])
                    if (((player_x[l2] + 2304) > itgun_shot_x[l]) &&
                        ((player_x[l2] - 2304) < itgun_shot_x[l]) && ((player_y[l2] + 2304) > itgun_shot_y[l]) && ((player_y[l2] - 2304) < itgun_shot_y[l]))
                        if (plane_p[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]][player_upsidedown[l2]]
                            [(itgun_shot_x[l] >> 8) - (player_x_8[l2]) + 10 + ((itgun_shot_y[l] >> 8) - (player_y_8[l2]) + 10) * 20] != 255) {
                            start_itgun_explosion(l);
                            break;
                        }
            }
        }

    }
}

void infan_to_plane(int l) {
    int l2;
    int angle;
    int distance;

    for (l2 = 0; l2 < 16; l2++) {
        if (!plane_present[l2])
            continue;

        if (l2 == infan_country[l])
            continue;

        if (player_sides[infan_country[l]] == player_sides[l2])
            continue;

        if (abs(player_x_8[l2] - infan_x[l]) > 150)
            continue;


        calculate_difference(infan_x[l] + 7, infan_y[l] + 7, ((player_x[l2] + (player_x_speed[l2] >> 6)) >> 8),
                             ((player_y[l2] - (player_y_speed[l2] >> 7)) >> 8), &distance, &angle);


        if ((player_on_airfield[infan_country[l]] || ((abs(player_x_8[l2] - player_x_8[infan_country[l]]) > 150) || !plane_present[infan_country[l]]))
            && angle <= 180) {
            infan_state[l] = 1;
            if (++infan_last_shot[l] > 6) {
                infan_last_shot[l] = 0;
                infan_state[l] = 2;

                angle += wrandom(INFANTRY_AIM_RANDOM) - (INFANTRY_AIM_RANDOM >> 1);

                if (angle >= 360)
                    angle -= 360;
                if (angle < 0)
                    angle += 360;

                start_shot(infan_x[l] + 7, infan_y[l] + 7, angle, INFANT_SHOTS_SPEED, infan_country[l]);


            }

            if (angle >= 270 || angle <= 90) {
                if (angle >= 270)
                    infan_frame[l] = 0;
                else
                    infan_frame[l] = angle >> 4;

                infan_direction[l] = 1;
            } else {
                if (angle > 180)
                    infan_frame[l] = 0;
                else
                    infan_frame[l] = (180 - angle) >> 4;

                infan_direction[l] = 0;
            }

        }



        break;
    }
}

void infan_to_infan(int l) {
    int l2;
    int angle;
    int distance;

    for (l2 = 0; l2 < MAX_INFANTRY; l2++) {
        if (!infan_x[l2])
            continue;

        if (infan_country[l2] == infan_country[l])
            continue;

        if (player_sides[infan_country[l]] == player_sides[infan_country[l2]])
            continue;

        if (abs(infan_x[l2] - infan_x[l]) > 50)
            continue;

        calculate_difference(infan_x[l] + 7, infan_y[l] + 7, infan_x[l2] + 7, infan_y[l2] + 7, &distance, &angle);



        if (angle <= 180) {
            infan_state[l] = 1;
            if (++infan_last_shot[l] > 6) {
                infan_last_shot[l] = 0;
                infan_state[l] = 2;

                angle += wrandom(INFANTRY_AIM_RANDOM) - (INFANTRY_AIM_RANDOM >> 1);

                if (angle >= 360)
                    angle -= 360;
                if (angle < 0)
                    angle += 360;

                start_shot(infan_x[l] + 7, infan_y[l] + 7, angle, INFANT_SHOTS_SPEED, infan_country[l]);


            }

            if (angle >= 270 || angle <= 90) {
                if (angle >= 270)
                    infan_frame[l] = 0;
                else
                    infan_frame[l] = angle >> 4;

                infan_direction[l] = 1;
            } else {
                if (angle > 180)
                    infan_frame[l] = 0;
                else
                    infan_frame[l] = (180 - angle) >> 4;

                infan_direction[l] = 0;
            }

        }


        break;
    }


}


void infan_to_struct(int l) {
    int l2;
    ///
    for (l2 = 0; l2 < MAX_AA_GUNS; l2++)
        if (kkbase_x[l2] && kkbase_status[l2] != 2 && (leveldata.struct_owner[kkbase_number[l2]] != 4)
            && (player_sides[leveldata.struct_owner[kkbase_number[l2]]] != player_sides[infan_country[l]])) {
            if (infan_x[l] + 15 >= kkbase_x[l2] && infan_x[l] <= (kkbase_x[l2] + 26))
            {

                if (config.flames)
                    start_flame(kkbase_x[l2], kkbase_y[l2] + 21, 26);

                if (playing_solo) {
                    if (leveldata.struct_type[kkbase_number[l2]] == 1)
                        solo_dest_remaining--;

                    if (leveldata.struct_type[kkbase_number[l2]] == 2)
                        solo_failed = 1;

                }

                kkbase_status[l2] = 2;
                kkbase_frame[l2] = 0;
                struct_state[kkbase_number[l2]] = 1;


            }
        }

    for (l2 = 0; l2 < MAX_STRUCTURES; l2++)
        if ((structures[l2][1] != NULL) && (leveldata.struct_owner[l2] != 4) && (player_sides[leveldata.struct_owner[l2]] != player_sides[infan_country[l]])
            && (!struct_state[l2])) {
            if (infan_x[l] + 15 >= leveldata.struct_x[l2] && infan_x[l] <= (leveldata.struct_x[l2] + struct_width[l2])) {
                if (config.flames)
                    start_flame(leveldata.struct_x[l2], leveldata.struct_y[l2] + struct_heigth[l2], struct_width[l2]);

                if (playing_solo) {
                    if (leveldata.struct_type[l2] == 1)
                        solo_dest_remaining--;

                    if (leveldata.struct_type[l2] == 2)
                        solo_failed = 1;

                }

                struct_state[l2] = 1;

                if (current_mode == SVGA_MODE) {

                    structures[l2][1]->blit_to_bitmap(standard_background, leveldata.struct_x[l2] - (leveldata.struct_x[l2] / 800) * 800,
                                                      leveldata.struct_y[l2] + (leveldata.struct_x[l2] / 800) * 196 - 4);

                    structures[l2][1]->blit(leveldata.struct_x[l2] - (leveldata.struct_x[l2] / 800) * 800,
                                            leveldata.struct_y[l2] + (leveldata.struct_x[l2] / 800) * 196 - 4);

                    if ((leveldata.struct_x[l2] - (leveldata.struct_x[l2] / 800) * 800) + struct_width[l2] > 800)
                        structures[l2][1]->blit(leveldata.struct_x[l2] - (leveldata.struct_x[l2] / 800) * 800 - 800,
                                                leveldata.struct_y[l2] + (leveldata.struct_x[l2] / 800 + 1) * 196 - 4);
                }

                if (leveldata.struct_hit[l2]) {
                    structures[l2][1]->blit_to_bitmap(maisema, leveldata.struct_x[l2], leveldata.struct_y[l2]);
                }



            }
        }

}


void infan_take_hits(int l) {
    int l2;
    int soundi;

    for (l2 = wrandom(3); l2 < MAX_SHOTS; l2 += 3) {
        if (!shots_flying_x[l2])
            continue;

        if ((shots_flying_x[l2] >> 8) > infan_x[l] && (shots_flying_x[l2] >> 8) < (infan_x[l] + 14) &&
            (shots_flying_y[l2] >> 8) > infan_y[l] && (shots_flying_y[l2] >> 8) < (infan_x[l] + 13) && shots_flying_infan[l2] != infan_country[l]) {
            //if(wrandom(3))
            //  continue;   

            if (playing_solo) {
                if (leveldata.struct_type[l] == 1)
                    solo_dest_remaining--;

                if (leveldata.struct_type[l] == 2)
                    solo_failed = 1;

            }

            if (shots_flying_x_speed[l2] > 0) {
                if (infan_direction[l])
                    infan_state[l] = 5;
                else
                    infan_state[l] = 3;
            } else {
                if (!infan_direction[l])
                    infan_state[l] = 5;
                else
                    infan_state[l] = 3;
            }

            start_rifle(infan_x[l], infan_y[l]);

            if (config.infantry_sounds && config.sound_on && config.sfx_on) {
                soundi = wrandom(18);
                if (soundi < 9)
                    play_2d_sample(sample_die[soundi], player_x_8[solo_country], infan_x[l]);

            }


            infan_frame[l] = 0;
            shots_flying_x[l2] = 0;
            break;
        }

    }

}


void do_infan(void) {
    int l, l2;
    int angle;
    int distance;
    int diff;

    for (l = 0; l < MAX_INFANTRY; l++) {
        if (!infan_x[l])
            continue;

        if (infan_state[l] == 1 || infan_state[l] == 2) {
            infan_direction[l] = wrandom(2);
            infan_state[l] = 0;
        }

        if (!infan_state[l] && infan_frame[l] < 12)
            infan_to_plane(l);

        if (!infan_state[l] && infan_frame[l] < 12)
            infan_to_infan(l);

        if (!infan_state[l])
            infan_to_struct(l);

        if (infan_state[l] == 5 || infan_state[l] == 4) {
            if (++infan_frame[l] >= 20) {
                infan_x[l] = 0;

            } else {
                if (infan_state[l] == 4)
                    infan_x[l] += infan_x_speed[l];

            }

            angle = terrain_level[infan_x[l] + 11] - 13;
            distance = terrain_level[infan_x[l] + 3] - 13;

            angle = ((angle < distance) ? angle : distance) - infan_y[l];

            if (angle > 2)
                angle = 2;

            if (angle < -2)
                angle = -2;

            infan_y[l] += angle;

            continue;
        }

        if (infan_state[l] == 3) {
            if (++infan_frame[l] >= 14) {
                infan_x[l] = 0;

            }

            continue;
        }

        infan_take_hits(l);


        if (infan_state[l])
            continue;


        for (l2 = 0; l2 < 8; l2++) {
            if ((infan_x[l] + (infan_direction[l] ? 1 : -1)) == infan_stop[l2]) {
                wtoggle(&infan_direction[l]);
                break;
            }
        }


        if (l2 == 8
            && ((terrain_level[infan_x[l] + (infan_direction[l] ? 14 : 0)] - 2) >
                terrain_level[infan_x[l] + (infan_direction[l] ? 14 : 0) + (infan_direction[l] ? 1 : -1)])) {
            wtoggle(&infan_direction[l]);

        } else {
            infan_x[l] += (infan_direction[l] ? 1 : -1);
            if (infan_x[l] >= 2400)
                infan_x[l] = 0;

        }

        angle = terrain_level[infan_x[l] + 11] - 13;
        distance = terrain_level[infan_x[l] + 3] - 13;

        diff = ((angle < distance) ? angle : distance) - infan_y[l];

        if (diff > 1) {
            infan_y[l] += 2;
            infan_frame[l] = 12;
        } else {
            infan_y[l] += diff;

            if (!(++infan_frame[l] == 13)) {

                if (infan_frame[l] >= 12)
                    infan_frame[l] = 0;

            }

        }

        //infan_y[l]=(angle<distance) ? angle : distance;


    }
}

void do_kkbase(void) {
    int l, l2;
    int angle, distance, tdistance;
    int tempframe;

    for (l = 0; l < MAX_AA_GUNS; l++) {
        if (!kkbase_x[l])
            continue;

        if (kkbase_status[l] == 1)
            kkbase_status[l] = 0;

        if (kkbase_status[l] == 2 && kkbase_frame[l] < 9) {
            kkbase_frame[l]++;
        }

        if (!kkbase_status[l]) {
            for (l2 = 0; l2 < 16; l2++) {
                if (kkbase_country[l] == l2)
                    continue;

                if (!plane_present[l2])
                    continue;

                if (player_sides[kkbase_country[l]] == player_sides[l2])
                    continue;

                if (abs(kkbase_x[l] - player_x_8[l2]) < 150) {
                    calculate_difference(kkbase_x[l] + 13, kkbase_y[l] + 10, player_x_8[l2], player_y_8[l2], &tdistance, &angle);
                    calculate_difference(kkbase_x[l] + 13, kkbase_y[l] + 10, ((player_x[l2] + ((player_x_speed[l2] >> 8) * (tdistance << 8) / 1700)) >> 8),
                                         ((player_y[l2] + ((player_y_speed[l2] >> 8) * (tdistance << 8) / 1700)) >> 8), &distance, &angle);

                    if (angle >= 45 && angle <= 135) {
                        tempframe = (135 - angle) / 13;

                        if (abs(tempframe - kkbase_frame[l]) > 1) {
                            if (tempframe > kkbase_frame[l])
                                kkbase_frame[l]++;
                            else
                                kkbase_frame[l]--;

                            break;
                        } else
                            kkbase_frame[l] = tempframe;

                        if (++kkbase_last_shot[l] > (kkbase_type[l] ? ITGUN_SHOT_RATE : 7)) {
                            if (++kkbase_shot_number[l] > 6) {
                                kkbase_last_shot[l] = 0;
                                kkbase_shot_number[l] = 0;
                            } else {
                                if (kkbase_shot_number[l] == 1)
                                    kkbase_sound(kkbase_type[l], kkbase_x[l]);

                                if (kkbase_type[l]) {
                                    if (kkbase_shot_number[l] == 1) {
                                        kkbase_status[l] = 1;
                                        start_it_shot(kkbase_x[l] + 13, kkbase_y[l] + 10, angle);
                                    } else
                                        kkbase_status[l] = 0;


                                } else {

                                    if (kkbase_shot_number[l] & 1)
                                        kkbase_status[l] = 1;
                                    else
                                        kkbase_status[l] = 0;

                                    start_shot(kkbase_x[l] + 13, kkbase_y[l] + 10, angle, AA_MG_SHOT_SPEED);
                                }
                            }

                        }

                        break;
                    }
                }
            }


        } else {

        }



    }

}

void ai_turn_down(int number) {
    int flaggy;

    if (going_left)
        flaggy = 1;
    else
        flaggy = 0;

    if (!player_upsidedown[number])
        wtoggle(&flaggy);

    controls_down[number] = flaggy;
    controls_up[number] = (1 ^ flaggy) & 1;

}

void ai_turn_up(int number) {
    int flaggy;

    if (going_left)
        flaggy = 1;
    else
        flaggy = 0;

    if (!player_upsidedown[number])
        wtoggle(&flaggy);

    controls_down[number] = (1 ^ flaggy) & 1;
    controls_up[number] = flaggy;
}

void ai_evade_terrain(int number) {
    int x_kohta, y_kohta;

    x_kohta = ((((player_x_speed[number] * AVOID_TERRAIN_FRAMES) >> 8) + player_x[number]) >> 8);
    y_kohta = ((player_y[number] - ((player_y_speed[number] * AVOID_TERRAIN_FRAMES) >> 8)) >> 8) + PLANE_MODIFICATION * 2;
    if (x_kohta >= 2400)
        x_kohta = 2399;
    if (y_kohta >= 200)
        y_kohta = 199;
    if (x_kohta < 0)
        x_kohta = 0;
    if (y_kohta < 0)
        y_kohta = 0;


    if (wide_terrain_level[x_kohta] < y_kohta && !player_on_airfield[number]) {
        if (going_up > AI_GOING_UP_LIMIT - AI_ANGLE_MARGINAL)
            ai_turn_down(number);

        if (going_up < AI_GOING_UP_LIMIT)
            ai_turn_up(number);

        if (current_mission[number] != AIM_EVADE_TERRAIN) {
            current_mission[number] = AIM_EVADE_TERRAIN;
            mission_phase[number] = 0;

        }

    }


}

void ai_turnplus(int number) {
    int flaggy = 0;

    if (player_upsidedown[number])
        flaggy = 1;

    controls_up[number] = flaggy ? 0 : 1;
    controls_down[number] = flaggy;
}

void ai_turnminus(int number) {
    int flaggy = 0;

    if (player_upsidedown[number])
        flaggy = 1;

    controls_up[number] = flaggy;
    controls_down[number] = flaggy ? 0 : 1;
}


void do_ai(int number) {
    int xdistance, ydistance;
    int distance_enemy[16], angle_enemy[16], angle_difference[16];
    int flaggy;
    int trueangle;
    int l;

    if (!player_exists[number])
        return;

    for (flaggy = 0; flaggy < 16; flaggy++) {
        if (flaggy == number)
            continue;

        if (!player_exists[flaggy])
            continue;

        //calculate_difference(player_x[number]>>8,player_y_8[number]>>8,(player_x[flaggy]>>8),(player_y[flaggy]>>8),&distance_enemy[flaggy],&angle_enemy[flaggy]);     
        calculate_difference(player_x_8[number], player_y_8[number], ((player_x[flaggy] + (player_x_speed[flaggy] >> 7)) >> 8),
                             ((player_y[flaggy] - (player_y_speed[flaggy] >> 7)) >> 8), &distance_enemy[flaggy], &angle_enemy[flaggy]);

        angle_difference[flaggy] = abs((player_angle[number] >> 8) - angle_enemy[flaggy]);
        if (angle_difference[flaggy] > 180) {
            angle_difference[flaggy] = abs(angle_difference[flaggy] - 360);
        }
    }


    if ((player_angle[number] <= 69120) && (player_angle[number] > 23040))
        going_left = 1;
    else
        going_left = 0;

    if ((player_angle[number] < 46080)) {
        going_up = player_angle[number];
        if (going_up > 23040)
            going_up = 46080 - going_up;
    } else
        going_up = 0;

    controls_down[number] = 0;
    controls_up[number] = 0;

    if (player_angle[number] >= 57600 && player_angle[number] <= 80640)
        controls_power[number] = 0;
    else
        controls_power[number] = 1;


    switch (current_mission[number]) {
    case AIM_TAKEOFF:
        if ((player_y_8[number]) + 40 > terrain_level[player_x_8[number]])
            mission_phase[number] = 0;
        else
            current_mission[number] = AIM_NOMISSION;

        ai_evade_terrain(number);
        break;

    case AIM_LAND:
        switch (mission_phase[number]) {
        case 0:
            if ((player_upsidedown[number] && (!going_left)) || (!player_upsidedown[number] && (going_left)))
                if (!player_rolling[number])
                    player_rolling[number] = 1;




            if (going_left) {
                xdistance = -((leveldata.airfield_x[player_tsides[number]] + (leveldata.airfield_lenght[player_tsides[number]] >> 1)) - player_x_8[number]);

            } else {
                xdistance = ((leveldata.airfield_x[player_tsides[number]] + (leveldata.airfield_lenght[player_tsides[number]] >> 1)) - player_x_8[number]);

            }

            ydistance = leveldata.airfield_y[player_tsides[number]] - player_y_8[number];

            if (xdistance < -9) {
                current_mission[number] = AIM_TAKEOFF;
                mission_phase[number] = 0;


            }

            int initial_turn;
            int mass;
            int turn_temp;
            mass = plane_mass[number];
            mass += player_ammo[number] / 10;
            if (plane_bombs[number] != 6)
                mass += player_bombs[number] * 12;
            else
                mass += player_bombs[number] * 10;

            mass += player_gas[number] / 160;

            initial_turn = (plane_manover[number] << 8) / (mass + 200);
            turn_temp = 1 + (((player_speed[number] - 768) / 20) >> 8);
            if (!turn_temp)
                turn_temp = 1;
            initial_turn /= turn_temp;

            if (going_left) {
                if (player_angle[number] >= ((180 + HIT_ANGLE) << 8)) {
                    controls_up[number] = 1;
                    controls_down[number] = 0;

                }
            } else {
                if ((player_angle[number] <= ((360 - HIT_ANGLE) << 8)) && player_angle[number] > 50000) {
                    controls_up[number] = 1;
                    controls_down[number] = 0;

                }
            }



            if (ydistance > (xdistance / 3)) {
                if (going_left) {
                    if (player_angle[number] < ((180 + HIT_ANGLE) << 8) - initial_turn) {
                        controls_up[number] = 0;
                        controls_down[number] = 1;

                    }
                } else {
                    if ((player_angle[number] > ((360 - HIT_ANGLE) << 8) + initial_turn) || player_angle[number] < 23041) {
                        controls_up[number] = 0;
                        controls_down[number] = 1;

                    }

                }

                if (player_speed[number] < 850) {
                    controls_power[number] = 1;

                } else {
                    controls_power[number] = 0;
                    if (!controls_up[number] && !controls_down[number]) {
                        controls_up[number] = 1;
                        controls_down[number] = 1;


                    }

                }



            } else {
                if (going_left) {
                    if (player_angle[number] > ((180 - TAIL_HIT_ANGLE) << 8)) {
                        controls_up[number] = 1;
                        controls_down[number] = 0;

                    }
                } else {
                    if (player_angle[number] < ((TAIL_HIT_ANGLE) << 8) || player_angle[number] > 69119) {
                        controls_up[number] = 1;
                        controls_down[number] = 0;

                    }

                }

                if (player_speed[number] < 2048) {
                    controls_power[number] = 1;

                } else {
                    controls_power[number] = 0;

                }




            }


            if (player_on_airfield[number]) {
                mission_phase[number] = 1;

            }

            break;

        case 1:

            controls_up[number] = 1;
            controls_down[number] = 1;
            controls_power[number] = 0;

            break;

        }



        break;


    case AIM_IMMELMAN:
        switch (mission_phase[number]) {
        case 0:
            mission_phase[number] = 1;
            controls_power[number] = 1;
            controls_up[number] = 0;
            controls_down[number] = 1;
            mission_target[number] = going_left ? 0 : 1;
            break;

        case 1:
            if (player_speed[number] > 2700 && (player_y_8[number]) > 100) {
                mission_phase[number] = 2;

            }
            if (player_speed[number] < 4000)
                controls_power[number] = 1;
            else
                controls_power[number] = 0;

            controls_up[number] = 0;
            controls_down[number] = 0;

            if (going_left && (player_angle[number] < (190 * 256)))
                controls_down[number] = 1;
            else if ((!going_left) && (player_angle[number] > (350 * 256) || player_angle[number] < (90 * 256)))
                controls_down[number] = 1;
            break;

        case 2:
            if (mission_target[number] == 0) {
                if (!going_left && player_angle[number] > 69120) {
                    current_mission[number] = AIM_NOMISSION;
                    break;
                }
            } else {
                if (going_left && player_angle[number] > 46080) {
                    current_mission[number] = AIM_NOMISSION;
                    break;
                }
            }

            controls_power[number] = 1;
            controls_up[number] = 1;
            controls_down[number] = 0;

            break;


        }


        break;

    case AIM_SPLITS:
        switch (mission_phase[number]) {
        case 0:
            if ((player_upsidedown[number] && (going_left)) || (!player_upsidedown[number] && (!going_left)))
                if (!player_rolling[number])
                    player_rolling[number] = 1;
            mission_phase[number] = 1;
            controls_power[number] = 0;
            mission_target[number] = going_left ? 0 : 1;
            ai_turn_up(number);
            break;

        case 1:
            if (!player_rolling[number] && player_speed[number] < 1000) {
                mission_phase[number] = 2;
                controls_up[number] = 1;
                controls_down[number] = 0;
            }

            if (going_up) {
                controls_up[number] = 1;
                controls_down[number] = 1;
            } else {
                ai_turn_up(number);

            }

            controls_power[number] = 0;
            break;

        case 2:
            if (mission_target[number] == 0) {
                if (!going_left && player_angle[number] > 0) {
                    mission_phase[number] = 3;
                    break;
                }
            } else {
                if (going_left && player_angle[number] < 46080) {
                    mission_phase[number] = 3;
                    break;
                }
            }
            controls_up[number] = 1;
            controls_down[number] = 0;
            controls_power[number] = 0;

            break;

        case 3:
            if (going_up < 750)
                ai_turn_up(number);
            else
                current_mission[number] = AIM_NOMISSION;
            break;

        }

        break;

    case AIM_FLY_LEVEL:

        break;




    case AIM_BOMB_STRUCTURE:

        break;

    case AIM_CHASE_PLANE:
        if ((!player_ammo[number]) || distance_enemy[mission_target[number]] > AIM_GIVEUPFIGHT_DISTANCE || !plane_present[mission_target[number]]) {
            current_mission[number] = AIM_NOMISSION;

        } else {
            distances[number] = distance_enemy[mission_target[number]];
            angles[number] = angle_enemy[mission_target[number]];

            trueangle = (player_angle[number] >> 8) - angle_enemy[mission_target[number]];
            if (abs(trueangle) > 180) {
                if (trueangle < 0) {
                    trueangle += 360;
                } else {
                    trueangle -= 360;

                }
            }


            if (trueangle > 0) {

                ai_turnminus(number);
            }
            if (trueangle < 0) {

                ai_turnplus(number);
            }
        }
        /// nysv„yslause

        if (controls_up[number] && player_speed[number] < (1800)) {

            if (going_up && (player_angle[number] > 32000 || player_angle[number] < 14080)) {
                ai_turn_down(number);

            }


        } else if (controls_up[number] && player_speed[number] < (2350 + (player_bombs[number] << 6))) {

            if (!going_up) {
                controls_up[number] = 0;
                controls_down[number] = 0;

            }

        }

    case AIM_NOMISSION:
        if (current_mission[number] != AIM_CHASE_PLANE)
            for (l = 0; l < 16; l++) {
                if (l == number)
                    continue;

                if (!plane_present[l])
                    continue;

                if (player_sides[number] == player_sides[l])
                    continue;

                if (player_ammo[number] && distance_enemy[l] < AI_ATTACK_DISTANCE) {
                    current_mission[number] = AIM_CHASE_PLANE;
                    mission_target[number] = l;


                }
            }

        if ((player_x_8[number] < (120 + ((player_speed[number]) / 40)) && going_left) ||
            (player_x_8[number] > (2280 - ((player_speed[number]) / 40)) && !going_left)) {
            if ((wide_terrain_level[player_x_8[number]] - (player_y_8[number])) > ((player_y_8[number]) + 15))
                current_mission[number] = AIM_SPLITS;
            else
                current_mission[number] = AIM_IMMELMAN;
            mission_phase[number] = 0;

        } else if ((player_upsidedown[number] && (!going_left)) || (!player_upsidedown[number] && (going_left)))
            if (!player_rolling[number])
                player_rolling[number] = 1;

        ai_evade_terrain(number);

        break;

    case AIM_EVADE_TERRAIN:

        if ((player_x[number] < (80 + (player_speed[number] >> 5)) * 256 && going_left) ||
            (player_x[number] > (2320 - (player_speed[number] >> 5)) * 256 && !going_left)) {
            if ((wide_terrain_level[player_x_8[number]] - (player_y_8[number])) > ((player_y_8[number]) + 15))
                current_mission[number] = AIM_SPLITS;
            else
                current_mission[number] = AIM_IMMELMAN;
            mission_phase[number] = 0;

            break;
        } else if ((player_upsidedown[number] && (!going_left)) || (!player_upsidedown[number] && (going_left)))
            if (!player_rolling[number])
                player_rolling[number] = 1;

        ai_evade_terrain(number);

        mission_phase[number]++;
        if (mission_phase[number] >= 24)
            current_mission[number] = AIM_NOMISSION;
        break;


    }


    if ((player_on_airfield[number] - 1) == player_tsides[number]
        && !player_speed[number] && (!player_gas[number] || current_mission[number] == AIM_LAND)) {
        plane_wants_in[number] = 1;

    }

    if ((!player_bombs[number] && !player_ammo[number]) || player_gas[number] < 500 || current_mission[number] == AIM_HEAD_FOR_HOME) {

        if (leveldata.plane_direction[player_tsides[number]]) {
            if (going_left) {

                xdistance = -((leveldata.airfield_x[player_tsides[number]] + (leveldata.airfield_lenght[player_tsides[number]] >> 1)) - player_x_8[number]);
            } else {
                xdistance = 600;

            }
        } else {
            if (!going_left) {
                xdistance = ((leveldata.airfield_x[player_tsides[number]] + (leveldata.airfield_lenght[player_tsides[number]] >> 1)) - player_x_8[number]);
            } else {
                xdistance = 600;

            }
        }

        ydistance = leveldata.airfield_y[player_tsides[number]] - player_y_8[number];


        if ((xdistance < 600) && (xdistance > 100) && (ydistance + 5 > (xdistance / 3)) && (ydistance - 5 < (xdistance / 3))) {

            if (player_speed[number] < 2000 || (ydistance > 130) || (ydistance - (player_speed[number] >> 6)) > 0) {
                current_mission[number] = AIM_LAND;
                mission_phase[number] = 0;
            }
        }



    }


    if (going_up && player_y[number] <= 5120)
        ai_turn_down(number);


    if (player_bombs[number] && !in_closing[number]) {

        if (((player_angle[number] <= 23040 || player_angle[number] >= 69120) && !player_upsidedown[number]) ||
            ((player_angle[number] >= 23040 && player_angle[number] <= 69120) && player_upsidedown[number]))
            ai_do_bombing(number);

    }




    if (player_spinning[number]) {
        if (!player_rolling[number])
            player_rolling[number] = 1;
    }

    if (spinning_remaining[number]) {
        spinning_remaining[number]--;
        if (!player_rolling[number])
            player_rolling[number] = 1;
    }

    if (plane_present[number] && player_ammo[number] && (player_last_shot[number] > SHOTS_RATE)) {
        for (flaggy = 0; flaggy < 16; flaggy++) {
            if (flaggy == number)
                continue;

            if (!plane_present[flaggy])
                continue;

            if (player_sides[number] != player_sides[flaggy])
                continue;


            if (distance_enemy[flaggy] < (SHOT_AT_RANGE - 20) && angle_difference[flaggy] < 8) {
                return;

            }
        }

        for (flaggy = 0; flaggy < 16; flaggy++) {

            if (!plane_present[flaggy])
                continue;

            if (player_sides[number] == player_sides[flaggy])
                continue;

            if (distance_enemy[flaggy] < SHOT_AT_RANGE && angle_difference[flaggy] < SHOT_AT_ANGLE) {

                start_shot(number);
                break;
            }
        }
    }
}

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

/* Triplane Turmoil flying objects code */

#include "world/constants.h"
#include "triplane.h"
#include "tripai.h"
#include "util/wutil.h"
#include "world/plane.h"
#include "io/sound.h"
#include "world/tripaudio.h"

void do_shots(void);
void start_shot(int player);
void start_shot(int x, int y, int angle, int speed, int infan = -1);
void do_fobjects(void);
void start_smoke(int player);
void start_ssmoke(int x, int y);
void detect_damage(void);
void start_parts(int player);
void start_rifle(int x, int y);
void start_gun_wave(int x);
void drop_bomb(int player, int target = -1);
void do_bombs(void);
void start_bomb_explo(int l, int hitted = 0);
void start_explox(int x, int y);
void start_wave(int x);
void start_flame(int x, int y, int width);
void do_flames(void);
void start_one_flame(int x, int y);

int flame_x[MAX_FLAMES];
int flame_y[MAX_FLAMES];
int flame_width[MAX_FLAMES];
int flame_age[MAX_FLAMES];

/******************************************************************************/

void start_flame(int x, int y, int width) {
    int l;

    for (l = 0; l < MAX_FLAMES; l++)
        if (!flame_x[l])
            break;

    if (l == MAX_FLAMES)
        return;

    flame_x[l] = x + 3;
    flame_y[l] = y - 7;
    flame_width[l] = width - 6;
    flame_age[l] = FLAME_AGE;
}

void do_flames(void) {
    int l;

    for (l = 0; l < MAX_FLAMES; l++) {
        if (!flame_x[l])
            continue;

        if (!(--flame_age[l])) {
            flame_x[l] = 0;
            continue;

        }


        start_one_flame(flame_x[l] + wrandom(flame_width[l]), flame_y[l]);

    }

}

void start_one_flame(int x, int y) {
    int l;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {
        fobjects[l].x = x << 8;
        fobjects[l].y = y << 8;

        fobjects[l].x_speed = 0;
        fobjects[l].y_speed = 0;
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 7;
        fobjects[l].height = 14;
        fobjects[l].type = FOBJECTS_FLAME;
        fobjects[l].phase = 0;
    }

}


void do_shots(void) {
    int l, l2;
    unsigned char kohta;

    for (l = 0; l < MAX_SHOTS; l++) {
        if (shots_flying_x[l]) {

            shots_flying_y_speed[l] -= SHOTS_GRAVITY;
            shots_flying_x[l] += shots_flying_x_speed[l] >> 9;
            shots_flying_y[l] -= shots_flying_y_speed[l] >> 9;

            if ((shots_flying_x[l] < 0) || ((shots_flying_x[l] >> 8) >= NUMBER_OF_SCENES * 160) || shots_flying_y[l] < 0 || (shots_flying_y[l] >> 8) >= 200)
                shots_flying_x[l] = 0;
            else {
                kohta = level_bitmap[(shots_flying_x[l] >> 8) + (shots_flying_y[l] >> 8) * 2400];
                if (kohta < 112 || kohta > 119) {
                    if (kohta >= 224 && kohta <= 231)
                        start_gun_wave(shots_flying_x[l] >> 8);

                    shots_flying_x[l] = 0;
                }
            }

            if ((shots_flying_age[l]++) > SHOTS_RANGE)
                shots_flying_x[l] = 0;

            for (l2 = 0; l2 < 16; l2++) {
                if (plane_present[l2])
                    if (((player_x[l2] + 2304) > shots_flying_x[l]) &&
                        ((player_x[l2] - 2304) < shots_flying_x[l]) &&
                        ((player_y[l2] + 2304) > shots_flying_y[l]) && ((player_y[l2] - 2304) < shots_flying_y[l]))
                        if (plane_p[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]][player_upsidedown[l2]]
                            [(shots_flying_x[l] >> 8) - (player_x_8[l2]) + 10 + ((shots_flying_y[l] >> 8) - (player_y_8[l2]) + 10) * 20] != 255) {

                            if (config.sound_on && config.sfx_on)
                                play_2d_sample(sample_hit[wrandom(4)], player_x_8[solo_country], player_x_8[l2]);

                            shots_flying_x[l] = 0;

                            if (shots_flying_owner[l] != -1) {
                                player_hits[shots_flying_owner[l]]++;
                                player_endurance[l2] -= wrandom(12);
                            } else
                                player_endurance[l2] -= wrandom(6);

                            if (player_endurance[l2] < 1) {
                                if (!player_spinning[l2] && !in_closing[l2]) {
                                    if (shots_flying_owner[l] != -1) {
                                        if (player_sides[l2] != player_sides[shots_flying_owner[l]]) {
                                            player_points[shots_flying_owner[l]]++;

                                        } else {
                                            player_points[shots_flying_owner[l]]--;
                                        }

                                        player_shots_down[shots_flying_owner[l]][l2]++;
                                    }
                                }
                                if (wrandom(4) && (!player_on_airfield[l2]))
                                    player_spinning[l2] = 1;
                                else {
                                    in_closing[l2] = 2;
                                    plane_present[l2] = 0;
                                    start_parts(l2);
                                }
                            }
                            break;
                        }
            }
        }


    }

}

void start_shot(int x, int y, int angle, int speed, int infan) {
    int l;

    for (l = 0; l < MAX_SHOTS; l++)
        if (!shots_flying_x[l])
            break;

    if (l != MAX_SHOTS) {
        shots_flying_age[l] = 0;
        shots_flying_x[l] = (x << 8);
        shots_flying_y[l] = (y << 8);
        shots_flying_x_speed[l] = (cosinit[angle] * speed) >> 2;
        shots_flying_y_speed[l] = (sinit[angle] * speed) >> 2;
        shots_flying_x[l] += shots_flying_x_speed[l] >> 6;
        shots_flying_y[l] -= shots_flying_y_speed[l] >> 6;
        shots_flying_owner[l] = -1;
        shots_flying_infan[l] = infan;
    }

}


void start_shot(int player) {
    int l;
    int plane_type = player;

    player_last_shot[player] = 0;

    if (in_closing[player])
        return;

    player_fired[player]++;
    player_ammo[player]--;
    for (l = 0; l < MAX_SHOTS; l++)
        if (!shots_flying_x[l])
            break;

    if (l != MAX_SHOTS) {
        shots_flying_age[l] = 0;
        shots_flying_x[l] = player_x[player] + 12 * cosinit[player_angle[player] >> 8];
        shots_flying_y[l] = player_y[player] - 12 * sinit[player_angle[player] >> 8];
        shots_flying_x_speed[l] = (cosinit[player_angle[player] >> 8] * (player_speed[player] + SHOTS_SPEED)) >> 2;
        shots_flying_y_speed[l] = (sinit[player_angle[player] >> 8] * (player_speed[player] + SHOTS_SPEED)) >> 2;
        shots_flying_owner[l] = player;
        shots_flying_infan[l] = -1;
    }
    if (config.gunshot_sounds && config.sound_on && config.sfx_on) {
        if (plane_type == 0 || plane_type > 3)
            plane_type = 1;
        play_2d_sample(sample_konsu[plane_type], player_x_8[solo_country], player_x_8[player]);
    }
}

void do_fobjects(void) {
    int l, l2;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++) {
        if (fobjects[l].x) {

            fobjects[l].x += fobjects[l].x_speed >> 8;
            fobjects[l].y -= fobjects[l].y_speed >> 8;

            if ((fobjects[l].y < 0) || (fobjects[l].x < 0) || ((fobjects[l].x >> 8) >= NUMBER_OF_SCENES * 160) || (fobjects[l].y >> 8) >= 200) {
                fobjects[l].x = 0;
                continue;
            }

            if ((level_bitmap[(fobjects[l].x >> 8) + (fobjects[l].y >> 8) * 2400]) < 112
                || ((level_bitmap[(fobjects[l].x >> 8) + (fobjects[l].y >> 8) * 2400])) > 119) {
                fobjects[l].x = 0;
                continue;
            }

            if (fobjects[l].hit_plane && part_collision_detect)
                for (l2 = 0; l2 < 16; l2++) {
                    if (plane_present[l2]) {
                        if (((player_x[l2] + 2304) > fobjects[l].x) &&
                            ((player_x[l2] - 2304) < fobjects[l].x) && ((player_y[l2] + 2304) > fobjects[l].y) && ((player_y[l2] - 2304) < fobjects[l].y))
                            if (plane_p[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]][player_upsidedown[l2]]
                                [(fobjects[l].x >> 8) - (player_x_8[l2]) + 10 + ((fobjects[l].y >> 8) - (player_y_8[l2]) + 10) * 20] != 255) {
                                fobjects[l].x = 0;
                                player_endurance[l2] -= wrandom(FOBJECTS_DAMAGE);
                                if (player_endurance[l2] < 1) {
                                    if ((fobjects[l].owner != -1) && (!player_spinning[l2])) {
                                        player_shots_down[fobjects[l].owner][l2]++;
                                        if (player_sides[fobjects[l].owner] != player_sides[l2])
                                            player_points[fobjects[l].owner]++;
                                        else
                                            player_points[fobjects[l].owner]--;
                                    }

                                    if (wrandom(2) == 1 && (!player_on_airfield[l2]))
                                        player_spinning[l2] = 1;
                                    else {
                                        in_closing[l2] = 2;
                                        plane_present[l2] = 0;
                                        start_parts(l2);
                                    }
                                }
                                break;
                            }
                    }
                }



            switch (fobjects[l].type) {
            case FOBJECTS_SMOKE:
                fobjects[l].phase++;
                if (fobjects[l].phase == SMOKE_FRAMES)
                    fobjects[l].x = 0;
                break;

            case FOBJECTS_SSMOKE:
                fobjects[l].phase++;
                if (fobjects[l].phase == 17)
                    fobjects[l].x = 0;
                break;


            case FOBJECTS_RIFLE:
                fobjects[l].phase++;
                if (fobjects[l].phase == 12)
                    fobjects[l].phase = 0;
                fobjects[l].y_speed -= FOBJECTS_GRAVITY;
                break;


            case FOBJECTS_WAVE1:
                fobjects[l].phase++;
                if (fobjects[l].phase == WAVE1_FRAMES)
                    fobjects[l].x = 0;
                break;

            case FOBJECTS_WAVE2:
                fobjects[l].phase++;
                if (fobjects[l].phase == WAVE2_FRAMES)
                    fobjects[l].x = 0;
                break;


            case FOBJECTS_ITEXPLOSION:
                fobjects[l].phase++;
                if (fobjects[l].phase == ITEXPLOSION_FRAMES)
                    fobjects[l].x = 0;
                break;


            case FOBJECTS_PARTS:
                fobjects[l].y_speed -= FOBJECTS_GRAVITY;
                break;

            case FOBJECTS_EXPLOX:
                fobjects[l].phase++;
                if (fobjects[l].phase == EXPLOX_FRAMES)
                    fobjects[l].x = 0;
                break;

            case FOBJECTS_FLAME:
                fobjects[l].phase++;
                if (fobjects[l].phase == NUMBER_OF_FLAMES)
                    fobjects[l].x = 0;

                if (config.structure_smoke)
                    if (fobjects[l].phase & 3)
                        start_ssmoke(fobjects[l].x, fobjects[l].y - 256);
                break;
            }
        }
    }
}


void start_wave(int x) {
    int l;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {
        fobjects[l].x = (x) << 8;
        fobjects[l].y = (terrain_level[x] - 11) << 8;
        fobjects[l].x_speed = 0;
        fobjects[l].y_speed = 0;
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 23;
        fobjects[l].height = 23;
        fobjects[l].type = FOBJECTS_WAVE1;
        fobjects[l].phase = 0;
    }

}

void start_gun_wave(int x) {
    int l;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {
        fobjects[l].x = (x) << 8;
        fobjects[l].y = (terrain_level[x] - 2) << 8;
        fobjects[l].x_speed = 0;
        fobjects[l].y_speed = 0;
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 3;
        fobjects[l].height = 5;
        fobjects[l].type = FOBJECTS_WAVE2;
        fobjects[l].phase = 0;
    }

}


void start_smoke(int player) {
    int l;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {
        fobjects[l].x = player_x[player] - 12 * cosinit[player_angle[player] >> 8];
        fobjects[l].y = player_y[player] + 12 * sinit[player_angle[player] >> 8];

        fobjects[l].x_speed = 0;
        fobjects[l].y_speed = 0;
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 20;
        fobjects[l].height = 20;
        fobjects[l].type = FOBJECTS_SMOKE;
        fobjects[l].phase = 0;
    }

}

void start_ssmoke(int x, int y) {
    int l;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {

        fobjects[l].x = x;
        fobjects[l].y = y - 1024 + wrandom(2048);

        fobjects[l].x_speed = 0;
        fobjects[l].y_speed = 256 * 256 * 3;
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 9;
        fobjects[l].height = 9;
        fobjects[l].type = FOBJECTS_SSMOKE;
        fobjects[l].phase = 0;
    }

}


void start_explox(int x, int y) {
    int l;
    int l2;
    int number_of_exps;

    number_of_exps = wrandom(EXPLOX_RANDOM) + EXPLOX_CONST;

    for (l2 = 0; l2 < number_of_exps; l2++) {


        for (l = 0; l < MAX_FLYING_OBJECTS; l++)
            if (!fobjects[l].x)
                break;

        if (l != MAX_FLYING_OBJECTS) {
            fobjects[l].x = x + wrandom(EXPLOX_VARIETY) - (EXPLOX_VARIETY / 2);
            fobjects[l].y = y + wrandom(EXPLOX_VARIETY) - (EXPLOX_VARIETY / 2);

            fobjects[l].x_speed = 0;
            fobjects[l].y_speed = 0;
            fobjects[l].hit_plane = 0;
            fobjects[l].width = 9;
            fobjects[l].height = 9;
            fobjects[l].type = FOBJECTS_EXPLOX;
            fobjects[l].subtype = wrandom(4);
            fobjects[l].phase = 0 - wrandom(EXPLOX_PHASE_DIFF);

        }
    }
}

void detect_damage(void) {
    int l;

    for (l = 0; l < 16; l++) {
        if (!player_exists[l])
            continue;

        if (wrandom(plane_mass[l] >> 4) >= player_endurance[l])
            if (wrandom(2))
                start_smoke(l);

    }
}

void start_parts(int player) {
    int l, l2, limit;
    int xxx, yyy;
    int xsss;

    limit = wrandom(NUMBER_OF_EXPLOSION_PARTS);

    start_explox(player_x[player], player_y[player]);

    xxx = player_x[player] - 12 * cosinit[player_angle[player] >> 8];
    yyy = player_y[player] + 12 * sinit[player_angle[player] >> 8];
    xsss = (cosinit[player_angle[player] >> 8] * (player_speed[player])) >> 2;

    for (l2 = 0; l2 < limit; l2++) {
        for (l = 0; l < MAX_FLYING_OBJECTS; l++)
            if (!fobjects[l].x)
                break;

        if (l != MAX_FLYING_OBJECTS) {
            fobjects[l].x = xxx;
            fobjects[l].y = yyy;
            fobjects[l].x_speed = xsss + wrandom(PARTS_SPEED) - (PARTS_SPEED >> 1);
            fobjects[l].y_speed = (wrandom(PARTS_SPEED) - (PARTS_SPEED >> 1));
            fobjects[l].hit_plane = 1;
            fobjects[l].width = 10;
            fobjects[l].height = 10;
            fobjects[l].type = FOBJECTS_PARTS;
            fobjects[l].phase = wrandom(NUMBER_OF_BITES);
            fobjects[l].owner = -1;
        }
    }
}

void start_rifle(int x, int y) {
    int l;

    for (l = 0; l < MAX_FLYING_OBJECTS; l++)
        if (!fobjects[l].x)
            break;

    if (l != MAX_FLYING_OBJECTS) {
        fobjects[l].x = x << 8;
        fobjects[l].y = y << 8;
        fobjects[l].x_speed = 256 * 128 * 5 - wrandom(2560 * 128);
        fobjects[l].y_speed = wrandom(256 * 5 * 128);
        fobjects[l].hit_plane = 0;
        fobjects[l].width = 8;
        fobjects[l].height = 8;
        fobjects[l].type = FOBJECTS_RIFLE;
        fobjects[l].phase = 0;
        fobjects[l].owner = -1;
    }

}


void drop_bomb(int player, int target) {
    int l;

    for (l = 0; l < MAX_BOMBS; l++)
        if (!bomb_x[l])
            break;

    if (l == MAX_BOMBS)
        return;

    if (target != -1)
        bombs_going[target] = l;

    player_bombed[player]++;

    bomb_speed[l] = player_speed[player];
    bomb_angle[l] = player_angle[player];
    bomb_x_speed[l] = (cosinit[bomb_angle[l] >> 8] * bomb_speed[l]) >> 2;
    bomb_y_speed[l] = (sinit[bomb_angle[l] >> 8] * bomb_speed[l]) >> 2;
    bomb_x[l] = player_x[player] + (9 - player_upsidedown[player] * 18) * sinit[player_angle[player] >> 8];
    bomb_y[l] = player_y[player] + (9 - player_upsidedown[player] * 18) * cosinit[player_angle[player] >> 8];

    bomb_owner[l] = player;
}

void do_bombs(void) {
    int l2, l, tempero;

    for (l = 0; l < MAX_BOMBS; l++) {
        if (!bomb_x[l])
            continue;

        bomb_y_speed[l] -= BOMB_GRAVITY;
        bomb_x[l] += bomb_x_speed[l] >> 9;
        bomb_y[l] -= bomb_y_speed[l] >> 9;



        if (bomb_y_speed[l] == 0) {
            if (bomb_x_speed[l] > 0)
                bomb_angle[l] = 0;
            else
                bomb_angle[l] = 46080;
        } else {
            if (bomb_x_speed[l] == 0) {
                if (bomb_y_speed[l] > 0)
                    bomb_angle[l] = 23040;
                else
                    bomb_angle[l] = 69120;
            } else {
                tempero = ((bomb_y_speed[l] << 7) / bomb_x_speed[l]);
                if (tempero > 512)
                    tempero = 512;
                if (tempero < (-512))
                    tempero = -512;

                if (bomb_x_speed[l] > 0)
                    bomb_angle[l] = tempero * 45;
                else
                    bomb_angle[l] = 46080 + tempero * 45;

            }
        }



        if (bomb_angle[l] >= 92160)
            bomb_angle[l] -= 92160;

        if (bomb_angle[l] < 0)
            bomb_angle[l] += 92160;


        if ((bomb_x[l] < 0) || (bomb_x[l] >> 8) >= 2400 || (bomb_y[l] >> 8) > 199) {
            bomb_x[l] = 0;
            continue;
        }

        if (bomb_y[l] >= 0)
            if ((level_bitmap[(bomb_x[l] >> 8) + (bomb_y[l] >> 8) * 2400]) < 112 || ((level_bitmap[(bomb_x[l] >> 8) + (bomb_y[l] >> 8) * 2400])) > 119
                || ((bomb_y[l] >> 8) >= 200)) {
                start_bomb_explo(l);

                bomb_x[l] = 0;
                continue;
            }


        for (l2 = 0; l2 < 16; l2++) {
            if (plane_present[l2]) {
                if (((player_x[l2] + 2304) > bomb_x[l]) &&
                    ((player_x[l2] - 2304) < bomb_x[l]) && ((player_y[l2] + 2304) > bomb_y[l]) && ((player_y[l2] - 2304) < bomb_y[l]))
                    if (plane_p[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]][player_upsidedown[l2]]
                        [(bomb_x[l] >> 8) - (player_x_8[l2]) + 10 + ((bomb_y[l] >> 8) - (player_y_8[l2]) + 10) * 20] != 255) {
                        bomb_x[l] = 0;
                        player_endurance[l2] = 0;
                        if (player_endurance[l2] < 1) {
                            if ((!player_spinning[l2]) && (!in_closing[l2])) {
                                if (player_sides[bomb_owner[l]] != player_sides[l2])
                                    player_points[bomb_owner[l]]++;
                                else
                                    player_points[bomb_owner[l]]--;

                                player_shots_down[bomb_owner[l]][l2]++;
                            }

                            in_closing[l2] = 2;
                            plane_present[l2] = 0;
                            start_parts(l2);
                            start_bomb_explo(l, 1);

                        }
                        break;
                    }
            }
        }


    }


}

void start_bomb_explo(int bb, int hitted) {
    int l, l2, limit;
    int palasia = 0;

    if (playing_solo) {
        if (bomb_owner[bb] != solo_country) {
            for (l = 0; l < 100; l++) {
                if (bombs_going[l] == bb) {
                    bombs_going[l] = -1;
                    break;

                }

            }

        }

    } else {
        if (computer_active[bomb_owner[bb]]) {
            for (l = 0; l < 100; l++) {
                if (bombs_going[l] == bb) {
                    bombs_going[l] = -1;
                    break;

                }

            }

        }

    }


    limit = wrandom(MAX_BOMB_PARTS - MIN_BOMB_PARTS);
    limit += MIN_BOMB_PARTS;


    if ((level_bitmap[(bomb_x[bb] >> 8) + (bomb_y[bb] >> 8) * 2400] >= 224)
        && (level_bitmap[(bomb_x[bb] >> 8) + (bomb_y[bb] >> 8) * 2400] <= 231)) {
        start_wave(bomb_x[bb] >> 8);
        if (config.splash && config.sound_on && config.sfx_on)
            play_2d_sample(sample_splash[wrandom(3)], player_x_8[solo_country], bomb_x[bb] >> 8);
    } else {
        start_explox(bomb_x[bb], bomb_y[bb]);
        if (config.explosion_sounds && config.sound_on && config.sfx_on)
            play_2d_sample(sample_bomb[wrandom(3) + 1], player_x_8[solo_country], bomb_x[bb] >> 8);

    }

    for (l = 0; l < MAX_AA_GUNS; l++)
        if (kkbase_x[l] && kkbase_status[l] != 2) {
            if ((bomb_x[bb] >> 8) >= kkbase_x[l] && (bomb_x[bb] >> 8) <= (kkbase_x[l] + 25) &&
                (bomb_y[bb] >> 8) >= (kkbase_y[l] - 5) && (bomb_y[bb] >> 8) <= (kkbase_y[l] + 30)) {
                if (kkbase_country[l] == player_sides[bomb_owner[bb]])
                    hitted = -1;

                if (config.flames)
                    start_flame(kkbase_x[l], kkbase_y[l] + 21, 26);

                if (playing_solo) {
                    if (leveldata.struct_type[kkbase_number[l]] == 1)
                        solo_dest_remaining--;

                    if (leveldata.struct_type[kkbase_number[l]] == 2)
                        solo_failed = 1;

                }

                kkbase_status[l] = 2;
                kkbase_frame[l] = 0;
                struct_state[kkbase_number[l]] = 1;

                if (hitted != -1)
                    hitted = 1;
            }
        }

    for (l = 0; l < MAX_STRUCTURES; l++)
        if ((structures[l][1] != NULL) && (!struct_state[l])) {
            if ((bomb_x[bb] >> 8) >= leveldata.struct_x[l] && (bomb_x[bb] >> 8) <= (leveldata.struct_x[l] + struct_width[l]) &&
                (bomb_y[bb] >> 8) >= (leveldata.struct_y[l] - 5) && (bomb_y[bb] >> 8) <= (leveldata.struct_y[l] + 5 + struct_heigth[l])) {
                if (config.flames)
                    start_flame(leveldata.struct_x[l], leveldata.struct_y[l] + struct_heigth[l], struct_width[l]);



                struct_state[l] = 1;

                if (playing_solo) {
                    if (leveldata.struct_type[l] == 1)
                        solo_dest_remaining--;

                    if (leveldata.struct_type[l] == 2)
                        solo_failed = 1;

                }

                if (player_sides[leveldata.struct_owner[l]] == player_sides[bomb_owner[bb]])
                    hitted = -1;

                if (hitted != -1)
                    hitted = 1;

                if (current_mode == SVGA_MODE) {

                    structures[l][1]->blit_to_bitmap(standard_background, leveldata.struct_x[l] - (leveldata.struct_x[l] / 800) * 800,
                                                     leveldata.struct_y[l] + (leveldata.struct_x[l] / 800) * 196 - 4);

                    structures[l][1]->blit(leveldata.struct_x[l] - (leveldata.struct_x[l] / 800) * 800,
                                           leveldata.struct_y[l] + (leveldata.struct_x[l] / 800) * 196 - 4);

                    if ((leveldata.struct_x[l] - (leveldata.struct_x[l] / 800) * 800) + struct_width[l] > 800)
                        structures[l][1]->blit(leveldata.struct_x[l] - (leveldata.struct_x[l] / 800) * 800 - 800,
                                               leveldata.struct_y[l] + (leveldata.struct_x[l] / 800 + 1) * 196 - 4);
                }

                if (leveldata.struct_hit[l]) {
                    structures[l][1]->blit_to_bitmap(maisema, leveldata.struct_x[l], leveldata.struct_y[l]);
                }



            }
        }

    for (l = 0; l < MAX_INFANTRY; l++) {
        if (infan_x[l] && infan_state[l] < 3) {
            if ((bomb_x[bb] >> 8) >= (infan_x[l] - 30) && (bomb_x[bb] >> 8) <= (infan_x[l] + 44) &&
                (bomb_y[bb] >> 8) >= (infan_y[l] - 20) && (bomb_y[bb] >> 8) <= (infan_y[l] + 30)) {
                if (player_sides[infan_country[l]] == player_sides[bomb_owner[bb]])
                    hitted = -1;

                if (hitted != -1)
                    hitted = 1;

                if (playing_solo) {
                    if (leveldata.struct_type[l] == 1)
                        solo_dest_remaining--;

                    if (leveldata.struct_type[l] == 2)
                        solo_failed = 1;

                }

                start_rifle(infan_x[l], infan_y[l]);

                if (infan_x[l] == (bomb_x[bb] >> 8)) {
                    infan_x[l] = 0;

                }

                if (infan_x[l] > (bomb_x[bb] >> 8)) {

                    infan_direction[l] = 1;
                    infan_frame[l] = 0;
                    infan_state[l] = 4;
                    infan_x_speed[l] = 2;
                }

                if (infan_x[l] < (bomb_x[bb] >> 8)) {

                    infan_direction[l] = 0;
                    infan_frame[l] = 0;
                    infan_state[l] = 4;
                    infan_x_speed[l] = -2;
                }


            }

        }
    }


    if (hitted == 1) {
        player_bomb_hits[bomb_owner[bb]]++;
    }

    if (palasia)
        for (l2 = 0; l2 < limit; l2++) {
            for (l = 0; l < MAX_FLYING_OBJECTS; l++)
                if (!fobjects[l].x)
                    break;

            if (l != MAX_FLYING_OBJECTS) {
                fobjects[l].x = bomb_x[bb];
                fobjects[l].y = bomb_y[bb];
                fobjects[l].x_speed = wrandom(PARTS_SPEED) - PARTS_SPEED / 2;
                fobjects[l].y_speed = (wrandom(PARTS_SPEED) - PARTS_SPEED / 2);
                fobjects[l].hit_plane = 1;
                fobjects[l].width = 10;
                fobjects[l].height = 10;
                fobjects[l].type = FOBJECTS_PARTS;
                fobjects[l].phase = wrandom(NUMBER_OF_BITES);
                fobjects[l].owner = bomb_owner[bb];
            }
        }
}

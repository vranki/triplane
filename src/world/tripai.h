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

#include "constants.h"

constexpr int32_t SPEED = 4;

constexpr int32_t MEKAN_DIRECTION_LEFT = 0;
constexpr int32_t MEKAN_DIRECTION_RIGHT = 1;

constexpr int32_t MEKAN_MISSION_IDLE = 0;
constexpr int32_t MEKAN_MISSION_RETURN = 1;
constexpr int32_t MEKAN_MISSION_PUSH_OUT = 2;
constexpr int32_t MEKAN_MISSION_PUSH_IN = 3;

/* Submissions of MEKAN_MISSION_PUSH_IN */
constexpr int32_t MEKAN_PUSH_IN_MOVE_TO_PLANE = 0;
constexpr int32_t MEKAN_PUSH_IN_PUSH = 1;
constexpr int32_t MEKAN_PUSH_IN_WAIT_DOOR_CLOSE = 2;

/* Submissions of MEKAN_MISSION_PUSH_OUT */
constexpr int32_t MEKAN_PUSH_OUT_WAIT_DOOR_OPEN = 0;
constexpr int32_t MEKAN_PUSH_OUT_MOVE_TO_PLANE = 1;
constexpr int32_t MEKAN_PUSH_OUT_PUSH = 2;

/* These only affect graphics. */
constexpr int32_t MEKAN_ANIMATION_INVISIBLE = 0;
constexpr int32_t MEKAN_ANIMATION_WALK = 1;
constexpr int32_t MEKAN_ANIMATION_PUSH_PROPELLER = 2;
constexpr int32_t MEKAN_ANIMATION_PUSH_TAIL = 3;

extern int infan_x[MAX_INFANTRY];
extern int infan_y[MAX_INFANTRY];
extern int infan_direction[MAX_INFANTRY];
extern int infan_last_shot[MAX_INFANTRY];
extern int infan_state[MAX_INFANTRY];
extern int infan_country[MAX_INFANTRY];
extern int infan_frame[MAX_INFANTRY];
extern int infan_stop[8];
extern int infan_x_speed[MAX_INFANTRY];

extern int itgun_shot_x[MAX_ITGUN_SHOTS];
extern int itgun_shot_y[MAX_ITGUN_SHOTS];
extern int itgun_shot_x_speed[MAX_ITGUN_SHOTS];
extern int itgun_shot_y_speed[MAX_ITGUN_SHOTS];
extern int itgun_shot_age[MAX_ITGUN_SHOTS];

namespace world::tripai {

    void do_infan();

    void infan_to_plane(int l);

    void infan_to_infan(int l);

    void infan_to_struct(int l);

    void infan_take_hits(int l);

    void do_kkbase();

    void start_it_shot(int x, int y, int angle);

    void do_it_shots();

    void start_itgun_explosion(int number);

    void do_ai(int number);

    void ai_turn_down(int number);

    void ai_turn_up(int number);

    void ai_evade_terrain(int number);

    void ai_turnplus(int number);

    void ai_turnminus(int number);

    void ai_do_bombing(int number);

    int check_structs(int x, int y, int number);

    void do_mekan();

    void do_doors();

} // namespace world::tripai

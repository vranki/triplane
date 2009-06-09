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

/* Triplane Turmoil flying objects header */

extern void do_shots(void);
extern void start_shot(int player);
extern void start_shot(int x, int y, int angle, int speed, int infan = -1);
extern void do_fobjects(void);
extern void start_smoke(int player);
extern void detect_damage(void);
extern void start_parts(int player);
extern void start_rifle(int x, int y);
extern void start_gun_wave(int x);
extern void drop_bomb(int player, int target = -1);
extern void do_bombs(void);
extern void start_bomb_explo(int l, int hitted = 0);
extern void start_explox(int x, int y);
extern void start_wave(int x);
extern void start_flame(int x, int y, int width);
extern void do_flames(void);

extern int flame_x[MAX_FLAMES];
extern int flame_y[MAX_FLAMES];
extern int flame_width[MAX_FLAMES];
extern int flame_age[MAX_FLAMES];

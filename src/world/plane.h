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

#ifndef PLANE_H
#define PLANE_H

/* Note: 1== enabled, 0== disabled */
extern unsigned char controls_up[16], controls_down[16], controls_power[16], controls_power2[16];

extern int player_x[16], player_y[16];
extern int player_speed[16];
extern int player_angle[16];
extern int player_ammo[16], player_bombs[16], player_gas[16];
extern int player_upsidedown[16];
extern int player_rolling[16];
extern int player_spinning[16];
extern int spinning_remaining[16];
extern int player_x_speed[16], player_y_speed[16];
extern int player_last_shot[16];
extern int player_endurance[16];
extern int player_points[16];

extern int player_x_8[16];
extern int player_y_8[16];

extern int plane_power[16];
extern int plane_manover[16];
extern int plane_mass[16];
extern char plane_name[16][15];
extern int plane_bombs[16];

extern int plane_gas[16];
extern int plane_ammo[16];
extern int player_was_on_airfield[16];

extern int t_plane_power[16];
extern int t_plane_manover[16];
extern int t_plane_mass[16];
extern int t_plane_bombs[16];
extern int t_plane_gas[16];
extern int t_plane_ammo[16];

void model_planes(void);

#endif

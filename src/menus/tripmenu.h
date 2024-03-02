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

#ifndef TRIPMENU_H
#define TRIPMENU_H

/* Triplane Turmoil menusystem headerfile */

#include "../gfx/gfx.h"
#include "../gfx/bitmap.h"
#include <cstdint>

constexpr int32_t PROJECTOR_Y_ADJUSTMENT = 21;
constexpr int32_t PROJECTOR_X1 = 5;
constexpr int32_t PROJECTOR_X2 = 315;
constexpr int32_t PROJECTOR_Y1 = 8;
constexpr int32_t PROJECTOR_Y2 = 178;

void main_menu();
void roster_menu();
void credits_menu();
void letter_menu();
void aces_menu();
void options_menu();
void controls_menu();
int kangas_menu();
int solo_player_menu();

void print_clear_roster(Bitmap *rosteri);
void print_filled_roster(int number);
void wait_mouse_relase(int nokb = 0);
void load_descriptions(int number);
void show_descriptions(int number);
void sort_and_show(int percent = 0);
int get_rank(int player);

void aces_grand_total();
void aces_multi_total();
void aces_solo_total();
void aces_one_solo(int country, int mission);
int calculate_multitotal(int player);
void show_feat5();

#endif

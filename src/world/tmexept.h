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

/* Solomission exceptions code headerfile */

namespace world::tmexept {

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

} // namespace world::tmexept

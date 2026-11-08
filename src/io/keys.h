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

#ifndef TRIPLANE_KEYS_H
#define TRIPLANE_KEYS_H

#include <SDL3/SDL_keycode.h>

/*
 * Triplane key codes.
 *
 * The game was written against SDL 1.2 and stores raw key codes in
 * keyset.dta and roster.dta. To keep those files valid, the game keeps
 * using SDL 1.2 key numbering internally and translates SDL3 key codes
 * into it. Printable keys use their (lowercase) Latin-1 value.
 */
enum : int {
  TK_UNKNOWN = 0,
  TK_BACKSPACE = 8,
  TK_TAB = 9,
  TK_CLEAR = 12,
  TK_RETURN = 13,
  TK_PAUSE = 19,
  TK_ESCAPE = 27,
  TK_SPACE = 32,
  TK_PLUS = 43,
  TK_COMMA = 44,
  TK_MINUS = 45,
  TK_PERIOD = 46,
  TK_0 = 48, TK_1, TK_2, TK_3, TK_4, TK_5, TK_6, TK_7, TK_8, TK_9,
  TK_a = 97, TK_b, TK_c, TK_d, TK_e, TK_f, TK_g, TK_h, TK_i, TK_j, TK_k,
  TK_l, TK_m, TK_n, TK_o, TK_p, TK_q, TK_r, TK_s, TK_t, TK_u, TK_v, TK_w,
  TK_x, TK_y, TK_z,
  TK_DELETE = 127,
  TK_KP0 = 256, TK_KP1, TK_KP2, TK_KP3, TK_KP4, TK_KP5, TK_KP6, TK_KP7,
  TK_KP8, TK_KP9,
  TK_KP_PERIOD = 266,
  TK_KP_DIVIDE,
  TK_KP_MULTIPLY,
  TK_KP_MINUS,
  TK_KP_PLUS,
  TK_KP_ENTER,
  TK_KP_EQUALS,
  TK_UP = 273,
  TK_DOWN,
  TK_RIGHT,
  TK_LEFT,
  TK_INSERT,
  TK_HOME,
  TK_END,
  TK_PAGEUP,
  TK_PAGEDOWN,
  TK_F1 = 282, TK_F2, TK_F3, TK_F4, TK_F5, TK_F6, TK_F7, TK_F8, TK_F9,
  TK_F10, TK_F11, TK_F12, TK_F13, TK_F14, TK_F15,
  TK_NUMLOCK = 300,
  TK_CAPSLOCK,
  TK_SCROLLOCK,
  TK_RSHIFT,
  TK_LSHIFT,
  TK_RCTRL,
  TK_LCTRL,
  TK_RALT,
  TK_LALT,
  TK_RMETA,
  TK_LMETA,
  TK_LSUPER,
  TK_RSUPER,
  TK_MODE,
  TK_COMPOSE,
  TK_HELP,
  TK_PRINT,
  TK_SYSREQ,
  TK_BREAK,
  TK_MENU,
  TK_POWER,
  TK_EURO,
  TK_UNDO,
  TK_LAST
};

/* Pressed state of every key, indexed by TK_* code. */
extern unsigned char *key;

/* Translate between SDL3 key codes and TK_* codes (0 if unknown). */
int key_from_sdl(SDL_Keycode k);
SDL_Keycode key_to_sdl(int k);

/* Human readable name of a TK_* key code. */
const char *key_name(int k);

/* Rebuild key[] from SDL's keyboard state. Call after pumping events. */
void keys_refresh();

#endif

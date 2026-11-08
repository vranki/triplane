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

#include "keys.h"

#include <SDL3/SDL.h>
#include <cstring>

static unsigned char key_state[TK_LAST];
unsigned char *key = key_state;

namespace {

struct key_pair {
  SDL_Keycode sdl;
  int tk;
};

/* Keys whose SDL3 code is not their Latin-1 value. */
constexpr key_pair special_keys[] = {
    {SDLK_PAUSE, TK_PAUSE},
    {SDLK_CLEAR, TK_CLEAR},
    {SDLK_KP_0, TK_KP0},
    {SDLK_KP_1, TK_KP1},
    {SDLK_KP_2, TK_KP2},
    {SDLK_KP_3, TK_KP3},
    {SDLK_KP_4, TK_KP4},
    {SDLK_KP_5, TK_KP5},
    {SDLK_KP_6, TK_KP6},
    {SDLK_KP_7, TK_KP7},
    {SDLK_KP_8, TK_KP8},
    {SDLK_KP_9, TK_KP9},
    {SDLK_KP_PERIOD, TK_KP_PERIOD},
    {SDLK_KP_DIVIDE, TK_KP_DIVIDE},
    {SDLK_KP_MULTIPLY, TK_KP_MULTIPLY},
    {SDLK_KP_MINUS, TK_KP_MINUS},
    {SDLK_KP_PLUS, TK_KP_PLUS},
    {SDLK_KP_ENTER, TK_KP_ENTER},
    {SDLK_KP_EQUALS, TK_KP_EQUALS},
    {SDLK_UP, TK_UP},
    {SDLK_DOWN, TK_DOWN},
    {SDLK_RIGHT, TK_RIGHT},
    {SDLK_LEFT, TK_LEFT},
    {SDLK_INSERT, TK_INSERT},
    {SDLK_HOME, TK_HOME},
    {SDLK_END, TK_END},
    {SDLK_PAGEUP, TK_PAGEUP},
    {SDLK_PAGEDOWN, TK_PAGEDOWN},
    {SDLK_F1, TK_F1},
    {SDLK_F2, TK_F2},
    {SDLK_F3, TK_F3},
    {SDLK_F4, TK_F4},
    {SDLK_F5, TK_F5},
    {SDLK_F6, TK_F6},
    {SDLK_F7, TK_F7},
    {SDLK_F8, TK_F8},
    {SDLK_F9, TK_F9},
    {SDLK_F10, TK_F10},
    {SDLK_F11, TK_F11},
    {SDLK_F12, TK_F12},
    {SDLK_F13, TK_F13},
    {SDLK_F14, TK_F14},
    {SDLK_F15, TK_F15},
    {SDLK_NUMLOCKCLEAR, TK_NUMLOCK},
    {SDLK_CAPSLOCK, TK_CAPSLOCK},
    {SDLK_SCROLLLOCK, TK_SCROLLOCK},
    {SDLK_RSHIFT, TK_RSHIFT},
    {SDLK_LSHIFT, TK_LSHIFT},
    {SDLK_RCTRL, TK_RCTRL},
    {SDLK_LCTRL, TK_LCTRL},
    {SDLK_RALT, TK_RALT},
    {SDLK_LALT, TK_LALT},
    {SDLK_RGUI, TK_RMETA},
    {SDLK_LGUI, TK_LMETA},
    {SDLK_MODE, TK_MODE},
    {SDLK_HELP, TK_HELP},
    {SDLK_PRINTSCREEN, TK_PRINT},
    {SDLK_SYSREQ, TK_SYSREQ},
    {SDLK_MENU, TK_MENU},
    {SDLK_POWER, TK_POWER},
    {SDLK_UNDO, TK_UNDO},
};

} // namespace

int key_from_sdl(SDL_Keycode k) {
  if (k > 0 && k < 256)
    return (int)k;
  for (const auto &p : special_keys)
    if (p.sdl == k)
      return p.tk;
  return TK_UNKNOWN;
}

SDL_Keycode key_to_sdl(int k) {
  if (k > 0 && k < 256)
    return (SDL_Keycode)k;
  for (const auto &p : special_keys)
    if (p.tk == k)
      return p.sdl;
  return SDLK_UNKNOWN;
}

const char *key_name(int k) {
  SDL_Keycode sdl = key_to_sdl(k);
  if (sdl == SDLK_UNKNOWN)
    return "unknown key";
  const char *name = SDL_GetKeyName(sdl);
  return (name && *name) ? name : "unknown key";
}

void keys_refresh() {
  int numkeys = 0;
  const bool *state = SDL_GetKeyboardState(&numkeys);

  memset(key_state, 0, sizeof(key_state));
  for (int sc = 0; sc < numkeys; sc++) {
    if (!state[sc])
      continue;
    int tk = key_from_sdl(
        SDL_GetKeyFromScancode((SDL_Scancode)sc, SDL_KMOD_NONE, false));
    if (tk > 0 && tk < TK_LAST)
      key_state[tk] = 1;
  }
}

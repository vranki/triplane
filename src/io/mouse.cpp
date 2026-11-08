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

/*******************************************************************************

   Purpose:
        Mouse handling part of Wsystem 2.0 for DJGPP v.2.0�5

*******************************************************************************/

#include "../io/mouse.h"
#include "../io/video.h"
#include <SDL3/SDL.h>
#include "../io/sdl_compat.h"

void hiiri_to(int x, int y) {
  float wx, wy;
  SDL_RenderCoordinatesToWindow(video_state.renderer,
                                (float)(x * pixel_multiplier),
                                (float)(y * pixel_multiplier), &wx, &wy);
  SDL_WarpMouseInWindow(video_state.window, wx, wy);
}

void koords(int *x, int *y, int *n1, int *n2) {
  SDL_MouseButtonFlags ret;
  float wx, wy, lx, ly;

  pump_events();
  ret = SDL_GetMouseState(&wx, &wy);
  *n1 = !!(ret & SDL_BUTTON_LMASK);
  *n2 = !!(ret & SDL_BUTTON_RMASK);

  SDL_RenderCoordinatesFromWindow(video_state.renderer, wx, wy, &lx, &ly);
  *x = (int)lx / pixel_multiplier;
  *y = (int)ly / pixel_multiplier;
  if (video_state.surface) {
    *x = SDL_clamp(*x, 0, video_state.surface->w / pixel_multiplier - 1);
    *y = SDL_clamp(*y, 0, video_state.surface->h / pixel_multiplier - 1);
  }
}

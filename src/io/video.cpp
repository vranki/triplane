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

#include "video.h"

#include <SDL/SDL.h>
#include <cassert>
#include <csignal>
#include <unistd.h>

#include "dksfile.h"
#include "../util/wutil.h"

struct video_state_t video_state = {nullptr, 0, 0};

struct naytto ruutu;

int current_mode = VGA_MODE;
unsigned char *vircr;
int update_vircr_mode = 1;
int draw_with_vircr_mode = 1;
int pixel_multiplier = 1; /* current pixel multiplier */
int pixel_multiplier_vga = 1, pixel_multiplier_svga = 1;
int wantfullscreen = 1;

// Screen resolution
int screen_width = 800;
int screen_height = 600;
int screen_width_less = screen_width - 1;
int screen_height_less = screen_height - 1;

int split_num = 2;

SDL_Color curpal[256];

/**
 * Sets palette entries firstcolor to firstcolor+n-1
 * from pal[0] to pal[n-1].
 * @param pal the palette, specify nullptr to set all colors to black=(0,0,0)
 * @param reverse = 1 to read colors in reverse order (pal[n-1] to pal[0])
 */
void setpal_range(const char pal[][3], int firstcolor, int n, int reverse) {
  SDL_Color *cc = (SDL_Color *)walloc(n * sizeof(SDL_Color));
  int i, from = (reverse ? n - 1 : 0);

  for (i = 0; i < n; i++) {
    if (pal == nullptr) {
      cc[i].r = cc[i].g = cc[i].b = 0;
    } else {
      cc[i].r = 4 * pal[from][0];
      cc[i].g = 4 * pal[from][1];
      cc[i].b = 4 * pal[from][2];
    }
    if (reverse)
      from--;
    else
      from++;
  }

  if (draw_with_vircr_mode) {
    SDL_SetPalette(video_state.surface,
                   video_state.haverealpalette ? SDL_PHYSPAL : SDL_LOGPAL, cc,
                   firstcolor, n);
  } else {
    SDL_SetPalette(video_state.surface, SDL_PHYSPAL | SDL_LOGPAL, cc,
                   firstcolor, n);
  }
  memcpy(&curpal[firstcolor], cc, n * sizeof(SDL_Color));
  wfree(cc);
}

static Uint32 getcolor(unsigned char c) {
  if (video_state.haverealpalette)
    return c;
  else
    return SDL_MapRGB(video_state.surface->format, curpal[c].r, curpal[c].g,
                      curpal[c].b);
}

void fillrect(const int x, const int y, const int w, const int h, const int c) {
  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  if (pixel_multiplier > 1) {
    r.x *= pixel_multiplier;
    r.y *= pixel_multiplier;
    r.w *= pixel_multiplier;
    r.h *= pixel_multiplier;
  }
  SDL_FillRect(video_state.surface, &r, getcolor(c));
}

void do_all(int /*do_retrace*/) {
  if (draw_with_vircr_mode) {
    if (pixel_multiplier > 1) {
      int i, j, k;
      int w = (current_mode == VGA_MODE) ? 320 : screen_width;
      int h = (current_mode == VGA_MODE) ? 200 : screen_height;
      uint8_t *in = vircr, *out = (uint8_t *)video_state.surface->pixels;
      /* optimized versions using 32-bit and 16-bit writes when possible */
      if (pixel_multiplier == 4 && sizeof(char *) >= 4) { /* word size >= 4 */
        uint32_t cccc;
        for (j = 0; j < h * pixel_multiplier; j += pixel_multiplier) {
          for (i = 0; i < w * pixel_multiplier; i += pixel_multiplier) {
            cccc = *in | (*in << 8) | (*in << 16) | (*in << 24);
            in++;
            for (k = 0; k < pixel_multiplier; k++) {
              *(uint32_t *)(&out[(j + k) * (w * pixel_multiplier) + i]) = cccc;
            }
          }
        }
      } else if (pixel_multiplier == 3) {
        uint16_t cc, c;
        for (j = 0; j < h * pixel_multiplier; j += pixel_multiplier) {
          for (i = 0; i < w * pixel_multiplier; i += pixel_multiplier) {
            c = *in++;
            cc = c | (c << 8);
            for (k = 0; k < pixel_multiplier; k++) {
              *(uint16_t *)(&out[(j + k) * (w * pixel_multiplier) + i]) = cc;
              out[(j + k) * (w * pixel_multiplier) + i + 2] = c;
            }
          }
        }
      } else if (pixel_multiplier == 2) {
        uint16_t cc;
        for (j = 0; j < h * pixel_multiplier; j += pixel_multiplier) {
          for (i = 0; i < w * pixel_multiplier; i += pixel_multiplier) {
            cc = *in | (*in << 8);
            in++;
            for (k = 0; k < pixel_multiplier; k++) {
              *(uint16_t *)(&out[(j + k) * (w * pixel_multiplier) + i]) = cc;
            }
          }
        }
      } else { /* unoptimized version */
        int l;
        uint8_t c;
        for (j = 0; j < h * pixel_multiplier; j += pixel_multiplier) {
          for (i = 0; i < w * pixel_multiplier; i += pixel_multiplier) {
            c = *in++;
            for (k = 0; k < pixel_multiplier; k++) {
              for (l = 0; l < pixel_multiplier; l++) {
                out[(j + k) * (w * pixel_multiplier) + (i + l)] = c;
              }
            }
          }
        }
      }
    }
  }

  SDL_Flip(video_state.surface);
}

static void sigint_handler(int /*dummy*/) { _exit(1); }

void init_video() {
  int ret;

  if (!video_state.init_done) {
    ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK |
                   SDL_INIT_NOPARACHUTE);
    if (ret) {
      fprintf(stderr,
              "SDL_Init failed with %d. Is your DISPLAY environment variable "
              "set?\n",
              ret);
      exit(1);
    }
    signal(SIGINT, sigint_handler);
    atexit(SDL_Quit);
    video_state.init_done = 1;

    SDL_WM_SetCaption("Triplane Classic", "Triplane Classic");
    SDL_ShowCursor(SDL_DISABLE);

    if (!draw_with_vircr_mode) {
      vircr = (unsigned char *)walloc(screen_width * screen_height);
    }
  }
}

static int init_mode(int new_mode, const char *paletname) {
  Uint32 mode_flags;
  const SDL_VideoInfo *vi;
  int las, las2;
  int w = (new_mode == SVGA_MODE) ? screen_width : 320;
  int h = (new_mode == SVGA_MODE) ? screen_height : 200;

  init_video();

  mode_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWPALETTE;

  if (!draw_with_vircr_mode)
    mode_flags |= SDL_ANYFORMAT;
  if (wantfullscreen)
    mode_flags |= SDL_FULLSCREEN;

  if (draw_with_vircr_mode && pixel_multiplier > 1)
    wfree(vircr);

  pixel_multiplier =
      (new_mode == SVGA_MODE) ? pixel_multiplier_svga : pixel_multiplier_vga;

  video_state.surface = SDL_SetVideoMode(w * pixel_multiplier,
                                         h * pixel_multiplier, 8, mode_flags);
  assert(video_state.surface);

  if (draw_with_vircr_mode) {
    if (pixel_multiplier > 1) {
      vircr = (uint8_t *)walloc(w * h);
    } else {
      vircr = (uint8_t *)video_state.surface->pixels;
    }
  }
  /* else vircr is preallocated in init_video */
  vi = SDL_GetVideoInfo();
  video_state.haverealpalette = (vi->vfmt->palette != nullptr);

  dksopen(paletname);

  dksread(ruutu.normaalipaletti, sizeof(ruutu.normaalipaletti));
  for (las = 0; las < 256; las++)
    for (las2 = 0; las2 < 3; las2++)
      ruutu.paletti[las][las2] = ruutu.normaalipaletti[las][las2];

  dksclose();

  setpal_range(ruutu.paletti, 0, 256);
  all_bitmaps_refresh();

  current_mode = new_mode;
  return 1;
}

int init_vesa(const char *paletname) { return init_mode(SVGA_MODE, paletname); }

void init_vga(const char *paletname) { init_mode(VGA_MODE, paletname); }

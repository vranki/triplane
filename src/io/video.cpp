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

#include <csignal>
#include <unistd.h>

#include "dksfile.h"
#include "../util/wutil.h"

struct video_state_t video_state;

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
  SDL_Color *cc = (SDL_Color *)util::wutil::walloc(n * sizeof(SDL_Color));
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

  for (i = 0; i < n; i++)
    cc[i].a = SDL_ALPHA_OPAQUE;
  if (video_state.palette)
    SDL_SetPaletteColors(video_state.palette, cc, firstcolor, n);
  memcpy(&curpal[firstcolor], cc, n * sizeof(SDL_Color));
  util::wutil::wfree(cc);
}

static Uint32 getcolor(unsigned char c) { return c; }

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
  SDL_FillSurfaceRect(video_state.surface, &r, getcolor(c));
}

/* Convert the paletted frame buffer to RGB and show it. */
static void present_frame() {
  SDL_Surface *src = video_state.surface;
  void *pixels;
  int pitch;
  Uint32 lut[256];

  for (int i = 0; i < 256; i++)
    lut[i] = 0xff000000u | ((Uint32)curpal[i].r << 16) |
             ((Uint32)curpal[i].g << 8) | (Uint32)curpal[i].b;

  if (SDL_LockTexture(video_state.texture, nullptr, &pixels, &pitch)) {
    for (int y = 0; y < src->h; y++) {
      const uint8_t *in = (const uint8_t *)src->pixels + y * src->pitch;
      Uint32 *out = (Uint32 *)((uint8_t *)pixels + y * pitch);
      for (int x = 0; x < src->w; x++)
        out[x] = lut[in[x]];
    }
    SDL_UnlockTexture(video_state.texture);
  }

  SDL_SetRenderDrawColor(video_state.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(video_state.renderer);
  SDL_RenderTexture(video_state.renderer, video_state.texture, nullptr, nullptr);
  SDL_RenderPresent(video_state.renderer);
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

  present_frame();
}

static void sigint_handler(int /*dummy*/) { _exit(1); }

void init_video() {
  if (!video_state.init_done) {
    SDL_SetAppMetadata("Triplane Classic", TRIPLANE_VERSION,
                       "fi.iki.triplane");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
      fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
      exit(1);
    }
    signal(SIGINT, sigint_handler);
    atexit(SDL_Quit);
    video_state.init_done = 1;

    if (!SDL_CreateWindowAndRenderer("Triplane Classic", 960, 600,
                                     SDL_WINDOW_RESIZABLE |
                                         (wantfullscreen ? SDL_WINDOW_FULLSCREEN
                                                         : 0),
                                     &video_state.window,
                                     &video_state.renderer)) {
      fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
      exit(1);
    }
    SDL_HideCursor();

    video_state.palette = SDL_CreatePalette(256);
    if (!video_state.palette) {
      fprintf(stderr, "SDL_CreatePalette: %s\n", SDL_GetError());
      exit(1);
    }

    if (!draw_with_vircr_mode) {
      vircr = (unsigned char *)util::wutil::walloc(screen_width * screen_height);
    }
  }
}

static int init_mode(int new_mode, const char *paletname) {
  int las, las2;
  int w = (new_mode == SVGA_MODE) ? screen_width : 320;
  int h = (new_mode == SVGA_MODE) ? screen_height : 200;

  init_video();

  if (draw_with_vircr_mode && pixel_multiplier > 1)
      util::wutil::wfree(vircr);

  pixel_multiplier =
      (new_mode == SVGA_MODE) ? pixel_multiplier_svga : pixel_multiplier_vga;

  int sw = w * pixel_multiplier, sh = h * pixel_multiplier;

  if (video_state.surface)
    SDL_DestroySurface(video_state.surface);
  video_state.surface = SDL_CreateSurface(sw, sh, SDL_PIXELFORMAT_INDEX8);
  if (!video_state.surface) {
    fprintf(stderr, "SDL_CreateSurface: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_SetSurfacePalette(video_state.surface, video_state.palette);
  /* the game writes to vircr directly, so the pitch must equal the width */
  if (video_state.surface->pitch != sw) {
    fprintf(stderr, "Unexpected surface pitch %d for width %d\n",
            video_state.surface->pitch, sw);
    exit(1);
  }

  if (video_state.texture)
    SDL_DestroyTexture(video_state.texture);
  video_state.texture =
      SDL_CreateTexture(video_state.renderer, SDL_PIXELFORMAT_XRGB8888,
                        SDL_TEXTUREACCESS_STREAMING, sw, sh);
  if (!video_state.texture) {
    fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_SetTextureScaleMode(video_state.texture, SDL_SCALEMODE_NEAREST);
  SDL_SetRenderLogicalPresentation(video_state.renderer, sw, sh,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  if (!wantfullscreen) {
    /* Show low resolution modes at a usable size, like 3x for 320x200. */
    int scale = 1;
    while (sw * (scale + 1) <= 1280 && sh * (scale + 1) <= 800)
      scale++;
    SDL_SetWindowSize(video_state.window, sw * scale, sh * scale);
  }

  if (draw_with_vircr_mode) {
    if (pixel_multiplier > 1) {
      vircr = (uint8_t *)util::wutil::walloc(w * h);
    } else {
      vircr = (uint8_t *)video_state.surface->pixels;
    }
  }
  /* else vircr is preallocated in init_video */
  video_state.haverealpalette = 1;

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

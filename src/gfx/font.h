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

#ifndef FONT_H
#define FONT_H


class Font {
  private:
    Bitmap * glyphs[256];
    int width;
    int height;
    int char_width[256];
    int char_start[256];
    int scaled;
    int scaled_space;

  public:
      Font(const char *font_name);
     ~Font();
    int printf(int x, int y, const char *fmt, ...);
    int scanf(int x, int y, char *str, int max_len);
    void scale(void);
    void unscale(void);
    void count_scale(void);
    void set_space(int space);
};

#endif

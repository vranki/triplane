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
        DKS-datafile viewer for Wsystem 2.0 for DJGPP v.2.0

*******************************************************************************/

#include "gfx/gfx.h"
#include "io/dksfile.h"
#include "io/trip_io.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>


int main(int argv, char *argc[]) {
  char nimi[FILENAME_MAX];
  int lask, lask2 = 0;

  wantfullscreen = 0;
  printf("PGDView 1.1 for Wsystem.  (c) 1996 Dodekaedron Software Creations, "
         "Inc.\n\n");

  if ((argv != 3) && (argv != 2)) {
    printf("Usage: PGDVIEW dks-datafilename [picturename] (no extension)\n");
    exit(1);
  }
  snprintf(nimi, sizeof(nimi), "%s.dks", argc[1]);

  if (!dksinit(nimi)) {
    printf("Cannot open %s\n", nimi);
    exit(1);
  }

  if (argv == 3) {
    init_vga("PALET1");
    tyhjaa_vircr();
    Bitmap kuva(argc[2]);
    kuva.blit(0, 0);
    do_all_clear();
    getch();
  } else {
    printf("\n");
    for (lask = 0; lask < DKS_MAX_ENTRIES; lask++) {
      if (!dirri[lask].nimi[0])
        break;
      printf("%-6s ", dirri[lask].nimi);
      lask2++;
      if (lask2 == 10) {
        lask2 = 0;
        printf("\n");
      }
    }
  }
  printf("\n");
  return 0;
}

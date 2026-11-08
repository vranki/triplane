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
        DKS-datafile handling routines for Wsystem 2.0 for DJGPP v.2.0

*******************************************************************************/

#include "../io/dksfile.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../util/wutil.h"
#include <SDL3/SDL_endian.h>

static main_directory_entry directory[DKS_MAX_ENTRIES];
main_directory_entry *dirri = directory;
FILE *dks_faili;
char dks_tiedosto[FILENAME_MAX];
int nykyinen_faili;

/*
 * Find absolute path to given datafile. The following paths are tried
 * (in that order):
 *
 * $TRIPLANE_DATA/$name
 * TRIPLANE_DATA/$name
 * $name
 *
 * @param name name of datafile
 * @param path pointer to FILENAME_MAX bytes where absolute path can be
 * written.
 */
static void get_datafile_path(const char *name, char *path) {
  const char *data;

  data = getenv("TRIPLANE_DATA");
  if (data) {
    snprintf(path, FILENAME_MAX, "%s/%s", data, name);
    return;
  }

  data = TRIPLANE_DATA;
  {
    int ret;
    struct stat st;

    ret = stat(data, &st);
    if (!ret) {
      snprintf(path, FILENAME_MAX, "%s/%s", data, name);
      return;
    }
  }

  snprintf(path, FILENAME_MAX, "%s", name);
}

int dksinit(const char *tiedosto) {
  char tekstia[20];

  get_datafile_path(tiedosto, dks_tiedosto);

  if ((dks_faili = fopen(dks_tiedosto, "rb")) == nullptr)
    return (0);

  if (fread(tekstia, sizeof(tekstia), 1, dks_faili) != 1 ||
      fread(dirri, sizeof(main_directory_entry) * DKS_MAX_ENTRIES, 1,
            dks_faili) != 1) {
    fclose(dks_faili);
    return (0);
  }
  for (int i = 0; i < DKS_MAX_ENTRIES; i++) {
    dirri[i].offset = SDL_Swap32LE(dirri[i].offset);
    dirri[i].koko = SDL_Swap32LE(dirri[i].koko);
  }

  fclose(dks_faili);

  return (1);
}

int dksopen(const char *nimi) {
  int lask;
  int kohta = -1;

  for (lask = 0; lask < DKS_MAX_ENTRIES && dirri[lask].nimi[0]; lask++) {
    if (!strcmp(dirri[lask].nimi, nimi)) {
      kohta = lask;
      break;
    }
  }

  if (kohta == -1) {
    return (0);
  }

  if ((dks_faili = fopen(dks_tiedosto, "rb")) == nullptr)
    return (0);

  fseek(dks_faili, dirri[kohta].offset, SEEK_SET);

  nykyinen_faili = kohta;

  return (1);
}

int extdksopen(const char *nimi) {
  int lask;
  int kohta = -1;

  for (lask = 0; lask < DKS_MAX_ENTRIES && dirri[lask].nimi[0]; lask++) {
    if (!strcmp(dirri[lask].nimi, nimi)) {
      kohta = lask;
      break;
    }
  }

  if (kohta == -1) {
    dks_faili = fopen(nimi, "rb");
    if (dks_faili == nullptr)
      return (0);
  } else {
    if ((dks_faili = fopen(dks_tiedosto, "rb")) == nullptr)
      return (0);

    fseek(dks_faili, dirri[kohta].offset, SEEK_SET);

    nykyinen_faili = kohta;
  }

  return (1);
}

void dksclose() { fclose(dks_faili); }

int dkssize() { return dirri[nykyinen_faili].koko; }

int dksread(void *mihin, unsigned long int koko) {
  if (fread(mihin, koko, 1, dks_faili))
    return 1;
  else
    return 0;
}



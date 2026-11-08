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

#ifndef DKSFILE_H
#define DKSFILE_H

#include <cstdint>
#include <cstdio>

constexpr int32_t DKS_MAX_ENTRIES = 8192;

/* One entry of the directory at the start of a .dks file, as stored on disk
 * (little-endian). Shared with dksbuild and pgdview. */
#pragma pack(push, 1)
struct main_directory_entry {
  char nimi[7];
  uint32_t offset;
  uint32_t koko;
};
#pragma pack(pop)

extern main_directory_entry *dirri;

int dksinit(const char *tiedosto);
int dksopen(const char *nimi);
int extdksopen(const char *nimi);
void dksclose();
int dksread(void *mihin, unsigned long int koko);
int dkssize();

extern FILE *dks_faili;

#endif

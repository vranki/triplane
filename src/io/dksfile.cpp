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
   	DKS-datafile handling routines for Wsystem 2.0 for DJGPP v.2.0á5

*******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "util/wutil.h"
#include "io/dksfile.h"

#define MAX_ENTRIES 8192

#pragma pack(1)
struct main_directory_entry {
    char nimi[7];
    unsigned long int offset __attribute__ ((packed));
    unsigned long int koko __attribute__ ((packed));
};

#pragma pack()

main_directory_entry *dirri;

class Dirrikka {
  public:
    Dirrikka(void);
     ~Dirrikka();

};

Dirrikka::Dirrikka(void) {
    dirri = (main_directory_entry *)
        walloc(MAX_ENTRIES * sizeof(main_directory_entry));
}

Dirrikka::~Dirrikka() {
    free(dirri);
}

Dirrikka dirrikka;
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
        strncpy(path, data, FILENAME_MAX - 1);
        strncat(path, "/", FILENAME_MAX - 1);
        strncat(path, name, FILENAME_MAX - 1);
        return;
    }

    data = TRIPLANE_DATA;
    {
        int ret;
        struct stat st;

        ret = stat(data, &st);
        if (!ret) {
            strncpy(path, data, FILENAME_MAX - 1);
            strncat(path, "/", FILENAME_MAX - 1);
            strncat(path, name, FILENAME_MAX - 1);
            return;
        }
    }

    strncpy(path, name, FILENAME_MAX - 1);
}

int dksinit(const char *tiedosto) {

    char tekstia[20];

    get_datafile_path(tiedosto, dks_tiedosto);

    if ((dks_faili = fopen(dks_tiedosto, "rb")) == NULL)
        return (0);

    fread(tekstia, sizeof(tekstia), 1, dks_faili);
    fread(dirri, sizeof(main_directory_entry) * MAX_ENTRIES, 1, dks_faili);

    fclose(dks_faili);

    return (1);
}

int dksopen(const char *nimi) {
    int lask;
    int kohta = -1;

    for (lask = 0; lask < MAX_ENTRIES; lask++) {
        if (!strcmp(dirri[lask].nimi, nimi)) {
            kohta = lask;
            break;
        }
    }

    if (kohta == -1) {

        return (0);
    }


    if ((dks_faili = fopen(dks_tiedosto, "rb")) == NULL)
        return (0);

    fseek(dks_faili, dirri[kohta].offset, SEEK_SET);

    nykyinen_faili = kohta;

    return (1);
}

int extdksopen(const char *nimi) {
    int lask;
    int kohta = -1;
    int faili = 0;

    for (lask = 0; lask < MAX_ENTRIES; lask++) {
        if (!strcmp(dirri[lask].nimi, nimi)) {
            kohta = lask;
            break;
        }
    }

    if (kohta == -1) {
        dks_faili = fopen(nimi, "rb");
        faili = 1;
        if (dks_faili == NULL)
            return (0);
    } else {
        if ((dks_faili = fopen(dks_tiedosto, "rb")) == NULL)
            return (0);

        fseek(dks_faili, dirri[kohta].offset, SEEK_SET);

        nykyinen_faili = kohta;
    }

    return (1);
}



void dksclose(void) {
    fclose(dks_faili);

}

int dkssize(void) {
    return dirri[nykyinen_faili].koko;
}

int dksread(void *mihin, unsigned long int koko) {
    if (fread(mihin, koko, 1, dks_faili))
        return 1;
    else
        return 0;
}

int dksseek(int offset, int mode) {
    switch (mode) {
    case SEEK_SET:
        return (fseek(dks_faili, dirri[nykyinen_faili].offset + offset, SEEK_SET));
        break;

    case SEEK_CUR:
        return (fseek(dks_faili, offset, SEEK_CUR));
        break;

    case SEEK_END:
        return (fseek(dks_faili, dirri[nykyinen_faili].offset + dirri[nykyinen_faili].koko + offset, SEEK_SET));
        break;


    }


    return 1;
}

int dksgetc(void) {
    return (fgetc(dks_faili));
}

long dkstell(void) {
    return (ftell(dks_faili) - dirri[nykyinen_faili].offset);
}

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

   Purpose: Converts files to unsigned char strings in c

*******************************************************************************/

#include <stdio.h>
#include <io.h>
#include <mem.h>

int main(int argc, char **argv) {
    unsigned char *data;
    FILE *faili;
    int c;
    int size;

    printf("\nDat2C (c) 1996 Wraith/DKS\n\n");

    if (argc != 3) {
        printf("Usage: DAT2C inputfile outputfile\n");
        exit(0);
    }

    if ((faili = fopen(argv[1], "rb")) == NULL) {
        printf("Unable to open file %s\n", argv[1]);
        exit(1);

    }

    size = filelength(fileno(faili));

    data = (unsigned char *) malloc(size);
    if (data == NULL) {
        printf("Out of memory\n");
        fclose(faili);
        exit(1);

    }

    fread(data, size, 1, faili);
    fclose(faili);

    faili = fopen(argv[2], "wt");

    fprintf(faili, "/* Dat2C converted data begins here*/\n\n");

    fprintf(faili, "unsigned char converted_data[%d] = {\n", size);

    for (c = 0; c < (size - 1); c++) {
        fprintf(faili, "%d,", data[c]);
        if (c && (!(c % 32)))
            fprintf(faili, "\n");

    }

    fprintf(faili, "%d };", data[c]);
    fprintf(faili, "\n\n/* Dat2C converted data ends here */\n");

    fclose(faili);

}

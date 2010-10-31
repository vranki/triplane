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
   	.PCX to .PGD graphics converter for Wsystem 2.0 for DJGPP v.2.0á5
  
*******************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Dummy relics from DOS age. */
void gotoxy(int x, int y) {
}

int wherey(void) {
    return 0;
}

#define RAJA 192
unsigned char getpixel(int x, int y);
unsigned char *vkuva;
unsigned char *kuvadata;
char paletti[256][3];

void talleta_paletti() {
    FILE *out;


    if ((out = fopen("paletdta.000", "wb")) == NULL) {
        printf("\nError opening file: paletdta.000 \n");
        exit(1);
    }

    fwrite(paletti, sizeof(paletti), 1, out);

    fclose(out);

}


void pakit(int dx, int dy, char inputfile[], char picturename[]) {
    FILE *in, *out;
    long int xx, yy;
    short int x, y;
    char inf[FILENAME_MAX], outf[FILENAME_MAX];
    unsigned char vali;
    unsigned int lippu, lask, lask2, lask3;
    unsigned char *pointteri, *pointteri2;
    char kuvan_nimi[7];
    //int prossa,vprossa;
    double p;
    long int koko;
    int debug = 0;

    strcpy(inf, inputfile);
    strcpy(outf, inputfile);
    strcat(inf, ".gra");
    strcat(outf, ".pgd");
    strcpy(kuvan_nimi, picturename);

    x = dx;
    y = dy;

    xx = x;
    yy = y;

    koko = xx * yy;

    pointteri2 = (unsigned char *) malloc(800000);

    if (pointteri2 == NULL) {
        printf("Error in memory allocation -- %u bytes\n", x * y);
        exit(1);
    }

    pointteri = vkuva;

    if ((out = fopen(outf, "wb")) == NULL) {
        printf("Error opening file %s", outf);
        exit(1);
    }

    lippu = 1;
    lask = 0;
    lask2 = 0;
    // vprossa=prossa=0;

    if (debug) {
        printf("Picturename: %6s Size: %lu bytes.\n", kuvan_nimi, koko);

    }

    while (lippu) {


        if (pointteri[lask] == pointteri[lask + 1]) {
            if (debug) {
                gotoxy(60, wherey());
                printf("Match found\n");
            }
            for (lask3 = lask; ((*(pointteri + lask3) == *(pointteri + lask)) && (lask3 < (koko - 1)) && (lask3 - lask) < (255 - RAJA)); lask3++);

            if ((lask3 - lask) >= 2) {
                vali = RAJA;
                vali += (lask3 - lask);

                *(pointteri2 + lask2) = vali;
                *(pointteri2 + lask2 + 1) = *(pointteri + lask);

                if (debug) {
                    printf("%2x ", vali);
                    printf("%2x ", *(pointteri + lask));
                }

                lask += (lask3 - lask);
                lask2 += 2;
            } else {
                if (debug) {
                    gotoxy(60, wherey());
                    printf("in 3rd round now\n");
                }
                if (*(pointteri + lask) <= RAJA) {
                    *(pointteri2 + lask2) = *(pointteri + lask);
                    //printf("%2x ",*(pointteri+lask));

                } else {
                    *(pointteri2 + lask2) = RAJA + 1;
                    *(pointteri2 + lask2 + 1) = *(pointteri + lask);
                    lask2++;

                    if (debug) {
                        printf("%2x ", RAJA + 1);
                        printf("%2x ", *(pointteri + lask));
                    }
                }
                lask++;
                lask2++;
                if (lask >= (int) koko) {
                    lippu = 0;
                    gotoxy(60, wherey());
                    if (debug)
                        printf("Normal brake\n");

                    break;
                }
            }
        } else {
            if (*(pointteri + lask) <= RAJA) {
                *(pointteri2 + lask2) = *(pointteri + lask);
                if (debug)
                    printf("%2x ", *(pointteri + lask));

            } else {
                *(pointteri2 + lask2) = RAJA + 1;
                *(pointteri2 + lask2 + 1) = *(pointteri + lask);
                lask2++;
                if (debug) {
                    printf("%2x ", RAJA + 1);
                    printf("%2x ", *(pointteri + lask));
                }
            }
            lask++;
            lask2++;
            if (lask >= (int) koko) {
                lask += 2;
                lippu = 0;
                gotoxy(60, wherey());
                if (debug)
                    printf("Normal brake (1-part)\n");

                break;
            }
        }


    }

    printf("\nNew Size: %u (+13 bytes)  Ratio : %5.2f", lask2 + 1, (100.0 - ((lask2 * 1.0 + 1.0) * 100L) / (koko * 1.0)));


    fwrite(&x, sizeof(x), 1, out);
    fwrite(&y, sizeof(y), 1, out);
    lask2++; /* Why? Last byte is always uninitialized according to valgrind. */
    fwrite(&lask2, sizeof(lask2), 1, out);
    fwrite(kuvan_nimi, sizeof(kuvan_nimi), 1, out);
    fwrite(pointteri2, lask2, 1, out);

    fclose(out);

    free(pointteri2);
}


void pcx_load(char *file_name, short int *xl, short int *yl) {
    unsigned char a, b;
    int l, loop;

    FILE *fp;
    fp = fopen(file_name, "rb");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    fseek(fp, 8, SEEK_SET);
    fread(xl, 2, 1, fp);
    fread(yl, 2, 1, fp);
    (*xl)++;
    (*yl)++;

    fseek(fp, 128, SEEK_SET);
    l = 0;

    while (l < (*xl) * (*yl)) {
        a = fgetc(fp);
        if (a >= 0xc0) {
            b = fgetc(fp);
            for (loop = 0; loop < (a & 0x3f); loop++) {
                kuvadata[l] = b;
                l++;
            }
        } else {
            kuvadata[l] = a;
            l++;
        }

    }

    fseek(fp, -768, SEEK_END);

    for (loop = 0; loop < 256; loop++) {
        paletti[loop][0] = fgetc(fp) >> 2;
        paletti[loop][1] = fgetc(fp) >> 2;
        paletti[loop][2] = fgetc(fp) >> 2;
    }

    fclose(fp);

}


int main(int argc, char *argv[]) {
    char infile[FILENAME_MAX], outfile[FILENAME_MAX];
    char dxs[5], dys[5];
    int dx, dy;
    char ch;
    short int leveys, korkeus;
    int laskx, lasky, lask;
    char stringi1[FILENAME_MAX], stringi2[FILENAME_MAX];

    FILE *out;

    printf("\nPcx2pgd v. 1.14 For Wsystem 2.0 (c) 1996 Dodekaedron Software Creations, Inc.\n");
    printf("    This program is registered to Dodekaedron Squad. All rights reserved.\n\n");

    if (!(argc == 5 || argc == 3 || argc == 2)) {
        printf("\nUsage: PCX2PGD inputfile picturename [delta-x delta-y]\n");
        printf("Or:    PCX2PGD inputfile (creates palet)\n");
        printf("  NOTE: Filenames are given without extension!\n");
        exit(1);
    }

    vkuva = (unsigned char *) malloc(800000);

    if (vkuva == NULL) {
        printf("Error in memory allocation -- 800000 bytes\n");
        exit(1);
    }

    kuvadata = (unsigned char *) malloc(800000);

    if (kuvadata == NULL) {
        printf("Error in memory allocation -- 800000 bytes\n");
        exit(1);
    }

    strcpy(infile, argv[1]);
    strcpy(stringi1, argv[1]);


    strcpy(outfile, argv[1]);
    if (argc >= 3)
        strcpy(stringi2, argv[2]);




    if (argc == 5) {
        strcpy(dxs, argv[3]);

        strcpy(dys, argv[4]);

        dx = atoi(dxs);
        dy = atoi(dys);


        if (dx == 0)
            argc = 3;
    }

    printf("Loading pcx image %s.pcx...\n", infile);

    strcat(infile, ".pcx");


    pcx_load(infile, &leveys, &korkeus);

    if (argc == 2) {
        printf("Saving palet.\n");
        talleta_paletti();
        exit(0);
    }

    if (argc != 5) {
        dx = leveys;
        dy = korkeus;
    }

    printf("Loading complete.\n");
    printf("Imagesize %dx%d.\n", leveys, korkeus);
/*  leveys=dx;
  korkeus=dy;
*/
    if (argc == 3) {
        for (laskx = 0; laskx < leveys; laskx++)
            for (lasky = 0; lasky < korkeus; lasky++) {
                lask = kuvadata[laskx + lasky * leveys];
                if (lask == 254) {
                    /*dx=leveys=laskx;
                       dy=korkeus=lasky; */
                    dx = laskx;
                    dy = lasky;

                }
            }

    }

    printf("Sizedata dX:%d dY:%d\n", dx, dy);
    printf("Converting to new size...\n");

    for (int lx = 0; lx < dx; lx++)
        for (int ly = 0; ly < dy; ly++)
            vkuva[lx + dx * ly] = kuvadata[lx + ly * leveys];

    printf("Conversion complete.\n");

/*
  fwrite(&leveys,sizeof(leveys),1,out);
  fwrite(&korkeus,sizeof(korkeus),1,out);
  fwrite(&vkuva,(dx*dy),1,out);

  fclose(out); // suljetaan file
*/

    printf("Packing data...\n");
    pakit(dx, dy, stringi1, stringi2);

    if (argc == 3)
        printf("\nNote: autocoordinates were used. Size is : ( %d , %d )", dx, dy);

    free(kuvadata);
    free(vkuva);
    return (0);
}

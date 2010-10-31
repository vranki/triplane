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
   	Wsystem 2.0 Databuilder.

*******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_ENTRIES 8192

#pragma pack(1)
struct main_directory_entry {

	 char nimi[7];
	 unsigned long int offset;
	 unsigned long int koko;
};
#pragma pack()

struct buildaus_info {

	 char nimi[7];
	 char tiedosto[80];
	 unsigned long int koko;
};

int filelength(FILE *faili)
{
  int position;
  int size;

  position=ftell(faili);
  
  fseek(faili,0,SEEK_END);
  size=ftell(faili);

  fseek(faili,position,SEEK_SET);

  return size;

}

int main(int argc,char **argv)
{
  FILE *in,*out;
  char tekstia[20];
  int kohtien_lukumaara=0;
  char ch;
  char file_to_be[80];
  int monesko_merkki;
  int vaihe;
  buildaus_info *info;
  main_directory_entry *dirri;
  unsigned char *tila; 
  unsigned long koko;
  unsigned long offset; 
  char crypt_key[80]="";
  int crypt_length;
  int c_counter;
  unsigned char *crypt_pointer;
  int key_counter;
  int reg_number=5324434;

  if( (info=(buildaus_info *) malloc (MAX_ENTRIES*sizeof(buildaus_info))) ==NULL)
  { 
    printf("Out of memory");
    exit(1);
  }

  if( (dirri=(main_directory_entry *) malloc (MAX_ENTRIES*sizeof(main_directory_entry))) ==NULL)
  { 
    printf("Out of memory");
    exit(1);
  }

  if( (tila=(unsigned char* ) malloc(8000000))==NULL)
  {
    printf("Out of memory");
    exit(1);
  }

  printf("\nDKS Databuilder 2.1 for Wsystem        Copyright (C) 1996 Wraith\n");
  
  if(argc<2)
  {
    printf("\nUSAGE: DKSBUILD indexfilename\n");
    exit(1);   
  }

  for(vaihe=0;vaihe<MAX_ENTRIES;vaihe++)
  {
    dirri[vaihe].nimi[0]=0;
    dirri[vaihe].offset=0;
    dirri[vaihe].koko=0;
    info[vaihe].nimi[0]=0;
    info[vaihe].tiedosto[0]=0;

  }

  

  if( (in=fopen(argv[1],"r"))==NULL)
  {
    printf("\nError opening indexfile %s\n",argv[1]);
    exit(1);
  }
 
  strcpy(tekstia,"DKS Datafile\n\032\n"); 

  printf("\nReading file %s\n",argv[1]);
  
  ch=fgetc(in);
  while((!feof(in)) && (ch!='\n'))
  {
    file_to_be[kohtien_lukumaara]=ch;
    ch=fgetc(in);
    kohtien_lukumaara++;    
  }  
  
  file_to_be[kohtien_lukumaara]=0;
  kohtien_lukumaara=0;
  monesko_merkki=0;
  vaihe=0; 
  ch=fgetc(in);
  while(!feof(in))
  {  
    if(ch=='\n')
    { 
      if(!vaihe)
      { 
        info[kohtien_lukumaara].nimi[0]=0;
        info[kohtien_lukumaara].tiedosto[monesko_merkki]=0;
      }
      else
      {
        vaihe=0;
        info[kohtien_lukumaara].nimi[monesko_merkki]=0;
      }
      monesko_merkki=0;
      kohtien_lukumaara++;    
      
      printf(".");
      fflush(stdout);
    }
    else
      if(ch==32)
      {
        if(!vaihe)
        {
          info[kohtien_lukumaara].tiedosto[monesko_merkki]=0;
          monesko_merkki=0;
          vaihe=1;
        }
      }
      else
      {
        if(ch==';')
        {
        	while(ch!='\n')
        	  ch=fgetc(in);
        }  	
        else
        {
      	  if(vaihe)
          {
            if(monesko_merkki<6)
              info[kohtien_lukumaara].nimi[monesko_merkki]=ch;
            monesko_merkki++;

          }
          else
          {
            info[kohtien_lukumaara].tiedosto[monesko_merkki]=ch;
            monesko_merkki++;
          }
        }    
      }
      
    ch=fgetc(in);
  }    

  fclose(in);
  printf("\n");
  
   
  offset=sizeof(main_directory_entry)*MAX_ENTRIES+sizeof(tekstia);

  printf("\nProcessing entries found\n");
  



  for(vaihe=0;vaihe<kohtien_lukumaara;vaihe++)
  {
    if(!info[vaihe].tiedosto[0])
    {
      kohtien_lukumaara=vaihe;
      break;
    }
    if( (in=fopen(info[vaihe].tiedosto,"rb"))==NULL)
    {
      printf("\n\n Cannon't locate file %s\n",info[vaihe].tiedosto);
      exit(1);
    }
    
    koko=filelength(in);
    dirri[vaihe].koko=koko;
    dirri[vaihe].offset=offset;    

    if(!info[vaihe].nimi[0])
    {
      if( (strstr(info[vaihe].tiedosto,".pgd")) || (strstr(info[vaihe].tiedosto,".PGD")))
      {
        fseek(in,8,SEEK_SET); 
        fread(info[vaihe].nimi,sizeof(info[vaihe].nimi),1,in);
        fseek(in,0,SEEK_SET);
      }
      else
      {
        printf("\nError: file %s is not named.\n",info[vaihe].tiedosto);
        fclose(in);

        exit(1);
      }
    }

    strcpy(dirri[vaihe].nimi,info[vaihe].nimi);
    
    if(!fread(tila+offset-sizeof(tekstia)-sizeof(main_directory_entry)*MAX_ENTRIES,koko,1,in))
    {
       printf("\nError reading file %s.\n",info[vaihe].tiedosto);
       fclose(in);

       exit(1);
    }
    fclose(in);


    offset+=koko;
    printf(".");
    fflush(stdout);    
  } //for


  printf("\n\nTemporary data constructed, writing...\n");

  if( (out=fopen(file_to_be,"wb")) == NULL)
  {
    printf("\nError creating file %s\n",file_to_be);
    exit(1);

  }


  if(!fwrite(tekstia,sizeof(tekstia),1,out))
  {
    printf("\nError writing file %s\n",file_to_be);
    exit(1);
  }
  
  if(!fwrite(dirri,sizeof(main_directory_entry)*MAX_ENTRIES,1,out))
  {
    printf("\nError writing file %s\n",file_to_be);
    exit(1);
  }

  fwrite(tila,offset-20-sizeof(main_directory_entry)*MAX_ENTRIES,1,out);
  fclose(out);
  

 
  printf("\n%s created. Size: %lu Entries: %d.\n",file_to_be,offset,kohtien_lukumaara);

  free(info);
  free(dirri);
  free(tila);
    
  return (0);
} // main



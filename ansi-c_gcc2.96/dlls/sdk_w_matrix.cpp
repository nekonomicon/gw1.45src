/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_matrix.cpp 
   This is the weapons matrix module.
   This module loads weapon definition from the formatted file: gw.wd

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "sdk_w_matrix.h"


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "sdk_w_matrix.h"

static int loaded = 0;

unsigned char EXCEL_GetByte(FILE *fp);
inline int EXCEL_GetNextCell(FILE *fp, unsigned char *x);
float EXCEL_GetFloat(FILE *fp);
char *EXCEL_GetString(FILE *fp);
int EXCEL_GetInt(FILE *fp);

weapon_stat w_stat[MAX_WEAPONS];
advertisement_t advert[32];

int ReadAdFile(char *filename)
{
   int index, k;
   char c;
   FILE *fp;
   fp = fopen(filename, "r");
   if (!fp)
      return 0;
   for (k = 0; k < 32; k++)
      advert[k].length = 0;
   k = 0;
   while (!feof(fp) && k < 32)
      {
      index = 0;
      do
         {
         c = fgetc(fp);
         if (!c || c == '\n') break;
         if (index < 254) advert[k].data[index++] = c;
         } while (!feof(fp));
      advert[k].data[index] = 0;
      advert[k].length = index;
      k++;
      }
   fclose(fp);
   return k;
}

#ifdef OLD_WEAPON_MATRIX
int LoadWeaponMatrix(char *filename)
{
   int index = 0;
   int iinput = 0;
   unsigned char cinput, weapon_name[32];
   FILE  *fp;
   if (loaded) return 1;
   loaded = 1;
   fp = fopen(filename, "r");
   if (!fp)
      return 0;
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);
   EXCEL_GetString(fp);

   /*
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   printf ("-> %s\n", EXCEL_GetString(fp));
   */ 

   while (!feof(fp))
      {
      strcpy((char *)weapon_name, EXCEL_GetString(fp));
      cinput = EXCEL_GetInt(fp);
      if (cinput < 0 || cinput > MAX_WEAPONS) return cinput;
      index = cinput;
      strcpy((char *) w_stat[index].name, (char *)weapon_name);
      w_stat[index].id = index;
      //printf ("Weapon Name: %s\n", w_stat[index].name);
      //printf ("Weapon ID: %i\n", w_stat[index].id);
      w_stat[index].price = EXCEL_GetInt(fp);
      //printf (" Price: %i\n", w_stat[index].price);
      w_stat[index].clip = EXCEL_GetInt(fp);
      //printf (" Clip: %i\n", w_stat[index].clip);
      w_stat[index].max_carry = EXCEL_GetInt(fp);
      //printf (" Max Carry: %i\n", w_stat[index].max_carry);
      w_stat[index].recoil_pitch = EXCEL_GetFloat(fp);
      //printf (" Recoil Pitch: %.02f\n", w_stat[index].recoil_pitch);
      w_stat[index].recoil_yaw = EXCEL_GetFloat(fp);
      //printf (" Recoil Yaw: %.02f\n", w_stat[index].recoil_yaw);
      w_stat[index].zoom1 = EXCEL_GetFloat(fp);
      //printf (" Zoom: %.02f\n", w_stat[index].zoom);
      w_stat[index].zoom2 = EXCEL_GetFloat(fp);
      //printf (" Zoom2: %.02f\n", w_stat[index].zoom2);
      w_stat[index].damage1 = EXCEL_GetFloat(fp);
      //printf (" Primary Damage: %.02f\n", w_stat[index].damage1);
      w_stat[index].damage2 = EXCEL_GetFloat(fp);
      //printf (" Secondary Damage: %.02f\n", w_stat[index].damage2);
      w_stat[index].penetration = EXCEL_GetInt(fp);
      //printf (" Penetration : %i\n", w_stat[index].penetration);
      w_stat[index].rate = EXCEL_GetFloat(fp);
      //printf (" Fire Rate: %.02f\n", w_stat[index].rate);
      w_stat[index].rate2 = EXCEL_GetFloat(fp);
      //printf (" Fire Rate2: %.02f\n", w_stat[index].rate2);
      w_stat[index].speed = EXCEL_GetFloat(fp);
      //printf (" Speed: %.02f\n", w_stat[index].speed);
      }
   fclose(fp);
   return 1;
}
#else
int LoadWeaponMatrix(char *filename)
{
   int i, index, p;
   FILE *fp;
   unsigned char *xx;

   fp = fopen(filename, "rb");
   if (fp)
      {
      fread(&index, 4, 1, fp);
      for (i = 1; i < index; i++)
         {
         fread(&w_stat[i], sizeof(w_stat[i]), 1, fp);
         xx = (unsigned char *) &w_stat[i];
         for (p = 0; p < sizeof(w_stat[i]); p++)
            xx[p] -= 16;
         //printf("--> %s \n", w_stat[i].name);
         }
      fclose(fp);
      return 1;
      }
   return 0;
}
#endif

float EXCEL_GetFloat(FILE *fp)
{
   float f;
   int cell_size;
   unsigned char s[32];
   cell_size = EXCEL_GetNextCell(fp, s);
   if (cell_size)
      sscanf((char *)s, "%f", &f);
   return f;
}

unsigned char EXCEL_GetByte(FILE *fp)
{
   unsigned char b;
   int cell_size;
   unsigned char s[32];
   cell_size = EXCEL_GetNextCell(fp, s);
   if (cell_size)
      sscanf((char *)s, "%c", &b);
   return b;
}

int EXCEL_GetInt(FILE *fp)
{
   int i;
   int cell_size;
   unsigned char s[32];
   cell_size = EXCEL_GetNextCell(fp, s);
   if (cell_size)
      sscanf((char *)s, "%i", &i);
   return i;
}

char *EXCEL_GetString(FILE *fp)
{
   static unsigned char s[32];
   EXCEL_GetNextCell(fp, s);
   return (char *)s;
}

inline int EXCEL_GetNextCell(FILE *fp, unsigned char *x)
{
   int index = 0;
   unsigned char c;
   do
      {
      c = fgetc(fp);
      if (c != '\t' && c != '\n') 
         x[index++] = c;
      else
         break;
      } while (c != '\t' && !feof(fp));
   x[index] = 0;
   if (c == '\n') return -1;
   return index;
}


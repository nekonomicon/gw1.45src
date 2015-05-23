/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_u_mp3.cpp 
   This is the mp3 player module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include <stdio.h>
#include <windows.h>
#include "cvardef.h"

#include "sdk_u_mp3.h"

mp3lib_t mp3lib, mp3theme;
extern cvar_t *cl_beats;
void MyCenterPrint( const char *string );

int DJplaying = 0;
void MP3_LoadTracks()
{
   int i;
   char xxx[128];
   char *temp;
   if (mp3lib.num_tracks)
      return;
   if (mp3theme.num_tracks)
      return;

   mp3theme.track[0] = MP3_Load("gangwars/sound/audio/theme/draw.wav");
   mp3theme.num_tracks++;
   mp3theme.track[1] = MP3_Load("gangwars/sound/audio/theme/eastwin.wav");
   mp3theme.num_tracks++;
   mp3theme.track[2] = MP3_Load("gangwars/sound/audio/theme/westwin.wav");
   mp3theme.num_tracks++;
   for (i = 0; i < 32; i++)
      {
      sprintf(xxx, "gangwars/sound/audio/track%i.wav", i + 1);
      temp = MP3_Load(xxx);
      if (temp)
         mp3lib.track[mp3lib.num_tracks++] = temp;
      else
         return;
      }
}

void MP3_FlushTracks()
{
   int i;
   if (mp3lib.playing) MP3_Stop();
   if (mp3theme.playing) MP3_Theme_Stop();
   
   for (i = 0; i < mp3lib.num_tracks; i++)
      if (mp3lib.track[i]) 
         delete [] mp3lib.track[i];

   for (i = 0; i < mp3theme.num_tracks; i++)
      if (mp3theme.track[i]) 
         delete [] mp3theme.track[i];
   
   memset(&mp3lib, 0, sizeof(mp3lib_t));
   memset(&mp3theme, 0, sizeof(mp3lib_t));
}

void MP3_Init()
{
   memset(&mp3lib, 0, sizeof(mp3lib_t));
   memset(&mp3theme, 0, sizeof(mp3lib_t));
}

char *MP3_Load(char *filename)
{
   char *mp3_memory = NULL;
   FILE *fp;
   int size;
   fp = fopen(filename, "rb");
   if (fp)
      {
      fseek(fp, 0, SEEK_END);
      size = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      mp3_memory = new char[size];
      fread(mp3_memory, size, 1, fp);
      fclose(fp);
      }
   return mp3_memory;
}

void MP3_RandomPlay()
{
   int i;
   if (!cl_beats->value) return;
   if (!mp3lib.num_tracks) return;
   i =  rand() % mp3lib.num_tracks;
   if (mp3lib.track[i])
      {
      mp3lib.playing = 1;
      sndPlaySound(mp3lib.track[i], SND_MEMORY | SND_ASYNC | SND_LOOP);
      }
}

void MP3_Play(int i)
{
   if (!cl_beats->value) return;
   if (i < mp3lib.num_tracks && mp3lib.track[i])
      {
      mp3lib.playing = 1;
      sndPlaySound(mp3lib.track[i], SND_MEMORY | SND_ASYNC | SND_LOOP);
      }
}

void MP3_Stop()
{
   if (!cl_beats->value || mp3lib.playing)
      {
      mp3lib.playing = 0;
      sndPlaySound(NULL, SND_ASYNC);
      }
}


void MP3_Theme_Play(int i)
{
   if (!cl_beats->value) return;
   if (DJplaying) return;
   if (i < mp3theme.num_tracks && mp3theme.track[i])
      {
      mp3theme.playing = 1;
      sndPlaySound(mp3theme.track[i], SND_MEMORY | SND_ASYNC);
      }
}

void MP3_Theme_Stop()
{
   if (!cl_beats->value || mp3theme.playing)
      {
      mp3theme.playing = 0;
      sndPlaySound(NULL, SND_ASYNC);
      }
}

void MP3_DYNPlay(char *filename, int loop)
{
   char xxx[256];
   if (!cl_beats->value) return;
   if (filename[0] == '0' && !filename[1])
      {
      sndPlaySound(NULL, SND_ASYNC);
      DJplaying = 0;
      return;
      }

   sprintf(xxx, "gangwars/sound/audio/theme/%s.wav", filename);
   DJplaying = 1;
   if (loop)
      sndPlaySound(xxx, SND_ASYNC | SND_LOOP);
   else
      sndPlaySound(xxx, SND_ASYNC);
}

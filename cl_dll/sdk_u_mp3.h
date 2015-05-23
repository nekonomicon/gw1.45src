#ifndef SDK_U_MP3_H
#define SDK_U_MP3_H


   #define MAX_MP3_TRACKS     32
   typedef struct mp3lib_s
      {
      int num_tracks;
      int playing;
      char name[32];
      char *track[MAX_MP3_TRACKS];
      } mp3lib_t;

   extern mp3lib_t mp3lib;
      
   char *MP3_Load(char *filename);
   void MP3_RandomPlay();
   void MP3_Play(int i);
   void MP3_Stop();
   void MP3_Theme_Play(int i);
   void MP3_Theme_Stop();
   void MP3_LoadTracks(); 
   void MP3_FlushTracks(); 
   void MP3_Init(); 
   void MP3_DYNPlay(char *filename, int loop);

#endif
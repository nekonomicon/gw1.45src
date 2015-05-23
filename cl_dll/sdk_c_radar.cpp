/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_c_radar.cpp 
   This is the radar module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include < string.h >
#include < stdio.h >
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "in_defs.h"
#include "cvardef.h"
#include "usercmd.h"
#include "sdk_c_radar.h"

DECLARE_MESSAGE(m_Radar, Radar)
#define RADAR_NAME "sprites/radar.spr"
#define SPOT_NAME "sprites/blip.spr"
#define PI_180 (3.14159265358979 / 180.0)
void VectorAngles( const float *forward, float *angles );

vec3_t radar_3d[2];

int CHudRadar::Init(void)
{
   HOOK_MESSAGE(Radar);
   gHUD.AddHudElem(this);
   Reset();
   return 1;
}

void CHudRadar::Reset(void)
{
   m_iFlags &= ~HUD_ACTIVE;
}

int CHudRadar::VidInit(void)
{
   m_hSpriteRadar = 0;
   m_hSpriteSpot = 0;
   m_iFlags |= HUD_ACTIVE;
   return 1;
}

int CHudRadar::MsgFunc_Radar(const char *pszName,  int iSize, void *pbuf )
{
   int flag;
   BEGIN_READ( pbuf, iSize );
   flag = READ_BYTE();
   if (flag == 0)
      m_iFlags &= ~HUD_ACTIVE;  // turn off the Draw function
   else
      {
      if (flag == 1 || flag == 2)
         {
         m_iFlags |= HUD_ACTIVE;   // turn on the Draw function
         origin[flag-1].x = READ_COORD();
         origin[flag-1].y = READ_COORD();
         origin[flag-1].z = READ_COORD();         
         radar_3d[flag-1] = origin[flag-1];
         }
      }
   return 1;
}

extern cvar_t *cl_2dradar;
int CHudRadar::Draw(float flTime)
{
   int x, y;
   int radar_x, radar_y;  // origin of radar sprite
   int pos_x, pos_y;      // positions of the spots
   float radians;
   float dist;
   if (!(m_iFlags & HUD_ACTIVE))
      return 1;

   if (!cl_2dradar->value)
      return 1;

   // make sure we have the right sprite handles
   if (!m_hSpriteRadar)
      {
      m_hSpriteRadar = LoadSprite(RADAR_NAME);
      radar_width = SPR_Width(m_hSpriteRadar, 0);
      radar_height = SPR_Height(m_hSpriteRadar, 0);
      }
   if ( !m_hSpriteSpot )
      m_hSpriteSpot = LoadSprite(SPOT_NAME);

   SPR_Set(m_hSpriteRadar, 0xFF, 0xFF, 0xFF);

   // position the sprite in the upper right corner of display...
   x = 0;
   y = 0;
   SPR_DrawAdditive(0, x, y, NULL);
   //SPR_DrawHoles(0, x, y, NULL);
   vec3_t vv, cc;

   direction = origin[0] - gHUD.m_vecOrigin;
   distance = direction.Length();
   direction.Normalize();
   vv[0] = direction.x; vv[1] = direction.y; vv[2] = direction.z;
   VectorAngles(vv, cc);
   angle.x = cc[0]; angle.y = cc[1]; angle.z = cc[2];
   if (angle.y < 0) angle.y += 360;
   view_angle = gHUD.m_vecAngles.y;
   if (view_angle < 0) view_angle += 360;
   view_angle = (view_angle - angle.y); // * 10;
   if (distance < 2000)
      {
      // calculate center of radar circle (it's actually off by one pixel
      // to the left and up)
      radar_x = x + (radar_width / 2) - 1;
      radar_y = y + (radar_width / 2) - 1;
      radians = view_angle * PI_180; // convert degrees to radians
      // calculate distance from center of circle (max = 1000 units)
      dist = (distance / 2000.0) * (radar_width / 2);
      x = (int)(sin(radians) * dist);
      y = (int)(cos(radians) * dist);
      pos_x = radar_x + x - 2;
      pos_y = radar_y - y - 2;
      SPR_Set(m_hSpriteSpot, 0xFF, 0xFF, 0xFF);//rand() & 0xFF, 0, 0);
      SPR_DrawHoles(0, pos_x, pos_y, NULL);
      }

   x = 0;
   y = 0;
   direction = origin[1] - gHUD.m_vecOrigin;
   distance = direction.Length();
   direction.Normalize();
   vv[0] = direction.x; vv[1] = direction.y; vv[2] = direction.z;
   VectorAngles(vv, cc);
   angle.x = cc[0]; angle.y = cc[1]; angle.z = cc[2];
   if (angle.y < 0) angle.y += 360;
   view_angle = gHUD.m_vecAngles.y;
   if (view_angle < 0) view_angle += 360;
   view_angle = (view_angle - angle.y); // * 10;
   if (distance < 2000)
      {
      // calculate center of radar circle (it's actually off by one pixel
      // to the left and up)
      radar_x = x + (radar_width / 2) - 1;
      radar_y = y + (radar_width / 2) - 1;
      radians = view_angle * PI_180; // convert degrees to radians
      // calculate distance from center of circle (max = 1000 units)
      dist = (distance / 2000.0) * (radar_width / 2);
      x = (int)(sin(radians) * dist);
      y = (int)(cos(radians) * dist);
      pos_x = radar_x + x - 2;
      pos_y = radar_y - y - 2;
      SPR_Set(m_hSpriteSpot, 0xFF, 0x00, 0x00);//rand() & 0xFF, 0, 0);
      SPR_DrawHoles(0, pos_x, pos_y, NULL);
      }
   //SPR_DrawAdditive(0, pos_x, pos_y, NULL);
   return 1;
}

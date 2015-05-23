/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_hud.cpp 
   This is the Gangwars internal HUD module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "hud_servers.h"
#include "ammohistory.h"

#include "eiface.h"

extern float g_lastFOV;
extern int weapon_zoom;

DECLARE_MESSAGE(scope, SetScope)
void AlertMessage( ALERT_TYPE atype, char *szFmt, ... );
int Scope::MsgFunc_SetScope(const char*pszName, int iSize, void *pbuf) 
{
   int iId;
   BEGIN_READ(pbuf, iSize); 
   iId = READ_CHAR();
   weapon_zoom = READ_BYTE();
   WEAPON *pWeapon = gWR.GetWeapon( iId );   
   switch (weapon_zoom)
      {
      case 0:
         gHUD.m_iFOV = 90;
            SetCrosshair(pWeapon->hCrosshair, pWeapon->rcCrosshair, 255, 255, 255);
         break;
      case 1:
         gHUD.m_iFOV = w_stat[iId].zoom1;
      	if (pWeapon)
            SetCrosshair(pWeapon->hZoomedCrosshair, pWeapon->rcZoomedCrosshair, 255, 255, 255);   
         break;
      case 2:
         gHUD.m_iFOV = w_stat[iId].zoom2;
      	if (pWeapon)
				SetCrosshair(pWeapon->hZoomedAutoaim, pWeapon->rcZoomedAutoaim, 255, 255, 255);
         break;
      }
   return 1;
}

int Scope::Init(void) 
{
   HOOK_MESSAGE(SetScope);
   weapon_zoom = 0;
   gHUD.m_iFOV = 90;
   return 1;
}

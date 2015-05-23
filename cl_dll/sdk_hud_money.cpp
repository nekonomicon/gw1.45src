/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_hud_money.cpp 
   This is the money module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "STDIO.H"
#include "STDLIB.H"
#include "MATH.H"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include "sdk_hud_money.h"
#include "sdk_e_menu.h"
#include "sdk_w_armory.h"
#include "sdk_w_matrix.h"

extern int *gNANA;
DECLARE_MESSAGE(m_Money, NANA)
DECLARE_MESSAGE(m_Money, Money)
DECLARE_MESSAGE(m_Money, Armory)
DECLARE_MESSAGE(m_Money, Time)
DECLARE_MESSAGE(m_Money, NewTeam)
DECLARE_MESSAGE(m_Money, GetTeams)
DECLARE_MESSAGE(m_Money, CLAdvert)
DECLARE_MESSAGE(m_Money, CLStat)
DECLARE_MESSAGE(m_Money, CLID)
DECLARE_MESSAGE(m_Money, CLSpawn)
DECLARE_MESSAGE(m_Money, ServerVars)
DECLARE_MESSAGE(m_Money, CLUpdate)

int CHudMoney::Init(void)
{
	HOOK_MESSAGE(NANA);
	HOOK_MESSAGE(Money);
	HOOK_MESSAGE(Armory);
	HOOK_MESSAGE(Time);
	HOOK_MESSAGE(NewTeam);
	HOOK_MESSAGE(GetTeams);
	HOOK_MESSAGE(CLAdvert);
	HOOK_MESSAGE(CLStat);
	HOOK_MESSAGE(CLID);
	HOOK_MESSAGE(CLSpawn);
	HOOK_MESSAGE(ServerVars);
   HOOK_MESSAGE(CLUpdate);
   gHUD.AddHudElem(this);
   Reset();
   strcpy(s_top.data, "GangWars 1.45 - Check out http://www.gangwars.net/ for updates");
   s_top.length = strlen(s_top.data) * 10;
   s_top.index = 0;
   strcpy(s_bottom.data, "Please stay tuned. Stats will be available to you shortly");
   s_bottom.length = strlen(s_bottom.data) * 7;
   s_bottom.index = 0;
   gNANA = &m_iNANA;
	return 1;
}

void CHudMoney::Reset( void )
{
   gHUD.m_iFOV = 90;
   m_iNANA = 0;
   m_iNANAindex = 0;
   m_iNearArmory = 1;
	m_fFade = 0;
	m_iFlags = 0;
   m_fTimeLeft = 0;
   m_fBuyTimeLeft = 0;
   m_iID = 0xFF;
   m_iFlags |= HUD_ACTIVE;
}

int CHudMoney::VidInit(void)
{
	m_hSprite = 0;
	m_HUD_dollar = gHUD.GetSpriteIndex( "dollar" );
	return 1;
}

int CHudMoney:: MsgFunc_NANA(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iNANA = READ_BYTE();
   m_iNANAindex = READ_BYTE();
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudMoney:: MsgFunc_Money(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	unsigned int x = READ_SHORT();
	m_iFlags |= HUD_ACTIVE;
	// Only update the fade if we've changed money
	if (x != (unsigned) m_iMoney)
	   {
		m_fFade = FADE_TIME;
		m_iMoney = x;
	   }
	return 1;
}

int CHudMoney:: MsgFunc_Armory(const char *pszName,  int iSize, void *pbuf )
{
   int last_NearArmory = m_iNearArmory;
	BEGIN_READ( pbuf, iSize );
	m_iNearArmory = READ_BYTE();
	m_iFlags |= HUD_ACTIVE;

   if (last_NearArmory && !m_iNearArmory)
      {      
      gHUD.m_Menu.m_fMenuDisplayed = 0;
		gHUD.m_Menu.m_iFlags &= ~HUD_ACTIVE;
      }
	return 1;
}

int CHudMoney:: MsgFunc_Time(const char *pszName,  int iSize, void *pbuf )
{
   float x;
	BEGIN_READ( pbuf, iSize );
	x = READ_WORD();
   x /= 10;
   m_fTimeLeft = x + gEngfuncs.GetClientTime();
	x = READ_WORD();
   x /= 10;
   m_fBuyTimeLeft = x + gEngfuncs.GetClientTime();
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudMoney:: MsgFunc_CLSpawn(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	unsigned int client_index = READ_BYTE();
	unsigned int respaner_index = READ_BYTE();
	unsigned int team_index = READ_BYTE();
   char x[128];
   sprintf( x, "*** %s was respawned by %s ***\n",
            g_PlayerInfoList[client_index].name,
            g_PlayerInfoList[respaner_index].name);

   PlaySound( "respawn.wav", .6f );
   gHUD.m_SayText.SayTextPrint(x, strlen(x), client_index, team_index, 0);
	return 1;
}

int CHudMoney:: MsgFunc_ServerVars(const char *pszName,  int iSize, void *pbuf )
{
   unsigned short int recoil, k;
   float ff;
	BEGIN_READ( pbuf, iSize );
	recoil = READ_SHORT();
   
   ff = recoil;
   ff /= 1000.0f;

   if (ff != 1)
      {
      for (k = 0; k < MAX_WEAPONS; k++)
         {
         w_stat[k].recoil_pitch *= ff;
         w_stat[k].recoil_yaw *= ff;
         }
      }
   //char x[128];
   //sprintf( x, "*** recoil set %.2f ***\n", ff);
   //gHUD.m_SayText.SayTextPrint(x, strlen(x), 1, 1, 0);  
	return 1;
}

int CHudMoney:: MsgFunc_CLUpdate(const char *pszName,  int iSize, void *pbuf )
{
   int clientindex;
   
   /*
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	   {
		GetPlayerInfo( i, &g_PlayerInfoList[i] );

		if ( g_PlayerInfoList[i].thisplayer )
			m_pScoreBoard->m_iPlayerNum = i;  // !!!HACK: this should be initialized elsewhere... maybe gotten from the engine
	   }
   */

	BEGIN_READ( pbuf, iSize );
	clientindex = READ_BYTE();
   GetPlayerInfo( clientindex, &g_PlayerInfoList[clientindex] );
   char xxx[128];
   xxx[0] = 1;
   sprintf( xxx + 1, "*** %s has connected! ***\n", g_PlayerInfoList[clientindex].name);
   ConsolePrint( xxx );
	return 1;
}


int CHudMoney::Draw(float flTime)
{
   char xout[128], name[128];
	int r, g, b;
	int a = 0, x, y;
	int MoneyWidth;
   float currtime = gEngfuncs.GetClientTime();

   if (m_fTimeLeft)
      {
      y = ScreenHeight - 60;
      char xout[32] = {0};
      int mins, secs, critical = 0;
      char canbuy = 0;
      mins = (m_fBuyTimeLeft - currtime);
      if (mins > 0)
         canbuy = 1;
      mins = (m_fTimeLeft - currtime);
      if (mins <= 0)
         {
         critical = -1;
         mins = 0;
         }
      secs = mins % 60;
      mins /= 60;
      if (!mins && secs < 30 && secs)
         critical = 1;
      if (canbuy)
         sprintf(xout, "%3d:%02d A", mins, secs);
      else
         sprintf(xout, "%3d:%02d", mins, secs);
      if (critical == 1)
         gHUD.DrawHudString(10 + (rand() & 3), 
                            y + (rand() & 3), 
                            320, xout, 0x80 + (rand() & 0x7F), 0x0, 0x40 );
      else if (!critical)
         gHUD.DrawHudString(10,
                            y, 
                            320, xout, HUD_FG_RED, HUD_FG_GREEN, HUD_FG_BLUE ); 
      else 
         gHUD.DrawHudString(10,
                            y, 
                            320, "Round Over", 0xFF, 0x00, 0x00 ); 
      }
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;
	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
	   return 1;

	// Has health changed? Flash the health #
	if (m_fFade)
	   {
		m_fFade -= (gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
		   {
			a = MIN_ALPHA;
			m_fFade = 0;
		   }

		// Fade the health number back to dim

		a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;
	   }
	else
		a = MIN_ALPHA;

	// Money < 1400 has lower intensity:
	if (m_iMoney <= 1400)
		a = 64;
   else
      a = 127;
		

   // Blue Money:
   if (!m_iNearArmory)
      {r = HUD_FG_RED; g = HUD_FG_GREEN; b = HUD_FG_BLUE;}
   else
      {r = 5; g = 165; b = 118; a *= 2;}
	ScaleColors(r, g, b, a );

   MoneyWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;
	int DollarWidth = gHUD.GetSpriteRect(m_HUD_dollar).right - gHUD.GetSpriteRect(m_HUD_dollar).left;

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = DollarWidth / 2 + (ScreenWidth / 2);

   //x -= (gHUD.GetNumWidth( m_iAmmoAmounts[i], DHN_DRAWZERO ) * AmmoWidth);

	SPR_Set(gHUD.GetSprite(m_HUD_dollar), r, g, b);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_dollar));

	x = DollarWidth + MoneyWidth / 2 + (ScreenWidth / 2);
   x = gHUD.DrawHudNumber(x, y, DHN_DRAWZERO, m_iMoney, r, g, b); //DHN_3DIGITS | 


   /*
   x += MoneyWidth/2;
	int iHeight = gHUD.m_iFontHeight;
	int iWidth = MoneyWidth/10;
	FillRGBA(x, y, iWidth, iHeight, 255, 160, 0, a);
   */

   if (m_iID >= 1 && m_iID < MAX_PLAYERS)
      {
      if (g_PlayerInfoList[m_iID].name)
         {
         strcpy(name, g_PlayerInfoList[m_iID].name);
         name[14] = 0;
         if (m_iHealth < 101 && m_iHealth > 0)
            sprintf(xout, "%s - h:%02i j:%02i", name, m_iHealth, m_iJuice);
         else
            sprintf(xout, "%s - j:%02i", name, m_iJuice);
         if (g_PlayerExtraInfo[m_iID].teamnumber == 1)
            gHUD.DrawHudString(120, ScreenHeight - 30, ScreenWidth, xout, EAST_RED, EAST_GREEN, EAST_BLUE);
         else if (g_PlayerExtraInfo[m_iID].teamnumber == 2)
            gHUD.DrawHudString(120, ScreenHeight - 30, ScreenWidth, xout, WEST_RED, WEST_GREEN, WEST_BLUE); 
         }
      }

   if (m_iNANA)
      {
      if (g_PlayerInfoList[m_iNANAindex].name /*&& g_PlayerInfoList[m_iNANAindex].name[0]*/)
         {
         strcpy(name, g_PlayerInfoList[m_iNANAindex].name);
         name[24] = 0;
         y = 0;
         x = 0;
         while (name[y])
            {
            x += gHUD.m_scrinfo.charWidths[name[y]];
            y++;
            }
         sprintf(xout, "%s", name);
         gHUD.DrawHudString(ScreenWidth - x - 10, ScreenHeight - 60, ScreenWidth, xout, FALSE_RED, FALSE_GREEN >> 4, FALSE_BLUE >> 4); 
         }
      }

   return 1;
}

int CHudMoney:: MsgFunc_NewTeam(const char *pszName,  int iSize, void *pbuf )
{
   int clientindex, clientteam;
	BEGIN_READ( pbuf, iSize );
   clientindex = READ_BYTE();
   clientteam = READ_BYTE();
   if ( clientindex > 0 && clientindex <= MAX_PLAYERS )
      g_PlayerExtraInfo[clientindex].teamnumber = clientteam;
	return 1;
}

int CHudMoney:: MsgFunc_GetTeams(const char *pszName,  int iSize, void *pbuf )
{
   int clientindex, clientteam, num_clients, i;
	BEGIN_READ( pbuf, iSize );
   num_clients = READ_BYTE();
   for (i = 0; i < num_clients; i++)
      {
      clientindex = READ_BYTE();
      clientteam = READ_BYTE();
      if ( clientindex > 0 && clientindex <= MAX_PLAYERS )
         g_PlayerExtraInfo[clientindex].teamnumber = clientteam;
      }
	return 1;
}

char *GetCurrentStat(int messagetype);
int CHudMoney:: MsgFunc_CLAdvert(const char *pszName,  int iSize, void *pbuf )
{
   int message_type;
	BEGIN_READ( pbuf, iSize );
   message_type = READ_BYTE();
   strcpy(s_top.data, READ_STRING());
   s_top.length = strlen(s_top.data) * 10;
   s_top.index = message_type;
   s_top.x = ScreenWidth + 90;
	return 1;
}

int CHudMoney:: MsgFunc_CLStat(const char *pszName,  int iSize, void *pbuf )
{
   int message_type;
	BEGIN_READ( pbuf, iSize );
   message_type = READ_BYTE();
   strcpy(s_bottom.data, GetCurrentStat(message_type));
   s_bottom.length = strlen(s_bottom.data) * 7;
   s_bottom.index = message_type;
   s_bottom.x = ScreenWidth + 90;
	return 1;
}

int CHudMoney:: MsgFunc_CLID(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
   m_iID = READ_BYTE();
   if (m_iID != 0xFF)
      {
      m_iHealth = READ_BYTE();
      m_iJuice = READ_WORD();
      }
	return 1;
}

/*
void CHudHealth::UpdateTiles(float flTime, long bitsDamage)
{	
	DAMAGE_IMAGE *pdmg;

	// Which types are new?
	long bitsOn = ~m_bitsDamage & bitsDamage;
	
	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		pdmg = &m_dmg[i];

		// Is this one already on?
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE; // extend the duration
			if (!pdmg->fBaseline)
				pdmg->fBaseline = flTime;
		}

		// Are we just turning it on?
		if (bitsOn & giDmgFlags[i])
		{
			// put this one at the bottom
			pdmg->x = giDmgWidth/8;
			pdmg->y = ScreenHeight - giDmgHeight * 2;
			pdmg->fExpire=flTime + DMG_IMAGE_LIFE;
			
			// move everyone else up
			for (int j = 0; j < NUM_DMG_TYPES; j++)
			{
				if (j == i)
					continue;

				pdmg = &m_dmg[j];
				if (pdmg->y)
					pdmg->y -= giDmgHeight;

			}
			pdmg = &m_dmg[i];
		}	
	}	

	// damage bits are only turned on here;  they are turned off when the draw time has expired (in DrawDamage())
	m_bitsDamage |= bitsDamage;
}
*/

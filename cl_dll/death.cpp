/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// death notice
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "vgui_TeamFortressViewport.h"
#include "sdk_w_matrix.h"
#include "sdk_e_menu.h"

DECLARE_MESSAGE( m_DeathNotice, DeathMsg );

struct DeathNoticeItem {
	char szKiller[MAX_PLAYER_NAME_LENGTH*2];
	char szVictim[MAX_PLAYER_NAME_LENGTH*2];
   char szWhere[MAX_PLAYER_NAME_LENGTH*2];
   char szHow[MAX_PLAYER_NAME_LENGTH*2];
	int iId;	// the index number of the associated sprite
	int iSuicide;
	int iTeamKill;
	int iNonPlayerKill;
	float flDisplayTime;
	float *KillerColor;
	float *VictimColor;
};

#define MAX_DEATHNOTICES	4
static int DEATHNOTICE_DISPLAY_TIME = 6;

#define DEATHNOTICE_TOP		20

DeathNoticeItem rgDeathNoticeList[ MAX_DEATHNOTICES + 1 ];

float g_ColorEast[3] = {EAST_RED / 255.0f, EAST_GREEN / 255.0f, EAST_BLUE / 255.0f};
float g_ColorWest[3] = {WEST_RED / 255.0f, WEST_GREEN / 255.0f, WEST_BLUE / 255.0f};
float g_ColorFalse[3] = {FALSE_RED / 255.0f, FALSE_GREEN / 255.0f, FALSE_BLUE / 255.0f};
float g_ColorBlue[3]	= { 0.6, 0.8, 1.0 };
float g_ColorRed[3]		= { 1.0, 0.25, 0.25 };
float g_ColorGreen[3]	= { 0.6, 1.0, 0.6 };
float g_ColorYellow[3]	= { 1.0, 0.7, 0.0 };
float g_ColorWhite[3]	= { 1.0, 1.0, 1.0 };

float *GetClientColor( int clientIndex )
{
	switch ( g_PlayerExtraInfo[clientIndex].teamnumber )
	   {
	   case 1:  return g_ColorEast;
	   case 2:  return g_ColorWest;
	   }
   return g_ColorFalse;
	//return NULL;
}

float *GetTeamColor( int teamIndex )
{
	switch ( teamIndex )
	   {
	   case 1:	return g_ColorEast;
	   case 2:  return g_ColorWest;
	   }
	return g_ColorFalse;
}

int CHudDeathNotice :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( DeathMsg );

	CVAR_CREATE( "hud_deathnotice_time", "6", 0 );

	return 1;
}


void CHudDeathNotice :: InitHUDData( void )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}


int CHudDeathNotice :: VidInit( void )
{
	m_HUD_d_skull = gHUD.GetSpriteIndex( "d_skull" );

	return 1;
}

int CHudDeathNotice :: Draw( float flTime )
{
	int x, y; // r, g, b;

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	   {
		if ( rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		if ( rgDeathNoticeList[i].flDisplayTime < flTime )
		   {
			memmove( &rgDeathNoticeList[i], &rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--; 
			continue;
		   }

		rgDeathNoticeList[i].flDisplayTime = min( rgDeathNoticeList[i].flDisplayTime, gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME );

		// Only draw if the viewport will let me
		if ( gViewPort && gViewPort->AllowedToPrintText() )
		   {
			// Draw the death notice
			y = DEATHNOTICE_TOP + (20 * i);

			x = ScreenWidth - ConsoleStringLen(rgDeathNoticeList[i].szVictim);
         if (rgDeathNoticeList[i].szHow[0])
            x -= (5 + ConsoleStringLen(rgDeathNoticeList[i].szHow));
         if (rgDeathNoticeList[i].szWhere[0])
            x -= (5 + ConsoleStringLen(rgDeathNoticeList[i].szWhere));

			if (!rgDeathNoticeList[i].iSuicide)
			   {
				x -= (5 + ConsoleStringLen( rgDeathNoticeList[i].szKiller ) );
				if ( rgDeathNoticeList[i].KillerColor )
					gEngfuncs.pfnDrawSetTextColor( rgDeathNoticeList[i].KillerColor[0], rgDeathNoticeList[i].KillerColor[1], rgDeathNoticeList[i].KillerColor[2] );
				x = 5 + DrawConsoleString( x, y, rgDeathNoticeList[i].szKiller );
			   }

         gEngfuncs.pfnDrawSetTextColor(.6, .6, .6);
         if (rgDeathNoticeList[i].szHow[0])
			   x = 5 + DrawConsoleString( x, y, rgDeathNoticeList[i].szHow);

			if (rgDeathNoticeList[i].iNonPlayerKill == FALSE)
			   {
				if ( rgDeathNoticeList[i].VictimColor )
					gEngfuncs.pfnDrawSetTextColor( rgDeathNoticeList[i].VictimColor[0], rgDeathNoticeList[i].VictimColor[1], rgDeathNoticeList[i].VictimColor[2] );
				x = 5 + DrawConsoleString( x, y, rgDeathNoticeList[i].szVictim );
			   }
         if (rgDeathNoticeList[i].szWhere[0])
            {
            gEngfuncs.pfnDrawSetTextColor(.6, .6, .6);
		      DrawConsoleString( x, y, rgDeathNoticeList[i].szWhere);
            }
		   }
	   }

	return 1;
}

// This message handler may be better off elsewhere
int CHudDeathNotice :: MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );

	int killer = READ_BYTE();
	int victim = READ_BYTE();
	int medium = READ_BYTE();
	int location = READ_BYTE();

	strcpy( killedwith, "-" );
	strcpy( killedwhere, "" );

	if (gViewPort)
		gViewPort->DeathMsg( killer, victim );

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	   {
		if ( rgDeathNoticeList[i].iId == 0 )
			break;
	   }
	if ( i == MAX_DEATHNOTICES )
	   { // move the rest of the list forward to make room for this item
		memmove( rgDeathNoticeList, rgDeathNoticeList+1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	   }

	if (gViewPort)
		gViewPort->GetAllPlayersInfo();

	// Get the Killer's name
	char *killer_name = g_PlayerInfoList[ killer ].name;
	if ( !killer_name )
	   {
		killer_name = "";
		rgDeathNoticeList[i].szKiller[0] = 0;
	   }
	else
	   {
		rgDeathNoticeList[i].KillerColor = GetClientColor( killer );
		strncpy( rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
	   }

	// Get the Victim's name
	char *victim_name = NULL;
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if ( ((char)victim) != -1 )
      {
		victim_name = g_PlayerInfoList[ victim ].name;
	   if ( !victim_name )
	      {
		   victim_name = "";
		   rgDeathNoticeList[i].szVictim[0] = 0;
	      }
	   else
	      {
		   rgDeathNoticeList[i].VictimColor = GetClientColor( victim );
		   strncpy( rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );
		   rgDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
	      }
      if ( killer == victim || killer == 0 )
		   rgDeathNoticeList[i].iSuicide = TRUE;

      if (medium == D_MEDIUM_TEAMMATE)
         rgDeathNoticeList[i].iTeamKill = TRUE;
      }
   else 
	   {
		rgDeathNoticeList[i].iNonPlayerKill = TRUE;
      strcpy( rgDeathNoticeList[i].szVictim, "Unknown" );
      }

   rgDeathNoticeList[i].iId = 1;
	DEATHNOTICE_DISPLAY_TIME = CVAR_GET_FLOAT( "hud_deathnotice_time" );
	rgDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME;

	if (rgDeathNoticeList[i].iNonPlayerKill)
	   {
		ConsolePrint( rgDeathNoticeList[i].szKiller );
		ConsolePrint( " killed a " );
		ConsolePrint( rgDeathNoticeList[i].szVictim );
		ConsolePrint( "\n" );
	   }
	else
	   {
		// record the death notice in the console
		if ( rgDeathNoticeList[i].iSuicide )
		   {
			ConsolePrint( rgDeathNoticeList[i].szVictim );
         strcpy( killedwith, "suicide" );
			ConsolePrint( " killed self" );
		   }
		else if ( rgDeathNoticeList[i].iTeamKill )
		   {
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed his teammate " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		   }
		else
		   {
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		   }
      if (medium >= 0 && medium < WEAPON_KNIFE)
         strcpy(killedwith, (char *)w_stat[medium].name);
      else if (medium == WEAPON_KNIFE)
         strcpy(killedwith, "knife");
      switch (location)
         {
         case HITGROUP_HEAD:
            strcpy(killedwhere, "head");
            break;
         case HITGROUP_CHEST:
            strcpy(killedwhere, "chest");
            break;
         case HITGROUP_STOMACH:
            strcpy(killedwhere, "stomach");
            break;
         case HITGROUP_LEFTARM:
            strcpy(killedwhere, "left arm");
            break;
         case HITGROUP_RIGHTARM:
            strcpy(killedwhere, "right arm");
            break;
         case HITGROUP_LEFTLEG:
            strcpy(killedwhere, "left leg");
            break;
         case HITGROUP_RIGHTLEG:
            strcpy(killedwhere, "right leg");
            break;
         }
      
      if (killedwith && *killedwith)
         strcpy(rgDeathNoticeList[i].szHow, killedwith);
      else
         rgDeathNoticeList[i].szHow[0] = 0;
      if (killedwhere && *killedwhere)
         strcpy(rgDeathNoticeList[i].szWhere, killedwhere);
      else
         rgDeathNoticeList[i].szWhere[0] = 0;
      //sprintf(killedwhere, "- %i", medium);

      if (killedwith && *killedwith && !rgDeathNoticeList[i].iSuicide/*&& !rgDeathNoticeList[i].iTeamKill*/)
         {
         ConsolePrint( " with " );
         ConsolePrint( killedwith ); // skip over the "d_" part
         if (killedwhere)
            {
            ConsolePrint( " to the " ); // skip over the "d_" part
            ConsolePrint( killedwhere ); // skip over the "d_" part
            }
         }
      if (location != HITGROUP_HEAD)
         {
         killedwhere[0] = 0;
         rgDeathNoticeList[i].szWhere[0] = 0;
         }
		ConsolePrint( "\n" );
	   }  
	return 1;
}





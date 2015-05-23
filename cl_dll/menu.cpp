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
// menu.cpp
//
// generic menu handler
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "vgui_TeamFortressViewport.h"
#include "sdk_e_menu.h"

#define STATUS_PLAYER_CHOOSETEAM          1
#define STATUS_PLAYER_CHOOSEGROUP         2
#define STATUS_PLAYER_BUYWEAPONMENU       3
#define STATUS_PLAYER_BUYPISTOLMENU       4
#define STATUS_PLAYER_BUYSHOTGUNMENU      5
#define STATUS_PLAYER_BUYSUBMENU          6
#define STATUS_PLAYER_BUYAUTOMENU         7
#define STATUS_PLAYER_BUYRIFLEMENU        8
#define STATUS_PLAYER_BUYSPECIALTYGUNMENU 9
#define STATUS_PLAYER_BUYITEMMENU         10
#define STATUS_PLAYER_STATISTICSMENU      11
#define STATUS_PLAYER_HISTORYMENU         12
#define STATUS_PLAYER_CONFIGMENU          13
#define STATUS_PLAYER_AUDIOMENU1          14
#define STATUS_PLAYER_AUDIOMENU2          15

#define MAX_MENU_STRING	1024
char g_szMenuString[MAX_MENU_STRING];
char g_szPrelocalisedMenuString[MAX_MENU_STRING];

int KB_ConvertString( char *in, char **ppout );

DECLARE_MESSAGE( m_Menu, ShowMenu );
DECLARE_MESSAGE( m_Menu, CSOSMenu );

int CHudMenu :: Init( void )
{
   memset(&aw, 0, sizeof(aw));
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( ShowMenu );
	HOOK_MESSAGE( CSOSMenu );

	m_fMenuDisplayed = 0;
	m_bitsValidSlots = 0;
	InitHUDData();

	return 1;
}

void CHudMenu :: InitHUDData( void )
{
	Reset();
}

void CHudMenu :: Reset( void )
{
	g_szPrelocalisedMenuString[0] = 0;
	m_fWaitingForMore = FALSE;
}

int CHudMenu :: VidInit( void )
{
	return 1;
}

static int ColorX[10] = { 0,  26,  52,  76, 102, 
                        128, 154, 180, 204, 255};

int CHudMenu :: Draw( float flTime )
{
   static char temp[MAX_MENU_STRING] = {0};
   static int tup = 0;
   bool tabbed = 0;
   bool done = false;
	// check for if menu is set to disappear
	if ( m_flShutoffTime > 0 )
	   {
		if ( m_flShutoffTime <= gHUD.m_flTime )
		   {  // times up, shutoff
			m_fMenuDisplayed = 0;
			m_iFlags &= ~HUD_ACTIVE;
			return 1;
		   }
	   }

	// don't draw the menu if the scoreboard is being shown
	if ( gViewPort && gViewPort->IsScoreBoardVisible() )
		return 1;


	// draw the menu, along the left-hand side of the screen

	// count the number of newlines
	int nlc = 0;
	for ( int i = 0; i < MAX_MENU_STRING && g_szMenuString[i] != '\0'; i++ )
	   {
		if ( g_szMenuString[i] == '\n' )
			nlc++;
	   }

   // center it
	int y = (ScreenHeight/2) - ((nlc/2)*12) - 40; // make sure it is above the say text
	int x = 20;

	i = 0;
   int Blue = 255, Red = 255, Green = 255, j = 0, k = 0;
	while ( j < MAX_MENU_STRING && g_szMenuString[j] != '\0' )
	   {      
      k = i;
		while ( j < MAX_MENU_STRING && g_szMenuString[j] != '\0' && g_szMenuString[j] != '\n' )
         {
         if (g_szMenuString[j] == '^')
            {
            if (g_szMenuString[j + 1] == 'e')
               {
               Red = EAST_RED;
               Green = EAST_GREEN;
               Blue = EAST_BLUE;
               j++;
               }
            else if (g_szMenuString[j + 1] == 'w')
               {
               Red = WEST_RED;
               Green = WEST_GREEN;
               Blue = WEST_BLUE;
               j++;
               }
            else if (g_szMenuString[j + 1] == 't')
               {
               Red = TRUE_RED;
               Green = TRUE_GREEN;
               Blue = TRUE_BLUE;
               j++;
               }
            else if (g_szMenuString[j + 1] == 'f')
               {
               Red = FALSE_RED;
               Green = FALSE_GREEN;
               Blue = FALSE_BLUE;
               j++;
               }
            else
               {
               j++; if (g_szMenuString[j] == '*') Red = rand() & 255;
                    else Red = ColorX[g_szMenuString[j] - '0'];
               j++; if (g_szMenuString[j] == '*') Green = rand() & 255;
                    else Green = ColorX[g_szMenuString[j] - '0'];
               j++; if (g_szMenuString[j] == '*') Blue = rand() & 255;
                    else Blue = ColorX[g_szMenuString[j] - '0'];
               }
            j++; // Skip space
            }
         else if (g_szMenuString[j] == '\t')
            {
            j++; tabbed = true;
            temp[i] = 0; i++;
            break;
            }
         else
            {
            temp[i] = g_szMenuString[j];
			   i++;
            }
         j++;
         }
      if (!tabbed) temp[i] = g_szMenuString[j];
		if (g_szMenuString[j] == '\n') j++;
		if (temp[i] == '\n') i++;
      if (!done)
         {
         if (tup <= i - k) temp[k + tup] = toupper(temp[k + tup]);
         done = true;
		   gHUD.DrawHudString( x + (rand() & 3), y + (rand() & 3), 480, temp + k, Red, Green, Blue );
         }
      else
         {
         gHUD.DrawHudString( x, y, 480, temp + k, Red, Green, Blue );
         }
      if (!tabbed) {y += 12; x = 20;}
      else {tabbed = false; x = 20 + (10 * 20);}
   	}

   if ((rand() & 3) == 1)
      {  
      tup++;
      if (tup > 48)
         tup = 0;
      }

	return 1;
}

// selects an item from the menu
void CHudMenu :: SelectMenuItem( int menu_item )
{
	// if menu_item is in a valid slot,  send a menuselect command to the server
   if ((menu_item > 0) && m_bitsValidSlots & (1 << (menu_item-1)))
	   {
		char szbuf[32];
		sprintf( szbuf, "menuselect %d\n", menu_item );
		ClientCmd( szbuf );

		// remove the menu
		m_fMenuDisplayed = 0;
		m_iFlags &= ~HUD_ACTIVE;
	   }
}


// Message handler for ShowMenu message
// takes four values:
//		short: a bitfield of keys that are valid input
//		char : the duration, in seconds, the menu should stay up. -1 means is stays until something is chosen.
//		byte : a boolean, TRUE if there is more string yet to be received before displaying the menu, FALSE if it's the last string
//		string: menu string to display
// if this message is never received, then scores will simply be the combined totals of the players.
#define CANCEL_BIT   (1<<9)
int CHudMenu :: MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf )
{
	char *temp = NULL;

	BEGIN_READ( pbuf, iSize );

	m_bitsValidSlots = READ_SHORT();
	int DisplayTime = READ_CHAR();
	int NeedMore = READ_BYTE();

	if ( DisplayTime > 0 )
		m_flShutoffTime = DisplayTime + gHUD.m_flTime;
	else
		m_flShutoffTime = -1;

	if ( m_bitsValidSlots )
	{
		if ( !m_fWaitingForMore ) // this is the start of a new menu
		{
			strncpy( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING );
		}
		else
		{  // append to the current menu string
			strncat( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING - strlen(g_szPrelocalisedMenuString) );
		}
		g_szPrelocalisedMenuString[MAX_MENU_STRING-1] = 0;  // ensure null termination (strncat/strncpy does not)

		if ( !NeedMore )
		{  // we have the whole string, so we can localise it now
			strcpy( g_szMenuString, gHUD.m_TextMessage.BufferedLocaliseTextString( g_szPrelocalisedMenuString ) );

			// Swap in characters
			if ( KB_ConvertString( g_szMenuString, &temp ) )
			{
				strcpy( g_szMenuString, temp );
				free( temp );
			}
		}

		m_fMenuDisplayed = 1;
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		m_fMenuDisplayed = 0; // no valid slots means that the menu should be turned off
		m_iFlags &= ~HUD_ACTIVE;
	}

   if (m_bitsValidSlots & CANCEL_BIT)
      strcat(g_szMenuString, "\n\n^555 0: Exit menu\n");

	m_fWaitingForMore = NeedMore;

	return 1;
}

int CHudMenu :: MsgFunc_CSOSMenu( const char *pszName, int iSize, void *pbuf )
{
	char *temp = NULL;
   short menutype, team;

	BEGIN_READ( pbuf, iSize );

   menutype = READ_BYTE();
   team = READ_BYTE();
	m_flShutoffTime = -1;

   switch (menutype)
      {  
      case STATUS_PLAYER_CHOOSETEAM:
         {
         int thugs_e = READ_BYTE();
         int thugs_w = READ_BYTE();
         DisplayTeamMenu(g_szMenuString, (short *)&m_bitsValidSlots, team, thugs_e, thugs_w);
         break;
         }
      case STATUS_PLAYER_CHOOSEGROUP:
         DisplayGroupMenu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_BUYWEAPONMENU:
         aw.pistols = READ_BYTE();
         aw.shotguns = READ_BYTE();
         aw.subs = READ_BYTE();
         aw.autos = READ_BYTE();
         DisplayBuyWeaponMenu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_BUYPISTOLMENU:
         DisplayBuyPistolMenu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_BUYSHOTGUNMENU:
         DisplayBuyShotgunMenu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_BUYSUBMENU:
         DisplayBuySubMenu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_BUYAUTOMENU:
         DisplayBuyAutoMenu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_AUDIOMENU1:
         DisplayAudio1Menu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      case STATUS_PLAYER_AUDIOMENU2:
         DisplayAudio2Menu(g_szMenuString, (short *)&m_bitsValidSlots, team);
         break;
      default:
         break;
      }
   
	if ( m_bitsValidSlots )
	   {
		m_fMenuDisplayed = 1;
		m_iFlags |= HUD_ACTIVE;
	   }
   else
      {
		m_fMenuDisplayed = 0;
		m_iFlags &= ~HUD_ACTIVE;
      }

   if (m_bitsValidSlots & CANCEL_BIT)
      strcat(g_szMenuString, "\n\n^555 0: Exit menu\n");
	return 1;
}

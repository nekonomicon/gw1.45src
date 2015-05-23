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
// Message.cpp
//
// implementation of CHudMessage class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "sdk_e_menu.h"

DECLARE_MESSAGE( m_Message, HudText )
DECLARE_MESSAGE( m_Message, GameTitle )
DECLARE_MESSAGE( m_Message, Title)

// 1 Global client_textmessage_t for custom messages that aren't in the titles.txt
client_textmessage_t	g_pCustomMessage;
char *g_pCustomName = "Custom";
char g_pCustomText[1024];

int CHudMessage::Init(void)
{
	HOOK_MESSAGE( HudText );
	HOOK_MESSAGE( GameTitle );
	HOOK_MESSAGE(Title);

	gHUD.AddHudElem(this);
   strcpy(m_TitleText, "This is a test");

	Reset();
   m_iFlags = HUD_ACTIVE;

	return 1;
};

int CHudMessage::VidInit( void )
{
	m_HUD_title_half = gHUD.GetSpriteIndex( "title_half" );
	m_HUD_title_life = gHUD.GetSpriteIndex( "title_life" );

	return 1;
};


void CHudMessage::Reset( void )
{
 	memset( m_pMessages, 0, sizeof( m_pMessages[0] ) * maxHUDMessages );
	memset( m_startTime, 0, sizeof( m_startTime[0] ) * maxHUDMessages );
	
	m_gameTitleTime = 0;
	m_pGameTitle = NULL;
}


float CHudMessage::FadeBlend( float fadein, float fadeout, float hold, float localTime )
{
	float fadeTime = fadein + hold;
	float fadeBlend;

	if ( localTime < 0 )
		return 0;

	if ( localTime < fadein )
	{
		fadeBlend = 1 - ((fadein - localTime) / fadein);
	}
	else if ( localTime > fadeTime )
	{
		if ( fadeout > 0 )
			fadeBlend = 1 - ((localTime - fadeTime) / fadeout);
		else
			fadeBlend = 0;
	}
	else
		fadeBlend = 1;

	return fadeBlend;
}


int	CHudMessage::XPosition( float x, int width, int totalWidth )
{
	int xPos;

	if ( x == -1 )
	{
		xPos = (ScreenWidth - width) / 2;
	}
	else
	{
		if ( x < 0 )
			xPos = (1.0 + x) * ScreenWidth - totalWidth;	// Alight right
		else
			xPos = x * ScreenWidth;
	}

	if ( xPos + width > ScreenWidth )
		xPos = ScreenWidth - width;
	else if ( xPos < 0 )
		xPos = 0;

	return xPos;
}


int CHudMessage::YPosition( float y, int height )
{
	int yPos;

	if ( y == -1 )	// Centered?
		yPos = (ScreenHeight - height) * 0.5;
	else
	{
		// Alight bottom?
		if ( y < 0 )
			yPos = (1.0 + y) * ScreenHeight - height;	// Alight bottom
		else // align top
			yPos = y * ScreenHeight;
	}

	if ( yPos + height > ScreenHeight )
		yPos = ScreenHeight - height;
	else if ( yPos < 0 )
		yPos = 0;

	return yPos;
}


void CHudMessage::MessageScanNextChar( void )
{
	int srcRed, srcGreen, srcBlue, destRed, destGreen, destBlue;
	int blend;

	srcRed = m_parms.pMessage->r1;
	srcGreen = m_parms.pMessage->g1;
	srcBlue = m_parms.pMessage->b1;
	blend = 0;	// Pure source

	switch( m_parms.pMessage->effect )
	{
	// Fade-in / Fade-out
	case 0:
	case 1:
		destRed = destGreen = destBlue = 0;
		blend = m_parms.fadeBlend;
		break;

	case 2:
		m_parms.charTime += m_parms.pMessage->fadein;
		if ( m_parms.charTime > m_parms.time )
		{
			srcRed = srcGreen = srcBlue = 0;
			blend = 0;	// pure source
		}
		else
		{
			float deltaTime = m_parms.time - m_parms.charTime;

			destRed = destGreen = destBlue = 0;
			if ( m_parms.time > m_parms.fadeTime )
			{
				blend = m_parms.fadeBlend;
			}
			else if ( deltaTime > m_parms.pMessage->fxtime )
				blend = 0;	// pure dest
			else
			{
				destRed = m_parms.pMessage->r2;
				destGreen = m_parms.pMessage->g2;
				destBlue = m_parms.pMessage->b2;
				blend = 255 - (deltaTime * (1.0/m_parms.pMessage->fxtime) * 255.0 + 0.5);
			}
		}
		break;
	}
	if ( blend > 255 )
		blend = 255;
	else if ( blend < 0 )
		blend = 0;

	m_parms.r = ((srcRed * (255-blend)) + (destRed * blend)) >> 8;
	m_parms.g = ((srcGreen * (255-blend)) + (destGreen * blend)) >> 8;
	m_parms.b = ((srcBlue * (255-blend)) + (destBlue * blend)) >> 8;

	if ( m_parms.pMessage->effect == 1 && m_parms.charTime != 0 )
	{
		if ( m_parms.x >= 0 && m_parms.y >= 0 && (m_parms.x + gHUD.m_scrinfo.charWidths[ m_parms.text ]) <= ScreenWidth )
			TextMessageDrawChar( m_parms.x, m_parms.y, m_parms.text, m_parms.pMessage->r2, m_parms.pMessage->g2, m_parms.pMessage->b2 );
	}
}


void CHudMessage::MessageScanStart( void )
{
	switch( m_parms.pMessage->effect )
	{
	// Fade-in / out with flicker
	case 1:
	case 0:
		m_parms.fadeTime = m_parms.pMessage->fadein + m_parms.pMessage->holdtime;
		

		if ( m_parms.time < m_parms.pMessage->fadein )
		{
			m_parms.fadeBlend = ((m_parms.pMessage->fadein - m_parms.time) * (1.0/m_parms.pMessage->fadein) * 255);
		}
		else if ( m_parms.time > m_parms.fadeTime )
		{
			if ( m_parms.pMessage->fadeout > 0 )
				m_parms.fadeBlend = (((m_parms.time - m_parms.fadeTime) / m_parms.pMessage->fadeout) * 255);
			else
				m_parms.fadeBlend = 255; // Pure dest (off)
		}
		else
			m_parms.fadeBlend = 0;	// Pure source (on)
		m_parms.charTime = 0;

		if ( m_parms.pMessage->effect == 1 && (rand()%100) < 10 )
			m_parms.charTime = 1;
		break;

	case 2:
		m_parms.fadeTime = (m_parms.pMessage->fadein * m_parms.length) + m_parms.pMessage->holdtime;
		
		if ( m_parms.time > m_parms.fadeTime && m_parms.pMessage->fadeout > 0 )
			m_parms.fadeBlend = (((m_parms.time - m_parms.fadeTime) / m_parms.pMessage->fadeout) * 255);
		else
			m_parms.fadeBlend = 0;
		break;
	}
}


void CHudMessage::MessageDrawScan( client_textmessage_t *pMessage, float time )
{
	int i, j, length, width;
	const char *pText;
	unsigned char line[80];

	pText = pMessage->pMessage;
	// Count lines
	m_parms.lines = 1;
	m_parms.time = time;
	m_parms.pMessage = pMessage;
	length = 0;
	width = 0;
	m_parms.totalWidth = 0;
	while ( *pText )
	{
		if ( *pText == '\n' )
		{
			m_parms.lines++;
			if ( width > m_parms.totalWidth )
				m_parms.totalWidth = width;
			width = 0;
		}
		else
			width += gHUD.m_scrinfo.charWidths[*pText];
		pText++;
		length++;
	}
	m_parms.length = length;
	m_parms.totalHeight = (m_parms.lines * gHUD.m_scrinfo.iCharHeight);


	m_parms.y = YPosition( pMessage->y, m_parms.totalHeight );
	pText = pMessage->pMessage;

	m_parms.charTime = 0;

	MessageScanStart();

	for ( i = 0; i < m_parms.lines; i++ )
	{
		m_parms.lineLength = 0;
		m_parms.width = 0;
		while ( *pText && *pText != '\n' )
		{
			unsigned char c = *pText;
			line[m_parms.lineLength] = c;
			m_parms.width += gHUD.m_scrinfo.charWidths[c];
			m_parms.lineLength++;
			pText++;
		}
		pText++;		// Skip LF
		line[m_parms.lineLength] = 0;

		m_parms.x = XPosition( pMessage->x, m_parms.width, m_parms.totalWidth );

		for ( j = 0; j < m_parms.lineLength; j++ )
		{
			m_parms.text = line[j];
			int next = m_parms.x + gHUD.m_scrinfo.charWidths[ m_parms.text ];
			MessageScanNextChar();
			
			if ( m_parms.x >= 0 && m_parms.y >= 0 && next <= ScreenWidth )
				TextMessageDrawChar( m_parms.x, m_parms.y, m_parms.text, m_parms.r, m_parms.g, m_parms.b );
			m_parms.x = next;
		}

		m_parms.y += gHUD.m_scrinfo.iCharHeight;
	}
}


int CHudMessage::Draw( float fTime )
{
	int i, drawn;
	client_textmessage_t *pMessage;
	float endTime;

	drawn = 0;

   if (m_TitleTime > gHUD.m_flTime)
      {
      DrawTitle(fTime);
      drawn = 1;
      }
   else
      m_TitleTime = 0;

	if ( m_gameTitleTime > 0 )
	{
		float localTime = gHUD.m_flTime - m_gameTitleTime;
		float brightness;

		// Maybe timer isn't set yet
		if ( m_gameTitleTime > gHUD.m_flTime )
			m_gameTitleTime = gHUD.m_flTime;

		if ( localTime > (m_pGameTitle->fadein + m_pGameTitle->holdtime + m_pGameTitle->fadeout) )
			m_gameTitleTime = 0;
		else
		{
			brightness = FadeBlend( m_pGameTitle->fadein, m_pGameTitle->fadeout, m_pGameTitle->holdtime, localTime );

			int halfWidth = gHUD.GetSpriteRect(m_HUD_title_half).right - gHUD.GetSpriteRect(m_HUD_title_half).left;
			int fullWidth = halfWidth + gHUD.GetSpriteRect(m_HUD_title_life).right - gHUD.GetSpriteRect(m_HUD_title_life).left;
			int fullHeight = gHUD.GetSpriteRect(m_HUD_title_half).bottom - gHUD.GetSpriteRect(m_HUD_title_half).top;

			int x = XPosition( m_pGameTitle->x, fullWidth, fullWidth );
			int y = YPosition( m_pGameTitle->y, fullHeight );


			SPR_Set( gHUD.GetSprite(m_HUD_title_half), brightness * m_pGameTitle->r1, brightness * m_pGameTitle->g1, brightness * m_pGameTitle->b1 );
			SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect(m_HUD_title_half) );

			SPR_Set( gHUD.GetSprite(m_HUD_title_life), brightness * m_pGameTitle->r1, brightness * m_pGameTitle->g1, brightness * m_pGameTitle->b1 );
			SPR_DrawAdditive( 0, x + halfWidth, y, &gHUD.GetSpriteRect(m_HUD_title_life) );

			drawn = 1;
		}
	}
	// Fixup level transitions
	for ( i = 0; i < maxHUDMessages; i++ )
	{
		// Assume m_parms.time contains last time
		if ( m_pMessages[i] )
		{
			pMessage = m_pMessages[i];
			if ( m_startTime[i] > gHUD.m_flTime )
				m_startTime[i] = gHUD.m_flTime + m_parms.time - m_startTime[i] + 0.2;	// Server takes 0.2 seconds to spawn, adjust for this
		}
	}

	for ( i = 0; i < maxHUDMessages; i++ )
	{
		if ( m_pMessages[i] )
		{
			pMessage = m_pMessages[i];

			// This is when the message is over
			switch( pMessage->effect )
			{
			case 0:
			case 1:
				endTime = m_startTime[i] + pMessage->fadein + pMessage->fadeout + pMessage->holdtime;
				break;
			
			// Fade in is per character in scanning messages
			case 2:
				endTime = m_startTime[i] + (pMessage->fadein * strlen( pMessage->pMessage )) + pMessage->fadeout + pMessage->holdtime;
				break;
			}

			if ( fTime <= endTime )
			{
				float messageTime = fTime - m_startTime[i];

				// Draw the message
				// effect 0 is fade in/fade out
				// effect 1 is flickery credits
				// effect 2 is write out (training room)
				MessageDrawScan( pMessage, messageTime );

				drawn++;
			}
			else
			{
				// The message is over
				m_pMessages[i] = NULL;
			}
		}
	}

	// Remember the time -- to fix up level transitions
	m_parms.time = gHUD.m_flTime;
	// Don't call until we get another message
	if ( !drawn )
		m_iFlags &= ~HUD_ACTIVE;

	return 1;
}


void CHudMessage::MessageAdd( const char *pName, float time )
{
	int i,j;
	client_textmessage_t *tempMessage;

	for ( i = 0; i < maxHUDMessages; i++ )
	{
		if ( !m_pMessages[i] )
		{
			// Trim off a leading # if it's there
			if ( pName[0] == '#' ) 
				tempMessage = TextMessageGet( pName+1 );
			else
				tempMessage = TextMessageGet( pName );
			// If we couldnt find it in the titles.txt, just create it
			if ( !tempMessage )
			{
				g_pCustomMessage.effect = 2;
				g_pCustomMessage.r1 = g_pCustomMessage.g1 = g_pCustomMessage.b1 = g_pCustomMessage.a1 = 100;
				g_pCustomMessage.r2 = 240;
				g_pCustomMessage.g2 = 110;
				g_pCustomMessage.b2 = 0;
				g_pCustomMessage.a2 = 0;
				g_pCustomMessage.x = -1;		// Centered
				g_pCustomMessage.y = 0.7;
				g_pCustomMessage.fadein = 0.01;
				g_pCustomMessage.fadeout = 1.5;
				g_pCustomMessage.fxtime = 0.25;
				g_pCustomMessage.holdtime = 5;
				g_pCustomMessage.pName = g_pCustomName;
				strcpy( g_pCustomText, pName );
				g_pCustomMessage.pMessage = g_pCustomText;

				tempMessage = &g_pCustomMessage;
			}

			for ( j = 0; j < maxHUDMessages; j++ )
			{
				if ( m_pMessages[j] )
				{
					// is this message already in the list
					if ( !strcmp( tempMessage->pMessage, m_pMessages[j]->pMessage ) )
					{
						return;
					}
					// get rid of any other messages in same location (only one displays at a time)
					else if ( abs( tempMessage->y - m_pMessages[j]->y ) < 1 )
					{
						if ( abs( tempMessage->x - m_pMessages[j]->x ) < 1 )
						{
							m_pMessages[j] = NULL;
						}
					}
				}
			}

			m_pMessages[i] = tempMessage;
			m_startTime[i] = time;
			return;
		}
	}
}


int CHudMessage::MsgFunc_HudText( const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	char *pString = READ_STRING();

	MessageAdd( pString, gHUD.m_flTime );
	// Remember the time -- to fix up level transitions
	m_parms.time = gHUD.m_flTime;

	// Turn on drawing
	if ( !(m_iFlags & HUD_ACTIVE) )
		m_iFlags |= HUD_ACTIVE;

	return 1;
}


int CHudMessage::MsgFunc_GameTitle( const char *pszName,  int iSize, void *pbuf )
{
	m_pGameTitle = TextMessageGet( "GAMETITLE" );
	if ( m_pGameTitle != NULL )
	{
		m_gameTitleTime = gHUD.m_flTime;

		// Turn on drawing
		if ( !(m_iFlags & HUD_ACTIVE) )
			m_iFlags |= HUD_ACTIVE;
	}

	return 1;
}

int CHudMessage::MsgFunc_Title(const char *pszName, int iSize, void *pbuf )
{
   char *pString;
   int i, j;
   char cc;
   char *pText;
   int x, y, max;
   m_iLow = 0;
	BEGIN_READ( pbuf, iSize );
	m_iTeam = READ_BYTE();
	pString = READ_STRING();
   if (!pString) return 0;
   m_TitleTime = gHUD.m_flTime + 5;
   strcpy(m_TitleText, pString);
   max = strlen(pString);
   j = 0; i = 0;
   m_TitleText[0] = 0;
   while (pString[i])
      {
      do {
         if (pString[i] == '^')
            {
            i++;
            cc = pString[i];
            switch (cc)
               {
               case '_':
                  m_iLow = 1; i+=2; break;
               case 'r':
                  m_iTeam = 3; i+=2; break;
               case 'g':
                  m_iTeam = 4; i+=2; break;
               case 'b':
                  m_iTeam = 5; i+=2; break;
               case 'y':
                  m_iTeam = 6; i+=2; break;
               case 'c':
                  m_iTeam = 7; i+=2; break;
               case 'p':
                  m_iTeam = 8; i+=2; break;
               case 'o':
                  m_iTeam = 9; i+=2; break;
               case 'w':
                  m_iTeam = 10; i+=2; break;
               case 'n':
                  m_iTeam = 11; i+=2; break;
               }
            }
         }  while (i < max && pString[i] == '^');
      m_TitleText[j] = pString[i];
      i++;
      j++;
      }
   m_TitleText[j] = 0;

   switch (m_iTeam)
      {
      case 0:
         r =  FALSE_RED;
         g =  FALSE_GREEN;
         b =  FALSE_BLUE;
         break;
      case 1:
         r =  EAST_RED;
         g =  EAST_GREEN;
         b =  EAST_BLUE;
         break;
      case 2:
         r =  WEST_RED;
         g =  WEST_GREEN;
         b =  WEST_BLUE;
         break;
      case 3:
         r =  TRUE_RED;
         g =  0;
         b =  0;
         break;
      case 4:
         r =  0;
         g =  TRUE_GREEN;
         b =  0;
         break;
      case 5:
         r =  0;
         g =  0;
         b =  TRUE_BLUE;
         break;
      case 6:
         r =  TRUE_RED;
         g =  TRUE_GREEN;
         b =  0;
         break;
      case 7:
         r =  0;
         g =  TRUE_GREEN;
         b =  TRUE_BLUE;
         break;
      case 8:
         r =  TRUE_RED;
         g =  0;
         b =  TRUE_BLUE;
         break;
      case 9:
         r =  TRUE_RED;
         g =  FALSE_GREEN;
         b =  0;
         break;
      case 10:
         r =  TRUE_RED;
         g =  TRUE_GREEN;
         b =  TRUE_BLUE;
         break;
      case 11:
         r =  FALSE_RED;
         g =  FALSE_GREEN;
         b =  FALSE_BLUE;
         break;
      }


   pText = m_TitleText;
   y = 0;
   x = 0;
	while (*pText)
	   {
		if (*pText == '\n')
		   {
         x = (ScreenWidth - x) / 2;
         center_x[y++] = x;
			x = 0;
		   }
		else
         {
			x += gHUD.m_scrinfo.charWidths[*pText];
         }
		pText++;
	   }
   x = (ScreenWidth - x) / 2;
   center_x[y++] = x;
	x = 0;
   lines = y;
   m_iFlags |= HUD_ACTIVE;
   return 1;
}

int CHudMessage::DrawTitle( float fTime )
{
	int   lines = 1, 
         max_width = 0, 
         max_height = 0,
         x = 0, y, row;
	const char *pText;
   
   if (x > max_width) max_width = x;
   x = (ScreenWidth - max_width) / 2;
   max_height = lines * (gHUD.m_scrinfo.iCharHeight + 4);
   if (m_iLow)
      y = ScreenHeight - ((ScreenHeight - max_height) / 3);   
   else
      y = (ScreenHeight - max_height) / 3;
      
   pText = m_TitleText;
   if (m_iTeam < 3)
      {
      x = x + (rand() & 3);
      y = y + (rand() & 3);
      }
   row = 0;
   x = center_x[row++];
	while (*pText)
	   {      
		if (*pText == '\n')
         {
         y += gHUD.m_scrinfo.iCharHeight + 4;
         x = center_x[row++];
         }
		else
         {
         TextMessageDrawChar(x, y, *pText, r, g, b);
         x += gHUD.m_scrinfo.charWidths[*pText];
         }
		pText++;
	   }
   return 1;
}
/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_gui_stats.cpp 
   This is the player statistics module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "hud.h"
#include "cl_util.h"
#include "VGUI_Font.h"
#include "VGUI_ScrollPanel.h"
#include "VGUI_TextImage.h"
#include "vgui_int.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ServerBrowser.h"
#include "sdk_e_menu.h"
#include "vgui_textpanel.h"
#include "sdk_w_matrix.h"
#include "sdk_e_menu.h"
#include "sdk_u_mp3.h"

#define MOTD_TITLE_X		XRES(16)
#define MOTD_TITLE_Y		YRES(16)

#define MOTD_WINDOW_X				XRES(112)
#define MOTD_WINDOW_Y				YRES(80)
#define MOTD_WINDOW_SIZE_X			XRES(424)
#define MOTD_WINDOW_SIZE_Y			YRES(312)

extern cvar_t *cl_beats, *cl_chat;
sendstat_t gstat;
char sTop32[4096];
// #define MOTD_TEXT  "\n"
#define MOTD_TEXT "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "12345678901234567890123456789012345678901234567890\n"\
                     "\n"
CStatsMenu :: CStatsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
   statsstring = NULL;
	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
	SchemeHandle_t hMOTDText = pSchemes->getSchemeHandle( "Briefing Text" );

	// color schemes
	int r, g, b, a;

	// Create the window
	m_pBackgroundPanel = new CTransparentPanel( 255, MOTD_WINDOW_X, MOTD_WINDOW_Y, MOTD_WINDOW_SIZE_X, MOTD_WINDOW_SIZE_Y );
	m_pBackgroundPanel->setParent( this );
	m_pBackgroundPanel->setBorder( new LineBorder( Color(FALSE_RED,FALSE_GREEN, FALSE_BLUE,0)) );
	m_pBackgroundPanel->setVisible( true );

	int iXSize,iYSize,iXPos,iYPos;
	m_pBackgroundPanel->getPos( iXPos,iYPos );
	m_pBackgroundPanel->getSize( iXSize,iYSize );

	// Create the title
	pLabel = new Label( "", iXPos + MOTD_TITLE_X, iYPos + MOTD_TITLE_Y );
	pLabel->setParent( this );
	pLabel->setFont( pSchemes->getFont(hTitleScheme) );
	pLabel->setFont( Scheme::sf_primary1 );

	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
   r = TRUE_RED; g = TRUE_GREEN; b = TRUE_BLUE;

	pLabel->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	pLabel->setBgColor( r, g, b, a );
	pLabel->setContentAlignment( vgui::Label::a_west );
	pLabel->setText("Personal Statistics");

	// Create the Scroll panel
	ScrollPanel *pScrollPanel = new CTFScrollPanel( iXPos + XRES(16), iYPos + MOTD_TITLE_Y*2 + YRES(16), iXSize /*- XRES(16)*/, iYSize - (YRES(48) + BUTTON_SIZE_Y*4) );
	pScrollPanel->setParent(this);
	//force the scrollbars on so clientClip will take them in account after the validate
	pScrollPanel->setScrollBarAutoVisible(false, false);
	pScrollPanel->setScrollBarVisible(true, true);
	pScrollPanel->validate();

	// Create the text panel
	pText = new TextPanel( MOTD_TEXT, 0,0, 300,64);
	pText->setParent( pScrollPanel->getClient() );

	// get the font and colors from the scheme
	pText->setFont( pSchemes->getFont(hMOTDText) );
	pText->setFgColor( TRUE_RED, TRUE_GREEN, TRUE_BLUE, 0);
	pSchemes->getBgColor( hMOTDText, r, g, b, a );
	pText->setBgColor( r, g, b, a );
   CompileStats();
   //if (statsstring)
   pText->setText(MOTD_TEXT); //statsstring
   pText->setSize(pScrollPanel->getClientClip()->getWide()-2, 5000);

	// Get the total size of the MOTD text and resize the text panel
	int iScrollSizeX, iScrollSizeY;
	pText->getTextImage()->getTextSizeWrapped( iScrollSizeX, iScrollSizeY );
	pText->setSize( iScrollSizeX , iScrollSizeY );
	//pText->setBorder(new LineBorder());

	//turn the scrollbars back into automode
	pScrollPanel->setScrollBarAutoVisible(true, true);
	pScrollPanel->setScrollBarVisible(false, false);

	pScrollPanel->validate();

	pButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Close" ), 
             iXPos + XRES(32), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(40), BUTTON_SIZE_Y);
	
   pButton->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	pButton->setParent(this);

	pButtonAdmin = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Admin" ), 
             iXPos + XRES(32), iYPos + iYSize - YRES(20) - BUTTON_SIZE_Y*2 , XRES(40), BUTTON_SIZE_Y);
	pButtonAdmin->addActionSignal(new CMenuHandler_StringCommand( "adminmenu", true ));
	pButtonAdmin->setParent(this);

   pButtonVoteMap= new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Vote Map" ), 
             iXPos + XRES(102), iYPos + iYSize - YRES(20) - BUTTON_SIZE_Y*2, XRES(60), BUTTON_SIZE_Y);
	pButtonVoteMap->addActionSignal(new CMenuHandler_StringCommand( "votemaps", true ));
	pButtonVoteMap->setParent(this);

   pButtonTop10 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Top 10" ), 
             iXPos + XRES(182), iYPos + iYSize - YRES(20) - BUTTON_SIZE_Y*2, XRES(60), BUTTON_SIZE_Y);
	pButtonTop10->addActionSignal(new CMenuHandler_StringCommand( "topscores", true ));
	pButtonTop10->setParent(this);

	pButtonAudio = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Audio Off" ), 
             iXPos + XRES(262), iYPos + iYSize - YRES(20) - BUTTON_SIZE_Y*2, XRES(60), BUTTON_SIZE_Y);
	pButtonAudio->addActionSignal(new CMenuHandler_FlipMode(ACTION_TOGGLE_AUDIO));
	pButtonAudio->setParent(this);

	pButtonMute = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Chat Off" ), 
             iXPos + XRES(342), iYPos + iYSize - YRES(20) - BUTTON_SIZE_Y*2, XRES(60), BUTTON_SIZE_Y);
	pButtonMute->addActionSignal(new CMenuHandler_FlipMode(ACTION_TOGGLE_MUTE));
	pButtonMute->setParent(this);

	pButtonPrev = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Prev" ), 
             iXPos + XRES(102), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(35), BUTTON_SIZE_Y);
	pButtonPrev->addActionSignal(new CMenuHandler_StringCommand( "showstats 1", false ));
	pButtonPrev->setParent(this);

   pButtonNext = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Next" ), 
             iXPos + XRES(182), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(35), BUTTON_SIZE_Y);
	pButtonNext->addActionSignal(new CMenuHandler_StringCommand( "showstats 2", false ));
	pButtonNext->setParent(this);
}

void CStatsMenu :: SetActiveInfo( int iInfo )
{
	if (iInfo >= 0 && iInfo <= 1)
	{
		if (!iInfo)
		   {
			m_pText->setVisible( false );
		   }
		else
		   {
         /*
			char temp[128];
			m_pEditbox->getText(0,temp,128);
			m_pText->setText(temp);
			m_pText->setVisible( true );
         */
		   }
	}
}


void CStatsMenu :: setVisible( bool visible )
{
   static char isvisible = 0;
	if (visible)
      {
      char xxx[128] = {0};      
      if (gstat.ClientID >= 0 && gstat.ClientID < MAX_PLAYERS)
         {
         CompileStats();
         if (cl_beats->value)
            pButtonAudio->setText("Audio Off");
         else
            pButtonAudio->setText("Audio On");
         if (cl_chat->value)
            pButtonMute->setText("Chat Off");
         else
            pButtonMute->setText("Chat On");
         if (!g_PlayerInfoList[gstat.ClientID].name)
            g_PlayerInfoList[gstat.ClientID].name = "You";
         sprintf(xxx, "Stats for: %s", g_PlayerInfoList[gstat.ClientID].name);
	      pLabel->setText(xxx);
         pText->setText(statsstring);
         }
      if (!isvisible)
         {
	      CMenuPanel::setVisible(visible);
         MP3_RandomPlay();
         }
      isvisible = 1;
      }
   else
      {
      pButton->setArmed(false);
      pButtonAdmin->setArmed(false);
      pButtonAudio->setArmed(false);
      pButtonVoteMap->setArmed(false);
      pButtonTop10->setArmed(false);
	   CMenuPanel::setVisible(visible);
      MP3_Stop();
      isvisible = 0;
      }
}

void CStatsMenu :: CompileStats()
{
   float f, k, l = 0;
   int seconds, minutes;
   char xxx[4096];
   char xxx2[4096];
   if (statsstring)
      delete [] statsstring;

   seconds = gstat.TimeOn;
   seconds = seconds % 60;
   minutes = gstat.TimeOn - seconds;
   minutes /= 60;
   
   sprintf(xxx, "Client ID: %s\n"
                "Steam ID: %s\n"
                "Juice: %i\n"
                "Health: %i\n\n",
                  g_PlayerInfoList[gstat.ClientID].name,
                  gstat.SteamID,
                  gstat.Juice,
                  gstat.Health);

   // Highest shooting percentage
   if (gstat.Hits > 0 && gstat.Shots)
      {
      f = (float) gstat.Hits / (float)gstat.Shots;
      f *= 100;
      sprintf(xxx, "%sShooting percentage: %.2f%%\n", xxx, f);
      }
   else
      sprintf(xxx, "%sShooting percentage: N/A\n", xxx);

   // Head-shot kill percentage
   if (gstat.Frags > 0 && gstat.HeadShots)
      {
      f = (float) gstat.HeadShots / (float)gstat.Frags;
      f *= 100;
      sprintf(xxx, "%sHead-shot kills: %.2f%%\n", xxx, f);
      }
   else
      sprintf(xxx, "%sHead-shot kills: N/A\n", xxx);

   // Knife-kill kill percentage
   if (gstat.Frags > 0 && gstat.KnifeKills)
      {
      f = (float) gstat.KnifeKills / (float)gstat.Frags;
      f *= 100;
      sprintf(xxx, "%sKnife kills: %.2f%%\n", xxx, f);
      }
   else
      sprintf(xxx, "%sKnife kills: N/A\n", xxx);


   // Favorite Primary
   sprintf(xxx, "%s\nFavored Primary:\n", xxx);
   if (gstat.FavPrimary  && gstat.Frags > 0)
      {
      f = (float) gstat.PrimaryKills / (float)gstat.Frags;
      f *= 100;
      sprintf(xxx, "%s    %s (%.2f%%)\n", xxx, w_stat[gstat.FavPrimary].name, f);
      }
   else
      sprintf(xxx, "%s    None\n", xxx);
   // Favorite Secondary
   sprintf(xxx, "%sFavored Secondary:\n", xxx);
   if (gstat.FavSecondary && gstat.Frags > 0)
      {
      f = (float) gstat.SecondaryKills / (float)gstat.Frags;
      f *= 100;
      sprintf(xxx, "%s    %s (%.2f%%)\n", xxx, w_stat[gstat.FavSecondary].name, f);
      }
   else
      sprintf(xxx, "%s    None\n", xxx);

   // Kill ratio
   k = gstat.Frags; f = gstat.Deaths + k;
   if (k > 0 && f > 0)
      {        
      k /= f;
      k *= 100;
      sprintf(xxx, "%s\nKill ratio: %.2f%%\n", xxx, k);
      }
   else
      sprintf(xxx, "%s\nKill ratio: N/A\n", xxx);

   // Life expectancy
   k = gstat.Rounds;
   if (k > 0)
      {
      f = gstat.MaxRoundLives;
      f /= k;
      f *= 100;
      sprintf(xxx, "%sLife expectancy: %.2f%%\n", xxx, f);
      }
   else
      sprintf(xxx, "%sLife expectancy: N/A\n", xxx);

   // Body count per minute
   k = gstat.Frags;
   f = gstat.TimeOn/60.0f;
   if (k > 0 && f >= 1)
      {        
      k /= f;
      sprintf(xxx, "%sBody count per minute: %.2f\n", xxx, k);
      }
   else
      sprintf(xxx, "%sBody count per minute: N/A\n", xxx);

   sprintf(xxx2, "\n"
                "Kills: %i\n"
                "Deaths: %i\n"
                "Shots fired: %i\n"
                "Shots hit: %i\n"
                "Knife Kills: %i\n"
                "Head shots: %i\n\n"
                "Rounds played: %i\n"
                "Rounds lived: %i\n"
                "Current round kills: %i\n"
                "Maximum kills in one round: %i\n\n"
                "Offensive record: %i\n"
                "Defensive record: %i\n" 
                "Suicides: %i\n"
                "Time on server: %i:%02i\n"
                "Round winning kills: %i\n", 
                  gstat.Frags,
                  gstat.Deaths,
                  gstat.Shots,
                  gstat.Hits,
                  gstat.KnifeKills,
                  gstat.HeadShots,
                  gstat.Rounds,
                  gstat.MaxRoundLives,
                  gstat.RoundKills,
                  gstat.MaxRoundKills,
                  gstat.Offense,
                  gstat.Defense,
                  gstat.Suicides,
                  minutes, seconds,
                  gstat.LastKills);

   strcat(xxx, xxx2);
   statsstring = new char[strlen(xxx)];
   strcpy(statsstring, xxx);
}

CTop32Menu :: CTop32Menu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
   statsstring = NULL;
	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
	SchemeHandle_t hMOTDText = pSchemes->getSchemeHandle( "Scoreboard Title Text" );

	// color schemes
	int r, g, b, a;

	// Create the window
	m_pBackgroundPanel = new CTransparentPanel( 255, MOTD_WINDOW_X, MOTD_WINDOW_Y, MOTD_WINDOW_SIZE_X, MOTD_WINDOW_SIZE_Y );
	m_pBackgroundPanel->setParent( this );
	m_pBackgroundPanel->setBorder( new LineBorder( Color(FALSE_RED,FALSE_GREEN, FALSE_BLUE,0)) );
	m_pBackgroundPanel->setVisible( true );

	int iXSize,iYSize,iXPos,iYPos;
	m_pBackgroundPanel->getPos( iXPos,iYPos );
	m_pBackgroundPanel->getSize( iXSize,iYSize );

	// Create the title
	pLabel = new Label( "", iXPos + MOTD_TITLE_X, iYPos + MOTD_TITLE_Y );
	pLabel->setParent( this );
	pLabel->setFont( pSchemes->getFont(hTitleScheme) );
	pLabel->setFont( Scheme::sf_primary1 );

	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
   r = TRUE_RED; g = TRUE_GREEN; b = TRUE_BLUE;

	pLabel->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	pLabel->setBgColor( r, g, b, a );
	pLabel->setContentAlignment( vgui::Label::a_west );
	pLabel->setText("Top 10 Scores");

	// Create the Scroll panel
	ScrollPanel *pScrollPanel = new CTFScrollPanel( iXPos + XRES(16), iYPos + MOTD_TITLE_Y*2 + YRES(16), iXSize /*- XRES(16)*/, iYSize - (YRES(48) + BUTTON_SIZE_Y*2) );
	pScrollPanel->setParent(this);
	//force the scrollbars on so clientClip will take them in account after the validate
	pScrollPanel->setScrollBarAutoVisible(false, false);
	pScrollPanel->setScrollBarVisible(true, true);
	pScrollPanel->validate();

	// Create the text panel
	pText = new TextPanel( MOTD_TEXT, 0,0, 100,64);
	pText->setParent( pScrollPanel->getClient() );

	// get the font and colors from the scheme
	pText->setFont( pSchemes->getFont(hMOTDText) );
	pText->setFgColor( TRUE_RED, TRUE_GREEN, TRUE_BLUE, 0);
	pSchemes->getBgColor( hMOTDText, r, g, b, a );
	pText->setBgColor( r, g, b, a );
   pText->setSize(pScrollPanel->getClientClip()->getWide()-2, 5000);

	// Get the total size of the MOTD text and resize the text panel
	int iScrollSizeX, iScrollSizeY;
	pText->getTextImage()->getTextSizeWrapped( iScrollSizeX, iScrollSizeY );
	pText->setSize( iScrollSizeX , iScrollSizeY );
	//pText->setBorder(new LineBorder());

	//turn the scrollbars back into automode
	pScrollPanel->setScrollBarAutoVisible(true, true);
	pScrollPanel->setScrollBarVisible(false, false);

	pScrollPanel->validate();

	pButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Close" ), iXPos + XRES(32), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(40), BUTTON_SIZE_Y);
   pButton->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	pButton->setParent(this);

}

void CTop32Menu :: setVisible( bool visible )
{
   static char isvisible = 0;
	if (visible)
      {
      pText->setText(sTop32);
      if (!isvisible)
         {    
         MP3_RandomPlay();
	      CMenuPanel::setVisible(visible);
         }
      isvisible = 1;
      }
   else
      {
      pButton->setArmed(false);
	   CMenuPanel::setVisible(visible);
      isvisible = 0;
      MP3_Stop();
      }
}

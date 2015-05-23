/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_gui_mapvote.cpp
   This is the map voting module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "hud.h"
#include "cl_util.h"
#include "VGUI_Font.h"
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

int  num_players = 0;
map_vote_t  map_vote[MAX_MAP_VOTES];
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
CVoteMapMenu :: CVoteMapMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
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
	pLabel->setText("Vote For Map");

	pButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Close" ), iXPos + XRES(32), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(40), BUTTON_SIZE_Y);
   pButton->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	pButton->setParent(this);

	pButtonMap[0] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[0]->addActionSignal(new CMenuHandler_StringCommand( "votemap 1", true ));
	pButtonMap[0]->setParent(this);

	pButtonMap[1] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*2, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[1]->addActionSignal(new CMenuHandler_StringCommand( "votemap 2", true ));
	pButtonMap[1]->setParent(this);

	pButtonMap[2] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*3, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[2]->addActionSignal(new CMenuHandler_StringCommand( "votemap 3", true ));
	pButtonMap[2]->setParent(this);

	pButtonMap[3] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*4, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[3]->addActionSignal(new CMenuHandler_StringCommand( "votemap 4", true ));
	pButtonMap[3]->setParent(this);

	pButtonMap[4] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*5, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[4]->addActionSignal(new CMenuHandler_StringCommand( "votemap 5", true ));
	pButtonMap[4]->setParent(this);

	pButtonMap[5] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*6, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[5]->addActionSignal(new CMenuHandler_StringCommand( "votemap 6", true ));
	pButtonMap[5]->setParent(this);

	pButtonMap[6] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*7, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[6]->addActionSignal(new CMenuHandler_StringCommand( "votemap 7", true ));
	pButtonMap[6]->setParent(this);

	pButtonMap[7] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*8, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[7]->addActionSignal(new CMenuHandler_StringCommand( "votemap 8", true ));
	pButtonMap[7]->setParent(this);

	pButtonMap[8] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*9, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[8]->addActionSignal(new CMenuHandler_StringCommand( "votemap 9", true ));
	pButtonMap[8]->setParent(this);

	pButtonMap[9] = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Not Available" ), 
                 iXPos + XRES(32), iYPos + YRES(48) + BUTTON_SIZE_Y*10, XRES(150), BUTTON_SIZE_Y);
   pButtonMap[9]->addActionSignal(new CMenuHandler_StringCommand( "votemap 10", true ));
	pButtonMap[9]->setParent(this);
}

void CVoteMapMenu :: setVisible( bool visible )
{
   static char isvisible = 0;
	if (visible)
      {
      CompileMapVotes();
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
      pButtonMap[0]->setArmed(false);
      pButtonMap[1]->setArmed(false);
      pButtonMap[2]->setArmed(false);
      pButtonMap[3]->setArmed(false);
      pButtonMap[4]->setArmed(false);
      pButtonMap[5]->setArmed(false);
      pButtonMap[6]->setArmed(false);
      pButtonMap[7]->setArmed(false);
      pButtonMap[8]->setArmed(false);
      pButtonMap[9]->setArmed(false);
	   CMenuPanel::setVisible(visible);
      MP3_Stop();
      isvisible = 0;
      }
}

void CVoteMapMenu :: CompileMapVotes()
{
   char xxx[64];
   int i = 0;
   while (i < MAX_MAP_VOTES && map_vote[i].name)
      {
      sprintf(xxx, "%s (%i/%i votes)", map_vote[i].name, map_vote[i].votes, num_players + 1);
      pButtonMap[i]->setText(xxx);
      i++;
      }
}

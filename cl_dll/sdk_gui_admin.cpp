/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_gui_admin.cpp 
   This is the server administration module.

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

#define MOTD_TITLE_X		XRES(16)
#define MOTD_TITLE_Y		YRES(16)

#define MOTD_WINDOW_X				XRES(112)
#define MOTD_WINDOW_Y				YRES(80)
#define MOTD_WINDOW_SIZE_X			XRES(424)
#define MOTD_WINDOW_SIZE_Y			YRES(312)

unsigned short int admin_torture = 0;

#define ADMIN_TORTURE_FREEZE     2
#define ADMIN_TORTURE_GAG        4


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
                     "\n"
CAdminMenu :: CAdminMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
   statsstring = NULL;
	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
	SchemeHandle_t hMOTDText = pSchemes->getSchemeHandle( "Primary Button Text" );

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
	pLabel->setText("Admin Menu");


   m_pFreeze = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Freeze" ), iXPos + XRES(32), iYPos + YRES(64), XRES(80), BUTTON_SIZE_Y);
	m_pFreeze->setParent(this);
   m_pFreeze->addActionSignal(new CMenuHandler_StringCommand( "gw_vguifreeze", true ));
	
	m_pMute = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Gag" ), iXPos + XRES(32+100), iYPos + YRES(64), XRES(80), BUTTON_SIZE_Y);
	m_pMute->setParent(this);
   m_pMute->addActionSignal(new CMenuHandler_StringCommand( "gw_vguigag", true ));

	m_pSmack = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Smack" ), iXPos + XRES(32+200), iYPos + YRES(64), XRES(80), BUTTON_SIZE_Y);
	m_pSmack->setParent(this);
   m_pSmack->addActionSignal(new CMenuHandler_StringCommand( "gw_vguismack", false ));

   m_pKick = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Kick" ), iXPos + XRES(32+300), iYPos + YRES(64), XRES(80), BUTTON_SIZE_Y);
	m_pKick->setParent(this);
   m_pKick->addActionSignal(new CMenuHandler_StringCommand( "gw_vguikick", true ));

   m_pBan5 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Ban 5 min" ), iXPos + XRES(32), iYPos + YRES(80+32), XRES(80), BUTTON_SIZE_Y);
	m_pBan5->setParent(this);
   m_pBan5->addActionSignal(new CMenuHandler_StringCommand( "gw_vguiban5", true ));

   m_pBan30 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Ban 30 min" ), iXPos + XRES(32+120), iYPos + YRES(80+32), XRES(80), BUTTON_SIZE_Y);
	m_pBan30->setParent(this);
   m_pBan30->addActionSignal(new CMenuHandler_StringCommand( "gw_vguiban30", true ));

   m_pBan60 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Ban 60 min" ), iXPos + XRES(32+240), iYPos + YRES(80+32), XRES(80), BUTTON_SIZE_Y);
	m_pBan60->setParent(this);
   m_pBan60->addActionSignal(new CMenuHandler_StringCommand( "gw_vguiban60", true ));

   m_pBan24 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Ban 1 day" ), iXPos + XRES(32), iYPos + YRES(80+48), XRES(80), BUTTON_SIZE_Y);
	m_pBan24->setParent(this);
   m_pBan24->addActionSignal(new CMenuHandler_StringCommand( "gw_vguiban24", true ));

   m_pBan7 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Ban 1 week" ), iXPos + XRES(32+120), iYPos + YRES(80+48), XRES(80), BUTTON_SIZE_Y);
	m_pBan7->setParent(this);
   m_pBan7->addActionSignal(new CMenuHandler_StringCommand( "gw_vguiban7", true ));

   m_pBan0 = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Ban 4 Life" ), iXPos + XRES(32+240), iYPos + YRES(80+48), XRES(80), BUTTON_SIZE_Y);
	m_pBan0->setParent(this);
   m_pBan0->addActionSignal(new CMenuHandler_StringCommand( "gw_vguiban0", true ));

   m_pRenameBitch = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Name Bitch" ), iXPos + XRES(32), iYPos + YRES(80+80), XRES(80), BUTTON_SIZE_Y);
	m_pRenameBitch->setParent(this);
   m_pRenameBitch->addActionSignal(new CMenuHandler_StringCommand( "gw_vguibitch", false ));

   m_pRenameCheater = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Name Cheater" ), iXPos + XRES(32+120), iYPos + YRES(80+80), XRES(88), BUTTON_SIZE_Y);
	m_pRenameCheater->setParent(this);
   m_pRenameCheater->addActionSignal(new CMenuHandler_StringCommand( "gw_vguicheater", false ));

   m_pStrip= new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Strip Ganster" ), iXPos + XRES(32+240), iYPos + YRES(80+80), XRES(80), BUTTON_SIZE_Y);
	m_pStrip->setParent(this);
   m_pStrip->addActionSignal(new CMenuHandler_StringCommand( "gw_vguistrip", false ));

   m_pSmokeEast = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Smoke East" ), iXPos + XRES(32), iYPos + YRES(80+112), XRES(80), BUTTON_SIZE_Y);
	m_pSmokeEast->setParent(this);
   m_pSmokeEast->addActionSignal(new CMenuHandler_StringCommand( "gw_vguismokeeast", false ));

   m_pSmokeWest = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Smoke West" ), iXPos + XRES(32+120), iYPos + YRES(80+112), XRES(80), BUTTON_SIZE_Y);
	m_pSmokeWest->setParent(this);
   m_pSmokeWest->addActionSignal(new CMenuHandler_StringCommand( "gw_vguismokewest", false ));

   m_pRestart = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Reset Scores" ), iXPos + XRES(32+240), iYPos + YRES(80+112), XRES(90), BUTTON_SIZE_Y);
	m_pRestart->setParent(this);
   m_pRestart->addActionSignal(new CMenuHandler_StringCommand( "gw_vguirestart", true ));
   

	pButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Close" ), iXPos + XRES(32), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(40), BUTTON_SIZE_Y);
   pButton->addActionSignal(new CMenuHandler_StringCommand("showstats 1", true));
	pButton->setParent(this);


   /*
	pButtonNext = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Next" ), 
                                iXPos + MOTD_WINDOW_SIZE_X - XRES(35+32+50), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(35), BUTTON_SIZE_Y);
	pButtonNext->addActionSignal(new CMenuHandler_StringCommand( "showstats 2", false ));
	pButtonNext->setParent(this);

	pButtonPrev = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "Prev" ), 
                                iXPos + MOTD_WINDOW_SIZE_X - XRES(35+32), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, XRES(35), BUTTON_SIZE_Y);
	pButtonPrev->addActionSignal(new CMenuHandler_StringCommand( "showstats 1", false ));
	pButtonPrev->setParent(this);
   */
}

void CAdminMenu :: setVisible( bool visible )
{
   static char isvisible = 0;
	if (visible)
      {
      char xxx[256];
      if (gstat.ClientID > MAX_PLAYERS || gstat.ClientID < 0) return;
      if (!g_PlayerInfoList[gstat.ClientID].name)
         g_PlayerInfoList[gstat.ClientID].name = "You";
      if (admin_torture & ADMIN_TORTURE_FREEZE)
         m_pFreeze->setText("Unfreeze");
      else
         m_pFreeze->setText("Freeze");
      if (admin_torture & ADMIN_TORTURE_GAG)
         m_pMute->setText("Ungag");
      else
         m_pMute->setText("Gag");
      sprintf(xxx, "Administer: %s", g_PlayerInfoList[gstat.ClientID].name);
      pLabel->setText(xxx);
      if (!isvisible)
	      CMenuPanel::setVisible(visible);
      isvisible = 1;
      }
   else
      {
      m_pMute->setArmed(false);
      m_pFreeze->setArmed(false);
      m_pKick->setArmed(false);
      pButton->setArmed(false);
      m_pBan5->setArmed(false);
      m_pBan30->setArmed(false);
      m_pBan60->setArmed(false);
      m_pBan24->setArmed(false);
      m_pBan7->setArmed(false);
      m_pBan0->setArmed(false);
      m_pRestart->setArmed(false);
	   CMenuPanel::setVisible(visible);
      isvisible = 0;
      }
}

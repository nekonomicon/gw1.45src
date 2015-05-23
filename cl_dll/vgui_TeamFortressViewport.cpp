//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Client DLL VGUI Viewport
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#include<VGUI_Cursor.h>
#include<VGUI_Frame.h>
#include<VGUI_Label.h>
#include<VGUI_Surface.h>
#include<VGUI_BorderLayout.h>
#include<VGUI_Panel.h>
#include<VGUI_ImagePanel.h>
#include<VGUI_Button.h>
#include<VGUI_ActionSignal.h>
#include<VGUI_InputSignal.h>
#include<VGUI_MenuSeparator.h>
#include<VGUI_TextPanel.h>
#include<VGUI_LoweredBorder.h>
#include<VGUI_LineBorder.h>
#include<VGUI_Scheme.h>
#include<VGUI_Font.h>
#include<VGUI_App.h>
#include<VGUI_BuildGroup.h>

#include "hud.h"
#include "cl_util.h"
#include "camera.h"
#include "kbutton.h"
#include "cvardef.h"
#include "usercmd.h"
#include "const.h"
#include "camera.h"
#include "in_defs.h"
#include "parsemsg.h"
#include "../engine/keydefs.h"
#include "demo.h"
#include "demo_api.h"

#include "vgui_int.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ServerBrowser.h"
#include "vgui_ScorePanel.h"
#include "sdk_e_menu.h"
#include "sdk_u_mp3.h"

extern int g_iVisibleMouse;
class CCommandMenu;
int g_iPlayerClass;
int g_iTeamNumber;
int g_iUser1;
int g_iUser2;

void IN_ResetMouse( void );
extern CMenuPanel *CMessageWindowPanel_Create( const char *szMOTD, const char *szTitle, int iShadeFullscreen, int iRemoveMe, int x, int y, int wide, int tall );

using namespace vgui;
extern unsigned short int admin_torture;

// Team Colors
int iTeamColors[5][3] =
{
	{ 255, 255, 255 },
	{ 66, 115, 247 },
	{ 220, 51, 38 },
	{ 240, 135, 0 },
	{ 115, 240, 115 },
};


// Get the name of TGA file, based on GameDir
char* GetVGUITGAName(const char *pszName)
{
	int i;
	char sz[256]; 
	static char gd[256]; 
	const char *gamedir;

	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;
	sprintf(sz, pszName, i);

	gamedir = gEngfuncs.pfnGetGameDirectory();
	sprintf(gd, "%s/gfx/vgui/%s.tga",gamedir,sz);

	return gd;
}

//================================================================
// COMMAND MENU
//================================================================
void CCommandMenu::AddButton( CommandButton *pButton )
{
	if (m_iButtons >= MAX_BUTTONS)
		return;

	m_aButtons[m_iButtons] = pButton;
	m_iButtons++;
	pButton->setParent( this );
	pButton->setFont( Scheme::sf_primary3 );

	// give the button a default key binding
	if ( m_iButtons < 10 )
	{
		pButton->setBoundKey( m_iButtons + '0' );
	}
	else if ( m_iButtons == 10 )
	{
		pButton->setBoundKey( '0' );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Tries to find a button that has a key bound to the input, and
//			presses the button if found
// Input  : keyNum - the character number of the input key
// Output : Returns true if the command menu should close, false otherwise
//-----------------------------------------------------------------------------
bool CCommandMenu::KeyInput( int keyNum )
{
	// loop through all our buttons looking for one bound to keyNum
	for ( int i = 0; i < m_iButtons; i++ )
	{
		if ( !m_aButtons[i]->IsNotValid() )
		{
			if ( m_aButtons[i]->getBoundKey() == keyNum )
			{
				// hit the button
				if ( m_aButtons[i]->GetSubMenu() )
				{
					// open the sub menu
					gViewPort->SetCurrentCommandMenu( m_aButtons[i]->GetSubMenu() );
					return false;
				}
				else
				{
					// run the bound command
					m_aButtons[i]->fireActionSignal();
					return true;
				}
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: clears the current menus buttons of any armed (highlighted) 
//			state, and all their sub buttons
//-----------------------------------------------------------------------------
void CCommandMenu::ClearButtonsOfArmedState( void )
{
	for ( int i = 0; i < GetNumButtons(); i++ )
	{
		m_aButtons[i]->setArmed( false );

		if ( m_aButtons[i]->GetSubMenu() )
		{
			m_aButtons[i]->GetSubMenu()->ClearButtonsOfArmedState();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSubMenu - 
// Output : CommandButton
//-----------------------------------------------------------------------------
CommandButton *CCommandMenu::FindButtonWithSubmenu( CCommandMenu *pSubMenu )
{
	for ( int i = 0; i < GetNumButtons(); i++ )
	{
		if ( m_aButtons[i]->GetSubMenu() == pSubMenu )
			return m_aButtons[i];
	}

	return NULL;
}

// Recalculate the visible buttons
bool CCommandMenu::RecalculateVisibles( int iNewYPos, bool bHideAll )
{
	int  iCurrentY = 0;
	int  iXPos, iYPos;
	bool bHasButton = false;

	if (iNewYPos)
		setPos( _pos[0], iNewYPos );

	// Cycle through all the buttons in this menu, and see which will be visible
	for (int i = 0; i < m_iButtons; i++)
	{
		int iClass = m_aButtons[i]->GetPlayerClass();
		if ( (iClass && iClass != g_iPlayerClass ) || ( m_aButtons[i]->IsNotValid() ) || bHideAll )
		{
			m_aButtons[i]->setVisible( false );
			if ( m_aButtons[i]->GetSubMenu() != NULL )
			{
				(m_aButtons[i]->GetSubMenu())->RecalculateVisibles( _pos[1] + iCurrentY, true );
			}
		}
		else
		{
 			// If it's got a submenu, force it to check visibilities
			if ( m_aButtons[i]->GetSubMenu() != NULL )
			{
				if ( !(m_aButtons[i]->GetSubMenu())->RecalculateVisibles( _pos[1] + iCurrentY, false ) )
				{
					// The submenu had no visible buttons, so don't display this button
					m_aButtons[i]->setVisible( false );
					continue;
				}
			}

			m_aButtons[i]->setVisible( true );

			// Make sure it's at the right Y position
			m_aButtons[i]->getPos( iXPos, iYPos );
			m_aButtons[i]->setPos( iXPos, iCurrentY );

			iCurrentY += BUTTON_SIZE_Y - 1;
			bHasButton = true;
		}
	}

	// Set Size
	setSize( _size[0], iCurrentY + 1 );

	return bHasButton;
}

// Make sure all submenus can fit on the screen
void CCommandMenu::RecalculatePositions( int iYOffset )
{
	int iNewYPos = _pos[1] + iYOffset;
	int iAdjust = 0;

	// Calculate if this is going to fit onscreen, and shuffle it up if it won't
	int iBottom = iNewYPos + _size[1];
	if ( iBottom > ScreenHeight )
	{
		// Move in increments of button sizes
		while (iAdjust < (iBottom - ScreenHeight))
		{
			iAdjust += BUTTON_SIZE_Y - 1;
		}
		iNewYPos -= iAdjust;

		// Make sure it doesn't move off the top of the screen (the menu's too big to fit it all)
		if ( iNewYPos < 0 )
		{
			iAdjust -= (0 - iNewYPos);
			iNewYPos = 0;
		}
	}

	// We need to force all menus below this one to update their positions now, because they
	// might have submenus riding off buttons in this menu that have just shifted.
	for (int i = 0; i < m_iButtons; i++)
		m_aButtons[i]->UpdateSubMenus( iAdjust );

	setPos( _pos[0], iNewYPos );
}


// Make this menu and all menus above it in the chain visible
void CCommandMenu::MakeVisible( CCommandMenu *pChildMenu )
{
/*
	// Push down the button leading to the child menu
	for (int i = 0; i < m_iButtons; i++)
	{
		if ( (pChildMenu != NULL) && (m_aButtons[i]->GetSubMenu() == pChildMenu) )
		{
			m_aButtons[i]->setArmed( true );
		}
		else
		{
			m_aButtons[i]->setArmed( false );
		}
	}
*/

	setVisible(true);
	if (m_pParentMenu)
		m_pParentMenu->MakeVisible( this );
}

//================================================================
// CreateSubMenu
CCommandMenu *TeamFortressViewport::CreateSubMenu( CommandButton *pButton, CCommandMenu *pParentMenu )
{
	int iXPos = 0;
	int iYPos = 0;
	int iWide = CMENU_SIZE_X;
	int iTall = 0;

	if (pParentMenu)
	{
		iXPos = pParentMenu->GetXOffset() + CMENU_SIZE_X - 1;
		iYPos = pParentMenu->GetYOffset() + BUTTON_SIZE_Y * (m_pCurrentCommandMenu->GetNumButtons() - 1);
	}

	CCommandMenu *pMenu = new CCommandMenu(pParentMenu, iXPos, iYPos, iWide, iTall );
	pMenu->setParent(this);
	pButton->AddSubMenu( pMenu );
	pButton->setFont( Scheme::sf_primary3 );

	// Create the Submenu-open signal
	InputSignal *pISignal = new CMenuHandler_PopupSubMenuInput(pButton, pMenu);
	pButton->addInputSignal(pISignal);

	// Put a > to show it's a submenu
	CImageLabel *pLabel = new CImageLabel( "arrow", CMENU_SIZE_X - SUBMENU_SIZE_X, SUBMENU_SIZE_Y );
	pLabel->setParent(pButton);
	pLabel->addInputSignal(pISignal);

	// Reposition
	pLabel->getPos( iXPos, iYPos );
	pLabel->setPos( CMENU_SIZE_X - pLabel->getImageWide(), (BUTTON_SIZE_Y - pLabel->getImageTall()) / 2 );

	// Create the mouse off signal for the Label too
	if (!pButton->m_bNoHighlight)
		pLabel->addInputSignal( new CHandler_CommandButtonHighlight(pButton) );

	return pMenu;
}

//-----------------------------------------------------------------------------
// Purpose: Makes sure the memory allocated for TeamFortressViewport is nulled out
// Input  : stAllocateBlock - 
// Output : void *
//-----------------------------------------------------------------------------
void *TeamFortressViewport::operator new( size_t stAllocateBlock )
{
//	void *mem = Panel::operator new( stAllocateBlock );
	void *mem = ::operator new( stAllocateBlock );
	memset( mem, 0, stAllocateBlock );
	return mem;
}

//-----------------------------------------------------------------------------
// Purpose: InputSignal handler for the main viewport
//-----------------------------------------------------------------------------
class CViewPortInputHandler : public InputSignal
{
public:
	bool bPressed;

	CViewPortInputHandler()
	{
	}

	virtual void cursorMoved(int x,int y,Panel* panel) {}
	virtual void cursorEntered(Panel* panel) {}
	virtual void cursorExited(Panel* panel) {}
	virtual void mousePressed(MouseCode code,Panel* panel) 
	{
		if ( code != MOUSE_LEFT )
		{
			// send a message to close the command menu
			// this needs to be a message, since a direct call screws the timing
			gEngfuncs.pfnClientCmd( "ForceCloseCommandMenu\n" );
		}
	}
	virtual void mouseReleased(MouseCode code,Panel* panel)
	{
	}

	virtual void mouseDoublePressed(MouseCode code,Panel* panel) {}
	virtual void mouseWheeled(int delta,Panel* panel) {}
	virtual void keyPressed(KeyCode code,Panel* panel) {}
	virtual void keyTyped(KeyCode code,Panel* panel) {}
	virtual void keyReleased(KeyCode code,Panel* panel) {}
	virtual void keyFocusTicked(Panel* panel) {}
};


//================================================================
TeamFortressViewport::TeamFortressViewport(int x,int y,int wide,int tall) : Panel(x,y,wide,tall), m_SchemeManager(wide,tall)
{
	gViewPort = this;
	m_iInitialized = false;
	m_pScoreBoard = NULL;
	m_pSpectatorMenu = NULL;
	m_pSpectatorMenu2 = NULL;
	m_pCurrentMenu = NULL;
	m_pCurrentCommandMenu = NULL;
   m_pStatsMenu = NULL;
   m_pTop32Menu = NULL;
   m_pAdminMenu = NULL;

	CVAR_CREATE( "hud_classautokill", "1", FCVAR_ARCHIVE );		// controls whether or not to suicide immediately on TF class switch
	CVAR_CREATE( "hud_takesshots", "0", FCVAR_ARCHIVE );		// controls whether or not to automatically take screenshots at the end of a round

	Initialize();
	addInputSignal( new CViewPortInputHandler );

	int r, g, b, a;
	
	Scheme* pScheme = App::getInstance()->getScheme();

	// primary text color
	// Get the colors
	//!! two different types of scheme here, need to integrate
	SchemeHandle_t hPrimaryScheme = m_SchemeManager.getSchemeHandle( "Primary Button Text" );
	{
		// font
		pScheme->setFont( Scheme::sf_primary1, m_SchemeManager.getFont(hPrimaryScheme) );

		// text color
		//m_SchemeManager.getFgColor( hPrimaryScheme, r, g, b, a );
		//pScheme->setColor(Scheme::sc_primary1, r, g, b, a );		// sc_primary1 is non-transparent orange
      pScheme->setColor(Scheme::sc_primary1, SCORE_FG_RED, SCORE_FG_GREEN, SCORE_FG_BLUE, 0 );

		// background color (transparent black)
		//m_SchemeManager.getBgColor( hPrimaryScheme, r, g, b, a );
		// pScheme->setColor(Scheme::sc_primary3, r, g, b, a );
      pScheme->setColor(Scheme::sc_primary3, 0x00, 0x10, 0x20, 100 );

		// armed foreground color
		//m_SchemeManager.getFgArmedColor( hPrimaryScheme, r, g, b, a );
		//pScheme->setColor(Scheme::sc_secondary2, r, g, b, a );
      pScheme->setColor(Scheme::sc_secondary2, 0xFF, 0x40, 0x0, 0x80 );

		// armed background color
		//m_SchemeManager.getBgArmedColor( hPrimaryScheme, r, g, b, a );
		//pScheme->setColor(Scheme::sc_primary2, r, g, b, a );
      pScheme->setColor(Scheme::sc_primary2, FALSE_RED, FALSE_GREEN, FALSE_BLUE, 0);

		//!! need to get this color from scheme file
		// used for orange borders around buttons
		m_SchemeManager.getBorderColor( hPrimaryScheme, r, g, b, a );
		// pScheme->setColor(Scheme::sc_secondary1, r, g, b, a );
		pScheme->setColor(Scheme::sc_secondary1, TRUE_RED, TRUE_GREEN, TRUE_BLUE, 0);
	}

	// Change the second primary font (used in the scoreboard)
	SchemeHandle_t hScoreboardScheme = m_SchemeManager.getSchemeHandle( "Scoreboard Text" );
	{
		pScheme->setFont(Scheme::sf_primary2, m_SchemeManager.getFont(hScoreboardScheme) );
	}
	
	// Change the third primary font (used in command menu)
	SchemeHandle_t hCommandMenuScheme = m_SchemeManager.getSchemeHandle( "CommandMenu Text" );
	{
		pScheme->setFont(Scheme::sf_primary3, m_SchemeManager.getFont(hCommandMenuScheme) );
	}

	App::getInstance()->setScheme(pScheme);

	// VGUI MENUS
	CreateScoreBoard();
	CreateServerBrowser();
	CreateSpectatorMenu();
   CreateStatsMenu();
   CreateTop32Menu();
   CreateAdminMenu();
   CreateVoteMapMenu();
}

//-----------------------------------------------------------------------------
// Purpose: Called everytime a new level is started. Viewport clears out it's data.
//-----------------------------------------------------------------------------
void TeamFortressViewport::Initialize( void )
{
	if (m_pScoreBoard)
	   {
		m_pScoreBoard->Initialize();
		HideScoreBoard();
	   }
	if (m_pSpectatorMenu)
	   {
		// Spectator menu doesn't need initializing
		m_pSpectatorMenu->setVisible( false );
	   }

	if (m_pSpectatorMenu2)
	   {
		// Spectator menu doesn't need initializing
		m_pSpectatorMenu2->setVisible( false );
	   }
   if (m_pStatsMenu)
	   {
		m_pStatsMenu->setVisible( false );
	   }
   if (m_pTop32Menu)
	   {
		m_pTop32Menu->setVisible( false );
	   }
   if (m_pAdminMenu)
	   {
		m_pAdminMenu->setVisible( false );
	   }

	// Make sure all menus are hidden
	HideVGUIMenu();
	HideCommandMenu();

	// Clear out some data
	m_flScoreBoardLastUpdated = 0;

	// reset player info
	g_iPlayerClass = 0;
	g_iTeamNumber = 0;

	strcpy(m_sMapName, "");
	strcpy(m_szServerName, "");

	App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none) );
}

class CException;
//-----------------------------------------------------------------------------
// Purpose: Read the Command Menu structure from the txt file and create the menu.
//-----------------------------------------------------------------------------
void TeamFortressViewport::CreateCommandMenu( void )
{
	m_iInitialized = false;
}

void TeamFortressViewport::ToggleServerBrowser()
{
	if (!m_iInitialized)
		return;

	if ( !m_pServerBrowser )
		return;

	if ( m_pServerBrowser->isVisible() )
	{
		m_pServerBrowser->setVisible( false );
	}
	else
	{
		m_pServerBrowser->setVisible( true );
	}

	UpdateCursorState();
}

//=======================================================================
void TeamFortressViewport::ShowCommandMenu()
{
	if (!m_iInitialized)
		return;

	// Not visible while undefined
	if (g_iPlayerClass == 0)
		return;

	// is the command menu open?
	if ( m_pCurrentCommandMenu )
	{
		HideCommandMenu();
		return;
	}

	// Not visible while in intermission
	if ( gHUD.m_iIntermission )
		return;

	// Recalculate visible menus
	UpdateCommandMenu();
	HideVGUIMenu();

	SetCurrentCommandMenu( m_pCommandMenus[0] );
	m_flMenuOpenTime = gHUD.m_flTime;
	UpdateCursorState();

	// get command menu parameters
	for ( int i = 2; i < gEngfuncs.Cmd_Argc(); i++ )
	{
		const char *param = gEngfuncs.Cmd_Argv( i - 1 );
		if ( param )
		{
			if ( m_pCurrentCommandMenu->KeyInput(param[0]) )
			{
				// kill the menu open time, since the key input is final
				HideCommandMenu();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles the key input of "-commandmenu"
// Input  : 
//-----------------------------------------------------------------------------
void TeamFortressViewport::InputSignalHideCommandMenu()
{
	if (!m_iInitialized)
		return;

	// if they've just tapped the command menu key, leave it open
	if ( (m_flMenuOpenTime + 0.3) > gHUD.m_flTime )
		return;

	HideCommandMenu();
}

//-----------------------------------------------------------------------------
// Purpose: Hides the command menu
//-----------------------------------------------------------------------------
void TeamFortressViewport::HideCommandMenu( void )
{
	if (!m_iInitialized)
		return;

	if ( m_pCommandMenus[0] )
	{
		m_pCommandMenus[0]->ClearButtonsOfArmedState();
	}

	m_flMenuOpenTime = 0.0f;
	SetCurrentCommandMenu( NULL );
	UpdateCursorState();
}

//-----------------------------------------------------------------------------
// Purpose: Bring up the scoreboard
//-----------------------------------------------------------------------------
void TeamFortressViewport::ShowScoreBoard( void )
{
	if (m_pScoreBoard)
	   {
		// No Scoreboard in single-player
		if ( gEngfuncs.GetMaxClients() >= 1 )
		   {
			m_pScoreBoard->Open();
			UpdateCursorState();
		   }
	   }
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if the scoreboard is up
//-----------------------------------------------------------------------------
bool TeamFortressViewport::IsScoreBoardVisible( void )
{
	if (m_pScoreBoard)
		return m_pScoreBoard->isVisible();

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Hide the scoreboard
//-----------------------------------------------------------------------------
void TeamFortressViewport::HideScoreBoard( void )
{
	// Prevent removal of scoreboard during intermission
	if ( gHUD.m_iIntermission )
		return;

	if (m_pScoreBoard)
	{
		m_pScoreBoard->setVisible(false);
		UpdateCursorState();
	}
}

// Set the submenu of the Command Menu
void TeamFortressViewport::SetCurrentCommandMenu( CCommandMenu *pNewMenu )
{
	for (int i = 0; i < m_iNumMenus; i++)
		m_pCommandMenus[i]->setVisible(false);

	m_pCurrentCommandMenu = pNewMenu;

	if (m_pCurrentCommandMenu)
		m_pCurrentCommandMenu->MakeVisible( NULL );
}

void TeamFortressViewport::UpdateCommandMenu()
{
	m_pCommandMenus[0]->RecalculateVisibles( 0, false );
	m_pCommandMenus[0]->RecalculatePositions( 0 );
}

void TeamFortressViewport::UpdateSpectatorMenu()
{
	char sz[64];

	if (!m_pSpectatorMenu)
		return;

	if (m_iUser1)
	{
		m_pSpectatorMenu->setVisible( true );
		m_pSpectatorMenu2->setVisible( true );

		if (m_iUser2 > 0)
		{
			// Locked onto a target, show the player's name
			sprintf(sz, "#Spec_Mode%d : %s", m_iUser1, g_PlayerInfoList[ m_iUser2 ].name);
			m_pSpectatorLabel->setText( CHudTextMessage::BufferedLocaliseTextString( sz ) );
		}
		else
		{
			sprintf(sz, "#Spec_Mode%d", m_iUser1);
			m_pSpectatorLabel->setText( CHudTextMessage::BufferedLocaliseTextString( sz ) );
		}
	}
	else
	{
		m_pSpectatorMenu->setVisible( false );
		m_pSpectatorMenu2->setVisible( false );
	}
}

//======================================================================
void TeamFortressViewport::CreateScoreBoard( void )
{
	m_pScoreBoard = new ScorePanel(SBOARD_INDENT_X,SBOARD_INDENT_Y, ScreenWidth - (SBOARD_INDENT_X * 2), ScreenHeight - (SBOARD_INDENT_Y * 2));
	m_pScoreBoard->setParent(this);
	m_pScoreBoard->setVisible(false);
}

void TeamFortressViewport::CreateServerBrowser( void )
{
	m_pServerBrowser = new ServerBrowser( 0, 0, ScreenWidth, ScreenHeight );
	m_pServerBrowser->setParent(this);
	m_pServerBrowser->setVisible(false);
}


//======================================================================
// Set the VGUI Menu
void TeamFortressViewport::SetCurrentMenu( CMenuPanel *pMenu )
{
	m_pCurrentMenu = pMenu;
	if ( m_pCurrentMenu )
	{
		// Don't open menus in demo playback
		if ( gEngfuncs.pDemoAPI->IsPlayingback() )
			return;

		m_pCurrentMenu->Open();
	}
}

//================================================================
// VGUI Menus
void TeamFortressViewport::ShowVGUIMenu( int iMenu )
{
	CMenuPanel *pNewMenu = NULL;

	// Don't open menus in demo playback
	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
		return;

	// Don't create one if it's already in the list
	if (m_pCurrentMenu)
	   {
		CMenuPanel *pMenu = m_pCurrentMenu;
		while (pMenu != NULL)
		   {
			if (pMenu->GetMenuID() == iMenu)
            {
            if (pMenu->IsActive())
               pMenu->setVisible(true);
				return;
            }
			pMenu = pMenu->GetNextMenu();
		   }
      return;
	   }

   switch (iMenu)
      {
      case MENU_STATSMENU:
		   pNewMenu = ShowStatsMenu();
		   break;
      case MENU_TOP32MENU:
		   pNewMenu = ShowTop32Menu();
		   break;
      case MENU_ADMINMENU:
		   pNewMenu = ShowAdminMenu();
		   break;
      case MENU_VOTEMAPS:
		   pNewMenu = ShowVoteMapMenu();
		   break;
	   default:
		   break;
      }

   if (!pNewMenu)
		return;

	// Close the Command Menu if it's open
	HideCommandMenu();

	pNewMenu->SetMenuID( iMenu );
	pNewMenu->SetActive( true );

	// See if another menu is visible, and if so, cache this one for display once the other one's finished
	if (m_pCurrentMenu)
	{
		if ( m_pCurrentMenu->GetMenuID() == MENU_CLASS && iMenu == MENU_TEAM )
		{
			CMenuPanel *temp = m_pCurrentMenu;
			m_pCurrentMenu->Close();
			m_pCurrentMenu = pNewMenu;
			m_pCurrentMenu->SetNextMenu( temp );
			m_pCurrentMenu->Open();
			UpdateCursorState();
		}
		else
		{
			m_pCurrentMenu->SetNextMenu( pNewMenu );
		}
	}
	else
	{
		m_pCurrentMenu = pNewMenu;
		m_pCurrentMenu->Open();
		UpdateCursorState();
	}
}

// Removes all VGUI Menu's onscreen
void TeamFortressViewport::HideVGUIMenu()
{
	while (m_pCurrentMenu)
		HideTopMenu();
}

// Remove the top VGUI menu, and bring up the next one
void TeamFortressViewport::HideTopMenu()
{
	if (m_pCurrentMenu)
	{
		// Close the top one
		m_pCurrentMenu->Close();

		// Bring up the next one
		gViewPort->SetCurrentMenu( m_pCurrentMenu->GetNextMenu() );
	}

	UpdateCursorState();
}

// Return TRUE if the HUD's allowed to print text messages
bool TeamFortressViewport::AllowedToPrintText( void )
{
	// Prevent text messages when fullscreen menus are up
	if ( m_pCurrentMenu && g_iPlayerClass == 0 )
	{
		int iId = m_pCurrentMenu->GetMenuID();
		if ( iId == MENU_TEAM || iId == MENU_CLASS || iId == MENU_INTRO || iId == MENU_CLASSHELP )
			return FALSE;
	}

	return TRUE;
}

//======================================================================================
// SPECTATOR MENU
//======================================================================================
// Spectator "Menu" explaining the Spectator buttons
void TeamFortressViewport::CreateSpectatorMenu()
{
	// Create the Panel
	m_pSpectatorMenu = new CTransparentPanel(1, 0, ScreenHeight - YRES(59), ScreenWidth, YRES(60));
	m_pSpectatorMenu->setParent(this);
	m_pSpectatorMenu->setVisible(false);

   m_pSpectatorMenu2 = new CTransparentPanel(1, 0, 0, ScreenWidth, YRES(60));
	m_pSpectatorMenu2->setParent(this);
	m_pSpectatorMenu2->setVisible(false);

	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
	SchemeHandle_t hHelpText = pSchemes->getSchemeHandle( "Primary Button Text" );

	// color schemes
	int r, g, b, a;

	// Create the title
	m_pSpectatorLabel = new Label( "Spectator", 0, 0, ScreenWidth, YRES(25) );
	m_pSpectatorLabel->setParent( m_pSpectatorMenu );
	m_pSpectatorLabel->setFont( pSchemes->getFont(hTitleScheme) );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
   r = TRUE_RED; g = TRUE_GREEN; b = TRUE_BLUE;
	m_pSpectatorLabel->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	m_pSpectatorLabel->setBgColor( r, g, b, 255 );
	m_pSpectatorLabel->setContentAlignment( vgui::Label::a_north );

	// Create the Help
	Label *pLabel = new Label( CHudTextMessage::BufferedLocaliseTextString( "#Spec_Help" ), 0, YRES(25), ScreenWidth, YRES(15) );
	pLabel->setParent( m_pSpectatorMenu );
	pLabel->setFont( pSchemes->getFont(hHelpText) );
	pSchemes->getFgColor( hHelpText, r, g, b, a );
   r = FALSE_RED; g = FALSE_GREEN; b = FALSE_BLUE;
	pLabel->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hHelpText, r, g, b, a );
	pLabel->setBgColor( r, g, b, 255 );
	pLabel->setContentAlignment( vgui::Label::a_north );

	pLabel = new Label( CHudTextMessage::BufferedLocaliseTextString( "#Spec_Help2" ), 0, YRES(40), ScreenWidth, YRES(20) );
	pLabel->setParent( m_pSpectatorMenu );
	pLabel->setFont( pSchemes->getFont(hHelpText) );
	pSchemes->getFgColor( hHelpText, r, g, b, a );
   r = FALSE_RED; g = FALSE_GREEN; b = FALSE_BLUE;
	pLabel->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hHelpText, r, g, b, a );
	pLabel->setBgColor( r, g, b, 255 );
	pLabel->setContentAlignment( vgui::Label::a_center );
}

//======================================================================================
// UPDATE HUD SECTIONS
//======================================================================================
// We've got an update on player info
// Recalculate any menus that use it.
void TeamFortressViewport::UpdateOnPlayerInfo()
{
	if (m_pScoreBoard)
		m_pScoreBoard->Update();
}

void TeamFortressViewport::UpdateCursorState()
{
	// Need cursor if any VGUI window is up
	if ( m_pCurrentMenu || m_pServerBrowser->isVisible() )
	   {
		g_iVisibleMouse = true;
		App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow) );
		return;
	   }
	else if ( m_pCurrentCommandMenu )
	   {
		// commandmenu doesn't have cursor if hud_capturemouse is turned off
		if ( gHUD.m_pCvarStealMouse->value != 0.0f )
		   {
			g_iVisibleMouse = true;
			App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow) );
			return;
		   }
	   }

	IN_ResetMouse();
	g_iVisibleMouse = false;
	App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none) );
}

void TeamFortressViewport::UpdateHighlights()
{
	if (m_pCurrentCommandMenu)
		m_pCurrentCommandMenu->MakeVisible( NULL );
}

void TeamFortressViewport::GetAllPlayersInfo( void )
{
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	   {
		GetPlayerInfo( i, &g_PlayerInfoList[i] );

		if ( g_PlayerInfoList[i].thisplayer )
			m_pScoreBoard->m_iPlayerNum = i;  // !!!HACK: this should be initialized elsewhere... maybe gotten from the engine
	   }
}

void TeamFortressViewport::paintBackground()
{
	// See if the Spectator Menu needs to be update
	if ( g_iUser1 != m_iUser1 || g_iUser2 != m_iUser2 )
	   {
		m_iUser1 = g_iUser1;
		m_iUser2 = g_iUser2;
		UpdateSpectatorMenu();
	   }

	// Update the Scoreboard, if it's visible
	if ( m_pScoreBoard->isVisible() && (m_flScoreBoardLastUpdated < gHUD.m_flTime) )
	   {
		m_pScoreBoard->Update();
		m_flScoreBoardLastUpdated = gHUD.m_flTime + 2.5;
	   }

	int extents[4];
	getAbsExtents(extents[0],extents[1],extents[2],extents[3]);
	VGui_ViewportPaintBackground(extents);
   if (m_pScoreBoard->isVisible())
      {
      m_pSpectatorMenu->m_iOn = false;
      m_pSpectatorMenu2->m_iOn = false;
      }
   else
      {
      m_pSpectatorMenu->m_iOn = true;
      m_pSpectatorMenu2->m_iOn = true;
      if (m_pSpectatorMenu && m_pSpectatorMenu->isVisible())
         {
         //s_bottom.x -= 2;
         s_top.x--;
         if (s_top.x < -s_top.length) ClientCmd("s0");
         //if (s_bottom.x < -s_bottom.length) ClientCmd("s1");
         gEngfuncs.pfnDrawSetTextColor(FALSE_RED/255.0f, FALSE_GREEN/255.0f, FALSE_BLUE/255.0f);
         gHUD.DrawHudString(s_top.x, ScreenHeight - YRES(59) - 16, ScreenWidth, s_top.data, FALSE_RED/2, FALSE_GREEN/2, FALSE_BLUE/2);
         //DrawConsoleString(s_bottom.x, ScreenHeight - YRES(59) - 16 - 12, s_bottom.data);
         }
      }
}

void CDragNDropHandler::cursorMoved(int x,int y,Panel* panel)
{
	if(m_bDragging)
	{
		App::getInstance()->getCursorPos(x,y);			
		m_pPanel->setPos(m_iaDragOrgPos[0]+(x-m_iaDragStart[0]),m_iaDragOrgPos[1]+(y-m_iaDragStart[1]));
		
		if(m_pPanel->getParent()!=null)
		{			
			m_pPanel->getParent()->repaint();
		}
	}
}

void CDragNDropHandler::mousePressed(MouseCode code,Panel* panel)
{
	int x,y;
	App::getInstance()->getCursorPos(x,y);
	m_bDragging=true;
	m_iaDragStart[0]=x;
	m_iaDragStart[1]=y;
	m_pPanel->getPos(m_iaDragOrgPos[0],m_iaDragOrgPos[1]);
	App::getInstance()->setMouseCapture(panel);

	m_pPanel->setDragged(m_bDragging);
	m_pPanel->requestFocus();
} 

void CDragNDropHandler::mouseReleased(MouseCode code,Panel* panel)
{
	m_bDragging=false;
	m_pPanel->setDragged(m_bDragging);
	App::getInstance()->setMouseCapture(null);
}

bool TeamFortressViewport::SlotInput( int iSlot )
{
	// If there's a menu up, give it the input
	if ( m_pCurrentMenu )
		return m_pCurrentMenu->SlotInput( iSlot );

	return FALSE;
}

// Direct Key Input
int	TeamFortressViewport::KeyInput( int down, int keynum, const char *pszCurrentBinding )
{
   int iMenuID = 0;
	// Open Text Window?
	if (m_pCurrentMenu && gEngfuncs.Con_IsVisible() == false)
	   {
		int iMenuID = m_pCurrentMenu->GetMenuID();
		}

		// Grab enter keys to close TextWindows
		if ( down && (keynum == K_ENTER || keynum == K_KP_ENTER || keynum == K_SPACE || keynum == K_ESCAPE) )
			if (iMenuID == MENU_INTRO)
			   {
				HideTopMenu();
				return 0;
			   }

	// if we're in a command menu, try hit one of it's buttons
	if ( down && m_pCurrentCommandMenu )
	   {
		// Escape hides the command menu
		if ( keynum == K_ESCAPE )
		   {
			HideCommandMenu();
			return 0;
		   }

		// only trap the number keys
		if ( keynum >= '0' && keynum <= '9' )
		   {
			if ( m_pCurrentCommandMenu->KeyInput(keynum) )
				HideCommandMenu();
			return 0;
		   }
	   }

	return 1;
}

int TeamFortressViewport::MsgFunc_VGUIMenu(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iMenu = READ_BYTE();
	ShowVGUIMenu( iMenu );
	return 1;
}

int TeamFortressViewport::MsgFunc_PlayBeat(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int beat = READ_BYTE();
	if (!beat) MP3_Stop();
   else MP3_Play(beat - 1);
	return 1;
}

int TeamFortressViewport::MsgFunc_PlayMp3(const char *pszName, int iSize, void *pbuf )
{
   int loop;
	BEGIN_READ( pbuf, iSize );
   loop = READ_BYTE();
   MP3_DYNPlay(READ_STRING(), loop);
	return 1;
}

int TeamFortressViewport::MsgFunc_PlayTheme(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int beat = READ_BYTE();
	if (!beat) MP3_Theme_Stop();
   else MP3_Theme_Play(beat - 1);
	return 1;
}

int TeamFortressViewport::MsgFunc_SendStats(const char *pszName, int iSize, void *pbuf )
{
   /*
   unsigned int xxx;
   gstat.WonID = (unsigned short) READ_SHORT();
   xxx = (unsigned short) READ_SHORT();
   xxx <<= 16L;
   xxx &= 0xFFFF0000;
   gstat.WonID |= xxx;
   */

	BEGIN_READ( pbuf, iSize );
   gstat.ClientID = READ_SHORT();
   gstat.Juice = READ_SHORT();
   gstat.Health = READ_SHORT();
   gstat.Shots = READ_SHORT();
   gstat.Hits = READ_SHORT();
   gstat.MaxRoundKills = READ_SHORT();
   gstat.MaxRoundLives = READ_SHORT();
   gstat.RoundKills = READ_SHORT();
   gstat.Rounds = READ_SHORT();
   gstat.Offense = READ_SHORT();
   gstat.Defense = READ_SHORT();
   gstat.Suicides = READ_SHORT();
   gstat.TimeOn = READ_SHORT();
   gstat.LastKills = READ_SHORT();
   gstat.Frags = READ_SHORT();
   gstat.Deaths = READ_SHORT();
   gstat.KnifeKills = READ_SHORT();
   gstat.HeadShots = READ_SHORT();
   gstat.FavPrimary = READ_SHORT();
   gstat.PrimaryKills = READ_SHORT();
   gstat.FavSecondary = READ_SHORT();
   gstat.SecondaryKills = READ_SHORT();
	return 1;
}

int TeamFortressViewport::MsgFunc_SendSteamID(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
   strcpy(gstat.SteamID, READ_STRING());
	ShowVGUIMenu( MENU_STATSMENU );
   return 1;
}

int TeamFortressViewport::MsgFunc_SendTop32(const char *pszName, int iSize, void *pbuf )
{
   static char *ssTop32 = sTop32;  
	BEGIN_READ( pbuf, iSize );
   switch (READ_BYTE())
      {
      case 1:
         ssTop32 = sTop32;
         strcpy( ssTop32, READ_STRING());
         ssTop32 += 80;
         break;
      case 2:
         strcpy( ssTop32, READ_STRING());
         ssTop32 += 80;
         break;
      case 3:
         strcpy( ssTop32, READ_STRING());
         ssTop32 += 80;
	      ShowVGUIMenu( MENU_TOP32MENU );
         break;
      }
	return 1;
}

int TeamFortressViewport::MsgFunc_VoteMaps(const char *pszName, int iSize, void *pbuf )
{
   int num_maps;
   int k;
   
   memset(map_vote, 0, sizeof(map_vote_t) * MAX_MAP_VOTES);
	BEGIN_READ( pbuf, iSize );
   num_players = READ_BYTE();
   num_maps = READ_BYTE();
   for (k = 0; k < num_maps; k++)
      {
      strcpy(map_vote[k].name, READ_STRING());
      map_vote[k].votes = READ_BYTE();
      }
   /*
   static char *ssTop32 = sTop32;  
   switch (READ_BYTE())
      {
      case 1:
         ssTop32 = sTop32;
         strcpy( ssTop32, READ_STRING());
         ssTop32 += 80;
         break;
      case 2:
         strcpy( ssTop32, READ_STRING());
         ssTop32 += 80;
         break;
      case 3:
         strcpy( ssTop32, READ_STRING());
         ssTop32 += 80;
	      ShowVGUIMenu( MENU_TOP32MENU );
         break;
      }
   */
   ShowVGUIMenu( MENU_VOTEMAPS );
	return 1;
}

int TeamFortressViewport::MsgFunc_AdminMenu(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
   admin_torture = READ_SHORT();
	ShowVGUIMenu( MENU_ADMINMENU );
	return 1;
}


int TeamFortressViewport::MsgFunc_ServerName( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	strncpy( m_szServerName, READ_STRING(), MAX_SERVERNAME_LENGTH );
	return 1;
}

int TeamFortressViewport::MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();
	short frags = READ_SHORT();
	short deaths = READ_SHORT();
	if ( cl > 0 && cl <= MAX_PLAYERS )
	   {
		g_PlayerExtraInfo[cl].frags = frags;
		g_PlayerExtraInfo[cl].deaths = deaths;
		UpdateOnPlayerInfo();
	   }
	return 1;
}

int TeamFortressViewport::MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
   int i = READ_BYTE();
	g_TeamInfo[i].scores_overriden = TRUE;
	g_TeamInfo[i].frags = READ_SHORT();
	g_TeamInfo[i].deaths = READ_SHORT();
	return 1;
}

int TeamFortressViewport::MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();

	if ( cl > 0 && cl <= MAX_PLAYERS )
		strncpy( g_PlayerExtraInfo[cl].teamname, READ_STRING(), MAX_TEAM_NAME );

	if (m_pScoreBoard)
	   m_pScoreBoard->RebuildTeams();

	return 1;
} 

int TeamFortressViewport::MsgFunc_TeamIndex( const char *pszName, int iSize, void *pbuf )
{
	if (!m_pScoreBoard)
		return 1;

	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();
	if ( cl > 0 && cl <= MAX_PLAYERS )
      g_PlayerExtraInfo[cl].indexnumber = READ_BYTE();
	// rebuild the list of teams
	m_pScoreBoard->RebuildTeams();
	return 1;
}

typedef enum
	{
	at_notice,
	at_console,		// same as at_notice, but forces a ConPrintf, not a message box
	at_aiconsole,	// same as at_console, but only shown if developer level is 2!
	at_warning,
	at_error,
	at_logged		// Server print to console ( only in multiplayer games ).
	} ALERT_TYPE;

void AlertMessage( ALERT_TYPE atype, char *szFmt, ... );
int TeamFortressViewport::MsgFunc_CLsHealth( const char *pszName, int iSize, void *pbuf )
{
   char *pHealth = NULL;
   short num_clients;
   short curr_client, i;
	BEGIN_READ( pbuf, iSize );
   g_TeamInfo[1].wins = READ_SHORT();
   g_TeamInfo[2].wins = READ_SHORT();
   num_clients = READ_BYTE();
   for (i = 0; i < num_clients; i++)
      {
      curr_client = READ_BYTE();
      if (curr_client > 0 && curr_client <= MAX_PLAYERS)
         {
         g_PlayerExtraInfo[curr_client].health = READ_BYTE();
         g_PlayerExtraInfo[curr_client].juice = READ_WORD();
         if (g_PlayerExtraInfo[curr_client].juice < 0)
            {
            g_PlayerExtraInfo[curr_client].juice = -g_PlayerExtraInfo[curr_client].juice;
            g_PlayerExtraInfo[curr_client].rank = READ_BYTE();
            }
         else g_PlayerExtraInfo[curr_client].rank = -1;


         }
      }
   return 1;
}

void TeamFortressViewport::DeathMsg( int killer, int victim )
{
	m_pScoreBoard->DeathMsg(killer,victim);
}

int TeamFortressViewport::MsgFunc_Spectator( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();
	if ( cl > 0 && cl <= MAX_PLAYERS )
		g_IsSpectator[cl] = READ_BYTE();
	return 1;
}

int TeamFortressViewport::MsgFunc_AllowSpec( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iAllowSpectators = READ_BYTE();
	// Force the menu to update
	UpdateCommandMenu();
	return 1;
}



//======================================================================================
// STATS MENU
//======================================================================================
// Show the StatsMenu
CMenuPanel* TeamFortressViewport::ShowStatsMenu()
{
	// Don't open menus in demo playback
	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
		return NULL;

	m_pStatsMenu->Reset();
	return m_pStatsMenu;
}

void TeamFortressViewport::CreateStatsMenu()
{
	// Create the panel
	m_pStatsMenu = new CStatsMenu(100, false, 0, 0, ScreenWidth, ScreenHeight);
	m_pStatsMenu->setParent(this);
	m_pStatsMenu->setVisible( false );
}

CMenuPanel* TeamFortressViewport::ShowTop32Menu()
{
	// Don't open menus in demo playback
	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
		return NULL;

	m_pTop32Menu->Reset();
	return m_pTop32Menu;
}

void TeamFortressViewport::CreateTop32Menu()
{
	// Create the panel
	m_pTop32Menu = new CTop32Menu(100, false, 0, 0, ScreenWidth, ScreenHeight);
	m_pTop32Menu->setParent(this);
	m_pTop32Menu->setVisible( false );
}

CMenuPanel* TeamFortressViewport::ShowAdminMenu()
{
	// Don't open menus in demo playback
	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
		return NULL;

	m_pAdminMenu->Reset();
	return m_pAdminMenu;
}

void TeamFortressViewport::CreateAdminMenu()
{
	// Create the panel
	m_pAdminMenu = new CAdminMenu(100, false, 0, 0, ScreenWidth, ScreenHeight);
	m_pAdminMenu->setParent(this);
	m_pAdminMenu->setVisible( false );
}

void TeamFortressViewport::CreateVoteMapMenu()
{
	// Create the panel
	m_pVoteMapMenu = new CVoteMapMenu(100, false, 0, 0, ScreenWidth, ScreenHeight);
	m_pVoteMapMenu->setParent(this);
	m_pVoteMapMenu->setVisible( false );
}

CMenuPanel* TeamFortressViewport::ShowVoteMapMenu()
{
	// Don't open menus in demo playback
	if ( gEngfuncs.pDemoAPI->IsPlayingback() )
		return NULL;

	m_pVoteMapMenu->Reset();
	return m_pVoteMapMenu;
}

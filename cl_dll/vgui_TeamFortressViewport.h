
#ifndef TEAMFORTRESSVIEWPORT_H
#define TEAMFORTRESSVIEWPORT_H

#include<VGUI_Panel.h>
#include<VGUI_Frame.h>
#include<VGUI_TextPanel.h>
#include<VGUI_Label.h>
#include<VGUI_Button.h>
#include<VGUI_ActionSignal.h>
#include<VGUI_InputSignal.h>
#include<VGUI_Scheme.h>
#include<VGUI_Image.h>
#include<VGUI_FileInputStream.h>
#include<VGUI_BitmapTGA.h>
#include<VGUI_DesktopIcon.h>
#include<VGUI_App.h>
#include<VGUI_MiniApp.h>
#include<VGUI_LineBorder.h>
#include<VGUI_String.h>
#include<VGUI_ScrollPanel.h>
#include<VGUI_ScrollBar.h>
#include<VGUI_Slider.h>
#include<VGUI_TextEntry.h>

// custom scheme handling
#include "vgui_SchemeManager.h"

#define TF_DEFS_ONLY
#include "tf_defs.h"

using namespace vgui;

class Cursor;
class ScorePanel;
class CCommandMenu;
class CommandLabel;
class CommandButton;
class CMenuPanel;
class ServerBrowser;
class DragNDropPanel;
class CTransparentPanel;
class CTeamMenuPanel;
class CStatsMenu;
class CTop32Menu;
class CAdminMenu;
class CVoteMapMenu;

typedef struct 
   {
   short ClientID;
   short Juice;
   short Health;
   short Shots;
   short Hits;
   short MaxRoundKills;
   short MaxRoundLives;
   short RoundKills;
   short Rounds;
   short Offense;
   short Defense;
   short Suicides;
   short TimeOn;
   short LastKills;
   short Frags;
   short Deaths;
   short KnifeKills;
   short HeadShots;
   short FavPrimary;
   short FavSecondary;
   short PrimaryKills;
   short SecondaryKills;
   unsigned int WonID;
   char  SteamID[64];
   } sendstat_t;

extern sendstat_t gstat;
extern char sTop32[4096];

char* GetVGUITGAName(const char *pszName);
BitmapTGA *LoadTGA( const char* pImageName );
void ScaleColors( int &r, int &g, int &b, int a );
extern char *sTFClassSelection[];
extern int sTFValidClassInts[];
extern char *sLocalisedClasses[];
extern int iTeamColors[5][3];

#define MAX_SERVERNAME_LENGTH	32

// Use this to set any co-ords in 640x480 space
#define XRES(x)					(x  * ((float)ScreenWidth / 640))
#define YRES(y)					(y  * ((float)ScreenHeight / 480))

// Command Menu positions 
#define MAX_MENUS				40
#define MAX_BUTTONS				100

#define BUTTON_SIZE_Y			YRES(15)
#define CMENU_SIZE_X			XRES(130)

#define SUBMENU_SIZE_X			(CMENU_SIZE_X / 8)
#define SUBMENU_SIZE_Y			(BUTTON_SIZE_Y / 6)

#define CMENU_TOP				(BUTTON_SIZE_Y * 4)

#define MAX_TEAMNAME_SIZE		64
#define MAX_BUTTON_SIZE			32

// Map Briefing Window
#define MAPBRIEF_INDENT			30

// Team Menu
#define TMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define TMENU_HEADER			100
#define TMENU_SIZE_X			(ScreenWidth - (TMENU_INDENT_X * 2))
#define TMENU_SIZE_Y			(TMENU_HEADER + BUTTON_SIZE_Y * 7)
#define TMENU_PLAYER_INDENT		(((float)TMENU_SIZE_X / 3) * 2)
#define TMENU_INDENT_Y			(((float)ScreenHeight - TMENU_SIZE_Y) / 2)

// Class Menu
#define CLMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define CLMENU_HEADER			100
#define CLMENU_SIZE_X			(ScreenWidth - (CLMENU_INDENT_X * 2))
#define CLMENU_SIZE_Y			(CLMENU_HEADER + BUTTON_SIZE_Y * 11)
#define CLMENU_PLAYER_INDENT	(((float)CLMENU_SIZE_X / 3) * 2)
#define CLMENU_INDENT_Y			(((float)ScreenHeight - CLMENU_SIZE_Y) / 2)

// Arrows
enum
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
};

//==============================================================================
// VIEWPORT PIECES
//============================================================
// Wrapper for an Image Label without a background
class CImageLabel : public Label
{
public:
	BitmapTGA	*m_pTGA;

public:
	CImageLabel( const char* pImageName,int x,int y );
	CImageLabel( const char* pImageName,int x,int y,int wide,int tall );

	virtual int getImageTall();
	virtual int getImageWide();

	virtual void paintBackground()
	{
		// Do nothing, so the background's left transparent.
	}
};

// Command Label
// Overridden label so we can darken it when submenus open
class CommandLabel : public Label
{
private:
	int		m_iState;

public:
	CommandLabel(const char* text,int x,int y,int wide,int tall) : Label(text,x,y,wide,tall)
	{
		m_iState = false;
	}

	void PushUp()
	{
		m_iState = false;
		repaint();
	}

	void PushDown()
	{
		m_iState = true;
		repaint();
	}
};

//============================================================
// Command Buttons
class CommandButton : public Button
{
private:
	int		m_iPlayerClass;

	// Submenus under this button
	CCommandMenu *m_pSubMenu;
	CCommandMenu *m_pParentMenu;
	CommandLabel *m_pSubLabel;

	char m_sMainText[MAX_BUTTON_SIZE];
	char m_cBoundKey;

	SchemeHandle_t m_hTextScheme;

	void RecalculateText( void );

public:
	bool	m_bNoHighlight;

public:
	// Constructors
	CommandButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight = false);
	CommandButton( int iPlayerClass, const char* text,int x,int y,int wide,int tall);

	void Init( void );

	// Menu Handling
	void AddSubMenu( CCommandMenu *pNewMenu );
	void AddSubLabel( CommandLabel *pSubLabel )
	{
		m_pSubLabel = pSubLabel;
	}

	virtual int IsNotValid( void )
	{
		return false;
	}

	void UpdateSubMenus( int iAdjustment );
	int GetPlayerClass() { return m_iPlayerClass; };
	CCommandMenu *GetSubMenu() { return m_pSubMenu; };
	
	CCommandMenu *getParentMenu( void );
	void setParentMenu( CCommandMenu *pParentMenu );

	// Overloaded vgui functions
	virtual void paint();
	virtual void setText( const char *text );
	virtual void paintBackground();

	void cursorEntered( void );
	void cursorExited( void );

	void setBoundKey( char boundKey );
	char getBoundKey( void );
};

//============================================================
// Command Menus
class CCommandMenu : public Panel
{
private:
	CCommandMenu *m_pParentMenu;
	int			  m_iXOffset;
	int			  m_iYOffset;

	// Buttons in this menu
	CommandButton *m_aButtons[ MAX_BUTTONS ];
	int			  m_iButtons;

public:
	CCommandMenu( CCommandMenu *pParentMenu, int x,int y,int wide,int tall ) : Panel(x,y,wide,tall)
	{
		m_pParentMenu = pParentMenu;
		m_iXOffset = x;
		m_iYOffset = y;
		m_iButtons = 0;
	}

	void		AddButton( CommandButton *pButton );
	bool		RecalculateVisibles( int iNewYPos, bool bHideAll );
	void		RecalculatePositions( int iYOffset );
	void		MakeVisible( CCommandMenu *pChildMenu );

	CCommandMenu *GetParentMenu() { return m_pParentMenu; };
	int			GetXOffset() { return m_iXOffset; };
	int			GetYOffset() { return m_iYOffset; };
	int			GetNumButtons() { return m_iButtons; };
	CommandButton *FindButtonWithSubmenu( CCommandMenu *pSubMenu );

	void		ClearButtonsOfArmedState( void );


	bool		KeyInput( int keyNum );

	virtual void paintBackground();
};

//==============================================================================
class TeamFortressViewport : public Panel
{
private:
	vgui::Cursor* _cursorNone;
	vgui::Cursor* _cursorArrow;

	int			 m_iInitialized;

	CCommandMenu *m_pCommandMenus[ MAX_MENUS ];
	CCommandMenu *m_pCurrentCommandMenu;
	float		    m_flMenuOpenTime;
	float		    m_flScoreBoardLastUpdated;
	int			 m_iNumMenus;
	int			 m_iUser1;
	int			 m_iUser2;

	// VGUI Menus
	void		   CreateSpectatorMenu( void );
   void		   CreateStatsMenu( void );
	CMenuPanel  *ShowStatsMenu( void );
   void		   CreateTop32Menu( void );
	CMenuPanel  *ShowTop32Menu( void );
   void		   CreateAdminMenu( void );
	CMenuPanel  *ShowAdminMenu( void );
   void		   CreateVoteMapMenu( void );
	CMenuPanel  *ShowVoteMapMenu( void );

	
	// Scheme handler
	CSchemeManager m_SchemeManager;

	// Server Browser
	ServerBrowser *m_pServerBrowser;

	// Spectator "menu"
	CTransparentPanel	*m_pSpectatorMenu;
	CTransparentPanel	*m_pSpectatorMenu2;
	Label				   *m_pSpectatorLabel;
	int					m_iAllowSpectators;

	int		m_iRandomPC;
	char		m_sMapName[64];
public:
	TeamFortressViewport(int x,int y,int wide,int tall);
	void Initialize( void );

	void CreateCommandMenu( void );
	void CreateScoreBoard( void );
	void CreateServerBrowser( void );

	void UpdateCursorState( void );
	void UpdateCommandMenu( void );
	void UpdateOnPlayerInfo( void );
	void UpdateHighlights( void );
	void UpdateSpectatorMenu( void );

	int  KeyInput( int down, int keynum, const char *pszCurrentBinding );
	void GetAllPlayersInfo( void );
	void DeathMsg( int killer, int victim );

	void ShowCommandMenu( void );
	void InputSignalHideCommandMenu( void );
	void HideCommandMenu( void );
	void SetCurrentCommandMenu( CCommandMenu *pNewMenu );
	void SetCurrentMenu( CMenuPanel *pMenu );

	void ShowScoreBoard( void );
	void HideScoreBoard( void );
	bool IsScoreBoardVisible( void );

	bool AllowedToPrintText( void );

	void ShowVGUIMenu( int iMenu );
	void HideVGUIMenu( void );
	void HideTopMenu( void );

	void ToggleServerBrowser( void );

	CCommandMenu *CreateSubMenu( CommandButton *pButton, CCommandMenu *pParentMenu );

	// Data Handlers
	int GetAllowSpectators() { return m_iAllowSpectators; };

	// Message Handlers
	int MsgFunc_VGUIMenu(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_SendStats(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_SendSteamID(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_SendTop32(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_AdminMenu(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_PlayBeat(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_PlayMp3(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_PlayTheme(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_VoteMaps(const char *pszName, int iSize, void *pbuf );
   int MsgFunc_Menu(const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ServerName( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamIndex( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Spectator( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_AllowSpec( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_CLsHealth( const char *pszName, int iSize, void *pbuf );

	// Input
	bool SlotInput( int iSlot );
	virtual void paintBackground();
	CSchemeManager *GetSchemeManager( void ) { return &m_SchemeManager; }
	void *operator new( size_t stAllocateBlock );

public:
	// VGUI Menus
	CMenuPanel		*m_pCurrentMenu;
	ScorePanel		*m_pScoreBoard;
   CStatsMenu		*m_pStatsMenu;
   CTop32Menu		*m_pTop32Menu;
   CAdminMenu     *m_pAdminMenu;
   CVoteMapMenu   *m_pVoteMapMenu;
	char			m_szServerName[ MAX_SERVERNAME_LENGTH ];
};

//============================================================
// Command Menu Button Handlers
#define MAX_COMMAND_SIZE	256

class CMenuHandler_StringCommand : public ActionSignal
{
protected:
	char	m_pszCommand[MAX_COMMAND_SIZE];
	int		m_iCloseVGUIMenu;
public:
	CMenuHandler_StringCommand( char *pszCommand )
	{
		strncpy( m_pszCommand, pszCommand, MAX_COMMAND_SIZE);
		m_pszCommand[MAX_COMMAND_SIZE-1] = '\0';
		m_iCloseVGUIMenu = false;
	}

	CMenuHandler_StringCommand( char *pszCommand, int iClose )
	{
		strncpy( m_pszCommand, pszCommand, MAX_COMMAND_SIZE);
		m_pszCommand[MAX_COMMAND_SIZE-1] = '\0';
		m_iCloseVGUIMenu = iClose;
	}

	virtual void actionPerformed(Panel* panel)
	{
		gEngfuncs.pfnClientCmd(m_pszCommand);

		if (m_iCloseVGUIMenu)
			gViewPort->HideTopMenu();
		else
			gViewPort->HideCommandMenu();
	}
};

// This works the same as CMenuHandler_StringCommand, except it watches the string command 
// for specific commands, and modifies client vars based upon them.
class CMenuHandler_StringCommandWatch : public CMenuHandler_StringCommand
{
private:
public:
	CMenuHandler_StringCommandWatch( char *pszCommand ) : CMenuHandler_StringCommand( pszCommand )
	{
	}

	CMenuHandler_StringCommandWatch( char *pszCommand, int iClose ) : CMenuHandler_StringCommand( pszCommand, iClose )
	{
	}

	virtual void actionPerformed(Panel* panel)
	{
		CMenuHandler_StringCommand::actionPerformed( panel );

		// Try to guess the player's new team (it'll be corrected if it's wrong)
		if ( !strcmp( m_pszCommand, "jointeam 1" ) )
			g_iTeamNumber = 1;
		else if ( !strcmp( m_pszCommand, "jointeam 2" ) )
			g_iTeamNumber = 2;
		else if ( !strcmp( m_pszCommand, "jointeam 3" ) )
			g_iTeamNumber = 3;
		else if ( !strcmp( m_pszCommand, "jointeam 4" ) )
			g_iTeamNumber = 4;
	}
};

// Used instead of CMenuHandler_StringCommand for Class Selection buttons.
// Checks the state of hud_classautokill and kills the player if set
class CMenuHandler_StringCommandClassSelect : public CMenuHandler_StringCommand
{
private:
public:
	CMenuHandler_StringCommandClassSelect( char *pszCommand ) : CMenuHandler_StringCommand( pszCommand )
	{
	}

	CMenuHandler_StringCommandClassSelect( char *pszCommand, int iClose ) : CMenuHandler_StringCommand( pszCommand, iClose )
	{
	}

	virtual void actionPerformed(Panel* panel);
};

class CMenuHandler_PopupSubMenuInput : public InputSignal
{
private:
	CCommandMenu *m_pSubMenu;
	Button		 *m_pButton;
public:
	CMenuHandler_PopupSubMenuInput( Button *pButton, CCommandMenu *pSubMenu )
	{
		m_pSubMenu = pSubMenu;
		m_pButton = pButton;
	}

	virtual void cursorMoved(int x,int y,Panel* panel)
	{
		//gViewPort->SetCurrentCommandMenu( m_pSubMenu );
	}

	virtual void cursorEntered(Panel* panel) 
	{
		gViewPort->SetCurrentCommandMenu( m_pSubMenu );

		if (m_pButton)
			m_pButton->setArmed(true);
	};
	virtual void cursorExited(Panel* Panel) {};
	virtual void mousePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};
};

class CMenuHandler_LabelInput : public InputSignal
{
private:
	ActionSignal *m_pActionSignal;
public:
	CMenuHandler_LabelInput( ActionSignal *pSignal )
	{
		m_pActionSignal = pSignal;
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		m_pActionSignal->actionPerformed( panel );
	}

	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorEntered(Panel* panel) {};
	virtual void cursorExited(Panel* Panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};
};

#define HIDE_TEXTWINDOW		   0
#define SHOW_MAPBRIEFING	   1
#define SHOW_CLASSDESC		   2
#define SHOW_MOTD			      3

#define ACTION_TOGGLE_AUDIO   1
#define ACTION_TOGGLE_MUTE    2


class CMenuHandler_TextWindow : public ActionSignal
{
private:
	int	m_iState;
public:
	CMenuHandler_TextWindow( int iState )
	{
		m_iState = iState;
	}

	virtual void actionPerformed(Panel* panel)
	{
		if (m_iState == HIDE_TEXTWINDOW)
		{
			gViewPort->HideTopMenu();
		}
		else 
		{
			gViewPort->HideCommandMenu();
			gViewPort->ShowVGUIMenu( m_iState );
		}
	}
};

class CDragNDropHandler : public InputSignal
{
private:
	DragNDropPanel*	m_pPanel;
	bool			m_bDragging;
	int				m_iaDragOrgPos[2];
	int				m_iaDragStart[2];

public:
	CDragNDropHandler(DragNDropPanel* pPanel)
	{
		m_pPanel = pPanel;
		m_bDragging = false;
	}

	void cursorMoved(int x,int y,Panel* panel);
	void mousePressed(MouseCode code,Panel* panel);
	void mouseReleased(MouseCode code,Panel* panel);

	void mouseDoublePressed(MouseCode code,Panel* panel) {};
	void cursorEntered(Panel* panel) {};
	void cursorExited(Panel* panel) {};
	void mouseWheeled(int delta,Panel* panel) {};
	void keyPressed(KeyCode code,Panel* panel) {};
	void keyTyped(KeyCode code,Panel* panel) {};
	void keyReleased(KeyCode code,Panel* panel) {};
	void keyFocusTicked(Panel* panel) {};
};

class CHandler_MenuButtonOver : public InputSignal
{
private:
	int			m_iButton;
	CMenuPanel	*m_pMenuPanel;
public:
	CHandler_MenuButtonOver( CMenuPanel *pPanel, int iButton )
	{
		m_iButton = iButton;
		m_pMenuPanel = pPanel;
	}
		
	void cursorEntered(Panel *panel);

	void cursorMoved(int x,int y,Panel* panel) {};
	void mousePressed(MouseCode code,Panel* panel) {};
	void mouseReleased(MouseCode code,Panel* panel) {};
	void mouseDoublePressed(MouseCode code,Panel* panel) {};
	void cursorExited(Panel* panel) {};
	void mouseWheeled(int delta,Panel* panel) {};
	void keyPressed(KeyCode code,Panel* panel) {};
	void keyTyped(KeyCode code,Panel* panel) {};
	void keyReleased(KeyCode code,Panel* panel) {};
	void keyFocusTicked(Panel* panel) {};
};

class CHandler_ButtonHighlight : public InputSignal
{
private:
	Button *m_pButton;
public:
	CHandler_ButtonHighlight( Button *pButton )
	{
		m_pButton = pButton;
	}

	virtual void cursorEntered(Panel* panel) 
	{ 
		m_pButton->setArmed(true);
	};
	virtual void cursorExited(Panel* Panel) 
	{
		m_pButton->setArmed(false);
	};
	virtual void mousePressed(MouseCode code,Panel* panel) {};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};
};

//-----------------------------------------------------------------------------
// Purpose: Special handler for highlighting of command menu buttons
//-----------------------------------------------------------------------------
class CHandler_CommandButtonHighlight : public CHandler_ButtonHighlight
{
private:
	CommandButton *m_pCommandButton;
public:
	CHandler_CommandButtonHighlight( CommandButton *pButton ) : CHandler_ButtonHighlight( pButton )
	{
		m_pCommandButton = pButton;
	}

	virtual void cursorEntered( Panel *panel )
	{
		m_pCommandButton->cursorEntered();
	}

	virtual void cursorExited( Panel *panel )
	{
		m_pCommandButton->cursorExited();
	}
};


//================================================================
// Overidden Command Buttons for special visibilities
class ClassButton : public CommandButton
{
protected:
	int	m_iPlayerClass;

public:
	ClassButton( int iClass, const char* text,int x,int y,int wide,int tall, bool bNoHighlight ) : CommandButton( text,x,y,wide,tall, bNoHighlight)
	{
		m_iPlayerClass = iClass;
	}

	virtual int IsNotValid();
};

class SpectateButton : public CommandButton
{
public:
	SpectateButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight ) : CommandButton( text,x,y,wide,tall, bNoHighlight)
	{
	}

	virtual int IsNotValid()
	{
		// Only visible if the server allows it
		if ( gViewPort->GetAllowSpectators() != 0 )
			return false;

		return true;
	}
};

#define		DISGUISE_TEAM1		(1<<0)
#define		DISGUISE_TEAM2		(1<<1)
#define		DISGUISE_TEAM3		(1<<2)
#define		DISGUISE_TEAM4		(1<<3)

//============================================================
// Panel that can be dragged around
class DragNDropPanel : public Panel
{
private:
	bool		m_bBeingDragged;
	LineBorder	*m_pBorder;
public:
	DragNDropPanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
	{
		m_bBeingDragged = false;

		// Create the Drag Handler
		addInputSignal( new CDragNDropHandler(this) );

		// Create the border (for dragging)
		m_pBorder = new LineBorder();
	}

	virtual void setDragged( bool bState )
	{
		m_bBeingDragged = bState;

		if (m_bBeingDragged)
			setBorder(m_pBorder);
		else
			setBorder(NULL);
	}
};

//================================================================
// Panel that draws itself with a transparent black background
class CTransparentPanel : public Panel
{
private:
	int	m_iTransparency;
public:
   int   m_iOn;
	CTransparentPanel(int iTrans, int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
	{
		m_iTransparency = iTrans;
      m_iOn = false;
	}

	virtual void paintBackground()
	{
		if (m_iTransparency && m_iOn)
		{
			// Transparent black background
			drawSetColor( 0,0,0, m_iTransparency );
			drawFilledRect(0,0,_size[0],_size[1]);
		}
	}
};

//================================================================
// Menu Panel that supports buffering of menus
class CMenuPanel : public CTransparentPanel
{
private:
	CMenuPanel *m_pNextMenu;
	int			m_iMenuID;
	int			m_iRemoveMe;
	int			m_iIsActive;
	float		m_flOpenTime;
public:
	CMenuPanel(int iRemoveMe, int x,int y,int wide,int tall) : CTransparentPanel(100, x,y,wide,tall)
	{
		Reset();
		m_iRemoveMe = iRemoveMe;
	}

	CMenuPanel(int iTrans, int iRemoveMe, int x,int y,int wide,int tall) : CTransparentPanel(iTrans, x,y,wide,tall)
	{
		Reset();
		m_iRemoveMe = iRemoveMe;
	}

	virtual void Reset( void )
	{
		m_pNextMenu = NULL;
		m_iIsActive = false;
		m_flOpenTime = 0;
	}

	void SetNextMenu( CMenuPanel *pNextPanel )
	{
		if (m_pNextMenu)
			m_pNextMenu->SetNextMenu( pNextPanel );
		else
			m_pNextMenu = pNextPanel;
	}

	void SetMenuID( int iID )
	{
		m_iMenuID = iID;
	}

	void SetActive( int iState )
	{
		m_iIsActive = iState;
	}

	virtual void Open( void )
	{
		setVisible( true );

		// Note the open time, so we can delay input for a bit
		m_flOpenTime = gHUD.m_flTime;
	}

	virtual void Close( void )
	{
		setVisible( false );
		m_iIsActive = false;

		if ( m_iRemoveMe )
			gViewPort->removeChild( this );

		// This MenuPanel has now been deleted. Don't append code here.
	}

	int			ShouldBeRemoved() { return m_iRemoveMe; };
	CMenuPanel* GetNextMenu() { return m_pNextMenu; };
	int			GetMenuID() { return m_iMenuID; };
	int			IsActive() { return m_iIsActive; };
	float		GetOpenTime() { return m_flOpenTime; };

	// Numeric input
	virtual bool SlotInput( int iSlot ) { return false; };
	virtual void SetActiveInfo( int iInput ) {};
};

//================================================================
// Custom drawn scroll bars
class CTFScrollButton : public CommandButton
{
private:
	BitmapTGA	*m_pTGA;

public:
	CTFScrollButton(int iArrow, const char* text,int x,int y,int wide,int tall);

	virtual void paint( void );
	virtual void paintBackground( void );
};

// Custom drawn slider bar
class CTFSlider : public Slider
{
public:
	CTFSlider(int x,int y,int wide,int tall,bool vertical) : Slider(x,y,wide,tall,vertical)
	{
	};

	virtual void paintBackground( void );
};

// Custom drawn scrollpanel
class CTFScrollPanel : public ScrollPanel
{
public:
	CTFScrollPanel(int x,int y,int wide,int tall);
};

//================================================================
// Menu Panels that take key input
//============================================================
class CClassMenuPanel : public CMenuPanel
{
private:
	CTransparentPanel	*m_pClassInfoPanel[PC_LASTCLASS];
	Label				*m_pPlayers[PC_LASTCLASS];
	ClassButton			*m_pButtons[PC_LASTCLASS];
	CommandButton		*m_pCancelButton;
	ScrollPanel			*m_pScrollPanel;

	CImageLabel			*m_pClassImages[MAX_TEAMS][PC_LASTCLASS];

	int					m_iCurrentInfo;

	enum { STRLENMAX_PLAYERSONTEAM = 128 };
	char m_sPlayersOnTeamString[STRLENMAX_PLAYERSONTEAM];

public:
	CClassMenuPanel(int iTrans, int iRemoveMe, int x,int y,int wide,int tall);

	virtual bool SlotInput( int iSlot );
	virtual void Open( void );
	virtual void Update( void );
	virtual void SetActiveInfo( int iInput );
	virtual void Initialize( void );

	virtual void Reset( void )
	{
		CMenuPanel::Reset();
		m_iCurrentInfo = 0;
	}
};

class CTeamMenuPanel : public CMenuPanel
{
public:
	ScrollPanel         *m_pScrollPanel;
	CTransparentPanel	*m_pTeamWindow;
	Label				*m_pMapTitle;
	TextPanel			*m_pBriefing;
	TextPanel			*m_pTeamInfoPanel[6];
	CommandButton		*m_pButtons[6];
	bool				m_bUpdatedMapName;
	CommandButton		*m_pCancelButton;
	CommandButton		*m_pSpectateButton;

	int					m_iCurrentInfo;

public:
	CTeamMenuPanel(int iTrans, int iRemoveMe, int x,int y,int wide,int tall);

	virtual bool SlotInput( int iSlot );
	virtual void Open( void );
	virtual void Update( void );
	virtual void SetActiveInfo( int iInput );
	virtual void paintBackground( void );

	virtual void Initialize( void );

	virtual void Reset( void )
	{
		CMenuPanel::Reset();
		m_iCurrentInfo = 0;
	}
};

//=========================================================
// Specific Menus to handle old HUD sections
class CHealthPanel : public DragNDropPanel
{
private:
	BitmapTGA	*m_pHealthTGA;
	Label		*m_pHealthLabel;
public:
	CHealthPanel(int x,int y,int wide,int tall) : DragNDropPanel(x,y,wide,tall)
	{
		// Load the Health icon
		FileInputStream* fis = new FileInputStream( GetVGUITGAName("%d_hud_health"), false);
		m_pHealthTGA = new BitmapTGA(fis,true);
		fis->close();

		// Create the Health Label
		int iXSize,iYSize;
		m_pHealthTGA->getSize(iXSize,iYSize);
		m_pHealthLabel = new Label("",0,0,iXSize,iYSize);
		m_pHealthLabel->setImage(m_pHealthTGA);
		m_pHealthLabel->setParent(this);

		// Set panel dimension
		// Shouldn't be needed once Billy's fized setImage not recalculating the size
		//setSize( iXSize + 100, gHUD.m_iFontHeight + 10 );
		//m_pHealthLabel->setPos( 10, (getTall() - iYSize) / 2 );
	}

	virtual void paintBackground()
	{
	}

	void paint()
	{
		// Get the paint color
		int r,g,b,a;
		// Has health changed? Flash the health #
		if (gHUD.m_Health.m_fFade)
		{
			gHUD.m_Health.m_fFade -= (gHUD.m_flTimeDelta * 20);
			if (gHUD.m_Health.m_fFade <= 0)
			{
				a = MIN_ALPHA;
				gHUD.m_Health.m_fFade = 0;
			}

			// Fade the health number back to dim
			a = MIN_ALPHA +  (gHUD.m_Health.m_fFade/FADE_TIME) * 128;
		}
		else
			a = MIN_ALPHA;

		gHUD.m_Health.GetPainColor( r, g, b );
		ScaleColors(r, g, b, a );

		// If health is getting low, make it bright red
		if (gHUD.m_Health.m_iHealth <= 15)
			a = 255;

		int iXSize,iYSize, iXPos, iYPos;
		m_pHealthTGA->getSize(iXSize,iYSize);
		m_pHealthTGA->getPos(iXPos, iYPos);

		// Paint the player's health
		int x = gHUD.DrawHudNumber( iXPos + iXSize + 5, iYPos + 5, DHN_3DIGITS | DHN_DRAWZERO, gHUD.m_Health.m_iHealth, r, g, b);

		// Draw the vertical line
		int HealthWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;
		x += HealthWidth / 2;
		FillRGBA(x, iYPos + 5, HealthWidth / 10, gHUD.m_iFontHeight, 255, 160, 0, a);
	}
};

//================================================================
// Player stats menu!
//============================================================
class CStatsMenu : public CMenuPanel
{
private:
	CommandButton		*m_pCancelButton;
   CTransparentPanel	*m_pPanel;
   Label				   *m_pTitle;
   CommandButton		*m_pSpeak;
   char              *statsstring;

	int					m_iShowText;
	Label				   *m_pText;
   Label             *pLabel;
	CommandButton		*m_pShowButton;
	CommandButton		*m_pHideButton;
   TextPanel         *pText;
   CommandButton     *pButton,
                     *pButtonAdmin,
                     *pButtonNext,
                     *pButtonPrev,
                     *pButtonAudio,
                     *pButtonVoteMap,
                     *pButtonTop10,
                     *pButtonMute;


public:
	CStatsMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
   void SetActiveInfo( int iShowText );
   void setVisible( bool visible );
   void CompileStats();
private:
   CTransparentPanel *m_pBackgroundPanel;

};

class CTop32Menu: public CMenuPanel
{
private:
	CommandButton		*m_pCancelButton;
   CTransparentPanel	*m_pPanel;
   Label				   *m_pTitle;
   CommandButton		*m_pSpeak;
   char              *statsstring;

	int					m_iShowText;
	Label				   *m_pText;
   Label             *pLabel;
	CommandButton		*m_pShowButton;
	CommandButton		*m_pHideButton;
   TextPanel         *pText;
   CommandButton     *pButton;

public:
	CTop32Menu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
   void setVisible( bool visible );
private:
   CTransparentPanel *m_pBackgroundPanel;
};


class CAdminMenu : public CMenuPanel
{
private:
	CommandButton		*m_pCancelButton;
   CTransparentPanel	*m_pPanel;
   Label				   *m_pTitle;
   CommandButton		*m_pSpeak;
   char              *statsstring;

	int					m_iShowText;
   Label             *pLabel;
	CommandButton		*m_pShowButton;
	CommandButton		*m_pHideButton;

   CommandButton     *m_pMute;
   CommandButton     *m_pKick;
   CommandButton     *m_pSmack;
   CommandButton     *m_pFreeze;
   CommandButton     *m_pBan5;
   CommandButton     *m_pBan30;
   CommandButton     *m_pBan60;
   CommandButton     *m_pBan24;
   CommandButton     *m_pBan7;
   CommandButton     *m_pBan0;
   CommandButton     *m_pRenameBitch;
   CommandButton     *m_pRenameCheater;
   CommandButton     *m_pSmokeEast;
   CommandButton     *m_pSmokeWest;
   CommandButton     *m_pRestart;
   CommandButton     *m_pStrip;


	TextEntry			*m_pEditbox;
	CImageLabel			*m_pMyPicture;
   TextPanel         *pText;
   CommandButton     *pButton,
                     *pButtonNext,
                     *pButtonPrev;

public:
	CAdminMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
   void setVisible( bool visible );
private:
   CTransparentPanel *m_pBackgroundPanel;

};

class CVoteMapMenu : public CMenuPanel
{
private:
	CommandButton		*m_pCancelButton;
   CTransparentPanel	*m_pPanel;
   Label				   *m_pTitle;
   CommandButton		*m_pSpeak;
   char              *statsstring;

	int					m_iShowText;
   Label             *pLabel;
	CommandButton		*m_pShowButton;
	CommandButton		*m_pHideButton;
   TextPanel         *pText;
   CommandButton     *pButton,
                     *pButtonMap[10];

public:
	CVoteMapMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
   void setVisible( bool visible );
   void CompileMapVotes();
private:
   CTransparentPanel *m_pBackgroundPanel;
};

class CMenuHandler_FlipMode : public ActionSignal
{
   private:
	   int	m_iState;
   public:
	   CMenuHandler_FlipMode( int iState );
      virtual void actionPerformed(Panel* panel);
};

#endif
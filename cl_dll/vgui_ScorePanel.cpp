//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: VGUI scoreboard
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================


#include<VGUI_LineBorder.h>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ScorePanel.h"
#include "sdk_e_menu.h"

extern cvar_t *cl_teamscores;


hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];   // additional player info sent directly to the client dll
team_info_t			 g_TeamInfo[MAX_TEAMS+1];
int					 g_IsSpectator[MAX_PLAYERS+1];

int HUD_IsGame( const char *game );
int EV_TFC_IsAllyTeam( int iTeam1, int iTeam2 );

// Scoreboard dimensions
#define SBOARD_TITLE_SIZE_Y			YRES(24)
#define SBOARD_HEADER_SIZE_Y		YRES(40)
#define SBOARD_TABLE_X				XRES(16)
#define SBOARD_TABLE_Y				(SBOARD_TITLE_SIZE_Y + SBOARD_HEADER_SIZE_Y)

#define SBOARD_TEAM_CELL_SIZE_Y		YRES(11)
#define SBOARD_CELL_SIZE_Y			YRES(11)

// Column sizes
#define CSIZE_NAME				XRES(140)
#define CSIZE_CLASS				CSIZE_NAME + XRES(54)
#define CSIZE_KILLS				CSIZE_CLASS + XRES(50)
#define CSIZE_DEATHS			   CSIZE_KILLS + XRES(54)
#define CSIZE_JUICE			   CSIZE_DEATHS + XRES(54)

#define SMALL_CSIZE_NAME		XRES(124)					
#define SMALL_CSIZE_CLASS		SMALL_CSIZE_NAME + XRES(50)	
#define SMALL_CSIZE_KILLS		SMALL_CSIZE_CLASS + XRES(54)
#define SMALL_CSIZE_DEATHS		SMALL_CSIZE_KILLS + XRES(60)
#define SMALL_CSIZE_JUICE		SMALL_CSIZE_DEATHS + XRES(60)

#define TEAM_NO				0
#define TEAM_EAST			   1
#define TEAM_WEST			   2
#define TEAM_UNASSIGNED		3
#define TEAM_SPECTATORS		4

// Team Colors used in the scoreboard
int ScoreColorsBG[5][3] =
{
   /*
	{ 0, 0, 0 },
	{ 66, 114, 247 },
	{ 220, 51, 38 },
	{ 236, 212, 48 },
	{ 68, 199, 42 },
   */
   {FALSE_RED/2, FALSE_GREEN/2, FALSE_BLUE/2},
   {EAST_RED/2, EAST_GREEN/2, EAST_BLUE/2},
   {WEST_RED/2, WEST_GREEN/2, WEST_BLUE/2},
	{ 236, 212, 48 },
	{ 68, 199, 42 }
};

int ScoreColorsFG[5][3] =
{
   {FALSE_RED, FALSE_GREEN, FALSE_BLUE},
   {EAST_RED, EAST_GREEN, EAST_BLUE},
   {WEST_RED, WEST_GREEN, WEST_BLUE},
	{ 236, 212, 48 },
	{ 68, 199, 42 }
   /*
	{ 255, 255, 255 },
	{ 170, 193, 251 },
	{ 215, 151, 146 },
	{ 227, 203, 46 },
	{ 143, 215, 142 },
   */
};

//-----------------------------------------------------------------------------
// Purpose: Set different cell heights for Teams and Players
//-----------------------------------------------------------------------------
int ScoreTablePanel::getCellTall(int row)
{
	//if ( m_iIsATeam[row] )
		//return SBOARD_TEAM_CELL_SIZE_Y;

	return SBOARD_CELL_SIZE_Y;
}

//-----------------------------------------------------------------------------
// Purpose: Render each of the cells in the Table
//-----------------------------------------------------------------------------
Panel* ScoreTablePanel::getCellRenderer(int column,int row,bool columnSelected,bool rowSelected,bool cellSelected)
{
	char sz[128];
	hud_player_info_t *pl_info = NULL;
	team_info_t *team_info = NULL;
   int teamnumber;
	if (!m_iIsATeam[row])
	   {
		pl_info = &g_PlayerInfoList[ m_iSortedRows[row] ];
      teamnumber = g_PlayerExtraInfo[m_iSortedRows[row]].teamnumber;
      switch (teamnumber)
         {
         case 1:
            m_pLabel->setFgColor(EAST_RED, EAST_GREEN, EAST_BLUE, 0);
            break;
         case 2:
            m_pLabel->setFgColor(WEST_RED, WEST_GREEN, WEST_BLUE, 0);
            break;
         default:
            m_pLabel->setFgColor(FALSE_RED, FALSE_GREEN, FALSE_BLUE, 0);
            break;
         }
		if ( pl_info->thisplayer )
         m_pLabel->setBgColor(FALSE_RED, FALSE_GREEN, FALSE_BLUE, 200);
		else if ( m_iSortedRows[row] == m_iLastKilledBy && m_fLastKillTime && m_fLastKillTime > gHUD.m_flTime )
         m_pLabel->setBgColor(FALSE_RED, FALSE_GREEN, FALSE_BLUE, 220);
			//m_pLabel->setBgColor( 255,0,0, 255 - ((float)15 * (float)(m_fLastKillTime - gHUD.m_flTime)) );
		else
			m_pLabel->setBgColor( 0,0,0, 255 );
	   }
	else if ( m_iIsATeam[row] == TEAM_EAST )
	   {
      team_info = &g_TeamInfo[ TEAM_EAST ];
      m_pLabel->setBgColor(EAST_RED, EAST_GREEN, EAST_BLUE, 200);
      m_pLabel->setFgColor(TRUE_RED, TRUE_GREEN, TRUE_BLUE, 0);
	   }
	else if ( m_iIsATeam[row] == TEAM_WEST )
	   {
		team_info = &g_TeamInfo[ TEAM_WEST ];
      m_pLabel->setBgColor(WEST_RED, WEST_GREEN, WEST_BLUE, 200);
      m_pLabel->setFgColor(TRUE_RED, TRUE_GREEN, TRUE_BLUE, 0);
	   }
	else if ( m_iIsATeam[row] == 3 )
	   {
		team_info = &g_TeamInfo[ TEAM_WEST ];
      m_pLabel->setBgColor(FALSE_RED, FALSE_GREEN, FALSE_BLUE, 200);
      m_pLabel->setFgColor(TRUE_RED, TRUE_GREEN, TRUE_BLUE, 0);
	   }

	// Align 
	if (column < 1)
	   {
		if ( m_iIsATeam[row] )
			m_pLabel->setContentAlignment( vgui::Label::a_southwest );
		else
			m_pLabel->setContentAlignment( vgui::Label::a_west );
	   }
	else
	   {
		if ( m_iIsATeam[row] )
			m_pLabel->setContentAlignment( vgui::Label::a_south );
		else
			m_pLabel->setContentAlignment( vgui::Label::a_center );
	   }

	// Fill out with the correct data
	if ( m_iIsATeam[row] )
	   {
		char sz2[128];
		strcpy(sz, "");

		switch (column)
		   {
		   case 0:
            switch (m_iIsATeam[row])
               {
               case TEAM_EAST:
                  sprintf( sz2, CHudTextMessage::BufferedLocaliseTextString( "East Coast: " ) );
                  break;
               case TEAM_WEST:
                  sprintf( sz2, CHudTextMessage::BufferedLocaliseTextString( "West Coast: " ) );
                  break;
               case 3:
                  sprintf( sz2, CHudTextMessage::BufferedLocaliseTextString( "Spectators: " ) );
                  break;
               }				
			   // Append the number of players
			   if ( m_iIsATeam[row] < 3 )
			      {
				   if (team_info->players == 1)
					   sprintf(sz, "%s (1 gangster)", sz2 );
				   else
					   sprintf(sz, "%s (%d gangsters)", sz2, team_info->players );
			      }
            else
               {
               strcpy(sz, sz2);
               }
			   break;
		   case 1:
			   if ( m_iIsATeam[row] < 3 )
				   sprintf(sz, "%d",  team_info->wins );
            else
               sprintf(sz, " ");
			   break;
		   case 2:
			   if ( m_iIsATeam[row] < 3 )
				   sprintf(sz, "%d",  team_info->frags );
            else
               sprintf(sz, " ");
			   break;
		   case 3:
			   if ( m_iIsATeam[row] < 3 )
				   sprintf(sz, "%d",  team_info->deaths );
            else
               sprintf(sz, " ");
			   break;
		   case 4:
            sprintf(sz, " ");
			   break;
		   case 5:
			   if ( m_iIsATeam[row] < 3 )
				   sprintf(sz, "%d", team_info->ping );
            else
               sprintf(sz, " ");
			   break;
		   default:
			   break;
		   }
		m_pLabel->setText(sz);
	   }
	else
	   {
		switch (column)
		   {
		   case 0:
			   sprintf(sz, "  %s", pl_info->name);
			   break;
		   case 1:
            switch (g_PlayerExtraInfo[ m_iSortedRows[row] ].health)
               {
               case CLS_DEAD:
                  strcpy(sz, "----");
                  break;
               case CLS_NONE:
                  strcpy(sz, "");
                  break;
               case CLS_BAG:
                  strcpy(sz, "BOSS");
                  break;
               default:
                  sprintf(sz, "%d", g_PlayerExtraInfo[ m_iSortedRows[row] ].health);
                  break;
               }
			   break;
		   case 2:
			   sprintf(sz, "%d",  g_PlayerExtraInfo[ m_iSortedRows[row] ].frags );
			   break;
		   case 3:
			   sprintf(sz, "%d",  g_PlayerExtraInfo[ m_iSortedRows[row] ].deaths );
			   break;
		   case 4:
            if (g_PlayerExtraInfo[ m_iSortedRows[row] ].rank > 0)
			      sprintf(sz, "%d - %d", g_PlayerExtraInfo[ m_iSortedRows[row] ].juice, g_PlayerExtraInfo[ m_iSortedRows[row] ].rank);
            else
			      sprintf(sz, "%d", g_PlayerExtraInfo[ m_iSortedRows[row] ].juice );
   			   break;
		   case 5:
			   sprintf(sz, "%d", g_PlayerInfoList[ m_iSortedRows[row] ].ping );
			   break;
		   default:
			   strcpy(sz, "");
		   }
		m_pLabel->setText(sz);
	   }

	return m_pLabel;
}

//-----------------------------------------------------------------------------
// Purpose: Create the ScoreBoard panel
//-----------------------------------------------------------------------------
ScorePanel::ScorePanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
	setBorder(new LineBorder(Color(SCORE_FG_RED, SCORE_FG_GREEN, SCORE_FG_BLUE, 0)));
	setBgColor( SCORE_BG_RED, SCORE_BG_GREEN, SCORE_BG_BLUE, 0xB0 );

	m_pTitleLabel = new Label( "  SCORES", 0,0, wide, SBOARD_TITLE_SIZE_Y );
	m_pTitleLabel->setBgColor( SCORE_BG_RED, SCORE_BG_GREEN, SCORE_BG_BLUE, 0xFF);
   m_pTitleLabel->setFgColor( SCORE_FG_RED, SCORE_FG_GREEN, SCORE_FG_BLUE, 0);
	m_pTitleLabel->setContentAlignment( vgui::Label::a_west );
	m_pTitleLabel->setParent(this);

	_headerPanel = new HeaderPanel( SBOARD_TABLE_X, SBOARD_TITLE_SIZE_Y, wide - (SBOARD_TABLE_X * 2), SBOARD_HEADER_SIZE_Y);
	_headerPanel->setParent(this);

	// BUGBUG: This isn't working. gHUD.m_Teamplay hasn't been initialized yet.
	if ( gHUD.m_Teamplay )
		_headerPanel->addSectionPanel( new CLabelHeader( CHudTextMessage::BufferedLocaliseTextString( "#TEAMS" ), true) );
	else
		_headerPanel->addSectionPanel( new CLabelHeader( CHudTextMessage::BufferedLocaliseTextString( "#PLAYERS" ), true) );

   _headerPanel->addSectionPanel( new CLabelHeader("STATUS", true) );
	_headerPanel->addSectionPanel( new CLabelHeader( CHudTextMessage::BufferedLocaliseTextString( "KILLS" )) );
	_headerPanel->addSectionPanel( new CLabelHeader( CHudTextMessage::BufferedLocaliseTextString( "DEATHS")) );
	_headerPanel->addSectionPanel( new CLabelHeader( CHudTextMessage::BufferedLocaliseTextString( "JUICE")) );
	_headerPanel->addSectionPanel( new CLabelHeader( CHudTextMessage::BufferedLocaliseTextString( "PING")) );
	_headerPanel->setBgColor( SCORE_BG_RED, SCORE_BG_GREEN, SCORE_BG_BLUE, 0xFF);
   //_headerPanel->setFgColor( SCORE_FG_RED, SCORE_FG_GREEN, SCORE_FG_BLUE, 0);

	// Need to special case 400x300, because the titles just wont fit otherwise
	if ( ScreenWidth == 400 )
	   {
		_headerPanel->setSliderPos( 0, SMALL_CSIZE_NAME );
		_headerPanel->setSliderPos( 1, SMALL_CSIZE_CLASS );
		_headerPanel->setSliderPos( 2, SMALL_CSIZE_KILLS );
		_headerPanel->setSliderPos( 3, SMALL_CSIZE_DEATHS );
		_headerPanel->setSliderPos( 4, SMALL_CSIZE_JUICE );
		_headerPanel->setSliderPos( 5, wide - (SBOARD_TABLE_X * 2) - 1 );
	   }
	else
	   {
		_headerPanel->setSliderPos( 0, CSIZE_NAME );
		_headerPanel->setSliderPos( 1, CSIZE_CLASS );
		_headerPanel->setSliderPos( 2, CSIZE_KILLS );
		_headerPanel->setSliderPos( 3, CSIZE_DEATHS );
		_headerPanel->setSliderPos( 4, CSIZE_JUICE );
		_headerPanel->setSliderPos( 5, wide - (SBOARD_TABLE_X * 2) - 1 );
	   }

	_tablePanel = new ScoreTablePanel(SBOARD_TABLE_X, SBOARD_TABLE_Y, wide - (SBOARD_TABLE_X * 2), tall - SBOARD_TABLE_Y, NUM_COLUMNS);
	_tablePanel->setParent(this);
	_tablePanel->setHeaderPanel(_headerPanel);
	_tablePanel->setBgColor( 0,0,0, 255 );

	Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Called each time a new level is started.
//-----------------------------------------------------------------------------
void ScorePanel::Initialize( void )
{
	// Clear out scoreboard data
	_tablePanel->m_iLastKilledBy = 0;
	_tablePanel->m_fLastKillTime = 0;
	m_iPlayerNum = 0;
	m_iNumTeams = 0;
	memset( g_PlayerExtraInfo, 0, sizeof g_PlayerExtraInfo );
	memset( g_TeamInfo, 0, sizeof g_TeamInfo );
}

//-----------------------------------------------------------------------------
// Purpose: Set the size of the header and table when this panel's size changes
//-----------------------------------------------------------------------------
void ScorePanel::setSize(int wide,int tall)
{
	Panel::setSize(wide,tall);

	_headerPanel->setBounds(0,0,wide,SBOARD_HEADER_SIZE_Y);
	_tablePanel->setBounds(0,20,wide,tall - SBOARD_HEADER_SIZE_Y);
}

//-----------------------------------------------------------------------------
// Purpose: Recalculate the internal scoreboard data
//-----------------------------------------------------------------------------
void ScorePanel::Update()
{
	// Set the title
	if (gViewPort->m_szServerName)
	{
		char sz[MAX_SERVERNAME_LENGTH + 16];
		sprintf(sz, "  SCORES: %s", gViewPort->m_szServerName );
		m_pTitleLabel->setText(sz);
	}

	gViewPort->GetAllPlayersInfo();

	// Clear out sorts
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		_tablePanel->m_iSortedRows[i] = 0;
		_tablePanel->m_iIsATeam[i] = TEAM_NO;
		_tablePanel->m_bHasBeenSorted[i] = false;
	}

	// If it's not teamplay, sort all the players. Otherwise, sort the teams.
	//if ( !gHUD.m_Teamplay )
   if (cl_teamscores->value)
		SortPlayers( 0, NULL );
	else
		SortTeams();
}

//-----------------------------------------------------------------------------
// Purpose: Sort all the teams
//-----------------------------------------------------------------------------
void ScorePanel::SortTeams()
{
   int i, j, k;
   int team_row[3] = {1, 1, 1};
	_tablePanel->m_iRows = 0;

	for (i = 1; i < MAX_PLAYERS; i++ )
		{
      if (!g_PlayerInfoList[i].name)
         continue;
      k = g_PlayerExtraInfo[i].teamnumber;
      switch (k)
         {
         case 1:
            team_row[1]++;
            break;
         case 2:
            team_row[2]++;
            break;
         }
      }
   team_row[2] += team_row[1];
	_tablePanel->m_iIsATeam[ 0 ] = TEAM_EAST;
   //_tablePanel->m_iSortedRows[team_row[1]++] = -1;
	//_tablePanel->m_iSortedRows[team_row[2]++] = -1;
	_tablePanel->m_iIsATeam[ team_row[1]++ ] = TEAM_WEST;
	_tablePanel->m_iIsATeam[ team_row[2]++ ] = 3;
	// draw the players, in order,  and restricted to team if set
	for (i = 1; i <= 2; i++ )
	   {
      g_TeamInfo[i].frags = g_TeamInfo[i].deaths = 0;
		g_TeamInfo[i].ping = g_TeamInfo[i].packetloss = 0;
      g_TeamInfo[i].players = 0;
	   }

	while ( 1 )
	   {
		// Find the top ranking player
		int highest_frags = -99999;	int lowest_deaths = 99999;
		int best_player;
		best_player = 0;

		for (i = 1; i < MAX_PLAYERS; i++ )
		   {
			if ( _tablePanel->m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_frags )
			   {
				cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );
				extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
				if ( pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths )
					{
					best_player = i;
					lowest_deaths = pl_info->deaths;
					highest_frags = pl_info->frags;
					}
			   }
		   }

		if ( !best_player )
			break;

	   // Put this player in the sorted list
      k = g_PlayerExtraInfo[best_player].teamnumber;
      switch (k)
         {
         case 0:
            k = team_row[2]++;
            break;
         case 1:
            k = team_row[0]++;
            break;
         case 2:
            k = team_row[1]++;
            break;
         }
      _tablePanel->m_iSortedRows[k] = best_player;	   
	   _tablePanel->m_bHasBeenSorted[ best_player ] = true;
	   _tablePanel->m_iRows++;
	   }
   _tablePanel->m_iRows += 3;

	for (i = 1; i < MAX_PLAYERS; i++ )
	   {
		if ( g_PlayerInfoList[i].name == NULL )
			continue;

		// find what team this player is in
      j = g_PlayerExtraInfo[i].teamnumber;
      g_TeamInfo[j].frags += g_PlayerExtraInfo[i].frags;
		g_TeamInfo[j].deaths += g_PlayerExtraInfo[i].deaths;
		g_TeamInfo[j].ping += g_PlayerInfoList[i].ping;
		g_TeamInfo[j].packetloss += g_PlayerInfoList[i].packetloss;
      g_TeamInfo[j].players++;
	   }

	for ( i = 1; i <= 2; i++ )
	   {
		g_TeamInfo[i].already_drawn = FALSE;
		if ( g_TeamInfo[i].players > 0 )
		   {
			g_TeamInfo[i].ping /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
			g_TeamInfo[i].packetloss /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
		   }
	   }
}

//-----------------------------------------------------------------------------
// Purpose: Sort a list of players
//-----------------------------------------------------------------------------
void ScorePanel::SortPlayers( int iTeam, char *team )
{
   int i, j;

	_tablePanel->m_iRows = 2;

	// draw the players, in order,  and restricted to team if set
	for (i = 1; i <= 2; i++ )
	   {
      g_TeamInfo[i].frags = g_TeamInfo[i].deaths = 0;
		g_TeamInfo[i].ping = g_TeamInfo[i].packetloss = 0;
      g_TeamInfo[i].players = 0;
	   }

	while ( 1 )
	   {
		// Find the top ranking player
		int highest_frags = -99999;	int lowest_deaths = 99999;
		int best_player;
		best_player = 0;

		for (i = 1; i < MAX_PLAYERS; i++ )
		   {
			if ( _tablePanel->m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_frags )
			   {
				cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );
				extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
				if ( pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths )
					{
					best_player = i;
					lowest_deaths = pl_info->deaths;
					highest_frags = pl_info->frags;
					}
			   }
		   }

		if ( !best_player )
			break;

	   // Put this player in the sorted list
	   _tablePanel->m_iSortedRows[ _tablePanel->m_iRows ] = best_player;
	   _tablePanel->m_bHasBeenSorted[ best_player ] = true;
	   _tablePanel->m_iRows++;
	   }

	_tablePanel->m_iIsATeam[ 0 ] = TEAM_EAST;
	_tablePanel->m_iIsATeam[ 1 ] = TEAM_WEST;

	for (i = 1; i < MAX_PLAYERS; i++ )
	   {
		if ( g_PlayerInfoList[i].name == NULL )
			continue;

		// find what team this player is in
      j = g_PlayerExtraInfo[i].teamnumber;
      g_TeamInfo[j].frags += g_PlayerExtraInfo[i].frags;
		g_TeamInfo[j].deaths += g_PlayerExtraInfo[i].deaths;
		g_TeamInfo[j].ping += g_PlayerInfoList[i].ping;
		g_TeamInfo[j].packetloss += g_PlayerInfoList[i].packetloss;
      g_TeamInfo[j].players++;
	   }

	for ( i = 1; i <= 2; i++ )
	   {
		g_TeamInfo[i].already_drawn = FALSE;
		if ( g_TeamInfo[i].players > 0 )
		   {
			g_TeamInfo[i].ping /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
			g_TeamInfo[i].packetloss /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
		   }
	   }

}

//-----------------------------------------------------------------------------
// Purpose: Recalculate the existing teams in the match
//-----------------------------------------------------------------------------
void ScorePanel::RebuildTeams()
{
	// clear out player counts from teams
	for ( int i = 1; i <= m_iNumTeams; i++ )
		g_TeamInfo[i].players = 0;

	// rebuild the team list
	gViewPort->GetAllPlayersInfo();
	m_iNumTeams = 0;
	for ( i = 1; i < MAX_PLAYERS; i++ )
	   {
		if ( g_PlayerInfoList[i].name == NULL )
			continue;

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// is this player in an existing team?
		for ( int j = 1; j <= m_iNumTeams; j++ )
		   {
			if ( g_TeamInfo[j].name[0] == '\0' )
				break;

			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		   }

		if ( j > m_iNumTeams )
		   { // they aren't in a listed team, so make a new one
			// search through for an empty team slot
			for ( int j = 1; j <= m_iNumTeams; j++ )
			   {
				if ( g_TeamInfo[j].name[0] == '\0' )
					break;
			   }
			m_iNumTeams = max( j, m_iNumTeams );

			strncpy( g_TeamInfo[j].name, g_PlayerExtraInfo[i].teamname, MAX_TEAM_NAME );
			g_TeamInfo[j].players = 0;
		   }

		g_TeamInfo[j].players++;
	   }

	// clear out any empty teams
	for ( i = 1; i <= m_iNumTeams; i++ )
	   {
		if ( g_TeamInfo[i].players < 1 )
			memset( &g_TeamInfo[i], 0, sizeof(team_info_t) );
	   }

	// Update the scoreboard
	Update();
}

//-----------------------------------------------------------------------------
// Purpose: Setup highlights for player names in scoreboard
//-----------------------------------------------------------------------------
void ScorePanel::DeathMsg( int killer, int victim )
{
	// if we were the one killed,  or the world killed us, set the scoreboard to indicate suicide
	if ( victim == m_iPlayerNum || killer == 0 )
	{
		_tablePanel->m_iLastKilledBy = killer ? killer : m_iPlayerNum;
		_tablePanel->m_fLastKillTime = gHUD.m_flTime + 10;	// display who we were killed by for 10 seconds

		if ( killer == m_iPlayerNum )
			_tablePanel->m_iLastKilledBy = m_iPlayerNum;
	}
}


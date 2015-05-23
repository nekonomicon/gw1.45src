/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
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
#include "extdll.h"
#include "eiface.h"
#include "util.h"
#include "game.h"


cvar_t	displaysoundlist = {"displaysoundlist","0"};
cvar_t  mapcyclefile = {"mapcyclefile","mapcycle.txt"};
cvar_t  servercfgfile = {"servercfgfile","server.cfg"};
cvar_t  lservercfgfile = {"lservercfgfile","listenserver.cfg"};

// multiplayer server rules
cvar_t	fragsleft	= {"mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	timeleft	= {"mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "

// multiplayer server rules
cvar_t	teamplay	= {"mp_teamplay","0", FCVAR_SERVER };
cvar_t	fraglimit	= {"mp_fraglimit","0", FCVAR_SERVER };
cvar_t	timelimit	= {"mp_timelimit","0", FCVAR_SERVER };
cvar_t	friendlyfire= {"mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage	= {"mp_falldamage","0", FCVAR_SERVER };
cvar_t	forcerespawn= {"mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight	= {"mp_flashlight","0", FCVAR_SERVER };
cvar_t	aimcrosshair= {"mp_autocrosshair","1", FCVAR_SERVER };
cvar_t	decalfrequency = {"decalfrequency","30", FCVAR_SERVER };
cvar_t	teamlist = {"mp_teamlist","hgrunt;scientist", FCVAR_SERVER };
cvar_t	teamoverride = {"mp_teamoverride","1" };
cvar_t	defaultteam = {"mp_defaultteam","0" };
cvar_t	allowmonsters={"mp_allowmonsters","0", FCVAR_SERVER };
cvar_t	gw_pass0 = {"gw_pass0", "", FCVAR_PROTECTED };
cvar_t	gw_pass1 = {"gw_pass1", "", FCVAR_PROTECTED };
cvar_t	gw_pass2 = {"gw_pass2", "", FCVAR_PROTECTED };
cvar_t	gw_pass3 = {"gw_pass3", "", FCVAR_PROTECTED };
cvar_t	gw_ping_min = {"gw_ping_min", "0", FCVAR_SERVER };
cvar_t	gw_ping_rounds = {"gw_ping_rounds", "2", FCVAR_SERVER };
cvar_t	gw_ping_ban = {"gw_ping_ban", "5", FCVAR_SERVER };
cvar_t	gw_mapvote_ratio = {"gw_mapvote_ratio", ".51", FCVAR_SERVER };
cvar_t	gw_mapvote = {"gw_mapvote", "1", FCVAR_SERVER };

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	// Register cvars here:
	CVAR_REGISTER (&displaysoundlist);
	CVAR_REGISTER (&mapcyclefile);
	CVAR_REGISTER (&servercfgfile);
	CVAR_REGISTER (&lservercfgfile);

	CVAR_REGISTER (&teamplay);
	CVAR_REGISTER (&fraglimit);
	CVAR_REGISTER (&timelimit);

	CVAR_REGISTER (&fragsleft);
	CVAR_REGISTER (&timeleft);

	CVAR_REGISTER (&friendlyfire);
	CVAR_REGISTER (&falldamage);
	CVAR_REGISTER (&forcerespawn);
	CVAR_REGISTER (&flashlight);
	CVAR_REGISTER (&aimcrosshair);
	CVAR_REGISTER (&decalfrequency);
	CVAR_REGISTER (&teamlist);
	CVAR_REGISTER (&teamoverride);
	CVAR_REGISTER (&defaultteam);
	CVAR_REGISTER (&allowmonsters);

   CVAR_REGISTER (&gw_pass0);
   CVAR_REGISTER (&gw_pass1);
   CVAR_REGISTER (&gw_pass2);
   CVAR_REGISTER (&gw_pass3);
   CVAR_REGISTER (&gw_ping_min);
   CVAR_REGISTER (&gw_ping_rounds);
   CVAR_REGISTER (&gw_ping_ban);
   CVAR_REGISTER (&gw_mapvote_ratio);
   CVAR_REGISTER (&gw_mapvote);
// REGISTER CVARS FOR SKILL LEVEL STUFF
	//SERVER_COMMAND( "exec skill.cfg\n" );
	SERVER_COMMAND( "con_color \"255 255 255\"\n" );
}


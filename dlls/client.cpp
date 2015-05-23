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
// Robin, 4-22-98: Moved set_suicide_frame() here from player.cpp to allow us to 
//				   have one without a hardcoded player.mdl in tf_client.cpp

/*

===== client.cpp ========================================================

  client/server game specific stuff

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "player.h"
#include "spectator.h"
#include "client.h"
#include "soundent.h"
#include "gamerules.h"
#include "customentity.h"
#include "weapons.h"
#include "weaponinfo.h"
#include "usercmd.h"
#include "netadr.h"
#include "sdk_u_main.h"
#include "sdk_e_menu.h"
#include "sdk_c_global.h"
#include	"sdk_c_waitlist.h"
#include <time.h>

extern DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern DLL_GLOBAL int		g_iSkillLevel;
extern DLL_GLOBAL ULONG		g_ulFrameCount;

extern void CopyToBodyQue(entvars_t* pev);
extern int gmsgSayText;

static int g_serveractive = 0;

void DropNANA3(CBasePlayer * player);
void UpdateTeamMembers();
void LinkUserMessages( void );
void ListPlayers (CBasePlayer *current);
void ClientCommandString( edict_t *pEntity, char *pcmd);
void SendTitle(char *print_string, CBasePlayer *me);

void set_suicide_frame(entvars_t* pev)
{       
	if (!FStrEq(STRING(pev->model), "models/player.mdl"))
		return; // allready gibbed
	pev->solid		= SOLID_NOT;
	pev->movetype	= MOVETYPE_TOSS;
	pev->deadflag	= DEAD_DEAD;
	pev->nextthink	= -1;
}


/*
===========
ClientConnect
called when a player connects to a server
===========
*/
BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{	
   /*
   time_t rawtime;
   struct tm * timeinfo;

   time ( &rawtime );
   timeinfo = localtime ( &rawtime );
   if (map_stat.logfile)
      {
      fprintf(map_stat.logfile, "C\t%s has joined the server\n" 
                                "\tDate:%s" 
                                "\tMap:%s\n" 
                                "\tIP:%s\n"
                                "\tWONid:%i\n\n",
                                pszName, 
                                asctime(timeinfo),
                                (char *)STRING(gpGlobals->mapname),
                                pszAddress,
                                g_engfuncs.pfnGetPlayerWONId(pEntity));
      }
   */
	return g_pGameRules->ClientConnected( pEntity, pszName, pszAddress, szRejectReason );
}


/*
===========
ClientDisconnect
called when a player disconnects from a server
GLOBALS ASSUMED SET:  g_fGameOver
============
*/
void ClientDisconnect( edict_t *pEntity )
{
	if (g_fGameOver)
		return;

	char text[256];
	sprintf( text, "*** %s has left the game ***\n", STRING(pEntity->v.netname) );
	MESSAGE_BEGIN( MSG_ALL, gmsgSayText, NULL );
		WRITE_BYTE( ENTINDEX(pEntity) );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 1 );
		WRITE_STRING( text );
	MESSAGE_END();

	CSound *pSound;
	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt::ClientSoundIndex( pEntity ) );
	if ( pSound )
      pSound->Reset();

	// Allocate a CBasePlayer for pev, and call spawn

	pEntity->v.takedamage = DAMAGE_NO;// don't attract autoaim
	pEntity->v.solid = SOLID_NOT;// nonsolid
   pEntity->v.flags = FL_DORMANT;// nonsolid	
   UTIL_SetOrigin ( &pEntity->v, pEntity->v.origin );
	g_pGameRules->ClientDisconnected( pEntity );
}


// called by ClientKill and DeadThink
void respawn(entvars_t* pev, BOOL fCopyCorpse)
{
   // !!FIXME!!
   if (fCopyCorpse)
      CopyToBodyQue(pev);
}

/*
============
ClientKill

Player entered the suicide command

GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
============
*/
void ClientKill( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;

   if (pev->deadflag > DEAD_NO)
      return;
	CBasePlayer *pl = (CBasePlayer*) CBasePlayer::Instance( pev );

   if (!pl)
      {
      UTIL_LogPrintf("Crashed @ 001\n");
      return;
      }

	if ( pl->m_fNextSuicideTime > gpGlobals->time )
		return;  // prevent suiciding too often

	pl->m_fNextSuicideTime = gpGlobals->time + 1;  // don't let them suicide for 5 seconds after suiciding

	// have the player kill themself
	pev->health = 0;
	pl->Killed( pev, GIB_NEVER );   
}

/*
===========
ClientPutInServer

called each time a player is spawned
============
*/
extern int gmsgMoney;
void ClientPutInServer( edict_t *pEntity )
{
	CBasePlayer *pPlayer;

	entvars_t *pev = &pEntity->v;

	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pPlayer->SetCustomDecalFrames(-1); // Assume none;

	// Allocate a CBasePlayer for pev, and call spawn
   pPlayer->pev->deadflag = DEAD_NO;
   pPlayer->m_iPlayerStatus = 0;
   pPlayer->m_iPoints = 0;
   pPlayer->m_iJuice = 0;
   pPlayer->m_ilast_ID = 0;
   pPlayer->m_iTeam = 0;
   pPlayer->m_iMenu = 0;
   pPlayer->m_iGroup = 0;
   pPlayer->m_iMoney = aw.basemoney;
   pPlayer->m_iLicense = aw.baselicense;
   pPlayer->m_iKilled = 0;
   pPlayer->m_iVoteMap= -1;
   memset(&pPlayer->stat, 0, sizeof (pPlayer->stat));
   pPlayer->m_iClientFlag = CF_JUST_CONNECTED | CF_NEED_SERVER_VARIABLES;
   pPlayer->m_fShowTeamSelectTime = gpGlobals->time + 5;
   pPlayer->stat.TimeOn = gpGlobals->time;
   pPlayer->m_flNextMapVote = gpGlobals->time + 300;
   pPlayer->m_iPrimary = 0;
   pPlayer->m_iSecondary = 0;
   pPlayer->m_iTagged = 0;
   pPlayer->m_iAdmin = 0;
   pPlayer->m_iNANA = 0;
   pPlayer->m_iMaxSpeed = 0;
   pPlayer->m_iLastMaxSpeed = 0;
   pPlayer->m_iWONID = g_engfuncs.pfnGetPlayerWONId(pPlayer->edict());
   strcpy(pPlayer->m_iSteamID, g_engfuncs.pfnGetPlayerAuthId(pPlayer->edict()));
	pPlayer->Spawn();
   UpdateTeamMembers(); 
}

//// HOST_SAY
// String comes in as
// say blah blah blah
// or as
// blah blah blah
//
void Host_Say( edict_t *pEntity, int teamonly )
{
	CBasePlayer *client, *pPlayer;
	int		j;
	char	*p;
	char	text[256];
	char    szTemp[256];
	const char *cpSay = "say";
	const char *cpSayTeam = "say_team";
	const char *pcmd = CMD_ARGV(0);

	// We can get a raw string now, without the "say " prepended
	if ( CMD_ARGC() == 0 )
		return;


	if ( !stricmp( pcmd, cpSay) || !stricmp( pcmd, cpSayTeam ) )
	   {
		if ( CMD_ARGC() >= 2 )
			p = (char *)CMD_ARGS();
		else
			return;
	   }
	else  // Raw text, need to prepend argv[0]
	   {
		if ( CMD_ARGC() >= 2 )
			sprintf( szTemp, "%s %s", ( char * )pcmd, (char *)CMD_ARGS() );
		else
			// Just a one word command, use the first word...sigh
			sprintf( szTemp, "%s", ( char * )pcmd );
		p = szTemp;
	   }

   // remove quotes if present
	if (*p == '"')
	   {
		p++;
		p[strlen(p)-1] = 0;
	   }

   // make sure the text has content
	for ( char *pc = p; pc != NULL && *pc != 0; pc++ )
	   {
		if ( isprint( *pc ) && !isspace( *pc ) )
		   {
			pc = NULL;	// we've found an alphanumeric character,  so text is valid
			break;
		   }
	   }
	if ( pc != NULL )
		return;  // no character found, so say nothing

	client = NULL;
   pPlayer = (CBasePlayer *)CBaseEntity::Instance( pEntity );
   if (!pPlayer)
      {
      UTIL_LogPrintf("Crashed @ 002\n");
      return;
      }
   if (pPlayer->m_fNextLastSay < gpGlobals->time)
      {
      pPlayer->m_fNextLastSay = gpGlobals->time + 5;
      pPlayer->m_iLastSay = 0;
      }
   if (pPlayer->m_iLastSay++ > 4 && pPlayer->m_iAdmin <= 1)
      {
      char xxx[256];
      UTIL_LogPrintf( "%s has been banned for flooding\n",  STRING( pPlayer->pev->netname ));
      sprintf(xxx, "banid 30 %s kick\n", pPlayer->m_iSteamID);
      SERVER_COMMAND (xxx);
      CLIENT_COMMAND(pPlayer->edict(), "exit\n");
      return;
      }

   if ( pPlayer->m_iAdminTorture & ADMIN_TORTURE_GAG)
      return;

   // turn on color set 2  (color on,  no sound)
	if ( teamonly )
      {
      if (pPlayer->pev->deadflag == DEAD_NO)
		   sprintf( text, "(GANG) %s: ", STRING( pEntity->v.netname ) );
      else
         sprintf( text, "DEAD - (GANG) %s: ", STRING( pEntity->v.netname ) );
      }
	else
      {
      if (pPlayer->pev->deadflag == DEAD_NO)
		   sprintf( text, "%s: ", STRING( pEntity->v.netname ) );
      else
         sprintf( text, "DEAD - %s: ", STRING( pEntity->v.netname ) );
      }

	j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
	if ( (int)strlen(p) > j )
		p[j] = 0;

	strcat( text, p );
	strcat( text, "\n" );

	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop

   if (pPlayer->pev->deadflag > DEAD_NO)
      {
	   while ( ((client = (CBasePlayer*)UTIL_FindEntityByClassname( client, "player" )) != NULL) && (!FNullEnt(client->edict())) ) 
	      {
		   if ( !client->pev )
			   continue;
		   
		   if ( client->edict() == pEntity )
			   continue;

		   if ( !(client->IsNetClient()) )	// Not a client ? (should never be true)
			   continue;

		   if ( client->pev->deadflag <= DEAD_NO )
			   continue;

		   if (teamonly && client->m_iTeam != pPlayer->m_iTeam)
			   continue;

		   MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, client->pev );
			   WRITE_BYTE( ENTINDEX(pEntity) );
			   WRITE_BYTE( pPlayer->m_iTeam );
            WRITE_BYTE( 0 );
			   WRITE_STRING( text );
		   MESSAGE_END();
	      }
      }
   else
      {
	   while ( ((client = (CBasePlayer*)UTIL_FindEntityByClassname( client, "player" )) != NULL) && (!FNullEnt(client->edict())) ) 
	      {
		   if ( !client->pev )
			   continue;
		   
		   if ( client->edict() == pEntity )
			   continue;

		   if ( !(client->IsNetClient()) )	// Not a client ? (should never be true)
			   continue;

		   if ( teamonly && client->m_iTeam != pPlayer->m_iTeam)
			   continue;

		   MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, client->pev );
			   WRITE_BYTE( ENTINDEX(pEntity) );
			   WRITE_BYTE( pPlayer->m_iTeam );
            WRITE_BYTE( 1 );
			   WRITE_STRING( text );
		   MESSAGE_END();
	      }
      }

	// print to the sending client
	MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, &pEntity->v );
		WRITE_BYTE( ENTINDEX(pEntity) );
      WRITE_BYTE( pPlayer->m_iTeam );
      if (pPlayer->pev->health > 0 && pPlayer->pev->deadflag == DEAD_NO)
		   WRITE_BYTE( 1 );
      else
         WRITE_BYTE( 0 );
		WRITE_STRING( text );
	MESSAGE_END();

	// echo to server console
	g_engfuncs.pfnServerPrint( text );
   if (p[0] == '/')
      ClientCommandString( pEntity, p + 1);  

   UTIL_LogPrintf( text );
}


/*
===========
ClientCommand
called each time a player uses a "cmd" command
============
*/
extern float g_flWeaponCheat;

// Use CMD_ARGV,  CMD_ARGV, and CMD_ARGC to get pointers the character string command.
void TagPlayer (CBasePlayer *current, int idx, int tagnumber, int write_on);
void AccessPlayer (int idx, int value);

void SendTitle(char *, int);
bool GWClientCommand( edict_t *pEntity )
{
	const char *pcmd = CMD_ARGV(0);

	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return FALSE;

	entvars_t *pev = &pEntity->v;
	CBasePlayer* player = GetClassPtr((CBasePlayer *)pev);

   if (!player->m_iAdmin)
      return FALSE;

   char xxx[256];

	if ( FStrEq(pcmd, "gw_hurt" ) )
      {
      if (CMD_ARGV(1))
         {
         int idx = atoi( CMD_ARGV(1) );
         int tag = 1;
         if (CMD_ARGC() > 2) tag = atoi( CMD_ARGV(2) );
         TagPlayer (player, idx, TAG_HURT, tag);
         }
      return TRUE;
      }
	
   if ( FStrEq(pcmd, "gw_strip" ) )
      {
      if (CMD_ARGV(1))
         {
         int idx = atoi( CMD_ARGV(1) );
         int tag = 1;
         if (CMD_ARGC() > 2) tag = atoi( CMD_ARGV(2) );
         TagPlayer (player, idx, TAG_DONTSHOOT, tag);
         }
      return TRUE;
      }
	
   if ( FStrEq(pcmd, "gw_kill" ) )
      {
      if (CMD_ARGV(1))
         {
         int idx = atoi( CMD_ARGV(1) );
         TagPlayer (player, idx, TAG_KILL, 0);
         }
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_say" ) )
      {    
      if ( CMD_ARGC() >= 2 )
			SendTitle((char *)CMD_ARGS(), 11);
      return TRUE;
      }

   if ( FStrEq(pcmd, "gw_map" ) && player->m_iAdmin >= 2)
      {    
      if ( CMD_ARGC() >= 2 )
         {
         sprintf(xxx, "changelevel %s\n", (char *)CMD_ARGS());
         SERVER_COMMAND (xxx);
         }
      return TRUE;
      }

   if ( FStrEq(pcmd, "gw_beep" ) && player->m_iAdmin >= 2)
      {    
      if (rand() & 1)
         GW_EMIT_SOUND(pEntity, CHAN_STREAM, "max/bell.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 93 + RANDOM_LONG(0,0xF));
      else
         GW_EMIT_SOUND(pEntity, CHAN_STREAM, "max/doorbell.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 93 + RANDOM_LONG(0,0xF));
      if ( CMD_ARGC() >= 2 )
			SendTitle((char *)CMD_ARGS(), 9);
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_sound" )  && player->m_iAdmin >= 2)
      {    
      GW_EMIT_SOUND(pEntity, CHAN_STREAM, CMD_ARGS(), RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 93 + RANDOM_LONG(0,0xF));
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_cc" )  && player->m_iAdmin >= 2)
      {    
      if ( CMD_ARGC() < 2 ) return TRUE;
      int i = 1;
	   for ( i = 1; i <= gpGlobals->maxClients; i++ )
	      {
		   CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
         strcpy(xxx, CMD_ARGS());
         strcat(xxx, "\n");
		   if ( pPlayer )
            CLIENT_COMMAND (pPlayer->edict(), xxx);			   
	      }      
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_set" ) )
      {
      char *xtra;
      int param1;
      if (CMD_ARGC() > 1) 
         {
         xtra = (char *) CMD_ARGV(1);
         if (xtra)
            {
            if (!strcmp(xtra, "license"))
               {
               if (CMD_ARGC() > 2)
                  {
                  param1 = atoi(CMD_ARGV(2));
                  AccessPlayer (GW_SET_LICENSE, param1);
                  }
               }
            else if (!strcmp(xtra, "weapons"))
               AccessPlayer (GW_SET_WEAPONS, 0);
            else if (!strcmp(xtra, "damage"))
               {  
               if (CMD_ARGC() > 2)
                  {
                  param1 = atoi(CMD_ARGV(2));
                  AccessPlayer (GW_SET_DAMAGE, param1);
                  }
               }
            else if (!strcmp(xtra, "bunnyhop"))
               {
               aw.anti_cheats |= GW_AC_BUNNYHOP;
               SendTitle("Bunny hop handicap ON", 2);
               }
            }
         }  
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_reset" ) )
      {
      char *xtra;
      if (CMD_ARGC() > 1) 
         {
         xtra = (char *) CMD_ARGV(1);
         if (xtra)
            {
            if (!strcmp(xtra, "license"))
               AccessPlayer (GW_RESET_LICENSE, 0);
            else if (!strcmp(xtra, "weapons"))
               AccessPlayer (GW_RESET_WEAPONS, 0);
            else if (!strcmp(xtra, "scores"))
               AccessPlayer (GW_RESET_SCORES, 0);
            else if (!strcmp(xtra, "damage"))
               AccessPlayer (GW_RESET_DAMAGE, 0);
            else if (!strcmp(xtra, "bunnyhop"))
               {
               aw.anti_cheats &= ~GW_AC_BUNNYHOP;
               SendTitle("Bunny hop handicap OFF", 1);
               }
            //else if (!strcmp(xtra, "map"))
               //AccessPlayer (6, 0);
            }
         }  
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_vguikick" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target && player->m_iAdmin < 1)
         {
         sprintf(xxx, "kick \"%s\"\n", STRING(target->pev->netname));
         SERVER_COMMAND (xxx);
         }
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_vguifreeze" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target)
         {
         target->m_iLastMaxSpeed = -1;
         target->m_iAdminTorture ^= ADMIN_TORTURE_FREEZE;
         if (target->m_iAdminTorture & ADMIN_TORTURE_FREEZE)
            {
            sprintf(xxx, "%s has been frozen\n", STRING(target->pev->netname));
            //EMIT_SOUND_DYN(ENT(target->pev), CHAN_STREAM, "ambience/barmusic.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, SND_SPAWNING, 93 + RANDOM_LONG(0,0xF));
            SendTitle("^_ ^o Yo ass is frozen foo!", target);
            ClientPrint( player->pev, HUD_PRINTCONSOLE, xxx );
            }
         else
            {
            sprintf(xxx, "%s has been unfrozen\n", STRING(target->pev->netname));
            //EMIT_SOUND_DYN(ENT(target->pev), CHAN_STREAM, "ambience/barmusic.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, SND_STOP, 93 + RANDOM_LONG(0,0xF));           
            SendTitle("^_ ^o Move it sucka!", target);
            ClientPrint( player->pev, HUD_PRINTCONSOLE, xxx );
            }
         }
      return TRUE;
      }

   if ( FStrEq(pcmd, "gw_vguigag" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target)
         {

         if (!(target->m_iAdminTorture & ADMIN_TORTURE_GAG))
            {
            sprintf (xxx, "*** %s has been gagged! ***\n", STRING(target->pev->netname));
            UTIL_SayTextAll(xxx, target);
            }
         target->m_iAdminTorture ^= ADMIN_TORTURE_GAG;         
         }
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_vguismack" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target)
         target->Slap(10);
      return TRUE;
      }
   
   if ( FStrEq(pcmd, "gw_vguistrip" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target)
         CLIENT_COMMAND (target->edict(), "drop\n");
      return TRUE;
      }
     if ( FStrEq(pcmd, "gw_vguibitch" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target)
         CLIENT_COMMAND (target->edict(), "name \"I am a lame bitch\"\n");
      return TRUE;
      }

   if ( FStrEq(pcmd, "gw_vguicheater" ) )
      {
      CBasePlayer *target;
      target = GetClientWithSteamID(player->m_iTargetSteamID);
      if (target)
         CLIENT_COMMAND (target->edict(), "name \"I am a cheater!\"\n");
      return TRUE;
      }
   

   if (player->m_iAdmin > 2)
      {
      if ( FStrEq(pcmd, "gw_vguiban5" ) )
         {
         CBasePlayer *target;
         target = GetClientWithSteamID(player->m_iTargetSteamID);
         if (target && target->m_iAdmin < 2)
            {
            sprintf(xxx, "banid 5 %s kick\n", target->m_iSteamID);
            SERVER_COMMAND (xxx);
            }
         return TRUE;
         }
      if ( FStrEq(pcmd, "gw_vguiban30" ) )
         {
         CBasePlayer *target;
         target = GetClientWithSteamID(player->m_iTargetSteamID);
         if (target && target->m_iAdmin < 2)
            {
            sprintf(xxx, "banid 30 %s kick\n", target->m_iSteamID);
            SERVER_COMMAND (xxx);
            }
         return TRUE;
         }     
      if (player->m_iAdmin > 3)
         {
         if ( FStrEq(pcmd, "gw_vguiban60" ) )
            {
            CBasePlayer *target;
            target = GetClientWithSteamID(player->m_iTargetSteamID);
            if (target && target->m_iAdmin < 2)
               {
               sprintf(xxx, "banid 60 %s kick\n", target->m_iSteamID);
               SERVER_COMMAND (xxx);
               }
            return TRUE;
            }
         if ( FStrEq(pcmd, "gw_vguiban24" ) )
            {
            CBasePlayer *target;
            target = GetClientWithSteamID(player->m_iTargetSteamID);
            if (target && target->m_iAdmin < 2)
               {
               sprintf(xxx, "banid 1440 %s kick\n", target->m_iSteamID);
               SERVER_COMMAND (xxx);
               }
            return TRUE;
            }
         if ( FStrEq(pcmd, "gw_vguiban7" ) )
            {
            CBasePlayer *target;
            target = GetClientWithSteamID(player->m_iTargetSteamID);
            if (target && target->m_iAdmin < 2)
               {
               sprintf(xxx, "banid 10080 %s kick\n", target->m_iSteamID);
               SERVER_COMMAND (xxx);
               }
            return TRUE;
            }
         if ( FStrEq(pcmd, "gw_vguiban0" ) )
            {
            CBasePlayer *target;
            target = GetClientWithSteamID(player->m_iTargetSteamID);
            if (target && target->m_iAdmin < 2)
               {
               sprintf(xxx, "banid 0 %s kick\n", target->m_iSteamID);
               SERVER_COMMAND (xxx);
               }
            return TRUE;
            }
         if ( FStrEq(pcmd, "gw_vguismokeeast" ) )
            {
            AccessPlayer (GW_SMOKE_EAST, 0);
            return TRUE;
            }

         if ( FStrEq(pcmd, "gw_vguismokewest" ) )
            {
            AccessPlayer (GW_SMOKE_WEST, 0);
            return TRUE;
            }
   
         if ( FStrEq(pcmd, "gw_vguirestart" ) )
            {
            AccessPlayer (GW_RESET_SCORES, 0);
            return TRUE;
            }
         }
      }
   

   return FALSE;
}

void DropPlayerNANA(CBasePlayer *);
extern int gsmgAdminMenu;
void ClientCommandString( edict_t *pEntity, char *pcmd)
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer* player = GetClassPtr((CBasePlayer *)pev);

	if ( FStrEq(pcmd, "spy" ) )
	   {
      GetClassPtr((CBasePlayer *)pev)->SendMyStats(-1);
	   }
	else if ( FStrEq(pcmd, "stats" ) )
	   {
      GetClassPtr((CBasePlayer *)pev)->SendMyStats(0);
	   }
	else if ( FStrEq(pcmd, "top10" ) )
	   {
      GetClassPtr((CBasePlayer *)pev)->SendTop32(  );
	   }
	else if ( FStrEq(pcmd, "vote" ) )
	   {
      SendVoteMaps(player, CompileMapVotes());
      }
	else if ( FStrEq(pcmd, "list" ) )
	   {
      wl[player->m_iTeam].DisplayList(player);
      }
   else if (!strcmp(pcmd, "users"))
      {
      ListPlayers(player);
      }
   else if (!strcmp(pcmd, "teams"))
      {
      DisplayTeamMenu(player);
      }
   else if (!strcmp(pcmd, "timeleft"))
      {
      char output[1204];
      char output1[256];
      char output2[256] = {0};
      int minutes;
      int seconds;
	   float time_remaining, flTimeLimit;
      flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;	   
      time_remaining = map_stat.g_fRoundEndTime - gpGlobals->time;
      seconds = time_remaining;
      seconds = seconds % 60;
      minutes = time_remaining - seconds;
      minutes /= 60;
      sprintf (output, "Timeleft on round %i is %i:%02i\n", 
               map_stat.g_iRounds, minutes, seconds);
      ClientPrint(player->pev, HUD_PRINTCONSOLE, output);
	   time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);
      if (time_remaining)
         {         
         seconds = time_remaining;
         seconds = seconds % 60;
         minutes = time_remaining - seconds;
         minutes /= 60;
         sprintf (output1, "Timeleft on '%s' is %i:%02i\n", 
                 (char *)STRING(gpGlobals->mapname), 
                 minutes, seconds);
         strcat(output, output1);
         if (aw.nextmap[0])
            {
            sprintf (output2, "Next map is %s\n", aw.nextmap);
            strcat(output, output2);
            }
         strcat(output, "^_ ^n");
         SendTitle(output, player);
         }
      else
         SendTitle("^_ ^n *** This map has no time limit ***", player);
      }

}

void ClientCommand( edict_t *pEntity )
{
	const char *pcmd = CMD_ARGV(0);
	const char *pstr;

	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

	entvars_t *pev = &pEntity->v;
	CBasePlayer* player = GetClassPtr((CBasePlayer *)pev);

	if ( FStrEq(pcmd, "say" ) )
		Host_Say( pEntity, 0 );
	else if ( FStrEq(pcmd, "say_team" ) )
		Host_Say( pEntity, 1 );
	else if ( FStrEq(pcmd, "drop" ) )
      DropPrimaryWeapon(player);
   /*
	else if ( FStrEq(pcmd, "dropNANA" ) )
      DropPlayerNANA(player);
   */
	else if ( FStrEq(pcmd, "use" ) )
		GetClassPtr((CBasePlayer *)pev)->SelectItem((char *)CMD_ARGV(1));
	else if (((pstr = strstr(pcmd, "weapon_")) != NULL)  && (pstr == pcmd))
		GetClassPtr((CBasePlayer *)pev)->SelectItem(pcmd);
	else if (FStrEq(pcmd, "lastinv" ))
		GetClassPtr((CBasePlayer *)pev)->SelectLastItem();
	else if ( g_pGameRules->ClientCommand( GetClassPtr((CBasePlayer *)pev), pcmd ) ) 
      {}
   /*
	else if ( FStrEq(pcmd, "vguimenu" ) )
	   {
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->ShowVGUIMenu(atoi(CMD_ARGV(1)));
	   }
   */
	else if ( FStrEq(pcmd, "lockedstats" ) )
	   {
      GetClassPtr((CBasePlayer *)pev)->SendMyStats(-1);
	   }
	else if ( FStrEq(pcmd, "showstats" ) )
	   {
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->SendMyStats(atoi(CMD_ARGV(1)));
	   }
	else if ( FStrEq(pcmd, "topscores" ) )
	   {
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->SendTop32(  );
	   }
	else if ( FStrEq(pcmd, "playbeat" ) )
	   {
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->PlayBeat(atoi(CMD_ARGV(1)));
	   }
	else if ( FStrEq(pcmd, "loopmp3" ) )
	   {
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->PlayMp3((char *)CMD_ARGV(1), 1);
	   }
	else if ( FStrEq(pcmd, "playmp3" ) )
	   {
		if (CMD_ARGC() >= 1)
			GetClassPtr((CBasePlayer *)pev)->PlayMp3((char *)CMD_ARGV(1), 0);
	   }
	else if ( FStrEq(pcmd, "votemap" ) )
	   {
		if (CMD_ARGC() >= 1)
         {
         if (player->m_flNextMapVote < gpGlobals->time)
            {
            player->m_iVoteMap = atoi(CMD_ARGV(1)) - 1;
            if (player->m_iVoteMap >=0 && player->m_iVoteMap < map_vote_count)
               player->m_flNextMapVote = gpGlobals->time + 60;
            CompileMapVotes();
            }
         else
            {
            char xxx[128];
            sprintf(xxx, "^_ ^n Next allowed vote is in %i seconds", 
                  (int) (player->m_flNextMapVote - gpGlobals->time));
            SendTitle(xxx, player);
            }
         }
	   }
	else if ( FStrEq(pcmd, "votemaps" ) )
	   {
      SendVoteMaps(player, CompileMapVotes());
      }
	else if ( FStrEq(pcmd, "waitlist" ) )
	   {
      wl[player->m_iTeam].DisplayList(player);
      }
	else if ( FStrEq(pcmd, "adminmenu" ) )
	   {
      if (player->m_iAdmin)
         {        
         CBasePlayer* target;
         target = GetClientWithSteamID(player->m_iTargetSteamID);
         if (target)
            {
            MESSAGE_BEGIN(MSG_ONE, gsmgAdminMenu, NULL, player->pev);
               WRITE_SHORT(target->m_iAdminTorture);
            MESSAGE_END();
            }        
         else
            {
            MESSAGE_BEGIN(MSG_ONE, gsmgAdminMenu, NULL, player->pev);
               WRITE_SHORT(player->m_iAdminTorture);
            MESSAGE_END();
            }
         }
      else
         {
         SendTitle("^o You need to log in as\nadministrator first!", player);
         }
	   }
   else if (GWClientCommand(pEntity))
      {}
	else
		ClientPrint( &pEntity->v, HUD_PRINTCONSOLE, "Unknown command.\n\n" );
}


/*
========================
ClientUserInfoChanged

called after the player changes
userinfo - gives dll a chance to modify it before
it gets sent into the rest of the engine.
========================
*/
void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

   CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);
   switch (pPlayer->m_iTeam)
      {
      case TEAM_EAST:
         switch (pPlayer->m_iGroup)
            {
            case 1:
            default:
               CLIENT_COMMAND (pEntity, "model east1\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "east1" );
               break;
            case 2:
               CLIENT_COMMAND (pEntity, "model east2\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "east2" );
               break;
            case 3:
               CLIENT_COMMAND (pEntity, "model east3\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "east3" );
               break;
            case 4:
               CLIENT_COMMAND (pEntity, "model east4\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "east4" );
               break;
            }
         break;
      case TEAM_WEST:
         switch (pPlayer->m_iGroup)
            {
            case 1:
            default:
               CLIENT_COMMAND (pEntity, "model west1\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "west1" );
               break;
            case 2:
               CLIENT_COMMAND (pEntity, "model west2\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "west2" );
               break;
            case 3:
               CLIENT_COMMAND (pEntity, "model west3\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "west3" );
               break;
            case 4:
               CLIENT_COMMAND (pEntity, "model west4\n");
               g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity),g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model", "west4" );
               break;
            }
      break;
      }

	// msg everyone if someone changes their name,  and it isn't the first time (changing no name to current name)
	if ( pEntity->v.netname && STRING(pEntity->v.netname)[0] && !FStrEq( STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" )) )
	   {
      if (pPlayer->pev->deadflag > DEAD_NO)
         {
         g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity), infobuffer, "name", (char *)STRING(pEntity->v.netname));
         SendTitle("^_ ^n You cannot change your name while dead\nWait until you respawn!", pPlayer);
         CLIENT_COMMAND(pPlayer->edict(), "name \"dead gangster\"\n");
         return;
         }
		char text[256];
		sprintf( text, "*** %s CHANGED NAME TO %s ***\n", STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );
		MESSAGE_BEGIN( MSG_ALL, gmsgSayText, NULL );
			WRITE_BYTE( ENTINDEX(pEntity) );
			WRITE_BYTE( 0 );
			WRITE_BYTE( 1 );
			WRITE_STRING( text );
		MESSAGE_END();

		UTIL_LogPrintf( "\"%s<%i>\" changed name to \"%s<%i>\"\n", STRING( pEntity->v.netname ), GETPLAYERUSERID( pEntity ), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ), GETPLAYERUSERID( pEntity ) );
	   }

	g_pGameRules->ClientUserInfoChanged( GetClassPtr((CBasePlayer *)&pEntity->v), infobuffer );
}


void ServerDeactivate( void )
{
	// It's possible that the engine will call this function more times than is necessary
	//  Therefore, only run it one time for each call to ServerActivate 
	if (g_serveractive != 1) return;

	g_serveractive = 0;
}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	int				i;
	CBaseEntity		*pClass;

	// Every call to ServerActivate should be matched by a call to ServerDeactivate
	g_serveractive = 1;

	// Clients have not been initialized yet
	for ( i = 0; i < edictCount; i++ )
	   {
		if ( pEdictList[i].free )
			continue;
		
		// Clients aren't necessarily initialized until ClientPutInServer()
		if ( i < clientMax || !pEdictList[i].pvPrivateData )
			continue;

		pClass = CBaseEntity::Instance( &pEdictList[i] );
		// Activate this entity if it's got a class & isn't dormant
		if ( pClass && !(pClass->pev->flags & FL_DORMANT) )
			pClass->Activate();
		else
			ALERT( at_console, "Can't instance %s\n", STRING(pEdictList[i].v.classname) );
	   }

	// Link user messages here to make sure first client can get them...
	LinkUserMessages();
}


/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
extern int gmsgRadar;
void PlayerPreThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);
	if (pPlayer && pPlayer->pev)
      {

      if (pPlayer->m_iMaxSpeed != pPlayer->m_iLastMaxSpeed)
         {         
         float ff;
         if (pPlayer->m_iMaxSpeed > 300) pPlayer->m_iMaxSpeed = 300;
         if (!(pPlayer->m_iAdminTorture & (ADMIN_TORTURE_FREEZE | ADMIN_TORTURE_ROUNDSTART)))
            {
            if (aw.speed > 0)
               {
               ff = pPlayer->m_iMaxSpeed;
               ff *= aw.speed;
               g_engfuncs.pfnSetClientMaxspeed( ENT( pPlayer->pev ), (int) ff);
               }
            else
               g_engfuncs.pfnSetClientMaxspeed( ENT( pPlayer->pev ), 300);
            }
         else
            g_engfuncs.pfnSetClientMaxspeed( ENT( pPlayer->pev ), 1);         
         pPlayer->m_iLastMaxSpeed = pPlayer->m_iMaxSpeed;
         }
		pPlayer->PreThink( );
      if (pPlayer->pev->deadflag == DEAD_NO)
         {
         if (pPlayer->m_fNextCheat < gpGlobals->time)
            {
	         if (UTIL_PointContents(pPlayer->pev->origin) == CONTENTS_SKY)
               {
               SendTitle("^r You are in the sky!\nYou shall die!\n", pPlayer);              
               CLIENT_COMMAND(pPlayer->edict(), "kill\n");
               }
            pPlayer->m_fNextCheat = gpGlobals->time + 2.0f;
            }
         if (pPlayer->m_iNANA && pPlayer->m_flNextNANABlip < gpGlobals->time)
            {
            CBaseEntity *dp = NULL;
            Vector v_other, v_radar;
            float distance;
            int index, sent = 0;
            for (index = 1; index <= gpGlobals->maxClients; index++)
               {
               dp = UTIL_PlayerByIndex( index );
               if (!dp) continue;
               if (!dp->pev) continue;
               if (dp->pev->deadflag > DEAD_NO) continue;
               v_other = pPlayer->pev->origin - dp->pev->origin;
               distance = v_other.Length();
               if (distance < 2000)
                  {
                  sent = 1;
                  MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, dp->pev);
                  if (((CBasePlayer *)dp)->m_iTeam == pPlayer->m_iTeam)
                     WRITE_BYTE(1); // Send Green
                  else 
                     WRITE_BYTE(2); // Send Red
                  WRITE_COORD(pPlayer->pev->origin.x);
                  WRITE_COORD(pPlayer->pev->origin.y);
                  WRITE_COORD(pPlayer->pev->origin.z);
                  MESSAGE_END();
                  if (distance < 300 && ((CBasePlayer *)dp)->m_iTeam == pPlayer->m_iTeam)
                     {
                     dp->pev->health += 6;
                     if (dp->pev->health > 100)
                        dp->pev->health = 100;
                     }
                  }
               }
            if (!sent) pPlayer->m_flNextNANABlip = gpGlobals->time + 2;
            else pPlayer->m_flNextNANABlip = gpGlobals->time + 2;           
            }
         }
      }
}

/*
================
PlayerPostThink

Called every frame after physics are run
================
*/
void PlayerPostThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PostThink( );
}



void ParmsNewLevel( void )
{
}


void ParmsChangeLevel( void )
{
	// retrieve the pointer to the save data
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	if ( pSaveData )
		pSaveData->connectionCount = BuildChangeList( pSaveData->levelList, MAX_LEVEL_CONNECTIONS );
}


//
// GLOBALS ASSUMED SET:  g_ulFrameCount
//
void StartFrame( void )
{
	if ( g_pGameRules )
		g_pGameRules->Think();

	if ( g_fGameOver )
		return;

	gpGlobals->teamplay = CVAR_GET_FLOAT("teamplay");
	g_iSkillLevel = CVAR_GET_FLOAT("skill");
	g_ulFrameCount++;
}


void ClientPrecache( void )
{
	// setup precaches always needed
	PRECACHE_SOUND("player/sprayer.wav");			// spray paint sound for PreAlpha
	
	// PRECACHE_SOUND("player/pl_jumpland2.wav");		// UNDONE: play 2x step sound
	
	PRECACHE_SOUND("player/pl_fallpain1.wav");		
	PRECACHE_SOUND("player/pl_fallpain2.wav");	
	PRECACHE_SOUND("player/pl_fallpain3.wav");
	
	PRECACHE_SOUND("player/pl_step1.wav");		// walk on concrete
	PRECACHE_SOUND("player/pl_step2.wav");
	PRECACHE_SOUND("player/pl_step3.wav");
	PRECACHE_SOUND("player/pl_step4.wav");

	PRECACHE_SOUND("common/npc_step1.wav");		// NPC walk on concrete
	PRECACHE_SOUND("common/npc_step2.wav");
	PRECACHE_SOUND("common/npc_step3.wav");
	PRECACHE_SOUND("common/npc_step4.wav");

	PRECACHE_SOUND("player/pl_metal1.wav");		// walk on metal
	PRECACHE_SOUND("player/pl_metal2.wav");
	PRECACHE_SOUND("player/pl_metal3.wav");
	PRECACHE_SOUND("player/pl_metal4.wav");

	PRECACHE_SOUND("player/pl_dirt1.wav");		// walk on dirt
	PRECACHE_SOUND("player/pl_dirt2.wav");
	PRECACHE_SOUND("player/pl_dirt3.wav");
	PRECACHE_SOUND("player/pl_dirt4.wav");

	PRECACHE_SOUND("player/pl_duct1.wav");		// walk in duct
	PRECACHE_SOUND("player/pl_duct2.wav");
	PRECACHE_SOUND("player/pl_duct3.wav");
	PRECACHE_SOUND("player/pl_duct4.wav");

	PRECACHE_SOUND("player/pl_grate1.wav");		// walk on grate
	PRECACHE_SOUND("player/pl_grate2.wav");
	PRECACHE_SOUND("player/pl_grate3.wav");
	PRECACHE_SOUND("player/pl_grate4.wav");

	PRECACHE_SOUND("player/pl_slosh1.wav");		// walk in shallow water
	PRECACHE_SOUND("player/pl_slosh2.wav");
	PRECACHE_SOUND("player/pl_slosh3.wav");
	PRECACHE_SOUND("player/pl_slosh4.wav");

	PRECACHE_SOUND("player/pl_tile1.wav");		// walk on tile
	PRECACHE_SOUND("player/pl_tile2.wav");
	PRECACHE_SOUND("player/pl_tile3.wav");
	PRECACHE_SOUND("player/pl_tile4.wav");

	PRECACHE_SOUND("player/pl_swim1.wav");		// breathe bubbles
	PRECACHE_SOUND("player/pl_swim2.wav");
	PRECACHE_SOUND("player/pl_swim3.wav");
	PRECACHE_SOUND("player/pl_swim4.wav");

	PRECACHE_SOUND("player/pl_ladder1.wav");	// climb ladder rung
	PRECACHE_SOUND("player/pl_ladder2.wav");
	PRECACHE_SOUND("player/pl_ladder3.wav");
	PRECACHE_SOUND("player/pl_ladder4.wav");

	PRECACHE_SOUND("player/pl_wade1.wav");		// wade in water
	PRECACHE_SOUND("player/pl_wade2.wav");
	PRECACHE_SOUND("player/pl_wade3.wav");
	PRECACHE_SOUND("player/pl_wade4.wav");

	PRECACHE_SOUND("debris/wood1.wav");			// hit wood texture
	PRECACHE_SOUND("debris/wood2.wav");
	PRECACHE_SOUND("debris/wood3.wav");

	PRECACHE_SOUND("plats/train_use1.wav");		// use a train

	PRECACHE_SOUND("buttons/spark5.wav");		// hit computer texture
	PRECACHE_SOUND("buttons/spark6.wav");
	PRECACHE_SOUND("debris/glass1.wav");
	PRECACHE_SOUND("debris/glass2.wav");
	PRECACHE_SOUND("debris/glass3.wav");

	PRECACHE_SOUND( SOUND_FLASHLIGHT_ON );
	PRECACHE_SOUND( SOUND_FLASHLIGHT_OFF );

// player gib sounds
	PRECACHE_SOUND("common/bodysplat.wav");

// player pain sounds
   PRECACHE_SOUND("player/bhit_flesh-1.wav");
   PRECACHE_SOUND("player/bhit_flesh-2.wav");
   PRECACHE_SOUND("player/bhit_flesh-3.wav");
   PRECACHE_SOUND("player/bhit_head.wav");

   PRECACHE_SOUND("player/die1.wav");
   PRECACHE_SOUND("player/die2.wav");
   PRECACHE_SOUND("player/die3.wav");
   PRECACHE_SOUND("player/die4.wav");

   PRECACHE_SOUND("player/knife1.wav");
   PRECACHE_SOUND("player/knife2.wav");
   PRECACHE_SOUND("player/knife3.wav");

	PRECACHE_MODEL("models/player.mdl");

   PRECACHE_SOUND("voices/aight.wav");
   PRECACHE_SOUND("voices/fuckdat.wav");
   PRECACHE_SOUND("voices/checkdisout.wav");
   PRECACHE_SOUND("voices/follow.wav");
   PRECACHE_SOUND("voices/puto.wav");
   PRECACHE_SOUND("voices/wassup.wav");
   PRECACHE_SOUND("voices/nuttin.wav");
   PRECACHE_SOUND("voices/waza1.wav");
   PRECACHE_SOUND("voices/waza2.wav");
   PRECACHE_SOUND("voices/waza3.wav");
   PRECACHE_SOUND("voices/waza4.wav");

   PRECACHE_SOUND("server/whip.wav");
   PRECACHE_SOUND("max/bell.wav");
   PRECACHE_SOUND("max/doorbell.wav");
   PRECACHE_SOUND("ambience/winbuzz.wav");
   //PRECACHE_SOUND("ambience/barmusic.wav");
	// hud sounds

	//PRECACHE_SOUND("common/wpn_hudoff.wav");
	//PRECACHE_SOUND("common/wpn_hudon.wav");
	//PRECACHE_SOUND("common/wpn_moveselect.wav");
	//PRECACHE_SOUND("common/wpn_select.wav");
	//PRECACHE_SOUND("common/wpn_denyselect.wav");
}

/*
================
Sys_Error

Engine is going to shut down, allows setting a breakpoint in game .dll to catch that occasion
================
*/
void Sys_Error( const char *error_string )
{
   FILE *fp;
   fp = fopen("crashed.txt", "a");
   if (fp)
      {
      fprintf(fp, " CRASHED - %s\n", error_string);
      fclose(fp);
      }
   UTIL_LogPrintf((char *)error_string);
	// Default case, do nothing.  MOD AUTHORS:  Add code ( e.g., _asm { int 3 }; here to cause a breakpoint for debugging your game .dlls
}

/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if ( g_pGameRules ) // this function may be called before the world has spawned, and the game rules initialized
		return "Gangwars"; //g_pGameRules->GetGameDescription();
	else
		return "Half-Life";
}

/*
================
PlayerCustomization

A new player customization has been registered on the server
UNDONE:  This only sets the # of frames of the spray can logo
animation right now.
================
*/
void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
	{
		ALERT(at_console, "PlayerCustomization:  Couldn't get player!\n");
		return;
	}

	if (!pCust)
	{
		ALERT(at_console, "PlayerCustomization:  NULL customization!\n");
		return;
	}

	switch (pCust->resource.type)
	{
	case t_decal:
		pPlayer->SetCustomDecalFrames(pCust->nUserData2); // Second int is max # of frames.
		break;
	case t_sound:
	case t_skin:
	case t_model:
		// Ignore for now.
		break;
	default:
		ALERT(at_console, "PlayerCustomization:  Unknown customization type!\n");
		break;
	}
}

/*
================
SpectatorConnect

A spectator has joined the game
================
*/
void SpectatorConnect( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorConnect( );
}

/*
================
SpectatorConnect

A spectator has left the game
================
*/
void SpectatorDisconnect( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorDisconnect( );
}

/*
================
SpectatorConnect

A spectator has sent a usercmd
================
*/
void SpectatorThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorThink( );
}

////////////////////////////////////////////////////////
// PAS and PVS routines for client messaging
//

/*
================
SetupVisibility

A client can have a separate "view entity" indicating that his/her view should depend on the origin of that
view entity.  If that's the case, then pViewEntity will be non-NULL and will be used.  Otherwise, the current
entity's origin is used.  Either is offset by the view_ofs to get the eye position.

From the eye position, we set up the PAS and PVS to use for filtering network messages to the client.  At this point, we could
 override the actual PAS or PVS values, or use a different origin.

NOTE:  Do not cache the values of pas and pvs, as they depend on reusable memory in the engine, they are only good for this one frame
================
*/
void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
{
	Vector org;
	edict_t *pView = pClient;

	// Find the client's PVS
	if ( pViewEntity )
		pView = pViewEntity;

   // Tracking Spectators use the visibility of their target
	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );
   if (!pPlayer)
      {
      UTIL_LogPrintf("Crashed @ 003\n");
      return;
      }

	if ( (pPlayer->pev->iuser2 != 0) && (pPlayer->m_hObserverTarget != NULL) )
		pView = pPlayer->m_hObserverTarget->edict();

	org = pView->v.origin + pView->v.view_ofs;
	if ( pView->v.flags & FL_DUCKING )
		org = org + ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );

	*pvs = ENGINE_SET_PVS ( (float *)&org );
	*pas = ENGINE_SET_PAS ( (float *)&org );
}

#include "entity_state.h"

/*
AddToFullPack

Return 1 if the entity state has been filled in for the ent and the entity will be propagated to the client, 0 otherwise

state is the server maintained copy of the state info that is transmitted to the client
a MOD could alter values copied into state to send the "host" a different look for a particular entity update, etc.
e and ent are the entity that is being added to the update, if 1 is returned
host is the player's edict of the player whom we are sending the update to
player is 1 if the ent/e is a player and 0 otherwise
pSet is either the PAS or PVS that we previous set up.  We can use it to ask the engine to filter the entity against the PAS or PVS.
we could also use the pas/ pvs that we set in SetupVisibility, if we wanted to.  Caching the value is valid in that case, but still only for the current frame
*/
int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
{
	int					i;

	// don't send if flagged for NODRAW and it's not the host getting the message
	if ( ( ent->v.effects == EF_NODRAW ) &&
		 ( ent != host ) )
		return 0;

	// Ignore ents without valid / visible models
	if ( !ent->v.modelindex || !STRING( ent->v.model ) )
		return 0;

	// Don't send spectators to other players
	if ( ( ent->v.flags & FL_SPECTATOR ) && ( ent != host ) )
	{
		return 0;
	}

	// Ignore if not the host and not touching a PVS/PAS leaf
	// If pSet is NULL, then the test will always succeed and the entity will be added to the update
	if ( ent != host )
	{
		if ( !ENGINE_CHECK_VISIBILITY( (const struct edict_s *)ent, pSet ) )
		{
			return 0;
		}
	}


	// Don't send entity to local client if the client says it's predicting the entity itself.
	if ( ent->v.flags & FL_SKIPLOCALHOST )
	{
		if ( ( hostflags & 1 ) && ( ent->v.owner == host ) )
			return 0;
	}
	
	if ( host->v.groupinfo )
	{
		UTIL_SetGroupTrace( host->v.groupinfo, GROUP_OP_AND );

		// Should always be set, of course
		if ( ent->v.groupinfo )
		{
			if ( g_groupop == GROUP_OP_AND )
			{
				if ( !(ent->v.groupinfo & host->v.groupinfo ) )
					return 0;
			}
			else if ( g_groupop == GROUP_OP_NAND )
			{
				if ( ent->v.groupinfo & host->v.groupinfo )
					return 0;
			}
		}

		UTIL_UnsetGroupTrace();
	}

	memset( state, 0, sizeof( *state ) );

	// Assign index so we can track this entity from frame to frame and
	//  delta from it.
	state->number	  = e;
	state->entityType = ENTITY_NORMAL;
	
	// Flag custom entities.
	if ( ent->v.flags & FL_CUSTOMENTITY )
	{
		state->entityType = ENTITY_BEAM;
	}

	// 
	// Copy state data
	//

	// Round animtime to nearest millisecond
	state->animtime   = (int)(1000.0 * ent->v.animtime ) / 1000.0;

	memcpy( state->origin, ent->v.origin, 3 * sizeof( float ) );
	memcpy( state->angles, ent->v.angles, 3 * sizeof( float ) );
	memcpy( state->mins, ent->v.mins, 3 * sizeof( float ) );
	memcpy( state->maxs, ent->v.maxs, 3 * sizeof( float ) );

	memcpy( state->startpos, ent->v.startpos, 3 * sizeof( float ) );
	memcpy( state->endpos, ent->v.endpos, 3 * sizeof( float ) );

	state->impacttime = ent->v.impacttime;
	state->starttime = ent->v.starttime;

	state->modelindex = ent->v.modelindex;
		
	state->frame      = ent->v.frame;

	state->skin       = ent->v.skin;
	state->effects    = ent->v.effects;

	// This non-player entity is being moved by the game .dll and not the physics simulation system
	//  make sure that we interpolate it's position on the client if it moves
	if ( !player &&
		 ent->v.animtime &&
		 ent->v.velocity[ 0 ] == 0 && 
		 ent->v.velocity[ 1 ] == 0 && 
		 ent->v.velocity[ 2 ] == 0 )
	{
		state->eflags |= EFLAG_SLERP;
	}

	state->scale	  = ent->v.scale;
	state->solid	  = ent->v.solid;
	state->colormap   = ent->v.colormap;
	state->movetype   = ent->v.movetype;
	state->sequence   = ent->v.sequence;
	state->framerate  = ent->v.framerate;
	state->body       = ent->v.body;

	for (i = 0; i < 4; i++)
	{
		state->controller[i] = ent->v.controller[i];
	}

	for (i = 0; i < 2; i++)
	{
		state->blending[i]   = ent->v.blending[i];
	}

	state->rendermode    = ent->v.rendermode;
	state->renderamt     = ent->v.renderamt; 
	state->renderfx      = ent->v.renderfx;
	state->rendercolor.r = ent->v.rendercolor[0];
	state->rendercolor.g = ent->v.rendercolor[1];
	state->rendercolor.b = ent->v.rendercolor[2];

	state->aiment = 0;
	if ( ent->v.aiment )
	{
		state->aiment = ENTINDEX( ent->v.aiment );
	}

	state->owner = 0;
	if ( ent->v.owner )
	{
		int owner = ENTINDEX( ent->v.owner );
		
		// Only care if owned by a player
		if ( owner >= 1 && owner <= gpGlobals->maxClients )
		{
			state->owner = owner;	
		}
	}

	// HACK:  Somewhat...
	// Class is overridden for non-players to signify a breakable glass object ( sort of a class? )
	if ( !player )
	{
		state->playerclass  = ent->v.playerclass;
	}

	// Special stuff for players only
	if ( player )
	{
		memcpy( state->basevelocity, ent->v.basevelocity, 3 * sizeof( float ) );

		state->weaponmodel  = MODEL_INDEX( STRING( ent->v.weaponmodel ) );
		state->gaitsequence = ent->v.gaitsequence;
		state->spectator = ent->v.flags & FL_SPECTATOR;
		state->friction     = ent->v.friction;

		state->gravity      = ent->v.gravity;
//		state->team			= ent->v.team;
//		state->playerclass  = ent->v.playerclass;
		state->usehull      = ( ent->v.flags & FL_DUCKING ) ? 1 : 0;
		state->health		= ent->v.health;
	}

	return 1;
}

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT	16 
/*28*/

/*
===================
CreateBaseline

Creates baselines used for network encoding, especially for player data since players are not spawned until connect time.
===================
*/
void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
{
	baseline->origin		= entity->v.origin;
	baseline->angles		= entity->v.angles;
	baseline->frame			= entity->v.frame;
	baseline->skin			= (short)entity->v.skin;

	// render information
	baseline->rendermode	= (byte)entity->v.rendermode;
	baseline->renderamt		= (byte)entity->v.renderamt;
	baseline->rendercolor.r	= (byte)entity->v.rendercolor[0];
	baseline->rendercolor.g	= (byte)entity->v.rendercolor[1];
	baseline->rendercolor.b	= (byte)entity->v.rendercolor[2];
	baseline->renderfx		= (byte)entity->v.renderfx;

	if ( player )
	{
		baseline->mins			= player_mins;
		baseline->maxs			= player_maxs;

		baseline->colormap		= eindex;
		baseline->modelindex	= playermodelindex;
		baseline->friction		= 1.0;
		baseline->movetype		= MOVETYPE_WALK;

		baseline->scale			= entity->v.scale;
		baseline->solid			= SOLID_SLIDEBOX;
		baseline->framerate		= 1.0;
		baseline->gravity		= 1.0;

	}
	else
	{
		baseline->mins			= entity->v.mins;
		baseline->maxs			= entity->v.maxs;

		baseline->colormap		= 0;
		baseline->modelindex	= entity->v.modelindex;//SV_ModelIndex(pr_strings + entity->v.model);
		baseline->movetype		= entity->v.movetype;

		baseline->scale			= entity->v.scale;
		baseline->solid			= entity->v.solid;
		baseline->framerate		= entity->v.framerate;
		baseline->gravity		= entity->v.gravity;
	}
}

typedef struct
{
	char name[32];
	int	 field;
} entity_field_alias_t;

#define FIELD_ORIGIN0			0
#define FIELD_ORIGIN1			1
#define FIELD_ORIGIN2			2
#define FIELD_ANGLES0			3
#define FIELD_ANGLES1			4
#define FIELD_ANGLES2			5

static entity_field_alias_t entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
};

void Entity_FieldInit( struct delta_s *pFields )
{
	entity_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN0 ].name );
	entity_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN1 ].name );
	entity_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN2 ].name );
	entity_field_alias[ FIELD_ANGLES0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES0 ].name );
	entity_field_alias[ FIELD_ANGLES1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES1 ].name );
	entity_field_alias[ FIELD_ANGLES2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES2 ].name );
}

/*
==================
Entity_Encode

Callback for sending entity_state_t info over network. 
FIXME:  Move to script
==================
*/
void Entity_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;

	int localplayer = 0;
	static int initialized = 0;

	if ( !initialized )
	{
		Entity_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->impacttime != 0 ) && ( t->starttime != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );

		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) &&
		 ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

static entity_field_alias_t player_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
};

void Player_FieldInit( struct delta_s *pFields )
{
	player_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN0 ].name );
	player_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN1 ].name );
	player_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN2 ].name );
}

/*
==================
Player_Encode

Callback for sending entity_state_t for players info over network. 
==================
*/
void Player_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int localplayer = 0;
	static int initialized = 0;

	if ( !initialized )
	{
		Player_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) &&
		 ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

#define CUSTOMFIELD_ORIGIN0			0
#define CUSTOMFIELD_ORIGIN1			1
#define CUSTOMFIELD_ORIGIN2			2
#define CUSTOMFIELD_ANGLES0			3
#define CUSTOMFIELD_ANGLES1			4
#define CUSTOMFIELD_ANGLES2			5
#define CUSTOMFIELD_SKIN			6
#define CUSTOMFIELD_SEQUENCE		7
#define CUSTOMFIELD_ANIMTIME		8

entity_field_alias_t custom_entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
	{ "skin",				0 },
	{ "sequence",			0 },
	{ "animtime",			0 },
};

void Custom_Entity_FieldInit( struct delta_s *pFields )
{
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].name );
}

/*
==================
Custom_Encode

Callback for sending entity_state_t info ( for custom entities ) over network. 
FIXME:  Move to script
==================
*/
void Custom_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int beamType;
	static int initialized = 0;

	if ( !initialized )
	{
		Custom_Entity_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	beamType = t->rendermode & 0x0f;
		
	if ( beamType != BEAM_POINTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field );
	}

	if ( beamType != BEAM_POINTS )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field );
	}

	if ( beamType != BEAM_ENTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field );
	}

	// animtime is compared by rounding first
	// see if we really shouldn't actually send it
	if ( (int)f->animtime == (int)t->animtime )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field );
	}
}

/*
=================
RegisterEncoders

Allows game .dll to override network encoding of certain types of entities and tweak values, etc.
=================
*/
void RegisterEncoders( void )
{
	DELTA_ADDENCODER( "Entity_Encode", Entity_Encode );
	DELTA_ADDENCODER( "Custom_Encode", Custom_Encode );
	DELTA_ADDENCODER( "Player_Encode", Player_Encode );
}

int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
{
#if defined( CLIENT_WEAPONS )
	int i;
	weapon_data_t *item;
	entvars_t *pev = &player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );
	CBasePlayerWeapon *gun;
	
	ItemInfo II;

	memset( info, 0, 32 * sizeof( weapon_data_t ) );

	if ( !pl )
		return 1;

	// go through all of the weapons and make a list of the ones to pack
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( pl->m_rgpPlayerItems[ i ] )
		{
			// there's a weapon here. Should I pack it?
			CBasePlayerItem *pPlayerItem = pl->m_rgpPlayerItems[ i ];

			while ( pPlayerItem )
			{
				gun = (CBasePlayerWeapon *)pPlayerItem->GetWeaponPtr();
				if ( gun && gun->UseDecrement() )
				{
					// Get The ID.
					memset( &II, 0, sizeof( II ) );
					gun->GetItemInfo( &II );

					if ( II.iId >= 0 && II.iId < 32 )
					{
						item = &info[ II.iId ];
						
						item->m_iId						= II.iId;
						item->m_iClip					= gun->m_iClip;

						item->m_flTimeWeaponIdle		= max( gun->m_flTimeWeaponIdle, -0.001 );
						item->m_flNextPrimaryAttack		= max( gun->m_flNextPrimaryAttack, -0.001 );
						item->m_flNextSecondaryAttack	= max( gun->m_flNextSecondaryAttack, -0.001 );
						item->m_fInReload				= gun->m_fInReload;
					}
				}
				pPlayerItem = pPlayerItem->m_pNext;
			}
		}
	}
#else
	memset( info, 0, 32 * sizeof( weapon_data_t ) );
#endif
	return 1;
}

/*
=================
UpdateClientData

Data sent to current client only
engine sets cd to 0 before calling.
=================
*/
void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
{
	cd->flags			= ent->v.flags;
	cd->health			= ent->v.health;

	cd->viewmodel		= MODEL_INDEX( STRING( ent->v.viewmodel ) );
	cd->waterlevel		= ent->v.waterlevel;
	cd->watertype		= ent->v.watertype;
	cd->weapons			= ent->v.weapons;

	// Vectors
	cd->origin			= ent->v.origin;
	cd->velocity		= ent->v.velocity;
	cd->view_ofs		= ent->v.view_ofs;
	cd->punchangle		= ent->v.punchangle;

	cd->bInDuck			= ent->v.bInDuck;
	cd->flTimeStepSound = ent->v.flTimeStepSound;
	cd->flDuckTime		= ent->v.flDuckTime;
	cd->flSwimTime		= ent->v.flSwimTime;
	cd->waterjumptime	= ent->v.teleport_time;

	strcpy( cd->physinfo, ENGINE_GETPHYSINFO( ent ) );

	cd->maxspeed		= ent->v.maxspeed;
	cd->fov				= ent->v.fov;
	cd->weaponanim		= ent->v.weaponanim;

	cd->pushmsec		= ent->v.pushmsec;

   cd->iuser1        = ent->v.iuser1;
   cd->iuser2        = ent->v.iuser2;

#if defined( CLIENT_WEAPONS )
	if ( sendweapons )
	{
		entvars_t *pev = (entvars_t *)&ent->v;
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

		if ( pl )
		{
			cd->m_flNextAttack	= pl->m_flNextAttack;

			if ( pl->m_pActiveItem )
			{
				CBasePlayerWeapon *gun;
				gun = (CBasePlayerWeapon *)pl->m_pActiveItem->GetWeaponPtr();
				if ( gun && gun->UseDecrement() )
				{
					ItemInfo II;
					memset( &II, 0, sizeof( II ) );
					gun->GetItemInfo( &II );

					cd->m_iId = II.iId;
				}
			}
		}
	}
#endif
}

/*
=================
CmdStart

We're about to run this usercmd for the specified player.  We can set up groupinfo and masking here, etc.
This is the time to examine the usercmd for anything extra.  This call happens even if think does not.
=================
*/
void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
{
	entvars_t *pev = (entvars_t *)&player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

	if( !pl )
		return;

	if ( pl->pev->groupinfo != 0 )
	{
		UTIL_SetGroupTrace( pl->pev->groupinfo, GROUP_OP_AND );
	}

	pl->random_seed = random_seed;
}

/*
=================
CmdEnd

Each cmdstart is exactly matched with a cmd end, clean up any group trace flags, etc. here
=================
*/
void CmdEnd ( const edict_t *player )
{
	entvars_t *pev = (entvars_t *)&player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

	if( !pl )
		return;
	if ( pl->pev->groupinfo != 0 )
	{
		UTIL_UnsetGroupTrace();
	}
}

/*
================================
ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

/*
================================
GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:				// Normal player
		mins = VEC_HULL_MIN;
		maxs = VEC_HULL_MAX;
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = VEC_DUCK_HULL_MIN;
		maxs = VEC_DUCK_HULL_MAX;
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = Vector( 0, 0, 0 );
		maxs = Vector( 0, 0, 0 );
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
CreateInstancedBaselines

Create pseudo-baselines for items that aren't placed in the map at spawn time, but which are likely
to be created during play ( e.g., grenades, ammo packs, projectiles, corpses, etc. )
================================
*/
void CreateInstancedBaselines ( void )
{
	int iret = 0;
	entity_state_t state;

	memset( &state, 0, sizeof( state ) );

	// Create any additional baselines here for things like grendates, etc.
	// iret = ENGINE_INSTANCE_BASELINE( pc->pev->classname, &state );

	// Destroy objects.
	//UTIL_Remove( pc );
}

/*
================================
InconsistentFile

One of the ENGINE_FORCE_UNMODIFIED files failed the consistency check for the specified player
 Return 0 to allow the client to continue, 1 to force immediate disconnection ( with an optional disconnect message of up to 256 characters )
================================
*/
int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
{
	// Server doesn't care?
	if ( CVAR_GET_FLOAT( "mp_consistency" ) != 1 )
		return 0;

	// Default behavior is to kick the player
	sprintf( disconnect_message, "Server is enforcing file consistency for %s\n", filename );

	// Kick now with specified disconnect message.
	return 1;
}

/*
================================
AllowLagCompensation

 The game .dll should return 1 if lag compensation should be allowed ( could also just set
  the sv_unlag cvar.
 Most games right now should return 0, until client-side weapon prediction code is written
  and tested for them ( note you can predict weapons, but not do lag compensation, too, 
  if you want.
================================
*/
int AllowLagCompensation( void )
{
	return 1;
}

